#include <sstream>
#include <DX3D/IO/Mouse.hpp>

dx3d::Mouse::Mouse(const MouseDesc& desc)
: Base(desc.base) 
{
	DX3DLogInfo("Initializing mouse");
}

dx3d::Mouse::~Mouse() {
	DX3DLogInfo("Destroying mouse");
}

const std::array<int, 2>& dx3d::Mouse::GetPos() const noexcept {
    return _pos;
}

const int32_t dx3d::Mouse::GetWheelDelta() noexcept {
    const auto delta = _wheel_delta;
    _wheel_delta = 0;
    return delta;
}

void dx3d::Mouse::OnMouseMove(int x, int y) noexcept {
    _pos[0] = x;
    _pos[1] = y;
}

void dx3d::Mouse::OnLeftPressed(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "[Mouse] : " << "Mouse left button pressed at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnLeftReleased(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "[Mouse] : " << "Mouse left button released at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnRightPressed(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "[Mouse] : " << "Mouse right button pressed at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnRightReleased(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "[Mouse] : " << "Mouse right button released at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnMiddlePressed(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "[Mouse] : " << "Mouse middle button pressed at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnMiddleReleased(int x, int y) noexcept {
    std::ostringstream oss;
    oss << "[Mouse] : " << "Mouse middle button released at " << x << ", " << y;

    DX3DLogInfo(oss.str().c_str());
}

void dx3d::Mouse::OnWheelMove(int delta, int x, int y) noexcept {
    _wheel_delta = delta;

    std::ostringstream oss;
    oss << "[Mouse] : " << "Mouse wheel scrolls by " << delta << " at " << x << ", " << y;
    DX3DLogInfo(oss.str().c_str());
}
