#include "GameScene.h"
#include "DirectXCommon.h"
#include "Matrix4x4.h"
#include "TextureManager.h"
#include "ImGuiManager.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();

	// ロード
	textureHandle_ = TextureManager::Load("resources/uvChecker.png");

	// デバックカメラ
	debugCamera_ = new DebugCamera();

	// 入力
	input_ = Input::GetInstans();

	// ライト
	directionalLight_ = new DirectionalLight();
	directionalLight_->color_ = { 1.0f,1.0f,1.0f };
	//directionalLight_->eye_Position_ = viewProjection_.translation_;
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;

	light_.reset(Sphere::Create());
	lightWorldTransform_.Initialize();
	lightWorldTransform_.scale_ = { 0.05f,0.05f,0.05f };
	lightWorldTransform_.translation_ = directionalLight_->direction_ * -1.0f;
	lightWorldTransform_.UpdateMatrix();

	// カメラの初期化
	viewProjection_.Initialize();
	// OBJ
	obj_.reset(OBJ::Create("axis", true));
	objWorldTransform_.Initialize();
	// 土台
	base_.reset(Basic::Create());
	baseWorldTransform_.Initialize();
	// cube
	cube_.reset(Cube::Create(true));
	cubeWorldTransform_.Initialize();
	// sphere
	sphere_.reset(Sphere::Create(true,true));
	sphereWorldTransform_.Initialize();
	// Pera
	pera_=std::make_unique<Pera>();
	pera_->Initialize();

	// peratest
	peraCube_.reset(Cube::Create(true));
	peraCubeWorldTransform_.Initialize();

	// Sprite_
	for (size_t i = 0; i < kCount; i++) {
		sprite_[i].reset(Sprite::Create());
		spriteWorldTransform_[i].Initialize();
		sprite_Color_[i] = { 1.0f,1.0f, 1.0f,1.0f };
		isTexture_[i] = false;
		spriteTextureHandle_[i] = 0u;
	}
}

