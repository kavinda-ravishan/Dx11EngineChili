#pragma once
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d
{
	class BNullPixelShader : public Bindable {
	public:
		BNullPixelShader(const BindableDesc& desc, Graphics& graphics);
		void Bind(Graphics& graphics) noexcept override;
		static std::shared_ptr<BNullPixelShader> Resolve(const BindableDesc& desc, Graphics& graphics);
		static std::string GenerateUID();
		std::string GetUID() const noexcept override;
	};
} // namespace dx3d