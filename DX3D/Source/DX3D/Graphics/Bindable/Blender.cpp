#include <cassert>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/Blender.hpp>

dx3d::BBlender::BBlender(const BindableDesc& desc, Graphics& graphics, bool blending, std::optional<float> factor)
    : Bindable(desc), _blending(blending)
{
	if (factor)
	{
		_factors.emplace();
		_factors->fill(*factor);
	}

	D3D11_BLEND_DESC blend_desc{ CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} } };
	auto& brt = blend_desc.RenderTarget[0];
	if (blending) {
		brt.BlendEnable = TRUE;

		if (factor) {
			brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
			brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
		} else {
			brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		}
	}

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateBlendState(&blend_desc, &_blender),
		"Failed to create blend state"
	);
}

void dx3d::BBlender::Bind(Graphics& graphics) noexcept {
	const float* data = _factors ? _factors->data() : nullptr;
	GetContext(graphics)->OMSetBlendState(_blender.Get(), data, 0xFFFFFFFFu);
}

void dx3d::BBlender::SetFactor(float factor) noxnd {
	assert(_factors);
	return _factors->fill(factor);
}

float dx3d::BBlender::GetFactor() const noxnd {
	assert(_factors);
	return _factors->front();
}

std::shared_ptr<dx3d::BBlender> dx3d::BBlender::Resolve(const BindableDesc& desc, Graphics& graphics, bool blending, std::optional<float> factor) {
    return Codex::Resolve<BBlender>(desc, graphics, blending, factor);
}

std::string dx3d::BBlender::GenerateUID(bool blending, std::optional<float> factor) {
    using namespace std::string_literals;
	return typeid(BBlender).name() + "#"s + (blending ? "b"s : "n"s) + (factor ? "#f"s + std::to_string(*factor) : "");
}

std::string dx3d::BBlender::GetUID() const noexcept {
	return GenerateUID(_blending, _factors ? _factors->front() : std::optional<float>{});
}
