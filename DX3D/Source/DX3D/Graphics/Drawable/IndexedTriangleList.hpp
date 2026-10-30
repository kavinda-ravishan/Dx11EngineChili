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
		
		// asserts face-independent vertices w/ normals cleared to zero
		void SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
		{
			using namespace DirectX;
			assert(_indices.size() % 3 == 0 && _indices.size() > 0);
			for (size_t i = 0; i < _indices.size(); i += 3)
			{
				auto& v0 = _vertices[_indices[i]];
				auto& v1 = _vertices[_indices[i + 1]];
				auto& v2 = _vertices[_indices[i + 2]];
				const auto p0 = XMLoadFloat3(&v0.pos);
				const auto p1 = XMLoadFloat3(&v1.pos);
				const auto p2 = XMLoadFloat3(&v2.pos);

				const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

				XMStoreFloat3(&v0.n, n);
				XMStoreFloat3(&v1.n, n);
				XMStoreFloat3(&v2.n, n);
			}
		}

	public:
		std::vector<T> _vertices;
		std::vector<unsigned short> _indices;
	};
} // namespace dx3d
