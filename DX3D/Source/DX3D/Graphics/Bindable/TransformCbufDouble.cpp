#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/TransformCbufDouble.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffers.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

dx3d::BTransformCbufDouble::BTransformCbufDouble(const BindableDesc& desc, Graphics& graphics, const Drawable& parent, UINT slot_v, UINT slot_p)
	:BTransformCbuf(desc, graphics, parent, slot_v)
{
	if (!_static_pixel_constant_buffer) {
		_static_pixel_constant_buffer = std::make_unique<BPixelConstantBuffer<Transforms>>(desc, graphics, slot_p);
	}
}

void dx3d::BTransformCbufDouble::Bind(Graphics& graphics) noexcept {

	const auto tf = GetTransforms(graphics);
	BTransformCbuf::UpdateBindImpl(graphics, tf);
	BTransformCbufDouble::UpdateBindImpl(graphics, tf);
}

void dx3d::BTransformCbufDouble::UpdateBindImpl(Graphics& graphics, const Transforms& tf) noexcept {
	_static_pixel_constant_buffer->Update(graphics, tf);
	_static_pixel_constant_buffer->Bind(graphics);
}

std::unique_ptr<dx3d::BPixelConstantBuffer<dx3d::BTransformCbuf::Transforms>> dx3d::BTransformCbufDouble::_static_pixel_constant_buffer;
