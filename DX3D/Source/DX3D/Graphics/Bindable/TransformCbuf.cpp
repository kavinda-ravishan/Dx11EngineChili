#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/TransformCbuf.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffers.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

dx3d::BTransformCbuf::BTransformCbuf(const BindableDesc& desc, Graphics& graphics, const Drawable& parent, UINT slot)
	: Bindable(desc), _parent(parent) 
{
	if (!_static_vertex_constant_buffer) {
		_static_vertex_constant_buffer = std::make_unique<BVertexConstantBuffer<Transforms>>(desc, graphics, slot);
	}
}

void dx3d::BTransformCbuf::Bind(Graphics& graphics) noexcept {
	UpdateBindImpl(graphics, GetTransforms(graphics));
}

void dx3d::BTransformCbuf::UpdateBindImpl(Graphics& graphics, const Transforms& tf) noexcept {
	_static_vertex_constant_buffer->Update(graphics, tf);
	_static_vertex_constant_buffer->Bind(graphics);
}

dx3d::BTransformCbuf::Transforms dx3d::BTransformCbuf::GetTransforms(Graphics& graphics) noexcept {
	const auto model_view = _parent.GetTransformXM() * graphics.GetCamera();

	return
	{
		DirectX::XMMatrixTranspose(model_view),
		DirectX::XMMatrixTranspose(
			model_view *
			graphics.GetProjection()
		)
	};
}

std::unique_ptr<dx3d::BVertexConstantBuffer<dx3d::BTransformCbuf::Transforms>> dx3d::BTransformCbuf::_static_vertex_constant_buffer;
