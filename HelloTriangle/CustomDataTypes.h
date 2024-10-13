#pragma once
#include "pch.h"

#define PI	 3.14159f
#define RAD 2*PI //2 pi = 1 rad

namespace dxh
{
	//Custom variable types
	typedef struct Float2
	{
		float x;
		float y;
		Float2()noexcept : x(0.0f), y(0.0f) {}
		Float2(const Float2& param)noexcept : x(param.x), y(param.y) {}
		Float2& operator=(Float2&& param)noexcept { x = param.x, y = param.y; return *this; }
		Float2& operator=(const Float2& param) { x = param.x, y = param.y; return *this; }
		Float2(const float& _x, const float& _y)noexcept { x = _x, y = _y; }
		constexpr float operator[](const size_t _i) { if (_i == 0) return x; return y; }
		bool operator==(const Float2& comp) { return (this->x == comp.x && this->y == comp.y); }
		bool operator!=(const Float2& comp) { return !Float2::operator==(comp); }
		Float2 operator+(const Float2& right)const { return Float2(this->x + right.x, this->y + right.y); }
		Float2 operator-(const Float2& right)const { return Float2(this->x - right.x, this->y - right.y); }
		friend std::ostream& operator<<(std::ostream& os, const Float2& obj)
		{
			os << obj.x << ", " << obj.y;
			return os;
		}
	}float2;

	typedef struct Float3
	{
		float x;
		float y;
		float z;
		Float3()noexcept : x(0.0f), y(0.0f), z(0.0f) { }
		Float3(const Float3& param) :x(param.x), y(param.y), z(param.z) { }
		Float3& operator=(Float3&& param)noexcept { x = param.x, y = param.y, z = param.z; return *this; }
		Float3& operator=(const Float3& param) { x = param.x, y = param.y, z = param.z; return *this; }
		Float3(float _x, float _y, float _z)noexcept { x = _x, y = _y, z = _z; }
		const float& operator[](const size_t _i)const { if (_i == 0) return x; if (_i == 1)return y; return z; }
		constexpr float operator[](const size_t _i) { if (_i == 0) return x; if (_i == 1)return y; return z; }
		bool operator==(const Float3& comp) { return (this->x == comp.x && this->y == comp.y && this->z == comp.z); }
		bool operator!=(const Float3& comp) { return !Float3::operator==(comp); }
		Float3 operator+(const Float3& right)const { return Float3(this->x + right.x, this->y + right.y, this->z + right.z); }
		Float3 operator-(const Float3& right)const { return Float3(this->x - right.x, this->y - right.y, this->z - right.z); }
		Float3 operator*(const float& right)const { return Float3(x * right, y * right, z * right); }
		//For output operations
		friend std::ostream& operator<<(std::ostream& os, const Float3& obj)
		{
			os << obj.x << ", " << obj.y << ", " << obj.z;
			return os;
		}
	}float3;

	typedef struct Float4
	{
		//Variables
		float x;
		float y;
		float z;
		float w;
		Float4()noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		Float4(const Float4& param) : x(param.x), y(param.y), z(param.z), w(param.w) { }
		Float4& operator=(Float4& param)noexcept { if (!(&param != this)) { x = param.x, y = param.y, z = param.z, w = param.w; } return *this; }
		Float4& operator=(Float4&& param)noexcept { x = std::move(param.x), y = std::move(param.y), z = std::move(param.z), w = std::move(param.w); return *this; }
		Float4(float _x, float _y, float _z, float _w)noexcept :x(_x), y(_y), z(_z), w(_w) { }
		constexpr float operator[](const size_t _i) const { if (_i == 0) return x; if (_i == 1)return y; if (_i == 2) return z; return w; }
		bool operator==(const Float4& comp)const { return (this->x == comp.x && this->y == comp.y && this->z == comp.z && this->w == comp.w); }
		bool operator!=(const Float4& comp)const { return !(Float4::operator==(comp)); }
		Float4 operator+(const Float4& right)const { return Float4(this->x + right.x, this->y + right.y, this->z + right.z, this->w + right.w); }
		Float4 operator-(const Float4& right)const { return Float4(this->x - right.x, this->y - right.y, this->z - right.z, this->w - right.w); }


	}float4;

	//VERTEX ==============================================================================================================================
	typedef struct Vertex
	{
		float3 pos;
		float2 uv;
		float3 normal;
		Vertex()noexcept : pos(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f) { }
		Vertex(const float3& _pos, const float3& _normal, const float2& _uv)
			: pos(_pos), uv(_uv), normal(_normal) {}
		Vertex& operator=(const Vertex& copy) { if (this != &copy) { pos = copy.pos; normal = copy.normal; uv = copy.uv; } return *this; }
		bool operator==(const Vertex& comp) { return (this->pos == comp.pos && pos == comp.pos && uv == comp.uv); }
		bool operator!=(const Vertex& comp) { return !(this == &comp); }
	}vertex;
	//VECTOR FORMULAS ====================================================================================================================
	inline
		const Float3 cross(const Float3 _AB, const Float3 _AC)noexcept
	{
		return Float3(_AB.y * _AC.z - _AB.z * _AC.y,
			_AB.x * _AC.z - _AB.z * _AC.x,
			_AB.x * _AC.y - _AB.y * _AC.x);
	}
	//MESH ==============================================================================================================================
	class Mesh
	{
	public:
		std::string name;
		std::vector<dxh::Vertex> vertices;
		std::vector<UINT> indices;
		Mesh() { name = "unnamed"; }
		Mesh(std::vector<dxh::Vertex> _vtxs, std::vector<UINT> _indices, std::string _name = "unnamed")
			: vertices(_vtxs), indices(_indices), name(_name) {}

		Mesh(const Mesh& copy) :name(copy.name), vertices(copy.vertices), indices(copy.indices) { }
		~Mesh() {}
		const std::vector<dxh::Vertex>& GetVertices() {
			return vertices;
		}
		Mesh& operator=(const Mesh& right) {
			if (&right != this) {
				name = right.name;
				vertices = right.vertices;
				indices = right.indices;
			}
			return *this;
		}
		const std::size_t ByteWidth() const { return sizeof(Vertex) * this->vertices.size(); } //returns the size of the vertex array in bytes for 
	};

	//CONSTANT BUFFER OBJECT TYPES============================================================================================================
	_declspec(align(16))
		struct WVP
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 project;
	};

	_declspec(align(16))
		struct SimpleLight
	{
		float4 pos;
		float4 color;
		float3 viewpos;
		float filler;
		//float intensity;
		SimpleLight() { filler = 0.0f; }
	};

	_declspec(align(16))
		struct SimpleMaterial
	{
		float4 ambi_col;
		float4 diff_col;
		float4 spec_col;
		float spec_factor;
		float3 filler;
		SimpleMaterial() :spec_factor(0.0f) {}
	};
	//For loading with stbi_load
	struct ImageData
	{
		int width;
		int height;
		int channels; //max 4, rgba
		std::vector<unsigned char> data;
		ImageData(int _width = 0, int _height = 0, int _channels = 3) : width(_width), height(_height), channels(_channels) {};
		~ImageData() {};
	};
}