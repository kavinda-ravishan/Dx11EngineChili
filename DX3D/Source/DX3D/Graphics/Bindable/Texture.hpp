#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <string>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class BTexture : public Bindable {
	public:
		BTexture(const BindableDesc& desc, Graphics& graphics, const std::string& path, unsigned int slot = 0);
		void Bind(Graphics& graphics) noexcept override;
	
		static std::shared_ptr<BTexture> Resolve(const BindableDesc& desc, Graphics& graphics, const std::string& path, unsigned int slot = 0);
		static std::string GenerateUID(const std::string& path, unsigned int slot = 0);
		std::string GetUID() const noexcept override;
		bool HasAlpha() const noexcept;

	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture_view{};

	private:
		const std::string _path{};
		const unsigned int _slot{};
		bool _has_alpha{};
	};
} // namespace dx3d
