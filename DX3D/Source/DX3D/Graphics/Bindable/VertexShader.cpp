#include <DX3D/Core/WinSwitches.hpp>
#include <d3dcompiler.h>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/VertexShader.hpp>

dx3d::VertexShader::VertexShader(const BindableDesc& desc, Graphics& graphics, const std::wstring& path) 
    : Bindable(desc)
{
	DX3DGraphicsLogThrowOnFail(
		D3DReadFileToBlob(path.c_str(), &_bytecode_blob),
		"Failed to read vertex shader binary"
	);

	DX3DGraphicsLogThrowOnFail(
		GetDevice(graphics)->CreateVertexShader(
			_bytecode_blob->GetBufferPointer(), 
			_bytecode_blob->GetBufferSize(), nullptr, 
			&_vertex_shader),
		"Failed to create vertex shader"
	);

}

void dx3d::VertexShader::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->VSSetShader(_vertex_shader.Get(), nullptr, 0u);
}

ID3DBlob* dx3d::VertexShader::GetBytecode() const noexcept {
	return _bytecode_blob.Get();
}
