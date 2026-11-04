#pragma once
#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Drawable/DrawableBase.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>

namespace dx3d {

	class Mesh : public DrawableBase<Mesh>
	{
	public:
		Mesh(const DrawableDesc& desc, Graphics& graphics, std::vector<std::unique_ptr<Bindable>> bindables);

		void Draw(Graphics& graphics, DirectX::FXMMATRIX accumulated_transform) const noexcept(!IS_DEBUG);

		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		mutable DirectX::XMFLOAT4X4 _transform;
	};

	class Node
	{
		friend class Model;
	public:
		Node(const std::string& name, std::vector<Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG);

		void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulated_transform) const noexcept(!IS_DEBUG);
		void ShowTree(int& node_index_tracked, std::optional<int>& selected_index) const noexcept;
	private:
		void AddChild(std::unique_ptr<Node> child) noexcept(!IS_DEBUG);

	private:
		const std::string _name{};
		std::vector<std::unique_ptr<Node>> _child_ptrs{};
		std::vector<Mesh*> _mesh_ptrs{};
		DirectX::XMFLOAT4X4 _transform{};
	};

	class Model : public Base
	{
	public:
		Model(const BaseDesc& desc, Graphics& graphics, const std::string file_name);
		~Model() noexcept;
		
		void Draw(Graphics& graphics) const;
		void ShowWindow(const char* window_name = nullptr) noexcept;

	private:
		static std::unique_ptr<Mesh> ParseMesh(const BaseDesc& desc, Graphics& graphics, const aiMesh& mesh);
		std::unique_ptr<Node> ParseNode(const aiNode& node);

	private:
		std::unique_ptr<Node> _root{};
		std::vector<std::unique_ptr<Mesh>> _mesh_ptrs{};
		std::unique_ptr<class ModelWindow> _window{};
	};
} // namespace dx3d
