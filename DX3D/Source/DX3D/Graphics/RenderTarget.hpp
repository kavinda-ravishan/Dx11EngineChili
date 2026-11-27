#pragma once
#include <DX3D/Graphics/GraphicsResource.hpp>

namespace dx3d {

	class DepthStencil;

	class RenderTarget final : public Base, public GraphicsResource {
	public:
		RenderTarget(const BaseDesc& desc, Graphics& graphics, UINT width, UINT height);
		void BindAsTexture(Graphics& graphics, UINT slot) const noexcept;
		void BindAsTarget(Graphics& graphics) const noexcept;
		void BindAsTarget(Graphics& graphics, const DepthStencil& depth_stencil) const noexcept;
	
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture_view{};
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _target_view{};
	};
} // namespace dx3d
