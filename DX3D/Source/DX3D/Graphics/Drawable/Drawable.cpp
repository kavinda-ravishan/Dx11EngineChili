#include <cassert>
#include <DX3D/Core/ConditionalNoexcept.hpp>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

dx3d::Drawable::Drawable(const DrawableDesc& desc) : Base(desc.base) {}

void dx3d::Drawable::AddTechnique(Technique technique) noexcept {
	technique.InitializeParentReferences(*this);
	_techniques.push_back(std::move(technique));
}

DirectX::XMMATRIX dx3d::Drawable::GetTransformXM() const noexcept
{
	return DirectX::XMMATRIX();
}

void dx3d::Drawable::Submit(FrameCommander& frame) const noexcept {
	for (const auto& technique : _techniques) {
		technique.Submit(frame, *this);
	}
}

void dx3d::Drawable::Bind(Graphics& graphics) const noexcept {
	_topology->Bind(graphics);
	_indices->Bind(graphics);
	_vertices->Bind(graphics);
}

void dx3d::Drawable::Accept(TechniqueProbe& probe) {
	for (auto& technique : _techniques) {
		technique.Accept(probe);
	}
}

UINT dx3d::Drawable::GetIndexCount() const noxnd {
	return _indices->GetCount();
}
