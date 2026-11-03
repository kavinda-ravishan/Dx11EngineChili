#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <DX3D/Graphics/Drawable/Vertex.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Drawable/AssImpTest.hpp>

dx3d::AssImpTest::AssImpTest(
	const DrawableDesc& desc, 
	Graphics& graphics, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist, 
	DirectX::XMFLOAT3 material, float scale)
	: TestObject<AssImpTest>(desc, rng, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		Assimp::Importer imp;
		const auto models = imp.ReadFile("Assets\\Models\\suzanne.obj", 
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);
		const auto mesh = models->mMeshes[0];

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh->mVertices[i].x * scale, mesh->mVertices[i].y * scale, mesh->mVertices[i].z * scale), 
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const auto& face = mesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		AddStaticBind(std::make_unique<VertexBufferB>(BINDABLE_DESC, graphics, vbuf));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(BINDABLE_DESC, graphics, indices));

		auto pvs = std::make_unique<VertexShader>(BINDABLE_DESC, graphics, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(BINDABLE_DESC, graphics, L"PhongPS.cso"));

		AddStaticBind(std::make_unique<InputLayout>(BINDABLE_DESC, graphics, vbuf.GetLayout().GetD3DLayout(), pvsbc));

		AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color{};
		float specular_intensity{ 0.6f };
		float specular_power{ 30.0f };
		float padding[3]{};
	} pmc;
	pmc.color = material;
	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BINDABLE_DESC, graphics, pmc, 1u));

	AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
}
