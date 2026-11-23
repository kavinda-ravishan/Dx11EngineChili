#pragma once
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {

	class BTopology final : public Bindable {
	public:
		BTopology(const BindableDesc& desc, Graphics& graphics, const D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(Graphics& graphics) noexcept override;

		static std::shared_ptr<BTopology> Resolve(const BindableDesc& desc, Graphics& graphics, const D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::string GenerateUID(const D3D11_PRIMITIVE_TOPOLOGY type);
		std::string GetUID() const noexcept override;

	protected:
		const D3D11_PRIMITIVE_TOPOLOGY _type{};
	};

} // namespace dx3d
