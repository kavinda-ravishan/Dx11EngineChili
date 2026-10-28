#include <DX3D/Utils/Surface.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/Texture.hpp>

dx3d::Texture::Texture(const BindableDesc& desc, Graphics& graphics, const Surface& surface)
	: Bindable(desc)
{
	// create texture resource
	D3D11_TEXTURE2D_DESC texture_desc{};
	texture_desc.Width = surface.GetWidth();
	texture_desc.Height = surface.GetHeight();
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = surface.GetBufferPtr();
	subresource_data.SysMemPitch = surface.GetWidth() * sizeof(Surface::Color);
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	DX3DGraphicsLogThrowOnFail(GetDevice(graphics)->CreateTexture2D(
		&texture_desc, &subresource_data, &texture
	), "Failed to create texture");

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	DX3DGraphicsLogThrowOnFail(GetDevice(graphics)->CreateShaderResourceView(
		texture.Get(), &shader_resource_view_desc, &_texture_view
	), "Failed to create texture resource view");
}

void dx3d::Texture::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->PSSetShaderResources(0u, 1u, _texture_view.GetAddressOf());
}
