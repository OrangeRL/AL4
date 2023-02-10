#include "GameScene.h"
#include "Collision.h"
#include <cassert>
#include <sstream>
#include <iomanip>
#include<imgui.h>

using namespace DirectX;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteBG;
	delete object3d1;
	delete object3d2;
	delete model;
	delete model1;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });

	//OBJからモデルデータを読み込む
	model = Model::LoadFromOBJ("ground");
	model1 = Model::LoadFromOBJ("chr_sword");
	model2 = Model::LoadFromOBJ("triangle");
	// 3Dオブジェクト生成
	object3d1 = Object3d::Create();
	object3d2 = Object3d::Create();
	object3d3 = Object3d::Create();
	//3Dオブジェクトと3Dモデルを紐づけ
	object3d1->SetModel(model);
	object3d2->SetModel(model1);
	object3d3->SetModel(model2);

	object3d1->SetScale(XMFLOAT3(planeScale));
	object3d1->SetPosition(XMFLOAT3(planePos));
	object3d2->SetScale(XMFLOAT3(playerScale));
	object3d2->SetPosition(XMFLOAT3(playerPos));
	object3d3->SetPosition({ 0.0f, 1.0f, 0.0f });
	object3d3->SetRotation({ 1.0f, 0.0f, 0.0f });
	object3d3->SetScale(XMFLOAT3(triangleScale));

	//球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1);//中心座標
	sphere.radius = 1.0f;//半径
	//平面の初期化を設定
	plane.normal = XMVectorSet(0, 1, 0, 0);//法線ベクトル
	plane.distance = 0.0f;//原点(0,0,0)からの距離
	//三角形の初期値を設定
	triangle.p0 = XMVectorSet(-1.0f, 0, -1.0f, 1);//左手前
	triangle.p1 = XMVectorSet(-1.0f, 0, +1.0f, 1);//左奥
	triangle.p2 = XMVectorSet(+1.0f, 0, -1.0f, 1);//右手前
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 1);//上向き
	//レイの初期値を設定
	ray.start = XMVectorSet(0, 1, 0, 1);//原点やや下
	ray.dir = XMVectorSet(0, -1, 0, 0);//下向き
}

