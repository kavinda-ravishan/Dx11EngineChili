#pragma once
#include <DX3D/Drawable/Drawable.hpp>
#include <DX3D/Graphics/Bindable/IndexBuffer.hpp>

namespace dx3d {
	template<class T>
	class DrawableBase : public Drawable {
	public:
		DrawableBase(const DrawableDesc& desc) : Drawable(desc) {}
		virtual ~DrawableBase() = default;

	protected:
		static bool IsStaticInitialized() noexcept {
			return !_static_bindables.empty();
		}

		static void AddStaticBind(std::unique_ptr<Bindable> bindable) noexcept(!IS_DEBUG) {
			assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bindable) != typeid(IndexBuffer));
			_static_bindables.push_back(std::move(bindable));
		}

		void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> index_buffer) noexcept(!IS_DEBUG) {
			assert("Attempting to add index buffer a second time" && _index_buffer_ptr == nullptr);
			_index_buffer_ptr = index_buffer.get();
			_static_bindables.push_back(std::move(index_buffer));
		}

		void SetIndexFromStatic() noexcept(!IS_DEBUG) {
			assert("Attempting to add index buffer a second time" && _index_buffer_ptr == nullptr);
			for (const auto& static_bindable : _static_bindables) {
				if (const auto p = dynamic_cast<IndexBuffer*>(static_bindable.get())) {
					_index_buffer_ptr = p;
					return;
				}
			}
			assert("Failed to find index buffer in static binds" && _index_buffer_ptr != nullptr);
		}

	private:
		const std::vector<std::unique_ptr<Bindable>>& GetStaticBindables() const noexcept override {
			return _static_bindables;
		}

	private:
		static std::vector<std::unique_ptr<Bindable>> _static_bindables;
	};

	template<class T>
	std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::_static_bindables{};
} // namespace dx3d
