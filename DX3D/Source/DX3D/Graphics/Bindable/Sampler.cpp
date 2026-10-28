#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/Sampler.hpp>

dx3d::Sampler::Sampler(const BindableDesc& desc, Graphics& graphics)
	: Bindable(desc)
{
	D3D11_SAMPLER_DESC sampler_desc{};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; // For 3D texture (this case we only use 2D texture)

	sampler_desc.BorderColor[0] = 0.0f;
	sampler_desc.BorderColor[1] = 1.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 0.0f;

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateSamplerState(&sampler_desc, &_sampler), 
		"Failed to create sampler"
	);
}

void dx3d::Sampler::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->PSSetSamplers(0u, 1u, _sampler.GetAddressOf());
}
