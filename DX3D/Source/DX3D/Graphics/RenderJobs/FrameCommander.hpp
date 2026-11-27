#pragma once
#include <array>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/DepthStencil.hpp>
#include <DX3D/Graphics/RenderTarget.hpp>
#include <DX3D/Graphics/RenderJobs/Pass.hpp>
#include <DX3D/Graphics/RenderJobs/Job.hpp>

namespace dx3d {

	class FrameCommander : public Base {
	public:
		FrameCommander(const BaseDesc& desc, Graphics& graphics)
			: 
			Base(desc), 
			_ds(desc, graphics, graphics.GetWidth(), graphics.GetHeight()), 
			_rt(desc, graphics, graphics.GetWidth(), graphics.GetHeight())
		{
			// setup fullscreen geometry
			dx3d_dvtx::VertexLayout lay;
			lay.Append(dx3d_dvtx::VertexLayout::Position2D);
			dx3d_dvtx::VertexBuffer buf_full{ lay };
			buf_full.EmplaceBack(DirectX::XMFLOAT2{ -1,1 });
			buf_full.EmplaceBack(DirectX::XMFLOAT2{ 1,1 });
			buf_full.EmplaceBack(DirectX::XMFLOAT2{ -1,-1 });
			buf_full.EmplaceBack(DirectX::XMFLOAT2{ 1,-1 });
			_vb_full_ptr = BVertexBuffer::Resolve(BINDABLE_DESC, graphics, "$Full", std::move(buf_full));
			std::vector<unsigned short> indices { 0,1,2,1,3,2 };
			_ib_full_ptr = BIndexBuffer::Resolve(BINDABLE_DESC, graphics, "$Full", std::move(indices));

			// setup fullscreen shaders
			_vs_full_ptr = BVertexShader::Resolve(BINDABLE_DESC, graphics, "FullScreen_VS.cso");
			_ps_full_ptr = BPixelShader::Resolve(BINDABLE_DESC, graphics, "FullScreenEffect_PS.cso");
			_layout_full_ptr = BInputLayout::Resolve(BINDABLE_DESC, graphics, lay, _vs_full_ptr->GetBytecode());
		}

		void Accept(Job job, size_t target) noexcept {
			_passes[target].Accept(job);
		}

		void Execute(const BindableDesc& desc, Graphics& graphics) const noxnd {
			// normally this would be a loop with each pass defining it setup / etc.
			// and later on it would be a complex graph with parallel execution contingent
			// on input / output requirements

			// setup render target used for normal passes
			_ds.Clear(graphics);
			// NOTE : to fix trying to bind a texture as a render target while that same resource is still bound as an input resource issue
			_rt.UnbindAsTexture(graphics, 0u);
			_rt.BindAsTarget(graphics, _ds);

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

			// fullscreen funky pass
			graphics.BindSwapBuffer();
			_rt.BindAsTexture(graphics, 0u);
			_vb_full_ptr->Bind(graphics);
			_ib_full_ptr->Bind(graphics);
			_vs_full_ptr->Bind(graphics);
			_ps_full_ptr->Bind(graphics);
			_layout_full_ptr->Bind(graphics);
			graphics.DrawIndexed(_ib_full_ptr->GetCount());
		}

		void Reset() noexcept {
			for (auto& p : _passes) {
				p.Reset();
			}
		}

	private:
		std::array<Pass, 3> _passes{};
		DepthStencil _ds;
		RenderTarget _rt;
		std::shared_ptr<BVertexBuffer> _vb_full_ptr{};
		std::shared_ptr<BIndexBuffer> _ib_full_ptr{};
		std::shared_ptr<BVertexShader> _vs_full_ptr{};
		std::shared_ptr<BPixelShader> _ps_full_ptr{};
		std::shared_ptr<BInputLayout> _layout_full_ptr{};
	};
} // namespace dx3d
