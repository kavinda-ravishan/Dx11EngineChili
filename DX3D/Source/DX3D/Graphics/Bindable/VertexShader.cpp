#include <DX3D/Core/WinSwitches.hpp>
#include <d3dcompiler.h>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/VertexShader.hpp>

dx3d::VertexShader::VertexShader(const BindableDesc& desc, Graphics& graphics, const std::string& path) 
    : Bindable(desc), _path(path)
{
	DX3DGraphicsLogThrowOnFail(
		D3DReadFileToBlob(std::wstring(path.begin(), path.end()).c_str(), &_bytecode_blob),
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

std::shared_ptr<dx3d::VertexShader> dx3d::VertexShader::Resolve(const BindableDesc& desc, Graphics& graphics, const std::string& path) {
	return Codex::Resolve<dx3d::VertexShader>(desc, graphics, path);
}

std::string dx3d::VertexShader::GenerateUID(const std::string& path) {
	using namespace std::string_literals;
	return typeid(VertexShader).name() + "#"s + path;
}

std::string dx3d::VertexShader::GetUID() const noexcept {
	return GenerateUID(_path);
}
