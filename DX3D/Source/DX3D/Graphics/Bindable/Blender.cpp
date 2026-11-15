#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/Blender.hpp>

dx3d::BBlender::BBlender(const BindableDesc& desc, Graphics& graphics, bool blending) 
    : Bindable(desc), _blending(blending)
{
	D3D11_BLEND_DESC blend_desc {};
	auto& brt = blend_desc.RenderTarget[0];
	if (blending)
	{
		brt.BlendEnable = TRUE;
		brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		brt.BlendOp = D3D11_BLEND_OP_ADD;
		brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
		brt.DestBlendAlpha = D3D11_BLEND_ZERO;
		brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else
	{
		brt.BlendEnable = FALSE;
		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateBlendState(&blend_desc, &_blender),
		"Failed to create blend state"
	);
}

void dx3d::BBlender::Bind(Graphics& graphics) noexcept {
    GetContext(graphics)->OMSetBlendState(_blender.Get(), nullptr, 0xFFFFFFFFu);
}

std::shared_ptr<dx3d::BBlender> dx3d::BBlender::Resolve(const BindableDesc& desc, Graphics& graphics, bool blending) {
    return Codex::Resolve<BBlender>(desc, graphics, blending);
}

std::string dx3d::BBlender::GenerateUID(bool blending) {
    using namespace std::string_literals;
    return typeid(BBlender).name() + "#"s + (blending ? "b" : "n");
}

std::string dx3d::BBlender::GetUID() const noexcept {
    return GenerateUID(_blending);
}
