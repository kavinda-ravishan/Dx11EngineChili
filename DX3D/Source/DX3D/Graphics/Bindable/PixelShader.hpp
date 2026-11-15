#pragma once
#include <memory>
#include <string>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {

	class BPixelShader final : public Bindable {
	public:
		BPixelShader(const BindableDesc& desc, Graphics& graphics, const std::string& path);
		void Bind(Graphics& graphics) noexcept override;

		static std::shared_ptr<BPixelShader> Resolve(const BindableDesc& desc, Graphics& graphics, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;

	protected:
		const std::string _path{};
		Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixel_shader{};
	};

} // namespace dx3d
