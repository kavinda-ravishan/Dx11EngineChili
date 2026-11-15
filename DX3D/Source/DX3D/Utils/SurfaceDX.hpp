#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <dxtex/DirectXTex.h>
#include <string>
#include <filesystem>
#include <DX3D/Core/Base.hpp>

namespace dx3d {
	class SurfaceDX final : public Base {
	public:
		class Color
		{
		public:
			unsigned int dword;
		public:
			constexpr Color() noexcept : dword()
			{
			}
			constexpr Color(const Color& col) noexcept
				:
				dword(col.dword)
			{
			}
			constexpr Color(unsigned int dw) noexcept
				:
				dword(dw)
			{
			}
		};

		SurfaceDX(const BaseDesc& desc, const std::filesystem::path image_path);
		~SurfaceDX();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;

		Color* GetBufferPtr() noexcept;
		const Color* GetBufferPtr() const noexcept;
		const Color* GetBufferPtrConst() const noexcept;

		bool AlphaLoaded() const noexcept;

		void Save(const std::filesystem::path image_path) const;

	private:
		static constexpr DXGI_FORMAT _format{ DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM };
		DirectX::ScratchImage _scratch{};
	};
} // namespace dx3d
