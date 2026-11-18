#include <DX3D/Graphics/RenderJobs/Technique.hpp>

void dx3d::Technique::Submit(FrameCommander& frame, const Drawable& drawable) const noexcept {
	if (_active) {
		for (const auto& step : _steps) {
			step.Submit(frame, drawable);
		}
	}
}

void dx3d::Technique::InitializeParentReferences(const Drawable& parent) noexcept {
	for (auto& s : _steps) {
		s.InitializeParentReferences(parent);
	}
}
