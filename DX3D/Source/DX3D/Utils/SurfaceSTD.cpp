#define STB_IMAGE_IMPLEMENTATION
#include <DX3D/Utils/STD_Image/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <DX3D/Utils/STD_Image/stb_image_write.h>

#include <stdexcept>
#include <DX3D/Utils/SurfaceSTD.hpp>

dx3d::SurfaceSTD::SurfaceSTD(const BaseDesc& desc, const std::filesystem::path image_path)
    : Base(desc)
{
    _data = stbi_load(
        image_path.string().c_str(),
        &_width,
        &_height,
        &_channels,
        STBI_rgb_alpha
    );

    if (!_data)
    {
        throw std::runtime_error(stbi_failure_reason());
    }

    _alpha_loaded = false;

    long total_pixels = _width * _height;

    for (long i = 0; i < total_pixels; ++i) {
        // Alpha is the last component of each pixel (index 1 for 2-channel, index 3 for 4-channel)
        unsigned char alpha = _data[(i * _channels) + (_channels - 1)];

        if (alpha < 255) {
            _alpha_loaded = true;
            break; // Found a transparent or semi-transparent pixel
        }
    }
}

dx3d::SurfaceSTD::~SurfaceSTD() {
    stbi_image_free(_data);
}

unsigned int dx3d::SurfaceSTD::GetWidth() const {
    return _width;
}

unsigned int dx3d::SurfaceSTD::GetHeight() const {
    return _height;
}

unsigned int dx3d::SurfaceSTD::GetChannels() const {
    return _channels;
}

dx3d::SurfaceSTD& dx3d::SurfaceSTD::RGBA2BGRA() noexcept {
    for (int i = 0; i < _width * _height; ++i)
    {
        uint8_t* p = _data + i * 4;

        // rgba -> bgra
        std::swap(p[0], p[2]);
    }

    return *this;
}

dx3d::SurfaceSTD::Color* dx3d::SurfaceSTD::GetBufferPtr() const noexcept {
    return reinterpret_cast<Color*>(_data);
}

const dx3d::SurfaceSTD::Color* dx3d::SurfaceSTD::GetBufferPtrConst() const noexcept {
    return reinterpret_cast<const Color*>(_data);
}

bool dx3d::SurfaceSTD::AlphaLoaded() const noexcept {
    return _alpha_loaded;
}

void dx3d::SurfaceSTD::Save(const std::string& path) const {
    stbi_write_png(path.c_str(), _width, _height, _channels, _data, _width * _channels);
}
