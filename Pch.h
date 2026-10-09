/*
 * FileName:     Pch.h
 * Author:       Takao Hayata
 * Last Updated: 2026/09/28
 * 
 * プリコンパイル済みヘッダファイル
 */

#pragma once

// 型チェックを厳密に
#define STRICT
// あまり使われない関数を省く
#define WIN32_LEAN_AND_MEAN

// WindowsAPI
#include <Windows.h>
// ComPtr
#include <wrl/client.h>

// Direct3D 11
#include <d3d11_4.h>
// DXGI
#include <dxgi1_6.h>
// Direct3D Compiler
#include <d3dcompiler.h>
// Direct2D
#include <initguid.h>
#include <d2d1_3.h>
#include <d2d1effects.h>
// DirectWrite
#include <dwrite_3.h>

// コンセプト
#include <concepts>
// アルゴリズム
#include <algorithm>
// ファイルシステム
#include <filesystem>
// 文字列
#include <string>
// ベクトル
#include <vector>
// 配列
#include <array>
// 辞書
#include <unordered_map>
// 集合
#include <unordered_set>
// 型インデックス
#include <typeindex>
// エラー処理
#include <exception>
// ファイルストリーム
#include <fstream>
// 文字列ストリーム
#include <sstream>

// Json
#include "nlohmann/json.hpp"
// 列挙型
#include "magic_enum/magic_enum.hpp"

// DirectXTK
#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#include "Audio.h"

// TがUを継承しているかどうか
template<typename T, typename U>
concept IsDerived = std::derived_from<T, U> && !std::same_as<T, U>;

// TがArgsのどれかと型が一致しているか
template<typename T, typename... Args>
concept IsSame = (std::same_as<T, Args> || ...);


// 便利関数群
#include "Scripts/Commons/Libraries/Utility.h"
// 乱数
#include "Scripts/Commons/Libraries/Random.h"
// 数学関係
#include "Scripts/Commons/Libraries/Math/Math.h"
#include "Scripts/Commons/Libraries/Math/Vector2Int.h"
#include "Scripts/Commons/Libraries/Math/Vector3Int.h"
#include "Scripts/Commons/Libraries/Math/Rect.h"
#include "Scripts/Commons/Libraries/Math/NonRotatingBox.h"
#include "Scripts/Commons/Libraries/Math/Sphere.h"
#include "Scripts/Commons/Libraries/Math/Circle.h"
#include "Scripts/Commons/Libraries/Math/Line.h"
#include "Scripts/Commons/Libraries/Math/Triangle.h"
#include "Scripts/Commons/Libraries/Math/Box.h"
#include "Scripts/Commons/Libraries/Math/Euler.h"
#include "Scripts/Commons/Libraries/Math/Geometry.h"
// カメラ
#include "Scripts/Commons/Libraries/Camera/QuaternionCamera.h"
#include "Scripts/Commons/Libraries/Camera/QuaternionTargetCamera.h"
#include "Scripts/Commons/Libraries/Camera/EulerCamera.h"
#include "Scripts/Commons/Libraries/Camera/EulerTargetCamera.h"
// 範囲値
#include "Scripts/Commons/Libraries/Limited.h"
// ループ値
#include "Scripts/Commons/Libraries/Cycled.h"
// イージング
#include "Scripts/Commons/Libraries/Easing.h"
// ばね
#include "Scripts/Commons/Libraries/Spring.h"
// メッシュ
#include "Scripts/Commons/Libraries/Mesh.h"
using namespace Libraries;

namespace GameObjects {}
using namespace GameObjects;

namespace Components {}
using namespace Components;
