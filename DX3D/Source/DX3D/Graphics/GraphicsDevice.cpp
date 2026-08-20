#include <DX3D/Core/WinSwitches.hpp>
#include <d3dcompiler.h>

#include <DX3D/Graphics/GraphicsLogUtils.hpp>
#include <DX3D/Graphics/GraphicsDevice.hpp>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

dx3d::GraphicsDevice::GraphicsDevice(const GraphicsDeviceDesc& desc) 
: Base(desc.base), _window_size(desc.window_size) {
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
		"Failed to create render target view"
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

void dx3d::GraphicsDevice::DrawTestTriangle() {

	// create and set vertex buffer
	struct VertexStruct {
		float x;
		float y;
	};

	const VertexStruct vertices[] = {
		{ 0.0f,  0.5f},
		{ 0.5f, -0.5f},
		{-0.5f, -0.5f}
	};

	D3D11_BUFFER_DESC vertex_buffer_desc{};
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.CPUAccessFlags = 0u;
	vertex_buffer_desc.MiscFlags = 0u;
	vertex_buffer_desc.ByteWidth = sizeof(vertices);
	vertex_buffer_desc.StructureByteStride = sizeof(VertexStruct);

	D3D11_SUBRESOURCE_DATA sub_resource_data{};
	sub_resource_data.pSysMem = vertices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	DX3DGraphicsLogThrowOnFail(
		_device->CreateBuffer(&vertex_buffer_desc, &sub_resource_data, &vertex_buffer),
		"Failed to create the buffer"
	);

	const UINT stride = sizeof(VertexStruct);
	const UINT offset = 0u;
	// NOTE : &vertex_buffer will get the pp but it release the currunt resources therefor use 'GetAddressOf' to get pp
	_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	
	// read and set vertex shader
	Microsoft::WRL::ComPtr<ID3DBlob> vs_blod{};
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader{};
	DX3DGraphicsLogThrowOnFail(
		D3DReadFileToBlob(L"VertexShader.cso", &vs_blod),
		"Failed to read vertex shader binary"
	);

	DX3DGraphicsLogThrowOnFail(
		_device->CreateVertexShader(vs_blod->GetBufferPointer(), vs_blod->GetBufferSize(), nullptr, &vertex_shader),
		"Failed to create vertex shader"
	);
	
	_context->VSSetShader(vertex_shader.Get(), nullptr, 0u);

	// input (vertex) layout (2d position only)
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout{};

	const D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	DX3DGraphicsLogThrowOnFail(
		_device->CreateInputLayout(
			input_element_desc,
			std::size(input_element_desc),
			vs_blod->GetBufferPointer(),
			vs_blod->GetBufferSize(),
			&input_layout),
		"Failed to create input layout"
	);

	_context->IASetInputLayout(input_layout.Get());

	// read and set pixel shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader{};
	Microsoft::WRL::ComPtr<ID3DBlob> ps_blod{};
	DX3DGraphicsLogThrowOnFail(
		D3DReadFileToBlob(L"PixelShader.cso", &ps_blod),
		"Failed to read pixel shader binary"
	);

	DX3DGraphicsLogThrowOnFail(
		_device->CreatePixelShader(ps_blod->GetBufferPointer(), ps_blod->GetBufferSize(), nullptr, &pixel_shader),
		"Failed to create pixel shader"
	);

	_context->PSSetShader(pixel_shader.Get(), nullptr, 0u);

	// bind render target
	// NOTE : &_target_view will get the pp but it release the currunt resources therefor use 'GetAddressOf' to get pp
	_context->OMSetRenderTargets(1u, _target_view.GetAddressOf(), nullptr);

	// set Primitive Topology to triangle list (groups of 3 vertices)
	_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// confifure viewport
	D3D11_VIEWPORT view_port{};
	view_port.Width = _window_size.width;
	view_port.Height= _window_size.height;
	view_port.MinDepth = 0;
	view_port.MaxDepth = 1;
	view_port.TopLeftX = 0;
	view_port.TopLeftY = 0;
	_context->RSSetViewports(1u, &view_port);

	// draw
	_context->Draw(std::size(vertices), 0u);
}
