#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/RenderJobs/Job.hpp>
#include <DX3D/Graphics/RenderJobs/Step.hpp>
#include <DX3D/Graphics/RenderJobs/FrameCommander.hpp>
#include <DX3D/Graphics/RenderJobs/TechniqueProbe.hpp>

dx3d::Step::Step(size_t target_pass)
	: _target_pass{ target_pass }
{}

void dx3d::Step::AddBindable(std::shared_ptr<Bindable> bind_in) noexcept {
	_bindables.push_back(std::move(bind_in));
}

void dx3d::Step::Submit(FrameCommander& frame, const Drawable& drawable) const {
	frame.Accept(Job{ this,&drawable }, _target_pass);
}

void dx3d::Step::Bind(Graphics& graphics) const {
	for (const auto& b : _bindables) {
		b->Bind(graphics);
	}
}

void dx3d::Step::InitializeParentReferences(const Drawable& parent) noexcept {
	for (auto& b : _bindables)
	{
		b->InitializeParentReference(parent);
	}
}

void dx3d::Step::Accept(TechniqueProbe& probe) {
	probe.SetStep(this);
	for (auto& pb : _bindables) {
		pb->Accept(probe);
	}
}
