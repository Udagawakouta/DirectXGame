#include "RailCamera.h"
#include "ViewProjection.h"
#include "ImGuiManager.h"
#include "MyMath.h"

//初期化
void RailCamera::Initialize() {

	//初期設定
	worldtransform_.translation_.z = -10;
	//ワールドトランスフォームの初期化
	worldtransform_.Initialize();
	
	//ビュープロジェクションの初期化
	//Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	viewProjection_.Initialize();
	viewProjection_.farZ = 1200.0f;
}

//更新
void RailCamera::Update() {
	//ワールドトランス座標の座標の数値の加算
	//worldtransform_.translation_.z += 0.05f;

	//ワールドトランスフォームのワールド行列再計算
	worldtransform_.UpdateMatrix();

	//カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldtransform_.matWorld_);


	////カメラの座標を画面表示する処理
	//ImGui::Begin("Camera");

	//ImGui::SliderFloat3("translation", &worldtransform_.translation_.x, -100, 100);
	//ImGui::SliderFloat3("rotation", &worldtransform_.rotation_.x, -6.28f, 6.28f);

	//ImGui::End();


}
