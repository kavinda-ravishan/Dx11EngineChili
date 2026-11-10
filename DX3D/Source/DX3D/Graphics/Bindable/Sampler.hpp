#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class BSampler : public Bindable {
	public:
		BSampler(const BindableDesc& desc, Graphics& graphics);
		void Bind(Graphics& graphics) noexcept override;

		static std::shared_ptr<BSampler> Resolve(const BindableDesc& desc, Graphics& graphics);
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler{};
	};
} // namespace dx3d
