#include "pch.h"
#include "Shaders.h"

void VertexShader::Release()
{
	COM_RELEASE(Shader);
	COM_RELEASE(InputLayout);
	COM_RELEASE(Blob);
}

void PixelShader::Release()
{
	COM_RELEASE(Shader);
	COM_RELEASE(Blob);
}