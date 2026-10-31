#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/TransformCbuf.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffers.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

dx3d::TransformCbuf::TransformCbuf(const BindableDesc& desc, Graphics& graphics, const Drawable& parent, UINT slot)
	: Bindable(desc), _parent(parent) 
{
	if (!_static_vertex_constant_buffer) {
		_static_vertex_constant_buffer = std::make_unique<VertexConstantBuffer<Transforms>>(desc, graphics, slot);
	}
}

void dx3d::TransformCbuf::Bind(Graphics& graphics) noexcept {

	const auto model = _parent.GetTransformXM();

	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(
			model *
			graphics.GetCamera() *
			graphics.GetProjection()
		)
	};

	_static_vertex_constant_buffer->Update(graphics, tf);
	_static_vertex_constant_buffer->Bind(graphics);
}

std::unique_ptr<dx3d::VertexConstantBuffer<dx3d::TransformCbuf::Transforms>> dx3d::TransformCbuf::_static_vertex_constant_buffer;
