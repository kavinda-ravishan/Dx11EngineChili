#pragma once
#include <wrl.h>
#include <DX3D/Bindable/Bindable.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>

namespace dx3d {
	template<typename C>
	class ConstantBuffer : public Bindable {
		public:
			ConstantBuffer(const BindableDesc& desc, Graphics& graphics) : Bindable(desc) {

				D3D11_BUFFER_DESC cbd{};
				cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				cbd.Usage = D3D11_USAGE_DYNAMIC;
				cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				cbd.MiscFlags = 0u;
				cbd.ByteWidth = sizeof(C);
				cbd.StructureByteStride = 0u;

				DX3DGraphicsLogThrowOnFail(
					GetDevice(graphics)->CreateBuffer(&cbd, nullptr, &_constant_buffer),
					"Failed to create the constant buffer"
				);
			}

			ConstantBuffer(const BindableDesc& desc, Graphics& graphics, const C& consts) : Bindable(desc) {
		
				D3D11_BUFFER_DESC cbd{};
				cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				cbd.Usage = D3D11_USAGE_DYNAMIC;
				cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				cbd.MiscFlags = 0u;
				cbd.ByteWidth = sizeof(consts);
				cbd.StructureByteStride = 0u;
		
				D3D11_SUBRESOURCE_DATA csd{};
				csd.pSysMem = &consts;

				DX3DGraphicsLogThrowOnFail(
					GetDevice(graphics)->CreateBuffer(&cbd, &csd, &_constant_buffer),
					"Failed to create the constant buffer"
				);
			}
			
			virtual ~ConstantBuffer() = default;

			void Update(Graphics& graphics, const C& consts) {

				D3D11_MAPPED_SUBRESOURCE msr{};
				DX3DGraphicsLogThrowOnFail(GetContext(graphics)->Map(
					_constant_buffer.Get(), 0u,
					D3D11_MAP_WRITE_DISCARD, 0u,
					&msr
				), "Failed to update the constant buffer");
				memcpy(msr.pData, &consts, sizeof(consts));
				GetContext(graphics)->Unmap(_constant_buffer.Get(), 0u);
			}
		protected:
			Microsoft::WRL::ComPtr<ID3D11Buffer> _constant_buffer;
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
