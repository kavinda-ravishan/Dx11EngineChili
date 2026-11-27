#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <string>
#include <DX3D/Core/Base.hpp>
#include <DX3D/Graphics/GraphicsResource.hpp>
#include <DX3D/Core/Common.hpp>

#define BINDABLE_DESC BindableDesc({ GetLogger() })

namespace dx3d {
	class Bindable : public ClonableBase, public GraphicsResource {
	public:
		explicit Bindable(const BindableDesc& desc);
		virtual ~Bindable() = default;

		virtual void InitializeParentReference(const Drawable&) noexcept;
		virtual void Accept(class TechniqueProbe&);
		virtual std::string GetUID() const noexcept;
		virtual void Bind(Graphics& graphics) noexcept = 0;
	};

	class CloningBindable : public Bindable {
	public:
		explicit CloningBindable(const BindableDesc& desc);
		virtual ~CloningBindable() = default;

		virtual std::unique_ptr<CloningBindable> Clone() const noexcept = 0;
	};
} // namespace dx3d
