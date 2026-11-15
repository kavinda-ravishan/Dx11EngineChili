#pragma once
#include <string>
#include <filesystem>
#include <DX3D/Core/Base.hpp>

namespace dx3d {
	class SurfaceSTD final : public Base {
	public:
		class Color {
		public:
			uint32_t _dword;
		public:
			constexpr Color() : _dword() {}
			constexpr Color(const Color& col)
				:
				_dword(col._dword)
			{
			}
			constexpr Color(uint32_t dw)
				:
				_dword(dw)
			{
			}
		};
		
		SurfaceSTD(const BaseDesc& desc, const std::filesystem::path image_path);
		~SurfaceSTD();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		unsigned int GetChannels() const;
		SurfaceSTD& RGBA2BGRA() noexcept;

		Color* GetBufferPtr() const noexcept;
		const Color* GetBufferPtrConst() const noexcept;

		bool AlphaLoaded() const noexcept;

		void Save(const std::string& path) const;

	private:
		unsigned char* _data{};
		int _width{};
		int _height{};
		int _channels{};
		bool _alpha_loaded{};
	};
} // namespace dx3d
