#pragma once
#include <optional>
#include <unordered_map>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Utils/DynamicConstant.hpp>

// ModelWindow
namespace dx3d {
	class Node;

	class ModelWindow { // pImpl idiom, only defined in this .cpp
	public:
		void Show(Graphics& graphics, const char* window_name, const Node& root) noexcept;
		void ApplyParameters() noxnd;

	private:
		DirectX::XMMATRIX GetTransform() const noxnd;
		const dx3d_dcb::Buffer& GetMaterial() const noxnd;
		bool TransformChanged() const noxnd;
		void ResetTransformChanged() noxnd;
		bool MaterialChanged() const noxnd;
		void ResetMaterialChanged() noxnd;
		bool IsChanged() const noxnd;

	private:
		struct TransformParameters {
			float roll{};
			float pitch{};
			float yaw{};
			float x{};
			float y{};
			float z{};
		};

		std::unordered_map<int, TransformParameters> _transforms;
	};
} // namespace dx3d
