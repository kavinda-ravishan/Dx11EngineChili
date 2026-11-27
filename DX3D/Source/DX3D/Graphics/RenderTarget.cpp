#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/DepthStencil.hpp>
#include <DX3D/Graphics/RenderTarget.hpp>

dx3d::RenderTarget::RenderTarget(const BaseDesc& desc, Graphics& graphics, UINT width, UINT height)
	: Base(desc)
{
	// create texture resource
	D3D11_TEXTURE2D_DESC texture_desc {};
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture{};
	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateTexture2D(&texture_desc, nullptr, &texture), 
		"[RenderTarget] : Failed to create Texture2D"
	);

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc {};
	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;
	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateShaderResourceView(texture.Get(), &srv_desc, &_texture_view), 
		"[RenderTarget] : Failed to create ShaderResourceView for Texture2D"
	);

	// create the target view on the texture
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc {};
	rtv_desc.Format = texture_desc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D = D3D11_TEX2D_RTV{ 0 };
	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateRenderTargetView(texture.Get(), &rtv_desc, &_target_view), 
		"[RenderTarget] : Failed to create RenderTargetView for Texture2D"
	);
}

void dx3d::RenderTarget::BindAsTexture(Graphics & graphics, UINT slot) const noexcept {
	GetContext(graphics)->PSSetShaderResources(slot, 1, _texture_view.GetAddressOf());
}

// NOTE : to fix trying to bind a texture as a render target while that same resource is still bound as an input resource issue
void dx3d::RenderTarget::UnbindAsTexture(Graphics& graphics, UINT slot) const noexcept {
	ID3D11ShaderResourceView* null_srv[] = { nullptr };
	GetContext(graphics)->PSSetShaderResources(slot, 1, null_srv);
}

void dx3d::RenderTarget::BindAsTarget(Graphics & graphics) const noexcept {
	GetContext(graphics)->OMSetRenderTargets(1, _target_view.GetAddressOf(), nullptr);
}

void dx3d::RenderTarget::BindAsTarget(Graphics & graphics, const DepthStencil & depth_stencil) const noexcept {
	GetContext(graphics)->OMSetRenderTargets(1, _target_view.GetAddressOf(), depth_stencil._depth_stencil_view.Get());
}
