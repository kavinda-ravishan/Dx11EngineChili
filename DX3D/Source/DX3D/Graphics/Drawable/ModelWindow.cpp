#include <DX3D/Graphics/Drawable/Node.hpp>
#include <DX3D/Graphics/Drawable/ModelWindow.hpp>

void dx3d::ModelWindow::Show(Graphics& graphics, const char* window_name, const Node& root) noexcept {
	//// window name defaults to "Model"
	//window_name = window_name ? window_name : "Model";
	//// need an ints to track node indices and selected node
	//int nodeIndexTracker = 0;
	//if (ImGui::Begin(window_name)) {
	//	ImGui::Columns(2, nullptr, true);
	//	root.ShowTree(_selected_node_ptr);

	//	ImGui::NextColumn();
	//	if (_selected_node_ptr != nullptr) {
	//		const auto id = _selected_node_ptr->GetID();
	//		auto i = _transforms.find(id);
	//		if (i == _transforms.end()) {
	//			const auto& applied = _selected_node_ptr->GetAppliedTransform();
	//			const auto angles = ExtractEulerAngles(applied);
	//			const auto translation = ExtractTranslation(applied);
	//			TransformParameters tp;
	//			tp.roll = angles.z;
	//			tp.pitch = angles.x;
	//			tp.yaw = angles.y;
	//			tp.x = translation.x;
	//			tp.y = translation.y;
	//			tp.z = translation.z;
	//			auto pMatConst = _selected_node_ptr->GetMaterialConstants();
	//			auto buf = pMatConst != nullptr ? std::optional<dx3d_dcb::Buffer>{ *pMatConst } : std::optional<dx3d_dcb::Buffer>{};
	//			std::tie(i, std::ignore) = _transforms.insert({ id,{ tp,false,std::move(buf),false } });
	//		}

	//		// link imgui ctrl to our cached transform params
	//		{
	//			auto& transform = i->second.tranform_params;
	//			// dirty check
	//			auto& dirty = i->second.transform_params_changed;
	//			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
	//			// widgets
	//			ImGui::Text("Orientation");
	//			dcheck(ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f));
	//			dcheck(ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f));
	//			dcheck(ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f));
	//			ImGui::Text("Position");
	//			dcheck(ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f));
	//			dcheck(ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f));
	//			dcheck(ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f));
	//		}

	//		// link imgui ctrl to our cached material params
	//		if (i->second.material_cbuf) {
	//			auto& mat = *i->second.material_cbuf;
	//			// dirty check
	//			auto& dirty = i->second.material_cbuf_changed;
	//			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
	//			// widgets
	//			ImGui::Text("Material");
	//			if (auto v = mat[PSCBV_normal_map_enabled]; v.Exists()) {
	//				dcheck(ImGui::Checkbox("Norm Map", &v));
	//			}

	//			if (auto v = mat[PSCBV_specular_map_enabled]; v.Exists()) {
	//				dcheck(ImGui::Checkbox("Spec Map", &v));
	//			}

	//			if (auto v = mat[PSCBV_has_gloss_map]; v.Exists()) {
	//				dcheck(ImGui::Checkbox("Gloss Map", &v));
	//			}

	//			if (auto v = mat[PSCBV_material_color]; v.Exists()) {
	//				dcheck(ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
	//			}

	//			if (auto v = mat[PSCBV_specular_power]; v.Exists()) {
	//				dcheck(ImGui::SliderFloat("Spec Power", &v, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
	//			}

	//			if (auto v = mat[PSCBV_specular_color]; v.Exists()) {
	//				dcheck(ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
	//			}

	//			if (auto v = mat[PSCBV_specular_map_weight]; v.Exists()) {
	//				dcheck(ImGui::SliderFloat("Spec Weight", &v, 0.0f, 4.0f));
	//			}

	//			if (auto v = mat[PSCBV_specular_intensity]; v.Exists()) {
	//				dcheck(ImGui::SliderFloat("Spec Intens", &v, 0.0f, 1.0f));
	//			}
	//		}
	//	}
	//}
	//ImGui::End();
}

void dx3d::ModelWindow::ApplyParameters() noexcept(!IS_DEBUG) {
	//if (TransformChanged()) {
	//	_selected_node_ptr->SetAppliedTransform(GetTransform());
	//	ResetTransformChanged();
	//}

	//if (MaterialChanged()) {
	//	_selected_node_ptr->SetMaterialConstants(GetMaterial());
	//	ResetMaterialChanged();
	//}
}

DirectX::XMMATRIX dx3d::ModelWindow::GetTransform() const noexcept(!IS_DEBUG) {
	return DirectX::XMMatrixIdentity();
}

bool dx3d::ModelWindow::TransformChanged() const noexcept(!IS_DEBUG) {
	//return _selected_node_ptr && _transforms.at(_selected_node_ptr->GetID()).transform_params_changed;
	return false;
}

void dx3d::ModelWindow::ResetTransformChanged() noexcept(!IS_DEBUG) {
	//_transforms.at(_selected_node_ptr->GetID()).transform_params_changed = false;
}

bool dx3d::ModelWindow::MaterialChanged() const noexcept(!IS_DEBUG) {
	//return _selected_node_ptr && _transforms.at(_selected_node_ptr->GetID()).material_cbuf_changed;
	return false;
}

void dx3d::ModelWindow::ResetMaterialChanged() noexcept(!IS_DEBUG) {
	//_transforms.at(_selected_node_ptr->GetID()).material_cbuf_changed = false;
}

bool dx3d::ModelWindow::IsChanged() const noexcept(!IS_DEBUG) {
	return TransformChanged() || MaterialChanged();
}
