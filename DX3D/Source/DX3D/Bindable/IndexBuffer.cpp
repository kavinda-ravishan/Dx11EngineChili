#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Bindable/IndexBuffer.hpp>

dx3d::IndexBuffer::IndexBuffer(const BindableDesc& desc, Graphics& graphics, const std::vector<unsigned short>& indices)
    : Bindable(desc), _count((UINT)indices.size()) 
{
	D3D11_BUFFER_DESC index_buffer_desc{};
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.CPUAccessFlags = 0u;
	index_buffer_desc.MiscFlags = 0u;
	index_buffer_desc.ByteWidth = UINT(_count * sizeof(unsigned short));
	index_buffer_desc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA i_sub_resource_data{};
	i_sub_resource_data.pSysMem = indices.data();

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateBuffer(&index_buffer_desc, &i_sub_resource_data, &_index_buffer),
		"Failed to create the index buffer"
	);
}

void dx3d::IndexBuffer::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->IASetIndexBuffer(_index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT dx3d::IndexBuffer::GetCount() const noexcept {
    return _count;
}
