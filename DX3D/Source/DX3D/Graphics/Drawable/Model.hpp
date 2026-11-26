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
	class ModelProbe;

	class Model final : public Base {
	public:
		Model(const BaseDesc& desc, Graphics& graphics, const std::filesystem::path file_path, const float scale = 1.0f);
		~Model() noexcept;

		void Submit(FrameCommander& frame) const;
		void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
		void Accept(ModelProbe& probe);

	private:
		std::unique_ptr<Node> ParseNode(int& next_id, const aiNode& node, DirectX::FXMMATRIX additional_transform);

	private:
		std::unique_ptr<Node> _root{};
		// sharing meshes here perhaps dangerous?
		std::vector<std::unique_ptr<Mesh>> _mesh_ptrs{};
	};
} // namespace dx3d
