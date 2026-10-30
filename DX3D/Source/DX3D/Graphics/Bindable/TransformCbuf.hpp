#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>
#include <DX3D/Core/Core.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	template<typename C>
	class VertexConstantBuffer;

	class TransformCbuf : public Bindable {
	private:
		struct Transforms {
			DirectX::XMMATRIX model_view_proj;
			DirectX::XMMATRIX model;
		};

	public:
		TransformCbuf(const BindableDesc& desc, Graphics& graphics, const Drawable& parent);
		void Bind(Graphics& graphics) noexcept override;
	
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> _static_vertex_constant_buffer;
		const Drawable& _parent;
	};
} // namespace dx3d
