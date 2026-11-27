#include <DX3D/Graphics/GraphicsResource.hpp>

ID3D11DeviceContext* dx3d::GraphicsResource::GetContext(Graphics& graphics) noexcept {
    return graphics._context.Get();
}

ID3D11Device* dx3d::GraphicsResource::GetDevice(Graphics& graphics) noexcept {
    return graphics._device.Get();
}
