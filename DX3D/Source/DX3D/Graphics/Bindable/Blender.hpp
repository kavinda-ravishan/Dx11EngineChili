#pragma once

#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <array>
#include <optional>
#include <DX3D/Core/ConditionalNoexcept.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class BBlender final : public Bindable {
	public:
		BBlender(const BindableDesc& desc, Graphics& graphics, bool blending, std::optional<float> factor = {});
		
		void Bind(Graphics& graphics) noexcept override;
		void SetFactor(float factor) noxnd;
		float GetFactor() const noxnd;
		static std::shared_ptr<BBlender> Resolve(const BindableDesc& desc, Graphics& graphics, bool blending, std::optional<float> factor = {});
		static std::string GenerateUID(bool blending, std::optional<float> factor);
		std::string GetUID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> _blender{};
		const bool _blending{};
		std::optional<std::array<float, 4>> _factors{};
	};
} // namespace dx3d
