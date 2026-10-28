#pragma once

#include <memory>
#include <string>

namespace cv {
	class Mat;
}

namespace dx3d {
	class Surface {
	public:
		class Color {
		public:
			uint32_t dword;
		public:
			constexpr Color() : dword() {}
			constexpr Color(const Color& col)
				:
				dword(col.dword)
			{
			}
			constexpr Color(uint32_t dw)
				:
				dword(dw)
			{
			}
			constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b)
				:
				dword((x << 24u) | (r << 16u) | (g << 8u) | b)
			{
			}
			constexpr Color(unsigned char r, unsigned char g, unsigned char b)
				:
				dword((r << 16u) | (g << 8u) | b)
			{
			}
			constexpr Color(Color col, unsigned char x)
				:
				Color((x << 24u) | col.dword)
			{
			}
			Color& operator =(Color color)
			{
				dword = color.dword;
				return *this;
			}
			constexpr unsigned char GetX() const
			{
				return dword >> 24u;
			}
			constexpr unsigned char GetA() const
			{
				return GetX();
			}
			constexpr unsigned char GetR() const
			{
				return (dword >> 16u) & 0xFFu;
			}
			constexpr unsigned char GetG() const
			{
				return (dword >> 8u) & 0xFFu;
			}
			constexpr unsigned char GetB() const
			{
				return dword & 0xFFu;
			}
			void SetX(unsigned char x)
			{
				dword = (dword & 0xFFFFFFu) | (x << 24u);
			}
			void SetA(unsigned char a)
			{
				SetX(a);
			}
			void SetR(unsigned char r)
			{
				dword = (dword & 0xFF00FFFFu) | (r << 16u);
			}
			void SetG(unsigned char g)
			{
				dword = (dword & 0xFFFF00FFu) | (g << 8u);
			}
			void SetB(unsigned char b)
			{
				dword = (dword & 0xFFFFFF00u) | b;
			}
		};
		
		Surface(const std::string image_path);
		Surface(unsigned int width, unsigned int height);
		~Surface();

		Surface(Surface&& source) noexcept;
		Surface(Surface&) = delete;
		Surface& operator=(Surface&& donor) noexcept;
		Surface& operator=(const Surface&) = delete;

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;

		void* GetBufferPtr() const noexcept;

		void Clear(const Color color);

		void PutPixel(int x, int y, const Color color);
		Color GetPixel(int x, int y) const;
		void PutPixelAlpha(int x, int y, Color color);
	private:
		unsigned int _width{};
		unsigned int _height{};
		std::unique_ptr<cv::Mat> _mat{};
	};
} // namespace dx3d
