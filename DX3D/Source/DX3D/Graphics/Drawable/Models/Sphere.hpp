#pragma once

#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>

#include <optional>

#include <DX3D/Math/Math.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>
#include <DX3D/Graphics/Drawable/IndexedTriangleList.hpp>

namespace dx3d {
	class Sphere
	{
	public:
		static IndexedTriangleList MakeTesselated(VertexLayout layout, int lat_div, int long_div)
		{
			assert(lat_div >= 3);
			assert(long_div >= 3);

			constexpr float radius = 1.0f;
			const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
			const float lattitude_angle = PI / lat_div;
			const float longitude_angle = 2.0f * PI / long_div;

			VertexBuffer vb{ std::move(layout) };
			for (int i_lat = 1; i_lat < lat_div; i_lat++)
			{
				const auto lat_base = DirectX::XMVector3Transform(
					base,
					DirectX::XMMatrixRotationX(lattitude_angle * i_lat)
				);
				for (int i_long = 0; i_long < long_div; i_long++)
				{
					DirectX::XMFLOAT3 calculated_pos;
					auto v = DirectX::XMVector3Transform(
						lat_base,
						DirectX::XMMatrixRotationZ(longitude_angle * i_long)
					);
					DirectX::XMStoreFloat3(&calculated_pos, v);
					vb.EmplaceBack(calculated_pos);
				}
			}

			// add the cap vertices
			const auto i_north_pole = (unsigned short)vb.Size();
			{
				DirectX::XMFLOAT3 northPos;
				DirectX::XMStoreFloat3(&northPos, base);
				vb.EmplaceBack(northPos);
			}
			const auto i_south_pole = (unsigned short)vb.Size();
			{
				DirectX::XMFLOAT3 southPos;
				DirectX::XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base));
				vb.EmplaceBack(southPos);
			}

			const auto calc_idx = [lat_div, long_div](unsigned short i_lat, unsigned short i_long)
				{ return i_lat * long_div + i_long; };
			std::vector<unsigned short> indices;
			for (unsigned short i_lat = 0; i_lat < lat_div - 2; i_lat++)
			{
				for (unsigned short i_long = 0; i_long < long_div - 1; i_long++)
				{
					indices.push_back(calc_idx(i_lat, i_long));
					indices.push_back(calc_idx(i_lat + 1, i_long));
					indices.push_back(calc_idx(i_lat, i_long + 1));
					indices.push_back(calc_idx(i_lat, i_long + 1));
					indices.push_back(calc_idx(i_lat + 1, i_long));
					indices.push_back(calc_idx(i_lat + 1, i_long + 1));
				}
				// wrap band
				indices.push_back(calc_idx(i_lat, long_div - 1));
				indices.push_back(calc_idx(i_lat + 1, long_div - 1));
				indices.push_back(calc_idx(i_lat, 0));
				indices.push_back(calc_idx(i_lat, 0));
				indices.push_back(calc_idx(i_lat + 1, long_div - 1));
				indices.push_back(calc_idx(i_lat + 1, 0));
			}

			// cap fans
			for (unsigned short i_long = 0; i_long < long_div - 1; i_long++)
			{
				// north
				indices.push_back(i_north_pole);
				indices.push_back(calc_idx(0, i_long));
				indices.push_back(calc_idx(0, i_long + 1));
				// south
				indices.push_back(calc_idx(lat_div - 2, i_long + 1));
				indices.push_back(calc_idx(lat_div - 2, i_long));
				indices.push_back(i_south_pole);
			}
			// wrap triangles
			// north
			indices.push_back(i_north_pole);
			indices.push_back(calc_idx(0, long_div - 1));
			indices.push_back(calc_idx(0, 0));
			// south
			indices.push_back(calc_idx(lat_div - 2, 0));
			indices.push_back(calc_idx(lat_div - 2, long_div - 1));
			indices.push_back(i_south_pole);

			return { std::move(vb),std::move(indices) };
		}
		
		static IndexedTriangleList Make(std::optional<VertexLayout> layout = std::nullopt)
		{
			using Element = VertexLayout::ElementType;
			if (!layout)
			{
				layout = VertexLayout{}.Append(Element::Position3D);
			}
			return MakeTesselated(std::move(*layout), 12, 24);
		}
	};
} // namespace dx3d
