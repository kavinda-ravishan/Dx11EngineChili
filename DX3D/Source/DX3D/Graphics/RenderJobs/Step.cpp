#include <DX3D/Graphics/RenderJobs/Job.hpp>
#include <DX3D/Graphics/RenderJobs/FrameCommander.hpp>
#include <DX3D/Graphics/RenderJobs/Step.hpp>

void dx3d::Step::Submit(FrameCommander& frame, const Drawable& drawable) const {
	frame.Accept(Job{ this,&drawable }, _target_pass);
}

void dx3d::Step::InitializeParentReferences(const Drawable& parent) noexcept {
	for (auto& b : _bindables)
	{
		b->InitializeParentReference(parent);
	}
}
