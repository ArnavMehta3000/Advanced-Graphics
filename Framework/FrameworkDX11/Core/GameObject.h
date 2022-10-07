#pragma once
#include "IDrawable.h"

class GameObject : public IDrawable
{
public:
	GameObject();
	~GameObject();

	virtual void Draw() const noexcept override;

private:

	ComPtr<ID3D11Buffer> m_vertexBuffer;
};