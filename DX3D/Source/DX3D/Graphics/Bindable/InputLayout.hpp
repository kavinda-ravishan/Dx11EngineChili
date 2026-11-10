#pragma once
#include <vector>
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <memory>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>

namespace dx3d {

	class BInputLayout : public Bindable {
	public:
		BInputLayout(const BindableDesc& desc,
			         Graphics&           graphics,
			         const VertexLayout  layout,
			         ID3DBlob*           vertex_shader_bytecode);
		void Bind(Graphics& graphics) noexcept override;
	
		static std::shared_ptr<BInputLayout> Resolve(const BindableDesc& desc, Graphics& graphics, const VertexLayout& layout, ID3DBlob* vertex_shader_bytecode);
		static std::string GenerateUID(const VertexLayout& layout, ID3DBlob* vertex_shader_bytecode = nullptr);
		std::string GetUID() const noexcept override;

	protected:
		const VertexLayout _layout{};
		Microsoft::WRL::ComPtr<ID3D11InputLayout> _input_layout{};
	};

} // namespace dx3d
