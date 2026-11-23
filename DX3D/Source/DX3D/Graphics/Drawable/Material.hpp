#pragma once
#include <vector>
#include <filesystem>
#include <assimp/scene.h>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>
#include <DX3D/Graphics/RenderJobs/Technique.hpp>

namespace dx3d {
	class Material {
	public:
		Material(const BindableDesc& desc, Graphics& graphics, const aiMaterial& material, const std::filesystem::path& path) noxnd;
		dx3d_dvtx::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
		std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept
		{
			std::vector<unsigned short> indices;
			indices.reserve(mesh.mNumFaces * 3);
			for (unsigned int i = 0; i < mesh.mNumFaces; i++)
			{
				const auto& face = mesh.mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
			return indices;
		}
		std::shared_ptr<BVertexBuffer> MakeVertexBindable(const BindableDesc& desc, Graphics& graphics, const aiMesh& mesh) const noxnd
		{
			return BVertexBuffer::Resolve(desc, graphics, MakeMeshTag(mesh), ExtractVertices(mesh));
		}
		std::shared_ptr<BIndexBuffer> MakeIndexBindable(const BindableDesc& desc, Graphics& graphics, const aiMesh& mesh) const noxnd
		{
			return BIndexBuffer::Resolve(desc, graphics, MakeMeshTag(mesh), ExtractIndices(mesh));
		}
		std::vector<Technique> GetTechniques() const noexcept;
	private:
		std::string MakeMeshTag(const aiMesh& mesh) const noexcept
		{
			return _model_path + "%" + mesh.mName.C_Str();
		}
	private:
		const std::string _model_path{};
		std::string _name{};
		dx3d_dvtx::VertexLayout _vtx_layout;
		std::vector<Technique> _techniques;
	};
} // namespace dx3d
