#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/TransformCbuf.hpp>
#include <DX3D/Drawable/Drawable.hpp>

dx3d::TransformCbuf::TransformCbuf(const BindableDesc& desc, Graphics& graphics, const Drawable& parent)
	: Bindable(desc), _vertex_constant_buffer(desc, graphics), _parent(parent) {}

void dx3d::TransformCbuf::Bind(Graphics& graphics) noexcept {

	_vertex_constant_buffer.Update(
		graphics,
		DirectX::XMMatrixTranspose(
			_parent.GetTransformXM() * graphics.GetProjection()
		)
	);
	_vertex_constant_buffer.Bind(graphics);
}
