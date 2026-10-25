#pragma once
#include <vector>
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <DX3D/Bindable/Bindable.hpp>

namespace dx3d {
	class IndexBuffer : public Bindable {
	public:
		IndexBuffer(const BindableDesc& desc, Graphics& gfx, const std::vector<unsigned short>& indices);
		void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;
	protected:
		const UINT _count{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> _index_buffer{};
	};
} // namespace dx3d
