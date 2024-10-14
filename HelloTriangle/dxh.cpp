#include "pch.h"
#include "dxh.h"


//implementing stb_image.h
#include "ImageLoader.h"

DXHandler::DXHandler(HWND handle)
{
	RECT rc;
	GetClientRect(handle, &rc);
	if (!SetUpInterface(handle, rc))
	{
		util::ErrorMessageBox("Failed to set up interface.");
		throw;
	}

	if (!SetUpPipeline(rc))
	{
		util::ErrorMessageBox("Failed to set up pipeline.");
		throw;
	}
}


DXHandler::~DXHandler()
{
	// Interface
	if (device)	device->Release();
	if (devicecontext) devicecontext->Release();
	if (swapchain) swapchain->Release();
	if (rasterizerState) rasterizerState->Release();
	if (bbRenderTargetView) bbRenderTargetView->Release();
	// Pipeline
	if (vertexShader) vertexShader->Release();
	if (inputLayout) inputLayout->Release();
	if (pixelShader) pixelShader->Release();
	// Depth buffer
	if (depthStencilView) depthStencilView->Release();
	if (depthStencilState) depthStencilState->Release();
	// Vertex & Constant Buffers
	if (bVertex) bVertex->Release();
	if (bMatrix) bMatrix->Release();
	if (bMaterial) bMaterial->Release();
	if (bLight) bLight->Release();
	if (bIndex) bIndex->Release();
	//Texture
	if (textureView) textureView->Release();
	if (samplerState) samplerState->Release();
}

bool DXHandler::SetUpInterface(HWND handle, RECT& rc)
{
	if (!CreateDeviceAndSwapChain(handle, swapchain, device, devicecontext))
	{
		util::ErrorMessageBox("Failed to create device and swapchain.");
		return false;
	}
	if (!CreateBackbufferRenderTargetView(swapchain, bbRenderTargetView))
	{
		util::ErrorMessageBox("Failed to create Rendertarget View.");
		return false;
	}

	if (!CreateRasterizerState(rasterizerState))
	{
		util::ErrorMessageBox("Failed to create rasterizer state.");
		return false;
	}

	if (!CreateDepthStencil(rc.right - rc.left, rc.bottom - rc.top, depthStencilView, depthStencilState))
	{
		util::ErrorMessageBox("Failed to create Depth Stencil");
		return false;
	}

	SetViewport(FLOAT(rc.right - rc.left), FLOAT(rc.bottom - rc.top), 0, 0, 1, 0);

	return true;
}

bool DXHandler::SetUpPipeline(RECT& rc)
{
	//Get window client area for later

	if (!CreateShaders(vertexShader, pixelShader, inputLayout)) return false;

	//CPU side objects
	GenerateMesh(mesh);
	SetupBufferObjects(rc);

	if (!CreateBuffers()) return false;
	if (!CreateTexture(textureView))
	{
		// create texture makes its own error message box if the texture fails to be created
		// or if the image file isn't loaded
		util::ErrorMessageBox("Failed to create shader resource view of texture!");
		return false;
	}
	if (!CreateSamplerState(samplerState))
	{
		util::ErrorMessageBox("Failed to create sampler state!");
		return false;
	}
	SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return true;
}

// **********************************************************************************************************
// INTERNAL/HELPER FUNCTIONS
// **********************************************************************************************************

std::string DXHandler::ReadShaderData(std::string filepath) {

	std::string data;
	std::ifstream fstr(filepath, std::ios::binary | std::ios::ate);
	if (!fstr.is_open())
	{
		util::ErrorMessageBox("Shader file \"" + filepath + "\" could not be opened.");
		return "";
	}

	fstr.seekg(0, std::ios::end);
	data.reserve(static_cast<unsigned int>(fstr.tellg()));
	fstr.seekg(0, std::ios::beg);

	data.assign((std::istreambuf_iterator<char>(fstr)), std::istreambuf_iterator<char>());

	return data;
}

void DXHandler::SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	devicecontext->IASetPrimitiveTopology(topology);
}
// **********************************************************************************************************
// DEVICE / INTERFACE
// **********************************************************************************************************

