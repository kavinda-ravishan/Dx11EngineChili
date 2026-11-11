#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <DirectXMath.h>
#include <vector>
#include <DX3D/Graphics/Drawable/Vertex.hpp>

namespace dx3d {
	class IndexedTriangleList
	{
	public:
		IndexedTriangleList(VertexBuffer vertices, std::vector<unsigned short> indices)
			:
			_vertices(std::move(vertices)),
			_indices(std::move(indices))
		{
			assert(_vertices.Size() > 2);
			assert(_indices.size() % 3 == 0);
		}
		
		void Transform(DirectX::FXMMATRIX matrix)
		{
			using Elements = VertexLayout::ElementType;
			for (int i = 0; i < _vertices.Size(); i++)
			{
				auto& pos = _vertices[i].Attr<Elements::Position3D>();
				DirectX::XMStoreFloat3(
					&pos,
					DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix)
				);
			}
		}
		
		// asserts face-independent vertices w/ normals cleared to zero
		void SetNormalsIndependentFlat() noxnd
		{
			using namespace DirectX;
			assert(_indices.size() % 3 == 0 && _indices.size() > 0);
			for (size_t i = 0; i < _indices.size(); i += 3)
			{
				auto v0 = _vertices[_indices[i]];
				auto v1 = _vertices[_indices[i + 1]];
				auto v2 = _vertices[_indices[i + 2]];
				const auto p0 = XMLoadFloat3(&v0.Attr<VertexLayout::ElementType::Position3D>());
				const auto p1 = XMLoadFloat3(&v1.Attr<VertexLayout::ElementType::Position3D>());
				const auto p2 = XMLoadFloat3(&v2.Attr<VertexLayout::ElementType::Position3D>());

				const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

				XMStoreFloat3(&v0.Attr<VertexLayout::ElementType::Normal>(), n);
				XMStoreFloat3(&v1.Attr<VertexLayout::ElementType::Normal>(), n);
				XMStoreFloat3(&v2.Attr<VertexLayout::ElementType::Normal>(), n);
			}
		}

	public:
		VertexBuffer _vertices;
		std::vector<unsigned short> _indices;
	};
} // namespace dx3d
