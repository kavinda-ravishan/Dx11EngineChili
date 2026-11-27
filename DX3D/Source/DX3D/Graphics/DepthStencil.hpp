#pragma once
#include <DX3D/Graphics/GraphicsResource.hpp>

namespace dx3d {
	class DepthStencil final : public Base, public GraphicsResource {
		friend class RenderTarget;
		friend class Graphics;

	public:
		DepthStencil(const BaseDesc& desc, Graphics& graphics, UINT width, UINT height);
		
		void BindAsDepthStencil(Graphics& graphics) const noexcept;
		void Clear(Graphics& graphics) const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depth_stencil_view{};
	};
} // namespace dx3d
