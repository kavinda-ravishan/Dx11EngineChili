#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/VertexBuffer.hpp>

void dx3d::VertexBuffer::Bind(Graphics& graphics) noexcept {
	const UINT offset = 0u;
	GetContext(graphics)->IASetVertexBuffers(0, 1, _vertex_buffer.GetAddressOf(), &_stride, &offset);
}
