#pragma once
#include <vector>
#include <memory>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/RenderJobs/TechniqueProbe.hpp>

namespace dx3d {
	class Step {
	public:
		Step(size_t target_pass)
			:
			_target_pass{ target_pass }
		{}

		void AddBindable(std::shared_ptr<Bindable> bind_in) noexcept {
			_bindables.push_back(std::move(bind_in));
		}

		void Submit(class FrameCommander& frame, const class Drawable& drawable) const;
		
		void Bind(Graphics& graphics) const {
			for (const auto& b : _bindables) {
				b->Bind(graphics);
			}
		}

		void InitializeParentReferences(const class Drawable& parent) noexcept;
		
		void Accept(TechniqueProbe& probe) {
			probe.SetStep(this);
			for (auto& pb : _bindables) {
				pb->Accept(probe);
			}
		}

	private:
		const size_t _target_pass{};
		std::vector<std::shared_ptr<Bindable>> _bindables{};
	};
} // namespace dx3d
