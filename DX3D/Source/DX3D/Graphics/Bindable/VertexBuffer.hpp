#pragma once
#include <vector>
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>

namespace dx3d {
	class VertexBuffer : public Bindable
	{
	public:
		template<class V>
		VertexBuffer(const BindableDesc& desc, Graphics& graphics, const std::vector<V>& vertices)
			: Bindable(desc), _stride(sizeof(V))
		{
			D3D11_BUFFER_DESC vertex_buffer_desc{};
			vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			vertex_buffer_desc.CPUAccessFlags = 0u;
			vertex_buffer_desc.MiscFlags = 0u;
			vertex_buffer_desc.ByteWidth = UINT(sizeof(V) * vertices.size());
			vertex_buffer_desc.StructureByteStride = sizeof(V);

			D3D11_SUBRESOURCE_DATA vertex_buffer_sub_resource_data{};
			vertex_buffer_sub_resource_data.pSysMem = vertices.data();

			DX3DGraphicsLogThrowOnFail(
				GetDevice(graphics)->CreateBuffer(
					&vertex_buffer_desc, 
					&vertex_buffer_sub_resource_data, 
					&_vertex_buffer),
				"Failed to create the vertex buffer"
			);
		}

		void Bind(Graphics& graphics) noexcept override;
	protected:
		const UINT _stride{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer{};
	};
} // namespace dx3d
