#pragma once

#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>

#include <optional>
#include <array>

#include <DX3D/Math/Math.hpp>
#include <DX3D/Graphics/Drawable/Vertex.hpp>
#include <DX3D/Graphics/Drawable/IndexedTriangleList.hpp>

namespace dx3d {

	class Cube
	{
	public:
		static IndexedTriangleList MakeIndependent(VertexLayout layout)
		{
			constexpr float side = 1.0f / 2.0f;

			VertexBuffer vertices(std::move(layout), 24u);
			vertices[0].Attr<VertexLayout::ElementType::Position3D>() = { -side,-side,-side };// 0 near side
			vertices[1].Attr<VertexLayout::ElementType::Position3D>() = { side,-side,-side };// 1
			vertices[2].Attr<VertexLayout::ElementType::Position3D>() = { -side,side,-side };// 2
			vertices[3].Attr<VertexLayout::ElementType::Position3D>() = { side,side,-side };// 3
			vertices[4].Attr<VertexLayout::ElementType::Position3D>() = { -side,-side,side };// 4 far side
			vertices[5].Attr<VertexLayout::ElementType::Position3D>() = { side,-side,side };// 5
			vertices[6].Attr<VertexLayout::ElementType::Position3D>() = { -side,side,side };// 6
			vertices[7].Attr<VertexLayout::ElementType::Position3D>() = { side,side,side };// 7
			vertices[8].Attr<VertexLayout::ElementType::Position3D>() = { -side,-side,-side };// 8 left side
			vertices[9].Attr<VertexLayout::ElementType::Position3D>() = { -side,side,-side };// 9
			vertices[10].Attr<VertexLayout::ElementType::Position3D>() = { -side,-side,side };// 10
			vertices[11].Attr<VertexLayout::ElementType::Position3D>() = { -side,side,side };// 11
			vertices[12].Attr<VertexLayout::ElementType::Position3D>() = { side,-side,-side };// 12 right side
			vertices[13].Attr<VertexLayout::ElementType::Position3D>() = { side,side,-side };// 13
			vertices[14].Attr<VertexLayout::ElementType::Position3D>() = { side,-side,side };// 14
			vertices[15].Attr<VertexLayout::ElementType::Position3D>() = { side,side,side };// 15
			vertices[16].Attr<VertexLayout::ElementType::Position3D>() = { -side,-side,-side };// 16 bottom side
			vertices[17].Attr<VertexLayout::ElementType::Position3D>() = { side,-side,-side };// 17
			vertices[18].Attr<VertexLayout::ElementType::Position3D>() = { -side,-side,side };// 18
			vertices[19].Attr<VertexLayout::ElementType::Position3D>() = { side,-side,side };// 19
			vertices[20].Attr<VertexLayout::ElementType::Position3D>() = { -side,side,-side };// 20 top side
			vertices[21].Attr<VertexLayout::ElementType::Position3D>() = { side,side,-side };// 21
			vertices[22].Attr<VertexLayout::ElementType::Position3D>() = { -side,side,side };// 22
			vertices[23].Attr<VertexLayout::ElementType::Position3D>() = { side,side,side };// 23

			return{
				std::move(vertices),{
					0,2, 1,    2,3,1,
					4,5, 7,    4,7,6,
					8,10, 9,  10,11,9,
					12,13,15, 12,15,14,
					16,17,18, 18,17,19,
					20,23,21, 20,22,23
				}
			};
		}
		static IndexedTriangleList MakeIndependentTextured()
		{
			auto itl = MakeIndependent(std::move(VertexLayout{}
				.Append(VertexLayout::ElementType::Position3D)
				.Append(VertexLayout::ElementType::Normal)
				.Append(VertexLayout::ElementType::Texture2D)
			));

			itl._vertices[0].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._vertices[1].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._vertices[2].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._vertices[3].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._vertices[4].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._vertices[5].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._vertices[6].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._vertices[7].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._vertices[8].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._vertices[9].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._vertices[10].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._vertices[11].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._vertices[12].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._vertices[13].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._vertices[14].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._vertices[15].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._vertices[16].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._vertices[17].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._vertices[18].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._vertices[19].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,1.0f };
			itl._vertices[20].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,0.0f };
			itl._vertices[21].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,0.0f };
			itl._vertices[22].Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f,1.0f };
			itl._vertices[23].Attr<VertexLayout::ElementType::Texture2D>() = { 1.0f,1.0f };

			return itl;
		}
	};
} // namespace dx3d
