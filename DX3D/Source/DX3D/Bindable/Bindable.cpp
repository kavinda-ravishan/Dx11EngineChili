#include <DX3D/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Graphics.hpp>

dx3d::Bindable::Bindable(const BindableDesc& desc) : Base(desc.base) {}

ID3D11DeviceContext* dx3d::Bindable::GetContext(Graphics& graphics) noexcept {
    return graphics._context.Get();
}

ID3D11Device* dx3d::Bindable::GetDevice(Graphics& graphics) noexcept {
    return graphics._device.Get();
}
