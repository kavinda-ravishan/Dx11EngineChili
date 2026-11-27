#pragma once
#include <DX3D/Graphics/Graphics.hpp>

namespace dx3d {
	class GraphicsResource {
	protected:
		static ID3D11DeviceContext* GetContext(Graphics& graphics) noexcept;
		static ID3D11Device* GetDevice(Graphics& graphics) noexcept;
	};
} // namespace dx3d
