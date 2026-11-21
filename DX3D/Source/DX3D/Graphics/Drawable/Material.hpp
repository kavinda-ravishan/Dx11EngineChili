#pragma once
#include <vector>
#include <filesystem>
#include <assimp/scene.h>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/RenderJobs/Technique.hpp>

namespace dx3d {

	class Material {
	public:
		Material(Graphics& gfx, const aiMaterial* pMaterial, const std::filesystem::path& path) noxnd
		{}

		dx3d_dvtx::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept {
			using Type = dx3d_dvtx::VertexLayout::ElementType;
			dx3d_dvtx::VertexBuffer buf{ vtxLayout };
			buf.Resize(mesh.mNumVertices);
			if (vtxLayout.Has<Type::Position3D>())
			{
				for (int i = 0; i < mesh.mNumVertices; i++)
				{
					buf[i].
				}
			}
		}

		std::vector<Technique> GetTechniques() const noexcept {
			return techniques;
		}

	private:
		dx3d_dvtx::VertexLayout vtxLayout;
		std::vector<Technique> techniques;
	};
} // namespace dx3d
