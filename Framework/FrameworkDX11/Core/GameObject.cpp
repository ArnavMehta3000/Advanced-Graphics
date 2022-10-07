#include "pch.h"
#include "GameObject.h"
#include "Utils/Primitives.h"
#include "structures.h"
#include "Direct3D.h"

GameObject::GameObject()
{
	D3D->CreateVertexBuffer(m_vertexBuffer, sizeof(SimpleVertex) * ARRAYSIZE(Primitives::CubeVertices), Primitives::CubeVertices);
}

GameObject::~GameObject()
{

}

void GameObject::Draw() const noexcept
{
}
