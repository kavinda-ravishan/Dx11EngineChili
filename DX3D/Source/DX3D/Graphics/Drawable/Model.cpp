#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DX3D/Graphics/Drawable/ModelWindow.hpp>
#include <DX3D/Graphics/Drawable/Mesh.hpp>
#include <DX3D/Graphics/Drawable/Node.hpp>
#include <DX3D/Graphics/Drawable/Model.hpp>

// Model
dx3d::Model::Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const float scale)
	: Base(desc), _window(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto scene = imp.ReadFile(file_path.string().c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (scene == nullptr) {
		DX3DLogThrowError(("Failed to load the model " + file_path.string()).c_str());
	}

	for (size_t i = 0; i < scene->mNumMeshes; i++) {
		_mesh_ptrs.push_back(ParseMesh(desc, graphics, file_path, *scene->mMeshes[i], scene->mMaterials, scale));
	}

	int next_id{ 0 };
	_root = ParseNode(next_id, *scene->mRootNode);
}

dx3d::Model::~Model() noexcept {}

void dx3d::Model::Submit(FrameCommander& frame) const {
	// I'm still not happy about updating parameters (i.e. mutating a bindable GPU state
	// which is part of a mesh which is part of a node which is part of the model that is
	// const in this call) Can probably do this elsewhere
	_window->ApplyParameters();
	_root->Submit(frame, DirectX::XMMatrixIdentity());
}

void dx3d::Model::ShowWindow(Graphics& graphics, const char* window_name) noexcept {
	_window->Show(graphics, window_name, *_root);
}

void dx3d::Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept {
	_root->SetAppliedTransform(tf);
}

std::unique_ptr<dx3d::Mesh> dx3d::Model::ParseMesh(
	const BaseDesc& desc,
	Graphics& graphics,
	const std::filesystem::path& file_path,
	const aiMesh& ai_mesh,
	const aiMaterial* const* ai_materials_ptr,
	const float scale)
{
	//const std::filesystem::path resource_path{ file_path.parent_path() };

	//std::vector<std::shared_ptr<Bindable>> bindable_ptrs;

	//bool has_specular_map = false;
	//bool has_alpha_gloss = false;
	//bool has_alpha_diffuse = false;
	//bool has_normal_map = false;
	//bool has_diffuse_map = false;
	//
	//float shininess = 2.0f;
	//DirectX::XMFLOAT4 specular_color{ 0.18f, 0.18f, 0.18f, 1.0f };
	//DirectX::XMFLOAT4 diffuse_color{ 0.45f, 0.45f, 0.85f, 1.0f };
	//
	//if (ai_mesh.mMaterialIndex >= 0)
	//{
	//	auto& material = *ai_materials_ptr[ai_mesh.mMaterialIndex];

	//	aiString tex_file_name;

	//	if (material.GetTexture(aiTextureType_DIFFUSE, 0, &tex_file_name) == aiReturn_SUCCESS)
	//	{
	//		auto tex = BTexture::Resolve(BindableDesc({ desc.logger }), graphics, (resource_path / tex_file_name.C_Str()).string());
	//		has_alpha_diffuse = tex->HasAlpha();
	//		bindable_ptrs.push_back(std::move(tex));
	//		has_diffuse_map = true;
	//	}
	//	else
	//	{
	//		material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuse_color));
	//	}

	//	if (material.GetTexture(aiTextureType_SPECULAR, 0, &tex_file_name) == aiReturn_SUCCESS)
	//	{
	//		auto tex = BTexture::Resolve(BindableDesc({ desc.logger }), graphics, (resource_path / tex_file_name.C_Str()).string(), 1u);
	//		has_alpha_gloss = tex->HasAlpha();
	//		bindable_ptrs.push_back(std::move(tex));
	//		has_specular_map = true;
	//	}
	//	else
	//	{
	//		material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specular_color));
	//	}
	//	
	//	if(!has_alpha_gloss)
	//	{
	//		material.Get(AI_MATKEY_SHININESS, shininess);
	//	}

	//	if (material.GetTexture(aiTextureType_NORMALS, 0, &tex_file_name) == aiReturn_SUCCESS)
	//	{
	//		auto tex = BTexture::Resolve(BindableDesc({ desc.logger }), graphics, (resource_path / tex_file_name.C_Str()).string(), 2u);
	//		has_alpha_gloss = tex->HasAlpha();
	//		bindable_ptrs.push_back(std::move(tex));
	//		has_normal_map = true;
	//	}

	//	if (has_diffuse_map || has_specular_map || has_normal_map)
	//	{
	//		bindable_ptrs.push_back(BSampler::Resolve(BindableDesc({ desc.logger }), graphics));
	//	}
	//}

	//const auto mesh_tag = file_path.string() + "%" + ai_mesh.mName.C_Str();

	//if (has_diffuse_map && has_normal_map && has_specular_map)
	//{
	//	dx3d_dvtx::VertexBuffer vbuf(std::move(
	//		dx3d_dvtx::VertexLayout{}
	//		.Append(dx3d_dvtx::VertexLayout::Position3D)
	//		.Append(dx3d_dvtx::VertexLayout::Normal)
	//		.Append(dx3d_dvtx::VertexLayout::Tangent)
	//		.Append(dx3d_dvtx::VertexLayout::Bitangent)
	//		.Append(dx3d_dvtx::VertexLayout::Texture2D)
	//	));

	//	for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
	//	{
	//		vbuf.EmplaceBack(
	//			DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mTangents[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mBitangents[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT2*>(&ai_mesh.mTextureCoords[0][i])
	//		);
	//	}

	//	std::vector<unsigned short> indices;
	//	indices.reserve(ai_mesh.mNumFaces * 3);
	//	for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
	//	{
	//		const auto& face = ai_mesh.mFaces[i];
	//		assert(face.mNumIndices == 3);
	//		indices.push_back(face.mIndices[0]);
	//		indices.push_back(face.mIndices[1]);
	//		indices.push_back(face.mIndices[2]);
	//	}

	//	bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

	//	bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

	//	auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVSNormalMap.cso");
	//	auto pvsbc = pvs->GetBytecode();
	//	bindable_ptrs.push_back(std::move(pvs));

	//	bindable_ptrs.push_back(BPixelShader::Resolve(
	//		BindableDesc({ desc.logger }), 
	//		graphics, 
	//		has_alpha_diffuse ? "PhongPSSpecNormalMask.cso" : "PhongPSSpecNormalMap.cso"
	//	));

	//	bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

	//	dx3d_dcb::RawLayout lay;
	//	lay.Add<dx3d_dcb::Bool>(PSCBV_normal_map_enabled);
	//	lay.Add<dx3d_dcb::Bool>(PSCBV_specular_map_enabled);
	//	lay.Add<dx3d_dcb::Bool>(PSCBV_has_gloss_map);
	//	lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);
	//	lay.Add<dx3d_dcb::Float3>(PSCBV_specular_color);
	//	lay.Add<dx3d_dcb::Float>(PSCBV_specular_map_weight);

	//	auto ps_cbuf = dx3d_dcb::Buffer(std::move(lay));
	//	ps_cbuf[PSCBV_normal_map_enabled] = true;
	//	ps_cbuf[PSCBV_specular_map_enabled] = true;
	//	ps_cbuf[PSCBV_has_gloss_map] = has_alpha_gloss;
	//	ps_cbuf[PSCBV_specular_power] = shininess;
	//	ps_cbuf[PSCBV_specular_color] = DirectX::XMFLOAT3{ 0.75f,0.75f,0.75f };
	//	ps_cbuf[PSCBV_specular_map_weight] = 0.671f;

	//	bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	//}
	//else if (has_diffuse_map && has_normal_map)
	//{
	//	dx3d_dvtx::VertexBuffer vbuf(std::move(
	//		dx3d_dvtx::VertexLayout{}
	//		.Append(dx3d_dvtx::VertexLayout::Position3D)
	//		.Append(dx3d_dvtx::VertexLayout::Normal)
	//		.Append(dx3d_dvtx::VertexLayout::Tangent)
	//		.Append(dx3d_dvtx::VertexLayout::Bitangent)
	//		.Append(dx3d_dvtx::VertexLayout::Texture2D)
	//	));

	//	for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
	//	{
	//		vbuf.EmplaceBack(
	//			DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mTangents[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mBitangents[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT2*>(&ai_mesh.mTextureCoords[0][i])
	//		);
	//	}

	//	std::vector<unsigned short> indices;
	//	indices.reserve(ai_mesh.mNumFaces * 3);
	//	for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
	//	{
	//		const auto& face = ai_mesh.mFaces[i];
	//		assert(face.mNumIndices == 3);
	//		indices.push_back(face.mIndices[0]);
	//		indices.push_back(face.mIndices[1]);
	//		indices.push_back(face.mIndices[2]);
	//	}

	//	bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

	//	bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

	//	auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVSNormalMap.cso");
	//	auto pvsbc = pvs->GetBytecode();
	//	bindable_ptrs.push_back(std::move(pvs));

	//	bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPSNormalMap.cso"));

	//	bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

	//	dx3d_dcb::RawLayout layout;
	//	layout.Add<dx3d_dcb::Float>(PSCBV_specular_intensity);
	//	layout.Add<dx3d_dcb::Float>(PSCBV_specular_power);
	//	layout.Add<dx3d_dcb::Bool>(PSCBV_normal_map_enabled);

	//	auto ps_cbuf = dx3d_dcb::Buffer(std::move(layout));
	//	ps_cbuf[PSCBV_specular_intensity] = (specular_color.x + specular_color.y + specular_color.z) / 3.0f;
	//	ps_cbuf[PSCBV_specular_power] = shininess;
	//	ps_cbuf[PSCBV_normal_map_enabled] = true;

	//	bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	//}
	//else if (has_diffuse_map && !has_normal_map && has_specular_map)
	//{
	//	dx3d_dvtx::VertexBuffer vbuf(std::move(
	//		dx3d_dvtx::VertexLayout{}
	//		.Append(dx3d_dvtx::VertexLayout::Position3D)
	//		.Append(dx3d_dvtx::VertexLayout::Normal)
	//		.Append(dx3d_dvtx::VertexLayout::Texture2D)
	//	));

	//	for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
	//	{
	//		vbuf.EmplaceBack(
	//			DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT2*>(&ai_mesh.mTextureCoords[0][i])
	//		);
	//	}

	//	std::vector<unsigned short> indices;
	//	indices.reserve(ai_mesh.mNumFaces * 3);
	//	for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
	//	{
	//		const auto& face = ai_mesh.mFaces[i];
	//		assert(face.mNumIndices == 3);
	//		indices.push_back(face.mIndices[0]);
	//		indices.push_back(face.mIndices[1]);
	//		indices.push_back(face.mIndices[2]);
	//	}

	//	bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

	//	bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

	//	auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVS.cso");
	//	auto pvsbc = pvs->GetBytecode();
	//	bindable_ptrs.push_back(std::move(pvs));

	//	bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPSSpec.cso"));

	//	bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

	//	dx3d_dcb::RawLayout lay;
	//	lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);
	//	lay.Add<dx3d_dcb::Bool>(PSCBV_has_gloss_map);
	//	lay.Add<dx3d_dcb::Float>(PSCBV_specular_map_weight);

	//	auto ps_cbuf = dx3d_dcb::Buffer(std::move(lay));
	//	ps_cbuf[PSCBV_specular_power] = shininess;
	//	ps_cbuf[PSCBV_has_gloss_map] = has_alpha_gloss;
	//	ps_cbuf[PSCBV_specular_map_weight] = 1.0f;

	//	bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	//}
	//else if (has_diffuse_map)
	//{
	//	dx3d_dvtx::VertexBuffer vbuf(std::move(
	//		dx3d_dvtx::VertexLayout{}
	//		.Append(dx3d_dvtx::VertexLayout::Position3D)
	//		.Append(dx3d_dvtx::VertexLayout::Normal)
	//		.Append(dx3d_dvtx::VertexLayout::Texture2D)
	//	));

	//	for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
	//	{
	//		vbuf.EmplaceBack(
	//			DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i]),
	//			*reinterpret_cast<DirectX::XMFLOAT2*>(&ai_mesh.mTextureCoords[0][i])
	//		);
	//	}

	//	std::vector<unsigned short> indices;
	//	indices.reserve(ai_mesh.mNumFaces * 3);
	//	for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
	//	{
	//		const auto& face = ai_mesh.mFaces[i];
	//		assert(face.mNumIndices == 3);
	//		indices.push_back(face.mIndices[0]);
	//		indices.push_back(face.mIndices[1]);
	//		indices.push_back(face.mIndices[2]);
	//	}

	//	bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

	//	bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

	//	auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVS.cso");
	//	auto pvsbc = pvs->GetBytecode();
	//	bindable_ptrs.push_back(std::move(pvs));

	//	bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPS.cso"));

	//	bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

	//	dx3d_dcb::RawLayout lay;
	//	lay.Add<dx3d_dcb::Float>(PSCBV_specular_intensity);
	//	lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);

	//	auto ps_cbuf = dx3d_dcb::Buffer(std::move(lay));
	//	ps_cbuf[PSCBV_specular_intensity] = (specular_color.x + specular_color.y + specular_color.z) / 3.0f;
	//	ps_cbuf[PSCBV_specular_power] = shininess;

	//	bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	//}
	//else if (!has_diffuse_map && !has_normal_map && !has_specular_map)
	//{
	//	dx3d_dvtx::VertexBuffer vbuf(std::move(
	//		dx3d_dvtx::VertexLayout{}
	//		.Append(dx3d_dvtx::VertexLayout::Position3D)
	//		.Append(dx3d_dvtx::VertexLayout::Normal)
	//	));

	//	for (unsigned int i = 0; i < ai_mesh.mNumVertices; i++)
	//	{
	//		vbuf.EmplaceBack(
	//			DirectX::XMFLOAT3(ai_mesh.mVertices[i].x * scale, ai_mesh.mVertices[i].y * scale, ai_mesh.mVertices[i].z * scale),
	//			*reinterpret_cast<DirectX::XMFLOAT3*>(&ai_mesh.mNormals[i])
	//		);
	//	}

	//	std::vector<unsigned short> indices;
	//	indices.reserve(ai_mesh.mNumFaces * 3);
	//	for (unsigned int i = 0; i < ai_mesh.mNumFaces; i++)
	//	{
	//		const auto& face = ai_mesh.mFaces[i];
	//		assert(face.mNumIndices == 3);
	//		indices.push_back(face.mIndices[0]);
	//		indices.push_back(face.mIndices[1]);
	//		indices.push_back(face.mIndices[2]);
	//	}

	//	bindable_ptrs.push_back(BVertexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, vbuf));

	//	bindable_ptrs.push_back(BIndexBuffer::Resolve(BindableDesc({ desc.logger }), graphics, mesh_tag, indices));

	//	auto pvs = BVertexShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongVSNotex.cso");
	//	auto pvsbc = pvs->GetBytecode();
	//	bindable_ptrs.push_back(std::move(pvs));

	//	bindable_ptrs.push_back(BPixelShader::Resolve(BindableDesc({ desc.logger }), graphics, "PhongPSNotex.cso"));

	//	bindable_ptrs.push_back(BInputLayout::Resolve(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout(), pvsbc));

	//	dx3d_dcb::RawLayout lay;
	//	lay.Add<dx3d_dcb::Float4>(PSCBV_material_color);
	//	lay.Add<dx3d_dcb::Float4>(PSCBV_specular_color);
	//	lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);

	//	auto ps_cbuf = dx3d_dcb::Buffer(std::move(lay));
	//	ps_cbuf[PSCBV_material_color] = diffuse_color;
	//	ps_cbuf[PSCBV_specular_color] = specular_color;
	//	ps_cbuf[PSCBV_specular_power] = shininess;

	//	bindable_ptrs.push_back(std::make_shared<BCachingPixelConstantBufferEX>(BindableDesc({ desc.logger }), graphics, ps_cbuf, 1u));
	//}
	//else
	//{
	//	throw std::runtime_error("terrible combination of textures in material smh");
	//}

	//// anything with alpha diffuse is 2-sided IN SPONZA, need a better way
	//// of signalling 2-sidedness to be more general in the future
	//bindable_ptrs.push_back(BRasterizer::Resolve(BindableDesc({ desc.logger }), graphics, has_alpha_diffuse));

	//bindable_ptrs.push_back(BBlender::Resolve(BindableDesc({ desc.logger }), graphics, false));

	//bindable_ptrs.push_back(
	//	std::make_shared<BStencil>(
	//		BindableDesc({ desc.logger }), 
	//		graphics, 
	//		BStencil::Mode::Off
	//	)
	//);

	//return std::make_unique<Mesh>(DrawableDesc({ desc.logger }), graphics, std::move(bindable_ptrs));

	return {};
}

std::unique_ptr<dx3d::Node> dx3d::Model::ParseNode(int& next_id, const aiNode& node)
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> cur_mesh_ptrs;
	cur_mesh_ptrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		cur_mesh_ptrs.push_back(_mesh_ptrs.at(meshIdx).get());
	}

	auto node_ptr = std::make_unique<Node>(next_id++, node.mName.C_Str(), std::move(cur_mesh_ptrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		node_ptr->AddChild(ParseNode(next_id, *node.mChildren[i]));
	}

	return node_ptr;
}
