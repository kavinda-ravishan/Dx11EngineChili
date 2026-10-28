#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class Sampler : public Bindable {
	public:
		Sampler(const BindableDesc& desc, Graphics& graphics);
		void Bind(Graphics& graphics) noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler{};
	};
} // namespace dx3d
