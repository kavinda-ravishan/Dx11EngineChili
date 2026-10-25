#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Bindable/Topology.hpp>

dx3d::Topology::Topology(const BindableDesc& desc, Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type) : Bindable(desc), _type(type) {}

void dx3d::Topology::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->IASetPrimitiveTopology(_type);
}
