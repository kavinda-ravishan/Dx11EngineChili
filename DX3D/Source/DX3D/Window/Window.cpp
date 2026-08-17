 //#define DX3D_LOG_WIN_MESSAGES

#include <Windows.h>
#include <DX3D/Window/Window.hpp>
#include <DX3D/Window/WindowsMessageMap.hpp>
#include <DX3D/IO/Keyboard.hpp>
#include <DX3D/IO/Mouse.hpp>

static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    dx3d::Window* const window_ptr = reinterpret_cast<dx3d::Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    void* const arg_hwnd = static_cast<void*>(hwnd);
    const uint32_t arg_msg = static_cast<uint32_t>(msg);
    const uint64_t arg_wparam = static_cast<uint64_t>(wparam);
    const int64_t arg_lparam = static_cast<int64_t>(lparam);
    
    return window_ptr->HandleMessage(arg_hwnd, arg_msg, arg_wparam, arg_lparam);
}

static LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

    if (WM_NCCREATE == msg) {
        const CREATESTRUCTW* const p_create = reinterpret_cast<CREATESTRUCTW*>(lparam);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(p_create->lpCreateParams));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcedure));

        return WindowProcedure(hwnd, msg, wparam, lparam);
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

dx3d::Window::Window(const WindowDesc& desc) : 
    Base(desc.base), 
    _window_size(desc.window_size), 
    _h_instance(GetModuleHandle(NULL)), 
    _keyboard(std::make_unique<Keyboard>(KeyboardDesc(desc.base))), 
    _mouse(std::make_unique<Mouse>(MouseDesc(desc.base)))
{
    DX3DLogInfo("Initializing window");

    auto RegisterWindowClass = [&]() {

        WNDCLASSEX wc = { };

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpszClassName = L"DX11EngineWindow";
        wc.lpfnWndProc = HandleMessageSetup;
        wc.hInstance = static_cast<HINSTANCE>(_h_instance);

        return RegisterClassEx(&wc);
        };

    static const auto window_class_id = std::invoke(RegisterWindowClass);

    constexpr DWORD window_style = WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;

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

        NULL,                                // Parent window    
        NULL,                                // Menu
        static_cast<HINSTANCE>(_h_instance), // Instance handle
        this                                 // Additional application data
    );

    if (nullptr == _window_handle) {
        DX3DLogThrowError("Failed to create window");
    }

    ShowWindow(static_cast<HWND>(_window_handle), SW_SHOW);
}

dx3d::Window::~Window() {
    DX3DLogInfo("Destroying window");
}

int64_t dx3d::Window::HandleMessage(void* arg_hwnd, uint32_t arg_msg, uint64_t arg_wparam, int64_t arg_lparam) {

    const HWND hwnd = static_cast<HWND>(arg_hwnd);
    const UINT msg = static_cast<UINT>(arg_msg);
    const WPARAM wparam = static_cast<WPARAM>(arg_wparam);
    const LPARAM lparam = static_cast<LPARAM>(arg_lparam);

#ifdef DX3D_LOG_WIN_MESSAGES
    static dx3d::WindowsMessageMap win_msg_map;
    DX3DLogInfo(win_msg_map(msg, wparam, lparam).c_str());
#endif // DX3D_LOG_WIN_MESSAGES

    switch (msg) {
    // Window events
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KILLFOCUS:
        _keyboard->ClearState();
        break;
    
    // Keyboard events
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lparam & 0x40000000) || _keyboard->AutoRepeatIsEnabled()) {
            _keyboard->OnKeyPressed(static_cast<unsigned char>(wparam));
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        _keyboard->OnKeyReleased(static_cast<unsigned char>(wparam));
        break;
    case WM_CHAR:
        _keyboard->OnChar(static_cast<unsigned char>(wparam));
        break;

     // Mouse events
    case WM_MOUSEMOVE:
    {
        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnMouseMove(pt.x, pt.y);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnLeftPressed(pt.x, pt.y);
        break;
    }
    case WM_LBUTTONUP:
    {
        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnLeftReleased(pt.x, pt.y);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnRightPressed(pt.x, pt.y);
        break;
    }
    case WM_RBUTTONUP:
    {
        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnRightReleased(pt.x, pt.y);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        const POINTS pt = MAKEPOINTS(lparam);
        const int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        if (delta > 0) {
            _mouse->OnWheelUp(pt.x, pt.y);
        }
        else if(delta < 0) {
            _mouse->OnWheelDown(pt.x, pt.y);
        }
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}
