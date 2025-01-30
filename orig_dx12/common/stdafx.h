#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <comdef.h>
#include <wrl.h>

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <array>
#include <chrono>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <memory>
#include <numbers>
#include <span>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "Error.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX; // for XMFLOAT3 etc.
