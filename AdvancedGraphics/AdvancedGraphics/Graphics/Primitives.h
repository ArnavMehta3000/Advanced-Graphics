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
			{ XMFLOAT3(1.0f,1.0f,-1.0f), XMFLOAT3(-0.5773f, 0.5773f, 0.5773f),   XMFLOAT2(1.0f, 0.0f) } ,
			{ XMFLOAT3(-1.0f,1.0f, -1.0f), XMFLOAT3(-0.5773f, 0.5773f, -0.5773f),  XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f,1.0f), XMFLOAT3(-0.5773f, -0.5773f, 0.5773f),  XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(-0.5773f, -0.5773f, -0.5773f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f,-1.0f,-1.0f),  XMFLOAT3(0.5773f, -0.5773f, -0.5773f),  XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f,-1.0f, -1.0f),  XMFLOAT3(0.5773f, 0.5773f, 0.5773f),    XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f,1.0f),  XMFLOAT3(0.5773f, 0.5773f, -0.5773f),   XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f),  XMFLOAT3(0.5773f, -0.5773f, 0.5773f),   XMFLOAT2(1.0f, 1.0f) }
		};



		const static inline WORD Indices[] =
		{
			// Face 1
			0,1,2, // -x
			0,2,3,

			// Face 2
			0,4,5, // +x
			0,5,1,

			// Face 3
			1,5,6, // -y
			1,6,2,

			// Face 4
			2,6,7, // +y
			2,7,3,

			// Face 5
			3,7,4,// -z
			3,4,0,

			// Face 6
			4,7,6, // +z
			4,6,7,
		};

		const static inline UINT VerticesTypeSize = sizeof(SimpleVertex);
		const static inline UINT VerticesCount = ARRAYSIZE(Vertices);
		const static inline UINT VerticesByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(Vertices);

		const static inline UINT IndicesTypeSize = sizeof(WORD);
		const static inline UINT IndicesCount = ARRAYSIZE(Indices);
		const static inline UINT IndicesByteWidth = sizeof(WORD) * ARRAYSIZE(Indices);
	};
}