void GameScene::Update()
{
	std::ostringstream spherestr;
	bool hit;
	switch (scene_) {
	case GameScene::Scene::Empty:
		break;
	case GameScene::Scene::CirclePlane:

		object3d1->SetScale(XMFLOAT3(planeScale));
		object3d1->SetPosition(XMFLOAT3(planePos));
		object3d2->SetScale(XMFLOAT3(playerScale));
		object3d2->SetPosition(XMFLOAT3(playerPos));

		XMFLOAT4 color = object3d2->GetColor();
		// オブジェクト移動
		if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
		{
			// 球移動
			XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);
			if (input->PushKey(DIK_W)) { sphere.center += moveY; }
			else if (input->PushKey(DIK_S)) { sphere.center -= moveY; }
			XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
			if (input->PushKey(DIK_D)) { sphere.center += moveX; }
			else if (input->PushKey(DIK_A)) { sphere.center -= moveX; }
		}
		//stringstreamで変数の値を埋め込んで整形する

		spherestr << "Sphere:("
			<< std::fixed << std::setprecision(2)//小数点以下2桁まで
			<< sphere.center.m128_f32[0] << ","//x
			<< sphere.center.m128_f32[1] << ","//y
			<< sphere.center.m128_f32[2] << ")";//z

		debugText.Print(spherestr.str(), 50, 180, 1.0f);

		// 球と平面の当たり判定
		XMVECTOR inter;
		hit = Collision::CheckSphere2Plane(sphere, plane, &inter);
		if (hit) {
			color = { 1.0f,0.0f,0.0f,1.0f };
			object3d2->SetColor(color);
		}
		else
		{
			color = { 1.0f,1.0f,1.0f,1.0f };
			object3d2->SetColor(color);
		}

		object3d2->SetPosition({ sphere.center.m128_f32[0],sphere.center.m128_f32[1], sphere.center.m128_f32[2] });
		// カメラ移動
		if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
		{
			if (input->PushKey(DIK_UP)) { Object3d::CameraMoveVector({ 0.0f,+0.2f,0.0f }); }
			else if (input->PushKey(DIK_DOWN)) { Object3d::CameraMoveVector({ 0.0f,-0.2f,0.0f }); }
			if (input->PushKey(DIK_RIGHT)) { Object3d::CameraMoveVector({ +0.2f,0.0f,0.0f }); }
			else if (input->PushKey(DIK_LEFT)) { Object3d::CameraMoveVector({ -0.2f,0.0f,0.0f }); }
		}

		//3Dオブジェクト更新
		object3d1->Update();
		object3d2->Update();
	
		break;
	case GameScene::Scene::CircleTriangle:

		object3d1->SetScale(XMFLOAT3(planeScale));
		object3d1->SetPosition(XMFLOAT3(planePos));
		object3d2->SetScale(XMFLOAT3(playerScale));
		object3d2->SetPosition(XMFLOAT3(playerPos));

		color = object3d2->GetColor();
		// オブジェクト移動
		if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
		{
			// 球移動
			XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);
			if (input->PushKey(DIK_W)) { sphere.center += moveY; }
			else if (input->PushKey(DIK_S)) { sphere.center -= moveY; }
			XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
			if (input->PushKey(DIK_D)) { sphere.center += moveX; }
			else if (input->PushKey(DIK_A)) { sphere.center -= moveX; }
		}
		//球と三角形の当たり判定
	inter;
	hit = Collision::CheckSphere2Triangle(sphere, triangle, &inter);
		if (hit) {
			color = { 1.0f,0.0f,0.0f,1.0f };
			object3d2->SetColor(color);
		}
		else
		{
			color = { 1.0f,1.0f,1.0f,1.0f };
			object3d2->SetColor(color);
		}
	
		object3d2->SetPosition({ sphere.center.m128_f32[0],sphere.center.m128_f32[1], sphere.center.m128_f32[2] });
		// カメラ移動
		if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
		{
			if (input->PushKey(DIK_UP)) { Object3d::CameraMoveVector({ 0.0f,+0.2f,0.0f }); }
			else if (input->PushKey(DIK_DOWN)) { Object3d::CameraMoveVector({ 0.0f,-0.2f,0.0f }); }
			if (input->PushKey(DIK_RIGHT)) { Object3d::CameraMoveVector({ +0.2f,0.0f,0.0f }); }
			else if (input->PushKey(DIK_LEFT)) { Object3d::CameraMoveVector({ -0.2f,0.0f,0.0f }); }
		}
		object3d3->SetScale(XMFLOAT3(triangleScale));
		//3Dオブジェクト更新
		object3d3->Update();
		object3d2->Update();
		break;
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	ImGui::Begin("CONTROLLER");
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetWindowSize(ImVec2(500, 100));
	ImGui::Combo("Scene", (int*)&scene_, "Empty\0Circle Plane\0Circle Triangle\0Scene 3\0");
	
	switch (scene_) {
	case GameScene::Scene::Empty:

		break;
	case GameScene::Scene::CirclePlane:
		object3d1->Draw();
		object3d2->Draw();
		ImGui::DragFloat3("PlayerPosition", sphere.center.m128_f32, 0.1f, -10.0f, 10.0f);
		break;
	case GameScene::Scene::CircleTriangle:
		object3d3->Draw();
		object3d2->Draw();
		ImGui::DragFloat3("PlayerPosition", sphere.center.m128_f32, 0.1f, -10.0f, 10.0f);
		ImGui::DragFloat3("Triangle", triangleScale, 0.1f, -10.0f, 10.0f);
		break;
	}

	ImGui::End();
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}