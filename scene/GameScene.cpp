#include "GameScene.h"
#include "AxisIndicator.h"
#include "ImGuiManager.h"
#include "PlayerBullet.h"
#include "PrimitiveDrawer.h"
#include "Skydome.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete skydome_;

}

void GameScene::Initialize() {
	deathNum = 0;
	isActive = false;
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("Reji.png");
	enemytextureHandle_ = TextureManager::Load("kamata.png");
	LoadEnemyPopData();

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	// viewProjection_.farZ = 0.5;

	// レールカメラの生成
	railcamera_ = new RailCamera();
	// レールカメラ初期化
	railcamera_->Initialize();

	// 自キャラの生成
	player_ = new Player();
	player_->SetParent(&railcamera_->GetWorldTransform());

	
	// 弾の処理
	playerBullets_.push_back (new PlayerBullet);



	// 自キャラの初期化
	Vector3 playerPosition(0, 0, 10);
	player_->Initialize(model_, textureHandle_, playerPosition);

	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railcamera_->GetWorldTransform());
	// 自弾とカメラオブジェクトの親子関係を結ぶ

	// デバックカメラの生成
	// debugCamera_ = new DebugCamera(1280, 720);

	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球初期化
	skydome_->Initialize(modelSkydome_);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	//レティクルのテクスチャ
	TextureManager::Load("./Resources/reticle.png");
}

void GameScene::Update() {

// release時は実行されない
#ifdef _DEBUG

	// デバッグ用
	if (input_->TriggerKey(DIK_1)) {
		// シーン終了フラグをオン
		isSceneEnd = true;
	}
#endif // DEBUG


	// 敵の出現するタイミングと座標
	UpdateEnemyPopCommands();
	// レールカメラ
	railcamera_->Update();

	viewProjection_.matView = railcamera_->GetViewProjection().matView;
	viewProjection_.matProjection = railcamera_->GetViewProjection().matProjection;

	viewProjection_.TransferMatrix();

	// 自キャラの更新
	player_->Update(viewProjection_);

	// 敵キャラの更新
	for (Enemy* enemy : enemy_) {
		enemy->Update();
	}
	// エネミーが死んでたら削除する
	enemy_.remove_if([](Enemy * enemy){
		// ここに敵が死んでいるか確認する
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	for (EnemyBullet* enemyBullet : enemyBullets_) {
		enemyBullet->Update();
	}
	// エネミー弾が死んでたら削除する
	enemy_.remove_if([](Enemy* enemybullets) {
		// ここに敵が死んでいるか確認する
		if (enemybullets->IsDead()) {
			delete enemybullets;
			return true;
		}
		return false;
	});


	if (isActive) {
		deathNum++;
		isActive = false;
	}
	// クリア判定
	if (deathNum >= 5) {
		// シーン終了フラグをオン
		isSceneEnd = true;
	}

	// 天球
	skydome_->Update();

	CheckAllCollisions();


#ifdef _DEBUG

	

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
	
	

	// 自キャラの描画
	player_->Draw(viewProjection_);
	// 敵キャラの描画
	for (Enemy* enemy : enemy_) {
		enemy->Draw(viewProjection_);
	}
	// 敵キャラの描画
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		enemyBullet->Draw(viewProjection_);
	}
	// 天球の描画
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
	player_->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	// 自キャラと敵弾の当たり判定
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* enemyBullet : enemyBullets_) {

		// 敵弾の座標
		posB = enemyBullet->GetWorldPosition();

		float radius = 0.6f;
		float dx = (posB.x - posA.x) * (posB.x - posA.x);
		float dy = (posB.y - posA.y) * (posB.y - posA.y);
		float dz = (posB.z - posA.z) * (posB.z - posA.z);
		float distance = dx + dy + dz;
		if (distance <= (radius + radius) * (radius + radius)) {
			// 今は即死なのでここでシーン終了フラグをオン
			player_->OnCollision();
			enemyBullet->OnCollision();
			isSceneEnd = true;
		}
	}

#pragma endregion

#pragma region 自弾と敵キャラの当たり判定

	// 自弾と敵キャラの当たり判定
	for (PlayerBullet* bullet : playerBullets) {

		posB = bullet->GetWorldPosition();
		for (Enemy* enemy : enemy_) {
			posA = enemy->GetWorldPosition();
			float radius = 0.6f;
			float dx = (posB.x - posA.x) * (posB.x - posA.x);
			float dy = (posB.y - posA.y) * (posB.y - posA.y);
			float dz = (posB.z - posA.z) * (posB.z - posA.z);
			float distance = dx + dy + dz;
			if (distance <= (radius + radius) * (radius + radius)) {
				enemy->OnCollision();
				bullet->OnCollision();
			}
		}
	}


#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	// 自弾と敵弾の当たり判定
	for (EnemyBullet* enemybullet : enemyBullets_) {
		posA = enemybullet->GetWorldPosition();
		for (PlayerBullet* bullet : playerBullets) {
			posB = bullet->GetWorldPosition();

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

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::LoadEnemyPopData() {
	enemyPopCommands_.clear();
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームコピーにコピー
	enemyPopCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::SpawnEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	// 自機の位置をもらう
	enemy->SetPlayer(player_);
	// 初期化
	enemy->Initialize(model_, enemytextureHandle_, pos);
	enemy->SetGameScene(this);
	// リストに登録
	enemy_.push_back(enemy);
}

void GameScene::Reset() {
	LoadEnemyPopData();

	//Enemy* enemy = new Enemy();
	// ここでエネミーのリセット用の関数を呼ぶ
	// やることはisEnemyDeadをtrue(これで削除する)
	for (Enemy* enemy : enemy_) {
		// 生きているenemyを殺す
		enemy->Dead(); // エネミーのメンバ変数isDeadがtrueになる
	}
	
	// エネミーのisDeadがtrueなら削除する
	enemy_.remove_if([](Enemy* enemy) {
		// ここに敵が死んでいるか確認する
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
	
	// 敵撃破カウントを0にする
	deathNum = 0;

	// 後はエネミーと同じように敵の弾も処理する
	for (EnemyBullet*enemybullet : enemyBullets_) {
		enemybullet->BulletDead();// 弾のメンバ変数isDeadがtrueになる
	}

	enemyBullets_.remove_if([](EnemyBullet* enemybullets) {
		if (enemybullets->IsDead()) {
			delete enemybullets;
			return true;
		}
		return false;
	});

}

void GameScene::UpdateEnemyPopCommands() {
	// 待機処理
	if (isWait_) {
		waitTime_--;
		if (waitTime_ <= 0) {
			// 待機完了
			isWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる関数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands_, line)) {
		// 1行分の文字列をストリームに変換して解説しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		//"//"から始まる行は飛ばす
		if (word.find("//") == 0) {
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SpawnEnemy(Vector3(x, y, z));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待ち時間
			isWait_ = true;
			waitTime_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}
