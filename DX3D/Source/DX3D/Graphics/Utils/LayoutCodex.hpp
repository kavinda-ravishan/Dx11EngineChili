#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <DX3D/Graphics/Utils/DynamicConstant.hpp>

namespace dx3d_dcb
{
	class LayoutCodex
	{
	public:
		static CookedLayout Resolve(RawLayout&& layout) noxnd;
	private:
		static LayoutCodex& Get_() noexcept;
		std::unordered_map<std::string, std::shared_ptr<LayoutElement>> map;
	};
} // namespace dx3d