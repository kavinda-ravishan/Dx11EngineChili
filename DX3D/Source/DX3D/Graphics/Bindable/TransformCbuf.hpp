#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>
#include <DX3D/Core/Core.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	template<typename C>
	class BVertexConstantBuffer;

	class BTransformCbuf : public CloningBindable {
	protected:
		struct Transforms {
			DirectX::XMMATRIX model_view;
			DirectX::XMMATRIX model_view_proj;
		};

	public:
		BTransformCbuf(const BindableDesc& desc, Graphics& graphics, UINT slot=0u);
		virtual ~BTransformCbuf() = default;

		void Bind(Graphics& graphics) noexcept override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
	protected:
		void UpdateBindImpl(Graphics& graphics, const Transforms& tf) noexcept;
		Transforms GetTransforms(Graphics& graphics) noexcept;

	private:
		static std::unique_ptr<BVertexConstantBuffer<Transforms>> _static_vertex_constant_buffer;
		const Drawable* _parent_ptr{nullptr};
	};
} // namespace dx3d
