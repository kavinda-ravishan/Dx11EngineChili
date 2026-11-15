#pragma once
#include <array>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class BRasterizer : public Bindable {
	public:
		BRasterizer(const BindableDesc& desc, Graphics& graphics, bool two_sided);
		void Bind(Graphics& graphics) noexcept override;
		static std::shared_ptr<BRasterizer> Resolve(const BindableDesc& desc, Graphics& graphics, bool two_sided);
		static std::string GenerateUID(bool two_sided);
		std::string GetUID() const noexcept override;
	
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizer{};
		const bool _two_sided{};
	};
} // namespace dx3d
