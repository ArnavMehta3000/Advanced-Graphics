#pragma once
#define _XM_NO_INTRINSICS_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// For checking memory leaks
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <stdlib.h>
#include <crtdbg.h>
#include <malloc.h>  
#endif // _DEBUG


#include <iostream>
#include <tchar.h>
#include <memory>

#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <wrl.h>

#include <SimpleMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

using namespace DirectX;
namespace sm = DirectX::SimpleMath;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;