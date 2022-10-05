#pragma once
#define _XM_NO_INTRINSICS_
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <memory>

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

using namespace DirectX;
using Microsoft::WRL::ComPtr;