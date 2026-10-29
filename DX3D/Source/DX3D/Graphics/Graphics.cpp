#include <DX3D/Core/WinSwitches.hpp>
#include <DX3D/Window/ImGUI/imgui_impl_dx11.h>
#include <DX3D/Window/ImGUI/imgui_impl_win32.h>
#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/Graphics.hpp>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

dx3d::Graphics::Graphics(const GraphicsDesc& desc) 
: Base(desc.base), _window_size(desc.window_size) {
	DX3DLogInfo("Initializing Graphics");

	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};

	swap_chain_desc.BufferDesc.Height = std::max(1, desc.window_size.height);
	swap_chain_desc.BufferDesc.Width = std::max(1, desc.window_size.width);
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 0;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = static_cast<HWND>(desc.win_handle);
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;

	UINT create_device_flags{};

	D3D_FEATURE_LEVEL feature_levels_requested[] = {
		 D3D_FEATURE_LEVEL_11_1,
		 D3D_FEATURE_LEVEL_11_0,
		 D3D_FEATURE_LEVEL_10_1,
		 D3D_FEATURE_LEVEL_10_0,
		 D3D_FEATURE_LEVEL_9_3,
		 D3D_FEATURE_LEVEL_9_2,
		 D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL feature_levels{};

#ifndef NDBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // NDBUG

	DX3DGraphicsLogThrowOnFail(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			create_device_flags,
			feature_levels_requested,
			ARRAYSIZE(feature_levels_requested),
			D3D11_SDK_VERSION,
			&swap_chain_desc,
			&_swap_chain,
			&_device,
			&feature_levels,
			&_context
		), 
		"Failed to create D3D11 device, swap chain and context"
	);

	switch (feature_levels) {
		case D3D_FEATURE_LEVEL_11_1: DX3DLogInfo("Feature Level: 11.1"); break;
		case D3D_FEATURE_LEVEL_11_0: DX3DLogInfo("Feature Level: 11.0"); break;
		case D3D_FEATURE_LEVEL_10_1: DX3DLogInfo("Feature Level: 10.1"); break;
		case D3D_FEATURE_LEVEL_10_0: DX3DLogInfo("Feature Level: 10.0"); break;
		case D3D_FEATURE_LEVEL_9_3:  DX3DLogInfo("Feature Level: 9.3"); break;
		case D3D_FEATURE_LEVEL_9_2:  DX3DLogInfo("Feature Level: 9.2"); break;
		case D3D_FEATURE_LEVEL_9_1:  DX3DLogInfo("Feature Level: 9.1"); break;
		default: DX3DLogInfo("Unknown Feature Level"); break;
	}

	Microsoft::WRL::ComPtr<ID3D11Resource> back_buffer{};
	DX3DGraphicsLogThrowOnFail(
		_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)),
		"Failed to get back buffer"
	);

	DX3DGraphicsLogThrowOnFail(
		_device->CreateRenderTargetView(back_buffer.Get(), nullptr, &_target_view),
		"Failed to create render target view"
	);

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC depth_stensil_desc{};
	depth_stensil_desc.DepthEnable = TRUE;
	depth_stensil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stensil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stensil_state{};
	DX3DGraphicsLogThrowOnFail(
		_device->CreateDepthStencilState(&depth_stensil_desc, &depth_stensil_state), 
		"Failed to create depth stensil state"
	);

	// bind depth stensil state
	_context->OMSetDepthStencilState(depth_stensil_state.Get(), 1u);

	// create depth stensil texture
	D3D11_TEXTURE2D_DESC depth_stensil_texture_desc{};
	depth_stensil_texture_desc.Width = _window_size.width;
	depth_stensil_texture_desc.Height = _window_size.height;
	depth_stensil_texture_desc.MipLevels = 1u;
	depth_stensil_texture_desc.ArraySize = 1u;
	depth_stensil_texture_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stensil_texture_desc.SampleDesc.Count = 1u;
	depth_stensil_texture_desc.SampleDesc.Quality = 0u;
	depth_stensil_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stensil_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stensil_texture{};
	DX3DGraphicsLogThrowOnFail(
		_device->CreateTexture2D(&depth_stensil_texture_desc, nullptr, &depth_stensil_texture),
		"Failed to create depth stensil texture"
	);

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stensil_view_desc{};
	depth_stensil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stensil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stensil_view_desc.Texture2D.MipSlice = 0u;

	DX3DGraphicsLogThrowOnFail(
		_device->CreateDepthStencilView(depth_stensil_texture.Get(), &depth_stensil_view_desc, &_depth_stencil_view),
		"Failed to create depth stensil view"
	);

	// bind render target and epth stensil view to OM
	// NOTE : &_target_view will get the pp but it release the currunt resources therefor use 'GetAddressOf' to get pp
	_context->OMSetRenderTargets(1u, _target_view.GetAddressOf(), _depth_stencil_view.Get());

	// confifure viewport
	D3D11_VIEWPORT view_port{};
	view_port.Width = static_cast<float>(_window_size.width);
	view_port.Height = static_cast<float>(_window_size.height);
	view_port.MinDepth = 0;
	view_port.MaxDepth = 1;
	view_port.TopLeftX = 0;
	view_port.TopLeftY = 0;
	_context->RSSetViewports(1u, &view_port);

	ImGui_ImplDX11_Init(_device.Get(), _context.Get());
}

dx3d::Graphics::~Graphics() {
	DX3DLogInfo("Destroying Graphics");
	ImGui_ImplDX11_Shutdown();
}

void dx3d::Graphics::EndFrame() {
	if (_imgui_enabled) {
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	HRESULT hr{};
	if (FAILED(hr = _swap_chain->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			DX3DGraphicsLogThrowOnFail(_device->GetDeviceRemovedReason(), "Failed the Present call [DEVICE_REMOVED]");
		}
	} else {
		DX3DGraphicsLogThrowOnFail(hr, "Failed the Present call");
	}
}

void dx3d::Graphics::BeginFrame(float red, float green, float blue) noexcept {
	if (_imgui_enabled) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { red, green, blue };
	_context->ClearRenderTargetView(_target_view.Get(), color);
	_context->ClearDepthStencilView(_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void dx3d::Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG) {
	_context->DrawIndexed(count, 0u, 0u);
}

void dx3d::Graphics::SetProjection(DirectX::FXMMATRIX projection) noexcept {
	_projection = projection;
}

DirectX::XMMATRIX dx3d::Graphics::GetProjection() const noexcept {
	return _projection;
}

void dx3d::Graphics::SetCamera(DirectX::FXMMATRIX camera) noexcept {
	_camera = camera;
}

DirectX::XMMATRIX dx3d::Graphics::GetCamera() const noexcept {
	return _camera;
}

void dx3d::Graphics::EnableImgui() noexcept {
	_imgui_enabled = true;
}

void dx3d::Graphics::DisableImgui() noexcept {
	_imgui_enabled = false;
}

bool dx3d::Graphics::IsImguiEnabled() const noexcept {
	return _imgui_enabled;
}
