#pragma once
#include <vector>
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <string>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d {
	class BIndexBuffer : public Bindable {
	public:
		BIndexBuffer(const BindableDesc& desc, Graphics& graphics, const std::vector<unsigned short>& indices);
		BIndexBuffer(const BindableDesc& desc, Graphics& graphics, const std::string& tag, const std::vector<unsigned short>& indices);
		void Bind(Graphics& graphics) noexcept override;
		UINT GetCount() const noexcept;
	
		static std::shared_ptr<BIndexBuffer> Resolve(const BindableDesc& desc, Graphics& graphics, const std::string tag, const std::vector<unsigned short>& indices);

		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore) {
			return GenerateUID_(tag);
		}

		std::string GetUID() const noexcept override;

	private:
		static std::string GenerateUID_(const std::string& tag);

	protected:
		const std::string _tag{};
		const UINT _count{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> _index_buffer{};
	};
} // namespace dx3d
