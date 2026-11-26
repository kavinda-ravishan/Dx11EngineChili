#pragma once
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

struct aiMesh;

namespace dx3d {
	class Material;
	class FrameCommander;

	class Mesh final : public Drawable {
	public:
		Mesh(const DrawableDesc& desc, Graphics& graphics, const class Material& mat, const aiMesh& mesh, const float scale = 1.0f) noxnd;
		void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulated_tranform) const noxnd;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	
	private:
		mutable DirectX::XMFLOAT4X4 _transform{};
	};
} // namespace dx3d
