#pragma once
#include <vector>
#include <string>
#include <DX3D/Graphics/Graphics.hpp>

namespace dx3d {

	class Node final {
		friend class Model;

	public:
		Node(const int id, const std::string& name, std::vector<class Mesh*> mesh_ptrs, const DirectX::XMMATRIX& transform) noxnd;

		void Submit(class FrameCommander& frame, DirectX::FXMMATRIX accumulated_tranform) const noxnd;
		void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
		const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
		void ShowTree(Node*& selected_node_ptr) const noexcept;
		int GetID() const noexcept;

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
} // namespace dx3d
