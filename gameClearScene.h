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

class gameClearScene {
public: 
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	bool IsSceneEnd() { return isSceneEnd; }
	void SetIsSceneEnd() { isSceneEnd = false; }
	SceneType NextScene() { return SceneType::kTitle; }


private: // メンバ変数
	bool isSceneEnd = false;
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// タイトル画面のスプライト
	Sprite* spriteTitle_ = nullptr;
};
