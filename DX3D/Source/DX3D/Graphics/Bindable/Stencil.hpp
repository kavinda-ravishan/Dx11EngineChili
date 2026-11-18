#pragma once
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Bindable/BindableCodex.hpp>

namespace dx3d {
	class BStencil final : public Bindable {
	public:
		enum class Mode {
			Off,
			Write,
			Mask
		};

		BStencil(const BindableDesc& desc, Graphics& graphics, Mode mode)
			: Bindable(desc), _mode(mode)
		{
			D3D11_DEPTH_STENCIL_DESC ds_desc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

			if (mode == Mode::Write) {
				ds_desc.DepthEnable = FALSE;
				ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				ds_desc.StencilEnable = TRUE;
				ds_desc.StencilWriteMask = 0xFF;
				ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			} else if (mode == Mode::Mask) {
				ds_desc.DepthEnable = FALSE;
				ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				ds_desc.StencilEnable = TRUE;
				ds_desc.StencilReadMask = 0xFF;
				ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			}

			GetDevice(graphics)->CreateDepthStencilState(&ds_desc, &_stencil);
		}

		void Bind(Graphics& graphics) noexcept override {
			GetContext(graphics)->OMSetDepthStencilState(_stencil.Get(), 0xFF);
		}

		static std::shared_ptr<BStencil> Resolve(const BindableDesc& desc, Graphics& graphics, Mode mode) {
			return Codex::Resolve<BStencil>(desc, graphics, mode);
		}

		static std::string GenerateUID(Mode mode) {
			using namespace std::string_literals;
			const auto modeName = [mode]() {
				switch (mode) {
				case Mode::Off:
					return "off"s;
				case Mode::Write:
					return "write"s;
				case Mode::Mask:
					return "mask"s;
				}
				return "ERROR"s;
				};
			return typeid(BStencil).name() + "#"s + modeName();
		}

		std::string GetUID() const noexcept override {
			return GenerateUID(_mode);
		}

	private:
		const Mode _mode{};
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _stencil;
	};
} // namespace dx3d