void GameScene::Update() {
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);
	{
		ImGui::Begin("Lighting");
		if (ImGui::TreeNode("DirectionalLight")) {
			ImGui::SliderFloat3("Color", &directionalLight_->color_.x, 1.0f, 0.0f);
			/*ImGui::Text("EYE: x:%f,y:%f,z:%f", 
				directionalLight_->eye_Position_.x,
				directionalLight_->eye_Position_.y,
				directionalLight_->eye_Position_.z);*/
			ImGui::SliderFloat3("Direction", &directionalLight_->direction_.x, 5.0f, -5.0f);
			ImGui::SliderFloat("Intensiy", &directionalLight_->intensiy_, 1.0f, 0.0f);
			ImGui::SliderFloat("Sharpness", &directionalLight_->sharpness_, 1.0f, 0.0f);
			//ImGui::Checkbox("Light EYE on/off", &IsCamera_[0]);
			ImGui::TreePop();
		}
		ImGui::End();
	}
	//directionalLight_->eye_Position_ = viewProjection_.translation_;
	//IsCamera_[0] == false ? IsCamera_[0] = false : IsCamera_[0] = true;
	cube_->SetDirectionalLight(*directionalLight_);
	obj_->SetDirectionalLight(*directionalLight_);
	sphere_->SetDirectionalLight(*directionalLight_);
	light_->SetDirectionalLight(*directionalLight_);
	lightWorldTransform_.translation_ = directionalLight_->direction_ * -1.0f;
	lightWorldTransform_.UpdateMatrix();
	//{
	//ImGui::ShowDemoWindow();
	//float chack_Color[4] = { sprite_Color_[0].x, sprite_Color_[0].y, sprite_Color_[0].z, sprite_Color_[0].w };
	//ImGui::Begin("[0]");
	//if (ImGui::TreeNode("Color")) {
	//	// 色
	//	ImGui::ColorEdit4("color", chack_Color);
	//	sprite_Color_[0] = Vector4(chack_Color[0], chack_Color[1], chack_Color[2], chack_Color[3]);
	//	sprite_[0]->SetMaterial(sprite_Color_[0]);
	//	ImGui::TreePop();
	//}
	//if (ImGui::TreeNode("WorldTransform")) {
	//	// SRT
	//	ImGui::SliderFloat3("scale", &spriteWorldTransform_[0].scale_.x, 5.0f, 0.0f);
	//	ImGui::SliderFloat3("rotate", &spriteWorldTransform_[0].rotation_.x, 5.0f, -5.0f);
	//	ImGui::SliderFloat3("translation", &spriteWorldTransform_[0].translation_.x, 5.0f, -5.0f);
	//	spriteWorldTransform_[0].UpdateMatrix();
	//	ImGui::TreePop();
	//}
	//if (ImGui::TreeNode("Texture")) {
	//	ImGui::Checkbox("Texture on/off", &isTexture_[0]);
	//	// boolがfalseなら白
	//	isTexture_[0] == false ? spriteTextureHandle_[0] = 0u : spriteTextureHandle_[0] = textureHandle_;
	//	ImGui::TreePop();
	//}
	//ImGui::End();
	//chack_Color[0] = sprite_Color_[1].x;
	//chack_Color[1] = sprite_Color_[1].y;
	//chack_Color[2] = sprite_Color_[1].z;
	//chack_Color[3] = sprite_Color_[1].w;
	//ImGui::Begin("[1]");
	//if (ImGui::TreeNode("Color")) {
	//	// 色
	//	ImGui::ColorEdit4("color", chack_Color);
	//	sprite_Color_[1] = Vector4(chack_Color[0], chack_Color[1], chack_Color[2], chack_Color[3]);
	//	sprite_[1]->SetMaterial(sprite_Color_[1]);
	//	ImGui::TreePop();
	//}
	//if (ImGui::TreeNode("WorldTransform")) {
	//	// SRT
	//	ImGui::SliderFloat3("scale", &spriteWorldTransform_[1].scale_.x, 5.0f, 0.0f);
	//	ImGui::SliderFloat3("rotate", &spriteWorldTransform_[1].rotation_.x, 5.0f, -5.0f);
	//	ImGui::SliderFloat3("translation", &spriteWorldTransform_[1].translation_.x, 5.0f, -5.0f);
	//	spriteWorldTransform_[1].UpdateMatrix();
	//	ImGui::TreePop();
	//}
	//if (ImGui::TreeNode("Texture")) {
	//	ImGui::Checkbox("Texture on/off", &isTexture_[1]);
	//	// boolがfalseなら白
	//	isTexture_[1] == false ? spriteTextureHandle_[1] = 0u : spriteTextureHandle_[1] = textureHandle_;
	//	ImGui::TreePop();
	//}
	//ImGui::End();
	//}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	TextureManager::PreDraw();
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Pera::PreDraw(commandList);
	Cube::PreDraw(commandList);
	Sphere::PreDraw(commandList);
	OBJ::PreDraw(commandList);
	Basic::PreDraw(commandList);
	


	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//base_->Draw(baseWorldTransform_,viewProjection_);
	//toon_->Draw(toonWorldTransform_,viewProjection_,textureHandle_);
	//obj_->Draw(objWorldTransform_, viewProjection_);
	//cube_->Draw(cubeWorldTransform_, viewProjection_, textureHandle_);
	//sphere_->Draw(sphereWorldTransform_, viewProjection_, textureHandle_);
	peraCube_->Draw(peraCubeWorldTransform_,viewProjection_);
	pera_->Draw();

	light_->Draw(lightWorldTransform_, viewProjection_);
	// 3Dオブジェクト描画後処理
	Basic::PostDraw();
	Sphere::PostDraw();
	OBJ::PostDraw();
	Cube::PostDraw();
	Pera::PostDraw();
#pragma endregion



#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	for (size_t i = 0; i < kCount; i++) {
		//sprite_[i]->Draw(spriteWorldTransform_[i], viewProjection_, spriteTextureHandle_[i]);
	}
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Release() {
	SafeDelete(debugCamera_);
	SafeDelete(directionalLight_);
}
