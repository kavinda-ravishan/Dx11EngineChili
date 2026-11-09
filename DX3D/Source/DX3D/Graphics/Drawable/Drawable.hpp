#pragma once
#include <vector>
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>
#include <DX3D/Core/Base.hpp>
#include <DX3D/Core/Core.hpp>

namespace dx3d {
	class Drawable : public Base {
	public:
		Drawable(const DrawableDesc& desc);
		virtual ~Drawable() = default;
		
		void Draw(Graphics& graphics) const noexcept(!IS_DEBUG);

		virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	
	protected:
		template<class T>
		T* QueryBindable() noexcept
		{
			for (auto& pb : _bindables)
			{
				if (auto pt = dynamic_cast<T*>(pb.get()))
				{
					return pt;
				}
			}
			return nullptr;
		}

	protected:
		void AddBind(std::shared_ptr<Bindable> bindable) noexcept(!IS_DEBUG);

	private:
		const class IndexBuffer* _index_buffer_ptr{ nullptr };
		std::vector<std::shared_ptr<Bindable>> _bindables{};
	};
} // namespace dx3d
