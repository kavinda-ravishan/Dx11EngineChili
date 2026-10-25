#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Bindable/InputLayout.hpp>

dx3d::InputLayout::InputLayout(const BindableDesc&                          desc,
							   Graphics&                                    graphics,
							   const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
							   ID3DBlob*                                    vertex_shader_bytecode)
	: Bindable(desc)
{
	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateInputLayout(
			layout.data(),
			(UINT)layout.size(),
			vertex_shader_bytecode->GetBufferPointer(),
			vertex_shader_bytecode->GetBufferSize(),
			&_input_layout),
		"Failed to create VS input layout"
	);

}

void dx3d::InputLayout::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->IASetInputLayout(_input_layout.Get());
}
