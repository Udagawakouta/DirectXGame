#include "Player.h"
#include "ImGuiManager.h"
#include "MyMath.h"
#include <cassert>

Player::~Player() {
	// bullet_の解散
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite2DReticle_;
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
	uint32_t textureReticle = TextureManager::Load("./Resources/reticle.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 320}, {1, 1, 1, 1}, {0.5f, 0.5f});

	

}

void Player::Update(const ViewProjection& viewProjection) {

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 回転速さ
	//const float kRotSpeed = 0.02f;

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 押した方向で移動ベクトルを変更
	/*if (input_->PushKey(DIK_LEFT)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		worldTransform_.rotation_.x += kRotSpeed;
	}*/

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_W)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_S)) {
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

	

	worldTransform_.UpdateMatrix();

	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	float playerPos[] = {
	    worldTransform_.translation_.x,
	    worldTransform_.translation_.y,
	    worldTransform_.translation_.z,
	};
	ImGui::SliderFloat3("PlayerPos", playerPos, 100, 1280);
	ImGui::End();

	// ↑処理のままだとSliderFloat3でplayerPosの値を変えているので実際の座標(translation)が
	// 変わっていいないのでここで変更する
	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];

	// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	{
		POINT mousePosition;
		// マウス座標(スクリーン座標)を取得
		GetCursorPos(&mousePosition);
		/*
		    GetCursorPosで行われていること
		    mousePosition.x = マウス座標X;
		    mousePosition.x = マウス座標Y;
		*/

		// クライアントエリア座標に変換する
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);
		mousePosition.x; // float型にキャストすればVector2のxに変換できる
		sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

		// ビューポート行列
		Matrix4x4 matViewport =
		    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュープロジェクションビューポート合成行列
		Matrix4x4 matVPV =
		    Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

		// 合成行列の逆行列を計算する
		Matrix4x4 matInverseVPV = Inverse(matVPV);

		// スクリーン座標
		Vector3 posNear = Vector3((float)mousePosition.x, (float)mousePosition.y, 0);
		Vector3 posFar = Vector3((float)mousePosition.x, (float)mousePosition.y, 1);

		// スクリーン座標系からワールド座標系へ
		posNear = Transform(posNear, matInverseVPV);
		posFar = Transform(posFar, matInverseVPV);

		// マウスレイの方向
		Vector3 mouseDirection = {
			posFar.x-posNear.x,// x
			posFar.y-posNear.y,// y
			posFar.z-posNear.z//  z
		};

		mouseDirection = Normalize(mouseDirection);

		// カメラから照準オブジェクトの距離
		const float kDistanceTestObject = 100.0f;
		worldTransform3DReticle_.translation_.x = (mouseDirection.x * kDistanceTestObject) + posNear.x;
		worldTransform3DReticle_.translation_.y = (mouseDirection.y * kDistanceTestObject) + posNear.y;
		worldTransform3DReticle_.translation_.z = (mouseDirection.z * kDistanceTestObject) + posNear.z;

		worldTransform3DReticle_.UpdateMatrix();
		
		

		//ImGui::Begin("Player");
		//ImGui::Text("2DReticle:(%f,%f)", sprite2DReticle_->GetPosition().x,sprite2DReticle_->GetPosition().y);
		//ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
		//ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		//ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
		//    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
		//ImGui::End();
	}
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
		const float kBulletSpeed = 2.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		Vector3 worldPos = GetWorldPosition();
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);


		// 自機から照準オブジェクトへのベクトル
		Vector3 worldReticlePos = {
		    worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		    worldTransform3DReticle_.matWorld_.m[3][2]};

		velocity = Subtract(worldReticlePos, worldPos);
		velocity = Normalize(velocity);
		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
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

void Player::DrawUI() { 
	// レティクル描画
	sprite2DReticle_->Draw();
}
