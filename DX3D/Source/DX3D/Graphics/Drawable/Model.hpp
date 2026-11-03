#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <DX3D/Graphics/Bindable/BindableBase.hpp>
#include <DX3D/Graphics/Drawable/DrawableBase.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>

namespace dx3d {

	class Mesh : public DrawableBase<Mesh>
	{
	public:
		Mesh(const DrawableDesc& desc, Graphics& graphics, std::vector<std::unique_ptr<Bindable>> bindables)
			: DrawableBase<Mesh>(desc)
		{
			if (!IsStaticInitialized())
			{
				AddStaticBind(std::make_unique<Topology>(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			}

			for (auto& bindable : bindables)
			{
				if (auto pi = dynamic_cast<IndexBuffer*>(bindable.get()))
				{
					AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
					bindable.release();
				}
				else
				{
					AddBind(std::move(bindable));
				}
			}

			AddBind(std::make_unique<TransformCbuf>(BINDABLE_DESC, graphics, *this));
		}
		void Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noexcept(!IS_DEBUG)
		{
			DirectX::XMStoreFloat4x4(&_transform, accumulated_transform);
			Drawable::Draw(graphics);
		}
		DirectX::XMMATRIX GetTransformXM() const noexcept override
		{
			return DirectX::XMLoadFloat4x4(&_transform);
		}
	private:
		mutable DirectX::XMFLOAT4X4 _transform;
	};

	class Node
	{
		friend class Model;
	public:
		Node(std::vector<Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
			:
			_mesh_ptrs(std::move(mesh_ptrs))
		{
			DirectX::XMStoreFloat4x4(&this->_transform, transform);
		}
		void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulated_transform) const noexcept(!IS_DEBUG)
		{
			const auto built = DirectX::XMLoadFloat4x4(&_transform) * accumulated_transform;
			for (const auto pm : _mesh_ptrs)
			{
				pm->Draw(gfx, built);
			}
			for (const auto& pc : _child_ptrs)
			{
				pc->Draw(gfx, built);
			}
		}
	private:
		void AddChild(std::unique_ptr<Node> child) noexcept(!IS_DEBUG)
		{
			assert(child);
			_child_ptrs.push_back(std::move(child));
		}
	private:
		std::vector<std::unique_ptr<Node>> _child_ptrs;
		std::vector<Mesh*> _mesh_ptrs;
		DirectX::XMFLOAT4X4 _transform;
	};

	class Model : public Base
	{
	public:
		Model(const BaseDesc& desc, Graphics& graphics, const std::string file_name)
			: Base(desc)
		{
			Assimp::Importer imp;
			const auto scene = imp.ReadFile(file_name.c_str(),
				aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices
			);

			for (size_t i = 0; i < scene->mNumMeshes; i++)
			{
				_mesh_ptrs.push_back(ParseMesh(desc, graphics, *scene->mMeshes[i]));
			}

			_root = ParseNode(*scene->mRootNode);
		}
		static std::unique_ptr<Mesh> ParseMesh(const BaseDesc& desc, Graphics& graphics, const aiMesh& mesh)
		{
			dx3dexp::VertexBuffer vbuf(std::move(
				dx3dexp::VertexLayout{}
				.Append(dx3dexp::VertexLayout::Position3D)
				.Append(dx3dexp::VertexLayout::Normal)
			));

			for (unsigned int i = 0; i < mesh.mNumVertices; i++)
			{
				auto& vertex = mesh.mVertices[i];
				auto& normal = mesh.mNormals[i];

				vbuf.EmplaceBack(
					*reinterpret_cast<DirectX::XMFLOAT3*>(&vertex),
					*reinterpret_cast<DirectX::XMFLOAT3*>(&normal)
				);
			}

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

			std::vector<std::unique_ptr<Bindable>> bindables;

			bindables.push_back(std::make_unique<VertexBuffer>(BindableDesc({ desc.logger }), graphics, vbuf));

			bindables.push_back(std::make_unique<IndexBuffer>(BindableDesc({ desc.logger }), graphics, indices));

			auto pvs = std::make_unique<VertexShader>(BindableDesc({ desc.logger }), graphics, L"PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();
			bindables.push_back(std::move(pvs));

			bindables.push_back(std::make_unique<PixelShader>(BindableDesc({ desc.logger }), graphics, L"PhongPS.cso"));

			bindables.push_back(std::make_unique<InputLayout>(BindableDesc({ desc.logger }), graphics, vbuf.GetLayout().GetD3DLayout(), pvsbc));

			struct PSMaterialConstant
			{
				DirectX::XMFLOAT3 color{ 0.6f,0.6f,0.8f };
				float specular_intensity{ 0.6f };
				float specular_power{ 30.0f };
				float padding[3]{};
			} pmc;
			bindables.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(BindableDesc({ desc.logger }), graphics, pmc, 1u));

			return std::make_unique<Mesh>(DrawableDesc({ desc.logger }), graphics, std::move(bindables));
		}
		std::unique_ptr<Node> ParseNode(const aiNode& node)
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

			auto node_ptr = std::make_unique<Node>(std::move(cur_mesh_ptrs), transform);
			for (size_t i = 0; i < node.mNumChildren; i++)
			{
				node_ptr->AddChild(ParseNode(*node.mChildren[i]));
			}

			return node_ptr;
		}
		void Draw(Graphics& graphics, DirectX::FXMMATRIX transform) const
		{
			_root->Draw(graphics, transform);
		}
	private:
		std::unique_ptr<Node> _root;
		std::vector<std::unique_ptr<Mesh>> _mesh_ptrs;
	};
} // namespace dx3d
