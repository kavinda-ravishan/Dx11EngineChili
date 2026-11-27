#pragma once
#include <array>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/DepthStencil.hpp>
#include <DX3D/Graphics/RenderJobs/Pass.hpp>
#include <DX3D/Graphics/RenderJobs/Job.hpp>

namespace dx3d {

	class FrameCommander : public Base {
	public:
		FrameCommander(const BaseDesc& desc, Graphics& graphics)
			: Base(desc), _ds(desc, graphics, graphics.GetWidth(), graphics.GetHeight())
		{}

		void Accept(Job job, size_t target) noexcept {
			_passes[target].Accept(job);
		}

		void Execute(const BindableDesc& desc, Graphics& graphics) const noxnd {
			// normally this would be a loop with each pass defining it setup / etc.
			// and later on it would be a complex graph with parallel execution contingent
			// on input / output requirements

			// setup render target used for all passes
			_ds.Clear(graphics);
			graphics.BindSwapBuffer(_ds);

			// main phong lighting pass
			BStencil::Resolve(desc, graphics, BStencil::Mode::Off)->Bind(graphics);
			_passes[0].Execute(graphics);
			// outline masking pass
			BStencil::Resolve(desc, graphics, BStencil::Mode::Write)->Bind(graphics);
			BNullPixelShader::Resolve(desc, graphics)->Bind(graphics);
			_passes[1].Execute(graphics);
			// outline drawing pass
			BStencil::Resolve(desc, graphics, BStencil::Mode::Mask)->Bind(graphics);
			_passes[2].Execute(graphics);
		}

		void Reset() noexcept {
			for (auto& p : _passes) {
				p.Reset();
			}
		}

	private:
		DepthStencil _ds;
		std::array<Pass, 3> _passes{};
	};
} // namespace dx3d
