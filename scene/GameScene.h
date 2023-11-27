#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "RailCamera.h"
#include "SafeDelete.h"
#include "Scene.h"
#include "Skydome.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <list>
#include <sstream>
#include "EnemyBullet.h"

/*
	クラス作るうえで大事なこと
	オブジェクト指向
	1. カプセル化
	2. 継承
	3. ポリモーフィズム

	カプセル化っていうのは
	例えば Playerクラス作るときに
	Playerクラスに使う変数を外で安易に変更できないようにする

	それを実現するためにh(ヘッダー)で
	private:
	public:
	で分けて書く
	// ゲームシーンでプレイヤーの変数からメンバ変数のxにアクセスして変更できるようにしてはいけない
	player.x = 100;

	なんでしてはいけないか
	クラス作る理由、処理がまとまって見やすくなる
	プログラム書くときに誰がどこに書いたかっていうのが大事になる
	様は責任を持たしたい、クラス名にそった処理をすべき

	じゃあ、クラスにないある変数の値が欲しかったりする場合あるよね
	例えば当たり判定の時は座標がほしい
	死んでるかどうか判定するために死亡フラグの値が欲しかったりするよね
	それらを解決するためにアクセッサを使う

	ゲッター,メンバ変数の値を戻り値(return)するだけの関数
	int IsSceenEnd() { return isSceneEnd; } // フラグ系はIs付ける
	Vector2 GetPositon() { return positno; }

	セッター , void型かつ引数ありの関数
	引数から受け取った値をメンバ変数に代入する
	外部でどうしてもメンバ変数の値を変更したいときに使う
	void SetPositon(Vector2 pos) {
		position.x = pos.x;
		position.y = pos.y;
	}
*/
// 定数みたいなもの
#define DESTORT_ENEMY_MAX 3

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SetIsSceneEnd() { isSceneEnd = false; }
	bool IsSceneEnd() { return isSceneEnd; }
	SceneType GameOverScene() { return SceneType::kGameOver; }
	SceneType GameClearScene() { return SceneType::kGameClear; }
	SceneType NextScene() {
		// やり方
		// 1. if文使ってクリアかオーバーを戻り値にする
		// 2.
		// SceneTypeをメンバ変数として持ってそれを戻り値にする、gamesceneのUpdateとかでクリア時にその変数にSceneType::kGameClearを代入する
		if (deathNum >= DESTORT_ENEMY_MAX) {
			return SceneType::kGameClear;
		}
		return SceneType::kGameOver;
	}

	void CheckAllCollisions();

	/// <summary>
	/// デストラクタ
	/// </summary>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵の発生処理
	/// </summary>
	/// <param name="pos"></param>
	void SpawnEnemy(Vector3 pos);

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return enemyBullets_; }

	void Reset();

private: // メンバ変数
	bool isSceneEnd = false;

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	uint32_t enemytextureHandle_ = 0;
	// uint32_t skydometextureHandle_ = 0;
	//  自キャラ
	Player* player_ = nullptr;

	// 自弾
	std::list<PlayerBullet*> playerBullets_;

	// 敵キャラ
	std::list<Enemy*> enemy_;

	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;

	// 敵発生コマンド
	std::stringstream enemyPopCommands_;

	// 天球
	Skydome* skydome_ = nullptr;

	// レールカメラ
	RailCamera* railcamera_ = nullptr;

	// デバックカメラ有効
	bool isDebugCameraActive_ = false;

	// デバックカメラ
	DebugCamera* debugCamera_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	// 3Dモデルスカイドーム
	Model* modelSkydome_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 敵が発生待機中か
	bool isWait_ = false;
	// 敵が発生するまでの時間
	int32_t waitTime_ = 0;

	int deathNum;

	bool isActive;
};
