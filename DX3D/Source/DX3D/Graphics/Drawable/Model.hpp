#pragma once
#include <filesystem>
#include <DX3D/Core/Base.hpp>
#include <DX3D/Graphics/Graphics.hpp>

struct aiMesh;
struct aiMaterial;
struct aiNode;

namespace dx3d {

	class Node;
	class Mesh;
	class FrameCommander;

	class Model final : public Base {
	public:
		Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const float scale = 1.0f);
		~Model() noexcept;

		void Submit(FrameCommander& frame) const;
		//void ShowWindow(Graphics& graphics, const char* window_name = nullptr) noexcept;
		void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;

	private:
		std::unique_ptr<Node> ParseNode(int& next_id, const aiNode& node);

	private:
		std::unique_ptr<Node> _root{};
		std::vector<std::unique_ptr<Mesh>> _mesh_ptrs{};
		//std::unique_ptr<class ModelWindow> _window{};
	};
} // namespace dx3d
