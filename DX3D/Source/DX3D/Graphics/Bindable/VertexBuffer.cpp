#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/VertexBuffer.hpp>

dx3d::BVertexBuffer::BVertexBuffer(const BindableDesc& desc, Graphics& graphics, const dx3d::VertexBuffer& vbuf)
	: BVertexBuffer(desc, graphics, "?", vbuf)
{}

dx3d::BVertexBuffer::BVertexBuffer(const BindableDesc& desc, Graphics& graphics, const std::string tag, const dx3d::VertexBuffer& vbuf)
	: Bindable(desc), _tag(tag), _stride((UINT)vbuf.GetLayout().Size())
{
	D3D11_BUFFER_DESC vertex_buffer_desc{};
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.CPUAccessFlags = 0u;
	vertex_buffer_desc.MiscFlags = 0u;
	vertex_buffer_desc.ByteWidth = UINT(vbuf.SizeBytes());
	vertex_buffer_desc.StructureByteStride = _stride;

	D3D11_SUBRESOURCE_DATA vertex_buffer_sub_resource_data{};
	vertex_buffer_sub_resource_data.pSysMem = vbuf.GetData();

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateBuffer(
			&vertex_buffer_desc,
			&vertex_buffer_sub_resource_data,
			&_vertex_buffer),
		"Failed to create the vertex buffer"
	);
}

void dx3d::BVertexBuffer::Bind(Graphics& graphics) noexcept {
	const UINT offset = 0u;
	GetContext(graphics)->IASetVertexBuffers(0, 1, _vertex_buffer.GetAddressOf(), &_stride, &offset);
}

std::shared_ptr<dx3d::BVertexBuffer> dx3d::BVertexBuffer::Resolve(const BindableDesc& desc, Graphics& graphics, const std::string tag, const VertexBuffer& vbuf) {
	assert(tag != "?");
	return Codex::Resolve<BVertexBuffer>(desc, graphics, tag, vbuf);
}

std::string dx3d::BVertexBuffer::GetUID() const noexcept {
	return GenerateUID(_tag);
}

std::string dx3d::BVertexBuffer::GenerateUID_(const std::string& tag) {
	using namespace std::string_literals;
	return typeid(VertexBuffer).name() + "#"s + tag;
}
