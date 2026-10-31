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
			alignas(16) DirectX::XMFLOAT3 pos;
			alignas(16) DirectX::XMFLOAT3 ambient;
			alignas(16) DirectX::XMFLOAT3 diffuse_color;
			alignas(16) DirectX::XMFLOAT3 material_color;
			float diffuse_intensity;
			float att_const;
			float att_lin;
			float att_quad;
		};
	private:
		PointLightCBuf _cb_data{};
		mutable SolidSphere _mesh;
		mutable PixelConstantBuffer<PointLightCBuf> _cbuf;
	};
} // namespace dx3d
