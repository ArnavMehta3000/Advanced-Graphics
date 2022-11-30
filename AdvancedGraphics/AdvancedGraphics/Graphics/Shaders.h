#pragma once
#include "Core/Core.h"

struct VertexShader
{
	VertexShader() {}
	~VertexShader() { this->Release(); }
	LPCWSTR Name = L"Vertex Shader";

	ComPtr<ID3D11VertexShader> Shader;
	ComPtr<ID3D11InputLayout> InputLayout;
	ComPtr<ID3DBlob> Blob;

	void Release();
};

struct PixelShader
{
	PixelShader() {}
	~PixelShader() { this->Release(); }

	LPCWSTR Name = L"PixelShader";
	ComPtr<ID3D11PixelShader> Shader;
	ComPtr<ID3DBlob> Blob;

	void Release();
};

struct Shader
{
	Shader();
	Shader(LPCWSTR vsFile, LPCWSTR psFile, LPCSTR vsEntry = "VS", LPCSTR psEntry = "PS");
	~Shader();

	void BindVS();
	void BindPS();
	void BindShader();

	VertexShader* m_VertexShader;
	PixelShader* m_PixelShader;
};