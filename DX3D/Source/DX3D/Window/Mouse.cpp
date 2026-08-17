#include <sstream>
#include <DX3D/Window/Mouse.hpp>

dx3d::Mouse::Mouse(const MouseDesc& desc)
: Base(desc.base) 
{
	DX3DLogInfo("Initializing mouse");
}

dx3d::Mouse::~Mouse() {
	DX3DLogInfo("Destroying mouse");
}

void dx3d::Mouse::OnMouseMove(int x, int y) noexcept {}

void dx3d::Mouse::OnLeftPressed(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "Mouse left button pressed at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnLeftReleased(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "Mouse left button released at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnRightPressed(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "Mouse right button pressed at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnRightReleased(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "Mouse right button released at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnWheelUp(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "Mouse wheel up at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnWheelDown(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "Mouse wheel down at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}
