#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class Texture : public Bindable {
	public:
		Texture(const BindableDesc& desc, Graphics& graphics, const class Surface& surface);
		void Bind(Graphics& graphics) noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture_view{};
	};
} // namespace dx3d
