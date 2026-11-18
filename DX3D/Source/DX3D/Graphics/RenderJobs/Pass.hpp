#pragma once
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/RenderJobs/Job.hpp>
#include <vector>

namespace dx3d {
	class Pass {
	public:
		void Accept(Job job) noexcept {
			_jobs.push_back(job);
		}

		void Execute(Graphics& graphics) const noxnd {
			for (const auto& j : _jobs) {
				j.Execute(graphics);
			}
		}

		void Reset() noexcept {
			_jobs.clear();
		}

	private:
		std::vector<Job> _jobs{};
	};
} // namespace dx3d
