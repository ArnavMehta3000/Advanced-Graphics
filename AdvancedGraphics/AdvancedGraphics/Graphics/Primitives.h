#pragma once
#include "Core/Structures.h"

namespace Primitives
{
	class Triangle
	{
	public:
		const static inline TriangleVertex Vertices[] =
		{
			XMFLOAT3(-0.5f, -0.5f, 0.0f),
			XMFLOAT3(0.5f, -0.5f, 0.0f),
			XMFLOAT3(0.0f,  0.5f, 0.0f),
		};

		const static inline WORD Indices[] =
		{
			2, 1, 0
		};

		const static inline UINT VerticesTypeSize = sizeof(TriangleVertex);
		const static inline UINT VerticesCount = ARRAYSIZE(Vertices);
		const static inline UINT VerticesByteWidth = sizeof(TriangleVertex) * ARRAYSIZE(Vertices);

		const static inline UINT IndicesTypeSize = sizeof(WORD);
		const static inline UINT IndicesCount = ARRAYSIZE(Indices);
		const static inline UINT IndicesByteWidth = sizeof(WORD) * ARRAYSIZE(Indices);
	};

	class Cube
	{
	public:
		const static inline SimpleVertex Vertices[] =
		{
			{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT3(-0.5773f, 0.5773f, 0.5773f),   XMFLOAT2(1.0f, 0.0f) } ,
			{ XMFLOAT3(-1.0f,-1.0f, 1.0f), XMFLOAT3(-0.5773f, 0.5773f, -0.5773f),  XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f,-1.0f), XMFLOAT3(-0.5773f, -0.5773f, 0.5773f),  XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-0.5773f, -0.5773f, -0.5773f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f,-1.0f,-1.0f),  XMFLOAT3(0.5773f, -0.5773f, -0.5773f),  XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f,-1.0f, 1.0f),  XMFLOAT3(0.5773f, 0.5773f, 0.5773f),    XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f,-1.0f),  XMFLOAT3(0.5773f, 0.5773f, -0.5773f),   XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f),  XMFLOAT3(0.5773f, -0.5773f, 0.5773f),   XMFLOAT2(1.0f, 1.0f) }
		};



		const static inline WORD Indices[] =
		{
			// Face 1
			1, 2, 0,// -x
			1,3,2,

			// Face 2
			4,6,5, // +x
			5,6,7,

			// Face 3
			0,5,1, // -y
			0,4,5,

			// Face 4
			2,7,6, // +y
			2,3,7,

			// Face 5
			0,6,4, // -z
			0,2,6,

			// Face 6
			1,7,3, // +z
			1,5,7,
		};

		const static inline UINT VerticesTypeSize = sizeof(SimpleVertex);
		const static inline UINT VerticesCount = ARRAYSIZE(Vertices);
		const static inline UINT VerticesByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(Vertices);

		const static inline UINT IndicesTypeSize = sizeof(WORD);
		const static inline UINT IndicesCount = ARRAYSIZE(Indices);
		const static inline UINT IndicesByteWidth = sizeof(WORD) * ARRAYSIZE(Indices);
	};
}