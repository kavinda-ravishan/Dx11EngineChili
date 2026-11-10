#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <memory>
#include <string>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {

	class VertexShader : public Bindable {
	public:
		VertexShader(const BindableDesc& desc, Graphics& graphics, const std::string& path);
		void Bind(Graphics& graphics) noexcept override;
		ID3DBlob* GetBytecode() const noexcept;
	
		static std::shared_ptr<VertexShader> Resolve(const BindableDesc& desc, Graphics& graphics, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept override;

	protected:
		const std::string _path{};
		Microsoft::WRL::ComPtr<ID3DBlob> _bytecode_blob{};
		Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertex_shader{};
	};

} // namespace dx3d
