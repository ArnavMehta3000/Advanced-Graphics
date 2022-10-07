#pragma once

struct Texture2D
{
	Texture2D() {}
	~Texture2D() { this->Release(); }

	ComPtr<ID3D11ShaderResourceView> ResourceView;

	void Release();
};