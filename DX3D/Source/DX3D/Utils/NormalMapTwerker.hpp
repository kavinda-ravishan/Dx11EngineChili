#pragma once

#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>
#include <string>
#include <DX3D/Math/Math.hpp>
#include <DX3D/Utils/Surface.hpp>

namespace dx3d {
	class NormalMapTwerker {
	public:
		static void RotateXAxis180(const std::string& path_in, const std::string& path_out) {
			const auto rotation = DirectX::XMMatrixRotationX(PI);
			const auto s_in = Surface(path_in);

			const auto n_pixels = s_in.GetWidth() * s_in.GetHeight();
			const auto begin_ptr = s_in.GetBufferPtr();
			const auto end_ptr = s_in.GetBufferPtrConst() + n_pixels;
		
			for (auto current_ptr = begin_ptr; current_ptr < end_ptr; current_ptr++) {
				auto n = ColorToVector(*current_ptr);
				n = XMVector3Transform(n, rotation);
				*current_ptr = VectorToColor(n);
			}

			s_in.Save(path_out);
		}

		static void RotateXAxis180(const std::string& path_in) {
			return RotateXAxis180(path_in, path_in);
		}

	private:
		static DirectX::XMVECTOR ColorToVector(Surface::Color c) {
			auto n = DirectX::XMVectorSet((float)c.GetR(), (float)c.GetG(), (float)c.GetB(), 0.0f);
			const auto all255 = DirectX::XMVectorReplicate(2.0f / 255.0f);
			n = DirectX::XMVectorMultiply(n, all255);
			const auto all1 = DirectX::XMVectorReplicate(1.0f);
			n = DirectX::XMVectorSubtract(n, all1);
			return n;
		}

		static Surface::Color VectorToColor(DirectX::FXMVECTOR n) {
			const auto all1 = DirectX::XMVectorReplicate(1.0f);
			DirectX::XMVECTOR n_out = DirectX::XMVectorAdd(n, all1);
			
			const auto all255 = DirectX::XMVectorReplicate(255.0f / 2.0f);
			n_out = DirectX::XMVectorMultiply(n_out, all255);
			
			DirectX::XMFLOAT3 floats;
			DirectX::XMStoreFloat3(&floats, n_out);
			
			return {
				(unsigned char)round(floats.x),
				(unsigned char)round(floats.y),
				(unsigned char)round(floats.z),
			};
		}
	};
} // namespace dx3d
