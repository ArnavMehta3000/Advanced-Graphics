#pragma once


struct VertexBuffer
{
	VertexBuffer() {}
	~VertexBuffer() { this->Release(); }

	ComPtr<ID3D11Buffer> Buffer;
	UINT Stride = 0;
	UINT Offset = 0;

	void Release();
};

struct IndexBuffer
{
	IndexBuffer() {}
	~IndexBuffer() { this->Release(); }

	ComPtr<ID3D11Buffer> Buffer;

	void Release();
};