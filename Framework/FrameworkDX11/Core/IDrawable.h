#pragma once
struct ID3D11Device;

class IDrawable
{
public:
	virtual void Draw() const noexcept = 0;
};