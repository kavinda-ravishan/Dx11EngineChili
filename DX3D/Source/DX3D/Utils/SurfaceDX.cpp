#include <stdexcept>
#include <DX3D/Utils/Utils.hpp>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Utils/SurfaceDX.hpp>

dx3d::SurfaceDX::SurfaceDX(const BaseDesc& desc, const std::filesystem::path image_path)
    : Base(desc)
{
    DirectX::ScratchImage scratch;
    DX3DGraphicsLogThrowOnFail(
        DirectX::LoadFromWICFile(ToWide(image_path.string()).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratch),
        "Failed to load image"
    );

    if (scratch.GetImage(0, 0, 0)->format != _format) {
        DirectX::ScratchImage converted;

        DX3DGraphicsLogThrowOnFail(
            DirectX::Convert(
                *scratch.GetImage(0, 0, 0),
                _format,
                DirectX::TEX_FILTER_DEFAULT,
                DirectX::TEX_THRESHOLD_DEFAULT,
                converted
            ),
            "Failed to convert image"
        );

        _scratch = std::move(converted);
    }

    _scratch = std::move(scratch);
}

dx3d::SurfaceDX::~SurfaceDX() {}

unsigned int dx3d::SurfaceDX::GetWidth() const {
    return (unsigned int)_scratch.GetMetadata().width;
}

unsigned int dx3d::SurfaceDX::GetHeight() const {
    return (unsigned int)_scratch.GetMetadata().height;
}

dx3d::SurfaceDX::Color* dx3d::SurfaceDX::GetBufferPtr() noexcept {
    return reinterpret_cast<Color*>(_scratch.GetPixels());
}

const dx3d::SurfaceDX::Color* dx3d::SurfaceDX::GetBufferPtr() const noexcept {
    return const_cast<SurfaceDX*>(this)->GetBufferPtr();
}

const dx3d::SurfaceDX::Color* dx3d::SurfaceDX::GetBufferPtrConst() const noexcept {
    return const_cast<SurfaceDX*>(this)->GetBufferPtr();
}

bool dx3d::SurfaceDX::AlphaLoaded() const noexcept {
    return !_scratch.IsAlphaAllOpaque();;
}

void dx3d::SurfaceDX::Save(const std::filesystem::path image_path) const {
    const auto GetCodecID = [](const std::filesystem::path& image_path) {
        const auto ext = image_path.extension().string();

        if (ext == ".png") {
            return DirectX::WIC_CODEC_PNG;
        }
        else if (ext == ".jpg") {
            return DirectX::WIC_CODEC_JPEG;
        }
        else if (ext == ".bmp") {
            return DirectX::WIC_CODEC_BMP;
        }

        throw std::runtime_error("Image format not supported");
        };

    DX3DGraphicsLogThrowOnFail(
        DirectX::SaveToWICFile(
            *_scratch.GetImage(0, 0, 0),
            DirectX::WIC_FLAGS_NONE,
            GetWICCodec(GetCodecID(image_path)),
            ToWide(image_path.string()).c_str()
        ),
        "Failed to save image"
    );
}

