#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <d3dcompiler.h>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/PixelShader.hpp>

dx3d::BPixelShader::BPixelShader(const BindableDesc& desc, Graphics& graphics, const std::string& path) 
	: Bindable(desc), _path(path)
{
	Microsoft::WRL::ComPtr<ID3DBlob> ps_blod{};
	DX3DGraphicsLogThrowOnFail(
		D3DReadFileToBlob(std::wstring(path.begin(), path.end()).c_str(), &ps_blod),
		"Failed to read pixel shader binary"
	);

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreatePixelShader(ps_blod->GetBufferPointer(), ps_blod->GetBufferSize(), nullptr, &_pixel_shader),
		"Failed to create pixel shader"
	);

}

void dx3d::BPixelShader::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->PSSetShader(_pixel_shader.Get(), nullptr, 0u);
}

std::shared_ptr<dx3d::BPixelShader> dx3d::BPixelShader::Resolve(const BindableDesc& desc, Graphics& graphics, const std::string& path) {
	return Codex::Resolve<BPixelShader>(desc, graphics, path);
}

std::string dx3d::BPixelShader::GenerateUID(const std::string& path) {
	using namespace std::string_literals;
	return typeid(BPixelShader).name() + "#"s + path;
}

std::string dx3d::BPixelShader::GetUID() const noexcept {
	return GenerateUID(_path);
}
