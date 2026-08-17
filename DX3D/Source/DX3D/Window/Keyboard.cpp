#include <sstream>
#include <DX3D/Window/Keyboard.hpp>


dx3d::Keyboard::Keyboard(const KeyboardDesc& desc)
: Base(desc.base)
{
	DX3DLogInfo("Initializing keyboard");
}

dx3d::Keyboard::~Keyboard() {
	DX3DLogInfo("Destroying keyboard");
}

void dx3d::Keyboard::OnKeyReleased(unsigned char keycode) noexcept {
	std::ostringstream oss;
	oss << keycode << " released";
	DX3DLogInfo(oss.str().c_str());
}

void dx3d::Keyboard::OnKeyPressed(unsigned char keycode) noexcept {
	std::ostringstream oss;
	oss << keycode << " pressed";
	DX3DLogInfo(oss.str().c_str());
}

void dx3d::Keyboard::OnChar(char character) noexcept {
	std::ostringstream oss;
	oss << "Char " << character;
	DX3DLogInfo(oss.str().c_str());
}

void dx3d::Keyboard::ClearState() noexcept {}

bool dx3d::Keyboard::AutoRepeatIsEnabled() {
	return false;
}
