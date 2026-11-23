#pragma once
#include <memory>
#include <vector>
#include <DX3D/Core/Core.hpp>

namespace dx3d {
	class Step {
	public:
		Step(size_t target_pass);

		Step(Step&&) = default;

		Step(const Step& src) noexcept;

		Step& operator=(const Step&) = delete;
		Step& operator=(Step&&) = delete;

		void AddBindable(std::shared_ptr<Bindable> bind_in) noexcept;

		void Submit(class FrameCommander& frame, const class Drawable& drawable) const;
		
		void Bind(Graphics& graphics) const;

		void InitializeParentReferences(const class Drawable& parent) noexcept;
		
		void Accept(class TechniqueProbe& probe);

	private:
		const size_t _target_pass{};
		std::vector<std::shared_ptr<Bindable>> _bindables{};
	};
} // namespace dx3d
