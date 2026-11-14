#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/Sampler.hpp>

dx3d::BSampler::BSampler(const BindableDesc& desc, Graphics& graphics)
	: Bindable(desc)
{
	D3D11_SAMPLER_DESC sampler_desc{};
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MinLOD = 0.0f;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	sampler_desc.BorderColor[0] = 0.0f;
	sampler_desc.BorderColor[1] = 1.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateSamplerState(&sampler_desc, &_sampler), 
		"Failed to create sampler"
	);
}

void dx3d::BSampler::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->PSSetSamplers(0u, 1u, _sampler.GetAddressOf());
}

std::shared_ptr<dx3d::BSampler> dx3d::BSampler::Resolve(const BindableDesc& desc, Graphics& graphics) {
	return Codex::Resolve<BSampler>(desc, graphics);
}

std::string dx3d::BSampler::GenerateUID() {
	return typeid(BSampler).name();
}

std::string dx3d::BSampler::GetUID() const noexcept {
	return GenerateUID();
}
