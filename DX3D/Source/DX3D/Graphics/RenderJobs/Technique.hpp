#pragma once
#include <vector>
#include <string>
#include <DX3D/Graphics/RenderJobs/Step.hpp>
#include <DX3D/Graphics/RenderJobs/TechniqueProbe.hpp>

namespace dx3d {
	class Technique {
	public:
		Technique() = default;
		Technique(std::string name) noexcept
			:
			_name(name)
		{}

		void Submit(class FrameCommander& frame, const class Drawable& drawable) const noexcept;
		
		void AddStep(Step step) noexcept {
			_steps.push_back(std::move(step));
		}

		bool IsActive() const noexcept {
			return _active;
		}

		void SetActiveState(bool active) noexcept {
			_active = active;
		}

		void InitializeParentReferences(const class Drawable& parent) noexcept;
		
		void Accept(TechniqueProbe& probe) {
			probe.SetTechnique(this);
			for (auto& s : _steps)
			{
				s.Accept(probe);
			}
		}

		const std::string& GetName() const noexcept {
			return _name;
		}

	private:
		bool _active{ true };
		std::vector<Step> _steps{};
		std::string _name{ "Nameless Tech" };
	};
} // namespace dx3d
