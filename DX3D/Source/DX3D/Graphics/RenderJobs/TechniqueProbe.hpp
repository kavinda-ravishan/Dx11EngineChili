#pragma once
#include <limits>

namespace dx3d_dcb {
	class Buffer;
}

namespace dx3d {

	class TechniqueProbe {
	public:
		// TODO: add callback for visiting each mesh

		void SetTechnique(class Technique* tech_ptr) {
			_tech_ptr = tech_ptr;
			_tech_idx++;
			OnSetTechnique();
		}

		void SetStep(class Step* step_ptr) {
			_step_ptr = step_ptr;
			_step_idx++;
			OnSetStep();
		}

		bool VisitBuffer(class dx3d_dcb::Buffer& buf) {
			_buf_idx++;
			return OnVisitBuffer(buf);
		}

	protected:
		virtual void OnSetTechnique() {}
		virtual void OnSetStep() {}
		virtual bool OnVisitBuffer(class dx3d_dcb::Buffer&) {
			return false;
		}

	protected:
		class Technique* _tech_ptr{ nullptr };
		class Step* _step_ptr{ nullptr };
		size_t _tech_idx{ std::numeric_limits<size_t>::max() };
		size_t _step_idx{ std::numeric_limits<size_t>::max() };
		size_t _buf_idx{ std::numeric_limits<size_t>::max() };
	};
} // namespace dx3d
