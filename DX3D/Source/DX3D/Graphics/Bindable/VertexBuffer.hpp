#pragma once
#include <vector>
#include <wrl.h>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>

namespace dx3d {
	class VertexBufferB : public Bindable {
	public:
		VertexBufferB(const BindableDesc& desc, Graphics& graphics, const dx3d::VertexBuffer& vbuf);
		void Bind(Graphics& graphics) noexcept override;
	
	protected:
		const UINT _stride{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer{};
	};
} // namespace dx3d
