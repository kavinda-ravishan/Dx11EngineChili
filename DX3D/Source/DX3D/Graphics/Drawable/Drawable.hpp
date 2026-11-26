#pragma once
#include <vector>
#include <DX3D/Core/WinSwitches.hpp>
#include <Windows.h>
#include <DirectXMath.h>
#include <DX3D/Core/ConditionalNoexcept.hpp>
#include <DX3D/Core/Base.hpp>
#include <DX3D/Core/Core.hpp>
#include <DX3D/Graphics/RenderJobs/Technique.hpp>

#define DRAWABLE_DESC DrawableDesc({ GetLogger() })

namespace dx3d {

	class Drawable : public Base {
	public:
		Drawable(const DrawableDesc& desc);
		Drawable(const DrawableDesc& desc, Graphics& graphics, const class Material& mat, const struct aiMesh& mesh, const float scale = 1.0f) noexcept;
		virtual ~Drawable() = default;
		
		void AddTechnique(Technique technique) noexcept;
		virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
		void Submit(class FrameCommander& frame) const noexcept;
		void Bind(Graphics& graphics) const noexcept;
		void Accept(class TechniqueProbe& probe);
		UINT GetIndexCount() const noxnd;
	
	protected:
		std::shared_ptr<class BIndexBuffer> _indices{};
		std::shared_ptr<class BVertexBuffer> _vertices{};
		std::shared_ptr<class BTopology> _topology{};
		std::vector<Technique> _techniques{};
	};
} // namespace dx3d
