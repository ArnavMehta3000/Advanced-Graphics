#pragma once
#include "IDrawable.h"
#include "structures.h"
#include "Core/Buffers.h"

struct Texture2D;

class GameObject : public IDrawable
{
public:
	GameObject(UINT vertexbyteWidth, const void* vertexData, UINT indexByteWidth, const void* indexData);
	~GameObject();

	void Set();
	void Update(double dt);

	virtual void Draw() const noexcept override;

public:
	MaterialPropertiesConstantBuffer m_materialProps;
	sm::Vector3 m_position;
	sm::Matrix m_world;

private:
 	ConstantBuffer* m_materialCBuffer;
	VertexBuffer*   m_vertexBuffer;
	IndexBuffer*    m_indexBuffer;
	Texture2D*      m_texture;
};