#pragma once
#include <vector>
#include <string>
#include <DX3D/Graphics/RenderJobs/Step.hpp>

namespace dx3d {
	class FrameCommander;
	class TechniqueProbe;

	class Technique {
	public:
		Technique() = default;
		Technique(std::string name, bool start_active = true) noexcept;

		void Submit(FrameCommander& frame, const class Drawable& drawable) const noexcept;
		void AddStep(Step step) noexcept;
		bool IsActive() const noexcept;
		void SetActiveState(bool active) noexcept;
		void InitializeParentReferences(const class Drawable& parent) noexcept;
		void Accept(TechniqueProbe& probe);
		const std::string& GetName() const noexcept;

	private:
		bool _active{ true };
		std::vector<Step> _steps{};
		std::string _name{ "Nameless Tech" };
	};
} // namespace dx3d
