#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>
#include <DX3D/Core/Core.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffers.hpp>

namespace dx3d {
	class TransformCbuf : public Bindable {
	public:
		TransformCbuf(const BindableDesc& desc, Graphics& graphics, const Drawable& parent);
		void Bind(Graphics& graphics) noexcept override;
	private:
		VertexConstantBuffer<DirectX::XMMATRIX> _vertex_constant_buffer;
		const Drawable& _parent;
	};
} // namespace dx3d
