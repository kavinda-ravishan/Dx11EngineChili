#pragma once

#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <array>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class BBlender final : public Bindable {
	public:
		BBlender(const BindableDesc& desc, Graphics& graphics, bool blending);
		
		void Bind(Graphics& graphics) noexcept override;
		static std::shared_ptr<BBlender> Resolve(const BindableDesc& desc, Graphics& graphics, bool blending);
		static std::string GenerateUID(bool blending);
		std::string GetUID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> _blender{};
		const bool _blending{};
	};
} // namespace dx3d
