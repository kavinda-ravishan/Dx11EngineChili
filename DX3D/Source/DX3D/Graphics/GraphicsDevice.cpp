#include <cmath>
#include <DX3D/Core/WinSwitches.hpp>
#include <d3dcompiler.h>
#include <DirectXMath.h>

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

#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

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
}

dx3d::GraphicsDevice::~GraphicsDevice() {
	DX3DLogInfo("Destroying Graphics Device");
}

void dx3d::GraphicsDevice::EndFrame() {
	HRESULT hr{};
	if (FAILED(hr = _swap_chain->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			DX3DGraphicsLogThrowOnFail(_device->GetDeviceRemovedReason(), "Failed the Present call [DEVICE_REMOVED]");
		}
	} else {
		DX3DGraphicsLogThrowOnFail(hr, "Failed the Present call");
	}
}

void dx3d::GraphicsDevice::ClearBuffer(float red, float green, float blue) noexcept {
	const float color[] = { red, green, blue };
	_context->ClearRenderTargetView(_target_view.Get(), color);
	_context->ClearDepthStencilView(_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void dx3d::GraphicsDevice::DrawTestCube(const float angle, const float mouse_pos_x, const float mouse_pos_y) {

	// create and set vertex buffer
	struct VertexStruct {
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	const VertexStruct vertices[] = {
		{-1.0f, -1.0f, -1.0f},
		{ 1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
		{ 1.0f,  1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
	};

	D3D11_BUFFER_DESC vertex_buffer_desc{};
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.CPUAccessFlags = 0u;
	vertex_buffer_desc.MiscFlags = 0u;
	vertex_buffer_desc.ByteWidth = sizeof(vertices);
	vertex_buffer_desc.StructureByteStride = sizeof(VertexStruct);

	D3D11_SUBRESOURCE_DATA v_sub_resource_data{};
	v_sub_resource_data.pSysMem = vertices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	DX3DGraphicsLogThrowOnFail(
		_device->CreateBuffer(&vertex_buffer_desc, &v_sub_resource_data, &vertex_buffer),
		"Failed to create the vertex buffer"
	);

	const UINT v_stride = sizeof(VertexStruct);
	const UINT v_offset = 0u;
	// NOTE : &vertex_buffer will get the pp but it release the currunt resources therefor use 'GetAddressOf' to get pp
	_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &v_stride, &v_offset);
	
	// create and set index buffer
	const unsigned short indices[] = {
		0, 2, 1,  2, 3, 1, 
		1, 3, 5,  3, 7, 5, 
		2, 6, 3,  3, 6, 7, 
		4, 5, 7,  4, 7, 6, 
		0, 4, 2,  2, 4, 6, 
		0, 1, 4,  1, 5, 4
	};

	D3D11_BUFFER_DESC index_buffer_desc{};
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.CPUAccessFlags = 0u;
	index_buffer_desc.MiscFlags = 0u;
	index_buffer_desc.ByteWidth = sizeof(indices);
	index_buffer_desc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA i_sub_resource_data{};
	i_sub_resource_data.pSysMem = indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer{};
	DX3DGraphicsLogThrowOnFail(
		_device->CreateBuffer(&index_buffer_desc, &i_sub_resource_data, &index_buffer),
		"Failed to create the index buffer"
	);

	_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// create constant buffer for transformation matrix
	struct VSConstantBufferStruct {
		DirectX::XMMATRIX transform;
	};

	const float aspect_ratio = float(_window_size.height) / _window_size.width;

	const VSConstantBufferStruct vs_constant_buffer = {
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(angle)*
				DirectX::XMMatrixRotationX(angle) *
				DirectX::XMMatrixTranslation(mouse_pos_x, 0.0f, mouse_pos_y + 4.0f) *
				DirectX::XMMatrixPerspectiveLH(1.0f, aspect_ratio, 0.5f, 10.0f)
			)
		}
	};

	D3D11_BUFFER_DESC vs_constant_buffer_desc{};
	vs_constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vs_constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	vs_constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vs_constant_buffer_desc.MiscFlags = 0u;
	vs_constant_buffer_desc.ByteWidth = sizeof(vs_constant_buffer);
	vs_constant_buffer_desc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA vs_constant_buffer_data{};
	vs_constant_buffer_data.pSysMem = &vs_constant_buffer;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> vs_constant_buffer_res{};
	DX3DGraphicsLogThrowOnFail(
		_device->CreateBuffer(&vs_constant_buffer_desc, &vs_constant_buffer_data, &vs_constant_buffer_res), 
		"Failed to create the VS constant buffer"
	);

	_context->VSSetConstantBuffers(0u, 1u, vs_constant_buffer_res.GetAddressOf());

	// create constant buffer for cube face colors
	struct PSConstantBuffer {
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};

	const PSConstantBuffer ps_constant_buffer = {
		{
			{1.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 1.0f, 1.0f}
		}
	};

	D3D11_BUFFER_DESC ps_constant_buffer_desc{};
	ps_constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	ps_constant_buffer_desc.CPUAccessFlags = 0u;
	ps_constant_buffer_desc.MiscFlags = 0u;
	ps_constant_buffer_desc.ByteWidth = sizeof(ps_constant_buffer);
	ps_constant_buffer_desc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA ps_constant_buffer_data{};
	ps_constant_buffer_data.pSysMem = &ps_constant_buffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> ps_constant_buffer_res{};
	DX3DGraphicsLogThrowOnFail(
		_device->CreateBuffer(&ps_constant_buffer_desc, &ps_constant_buffer_data, &ps_constant_buffer_res),
		"Failed to create the PS constant buffer"
	);

	_context->PSSetConstantBuffers(0u, 1u, ps_constant_buffer_res.GetAddressOf());

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

	const D3D11_INPUT_ELEMENT_DESC vs_input_element_desc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	DX3DGraphicsLogThrowOnFail(
		_device->CreateInputLayout(
			vs_input_element_desc,
			std::size(vs_input_element_desc),
			vs_blod->GetBufferPointer(),
			vs_blod->GetBufferSize(),
			&input_layout),
		"Failed to create VS input layout"
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

	// set Primitive Topology to triangle list (groups of 3 vertices)
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	_context->DrawIndexed(std::size(indices), 0u, 0u);
}
