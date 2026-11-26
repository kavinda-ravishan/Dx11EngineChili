#include <cassert>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>
#include <assimp/scene.h>
#include <DX3D/Graphics/Drawable/Material.hpp>

dx3d::Drawable::Drawable(const DrawableDesc& desc) : Base(desc.base) {}

dx3d::Drawable::Drawable(const DrawableDesc& desc, Graphics& graphics, const Material& mat, const aiMesh& mesh, const float scale) noexcept
	: Base(desc.base)
{
	_vertices = mat.MakeVertexBindable(BindableDesc(desc.base), graphics, mesh, scale);
	_indices = mat.MakeIndexBindable(BindableDesc(desc.base), graphics, mesh);
	_topology = BTopology::Resolve(BindableDesc(desc.base), graphics);

	for (auto& t : mat.GetTechniques())
	{
		AddTechnique(std::move(t));
	}
}

void dx3d::Drawable::AddTechnique(Technique technique) noexcept {
	technique.InitializeParentReferences(*this);
	_techniques.push_back(std::move(technique));
}

DirectX::XMMATRIX dx3d::Drawable::GetTransformXM() const noexcept {
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
