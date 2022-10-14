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

#define SAFE_DELETE(ptr) if (ptr){ delete ptr; ptr = nullptr;}

#define CREATE_ZERO(type, varName) type varName; ZeroMemory(&varName, sizeof(type))

#define CREATE_AND_ATTACH_CONSOLE()\
{\
    AllocConsole();\
    AttachConsole(GetCurrentProcessId());\
    FILE* fp = nullptr;\
    freopen_s(&fp, "CONIN$", "r", stdin);\
    freopen_s(&fp, "CONOUT$", "w", stdout);\
    freopen_s(&fp, "CONOUT$", "w", stderr);\
}

#define LOG(x)\
{\
    std::ostringstream oss;\
    oss << x;\
    std::cout << oss.str() << std::endl;\
}

#define HR(hr)\
{\
    if (FAILED(HRESULT(hr)))\
    {\
        char hrMsg[512]; \
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 0, hrMsg, ARRAYSIZE(hrMsg), nullptr); \
        std::stringstream error; \
        error << "HRESULT Error! " << hrMsg << "File:" << __FILE__ << "\nLine: " << __LINE__ << std::endl; \
        LOG(error.str())\
        __debugbreak(); \
    }\
} 

#define LOG_VEC(v) v.x << ", " << v.y << ". " << v.z