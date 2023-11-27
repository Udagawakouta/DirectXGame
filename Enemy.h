﻿#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "EnemyBullet.h"
#include "Scene.h"
#include <list>

class Player;
class GameScene;

///< summary>
/// 敵
///< summary>
class Enemy {
public:
	~Enemy();

	void Initialize(Model* model, uint32_t textureHandle,Vector3 pos);

	void Update();

	void Draw(ViewProjection& viewProjection);

	void Fire();
	//接近フェーズ初期化
	void Approach();

	void SetPlayer(Player* player) { player_ = player; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void Reset(Vector3 pos);

	void Dead();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	//敵ワールド座標を取得
	Vector3 GetWorldPosition();

	bool IsSceneEnd() { return isSceneEnd; }
	void SetIsSceneEnd() { isSceneEnd = false; }
	SceneType NextScene() { return SceneType::kGameClear; }

	static const int32_t kLifeTime = 60 * 5;

	static int deathNum;

	bool IsDead() const { return isDead_; } 


private:
	//発射感覚
	static const int kFireInterval = 120;


	// 行動フェーズ
	enum class Phase {
		DoApproach, // 接近する
		Leave,    // 離脱する
	};

	bool isSceneEnd = false;
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t enemytextureHandle_ = 0u;
	// フェーズ
	Phase phase_ = phase_;
	// キーボード入力
	Input* input_ = nullptr;
	// 弾
	//std::list<EnemyBullet*> bullets_;
	//発射タイマー
	int32_t shotTimer = 0;
	//自キャラ
	Player* player_ = nullptr;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
	// 爆発フラグ
	bool isBomb_ = false;
	//ゲームシーン
	GameScene* gameScene_ = nullptr;


	// 発射タイマーを初期化
	//int approach = 0;
};