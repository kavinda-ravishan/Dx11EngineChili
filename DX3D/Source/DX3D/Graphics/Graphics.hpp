#pragma once

#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class Graphics final : public Base {
		friend class Bindable;

	public:
		explicit Graphics(const GraphicsDesc& desc);
		~Graphics();

		void EndFrame();
		void ClearBuffer(float red, float green, float blue) noexcept;
		void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
		void SetProjection(DirectX::FXMMATRIX proj) noexcept;
		DirectX::XMMATRIX GetProjection() const noexcept;
	private:
		const Rect _window_size{};
		DirectX::XMMATRIX _projection{};
		Microsoft::WRL::ComPtr<ID3D11Device> _device{};
		Microsoft::WRL::ComPtr<IDXGISwapChain> _swap_chain{};
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context{};
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _target_view{};
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depth_stencil_view{};
	};

} // namespace dx3d
