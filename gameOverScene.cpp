#include "gameOverScene.h"

gameOverScene::gameOverScene() {}

gameOverScene::~gameOverScene() {}

void gameOverScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// タイトルのテクスチャの取得
	uint32_t textureTitle = TextureManager::Load("uvChecker.png");

	// タイトルスプライトの生成
	spriteTitle_ = Sprite::Create(textureTitle, {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});

}

void gameOverScene::Update() {
	if (input_->TriggerKey(DIK_1)) {
		isSceneEnd = true;
	}
}

void gameOverScene::Draw() {

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

	// タイトル描画
	spriteTitle_->Draw();


	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}