bool DXHandler::CreateDeviceAndSwapChain(HWND handle, IDXGISwapChain*& swapchain, ID3D11Device*& device, ID3D11DeviceContext*& devicecontext)
{
	DXGI_SWAP_CHAIN_DESC scd{ 0 };
	RECT rc{};
	GetClientRect(handle, &rc);

	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = (unsigned int)(rc.right - rc.left);
	scd.BufferDesc.Height = (unsigned int)(rc.bottom - rc.top);
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferCount = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = handle;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr =
		D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&scd,
			&swapchain, //storage pointer for swapchain
			&device,	//storage pointer for device
			NULL,
			&devicecontext //storage pointer for immediate context
		);

	return SUCCEEDED(hr);
}

bool DXHandler::CreateBackbufferRenderTargetView(IDXGISwapChain*& swapchain, ID3D11RenderTargetView*& rtv)
{
	ID3D11Texture2D* backbuffer = NULL;
	HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to get back buffer for render target view!");
		return false;
	}
	hr = device->CreateRenderTargetView(backbuffer, NULL, &rtv);
	backbuffer->Release();

	return SUCCEEDED(hr);
}

bool DXHandler::CreateRasterizerState(ID3D11RasterizerState*& ppRasterizerState)
{
	D3D11_RASTERIZER_DESC rzd{};
	rzd.FillMode = D3D11_FILL_SOLID;
	rzd.CullMode = D3D11_CULL_BACK;
	rzd.FrontCounterClockwise = false;
	rzd.MultisampleEnable = true;
	rzd.AntialiasedLineEnable = true;
	rzd.ScissorEnable = false;
	rzd.DepthClipEnable = false;

	return SUCCEEDED(device->CreateRasterizerState(&rzd, &ppRasterizerState));
}

void DXHandler::SetViewport(FLOAT width, FLOAT height, FLOAT topleftx, FLOAT toplefty, FLOAT maxdepth, FLOAT mindepth) {

	D3D11_VIEWPORT vp{ 0 };
	vp.Width = width;
	vp.Height = height;
	vp.TopLeftX = topleftx;
	vp.TopLeftY = toplefty;
	vp.MaxDepth = maxdepth;
	vp.MinDepth = mindepth;
	devicecontext->RSSetViewports(1, &vp);
}


// **********************************************************************************************************
// PIPELINE
// **********************************************************************************************************

bool DXHandler::CreateVertexShader(ID3D11VertexShader*& vshader, ID3D11InputLayout*& inputLayout, std::string filepath)
{
	std::string data = ReadShaderData(filepath);

	if (FAILED(device->CreateVertexShader(data.c_str(), data.length(), NULL, &vshader)))
	{
		return false;
	}

	if (!CreateInputLayout(inputLayout, data))
	{
		util::ErrorMessageBox("Failed to create input layout.");
		return false;
	}

	return true;
}

bool DXHandler::CreatePixelShader(ID3D11PixelShader*& pshader, std::string filepath) {

	std::string data = ReadShaderData(filepath);
	return SUCCEEDED(device->CreatePixelShader(data.c_str(), data.length(), NULL, &pshader));
}

bool DXHandler::CreateShaders(ID3D11VertexShader*& vertexshader, ID3D11PixelShader*& pixelshader, ID3D11InputLayout*& inputLayout)
{
	if (!CreateVertexShader(vertexshader, inputLayout, "hlsl/VertexShader.cso"))
	{
		util::ErrorMessageBox("Failed to create vertex shader. ");
		return false;
	}
	if (!CreatePixelShader(pixelshader, "hlsl/PixelShader.cso"))
	{
		util::ErrorMessageBox("Failed to create pixel shader. ");
		return false;
	}
	return true;
}

