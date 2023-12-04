#include "Enemy.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include "Player.h"
#include <cassert>

Enemy::~Enemy() {
}

void Enemy::Initialize(Model* model, uint32_t textureHandle, Vector3 pos) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	enemytextureHandle_ = textureHandle;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.translation_ = pos;

	// 発射タイマーを初期化
	shotTimer = kFireInterval;

	// 接近フェーズ初期化
	Approach();
}

void Enemy::Update() {

	// キャラクターの移動ベクトル
	Vector3 enemymove = {0, 0, 0};
	// 敵の接近速さ
	const float kApproachSpeed = 0.2f;
	// 敵の離脱速さ
	const float kLeaveSpeed = 0.2f;

	switch (phase_) {
	case Phase::DoApproach:
	default:
		// 移動
		worldTransform_.translation_.z -= kApproachSpeed;

		worldTransform_.UpdateMatrix();
		// 規定の位置に到達したら離脱
		if (worldTransform_.translation_.z < 5.0f) {
			phase_ = Phase::Leave;
		}

		// 敵の攻撃処理
		Approach();

		break;
	case Phase::Leave:
		// 移動
		worldTransform_.translation_.x -= kLeaveSpeed;
		worldTransform_.translation_.y += kLeaveSpeed;

		worldTransform_.UpdateMatrix();
		break;
	}
	if (deathNum>=5) {
		isSceneEnd = true;
	}

#ifdef DEBUG

	ImGui::Begin("Count");

	ImGui::Text("%d",deathNum);

	ImGui::End();

#endif // DEBUG

}

void Enemy::Draw(ViewProjection& viewProjection) {
	if (!isDead_) {
		model_->Draw(worldTransform_, viewProjection, enemytextureHandle_);
	}

	//// 弾描画
	//for (EnemyBullet* bullet : bullets_) {
	//	bullet->Draw(viewProjection);
	//}
}

void Enemy::Fire() {
	assert(player_);

	// 弾の速度
	const float kBulletSpeed = 1.3f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// プレイヤーワールド座標を取得
	Vector3 playerWorldPos = player_->GetWorldPosition();
	// 敵ワールド座標を取得
	Vector3 enemyWorldPos = GetWorldPosition();
	// 敵キャラ→自キャラの差分ベクトルを求める
	Vector3 dirVector = {
	    playerWorldPos.x - enemyWorldPos.x,
	    playerWorldPos.y - enemyWorldPos.y,
	    playerWorldPos.z - enemyWorldPos.z,
	};
	// ベクトルの正規化
	Vector3 resultNormalize = Normalize(dirVector);
	// ベクトルの長さを、早さに合わせる
	velocity = {
	    resultNormalize.x * kBulletSpeed,
	    resultNormalize.y * kBulletSpeed,
	    resultNormalize.z * kBulletSpeed,
	};
	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, GetWorldPosition(), velocity);

	// 弾を登録
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::Approach() {

	// 発射タイマーカウントダウン
	shotTimer--;
	// 指定時間に達した
	if (shotTimer <= 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		shotTimer = kFireInterval;
	}
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}


void Enemy::OnCollision() {
	bool isActive = true;
	if (isActive) {
		deathNum++;
		isActive = false;

	}

	isDead_ = true; 
	if (isBomb_){
		isDead_ = false;
		TextureManager::Load("FIRE.png");
	}

}

int Enemy::deathNum;

void Enemy::Reset(Vector3 pos) {
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.translation_ = pos;

	// 発射タイマーを初期化
	shotTimer = kFireInterval;

	// 接近フェーズ初期化
	Approach();
}

void Enemy::Dead() { isDead_ = true; }
