#pragma once

#include "WorldTransform.h"
#include "Model.h"

class RailCamera {
public:
	//初期化
	void Initialize();

	//更新
	void Update();
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	// プレイヤーワールド座標を取得
	const WorldTransform& GetWorldTransform() { return worldtransform_; }

private:
	// ワールド変換データ
	WorldTransform worldtransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	// モデル
	Model* model_ = nullptr;
	// 速度
	Vector3 CameraVelocity_;
};