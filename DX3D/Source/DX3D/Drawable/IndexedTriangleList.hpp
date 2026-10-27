#pragma once
#include <vector>
#include <DirectXMath.h>

namespace dx3d {
	template<class T>
	class IndexedTriangleList
	{
	public:
		IndexedTriangleList() = default;
		IndexedTriangleList(std::vector<T> vertices, std::vector<unsigned short> indices)
			:
			_vertices(std::move(vertices)),
			_indices(std::move(indices))
		{
			assert(_vertices.size() > 2);
			assert(_indices.size() % 3 == 0);
		}
		void Transform(DirectX::FXMMATRIX matrix)
		{
			for (auto& v : _vertices)
			{
				const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
				DirectX::XMStoreFloat3(
					&v.pos,
					DirectX::XMVector3Transform(pos, matrix)
				);
			}
		}

	public:
		std::vector<T> _vertices;
		std::vector<unsigned short> _indices;
	};
} // namespace dx3d
