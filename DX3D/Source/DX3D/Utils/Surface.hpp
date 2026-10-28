#pragma once

#include <memory>
#include <string>
#include <DX3D/Utils/Colors.hpp>

namespace cv {
	class Mat;
}

namespace dx3d {

	class Surface {
	public:
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
