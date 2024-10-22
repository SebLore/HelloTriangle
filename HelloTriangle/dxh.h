// Author: Sebastian Lorensson 2022-12-02
// DXHandler creates, handles rendering of and handles deletion of a renderable 3D scene of a textured quad rotating around the origin/center of the scene.
#pragma once
#include "pch.h"

#include "Utilities.h"
#include "CustomDataTypes.h"


#define MAX_NR_OF_TEXTURES 10

namespace dx = DirectX; //efficiency
constexpr auto Transpose = dx::XMMatrixTranspose;

class DXHandler
{
public:
	DXHandler() = default;
	DXHandler(HWND handle);
	~DXHandler();

	void Initialize(HWND handle);
	void Render(float dt = 0.0f);
	void Present();
	void SetViewport(FLOAT width, FLOAT height, FLOAT topleftx, FLOAT toplefty, FLOAT maxdepth, FLOAT mindepth);
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology)const;
	bool AddTexture(const std::string& filepath);
	ID3D11Device* GetDevice()const { return device; }
	ID3D11DeviceContext* GetContext()const { return context; };
private:
	// Initial state Setup
	bool SetupDirectX(HWND handle, RECT& rc);
	bool SetupPipeline();
	bool SetupInitialState(RECT& rc);
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
	// Transformations
	void SetTranslation(dxh::float3 translation);
	void SetScale(float scale = 1.0f);
	void RotateY(float angle = 2 * PI); //rotate around origo, +clockwise -counterclockwise. default 90 degrees
	void UpdateWorldMatrix();
	void FlipRotateDirection() { m_rotation_angle = -m_rotation_angle; }
	void SetRotationAngle(float angle);

	// Texture
	bool LoadImageToTexture(dxh::ImageData& target, const std::string &filepath); 
	void ChangeTexture(const std::string& filename);
	bool CreateTextureSRV(ID3D11ShaderResourceView*& shaderresourceview,const std::string &filepath); //filepath should be the file name
	bool CreateSamplerState(ID3D11SamplerState*& samplerstate);
	// Buffers
	bool CreateConstantBuffer(ID3D11Buffer*& cBuffer, UINT byteWidth);
	void SetupBufferData(RECT& rc);
	void MapBuffer(ID3D11Buffer*& cBuffer, const void* src, size_t size);
	bool CreateBuffers();
	void GenerateMesh(dxh::Mesh& mesh);
	void SetBufferUpdateFlag(bool val = true);
	void SetTransformationUpdateFlag(bool val = true);
	// Per frame
	void Rotate(float dt); //rotates dt * rotation_time * rotation around the origin.
	void Update(float deltatime);
	
	// State management
	void SetAllStates();
	void SetStateUpdateFlag(bool val = true);
	// MISC
	std::string ReadShaderData(const std::string& filepath);
	//void GenerateTexture(dxh::ImageData& id); //generates a default texture, not used
private:
	// VARIABLES
	// Device
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;
	ID3D11RenderTargetView* bbRenderTargetView; // backbuffer; this is what we draw to
	ID3D11RasterizerState* rasterizerState;
	// Pipeline
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;
	ID3D11PixelShader* pixelShader;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilState;
	// Buffers
	ID3D11Buffer* bVertex;
	ID3D11Buffer* bIndex;
	ID3D11Buffer* bMaterial;
	ID3D11Buffer* bMatrix;
	ID3D11Buffer* bLight;
	// Textures
	ID3D11ShaderResourceView* textureViews[MAX_NR_OF_TEXTURES];
	ID3D11SamplerState* samplerState;
	// Buffer Objects
	dxh::WVP m_wvp;		// struct of world, view and projection matrices
	dxh::ModelData mm;	// translation float3 x, y, z, rotation angle float, scale float
	dxh::SimpleLight m_light;
	dxh::SimpleMaterial m_material;
	dxh::Mesh m_mesh;
	// 
	FLOAT m_clearColor[4] = {0.5, 0.5, 0.5, 1.0f};

	// flags
	bool m_state_needs_update = true;
	bool m_buffer_objects_need_update = true;

	// IMGUI TEST VARIABLES
	bool m_is_rotating = true; // toggles rotation of the quad
	float m_rotation_time = 6.0f; // time for a single rotation in seconds
	float m_rotation_angle = RAD; // angle to be rotated after rotation_time has elapsed | Rotation per frame is: deltaTime * (angle/time) 
	float m_rotation = 0.0f; // the current rotation angle in radians, where 0 makes the quad face the screen.
	std::string m_active_texture = "sampletexture.png"; // name of texture in use
	std::unordered_map<std::string, size_t> m_texture_index_map; // stores indices of loaded textures
};
