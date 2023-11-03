#pragma once
#include "Model.h"
#include "WorldTransform.h"

class EnemyBullet {
public:
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	static const int32_t kLifeTime = 60 * 5;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	void BulletDead();

	bool IsDead() const { return isEnemyDead_; }

	Vector3 GetWorldPosition();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// 速度
	Vector3 velocity_;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isEnemyDead_ = false;
};