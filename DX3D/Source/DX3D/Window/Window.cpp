#include <Windows.h>
#include <DX3D/Window/Window.hpp>

static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

dx3d::Window::Window(const WindowDesc& desc) : Base(desc.base), _window_size(desc.window_size) {

    auto RegisterWindowClass = []() {

        WNDCLASS wc = { };

        wc.lpfnWndProc = WindowProcedure;
        wc.lpszClassName = L"DX11EngineWindow";

        return RegisterClass(&wc);
        };

    static const auto window_class_id = std::invoke(RegisterWindowClass);

    constexpr DWORD window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

    RECT rc = { 0, 0, _window_size.width, _window_size.height };

    AdjustWindowRect(&rc, window_style, false);

    // Create the window.
    _window_handle = CreateWindowEx(
        0,                              // Optional window styles.
        MAKEINTATOM(window_class_id),   // Window class
        L"DX11 Engine Window",          // Window text
        window_style,                   // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,

        NULL,       // Parent window    
        NULL,       // Menu
        NULL,       // Instance handle
        NULL        // Additional application data
    );

    if (nullptr == _window_handle) {
        DX3DLogThrowError("Failed to create window");
    }

    ShowWindow(static_cast<HWND>(_window_handle), SW_SHOW);
}
