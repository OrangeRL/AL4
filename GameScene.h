#pragma once

#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"
#include "CollisionPrimitive.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

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
	void Initialize(DirectXCommon* dxCommon, Input* input);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	DebugText debugText;

	/// <summary>
	/// ゲームシーン用
	enum class Scene
	{
		Empty,//タイトル
			CirclePlane,
			CircleTriangle,
	};
	Scene scene_ = Scene::Empty;
	/// </summary>
	Sprite* spriteBG = nullptr;
	Object3d* object3d1 = nullptr;
	Object3d* object3d2 = nullptr;
	Object3d* object3d3 = nullptr;

	Model* model = nullptr;
	Model* model1 = nullptr;
	//当たり判定　球
	Sphere sphere;
	//当たり判定　平面
	Plane plane;
	//当たり判定　三角形
	Triangle triangle;
	//当たり判定　レイ
	Ray ray;

	float playerPos[3] = { 1,0.0f,0 };
	float playerScale[3] = { 1.0f,1.0f,1.0f };
	float planePos[3] = { 1.0f,1.0f,1.0f };
	float planeScale[3] = { 0.1f,0.1f,0.1f };
};
