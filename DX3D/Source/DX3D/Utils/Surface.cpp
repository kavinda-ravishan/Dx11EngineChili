#define STB_IMAGE_IMPLEMENTATION
#include <DX3D/Utils/stb_image.h>

#include <stdexcept>
#include <DX3D/Utils/Surface.hpp>

dx3d::Surface::Surface(const std::filesystem::path image_path) {
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
}

dx3d::Surface::~Surface() {
    stbi_image_free(_data);
}

dx3d::Surface::Surface(Surface&& source) noexcept
    :
    _data(source._data),
    _width(source._width),
    _height(source._height),
    _channels(source._channels)
{
    source._data = nullptr;
}

dx3d::Surface& dx3d::Surface::operator=(dx3d::Surface&& donor) noexcept
{
    if (this != &donor)
    {
        stbi_image_free(_data);

        _data = donor._data;
        _width = donor._width;
        _height = donor._height;
        _channels = donor._channels;

        donor._data = nullptr;
    }

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

dx3d::Surface& dx3d::Surface::RGBA2BGRA() noexcept {
    for (int i = 0; i < _width * _height; ++i)
    {
        uint8_t* p = _data + i * 4;

        // rgba -> bgra
        std::swap(p[0], p[2]);
    }

    return *this;
}

void* dx3d::Surface::GetBufferPtr() const noexcept {
    return reinterpret_cast<void*>(_data);
}
