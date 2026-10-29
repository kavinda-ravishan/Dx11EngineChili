 //#define DX3D_LOG_WIN_MESSAGES

#include <DX3D/Core/WinSwitches.hpp>
#include <Windows.h>
#include <DX3D/Window/ImGUI/imgui_impl_win32.h>
#include <DX3D/Window/Window.hpp>
#include <DX3D/Window/WindowsMessageMap.hpp>
#include <DX3D/IO/Keyboard.hpp>
#include <DX3D/IO/Mouse.hpp>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    dx3d::Window* const window_ptr = reinterpret_cast<dx3d::Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    
    void* const arg_hwnd = static_cast<void*>(hwnd);
    const uint32_t arg_msg = static_cast<uint32_t>(msg);
    const uint64_t arg_wparam = static_cast<uint64_t>(wparam);
    const int64_t arg_lparam = static_cast<int64_t>(lparam);
    
    return (LRESULT)window_ptr->HandleMessage(arg_hwnd, arg_msg, arg_wparam, arg_lparam);
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

    ShowWindow(static_cast<HWND>(_window_handle), SW_SHOWDEFAULT);

    ImGui_ImplWin32_Init(static_cast<HWND>(_window_handle));
}

dx3d::Window::~Window() {
    DX3DLogInfo("Destroying window");
    ImGui_ImplWin32_Shutdown();
}

void* dx3d::Window::GetWindowHandle() const noexcept {
    return _window_handle;
}

dx3d::Rect dx3d::Window::GetWindowSize() const noexcept {
    return _window_size;
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

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return true;

    switch (msg) {
    // Window events
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KILLFOCUS:
        _keyboard->ClearState();
        break;
    
    // Keyboard events
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        if (!(lparam & 0x40000000) || _keyboard->AutoRepeatIsEnabled()) {
            _keyboard->OnKeyPressed(static_cast<unsigned char>(wparam));
        }
        break;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        _keyboard->OnKeyReleased(static_cast<unsigned char>(wparam));
        break;
    }
    case WM_CHAR:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        _keyboard->OnChar(static_cast<unsigned char>(wparam));
        break;
    }
    
    // Mouse events
    case WM_MOUSEMOVE:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnMouseMove(pt.x, pt.y);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnLeftPressed(pt.x, pt.y);
        break;
    }
    case WM_LBUTTONUP:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnLeftReleased(pt.x, pt.y);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnRightPressed(pt.x, pt.y);
        break;
    }
    case WM_RBUTTONUP:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnRightReleased(pt.x, pt.y);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnMiddlePressed(pt.x, pt.y);
        break;
    }
    case WM_MBUTTONUP:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        const POINTS pt = MAKEPOINTS(lparam);
        _mouse->OnMiddleReleased(pt.x, pt.y);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        if (ImGui::GetIO().WantCaptureMouse) break;

        POINT pt{
            static_cast<LONG>(MAKEPOINTS(lparam).x),
            static_cast<LONG>(MAKEPOINTS(lparam).y)
        };

        ScreenToClient(hwnd, &pt);

        const int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        _mouse->OnWheelMove(static_cast<int>(delta/(float)WHEEL_DELTA), pt.x, pt.y);
        break;
    }
    
    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}
