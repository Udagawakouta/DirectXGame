#include "Player.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include <cassert>

Player::~Player() {
	// bullet_の解散
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initialize(Model* model, uint32_t textureHandle, Vector3 playerPosition) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;

	worldTransform_.translation_ = playerPosition;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// X,Y,Z方向のスケーリングを設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// シングルトンインスタンスを取得
	input_ = Input::GetInstance();

	// X,Y,Z方向の回転を設定
	worldTransform_.translation_ = {0.0f, 0.0f, 20.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// レティクル用のテクスチャ取得
	uint32_t textureReticle = TextureManager::Load("reticle.png");

	//スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 320}, {1, 1, 1, 1}, {0.5f, 0.5f});
}

void Player::Update() {

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 回転速さ
	const float kRotSpeed = 0.02f;

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.x += kRotSpeed;
	}

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	// キャラクター攻撃処理
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// キーボード入力による移動距離

	// 移動限界座標座標
	const float kMoveLimitX = 35.0f;
	const float kmoveLimitY = 18.0f;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kmoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kmoveLimitY);

	// 座標移動(ベクトルの加算)
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	////スケーリング行列を宣言
	// Matrix4x4 matScale = {0};

	// matScale.m[0][0] = worldTransform_.scale_.x;
	// matScale.m[1][1] = worldTransform_.scale_.y;
	// matScale.m[2][2] = worldTransform_.scale_.z;
	// matScale.m[3][3] = 1;

	////X軸回転行列を宣言
	// Matrix4x4 matRotX = {0};
	// matRotX.m[0][0] = 1;
	// matRotX.m[1][1] = cosf(worldTransform_.rotation_.x);
	// matRotX.m[2][1] = -sinf(worldTransform_.rotation_.x);
	// matRotX.m[1][2] = sinf(worldTransform_.rotation_.x);
	// matRotX.m[2][2] = cosf(worldTransform_.rotation_.x);
	// matRotX.m[3][3] = 1;
	//
	////Y軸回転行列を宣言
	// Matrix4x4 matRotY = {0};
	// matRotY.m[0][0] = cosf(worldTransform_.rotation_.y);
	// matRotY.m[1][1] = 1;
	// matRotY.m[0][2] = -sinf(worldTransform_.rotation_.y);
	// matRotY.m[2][0] = sinf(worldTransform_.rotation_.y);
	// matRotY.m[2][2] = cosf(worldTransform_.rotation_.y);
	// matRotY.m[3][3] = 1;
	//
	////Z軸回転行列を宣言
	// Matrix4x4 matRotZ = {0};
	// matRotZ.m[0][0] = cosf(worldTransform_.rotation_.z);
	// matRotZ.m[1][1] = sinf(worldTransform_.rotation_.z);
	// matRotZ.m[0][1] = -sinf(worldTransform_.rotation_.z);
	// matRotZ.m[1][1] = cosf(worldTransform_.rotation_.z);
	// matRotZ.m[2][2] = 1;
	// matRotZ.m[3][3] = 1;

	// Matrix4x4 matRot = Multiply(Multiply(matRotZ, matRotX), matRotY);
	//
	////平行移動行列を宣言
	// Matrix4x4 matTrans = {0};

	// matTrans.m[0][0] = 1;
	// matTrans.m[1][1] = 1;
	// matTrans.m[2][2] = 1;
	// matTrans.m[3][3] = 1;
	// matTrans.m[3][0] = worldTransform_.translation_.x;
	// matTrans.m[3][1] = worldTransform_.translation_.y;
	// matTrans.m[3][2] = worldTransform_.translation_.z;

	// worldTransform_.matWorld_ = Multiply(Multiply(matScale, matRot), matTrans);

	////行列の転送
	// worldTransform_.TransferMatrix();

	worldTransform_.UpdateMatrix();

	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	float playerPos[] = {
	    worldTransform_.translation_.x,
	    worldTransform_.translation_.y,
	    worldTransform_.translation_.z,
	};
	ImGui::SliderFloat3("PlayerPos", playerPos, 100, 1280);
	// ↑処理のままだとSliderFloat3でplayerPosの値を変えているので実際の座標(translation)が
	// 変わっていいないのでここで変更する
	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];

	// 自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		// 自機から3Dレティクルへの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		// 自機から3Dレティクルへのオフセット(Z+向き)
		Vector3 offset = {0, 0, 1.0f};
		// 自機のワールド行列の回転を反映
		offset = TransformNormal(offset, worldTransform_.matWorld_);
		offset = Normalize(offset);
		// ベクトルの長さを整える
		offset.x *= kDistancePlayerTo3DReticle;
		offset.y *= kDistancePlayerTo3DReticle;
		offset.z *= kDistancePlayerTo3DReticle;
		// 3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_.x = GetWorldPosition().x + offset.x;
		worldTransform3DReticle_.translation_.y = GetWorldPosition().y + offset.y;
		worldTransform3DReticle_.translation_.z = GetWorldPosition().z + offset.z;

		worldTransform3DReticle_.UpdateMatrix();
	}

	ImGui::End();
}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	// 3Dレティクルを描画
	model_->Draw(worldTransform3DReticle_, viewProjection);
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		// 弾があれば解放する
		/*
		if (bullet_) {
		    delete bullet_;
		    bullet_ = nullptr;
		}
		*/

		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		Vector3 worldPos = GetWorldPosition();
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);

		// 自機から照準オブジェクトへのベクトル
		Vector3 worldReticlePos = {
		    worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		    worldTransform3DReticle_.matWorld_.m[3][2]};

		velocity = Subtract(worldReticlePos, worldPos);
		velocity = Normalize(velocity);
		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;
	}
}

void Player::OnCollision() {}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 WorldPos;

	// ワールド行列の平行移動成分を取得
	WorldPos.x = worldTransform_.matWorld_.m[3][0];
	WorldPos.y = worldTransform_.matWorld_.m[3][1];
	WorldPos.z = worldTransform_.matWorld_.m[3][2];
	return WorldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}