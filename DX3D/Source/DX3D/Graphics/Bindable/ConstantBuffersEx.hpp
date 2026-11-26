#pragma once
#include <DX3D/Core/ConditionalNoexcept.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/RenderJobs/TechniqueProbe.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Utils/DynamicConstant.hpp>

namespace dx3d {

	class ConstantBufferEx : public Bindable
	{
	public:
		void Update(Graphics& graphics, const dx3d_dcb::Buffer& buf)
		{
			assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());

			D3D11_MAPPED_SUBRESOURCE msr;
			DX3DGraphicsLogThrowOnFail(
				GetContext(graphics)->Map(
					_constant_buffer.Get(), 0u,
					D3D11_MAP_WRITE_DISCARD, 0u,
					&msr
				),
				"Failed to update the constant buffer"
			);

			memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
			GetContext(graphics)->Unmap(_constant_buffer.Get(), 0u);
		}
		// this exists for validation of the update buffer layout
		// reason why it's not getbuffer is becasue nocache doesn't store buffer
		virtual const dx3d_dcb::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		ConstantBufferEx(const BindableDesc& desc, Graphics& graphics, const dx3d_dcb::LayoutElement& layoutRoot, UINT slot, const dx3d_dcb::Buffer* buf_ptr)
			: Bindable(desc), _slot(slot)
		{
			D3D11_BUFFER_DESC cbd{};
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if (buf_ptr != nullptr)
			{
				D3D11_SUBRESOURCE_DATA csd{};
				csd.pSysMem = buf_ptr->GetData();
				DX3DGraphicsLogThrowOnFail(
					GetDevice(graphics)->CreateBuffer(&cbd, &csd, &_constant_buffer),
					"Failed to create the constant buffer"
				);
			}
			else
			{
				DX3DGraphicsLogThrowOnFail(
					GetDevice(graphics)->CreateBuffer(&cbd, nullptr, &_constant_buffer),
					"Failed to create the constant buffer"
				);
			}
		}
	protected:
		const UINT _slot{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> _constant_buffer{};
	};

	class PixelConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;

		PixelConstantBufferEx(const BindableDesc& desc, Graphics& graphics, const dx3d_dcb::LayoutElement& layout_root, UINT slot, const dx3d_dcb::Buffer* buf_ptr)
		: ConstantBufferEx(desc, graphics, layout_root, slot, buf_ptr){}

		void Bind(Graphics& graphics) noexcept override
		{
			GetContext(graphics)->PSSetConstantBuffers(_slot, 1u, _constant_buffer.GetAddressOf());
		}
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;

		VertexConstantBufferEx(const BindableDesc& desc, Graphics& graphics, const dx3d_dcb::LayoutElement& layout_root, UINT slot, const dx3d_dcb::Buffer* buf_ptr)
			: ConstantBufferEx(desc, graphics, layout_root, slot, buf_ptr) {}

		void Bind(Graphics& graphics) noexcept override
		{
			GetContext(graphics)->VSSetConstantBuffers(_slot, 1u, _constant_buffer.GetAddressOf());
		}
	};

	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(const BindableDesc& desc, Graphics& graphics, const dx3d_dcb::CookedLayout& layout, UINT slot)
			:
			T(desc, graphics, *layout.ShareRoot(), slot, nullptr),
			_buf(dx3d_dcb::Buffer(layout))
		{}
		CachingConstantBufferEx(const BindableDesc& desc, Graphics& graphics, const dx3d_dcb::Buffer& buf, UINT slot)
			:
			T(desc, graphics, buf.GetRootLayoutElement(), slot, &buf),
			_buf(buf)
		{}
		const dx3d_dcb::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return _buf.GetRootLayoutElement();
		}
		const dx3d_dcb::Buffer& GetBuffer() const noexcept
		{
			return _buf;
		}
		void SetBuffer(const dx3d_dcb::Buffer& buf_in)
		{
			_buf.CopyFrom(buf_in);
			_changed = true;
		}
		void Bind(Graphics& graphics) noexcept override
		{
			if (_changed)
			{
				T::Update(graphics, _buf);
				_changed = false;
			}
			T::Bind(graphics);
		}
		void Accept(TechniqueProbe& probe) override
		{
			if (probe.VisitBuffer(_buf))
			{
				_changed = true;
			}
		}
	private:
		bool _changed{ false };
		dx3d_dcb::Buffer _buf;
	};

	using BCachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using BCachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;
}