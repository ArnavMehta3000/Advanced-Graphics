#pragma once
#include "Core/Structures.h"

namespace Primitives
{
	class Triangle
	{
	public:
		const static inline SimpleVertex Vertices[] =
		{
			{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },  // Left
			{ XMFLOAT3(0.5f, -0.5f, 0.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },  // Right
			{ XMFLOAT3(0.0f,  0.5f, 0.0f),  XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.5f, 0.5f) },  // Top
		};

		const static inline WORD Indices[] =
		{
			2, 1, 0
		};

		const static inline UINT VerticesTypeSize = sizeof(SimpleVertex);
		const static inline UINT VerticesCount = ARRAYSIZE(Vertices);
		const static inline UINT VerticesByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(Vertices);

		const static inline UINT IndicesTypeSize = sizeof(WORD);
		const static inline UINT IndicesCount = ARRAYSIZE(Indices);
		const static inline UINT IndicesByteWidth = sizeof(WORD) * ARRAYSIZE(Indices);
	};

	// Cube vertices taken from MSDN: Defining a simple cube
	// https://learn.microsoft.com/en-us/windows/win32/direct3d9/defining-a-simple-cube
	class Cube
	{
	public:
		const static inline SimpleVertex Vertices[] =
		{
			{ XMFLOAT3( 1.0f, 1.0f,-1.0f), XMFLOAT3( 0.333333f,  0.666667f, -0.666667f), XMFLOAT2(0.0f, 1.0f) },  // 6 - 1
			{ XMFLOAT3(-1.0f, 1.0f,-1.0f), XMFLOAT3(-0.816497f,  0.408248f, -0.408248f), XMFLOAT2(1.0f, 1.0f) },  // 2 - 2
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-0.333333f,  0.666667f,  0.666667f), XMFLOAT2(0.0f, 1.0f) },  // 3 - 3
			{ XMFLOAT3( 1.0f, 1.0f, 1.0f), XMFLOAT3( 0.816497f,  0.408248f,  0.408248f), XMFLOAT2(1.0f, 1.0f) },  // 7 - 4
			{ XMFLOAT3( 1.0f,-1.0f,-1.0f), XMFLOAT3( 0.666667f, -0.666667f, -0.333333f), XMFLOAT2(0.0f, 0.0f) },  // 4 - 5
			{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT3(-0.408248f, -0.408248f, -0.816497f), XMFLOAT2(1.0f, 0.0f) },  // 0 - 6
			{ XMFLOAT3(-1.0f,-1.0f, 1.0f), XMFLOAT3(-0.666667f, -0.666667f,  0.333333f), XMFLOAT2(0.0f, 0.0f) },  // 1 - 7
			{ XMFLOAT3( 1.0f,-1.0f, 1.0f), XMFLOAT3( 0.408248f, -0.408248f,  0.816497f), XMFLOAT2(1.0f, 0.0f) },  // 5 - 8
		};



		const static inline WORD Indices[] =
		{
			0,1,2,
			0,2,3,
			0,4,5,
			0,5,1,
			1,5,6,
			1,6,2,
			2,6,7,
			2,7,3,
			3,7,4,
			3,4,0,
			4,7,6,
			4,6,5
		};

		const static inline UINT VerticesTypeSize = sizeof(SimpleVertex);
		const static inline UINT VerticesCount = ARRAYSIZE(Vertices);
		const static inline UINT VerticesByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(Vertices);

		const static inline UINT IndicesTypeSize = sizeof(WORD);
		const static inline UINT IndicesCount = ARRAYSIZE(Indices);
		const static inline UINT IndicesByteWidth = sizeof(WORD) * ARRAYSIZE(Indices);
	};
}