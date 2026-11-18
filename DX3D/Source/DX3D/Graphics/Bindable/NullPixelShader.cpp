#include <DX3D/Graphics/Bindable/BindableCodex.hpp>
#include <DX3D/Graphics/Bindable/NullPixelShader.hpp>

dx3d::BNullPixelShader::BNullPixelShader(const BindableDesc& desc, Graphics& graphics)
	: Bindable(desc)
{}

void dx3d::BNullPixelShader::Bind(Graphics& graphics) noexcept {
	GetContext(graphics)->PSSetShader(nullptr, nullptr, 0u);
}

std::shared_ptr<dx3d::BNullPixelShader> dx3d::BNullPixelShader::Resolve(const BindableDesc& desc, Graphics& graphics) {
	return Codex::Resolve<BNullPixelShader>(desc, graphics);
}

std::string dx3d::BNullPixelShader::GenerateUID() {
	return typeid(BNullPixelShader).name();
}

std::string dx3d::BNullPixelShader::GetUID() const noexcept {
	return GenerateUID();
}
