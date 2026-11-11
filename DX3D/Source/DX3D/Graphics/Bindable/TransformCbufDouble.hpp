#pragma once
#include <DX3D/Graphics/Bindable/TransformCbuf.hpp>

namespace dx3d {
	template<typename C>
	class BPixelConstantBuffer;

	class BTransformCbufDouble : public BTransformCbuf {
	public:
		BTransformCbufDouble(const BindableDesc& desc, Graphics& graphics, const Drawable& parent, UINT slot_v = 0u, UINT slot_p = 0u);
		void Bind(Graphics& graphics) noexcept override;
	
	protected:
		void UpdateBindImpl(Graphics& graphics, const Transforms& tf) noexcept;
	
	private:
		static std::unique_ptr<BPixelConstantBuffer<Transforms>> _static_pixel_constant_buffer;
	};
} // namespace dx3d
