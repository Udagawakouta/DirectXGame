#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "Scene.h"
#include "TextureManager.h"
#include "TitleScene.h"
#include "WinApp.h"
#include "gameOverScene.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	TitleScene* titleScene = nullptr;
	GameScene* gameScene = nullptr;

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"LE2C_03_ウダガワ_コウタ_AL3");

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion

#pragma region 更新処理
	gameScene = new GameScene();
	gameScene->Initialize();

	// タイトルシーンの初期化
	titleScene = new TitleScene();
	titleScene->Initialize();

	SceneType sceneNo = SceneType::kTitle;

	// ゲームオーバー初期化
	gameOverScene* gameoverScene = new gameOverScene();
	gameoverScene->Initialize();


	// 敵の初期

#pragma endregion

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();

		switch (sceneNo) {
		case SceneType::kTitle:

			titleScene->Update();

			if (titleScene->IsSceneEnd()) {
				// 次のシーンの値を代入してシーン切り替え
				sceneNo = titleScene->NextScene();
				titleScene->SetIsSceneEnd();
			}

			break;

		case SceneType::kGamePlay:
			gameScene->Update();
			
			// ここのif文の条件が通ったらシーン以降する、タイトルと同じ様にシーン終了フラグをオンにすればいい
			if (gameScene->IsSceneEnd()) {
				// 次のシーンの値を代入してシーン切り替え
				sceneNo = gameScene->NextScene();
				gameScene->SetIsSceneEnd();

				// ゲームシーンのリセット
				gameScene->Reset();
			}
			break;

		case SceneType::kGameOver:
			gameoverScene->Update();

			if (gameoverScene->IsSceneEnd()) {
				// 次のシーンの値を代入してシーン切り替え
				sceneNo = gameoverScene->NextScene();
				gameoverScene->SetIsSceneEnd();

			
			}
			
			break;
		}

		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

#pragma region 描画処理
		// 描画開始
		dxCommon->PreDraw();

		switch (sceneNo) {
		case SceneType::kTitle:
			// ゲームシーンの描画
			titleScene->Draw();
			break;

		case SceneType::kGamePlay:
			gameScene->Draw();

			break;

		case SceneType::kGameOver:
			gameoverScene->Draw();

			break;
		}

		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
#pragma endregion
	}

	// 各種解放
	SafeDelete(titleScene);
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}