#include <DX3D/Utils/Surface.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/Texture.hpp>
#include "Texture.hpp"

dx3d::BTexture::BTexture(const BindableDesc& desc, Graphics& graphics, const std::string& path, unsigned int slot)
	: Bindable(desc), _path(path), _slot(slot)
{
	DX3DLogInfo(("[Texture] : Loading: " + path).c_str());
	const auto surface = Surface(path);
	_has_alpha = surface.AlphaLoaded();

	// create texture resource
	D3D11_TEXTURE2D_DESC texture_desc{};
	texture_desc.Width = surface.GetWidth();
	texture_desc.Height = surface.GetHeight();
	texture_desc.MipLevels = 0;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	DX3DGraphicsLogThrowOnFail(GetDevice(graphics)->CreateTexture2D(
		&texture_desc, nullptr, &texture
	), "Failed to create texture");

	// write image data into top mip level
	GetContext(graphics)->UpdateSubresource(
		texture.Get(), 
		0u, 
		nullptr, 
		reinterpret_cast<const void*>(surface.GetBufferPtrConst()),
		surface.GetWidth() * sizeof(Surface::Color), 0u
	);

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
	shader_resource_view_desc.Texture2D.MipLevels = -1;

	DX3DGraphicsLogThrowOnFail(GetDevice(graphics)->CreateShaderResourceView(
		texture.Get(), &shader_resource_view_desc, &_texture_view
	), "Failed to create texture resource view");

	// generate the mip chain using the gpu rendering pipeline
	GetContext(graphics)->GenerateMips(_texture_view.Get());
}

void dx3d::BTexture::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->PSSetShaderResources(_slot, 1u, _texture_view.GetAddressOf());
}

std::shared_ptr<dx3d::BTexture> dx3d::BTexture::Resolve(const BindableDesc& desc, Graphics& graphics, const std::string& path, unsigned int slot) {
	return Codex::Resolve<BTexture>(desc, graphics, path, slot);
}

std::string dx3d::BTexture::GenerateUID(const std::string& path, unsigned int slot) {
	using namespace std::string_literals;
	return typeid(BTexture).name() + "#"s + path + "#" + std::to_string(slot);;
}

std::string dx3d::BTexture::GetUID() const noexcept {
	return GenerateUID(_path, _slot);
}

bool dx3d::BTexture::HasAlpha() const noexcept {
	return _has_alpha;
}
