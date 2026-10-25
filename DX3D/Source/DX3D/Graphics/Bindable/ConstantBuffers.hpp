#pragma once
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>

namespace dx3d {
	template<typename C>
	class ConstantBuffer : public Bindable {
		public:
			ConstantBuffer(const BindableDesc& desc, Graphics& graphics) : Bindable(desc) {

				D3D11_BUFFER_DESC constant_buffer_desc{};
				constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
				constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				constant_buffer_desc.MiscFlags = 0u;
				constant_buffer_desc.ByteWidth = sizeof(C);
				constant_buffer_desc.StructureByteStride = 0u;

				DX3DGraphicsLogThrowOnFail(
					GetDevice(graphics)->CreateBuffer(&constant_buffer_desc, nullptr, &_constant_buffer),
					"Failed to create the constant buffer"
				);
			}

			ConstantBuffer(const BindableDesc& desc, Graphics& graphics, const C& consts) : Bindable(desc) {
		
				D3D11_BUFFER_DESC constant_buffer_desc{};
				constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
				constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				constant_buffer_desc.MiscFlags = 0u;
				constant_buffer_desc.ByteWidth = sizeof(consts);
				constant_buffer_desc.StructureByteStride = 0u;
		
				D3D11_SUBRESOURCE_DATA constant_buffer_subresource_data{};
				constant_buffer_subresource_data.pSysMem = &consts;

				DX3DGraphicsLogThrowOnFail(
					GetDevice(graphics)->CreateBuffer(&constant_buffer_desc, &constant_buffer_subresource_data, &_constant_buffer),
					"Failed to create the constant buffer"
				);
			}
			
			virtual ~ConstantBuffer() = default;

			void Update(Graphics& graphics, const C& consts) {

				D3D11_MAPPED_SUBRESOURCE constant_buffer_mapped_subresource{};
				DX3DGraphicsLogThrowOnFail(GetContext(graphics)->Map(
					_constant_buffer.Get(), 0u,
					D3D11_MAP_WRITE_DISCARD, 0u,
					&constant_buffer_mapped_subresource
				), "Failed to update the constant buffer");
				memcpy(constant_buffer_mapped_subresource.pData, &consts, sizeof(consts));
				GetContext(graphics)->Unmap(_constant_buffer.Get(), 0u);
			}
		protected:
			Microsoft::WRL::ComPtr<ID3D11Buffer> _constant_buffer{};
	};

	template<typename C>
	class VertexConstantBuffer final : public ConstantBuffer<C> {
		using ConstantBuffer<C>::_constant_buffer;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;

		VertexConstantBuffer(const BindableDesc& desc, Graphics& graphics) : ConstantBuffer<C>(desc, graphics) {}
		VertexConstantBuffer(const BindableDesc& desc, Graphics& graphics, const C& consts) : ConstantBuffer<C>(desc, graphics, consts) {}

		void Bind(Graphics& graphics) noexcept override {
			GetContext(graphics)->VSSetConstantBuffers(0u, 1u, _constant_buffer.GetAddressOf());
		}
	};
	
	template<typename C>
	class PixelConstantBuffer final : public ConstantBuffer<C> {
		using ConstantBuffer<C>::_constant_buffer;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;

		PixelConstantBuffer(const BindableDesc& desc, Graphics& graphics) : ConstantBuffer<C>(desc, graphics) {}
		PixelConstantBuffer(const BindableDesc& desc, Graphics& graphics, const C& consts) : ConstantBuffer<C>(desc, graphics, consts) {}

		void Bind(Graphics& graphics) noexcept override {
			GetContext(graphics)->PSSetConstantBuffers(0u, 1u, _constant_buffer.GetAddressOf());
		}
	};
} // namespace dx3d
