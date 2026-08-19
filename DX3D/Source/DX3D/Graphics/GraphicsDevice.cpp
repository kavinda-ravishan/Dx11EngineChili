#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/GraphicsDevice.hpp>

#pragma comment(lib, "d3d11.lib")

dx3d::GraphicsDevice::GraphicsDevice(const GraphicsDeviceDesc& desc) 
: Base(desc.base) {
	DX3DLogInfo("Initializing Graphics Device");

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

#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	DX3DGraphicsLogThrowOnFail(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			create_device_flags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swap_chain_desc,
			&_swap_chain,
			&_device,
			nullptr,
			&_context
		), 
		"Failed to create D3D11 device, swap chain and context"
	);

	Microsoft::WRL::ComPtr<ID3D11Resource> back_buffer{};
	DX3DGraphicsLogThrowOnFail(
		_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)),
		"Failed to get back buffer"
	);

	DX3DGraphicsLogThrowOnFail(
		_device->CreateRenderTargetView(back_buffer.Get(), nullptr, &_target_view),
		"Failed to create render target"
	);
}

dx3d::GraphicsDevice::~GraphicsDevice() {
	DX3DLogInfo("Destroying Graphics Device");
}

void dx3d::GraphicsDevice::EndFrame() {
	DX3DGraphicsLogThrowOnFail(
		_swap_chain->Present(1u, 0u),
		"Failed the Present call"
	);
}

void dx3d::GraphicsDevice::ClearBuffer(float red, float green, float blue) {
	const float color[] = { red, green, blue };
	_context->ClearRenderTargetView(_target_view.Get(), color);
}
