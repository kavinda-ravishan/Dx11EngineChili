#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <d3d11.h>
#include <DX3D/Core/Base.hpp>
#include <DX3D/Core/Common.hpp>

namespace dx3d {
	class Bindable : public Base {
	public:
		explicit Bindable(const BindableDesc& desc);
		virtual ~Bindable() = default;

		virtual void Bind(Graphics& graphics) noexcept = 0;

	protected:
		static ID3D11DeviceContext* GetContext(Graphics& graphics) noexcept;
		static ID3D11Device* GetDevice(Graphics& graphics) noexcept;
	};
} // namespace dx3d
