#pragma once
#include <optional>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <DX3D/Window/ImGUI/imgui.h>

#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>
#include <DX3D/Graphics/Utils/Vertex.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffersEx.hpp>

namespace dx3d {

	class Mesh final : public Drawable
	{
	public:
		Mesh(const DrawableDesc& desc);
		Mesh(const DrawableDesc& desc, Graphics& graphics, const class Material& mat, const aiMesh& mesh) noxnd;
		void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulated_tranform) const noxnd;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		mutable DirectX::XMFLOAT4X4 _transform{};
	};

	class Node final
	{
		friend class Model;

	public:
		Node(const int id, const std::string& name, std::vector<Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noxnd;

		void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulated_tranform) const noxnd;
		void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
		const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
		void ShowTree(Node*& selected_node_ptr) const noexcept;
		int GetID() const noexcept;
		//const dx3d_dcb::Buffer* GetMaterialConstants() const noxnd;
		//void SetMaterialConstants(const dx3d_dcb::Buffer& buf_in) noxnd;

	private:
		void AddChild(std::unique_ptr<Node> child_ptr) noxnd;

	private:
		const int _id{};
		const std::string _name{};
		std::vector<std::unique_ptr<Node>> _child_ptrs{};
		std::vector<Mesh*> _mesh_ptrs{};
		DirectX::XMFLOAT4X4 _transform{};
		DirectX::XMFLOAT4X4 _applied_transform{};
	};

	class Model final : public Base
	{
	public:
		Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const float scale = 1.0f);
		~Model() noexcept;
		
		void Submit(FrameCommander& frame) const;
		void ShowWindow(Graphics& graphics, const char* window_name = nullptr) noexcept;
		void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;

	private:
		static std::unique_ptr<Mesh> ParseMesh(
			const BaseDesc& desc, 
			Graphics& graphics, 
			const std::filesystem::path& file_path, 
			const aiMesh& ai_mesh, 
			const aiMaterial* const* ai_material, 
			const float scale
		);
		std::unique_ptr<Node> ParseNode(int& next_id, const aiNode& node);

	private:
		std::unique_ptr<Node> _root{};
		std::vector<std::unique_ptr<Mesh>> _mesh_ptrs{};
		std::unique_ptr<class ModelWindow> _window{};
	};
} // namespace dx3d
