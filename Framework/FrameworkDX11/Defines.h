#pragma once

template <class T>
void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

#define COM_RELEASE(x) SafeRelease(x.GetAddressOf())

#define HR(x) if (FAILED(x)) __debugbreak()

#define CREATE_ZERO(type, varName) type varName; ZeroMemory(&varName, sizeof(type))