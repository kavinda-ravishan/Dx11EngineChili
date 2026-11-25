#include <DX3D/Graphics/RenderJobs/TechniqueProbe.hpp>
#include <DX3D/Graphics/RenderJobs/Technique.hpp>

dx3d::Technique::Technique(std::string name, bool start_active) noexcept
	: _active(start_active), _name(name)
{}

void dx3d::Technique::Submit(FrameCommander& frame, const Drawable& drawable) const noexcept {
	if (_active) {
		for (const auto& step : _steps) {
			step.Submit(frame, drawable);
		}
	}
}

void dx3d::Technique::AddStep(Step step) noexcept {
	_steps.push_back(std::move(step));
}

bool dx3d::Technique::IsActive() const noexcept {
	return _active;
}

void dx3d::Technique::SetActiveState(bool active) noexcept {
	_active = active;
}

void dx3d::Technique::InitializeParentReferences(const Drawable& parent) noexcept {
	for (auto& s : _steps) {
		s.InitializeParentReferences(parent);
	}
}

void dx3d::Technique::Accept(TechniqueProbe& probe) {
	probe.SetTechnique(this);
	for (auto& s : _steps)
	{
		s.Accept(probe);
	}
}

const std::string& dx3d::Technique::GetName() const noexcept {
	return _name;
}
