#include "GameScene.h"
#include "AxisIndicator.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "Skydome.h"
#include "RailCamera.h"
#include "Player.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	// delete sprite_;
	delete model_;
	delete player_;
	delete enemy_;
	delete modelSkydome_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("Reji.png");
	enemytextureHandle_ = TextureManager::Load("kamata.png");
	//3Dモデルの生成
	model_ = Model::Create();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//viewProjection_.farZ = 0.5;

	//レールカメラの生成
	railcamera_ = new RailCamera();
	//レールカメラ初期化
	railcamera_->Initialize();
	
	//自キャラの生成
	player_ = new Player();
	//自キャラの初期化
	Vector3 playerPosition(0, 0, 10);
	player_->Initialize(model_, textureHandle_, playerPosition);
	//自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railcamera_->GetWorldTransform());

	//敵キャラの生成
	enemy_ = new Enemy();
	//敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
	//敵キャラの初期化
	enemy_->Initialize(model_, enemytextureHandle_);

	//デバックカメラの生成
	//debugCamera_ = new DebugCamera(1280, 720);

	//3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	//天球の生成
	skydome_ = new Skydome();
	//天球初期化
	skydome_->Initialize(modelSkydome_);


	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {

	viewProjection_.UpdateMatrix();



	//自キャラの更新
	player_->Update();

	//敵キャラの更新
	enemy_->Update();

	//天球
	skydome_->Update();


	//当たり判定
	CheckAllCollisions();
	//レールカメラ
	railcamera_->Update();
	
	viewProjection_.matView = railcamera_->GetViewProjection().matView;
	viewProjection_.matProjection = railcamera_->GetViewProjection().matProjection;

	viewProjection_.TransferMatrix();

	// キャラクターの移動ベクトル
	// Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	// const float kCharacterSpeed = 0.2f;

#ifdef _DEBUG

	//// 押した方向で移動ベクトルを変更(左右)
	// if (input_->TriggerKey(DIK_LEFT)) {
	//	move.x -= kCharacterSpeed;
	// } else if (input_->TriggerKey(DIK_LIGHT)) {
	//	move.x += kCharacterSpeed;
	// }

	//// 押した方向で移動ベクトルを変更(上下)
	// if (input_->TriggerKey(DIK_UP)) {
	//	move.y += kCharacterSpeed;
	// } else if (input_->TriggerKey(DIK_DOWN)) {
	//	move.y -= kCharacterSpeed;
	// }

#endif
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	// sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	//自キャラの描画
	player_->Draw(viewProjection_);
	//敵キャラの描画
	enemy_->Draw(viewProjection_);
	//天球の描画
	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion
	// ラインを描画する
	// PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	// 自キャラと敵弾の当たり判定
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		
		// 敵弾の座標
		posB = bullet->GetWorldPosition();

		float radius = 0.6f;
		float dx = (posB.x - posA.x) * (posB.x - posA.x);
		float dy = (posB.y - posA.y) * (posB.y - posA.y);
		float dz = (posB.z - posA.z) * (posB.z - posA.z);
		float distance = dx + dy + dz;
		if (distance <= (radius + radius) * (radius + radius)){
			player_->OnCollision();
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	posA = enemy_->GetWorldPosition();
	//自弾と敵キャラの当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		
		posB = bullet->GetWorldPosition();
	
		float radius = 0.6f;
		float dx = (posB.x - posA.x) * (posB.x - posA.x);
		float dy = (posB.y - posA.y) * (posB.y - posA.y);
		float dz = (posB.z - posA.z) * (posB.z - posA.z);
		float distance = dx + dy + dz;
		if (distance <= (radius + radius) * (radius + radius)) {
			enemy_->OnCollision();
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	// 自弾と敵弾の当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		posB = bullet->GetWorldPosition();

		for (EnemyBullet* enemybullet : enemyBullets) {
			
			posA = enemybullet->GetWorldPosition();
	
			float radius = 0.6f;
			float dx = (posB.x - posA.x) * (posB.x - posA.x);
			float dy = (posB.y - posA.y) * (posB.y - posA.y);
			float dz = (posB.z - posA.z) * (posB.z - posA.z);
			float distance = dx + dy + dz;
			if (distance <= (radius + radius) * (radius + radius)) {
				bullet->OnCollision();
				enemybullet->OnCollision();
			}
		}
	}
#pragma endregion
}
