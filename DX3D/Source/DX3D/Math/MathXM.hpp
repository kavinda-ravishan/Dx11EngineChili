#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>

namespace dx3d {
	DirectX::XMMATRIX ScaleTranslation(DirectX::XMMATRIX matrix, float scale);
	DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& mat);
	DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix);
} // namespace dx3d
