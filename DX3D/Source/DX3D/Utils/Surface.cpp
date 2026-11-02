#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <DX3D/Utils/Surface.hpp>

dx3d::Surface::Surface(const std::string image_path) {
    _data = stbi_load(
        image_path.c_str(),
        &_width,
        &_height,
        &_channels,
        STBI_rgb_alpha
    );

    if (!_data)
    {
        throw std::runtime_error(stbi_failure_reason());
    }
}

dx3d::Surface::~Surface() {
    stbi_image_free(_data);
}

dx3d::Surface::Surface(Surface&& source) noexcept
    :
    _data(_data),
    _width(source._width),
    _height(source._height),
    _channels(source._channels)
{}

dx3d::Surface& dx3d::Surface::operator=(Surface&& donor) noexcept {
    _width = donor._width;
    _height = donor._height;
    _channels = donor._channels;

    _data = donor._data;
    donor._data = nullptr;
    return *this;
}

unsigned int dx3d::Surface::GetWidth() const {
    return _width;
}

unsigned int dx3d::Surface::GetHeight() const {
    return _height;
}

unsigned int dx3d::Surface::GetChannels() const {
    return _channels;
}

void* dx3d::Surface::GetBufferPtr() const noexcept {
    return reinterpret_cast<void*>(_data);
}
