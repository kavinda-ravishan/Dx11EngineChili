#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/Rasterizer.hpp>

dx3d::BRasterizer::BRasterizer(const BindableDesc& desc, Graphics& graphics, bool two_sided)
	: Bindable(desc), _two_sided(two_sided)
{
	D3D11_RASTERIZER_DESC raster_desc{ CD3D11_RASTERIZER_DESC{ CD3D11_DEFAULT{} } };
	raster_desc.CullMode = two_sided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateRasterizerState(&raster_desc, &_rasterizer),
		"Failed to create rasterizer state"
	);
}

void dx3d::BRasterizer::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->RSSetState(_rasterizer.Get());
}

std::shared_ptr<dx3d::BRasterizer> dx3d::BRasterizer::Resolve(const BindableDesc& desc, Graphics& graphics, bool two_sided) {
	return Codex::Resolve<BRasterizer>(desc, graphics, two_sided);
}

std::string dx3d::BRasterizer::GenerateUID(bool two_sided) {
	using namespace std::string_literals;
	return typeid(BRasterizer).name() + "#"s + (two_sided ? "2s" : "1s");
}

std::string dx3d::BRasterizer::GetUID() const noexcept {
	return GenerateUID(_two_sided);
}
