#pragma once

// Precompiled header file for the project.
#include <iostream>
#include <tchar.h>
#include <memory>

#include <algorithm>
#include <functional>
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

// Using DXTK
#include  <SimpleMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

using namespace DirectX;
namespace sm = DirectX::SimpleMath;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
