#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/Topology.hpp>

dx3d::BTopology::BTopology(const BindableDesc& desc, Graphics& graphics, const D3D11_PRIMITIVE_TOPOLOGY type) : Bindable(desc), _type(type) {}

void dx3d::BTopology::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->IASetPrimitiveTopology(_type);
}

std::shared_ptr<dx3d::BTopology> dx3d::BTopology::Resolve(const BindableDesc& desc, Graphics& graphics, const D3D11_PRIMITIVE_TOPOLOGY type) {
	return Codex::Resolve<BTopology>(desc, graphics, type);
}

std::string dx3d::BTopology::GenerateUID(const D3D11_PRIMITIVE_TOPOLOGY type) {
	using namespace std::string_literals;
	return typeid(BTopology).name() + "#"s + std::to_string(type);
}

std::string dx3d::BTopology::GetUID() const noexcept {
	return GenerateUID(_type);
}
