#pragma once
#include <vector>
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>
#include <DX3D/Core/Base.hpp>
#include <DX3D/Core/Core.hpp>

namespace dx3d {
	class Drawable : public Base {
		template<class T>
		friend class DrawableBase;
	
	public:
		Drawable(const DrawableDesc& desc);
		virtual ~Drawable() = default;
		
		void Draw(Graphics& graphics) const noexcept(!IS_DEBUG);

		virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
		virtual void Update(const float dt) noexcept = 0;
	
	protected:
		void AddBind(std::unique_ptr<Bindable> bindable) noexcept(!IS_DEBUG);
		void AddIndexBuffer(std::unique_ptr<class IndexBuffer> index_buffer) noexcept(!IS_DEBUG);

	private:
		virtual const std::vector<std::unique_ptr<Bindable>>&  GetStaticBindables() const noexcept = 0;

	private:
		const class IndexBuffer* _index_buffer_ptr{ nullptr };
		std::vector<std::unique_ptr<Bindable>> _bindables{};
	};
} // namespace dx3d
