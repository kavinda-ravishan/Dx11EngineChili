#include <string>
#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Drawable/Models/Cube.hpp>
#include <DX3D/Graphics/Drawable/TestCube.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

dx3d::TestCube::TestCube(const DrawableDesc& desc, Graphics& graphics, float size)
	: Drawable(desc)
{
	auto model = Cube::MakeIndependentTextured();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string(size);
	_vertices = BVertexBuffer::Resolve(BINDABLE_DESC, graphics, geometryTag, model._vertices);
	_indices = BIndexBuffer::Resolve(BINDABLE_DESC, graphics, geometryTag, model._indices);
	_topology = BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique standard;
		{
			Step only(0);

			only.AddBindable(BTexture::Resolve(BINDABLE_DESC, graphics, "Assets\\Images\\brickwall.jpg"));
			only.AddBindable(BSampler::Resolve(BINDABLE_DESC, graphics));

			auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();
			only.AddBindable(std::move(pvs));

			only.AddBindable(BPixelShader::Resolve(BINDABLE_DESC, graphics, "PhongPS.cso"));

			only.AddBindable(BPixelConstantBuffer<PSMaterialConstant>::Resolve(BINDABLE_DESC, graphics, _pmc, 1u));

			only.AddBindable(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

			only.AddBindable(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics));

			standard.AddStep(std::move(only));
		}
		AddTechnique(std::move(standard));
	}

	{
		Technique outline;
		{
			Step mask(1);

			auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable(std::move(pvs));

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

			mask.AddBindable(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(mask));
		}
		{
			Step draw(2);

			auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable(std::move(pvs));

			// this can be pass-constant
			draw.AddBindable(BPixelShader::Resolve(BINDABLE_DESC, graphics, "SolidPS.cso"));

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

			// quick and dirty... nicer solution maybe takes a lamba... we'll see :)
			class TransformCbufScaling : public BTransformCbuf
			{
			public:
				using BTransformCbuf::BTransformCbuf;
				void Bind(Graphics& gfx) noexcept override
				{
					const auto scale = DirectX::XMMatrixScaling(1.04f, 1.04f, 1.04f);
					auto xf = GetTransforms(gfx);
					xf.model_view = xf.model_view * scale;
					xf.model_view_proj = xf.model_view_proj * scale;
					UpdateBindImpl(gfx, xf);
				}
			};
			draw.AddBindable(std::make_shared<TransformCbufScaling>(BINDABLE_DESC, graphics));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(draw));
		}
		AddTechnique(std::move(outline));
	}
}

void dx3d::TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	_pos = pos;
}

void dx3d::TestCube::SetRotation(float roll, float pitch, float yaw) noexcept {
	_roll = roll;
	_pitch = pitch;
	_yaw = yaw;
}

DirectX::XMMATRIX dx3d::TestCube::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationRollPitchYaw(_roll, _pitch, _yaw) *
		DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}

void dx3d::TestCube::SpawnControlWindow(Graphics& graphics, const char* name) noexcept {
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &_pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &_pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &_pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &_yaw, -180.0f, 180.0f);
		//ImGui::Text("Shading");
		//bool changed0 = ImGui::SliderFloat("Spec. Int.", &_pmc.specular_intensity, 0.0f, 1.0f);
		//bool changed1 = ImGui::SliderFloat("Spec. Power", &_pmc.specular_power, 0.0f, 100.0f);
		//bool checkState = _pmc.normal_mapping_enabled == TRUE;
		//bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		//_pmc.normal_mapping_enabled = checkState ? TRUE : FALSE;
		//if (changed0 || changed1 || changed2)
		//{
		//	QueryBindable<BPixelConstantBuffer<PSMaterialConstant>>()->Update(graphics, _pmc);
		//}
	}
	ImGui::End();
}
