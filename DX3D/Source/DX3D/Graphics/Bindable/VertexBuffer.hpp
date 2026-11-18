#pragma once
#include <vector>
#include <string>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Utils/Vertex.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>

namespace dx3d {
	class BVertexBuffer final : public Bindable {
	public:
		BVertexBuffer(const BindableDesc& desc, Graphics& graphics, const dx3d_dvtx::VertexBuffer& vbuf);
		BVertexBuffer(const BindableDesc& desc, Graphics& graphics, const std::string tag, const dx3d_dvtx::VertexBuffer& vbuf);
		
		void Bind(Graphics& graphics) noexcept override;
		const dx3d_dvtx::VertexLayout& GetLayout() const noexcept;
		static std::shared_ptr<BVertexBuffer> Resolve(const BindableDesc& desc, Graphics& graphics, const std::string tag, const dx3d_dvtx::VertexBuffer& vbuf);
		
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore) {
			return GenerateUID_(tag);
		}

		std::string GetUID() const noexcept override;

	private:
		static std::string GenerateUID_(const std::string& tag);

	protected:
		const std::string _tag{};
		const UINT _stride{};
		const dx3d_dvtx::VertexLayout _layout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer{};
	};
} // namespace dx3d
