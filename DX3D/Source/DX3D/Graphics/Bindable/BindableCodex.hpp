#pragma once

#include <memory>
#include <unordered_map>

#include <DX3D/Core/ConditionalNoexcept.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>

namespace dx3d
{
	class Codex {
	public:
		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve(const BindableDesc& desc, Graphics& graphics, Params&&...p) noxnd {
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(desc, graphics, std::forward<Params>(p)...);
		}

	private:
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(const BindableDesc& desc, Graphics& graphics, Params&&...p) noxnd {
			const auto key = T::GenerateUID(std::forward<Params>(p)...);
			const auto i = _bindable.find(key);
			if (i == _bindable.end())
			{
				auto bindable = std::make_shared<T>(desc, graphics, std::forward<Params>(p)...);
				_bindable[key] = bindable;
				return bindable;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
			}
		}

		static Codex& Get() {
			static Codex codex;
			return codex;
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> _bindable{};
	};
} // namespace dx3d
