#pragma once

struct TriangleVertex
{
	sm::Vector3 Position;
};

struct SimpleVertex
{
	sm::Vector3 Pos;
	sm::Vector3 Normal;
	sm::Vector2 TexCoord;
};

// Buffer of data used to set the WVP matrices in the vertex shader
struct VSConstantBuffer
{
	sm::Matrix World;
	sm::Matrix View;
	sm::Matrix Projection;
	sm::Color OutputColor;
};