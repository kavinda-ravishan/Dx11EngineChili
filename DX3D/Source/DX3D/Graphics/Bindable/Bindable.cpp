#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Graphics.hpp>

// Bindable
dx3d::Bindable::Bindable(const BindableDesc& desc) : ClonableBase(desc.base) {}

void dx3d::Bindable::InitializeParentReference(const Drawable&) noexcept
{}

void dx3d::Bindable::Accept(TechniqueProbe&)
{}

std::string dx3d::Bindable::GetUID() const noexcept {
    assert(false);
    return "";
}

ID3D11DeviceContext* dx3d::Bindable::GetContext(Graphics& graphics) noexcept {
    return graphics._context.Get();
}

ID3D11Device* dx3d::Bindable::GetDevice(Graphics& graphics) noexcept {
    return graphics._device.Get();
}

// CloningBindable
dx3d::CloningBindable::CloningBindable(const BindableDesc& desc) : Bindable(desc) {}
