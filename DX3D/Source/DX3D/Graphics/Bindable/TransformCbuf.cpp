#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/TransformCbuf.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffers.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

dx3d::TransformCbuf::TransformCbuf(const BindableDesc& desc, Graphics& graphics, const Drawable& parent)
	: Bindable(desc), _parent(parent) 
{
	if (!_static_vertex_constant_buffer) {
		_static_vertex_constant_buffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(desc, graphics);
	}
}

void dx3d::TransformCbuf::Bind(Graphics& graphics) noexcept {

	_static_vertex_constant_buffer->Update(
		graphics,
		DirectX::XMMatrixTranspose(
			_parent.GetTransformXM() * 
			graphics.GetCamera() * 
			graphics.GetProjection()
		)
	);
	_static_vertex_constant_buffer->Bind(graphics);
}

std::unique_ptr<dx3d::VertexConstantBuffer<DirectX::XMMATRIX>> dx3d::TransformCbuf::_static_vertex_constant_buffer;
