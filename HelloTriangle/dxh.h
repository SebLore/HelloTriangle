#pragma once
#include "pch.h"

#include "Utilities.h"
#include "CustomDataTypes.h"

namespace dx = DirectX; //efficiency

class DXHandler
{
public:
	DXHandler(HWND handle);
	~DXHandler();
	void Render(float dt = 0.0f);
	void SetViewport(FLOAT width, FLOAT height, FLOAT topleftx, FLOAT toplefty, FLOAT maxdepth, FLOAT mindepth);
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
private:
	// Constructor
	bool SetUpInterface(HWND handle, RECT& rc);
	bool SetUpPipeline(RECT& rc);
	// Interface
	bool CreateDeviceAndSwapChain(HWND handle, IDXGISwapChain*& swapchain, ID3D11Device*& device, ID3D11DeviceContext*& devicecontext);
	bool CreateBackbufferRenderTargetView(IDXGISwapChain*& swapchain, ID3D11RenderTargetView*& rendertargetview);
	bool CreateRasterizerState(ID3D11RasterizerState*& ppRasterizerState);
	// Shaders & Pipeline
	bool CreateShaders(ID3D11VertexShader*& vertexshader, ID3D11PixelShader*& pixelshader, ID3D11InputLayout*& inputLayout);
	bool CreateVertexShader(ID3D11VertexShader*& vshader, ID3D11InputLayout*& inputLayout, std::string filepath);
	bool CreateIndexBuffer(ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh);
	bool CreatePixelShader(ID3D11PixelShader*& pshader, std::string filepath);
	bool CreateDepthStencil(UINT width, UINT height, ID3D11DepthStencilView*& dsview, ID3D11DepthStencilState*& dsstate);
	bool CreateInputLayout(ID3D11InputLayout*& layout, const std::string& data);
	bool CreateVertexBuffer(ID3D11Buffer*& vbuffer, const dxh::Mesh& mesh);
	// Texture
	bool LoadImageToTexture(dxh::ImageData& target, const std::string filepath);
	bool CreateTexture(ID3D11ShaderResourceView*& shaderresourceview);
	bool CreateSamplerState(ID3D11SamplerState*& samplerstate);
	// Constant buffer
	bool CreateConstantBuffer(ID3D11Buffer*& cBuffer, UINT byteWidth);
	// MISC
	std::string ReadShaderData(const std::string& filepath);
	bool CreateBuffers();
	void GenerateMesh(dxh::Mesh& mesh);
	void GenerateTexture(dxh::ImageData& id); //generates a default texture, not used
	void MapBuffer(ID3D11Buffer*& cBuffer, const void* src, size_t size);
	void Rotate(float dt);
	void SetAll();
	void SetupBufferObjects(RECT& rc);
	// FOR IMGUI TESTING

private:
	// VARIABLES
	// Interface
	ID3D11Device* device;
	ID3D11DeviceContext* devicecontext;
	IDXGISwapChain* swapchain;
	ID3D11RenderTargetView* bbRenderTargetView;
	ID3D11RasterizerState* rasterizerState;
	// Pipeline
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;
	ID3D11PixelShader* pixelShader;
	// DepthStencil 
	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilState;
	// Buffers
	ID3D11Buffer* bVertex;
	ID3D11Buffer* bIndex;
	ID3D11Buffer* bMaterial;
	ID3D11Buffer* bMatrix;
	ID3D11Buffer* bLight;
	// Texture
	ID3D11ShaderResourceView* textureView;
	ID3D11SamplerState* samplerState;
	// Misc, variables and what not
	dxh::WVP wvp;					//world, view projection matrices
	dxh::SimpleLight light;
	dxh::SimpleMaterial material;
	dxh::Mesh mesh;

	// IMGUI TEST VARIABLES
	float rotation_time = 6.0f; // time for a single rotation in seconds
	float rotation_angle = RAD; // angle to be rotated after rotation_time has elapsed | Rotation per frame is: deltaTime * (angle/time) 
	std::string texture = "sampletexture.png"; // texture being loaded

};
