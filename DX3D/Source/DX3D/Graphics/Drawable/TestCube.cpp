#include <string>
#include <DX3D/Window/ImGUI/imgui.h>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Utils/DynamicConstant.hpp>
#include <DX3D/Graphics/Drawable/Models/Cube.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Drawable/Constants.hpp>
#include <DX3D/Graphics/Drawable/TestCube.hpp>

dx3d::TestCube::TestCube(const DrawableDesc& desc, Graphics& graphics, float size)
	: Drawable(desc)
{
	auto model = Cube::MakeIndependentTextured();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const auto geometry_tag = "$cube." + std::to_string(size);
	_vertices = BVertexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._vertices);
	_indices = BIndexBuffer::Resolve(BINDABLE_DESC, graphics, geometry_tag, model._indices);
	_topology = BTopology::Resolve(BINDABLE_DESC, graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique shade("Shade");
		{
			Step only(0);

			only.AddBindable(BTexture::Resolve(BINDABLE_DESC, graphics, "Assets\\Images\\brickwall.jpg"));
			only.AddBindable(BSampler::Resolve(BINDABLE_DESC, graphics));

			auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "PhongVS.cso");
			auto pvsbc = pvs->GetBytecode();
			only.AddBindable(std::move(pvs));

			only.AddBindable(BPixelShader::Resolve(BINDABLE_DESC, graphics, "PhongPS.cso"));

			//only.AddBindable(BPixelConstantBuffer<PSMaterialConstant>::Resolve(BINDABLE_DESC, graphics, _pmc, 1u));
			dx3d_dcb::RawLayout lay;
			lay.Add<dx3d_dcb::Float>(PSCBV_specular_intensity);
			lay.Add<dx3d_dcb::Float>(PSCBV_specular_power);
			auto buf = dx3d_dcb::Buffer(std::move(lay));
			buf[PSCBV_specular_intensity] = 0.1f;
			buf[PSCBV_specular_power] = 20.0f;
			only.AddBindable(std::make_shared<BCachingPixelConstantBufferEX>(BINDABLE_DESC, graphics, buf, 1u));

			only.AddBindable(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

			only.AddBindable(std::make_shared<BTransformCbuf>(BINDABLE_DESC, graphics));

			shade.AddStep(std::move(only));
		}
		AddTechnique(std::move(shade));
	}

	{
		Technique outline("Outline");
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

			// these can be pass-constant (tricky due to layout issues)
			auto pvs = BVertexShader::Resolve(BINDABLE_DESC, graphics, "SolidVS.cso");
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable(std::move(pvs));

			// this can be pass-constant
			draw.AddBindable(BPixelShader::Resolve(BINDABLE_DESC, graphics, "SolidPS.cso"));

			dx3d_dcb::RawLayout lay;
			lay.Add<dx3d_dcb::Float4>(PSCBV_color);
			auto buf = dx3d_dcb::Buffer(std::move(lay));
			buf[PSCBV_color] = DirectX::XMFLOAT4{ 1.0f, 0.4f, 0.4f, 1.0f };
			draw.AddBindable(std::make_shared<BCachingPixelConstantBufferEX>(BINDABLE_DESC, graphics, buf, 1u));

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(BInputLayout::Resolve(BINDABLE_DESC, graphics, model._vertices.GetLayout(), pvsbc));

			// quick and dirty... nicer solution maybe takes a lamba... we'll see :)
			class TransformCbufScaling : public BTransformCbuf
			{
			public:
				TransformCbufScaling(const BindableDesc& desc, Graphics& graphics, float scale = 1.04)
					:
					BTransformCbuf(desc, graphics),
					_buf(MakeLayout())
				{
					_buf["scale"] = scale;
				}

				void Accept(TechniqueProbe& probe) override {
					probe.VisitBuffer(_buf);
				}

				void Bind(Graphics& graphics) noexcept override {
					const float scale = _buf["scale"];
					const auto scale_matrix = DirectX::XMMatrixScaling(scale, scale, scale);

					auto xf = GetTransforms(graphics);

					xf.model_view = xf.model_view * scale_matrix;
					xf.model_view_proj = xf.model_view_proj * scale_matrix;
					
					UpdateBindImpl(graphics, xf);
				}

			private:
				static dx3d_dcb::RawLayout MakeLayout() {
					dx3d_dcb::RawLayout layout;
					layout.Add<dx3d_dcb::Float>("scale");
					return layout;
				}

			private:
				dx3d_dcb::Buffer _buf;
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

		class Probe : public TechniqueProbe {
		public:
			void OnSetTechnique() override {
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, _tech_ptr->GetName().c_str());
				bool active = _tech_ptr->IsActive();
				ImGui::Checkbox(("Tech Active##"s + _tech_ptr->GetName()).c_str(), &active);
				_tech_ptr->SetActiveState(active);
			}
			
			bool OnVisitBuffer(dx3d_dcb::Buffer& buf) override {
				float changed = false;
				const auto dcheck = [&changed](bool updated) {changed = changed || updated; };

				if (auto v = buf["scale"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat("Scale", &v, 1.0f, 2.0f, "%.3f", ImGuiSliderFlags_Logarithmic));
				}
				if (auto v = buf[PSCBV_color]; v.Exists())
				{
					dcheck(ImGui::ColorPicker4(PSCBV_color, reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(v))));
				}
				if (auto v = buf[PSCBV_specular_intensity]; v.Exists())
				{
					dcheck(ImGui::SliderFloat("Spec. Intens.", &v, 0.0f, 1.0f));
				}
				if (auto v = buf[PSCBV_specular_power]; v.Exists())
				{
					dcheck(ImGui::SliderFloat("Glossiness", &v, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
				}
				return changed;
			}
		};

		static Probe probe;
		Accept(probe);
	}
	ImGui::End();
}
