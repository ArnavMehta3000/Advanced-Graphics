#pragma once
#include "IDrawable.h"
#include "Core/Buffers.h"

struct Transform
{
	sm::Vector3 Position;
	sm::Vector3 Rotation;
	sm::Vector3 Scale;
	sm::Matrix WorldMatrix;

	void UpdateMatrix()
	{
		sm::Matrix rot = 
			sm::Matrix::CreateRotationX(XMConvertToRadians(Rotation.x)) * 
			sm::Matrix::CreateRotationY(XMConvertToRadians(Rotation.y)) * 
			sm::Matrix::CreateRotationZ(XMConvertToRadians(Rotation.z));
		WorldMatrix = sm::Matrix::CreateScale(Scale) * rot * sm::Matrix::CreateTranslation(Position);
	}
	
	Transform()
	{
		Position = sm::Vector3::Zero;
		Rotation = sm::Vector3::Zero;
		Scale = sm::Vector3::One;
		UpdateMatrix();
	}

	Transform(const sm::Vector3 pos, const sm::Vector3 rot, const sm::Vector3 scale)
	{
		Position = pos;
		Rotation = rot;
		Scale = scale;
		UpdateMatrix();
	}
	
};

class GameObject : public IDrawable
{
public:
	GameObject(UINT vertexbyteWidth, const void* vertexData, UINT indexByteWidth, const void* indexData);
	~GameObject();

	void Set();
	void Update(double dt);

	virtual void Draw() const noexcept override;

public:
	Transform m_transform;

private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
};