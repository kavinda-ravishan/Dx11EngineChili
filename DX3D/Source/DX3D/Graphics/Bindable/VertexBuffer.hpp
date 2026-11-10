#pragma once
#include <vector>
#include <wrl.h>
#include <string>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>

namespace dx3d {
	class BVertexBuffer : public Bindable {
	public:
		BVertexBuffer(const BindableDesc& desc, Graphics& graphics, const dx3d::VertexBuffer& vbuf);
		BVertexBuffer(const BindableDesc& desc, Graphics& graphics, const std::string tag, const dx3d::VertexBuffer& vbuf);
		void Bind(Graphics& graphics) noexcept override;
	
		static std::shared_ptr<BVertexBuffer> Resolve(const BindableDesc& desc, Graphics& graphics, const std::string tag, const VertexBuffer& vbuf);
		
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
		Microsoft::WRL::ComPtr<ID3D11Buffer> _vertex_buffer{};
	};
} // namespace dx3d