bool DXHandler::CreateBuffers()
{
	if (!CreateVertexBuffer(bVertex, mesh))
	{
		util::ErrorMessageBox("Failed to set up Vertex Buffer");
		return false;
	}
	if (!CreateIndexBuffer(bIndex, mesh))
	{
		util::ErrorMessageBox("Failed to set up Index Buffer");
		return false;
	}

	if (!CreateConstantBuffer(bMatrix, sizeof(dxh::WVP)))
	{
		util::ErrorMessageBox("Could not set up WVP constant buffer.");
		return false;
	}
	if (!CreateConstantBuffer(bLight, sizeof(dxh::SimpleLight)))
	{
		util::ErrorMessageBox("could not set up light.");
		return false;
	}

	if (!CreateConstantBuffer(bMaterial, sizeof(dxh::SimpleMaterial)))
	{
		util::ErrorMessageBox("Failed to set up material buffer");
		return false;
	}
	return true;
}

bool DXHandler::CreateConstantBuffer(ID3D11Buffer*& cBuffer, UINT byteWidth)
{
	D3D11_BUFFER_DESC bd{};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = byteWidth;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	return SUCCEEDED(device->CreateBuffer(&bd, NULL, &cBuffer));
}

bool DXHandler::CreateDepthStencil(UINT width, UINT height, ID3D11DepthStencilView*& dsview, ID3D11DepthStencilState*& dsstate)
{
	D3D11_TEXTURE2D_DESC dstexdesc{};
	ZeroMemory(&dstexdesc, sizeof(dstexdesc));
	dstexdesc.Width = width;
	dstexdesc.Height = height;
	dstexdesc.MipLevels = 1;
	dstexdesc.ArraySize = 1;
	dstexdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dstexdesc.SampleDesc.Count = 1;
	dstexdesc.SampleDesc.Quality = 0;
	dstexdesc.Usage = D3D11_USAGE_DEFAULT;
	dstexdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dstexdesc.CPUAccessFlags = 0;
	dstexdesc.MiscFlags = 0;


	ID3D11Texture2D * dstex;
	HRESULT hr = device->CreateTexture2D(&dstexdesc, NULL, &dstex);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create 2D texture for depth stencil.");
		if (dstex) dstex->Release();
		return false;
	}

	hr = device->CreateDepthStencilView(dstex, NULL, &dsview);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create Depth Stencil View. ");
		if (dstex) dstex->Release();
		return false;
	}


	D3D11_DEPTH_STENCIL_DESC dsDesc{};
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = device->CreateDepthStencilState(&dsDesc, &dsstate);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create Depth Stencil State.");
		if (dstex) dstex->Release();
		return false;
	}
	if (dstex) dstex->Release();

	return true;
}

// **********************************************************************************************************
// TEXTURE
// **********************************************************************************************************

//Loads an image from a file using the stb_image library and converts it to a usable texture
bool DXHandler::LoadImageToTexture(dxh::ImageData& target, const std::string filepath)
{
	static ImageLoadRaw il;
	return	il.ImageFromFile(target, filepath.c_str());
}

