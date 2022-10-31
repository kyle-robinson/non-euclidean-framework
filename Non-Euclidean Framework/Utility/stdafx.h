#pragma once
#ifndef STDAFX_H
#define STDAFX_H

#include <memory>
#include <algorithm>

// Data Structures
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <map>

// Windows API
#include <Windows.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#define _XM_NO_INTRINSICS_
using namespace DirectX;

// Classes
#include "ErrorLogger.h"

#endif