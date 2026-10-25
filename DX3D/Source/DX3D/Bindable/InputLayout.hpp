#pragma once
#include <vector>
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Bindable/Bindable.hpp>

namespace dx3d {

	class InputLayout : public Bindable {
	public:
		InputLayout(const BindableDesc&                          desc,
			        Graphics&                                    graphics,
			        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
			        ID3DBlob*                                    vertex_shader_bytecode);
		void Bind(Graphics& graphics) noexcept override;
	
	protected:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> _input_layout;
	};

} // namespace dx3d
