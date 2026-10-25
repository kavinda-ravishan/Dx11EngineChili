#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {

	class Topology : public Bindable {
	public:
		Topology(const BindableDesc& desc, Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(Graphics& graphics) noexcept override;

	protected:
		const D3D11_PRIMITIVE_TOPOLOGY _type{};
	};

} // namespace dx3d
