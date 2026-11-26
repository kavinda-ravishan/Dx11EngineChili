#include <assimp/scene.h>
#include <DX3D/Graphics/Drawable/Constants.hpp>
#include <DX3D/Graphics/RenderJobs/Technique.hpp>
#include <DX3D/Graphics/RenderJobs/Step.hpp>
#include <DX3D/Graphics/Drawable/Material.hpp>

dx3d::Material::Material(const BindableDesc& desc, Graphics& graphics, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG)
	: _model_path(path.string())
{
	const auto root_path = path.parent_path().string() + "\\";
	{
		aiString temp_name;
		material.Get(AI_MATKEY_NAME, temp_name);
		_name = temp_name.C_Str();
	}

	// phong technique
	{
		Technique phong{ "Phong" };
		Step step(0);
		std::string shader_code{ "Phong" };
		aiString tex_file_name;

		// common (pre)
		_vtx_layout.Append(dx3d_dvtx::VertexLayout::Position3D);
		_vtx_layout.Append(dx3d_dvtx::VertexLayout::Normal);
		dx3d_dcb::RawLayout psc_layout;
		bool has_texture{ false };
		bool has_gloss_alpha{ false };

		// diffuse
		{
			bool has_alpha{ false };
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &tex_file_name) == aiReturn_SUCCESS)
			{
				has_texture = true;
				shader_code += "Dif";
				_vtx_layout.Append(dx3d_dvtx::VertexLayout::Texture2D);
				auto tex = BTexture::Resolve(desc, graphics, root_path + tex_file_name.C_Str());
				if (tex->HasAlpha())
				{
					has_alpha = true;
					shader_code += "Msk";
				}
				step.AddBindable(std::move(tex));
			}
			else
			{
				psc_layout.Add<dx3d_dcb::Float3>(PSCBV_material_color);
			}
			step.AddBindable(BRasterizer::Resolve(desc, graphics, has_alpha));
		}
		// specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &tex_file_name) == aiReturn_SUCCESS)
			{
				has_texture = true;
				shader_code += "Spc";
				_vtx_layout.Append(dx3d_dvtx::VertexLayout::Texture2D);
				auto tex = BTexture::Resolve(desc, graphics, root_path + tex_file_name.C_Str(), 1);
				has_gloss_alpha = tex->HasAlpha();
				step.AddBindable(std::move(tex));
				psc_layout.Add<dx3d_dcb::Bool>(PSCBV_use_gloss_alpha);
				psc_layout.Add<dx3d_dcb::Bool>(PSCBV_use_specular_map);
			}
			psc_layout.Add<dx3d_dcb::Float3>(PSCBV_specular_color);
			psc_layout.Add<dx3d_dcb::Float>(PSCBV_specular_weight);
			psc_layout.Add<dx3d_dcb::Float>(PSCBV_specular_gloss);
		}
		// normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &tex_file_name) == aiReturn_SUCCESS)
			{
				has_texture = true;
				shader_code += "Nrm";
				_vtx_layout.Append(dx3d_dvtx::VertexLayout::Texture2D);
				_vtx_layout.Append(dx3d_dvtx::VertexLayout::Tangent);
				_vtx_layout.Append(dx3d_dvtx::VertexLayout::Bitangent);
				step.AddBindable(BTexture::Resolve(desc, graphics, root_path + tex_file_name.C_Str(), 2));
				psc_layout.Add<dx3d_dcb::Bool>(PSCBV_use_normal_map);
				psc_layout.Add<dx3d_dcb::Float>(PSCBV_normal_map_weight);
			}
		}
		// common (post)
		{
			step.AddBindable(std::make_shared<BTransformCbuf>(desc, graphics, 0u));
			step.AddBindable(BBlender::Resolve(desc, graphics, false));
			auto pvs = BVertexShader::Resolve(desc, graphics, shader_code + "_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(BPixelShader::Resolve(desc, graphics, shader_code + "_PS.cso"));
			step.AddBindable(BInputLayout::Resolve(desc, graphics, _vtx_layout, pvsbc));
			if (has_texture)
			{
				step.AddBindable(BSampler::Resolve(desc, graphics));
			}
			// PS material params (cbuf)
			dx3d_dcb::Buffer buf{ std::move(psc_layout) };
			if (auto r = buf[PSCBV_material_color]; r.Exists())
			{
				aiColor3D color = { 0.45f,0.45f,0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf[PSCBV_use_gloss_alpha].SetIfExists(has_gloss_alpha);
			buf[PSCBV_use_specular_map].SetIfExists(true);
			if (auto r = buf[PSCBV_specular_color]; r.Exists())
			{
				aiColor3D color = { 0.18f,0.18f,0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf[PSCBV_specular_weight].SetIfExists(1.0f);
			if (auto r = buf[PSCBV_specular_gloss]; r.Exists())
			{
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf[PSCBV_use_normal_map].SetIfExists(true);
			buf[PSCBV_normal_map_weight].SetIfExists(1.0f);
			step.AddBindable(std::make_unique<BCachingPixelConstantBufferEX>(desc, graphics, std::move(buf), 1u));
		}
		phong.AddStep(std::move(step));
		_techniques.push_back(std::move(phong));
	}
	// outline technique
	{
		Technique outline("Outline", false);
		{
			Step mask(1);

			auto pvs = BVertexShader::Resolve(desc, graphics, "Solid_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable(std::move(pvs));

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable(BInputLayout::Resolve(desc, graphics, _vtx_layout, pvsbc));

			mask.AddBindable(std::make_shared<BTransformCbuf>(desc, graphics));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(mask));
		}
		{
			Step draw(2);

			// these can be pass-constant (tricky due to layout issues)
			auto pvs = BVertexShader::Resolve(desc, graphics, "Solid_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable(std::move(pvs));

			// this can be pass-constant
			draw.AddBindable(BPixelShader::Resolve(desc, graphics, "Solid_PS.cso"));

			dx3d_dcb::RawLayout lay;
			lay.Add<dx3d_dcb::Float3>(PSCBV_material_color);
			auto buf = dx3d_dcb::Buffer(std::move(lay));
			buf[PSCBV_material_color] = DirectX::XMFLOAT3{ 1.0f,0.4f,0.4f };
			draw.AddBindable(std::make_shared<BCachingPixelConstantBufferEX>(desc, graphics, buf, 1u));

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(BInputLayout::Resolve(desc, graphics, _vtx_layout, pvsbc));

			// quick and dirty... nicer solution maybe takes a lamba... we'll see :)
			class TransformCbufScaling : public BTransformCbuf
			{
			public:
				TransformCbufScaling(const BindableDesc& desc, Graphics& graphics, float scale = 1.04)
					:
					BTransformCbuf(desc, graphics),
					_buf(MakeLayout())
				{
					_buf["scale"] = scale;
				}
				void Accept(TechniqueProbe& probe) override
				{
					probe.VisitBuffer(_buf);
				}
				void Bind(Graphics& gfx) noexcept override
				{
					const float scale = _buf["scale"];
					const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
					auto xf = GetTransforms(gfx);
					xf.model_view = xf.model_view * scaleMatrix;
					xf.model_view_proj = xf.model_view_proj * scaleMatrix;
					UpdateBindImpl(gfx, xf);
				}
				std::unique_ptr<CloningBindable> Clone() const noexcept override
				{
					return std::make_unique<TransformCbufScaling>(*this);
				}
			private:
				static dx3d_dcb::RawLayout MakeLayout()
				{
					dx3d_dcb::RawLayout layout;
					layout.Add<dx3d_dcb::Float>("scale");
					return layout;
				}
			private:
				dx3d_dcb::Buffer _buf;
			};
			draw.AddBindable(std::make_shared<TransformCbufScaling>(desc, graphics));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(draw));
		}
		_techniques.push_back(std::move(outline));
	}
}

dx3d_dvtx::VertexBuffer dx3d::Material::ExtractVertices(const aiMesh & mesh) const noexcept {
	return { _vtx_layout,mesh };
}

std::vector<unsigned short> dx3d::Material::ExtractIndices(const aiMesh& mesh) const noexcept {
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}

std::shared_ptr<dx3d::BVertexBuffer> dx3d::Material::MakeVertexBindable(const BindableDesc& desc, Graphics& graphics, const aiMesh& mesh, const float scale) const noexcept(!IS_DEBUG) {
	auto vtc = ExtractVertices(mesh);

	if (scale != 1.0f) {
		for (auto i = 0u; i < vtc.Size(); i++) {
			DirectX::XMFLOAT3& pos = vtc[i].Attr<dx3d_dvtx::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}

	return BVertexBuffer::Resolve(desc, graphics, MakeMeshTag(mesh), std::move(vtc));
}

std::shared_ptr<dx3d::BIndexBuffer> dx3d::Material::MakeIndexBindable(const BindableDesc& desc, Graphics& graphics, const aiMesh& mesh) const noexcept(!IS_DEBUG) {
	return BIndexBuffer::Resolve(desc, graphics, MakeMeshTag(mesh), ExtractIndices(mesh));
}

std::vector<dx3d::Technique> dx3d::Material::GetTechniques() const noexcept {
	return _techniques;
}

std::string dx3d::Material::MakeMeshTag(const aiMesh& mesh) const noexcept {
	return _model_path + "%" + mesh.mName.C_Str();
}