bool DXHandler::CreateTexture(ID3D11ShaderResourceView*& shaderresourceview)
{
	dxh::ImageData idTex;
	if (!LoadImageToTexture(idTex, "resources/sampletexture.png"))
	{
		util::ErrorMessageBox("Failed to load image data.");
		return false;
	}


	D3D11_TEXTURE2D_DESC texDesc{ 0 };
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = idTex.width;
	texDesc.Height = idTex.height;
	texDesc.MipLevels = texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = 0;
	texDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA texData{};
	texData.pSysMem = &idTex.data[0]; //start point
	texData.SysMemPitch = idTex.width * idTex.channels; //width of one row
	texData.SysMemSlicePitch = 0;

	ID3D11Texture2D* texTemp;

	HRESULT hr = device->CreateTexture2D(&texDesc, &texData, &texTemp);
	if (FAILED(hr))
	{
		util::ErrorMessageBox("Failed to create 2D texture.");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = texDesc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	resViewDesc.Texture2D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(texTemp, &resViewDesc, &shaderresourceview);

	texTemp->Release();

	return SUCCEEDED(hr);
}

bool DXHandler::CreateSamplerState(ID3D11SamplerState*& samplerstate)
{
	D3D11_SAMPLER_DESC samplerDesc{};
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	return SUCCEEDED(device->CreateSamplerState(&samplerDesc, &samplerstate));
}

//*********************************************************
//MESH
//*********************************************************

void DXHandler::GenerateMesh(dxh::Mesh& mesh) {

	dxh::float3 positions[]
	{
		{ -0.5f, -0.5f, 0.0f}, // bottom left				  
		{ -0.5f,  0.5f, 0.0f}, // top left                 
		{  0.5f,  0.5f, 0.0f}, // top right				  
		{  0.5f, -0.5f, 0.0f}, // bottom right			
	};

	dxh::float3 normal = dxh::cross(positions[0] - positions[1], positions[0] - positions[2]);


	dxh::float2 uv[]
	{
		{0, 1}, // bottom left uv
		{0, 0}, // top left uv
		{1, 0}, // top right uv
		{1, 1}, // bottom right uv
	};

	UINT indices[]
	{
		0, 1, 2, //first triangle
		0, 2, 3  //second triangle
	};


	//Push into 
	for (int i = 0; i < 4; i++)
	{
		mesh.vertices.push_back(dxh::Vertex(positions[i], normal, uv[i]));
	}
	for (int i = 0; i < 6; i++)
	{
		mesh.indices.push_back(indices[i]);
	}
}

bool DXHandler::CreateInputLayout(ID3D11InputLayout*& layout, const std::string& bytecode)
{

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), bytecode.c_str(), bytecode.length(), &layout); //create input-layout for the assembler stage

	return SUCCEEDED(hr);
}

bool DXHandler::CreateVertexBuffer(ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh)
{

	D3D11_BUFFER_DESC buffdesc{ 0 };
	ZeroMemory(&buffdesc, sizeof(buffdesc));
	buffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffdesc.Usage = D3D11_USAGE_DYNAMIC;
	buffdesc.ByteWidth = mesh.ByteWidth();
	buffdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA blob{};
	blob.pSysMem = mesh.vertices.data();

	HRESULT hr = device->CreateBuffer(&buffdesc, &blob, &vbuffer);
	return SUCCEEDED(hr);
}

bool DXHandler::CreateIndexBuffer(ID3D11Buffer*& bIndex, const dxh::Mesh& mesh)
{
	D3D11_BUFFER_DESC ibd{};
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = static_cast<UINT>(mesh.indices.size())*sizeof(UINT);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = mesh.indices.data();
	HRESULT hr = device->CreateBuffer(&ibd, &initData, &bIndex);

	return SUCCEEDED(hr);
}

void DXHandler::SetupBufferObjects(RECT& rc)
{
	// CAMERA
	dxh::float3 viewpos = { 0.0f, 0.0, -1.0f };

	// world
	dx::XMStoreFloat4x4(&wvp.world, dx::XMMatrixIdentity());

	// view
	dx::XMStoreFloat4x4(&wvp.view, dx::XMMatrixTranspose(
		dx::XMMatrixLookAtLH(
			{ viewpos.x, viewpos.y, viewpos.z, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f }
		)
	)
	);
	// projection
	dx::XMStoreFloat4x4(&wvp.project, dx::XMMatrixTranspose(
			dx::XMMatrixPerspectiveFovLH(
				PI * 0.5, //90
				static_cast<FLOAT>((rc.right - rc.left) / (rc.bottom - rc.top)),
				0.1f,
				20.0f
			)
		)
	);

	// LIGHTS
	light.pos = dxh::Float4(-0.5f, 0.5f, -2.0f, 1.0f); //up, to the left and back
	light.color = dxh::Float4(1.0f, 1.0f, 1.0f, 1.0f);
	light.viewpos = viewpos;

	// MATERIAL
	material.ambi_col = dxh::float4(0.2f, 0.2f, 0.2f, 0.0f);
	material.diff_col = dxh::float4(0.6f, 0.6f, 0.6f, 0.0f);
	material.spec_col = dxh::float4(1.0f, 1.0f, 1.0f, 0.0f);
	material.spec_factor = 32.0f;
}

