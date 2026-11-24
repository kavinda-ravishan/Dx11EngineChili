#pragma once
#include <vector>
#include <filesystem>
#include <DX3D/Graphics/Bindable/BindableCommon.hpp>

struct aiMaterial;
struct aiMesh;

namespace dx3d {
	class Technique;

	class Material {
	public:
		Material(const BindableDesc& desc, Graphics& graphics, const aiMaterial& material, const std::filesystem::path& path) noxnd;
		dx3d_dvtx::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
		std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
		std::shared_ptr<BVertexBuffer> MakeVertexBindable(const BindableDesc& desc, Graphics& graphics, const aiMesh& mesh) const noxnd;
		std::shared_ptr<BIndexBuffer> MakeIndexBindable(const BindableDesc& desc, Graphics& graphics, const aiMesh& mesh) const noxnd;
		std::vector<Technique> GetTechniques() const noexcept;
	
	private:
		std::string MakeMeshTag(const aiMesh& mesh) const noexcept;
	
	private:
		const std::string _model_path{};
		std::string _name{};
		dx3d_dvtx::VertexLayout _vtx_layout;
		std::vector<Technique> _techniques;
	};
} // namespace dx3d
