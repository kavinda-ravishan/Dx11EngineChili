#include <DX3D/Graphics/Drawable/Drawable.hpp>
#include <DX3D/Graphics/RenderJobs/Job.hpp>
#include <DX3D/Graphics/RenderJobs/Step.hpp>

dx3d::Job::Job(const Step* step_ptr, const Drawable* drawable_ptr)
	: _step_ptr(step_ptr), _drawable_ptr(drawable_ptr)
{}

void dx3d::Job::Execute(Graphics& graphics) const noxnd {
	_drawable_ptr->Bind(graphics);
	_step_ptr->Bind(graphics);
	graphics.DrawIndexed(_drawable_ptr->GetIndexCount());
}
