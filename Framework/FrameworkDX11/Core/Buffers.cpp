#include "pch.h"
#include "Buffers.h"
#include "Defines.h"

void VertexBuffer::Release()
{
	COM_RELEASE(Buffer);
}

void IndexBuffer::Release()
{
	COM_RELEASE(Buffer);
}
