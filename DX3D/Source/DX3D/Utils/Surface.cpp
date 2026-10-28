#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp> 
#include <opencv2/core/utils/logger.hpp>
#include <DX3D/Utils/Surface.hpp>

dx3d::Surface::Surface(const std::string image_path) {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

    _mat = std::make_unique<cv::Mat>(cv::imread(image_path, cv::IMREAD_UNCHANGED));

    if (nullptr == _mat.get() || _mat->empty()) {
        throw std::runtime_error("Could not load image : " + image_path);
    }

    _width = _mat->cols;
    _height = _mat->rows;

    // Check channel count
    if (_mat->channels() == 3) {
        // Convert RGB/BGR to BGRA/RGBA if it only has 3 channels
        cv::cvtColor(*_mat, *_mat, cv::COLOR_BGR2BGRA);
    }
}

dx3d::Surface::Surface(unsigned int width, unsigned int height)
    :
    _mat(std::make_unique<cv::Mat>(height, width, CV_8UC4)),
    _width(width),
    _height(height) {
}

dx3d::Surface::~Surface() {}

dx3d::Surface::Surface(Surface&& source) noexcept
    :
    _mat(std::move(source._mat)),
    _width(source._width),
    _height(source._height) {
}

dx3d::Surface& dx3d::Surface::operator=(Surface&& donor) noexcept {
    _width = donor._width;
    _height = donor._height;

    _mat = std::move(donor._mat);
    donor._mat = nullptr;
    return *this;
}

unsigned int dx3d::Surface::GetWidth() const {
    return _width;
}

unsigned int dx3d::Surface::GetHeight() const {
    return _height;
}

void* dx3d::Surface::GetBufferPtr() const noexcept {
    return reinterpret_cast<void*>(_mat->data);
}

void dx3d::Surface::Clear(const Color color) {
    // Interpret the canvas as 32-bit pixels (CV_8UC4 assumed)
    uint32_t* ptr = _mat->ptr<uint32_t>(0);

    // Total number of pixels
    size_t total = _mat->rows * _mat->cols;

    // Fill with the initial color (packed BGRA)
    std::fill(ptr, ptr + total, color.dword);
}

void dx3d::Surface::PutPixel(int x, int y, const Color color) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < (int)_width);
    assert(y < (int)_height);

    // Pointer to the pixel (BGRA order in OpenCV)
    uint32_t* ptr = _mat->ptr<uint32_t>(y);
    ptr[x] = color.dword;
}

dx3d::Surface::Color dx3d::Surface::GetPixel(int x, int y) const {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < (int)_width);
    assert(y < (int)_height);

    const uint32_t* ptr = _mat->ptr<uint32_t>(y);
    return ptr[x];
}

void dx3d::Surface::PutPixelAlpha(int x, int y, Color color) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < (int)_width);
    assert(y < (int)_height);

    // load source pixel
    const Color background_color = GetPixel(x, y);

    // blend channels
    const unsigned char rsltRed = (color.GetR() * color.GetA() + background_color.GetR() * (255u - color.GetA())) / 256u;
    const unsigned char rsltGreen = (color.GetG() * color.GetA() + background_color.GetG() * (255u - color.GetA())) / 256u;
    const unsigned char rsltBlue = (color.GetB() * color.GetA() + background_color.GetB() * (255u - color.GetA())) / 256u;

    // pack channels back into pixel and fire pixel onto surface
    PutPixel(x, y, { rsltRed,rsltGreen,rsltBlue });
}
