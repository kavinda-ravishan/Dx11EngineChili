#include <DX3D/Core/WinSwitches.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/DepthStencil.hpp>

dx3d::DepthStencil::DepthStencil(const BaseDesc& desc, Graphics& graphics, UINT width, UINT height)
	: Base(desc)
{
	// create depth stensil texture
	D3D11_TEXTURE2D_DESC depth_stensil_texture_desc{};
	depth_stensil_texture_desc.Width = width;
	depth_stensil_texture_desc.Height = height;
	depth_stensil_texture_desc.MipLevels = 1u;
	depth_stensil_texture_desc.ArraySize = 1u;
	depth_stensil_texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stensil_texture_desc.SampleDesc.Count = 1u;
	depth_stensil_texture_desc.SampleDesc.Quality = 0u;
	depth_stensil_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stensil_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stensil_texture{};
	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateTexture2D(&depth_stensil_texture_desc, nullptr, &depth_stensil_texture),
		"Failed to create depth stensil texture"
	);

	// create view of depth stencil texture
	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateDepthStencilView(depth_stensil_texture.Get(), nullptr, &_depth_stencil_view),
		"Failed to create depth stensil view"
	);
}

void dx3d::DepthStencil::BindAsDepthStencil(Graphics & graphics) const noexcept {
	GetContext(graphics)->OMSetRenderTargets(0, nullptr, _depth_stencil_view.Get());
}

void dx3d::DepthStencil::Clear(Graphics & graphics) const noexcept {
	GetContext(graphics)->ClearDepthStencilView(_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}
