#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	template<typename C>
	class BConstantBuffer : public Bindable {
		public:
			BConstantBuffer(const BindableDesc& desc, Graphics& graphics, UINT slot = 0u)
				: Bindable(desc), _slot(slot)
			{

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

			BConstantBuffer(const BindableDesc& desc, Graphics& graphics, const C& consts, UINT slot = 0u)
				: Bindable(desc), _slot(slot)
			{
		
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
			
			virtual ~BConstantBuffer() = default;

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
			const UINT _slot{};
			Microsoft::WRL::ComPtr<ID3D11Buffer> _constant_buffer{};
	};

	template<typename C>
	class BVertexConstantBuffer final : public BConstantBuffer<C> {
		using BConstantBuffer<C>::_constant_buffer;
		using BConstantBuffer<C>::_slot;
		using Bindable::GetContext;
	public:
		using BConstantBuffer<C>::BConstantBuffer;

		BVertexConstantBuffer(const BindableDesc& desc, Graphics& graphics, UINT slot = 0u) : BConstantBuffer<C>(desc, graphics, slot) {}
		BVertexConstantBuffer(const BindableDesc& desc, Graphics& graphics, const C& consts, UINT slot = 0u) : BConstantBuffer<C>(desc, graphics, consts, slot) {}

		void Bind(Graphics& graphics) noexcept override {
			GetContext(graphics)->VSSetConstantBuffers(_slot, 1u, _constant_buffer.GetAddressOf());
		}

		static std::shared_ptr<BVertexConstantBuffer> Resolve(const BindableDesc& desc, Graphics& graphics, const C& consts, UINT slot = 0) {
			return Codex::Resolve<BVertexConstantBuffer>(desc, graphics, consts, slot);
		}

		static std::shared_ptr<BVertexConstantBuffer> Resolve(const BindableDesc& desc, Graphics& graphics, UINT slot = 0) {
			return Codex::Resolve<BVertexConstantBuffer>(desc, graphics, slot);
		}

		static std::string GenerateUID(const C&, UINT slot) {
			return GenerateUID(slot);
		}

		static std::string GenerateUID(UINT slot = 0) {
			using namespace std::string_literals;
			return typeid(BVertexConstantBuffer).name() + "#"s + std::to_string(slot);
		}

		std::string GetUID() const noexcept override {
			return GenerateUID(_slot);
		}
	};
	
	template<typename C>
	class BPixelConstantBuffer final : public BConstantBuffer<C> {
		using BConstantBuffer<C>::_constant_buffer;
		using BConstantBuffer<C>::_slot;
		using Bindable::GetContext;
	public:
		using BConstantBuffer<C>::BConstantBuffer;

		BPixelConstantBuffer(const BindableDesc& desc, Graphics& graphics, UINT slot = 0u) : BConstantBuffer<C>(desc, graphics, slot) {}
		BPixelConstantBuffer(const BindableDesc& desc, Graphics& graphics, const C& consts, UINT slot = 0u) : BConstantBuffer<C>(desc, graphics, consts, slot) {}

		void Bind(Graphics& graphics) noexcept override {
			GetContext(graphics)->PSSetConstantBuffers(_slot, 1u, _constant_buffer.GetAddressOf());
		}

		static std::shared_ptr<BPixelConstantBuffer> Resolve(const BindableDesc& desc, Graphics& graphics, const C& consts, UINT slot = 0) {
			return Codex::Resolve<BPixelConstantBuffer>(desc, graphics, consts, slot);
		}

		static std::shared_ptr<BPixelConstantBuffer> Resolve(const BindableDesc& desc, Graphics& graphics, UINT slot = 0) {
			return Codex::Resolve<BPixelConstantBuffer>(desc, graphics, slot);
		}

		static std::string GenerateUID(const C&, UINT slot) {
			return GenerateUID(slot);
		}

		static std::string GenerateUID(UINT slot = 0) {
			using namespace std::string_literals;
			return typeid(BPixelConstantBuffer).name() + "#"s + std::to_string(slot);
		}

		std::string GetUID() const noexcept override {
			return GenerateUID(_slot);
		}
	};
} // namespace dx3d
