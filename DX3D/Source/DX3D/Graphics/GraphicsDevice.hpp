#pragma once

#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <d3d11.h>
#include <DX3D/Core/Base.hpp>

namespace dx3d {

	class GraphicsDevice final : public Base {
	public:
		explicit GraphicsDevice(const GraphicsDeviceDesc& desc);
		~GraphicsDevice();

		void EndFrame();
		void ClearBuffer(float red, float green, float blue) noexcept;
		void DrawTestCube(const float angle, const float mouse_pos_x, const float mouse_pos_y);

	private:
		const Rect _window_size{};

		Microsoft::WRL::ComPtr<ID3D11Device> _device{};
		Microsoft::WRL::ComPtr<IDXGISwapChain> _swap_chain{};
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context{};
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _target_view{};
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depth_stencil_view{};
	};

} // namespace dx3d
