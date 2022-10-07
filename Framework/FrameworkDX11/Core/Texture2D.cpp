#include "pch.h"
#include "Texture2D.h"
#include "Defines.h"

void Texture2D::Release()
{
	COM_RELEASE(ResourceView);
}