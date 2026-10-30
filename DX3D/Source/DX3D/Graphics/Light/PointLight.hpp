#pragma once
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Drawable/SolidSphere.hpp>
#include <DX3D/Graphics/Bindable/ConstantBuffers.hpp>

namespace dx3d {
	class PointLight final : public Base
	{
	public:
		PointLight(const BaseDesc& desc, const DrawableDesc& drawable_desc, const BindableDesc& bindable_desc, Graphics& graphics, float radius = 0.5f);
		void SpawnControlWindow() noexcept;
		void Reset() noexcept;
		void Draw(Graphics& graphics) const noexcept(!IS_DEBUG);
		void Bind(Graphics& graphics) const noexcept;
	private:
		struct PointLightCBuf
		{
			DirectX::XMFLOAT3 pos;
			float padding;
		};
	private:
		DirectX::XMFLOAT3 _pos{ 0.0f,0.0f,0.0f };;
		mutable SolidSphere _mesh;
		mutable PixelConstantBuffer<PointLightCBuf> _cbuf;
	};
} // namespace dx3d
