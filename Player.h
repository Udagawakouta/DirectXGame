#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "PlayerBullet.h"
#include <list>

class Player {

public:
	///< summary>
	/// デストラクタ
	///< summary>
	~Player();

	void Initialize(Model* model, uint32_t textureHandle, Vector3 playerPosition);

	void Update();

	void Draw(ViewProjection& viewProjection);

	void Attack();

	void SetParent(const WorldTransform* parent);

	//衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
	//弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	static const int32_t kLifeTime = 60 * 5;

	//プレイヤーワールド座標を取得
	Vector3 GetWorldPosition();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	//キーボード入力
	Input* input_ = nullptr;
	// 弾
	std::list<PlayerBullet*> bullets_;
	// デスフラグ
	bool isEnemyDead_ = false;
};