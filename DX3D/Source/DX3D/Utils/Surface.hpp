#pragma once

#include <string>
#include <filesystem>

namespace dx3d {
	class Surface {
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
			constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b)
				:
				_dword((x << 24u) | (r << 16u) | (g << 8u) | b)
			{
			}
			constexpr Color(unsigned char r, unsigned char g, unsigned char b)
				:
				_dword((r << 16u) | (g << 8u) | b)
			{
			}
			constexpr Color(Color col, unsigned char x)
				:
				Color((x << 24u) | col._dword)
			{
			}
			Color& operator =(Color color)
			{
				_dword = color._dword;
				return *this;
			}
			constexpr unsigned char GetX() const
			{
				return _dword >> 24u;
			}
			constexpr unsigned char GetA() const
			{
				return GetX();
			}
			constexpr unsigned char GetR() const
			{
				return (_dword >> 16u) & 0xFFu;
			}
			constexpr unsigned char GetG() const
			{
				return (_dword >> 8u) & 0xFFu;
			}
			constexpr unsigned char GetB() const
			{
				return _dword & 0xFFu;
			}
			void SetX(unsigned char x)
			{
				_dword = (_dword & 0xFFFFFFu) | (x << 24u);
			}
			void SetA(unsigned char a)
			{
				SetX(a);
			}
			void SetR(unsigned char r)
			{
				_dword = (_dword & 0xFF00FFFFu) | (r << 16u);
			}
			void SetG(unsigned char g)
			{
				_dword = (_dword & 0xFFFF00FFu) | (g << 8u);
			}
			void SetB(unsigned char b)
			{
				_dword = (_dword & 0xFFFFFF00u) | b;
			}
		};
		
		Surface(const std::filesystem::path image_path);
		~Surface();

		Surface(Surface&& source) noexcept;
		Surface(Surface&) = delete;
		Surface& operator=(Surface&& donor) noexcept;
		Surface& operator=(const Surface&) = delete;

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		unsigned int GetChannels() const;
		Surface& RGBA2BGRA() noexcept;

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
