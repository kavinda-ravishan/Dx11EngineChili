#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Bindable/Bindable.hpp>

namespace dx3d {

	class PixelShader : public Bindable {
	public:
		PixelShader(const BindableDesc& desc, Graphics& graphics, const std::wstring& path);
		void Bind(Graphics& graphics) noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixel_shader;
	};

} // namespace dx3d
