#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/InputLayout.hpp>

dx3d::BInputLayout::BInputLayout(const BindableDesc& desc,
							     Graphics&           graphics,
	                             const VertexLayout  layout,
							     ID3DBlob*           vertex_shader_bytecode)
	: Bindable(desc), _layout(layout)
{

	const auto d3d_layout = layout.GetD3DLayout();

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateInputLayout(
			d3d_layout.data(),
			(UINT)d3d_layout.size(),
			vertex_shader_bytecode->GetBufferPointer(),
			vertex_shader_bytecode->GetBufferSize(),
			&_input_layout),
		"Failed to create VS input layout"
	);

}

void dx3d::BInputLayout::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->IASetInputLayout(_input_layout.Get());
}

std::shared_ptr<dx3d::BInputLayout> dx3d::BInputLayout::Resolve(const BindableDesc& desc, Graphics& graphics, const VertexLayout& layout, ID3DBlob* vertex_shader_bytecode) {
	return Codex::Resolve<BInputLayout>(desc, graphics, layout, vertex_shader_bytecode);
}

std::string dx3d::BInputLayout::GenerateUID(const VertexLayout& layout, ID3DBlob* vertex_shader_bytecode) {
	using namespace std::string_literals;
	return typeid(BInputLayout).name() + "#"s + layout.GetCode();
}

std::string dx3d::BInputLayout::GetUID() const noexcept {
	return GenerateUID(_layout);
}