//generates a default texture to an image data structure
#ifdef _TESTING_G
void DXHandler::GenerateTexture(dxh::ImageData& id)
{
	id.height = 512;
	id.width = 512;
	id.channels = 4;
	id.data.resize(id.width * id.height * 4);

	for (int h = 0; h < id.height; ++h)
	{
		for (int w = 0; w < id.width; ++w)
		{
			unsigned char r = w < id.width / 3 ? 255 : 0;
			unsigned char g = w >= id.width / 3 && w <= id.width / 1.5f ? 255 : 0;
			unsigned char b = w > id.width / 1.5f ? 255 : 0;
			unsigned int pos0 = w * id.channels + id.width * id.channels * h;
			id.data[pos0 + 0] = r;
			id.data[pos0 + 1] = g;
			id.data[pos0 + 2] = b;
			id.data[pos0 + 3] = 255;
		}
	}
}
#endif
//*********************************************************
// RENDER AND RENDER HELP FUNCTIONS
//*********************************************************

void DXHandler::Rotate(float dt) //Rotates world matrix at a rate of 2pi(rad)/rot_time(sec)
{
	// one whole lap in radians, time in seconds for a full rotation
	const float angle = RAD;
	// approximately one rotation every 8 seconds
	const float rot_time = 8.0;
	dx::XMMATRIX rotation	= dx::XMMatrixTranspose(dx::XMMatrixRotationY(angle * (dt / rot_time)));
	dx::XMMATRIX translate	= dx::XMMatrixTranspose(dx::XMMatrixTranslation(0, 0, -0.5f));
	dx::XMMATRIX inverse	= dx::XMMatrixTranspose(dx::XMMatrixTranslation(0, 0, 0.5f));
	dx::XMMATRIX world = dx::XMLoadFloat4x4(&wvp.world);

	world = world * (inverse * rotation * translate); //right to left

	dx::XMStoreFloat4x4(&wvp.world, world);
}

void DXHandler::SetAll()
{
	const UINT32 pStride = sizeof(dxh::Vertex);
	const UINT32 offset = 0;
	//Interface
	devicecontext->OMSetRenderTargets(1, &bbRenderTargetView, depthStencilView);
	devicecontext->OMSetDepthStencilState(depthStencilState, 0);
	devicecontext->RSSetState(rasterizerState);
	//Vertex Shader
	devicecontext->VSSetShader(vertexShader, nullptr, 0);
	devicecontext->VSSetConstantBuffers(0, 1, &bMatrix);
	devicecontext->IASetInputLayout(inputLayout);
	devicecontext->IASetVertexBuffers(0, 1, &bVertex, &pStride, &offset);
	devicecontext->IASetIndexBuffer(bIndex, DXGI_FORMAT_R32_UINT, 0);
	// Pixel shader
	devicecontext->PSSetShader(pixelShader, nullptr, 0);
	devicecontext->PSSetConstantBuffers(0, 1, &bLight);
	devicecontext->PSSetConstantBuffers(1, 1, &bMaterial);
	//Texture
	devicecontext->PSSetShaderResources(0, 1, &textureView);
	devicecontext->PSSetSamplers(0, 1, &samplerState);
}

void DXHandler::MapBuffer(ID3D11Buffer*& gBuffer, const void* src, size_t size)
{
	D3D11_MAPPED_SUBRESOURCE msrc;
	ZeroMemory(&msrc, sizeof(msrc));
	devicecontext->Map(gBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msrc);
	memcpy(msrc.pData, src, size);
	devicecontext->Unmap(gBuffer, 0);
}

//main render loop
void DXHandler::Render(float dt)
{
	const FLOAT clearColor[] = { 0.1f, 0.f, 0.3f, 1.0f };
	devicecontext->ClearRenderTargetView(bbRenderTargetView, clearColor);
	devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Set everything
	SetAll();

	// Update buffers
	Rotate(dt);

	// Map buffer data
	MapBuffer(bVertex, mesh.vertices.data(), mesh.ByteWidth());
	MapBuffer(bMatrix, &wvp, sizeof(wvp));
	MapBuffer(bLight, &light, sizeof(light));
	MapBuffer(bMaterial, &material, sizeof(material));

	//Draw vertices
	devicecontext->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);

	swapchain->Present(0, 0);
}