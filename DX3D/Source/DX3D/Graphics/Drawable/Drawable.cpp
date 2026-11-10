#include <cassert>
#include <DX3D/Core/ConditionalNoexcept.hpp>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Bindable/IndexBuffer.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

dx3d::Drawable::Drawable(const DrawableDesc& desc) : Base(desc.base) {}

void dx3d::Drawable::Draw(Graphics& graphics) const noxnd {
	for (auto& bindable : _bindables) {
		bindable->Bind(graphics);
	}

	graphics.DrawIndexed(_index_buffer_ptr->GetCount());
}

void dx3d::Drawable::AddBind(std::shared_ptr<Bindable> bindable) noxnd {

	// special case for index buffer
	if (typeid(*bindable) == typeid(BIndexBuffer)) {
		assert("Binding multiple index buffers not allowed" && _index_buffer_ptr == nullptr);
		_index_buffer_ptr = &static_cast<BIndexBuffer&>(*bindable);
	}

	_bindables.push_back(std::move(bindable));
}
