#pragma once
#include "Scene.h"

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class gameOverScene {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	gameOverScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~gameOverScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	bool IsSceneEnd() { return isSceneEnd; }
	void SetIsSceneEnd() {  isSceneEnd = false; }
	SceneType NextScene() { return SceneType::kTitle; }

private: // メンバ変数
	bool isSceneEnd = false;
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// タイトル画面のスプライト
	Sprite* spriteTitle_ = nullptr;
};
