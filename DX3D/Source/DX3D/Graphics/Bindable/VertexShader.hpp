#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {

	class VertexShader : public Bindable {
	public:
		VertexShader(const BindableDesc& desc, Graphics& graphics, const std::wstring& path);
		void Bind(Graphics& graphics) noexcept override;
		ID3DBlob* GetBytecode() const noexcept;
	
	protected:
		Microsoft::WRL::ComPtr<ID3DBlob> _bytecode_blob{};
		Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertex_shader{};
	};

} // namespace dx3d
