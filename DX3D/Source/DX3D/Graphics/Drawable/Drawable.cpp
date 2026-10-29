#include <cassert>
#include <DX3D/Graphics/Graphics.hpp>
#include <DX3D/Graphics/Bindable/Bindable.hpp>
#include <DX3D/Graphics/Bindable/IndexBuffer.hpp>
#include <DX3D/Graphics/Drawable/Drawable.hpp>

dx3d::Drawable::Drawable(const DrawableDesc& desc) : Base(desc.base) {}

void dx3d::Drawable::Draw(Graphics& graphics) const noexcept(!IS_DEBUG) {
	for (auto& bindable : _bindables) {
		bindable->Bind(graphics);
	}

	for (auto& bindable : GetStaticBindables()) {
		bindable->Bind(graphics);
	}

	graphics.DrawIndexed(_index_buffer_ptr->GetCount());
}

void dx3d::Drawable::AddBind(std::unique_ptr<Bindable> bindable) noexcept(!IS_DEBUG) {
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bindable) != typeid(IndexBuffer));
	_bindables.push_back(std::move(bindable));
}

void dx3d::Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> index_buffer) noexcept(!IS_DEBUG) {
	assert("Attempting to add index buffer a second time" && _index_buffer_ptr == nullptr);
	_index_buffer_ptr = index_buffer.get();
	_bindables.push_back(std::move(index_buffer));
}
