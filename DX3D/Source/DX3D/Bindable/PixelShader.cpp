#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <d3dcompiler.h>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Bindable/PixelShader.hpp>

dx3d::PixelShader::PixelShader(const BindableDesc& desc, Graphics& graphics, const std::wstring& path) : Bindable(desc)
{
	Microsoft::WRL::ComPtr<ID3DBlob> ps_blod{};
	DX3DGraphicsLogThrowOnFail(
		D3DReadFileToBlob(path.c_str(), &ps_blod),
		"Failed to read pixel shader binary"
	);

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreatePixelShader(ps_blod->GetBufferPointer(), ps_blod->GetBufferSize(), nullptr, &_pixel_shader),
		"Failed to create pixel shader"
	);

}

void dx3d::PixelShader::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->PSSetShader(_pixel_shader.Get(), nullptr, 0u);
}
