#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>アフィン変換行列 </summary>
/// <param name="scale"> </param>
/// <param name="rot"> </param>
/// <param name="translate"></param>
/// <returns> ワールド行列</returns>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

//ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

	// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth);
// 座標変換
Vector3 Transform(const Vector3 vector, const Matrix4x4 matrix);


//加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
// 内積
float Dot(const Vector3& v1, const Vector3& v2);
// 長さ(ノルム)
float Length(const Vector3& v);
// 正規化
Vector3 Normalize(const Vector3& v);
// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);