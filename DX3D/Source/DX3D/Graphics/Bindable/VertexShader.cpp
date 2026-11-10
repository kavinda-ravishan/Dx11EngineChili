#include <DX3D/Core/WinSwitches.hpp>
#include <d3dcompiler.h>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/VertexShader.hpp>

dx3d::BVertexShader::BVertexShader(const BindableDesc& desc, Graphics& graphics, const std::string& path)
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

void dx3d::BVertexShader::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->VSSetShader(_vertex_shader.Get(), nullptr, 0u);
}

ID3DBlob* dx3d::BVertexShader::GetBytecode() const noexcept {
	return _bytecode_blob.Get();
}

std::shared_ptr<dx3d::BVertexShader> dx3d::BVertexShader::Resolve(const BindableDesc& desc, Graphics& graphics, const std::string& path) {
	return Codex::Resolve<BVertexShader>(desc, graphics, path);
}

std::string dx3d::BVertexShader::GenerateUID(const std::string& path) {
	using namespace std::string_literals;
	return typeid(BVertexShader).name() + "#"s + path;
}

std::string dx3d::BVertexShader::GetUID() const noexcept {
	return GenerateUID(_path);
}
