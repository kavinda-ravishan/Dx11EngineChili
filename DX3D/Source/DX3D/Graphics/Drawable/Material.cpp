#include <DX3D/Graphics/Drawable/Constants.hpp>
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
			auto pvs = BVertexShader::Resolve(desc, graphics, shader_code + "VS.cso");
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(BPixelShader::Resolve(desc, graphics, shader_code + "PS.cso"));
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
	}
}

dx3d_dvtx::VertexBuffer dx3d::Material::ExtractVertices(const aiMesh & mesh) const noexcept {
	return { _vtx_layout,mesh };
}

std::vector<dx3d::Technique> dx3d::Material::GetTechniques() const noexcept {
	return _techniques;
}
