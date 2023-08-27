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
	input_ = Input::GetInstance();

	// ライト
	directionalLight_ = new cDirectionalLight();
	directionalLight_->color_ = { 1.0f,1.0f,1.0f };
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;

	// カメラの初期化
	viewProjection_.Initialize();
	Line::GetInstance()->SetViewProjection(&viewProjection_);

	sprite_.reset(Sprite::Create(textureHandle_, Vector2(0.0f, 0.0f)));

	// 音声再生
	audio = new Audio;
	audio->Initialize();
	soundHandle_ = audio->SoundLoadWave("resources/mokugyo.wav");
}

void GameScene::Update() {
	// 音声テスト
	//audio->SoundPlayWave(soundHandle_);
	// デバックカメラ
	debugCamera_->Update(&viewProjection_);


	ImGui::Begin("Sprite:");
		Vector2 position = sprite_->GetPosition();
		ImGui::SliderFloat2("position", &position.x, -10.0f, 10.0f);
		sprite_->SetPosition(position);
		Vector4 color = sprite_->GetColor();
		ImGui::SliderFloat4("color", &color.x, -1.0f, 1.0f);
		sprite_->SetColor(color);
		Vector2 size = sprite_->GetSize();
		ImGui::SliderFloat2("size", &size.x, -5.0f, 5.0f);
		sprite_->SetSize(size);
	ImGui::End();

#pragma region ライト
	{
		ImGui::Begin("Lighting");
		if (ImGui::TreeNode("DirectionalLight")) {
			ImVec4 color = ImVec4(directionalLight_->color_.x, directionalLight_->color_.y, directionalLight_->color_.z, 0.0f); // 初期値は赤色
			ImGui::ColorEdit3("Color", (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
			directionalLight_->color_.x = color.x, directionalLight_->color_.y = color.y, directionalLight_->color_.z = color.z;
			ImGui::SliderFloat3("Direction", &directionalLight_->direction_.x, 1.0f, -1.0f);
			directionalLight_->direction_ = Normalize(directionalLight_->direction_);
			ImGui::SliderFloat("Intensiy", &directionalLight_->intensiy_, 1.0f, 0.0f);
			ImGui::SliderFloat("Sharpness", &directionalLight_->sharpness_, 1.0f, 0.0f);
			ImGui::TreePop();
		}
		ImGui::End();
	}
	for (auto& i : obj_) {
		i->SetDirectionalLight(*directionalLight_);
	}
	for (auto& i : teapot_) {
		i->SetDirectionalLight(*directionalLight_);
	}
	for (auto& i : bunny_) {
		i->SetDirectionalLight(*directionalLight_);
	}
	for (auto& i : cube_) {
		i->SetDirectionalLight(*directionalLight_);
	}
	for (auto& i : sphere_) {
		i->SetDirectionalLight(*directionalLight_);
	}
	for (auto& i : plane_) {
		i->SetDirectionalLight(*directionalLight_);
	}
	for (auto& mmaterial : multiMaterial_) {
		for (size_t msize = 0; msize < mmaterial->GetModelSize(); msize++) {
			mmaterial->GetMaterial(msize)->SetDirectionalLight(*directionalLight_);
		}
	}
	for (auto& smaterial : suzanne_) {
		for (size_t ssize = 0; ssize < smaterial->GetModelSize(); ssize++) {
			smaterial->GetMaterial(ssize)->SetDirectionalLight(*directionalLight_);
		}
	}

#pragma endregion
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_MenuBar);
#pragma region コンボ
	// ドロップダウンメニューの表示
	static const char* items[] = { "cube", "sphere", "obj", "plane","teapot","bunny","multiMesh","multiMaterial","suzanne" };
	static int currentItem = 0;
	if (ImGui::BeginCombo("Model", items[currentItem])) {
		for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
			bool isSelected = (currentItem == i);
			if (ImGui::Selectable(items[i], isSelected)) {
				currentItem = i;
			}
		}
		ImGui::EndCombo();
	}
#pragma endregion
#pragma region ボタン
	if (ImGui::Button("Create")) {
		switch (currentItem) {
		case 0: // "cube"が選択された場合
		{
			cube_.emplace_back(std::unique_ptr<Cube>(Cube::Create(0)));
			cubeWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			cubeWorldTransform_.back()->Initialize();
			cubeUseTexture_.emplace_back(0);
			cubeUseToon_.emplace_back(0);

			std::unique_ptr<UVtranslation> uvTranslation = std::make_unique<UVtranslation>();
			uvTranslation->scale_ = Vector3(1.0f, 1.0f, 1.0f);
			uvTranslation->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
			uvTranslation->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			cubeUVtranslation_.emplace_back(std::move(uvTranslation));

			std::unique_ptr<cMaterial> material = std::make_unique<cMaterial>();
			material->color_ = { 1.0f,1.0f,1.0f,1.0f };
			material->enableLightint_ = 1;
			cubeMaterial_.emplace_back(std::move(material));
			break;
		}
		case 1: // "sphere"が選択された場合
		{
			sphere_.emplace_back(std::unique_ptr<Sphere>(Sphere::Create(0)));
			sphereWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			sphereWorldTransform_.back()->Initialize();
			sphereUseTexture_.emplace_back(0);
			sphereUseToon_.emplace_back(0);

			std::unique_ptr<UVtranslation> uvTranslation = std::make_unique<UVtranslation>();
			uvTranslation->scale_ = Vector3(1.0f, 1.0f, 1.0f);
			uvTranslation->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
			uvTranslation->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			sphereUVtranslation_.emplace_back(std::move(uvTranslation));

			std::unique_ptr<cMaterial> material = std::make_unique<cMaterial>();
			material->color_ = { 1.0f,1.0f,1.0f,1.0f };
			material->enableLightint_ = 1;
			sphereMaterial_.emplace_back(std::move(material));
			break;
		}
		case 2: // "obj"が選択された場合
		{
			obj_.emplace_back(std::unique_ptr<OBJ>(OBJ::Create("axis", 0)));
			objWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			objWorldTransform_.back()->Initialize();
			objUseTexture_.emplace_back(0);
			objUseToon_.emplace_back(0);

			std::unique_ptr<UVtranslation> uvTranslation = std::make_unique<UVtranslation>();
			uvTranslation->scale_ = Vector3(1.0f, 1.0f, 1.0f);
			uvTranslation->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
			uvTranslation->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			objUVtranslation_.emplace_back(std::move(uvTranslation));

			std::unique_ptr<cMaterial> material = std::make_unique<cMaterial>();
			material->color_ = { 1.0f,1.0f,1.0f,1.0f };
			material->enableLightint_ = 1;
			objMaterial_.emplace_back(std::move(material));
			break;
		}
		case 3: // "sprite"が選択された場合
		{
			plane_.emplace_back(std::unique_ptr<Plane>(Plane::Create(0)));
			planeWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			planeWorldTransform_.back()->Initialize();
			planeUseTexture_.emplace_back(0);
			planeUseToon_.emplace_back(0);

			std::unique_ptr<UVtranslation> uvTranslation = std::make_unique<UVtranslation>();
			uvTranslation->scale_ = Vector3(1.0f, 1.0f, 1.0f);
			uvTranslation->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
			uvTranslation->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			planeUVtranslation_.emplace_back(std::move(uvTranslation));

			std::unique_ptr<cMaterial> material = std::make_unique<cMaterial>();
			material->color_ = { 1.0f,1.0f,1.0f,1.0f };
			material->enableLightint_ = 1;
			planeMaterial_.emplace_back(std::move(material));
			break;
		}
		case 4: // "teapot"が選択された場合
		{
			teapot_.emplace_back(std::unique_ptr<OBJ>(OBJ::Create("teapot", 0)));
			teapotWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			teapotWorldTransform_.back()->Initialize();
			teapotUseTexture_.emplace_back(0);
			teapotUseToon_.emplace_back(0);

			std::unique_ptr<UVtranslation> uvTranslation = std::make_unique<UVtranslation>();
			uvTranslation->scale_ = Vector3(1.0f, 1.0f, 1.0f);
			uvTranslation->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
			uvTranslation->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			teapotUVtranslation_.emplace_back(std::move(uvTranslation));

			std::unique_ptr<cMaterial> material = std::make_unique<cMaterial>();
			material->color_ = { 1.0f,1.0f,1.0f,1.0f };
			material->enableLightint_ = 1;
			teapotMaterial_.emplace_back(std::move(material));
			break;
		}
		case 5: // "bunny"が選択された場合
		{
			bunny_.emplace_back(std::unique_ptr<OBJ>(OBJ::Create("bunny", 0)));
			bunnyWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			bunnyWorldTransform_.back()->Initialize();
			bunnyUseTexture_.emplace_back(0);
			bunnyUseToon_.emplace_back(0);

			std::unique_ptr<UVtranslation> uvTranslation = std::make_unique<UVtranslation>();
			uvTranslation->scale_ = Vector3(1.0f, 1.0f, 1.0f);
			uvTranslation->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
			uvTranslation->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			bunnyUVtranslation_.emplace_back(std::move(uvTranslation));

			std::unique_ptr<cMaterial> material = std::make_unique<cMaterial>();
			material->color_ = { 1.0f,1.0f,1.0f,1.0f };
			material->enableLightint_ = 1;
			bunnyMaterial_.emplace_back(std::move(material));
			break;
		}
		case 6: // "multiMesh"が選択された場合
		{
			multiMesh_.emplace_back(std::unique_ptr<OBJ>(OBJ::Create("multiMesh", 0)));
			multiMeshWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			multiMeshWorldTransform_.back()->Initialize();
			multiMeshUseTexture_.emplace_back(0);
			multiMeshUseToon_.emplace_back(0);

			std::unique_ptr<UVtranslation> uvTranslation = std::make_unique<UVtranslation>();
			uvTranslation->scale_ = Vector3(1.0f, 1.0f, 1.0f);
			uvTranslation->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
			uvTranslation->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			multiMeshUVtranslation_.emplace_back(std::move(uvTranslation));

			std::unique_ptr<cMaterial> material = std::make_unique<cMaterial>();
			material->color_ = { 1.0f,1.0f,1.0f,1.0f };
			material->enableLightint_ = 1;
			multiMeshMaterial_.emplace_back(std::move(material));
			break;
		}
		case 7: // "multiMaterial"が選択された場合
		{
			multiMaterial_.emplace_back(std::unique_ptr<Model>(Model::Create("multiMaterial")));
			multiMaterialWorldTransform_.emplace_back(std::make_unique<WorldTransform>());
			multiMaterialWorldTransform_.back()->Initialize();
			multiMaterialUseTexture_.emplace_back(0);
			multiMaterialUseToon_.emplace_back(0);

			size_t modelSize = multiMaterial_.back()->GetModelSize();

			std::vector<std::unique_ptr<UVtranslation>> uvTranslations;
			uvTranslations.reserve(modelSize);
			for (size_t i = 0; i < modelSize; i++) {
				uvTranslations.emplace_back(new UVtranslation);
				uvTranslations.back()->scale_ = Vector3(1.0f, 1.0f, 1.0f);
				uvTranslations.back()->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
				uvTranslations.back()->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			}
			multiMaterialUVtranslation_.emplace_back(std::move(uvTranslations));

			std::vector<std::unique_ptr<cMaterial>> materials;
			materials.reserve(modelSize);
			for (size_t i = 0; i < modelSize; i++) {
				materials.emplace_back(new cMaterial);
				materials.back()->color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
				materials.back()->enableLightint_ = 0;
			}
			multiMaterialMaterial_.emplace_back(std::move(materials));
			break;
		}
		case 8: // "suzanne"が選択された場合
		{
			suzanne_.emplace_back(std::unique_ptr<Model>(Model::Create("suzanne", 0)));
			suzanneWorldTransform_.emplace_back(new WorldTransform);
			suzanneWorldTransform_.back()->Initialize();
			suzanneUseTexture_.emplace_back(0);
			suzanneUseToon_.emplace_back(0);

			size_t modelSize = suzanne_.back()->GetModelSize();

			std::vector<std::unique_ptr<UVtranslation>> uvTranslations;
			uvTranslations.reserve(modelSize);
			for (size_t i = 0; i < modelSize; i++) {
				uvTranslations.emplace_back(new UVtranslation);
				uvTranslations.back()->scale_ = Vector3(1.0f, 1.0f, 1.0f);
				uvTranslations.back()->rotate_ = Vector3(0.0f, 0.0f, 0.0f);
				uvTranslations.back()->translate_ = Vector3(0.0f, 0.0f, 0.0f);
			}
			suzanneUVtranslation_.emplace_back(std::move(uvTranslations));

			std::vector<std::unique_ptr<cMaterial>> materials;
			materials.reserve(modelSize);
			for (size_t i = 0; i < modelSize; i++) {
				materials.emplace_back(new cMaterial);
				materials.back()->color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
				materials.back()->enableLightint_ = 1;
			}
			suzanneMaterial_.emplace_back(std::move(materials));
			break;
		}
		}
	}
#pragma endregion
	if (!obj_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < obj_.size(); ++i) {
			auto& worldTransform = objWorldTransform_[i];
			auto& material = objMaterial_[i];
			auto& uvTranslate = objUVtranslation_[i];
			if (ImGui::TreeNode(("OBJ_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (objUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					objUseToon_[i] ^= true;
					obj_[i]->SetToon(objUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (objUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseTexture_" + std::to_string(i)).c_str(), &flag)) {
					objUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				if (objUseTexture_[i] && ImGui::TreeNode(("Obj_MATERIAL_" + std::to_string(i)).c_str())) {
					ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
					ImGui::ColorEdit3(("Color_" + std::to_string(i)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
					material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
					ImGui::SliderFloat2(("Material_scale_" + std::to_string(i)).c_str(), &uvTranslate->scale_.x, 1.0f, 5.0f);
					ImGui::SliderFloat(("Material_rotate_" + std::to_string(i)).c_str(), &uvTranslate->rotate_.z, -5.0f, 5.0f);
					ImGui::SliderFloat2(("Material_translate_" + std::to_string(i)).c_str(), &uvTranslate->translate_.x, -5.0f, 5.0f);
					material->uvTransform_ = MakeAffineMatrix(uvTranslate->scale_, uvTranslate->rotate_, uvTranslate->translate_);
					// ドロップダウンメニューの表示
					const char* obj_Liting[] = { "None", "HalhLambert", "Lambert" };
					int obj_CurrentLiting = material->enableLightint_;
					if (ImGui::BeginCombo(("Lighting_" + std::to_string(i)).c_str(), obj_Liting[obj_CurrentLiting])) {
						for (int i = 0; i < IM_ARRAYSIZE(obj_Liting); i++) {
							bool isSelected = (obj_CurrentLiting == i);
							if (ImGui::Selectable(obj_Liting[i], isSelected)) {
								obj_CurrentLiting = i;
							}
						}
						ImGui::EndCombo();
					}
					material->enableLightint_ = obj_CurrentLiting;
					obj_[i]->SetMaterial(*material.get());
					ImGui::TreePop();
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					obj_.erase(obj_.begin() + i);
					objWorldTransform_.erase(objWorldTransform_.begin() + i);
					objMaterial_.erase(objMaterial_.begin() + i);
					objUVtranslation_.erase(objUVtranslation_.begin() + i);
					objUseTexture_.erase(objUseTexture_.begin() + i);
					objUseToon_.erase(objUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!cube_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < cube_.size(); ++i) {
			auto& worldTransform = cubeWorldTransform_[i];
			auto& material = cubeMaterial_[i];
			auto& uvTranslate = cubeUVtranslation_[i];
			if (ImGui::TreeNode(("CUBE_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (cubeUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					cubeUseToon_[i] ^= true;
					cube_[i]->SetToon(cubeUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (cubeUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseTexture_" + std::to_string(i)).c_str(), &flag)) {
					cubeUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				if (cubeUseTexture_[i] && ImGui::TreeNode(("Cube_MATERIAL_" + std::to_string(i)).c_str())) {
					ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
					ImGui::ColorEdit3(("Color_" + std::to_string(i)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
					material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
					ImGui::SliderFloat2(("Material_scale_" + std::to_string(i)).c_str(), &uvTranslate->scale_.x, 1.0f, 5.0f);
					ImGui::SliderFloat(("Material_rotate_" + std::to_string(i)).c_str(), &uvTranslate->rotate_.z, -5.0f, 5.0f);
					ImGui::SliderFloat2(("Material_translate_" + std::to_string(i)).c_str(), &uvTranslate->translate_.x, -5.0f, 5.0f);
					material->uvTransform_ = MakeAffineMatrix(uvTranslate->scale_, uvTranslate->rotate_, uvTranslate->translate_);
					// ドロップダウンメニューの表示
					const char* cube_Liting[] = { "None", "HalhLambert", "Lambert" };
					int cube_CurrentLiting = material->enableLightint_;
					if (ImGui::BeginCombo(("Lighting_" + std::to_string(i)).c_str(), cube_Liting[cube_CurrentLiting])) {
						for (int i = 0; i < IM_ARRAYSIZE(cube_Liting); i++) {
							bool isSelected = (cube_CurrentLiting == i);
							if (ImGui::Selectable(cube_Liting[i], isSelected)) {
								cube_CurrentLiting = i;
							}
						}
						ImGui::EndCombo();
					}
					material->enableLightint_ = cube_CurrentLiting;
					cube_[i]->SetMaterial(*material.get());
					ImGui::TreePop();
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					cube_.erase(cube_.begin() + i);
					cubeWorldTransform_.erase(cubeWorldTransform_.begin() + i);
					cubeMaterial_.erase(cubeMaterial_.begin() + i);
					cubeUVtranslation_.erase(cubeUVtranslation_.begin() + i);
					cubeUseTexture_.erase(cubeUseTexture_.begin() + i);
					cubeUseToon_.erase(cubeUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!sphere_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < sphere_.size(); ++i) {
			auto& worldTransform = sphereWorldTransform_[i];
			auto& material = sphereMaterial_[i];
			auto& uvTranslate = sphereUVtranslation_[i];
			if (ImGui::TreeNode(("SPHERE_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (sphereUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					sphereUseToon_[i] ^= true;
					sphere_[i]->SetToon(sphereUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (sphereUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("SphereUseTexture_" + std::to_string(i)).c_str(), &flag)) {
					sphereUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				if (sphereUseTexture_[i] && ImGui::TreeNode(("Sphere_MATERIAL_" + std::to_string(i)).c_str()))  {
					ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
					ImGui::ColorEdit3(("Color_" + std::to_string(i)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
					material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
					ImGui::SliderFloat2(("Material_scale_" + std::to_string(i)).c_str(), &uvTranslate->scale_.x, 1.0f, 5.0f);
					ImGui::SliderFloat(("Material_rotate_" + std::to_string(i)).c_str(), &uvTranslate->rotate_.z, -5.0f, 5.0f);
					ImGui::SliderFloat2(("Material_translate" + std::to_string(i)).c_str(), &uvTranslate->translate_.x, -5.0f, 5.0f);
					material->uvTransform_ = MakeAffineMatrix(uvTranslate->scale_, uvTranslate->rotate_, uvTranslate->translate_);
					// ドロップダウンメニューの表示
					const char* sphere_Liting[] = { "None", "HalhLambert", "Lambert" };
					int sphere_CurrentLiting = material->enableLightint_;
					if (ImGui::BeginCombo(("Lighting_" + std::to_string(i)).c_str(), sphere_Liting[sphere_CurrentLiting])) {
						for (int sl = 0; sl < IM_ARRAYSIZE(sphere_Liting); sl++) {
							bool isSelected = (sphere_CurrentLiting == sl);
							if (ImGui::Selectable(sphere_Liting[sl], isSelected)) {
								sphere_CurrentLiting = sl;
							}
						}
						ImGui::EndCombo();
					}
					material->enableLightint_ = sphere_CurrentLiting;
					sphere_[i]->SetMaterial(*material.get());
					ImGui::TreePop();
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					sphere_.erase(sphere_.begin() + i);
					sphereWorldTransform_.erase(sphereWorldTransform_.begin() + i);
					sphereMaterial_.erase(sphereMaterial_.begin() + i);
					sphereUVtranslation_.erase(sphereUVtranslation_.begin() + i);
					sphereUseTexture_.erase(sphereUseTexture_.begin() + i);
					sphereUseToon_.erase(sphereUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!plane_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < plane_.size(); ++i) {
			auto& worldTransform = planeWorldTransform_[i];
			auto& material = planeMaterial_[i];
			auto& uvTranslate = planeUVtranslation_[i];
			if (ImGui::TreeNode(("Plane_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				/*if (spriteUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					spriteUseToon_[i] ^= true;
					sprite_[i]->SetToon(spriteUseToon_[i]);
				}*/
#pragma endregion
#pragma region USETexture
				if (planeUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("PlaneUseTexture_" + std::to_string(i)).c_str(), &flag)) {
					planeUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				if (planeUseTexture_[i] && ImGui::TreeNode(("Plane_MATERIAL_" + std::to_string(i)).c_str())) {
					ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
					ImGui::ColorEdit3(("Color_" + std::to_string(i)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
					material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
					ImGui::SliderFloat2(("Material_scale_" + std::to_string(i)).c_str(), &uvTranslate->scale_.x, 1.0f, 5.0f);
					ImGui::SliderFloat(("Material_rotate_" + std::to_string(i)).c_str(), &uvTranslate->rotate_.z, -5.0f, 5.0f);
					ImGui::SliderFloat2(("Material_translate_" + std::to_string(i)).c_str(), &uvTranslate->translate_.x, -5.0f, 5.0f);
					material->uvTransform_ = MakeAffineMatrix(uvTranslate->scale_, uvTranslate->rotate_, uvTranslate->translate_);
					// ドロップダウンメニューの表示
					const char* sprite_Liting[] = { "None", "HalhLambert", "Lambert" };
					int sprite_CurrentLiting = material->enableLightint_;
					if (ImGui::BeginCombo(("Lighting_" + std::to_string(i)).c_str(), sprite_Liting[sprite_CurrentLiting])) {
						for (int i = 0; i < IM_ARRAYSIZE(sprite_Liting); i++) {
							bool isSelected = (sprite_CurrentLiting == i);
							if (ImGui::Selectable(sprite_Liting[i], isSelected)) {
								sprite_CurrentLiting = i;
							}
						}
						ImGui::EndCombo();
					}
					material->enableLightint_ = sprite_CurrentLiting;
					plane_[i]->SetMaterial(*material.get());
					ImGui::TreePop();
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					plane_.erase(plane_.begin() + i);
					planeWorldTransform_.erase(planeWorldTransform_.begin() + i);
					planeMaterial_.erase(planeMaterial_.begin() + i);
					planeUVtranslation_.erase(planeUVtranslation_.begin() + i);
					planeUseTexture_.erase(planeUseTexture_.begin() + i);
					planeUseToon_.erase(planeUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!teapot_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < teapot_.size(); ++i) {
			auto& worldTransform = teapotWorldTransform_[i];
			auto& material = teapotMaterial_[i];
			auto& uvTranslate = teapotUVtranslation_[i];
			if (ImGui::TreeNode(("TEAPOT_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (teapotUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					teapotUseToon_[i] ^= true;
					teapot_[i]->SetToon(teapotUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (teapotUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseTexture_" + std::to_string(i)).c_str(), &flag)) {
					teapotUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				if (teapotUseTexture_[i] && ImGui::TreeNode(("Teapot_MATERIAL_" + std::to_string(i)).c_str())) {
					ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
					ImGui::ColorEdit3(("Color_" + std::to_string(i)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
					material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
					ImGui::SliderFloat2(("Material_scale_" + std::to_string(i)).c_str(), &uvTranslate->scale_.x, 1.0f, 5.0f);
					ImGui::SliderFloat(("Material_rotate_" + std::to_string(i)).c_str(), &uvTranslate->rotate_.z, -5.0f, 5.0f);
					ImGui::SliderFloat2(("Material_translate" + std::to_string(i)).c_str(), &uvTranslate->translate_.x, -5.0f, 5.0f);
					material->uvTransform_ = MakeAffineMatrix(uvTranslate->scale_, uvTranslate->rotate_, uvTranslate->translate_);
					// ドロップダウンメニューの表示
					const char* teapot_Liting[] = { "None", "HalhLambert", "Lambert" };
					int teapot_CurrentLiting = material->enableLightint_;
					if (ImGui::BeginCombo(("Lighting_" + std::to_string(i)).c_str(), teapot_Liting[teapot_CurrentLiting])) {
						for (int i = 0; i < IM_ARRAYSIZE(teapot_Liting); i++) {
							bool isSelected = (teapot_CurrentLiting == i);
							if (ImGui::Selectable(teapot_Liting[i], isSelected)) {
								teapot_CurrentLiting = i;
							}
						}
						ImGui::EndCombo();
					}
					material->enableLightint_ = teapot_CurrentLiting;
					teapot_[i]->SetMaterial(*material.get());
					ImGui::TreePop();
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					teapot_.erase(teapot_.begin() + i);
					teapotWorldTransform_.erase(teapotWorldTransform_.begin() + i);
					teapotMaterial_.erase(teapotMaterial_.begin() + i);
					teapotUVtranslation_.erase(teapotUVtranslation_.begin() + i);
					teapotUseTexture_.erase(teapotUseTexture_.begin() + i);
					teapotUseToon_.erase(teapotUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!bunny_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < bunny_.size(); ++i) {
			auto& worldTransform = bunnyWorldTransform_[i];
			auto& material = bunnyMaterial_[i];
			auto& uvTranslate = bunnyUVtranslation_[i];
			if (ImGui::TreeNode(("BUNNY_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (bunnyUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					bunnyUseToon_[i] ^= true;
					bunny_[i]->SetToon(bunnyUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (bunnyUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseTexture_" + std::to_string(i)).c_str(), &flag)) {
					bunnyUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				if (bunnyUseTexture_[i] && ImGui::TreeNode(("Bunny_MATERIAL_" + std::to_string(i)).c_str())) {
					ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
					ImGui::ColorEdit3(("Color_" + std::to_string(i)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
					material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
					ImGui::SliderFloat2(("Material_scale_" + std::to_string(i)).c_str(), &uvTranslate->scale_.x, 1.0f, 5.0f);
					ImGui::SliderFloat(("Material_rotate_" + std::to_string(i)).c_str(), &uvTranslate->rotate_.z, -5.0f, 5.0f);
					ImGui::SliderFloat2(("Material_translate" + std::to_string(i)).c_str(), &uvTranslate->translate_.x, -5.0f, 5.0f);
					material->uvTransform_ = MakeAffineMatrix(uvTranslate->scale_, uvTranslate->rotate_, uvTranslate->translate_);
					// ドロップダウンメニューの表示
					const char* bunny_Liting[] = { "None", "HalhLambert", "Lambert" };
					int bunny_CurrentLiting = material->enableLightint_;
					if (ImGui::BeginCombo(("Lighting_" + std::to_string(i)).c_str(), bunny_Liting[bunny_CurrentLiting])) {
						for (int i = 0; i < IM_ARRAYSIZE(bunny_Liting); i++) {
							bool isSelected = (bunny_CurrentLiting == i);
							if (ImGui::Selectable(bunny_Liting[i], isSelected)) {
								bunny_CurrentLiting = i;
							}
						}
						ImGui::EndCombo();
					}
					material->enableLightint_ = bunny_CurrentLiting;
					bunny_[i]->SetMaterial(*material.get());
					ImGui::TreePop();
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					bunny_.erase(bunny_.begin() + i);
					bunnyWorldTransform_.erase(bunnyWorldTransform_.begin() + i);
					bunnyMaterial_.erase(bunnyMaterial_.begin() + i);
					bunnyUVtranslation_.erase(bunnyUVtranslation_.begin() + i);
					bunnyUseTexture_.erase(bunnyUseTexture_.begin() + i);
					bunnyUseToon_.erase(bunnyUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!multiMesh_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < multiMesh_.size(); ++i) {
			auto& worldTransform = multiMeshWorldTransform_[i];
			auto& material = multiMeshMaterial_[i];
			auto& uvTranslate = multiMeshUVtranslation_[i];
			if (ImGui::TreeNode(("MultiMesh_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (multiMeshUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					multiMeshUseToon_[i] ^= true;
					multiMesh_[i]->SetToon(multiMeshUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (multiMeshUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseTexture_" + std::to_string(i)).c_str(), &flag)) {
					multiMeshUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				if (multiMeshUseTexture_[i] && ImGui::TreeNode(("MultiMesh_MATERIAL_" + std::to_string(i)).c_str())) {
					ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
					ImGui::ColorEdit3(("Color_" + std::to_string(i)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
					material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
					ImGui::SliderFloat2(("Material_scale_" + std::to_string(i)).c_str(), &uvTranslate->scale_.x, 1.0f, 5.0f);
					ImGui::SliderFloat(("Material_rotate_" + std::to_string(i)).c_str(), &uvTranslate->rotate_.z, -5.0f, 5.0f);
					ImGui::SliderFloat2(("Material_translate" + std::to_string(i)).c_str(), &uvTranslate->translate_.x, -5.0f, 5.0f);
					material->uvTransform_ = MakeAffineMatrix(uvTranslate->scale_, uvTranslate->rotate_, uvTranslate->translate_);
					// ドロップダウンメニューの表示
					const char* multiMesh_Liting[] = { "None", "HalhLambert", "Lambert" };
					int multiMesh_CurrentLiting = material->enableLightint_;
					if (ImGui::BeginCombo(("Lighting_" + std::to_string(i)).c_str(), multiMesh_Liting[multiMesh_CurrentLiting])) {
						for (int i = 0; i < IM_ARRAYSIZE(multiMesh_Liting); i++) {
							bool isSelected = (multiMesh_CurrentLiting == i);
							if (ImGui::Selectable(multiMesh_Liting[i], isSelected)) {
								multiMesh_CurrentLiting = i;
							}
						}
						ImGui::EndCombo();
					}
					material->enableLightint_ = multiMesh_CurrentLiting;
					multiMesh_[i]->SetMaterial(*material.get());
					ImGui::TreePop();
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					multiMesh_.erase(multiMesh_.begin() + i);
					multiMeshWorldTransform_.erase(multiMeshWorldTransform_.begin() + i);
					multiMeshMaterial_.erase(multiMeshMaterial_.begin() + i);
					multiMeshUVtranslation_.erase(multiMeshUVtranslation_.begin() + i);
					multiMeshUseTexture_.erase(multiMeshUseTexture_.begin() + i);
					multiMeshUseToon_.erase(multiMeshUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!multiMaterial_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < multiMaterial_.size(); ++i) {
			auto& worldTransform = multiMaterialWorldTransform_[i];
			auto& material = multiMaterialMaterial_[i];
			auto& uvTranslate = multiMaterialUVtranslation_[i];
			if (ImGui::TreeNode(("MultiMaterial_" + std::to_string(i)).c_str()))  {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (multiMaterialUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					multiMaterialUseToon_[i] ^= true;
					multiMaterial_[i]->SetToon(multiMaterialUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (multiMaterialUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseTexture_" + std::to_string(i)).c_str(), &flag)) {
					multiMaterialUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				const size_t modelSize = multiMaterial_[i]->GetModelSize();
				for (size_t mmi = 0; mmi < modelSize; ++mmi) {
					if (multiMaterialUseTexture_[i] && ImGui::TreeNode(("MultiMaterial_MATERIAL_" + std::to_string(i) + "_" + std::to_string(mmi)).c_str())) {
						const auto& material = multiMaterial_[i]->GetMaterial(mmi)->GetMaterial();
						ImVec4 color = ImVec4(material->color_.x, material->color_.y, material->color_.z, 0.0f); // 初期値は赤色
						ImGui::ColorEdit3(("Color_" + std::to_string(i) + "_" + std::to_string(mmi)).c_str(), (float*)&color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar);
						material->color_.x = color.x, material->color_.y = color.y, material->color_.z = color.z;
						ImGui::SliderFloat2(("Material_scale_" + std::to_string(i) + "_" + std::to_string(mmi)).c_str(), &uvTranslate[mmi]->scale_.x, 1.0f, 5.0f);
						ImGui::SliderFloat(("Material_rotate_" + std::to_string(i) + "_" + std::to_string(mmi)).c_str(), &uvTranslate[mmi]->rotate_.z, -5.0f, 5.0f);
						ImGui::SliderFloat2(("Material_translate" + std::to_string(i) + "_" + std::to_string(mmi)).c_str(), &uvTranslate[mmi]->translate_.x, -5.0f, 5.0f);
						material->uvTransform_ = MakeAffineMatrix(uvTranslate[mmi]->scale_, uvTranslate[mmi]->rotate_, uvTranslate[mmi]->translate_);
						// ドロップダウンメニューの表示
						const char* multiMaterial_Liting[] = { "None", "HalhLambert", "Lambert" };
						std::vector<uint32_t> multiMaterial_CurrentLiting(modelSize, material->enableLightint_); // 配列を宣言
						if (ImGui::BeginCombo(("Lighting_" + std::to_string(i) + "_" + std::to_string(mmi)).c_str(), multiMaterial_Liting[multiMaterial_CurrentLiting[mmi]])) {
							for (int ml = 0; ml < IM_ARRAYSIZE(multiMaterial_Liting); ml++) {
								bool isSelected = (multiMaterial_CurrentLiting[mmi] == ml);
								if (ImGui::Selectable(multiMaterial_Liting[ml], isSelected)) {
									multiMaterial_CurrentLiting[mmi] = ml;
								}
							}
							ImGui::EndCombo();
						}
						material->enableLightint_ = multiMaterial_CurrentLiting[mmi];
						multiMaterial_[i]->GetMaterial(mmi)->SetMaterial(*material);
						ImGui::TreePop();
					}
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					multiMaterial_.erase(multiMaterial_.begin() + i);
					multiMaterialWorldTransform_.erase(multiMaterialWorldTransform_.begin() + i);
					multiMaterialMaterial_.erase(multiMaterialMaterial_.begin() + i);
					multiMaterialUVtranslation_.erase(multiMaterialUVtranslation_.begin() + i);
					multiMaterialUseTexture_.erase(multiMaterialUseTexture_.begin() + i);
					multiMaterialUseToon_.erase(multiMaterialUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
			ImGui::TreePop();
			}
		}
	}
	if (!suzanne_.empty()) {
		bool flag = false;
		for (size_t i = 0; i < suzanne_.size(); ++i) {
			auto& worldTransform = suzanneWorldTransform_[i];
			auto& material = suzanneMaterial_[i];
			auto& uvTranslate = suzanneUVtranslation_[i];
			if (ImGui::TreeNode(("Suzanne_" + std::to_string(i)).c_str())) {
#pragma region worldtransform
				ImGui::SliderFloat3(("scale_" + std::to_string(i)).c_str(), &worldTransform->scale_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("rotate_" + std::to_string(i)).c_str(), &worldTransform->rotation_.x, -5.0f, 5.0f);
				ImGui::SliderFloat3(("translate_" + std::to_string(i)).c_str(), &worldTransform->translation_.x, -5.0f, 5.0f);
				worldTransform->UpdateMatrix();
#pragma endregion
#pragma region USEToon
				if (suzanneUseToon_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseToon_" + std::to_string(i)).c_str(), &flag)) {
					suzanneUseToon_[i] ^= true;
					suzanne_[i]->SetToon(suzanneUseToon_[i]);
				}
#pragma endregion
#pragma region USETexture
				if (suzanneUseTexture_[i] == 0) {
					flag = false;
				}
				else {
					flag = true;
				}
				if (ImGui::Checkbox(("UseLiting_" + std::to_string(i)).c_str(), &flag)) {
					suzanneUseTexture_[i] ^= true;
				}
#pragma endregion
#pragma region material
				const size_t modelSize = suzanne_[i]->GetModelSize();
				for (size_t mmi = 0; mmi < modelSize; ++mmi) {
					if (suzanneUseTexture_[i] && ImGui::TreeNode(("Suzanne_Lighting_" + std::to_string(mmi)).c_str())) {
						const auto& material = suzanne_[i]->GetMaterial(mmi)->GetMaterial();
						// ドロップダウンメニューの表示
						const char* suzanne_Liting[] = { "None", "HalhLambert", "Lambert" };
						int suzanne_CurrentLiting = material->enableLightint_;
						if (ImGui::BeginCombo(("Lighting_" + std::to_string(mmi)).c_str(), suzanne_Liting[suzanne_CurrentLiting])) {
							for (int i = 0; i < IM_ARRAYSIZE(suzanne_Liting); i++) {
								bool isSelected = (suzanne_CurrentLiting == i);
								if (ImGui::Selectable(suzanne_Liting[i], isSelected)) {
									suzanne_CurrentLiting = i;
								}
							}
							ImGui::EndCombo();
						}
						material->enableLightint_ = suzanne_CurrentLiting;
						suzanne_[i]->GetMaterial(mmi)->SetMaterial(*material);
						ImGui::TreePop();
					}
				}
#pragma endregion
#pragma region Delete
				if (ImGui::Button("Delete")) {
					suzanne_.erase(suzanne_.begin() + i);
					suzanneWorldTransform_.erase(suzanneWorldTransform_.begin() + i);
					suzanneMaterial_.erase(suzanneMaterial_.begin() + i);
					suzanneUVtranslation_.erase(suzanneUVtranslation_.begin() + i);
					suzanneUseTexture_.erase(suzanneUseTexture_.begin() + i);
					suzanneUseToon_.erase(suzanneUseToon_.begin() + i);
					// 削除した要素の後ろの要素が前にずれるため、インデックスを調整する
					--i;
				}
#pragma endregion
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	
	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();
#pragma endregion
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Cube::PreDraw(commandList);
	Sphere::PreDraw(commandList);
	OBJ::PreDraw(commandList);
	Model::PreDraw(commandList);
	Line::PreDraw(commandList);
	Plane::PreDraw(commandList);

	


	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	if (!obj_.empty()) {
		for (size_t i = 0; i < obj_.size(); ++i) {
			if (!objUseTexture_[i]) {
				obj_[i]->Draw(*objWorldTransform_[i], viewProjection_, 10);
			}
			else {
				obj_[i]->Draw(*objWorldTransform_[i], viewProjection_);
			}

		}
	}
	if (!cube_.empty()) {
		for (auto i = 0; i < cube_.size(); ++i) {
			if (!cubeUseTexture_[i]) {
				cube_[i]->Draw(*cubeWorldTransform_[i], viewProjection_);
			}
			else {
				cube_[i]->Draw(*cubeWorldTransform_[i], viewProjection_, textureHandle_);
			}
		}
	}
	if (!sphere_.empty()) {
		for (auto i = 0; i < sphere_.size(); ++i) {
			if (!sphereUseTexture_[i]) {
				sphere_[i]->Draw(*sphereWorldTransform_[i], viewProjection_);
			}
			else {
				sphere_[i]->Draw(*sphereWorldTransform_[i], viewProjection_, textureHandle_);
			}
		}
	}
	if (!plane_.empty()) {
		for (size_t i = 0; i < plane_.size(); ++i) {
			if (!planeUseTexture_[i]) {
				plane_[i]->Draw(*planeWorldTransform_[i], viewProjection_);
			}
			else {
				plane_[i]->Draw(*planeWorldTransform_[i], viewProjection_, textureHandle_);
			}
		}
	}
	if (!teapot_.empty()) {
		for (auto i = 0; i < teapot_.size(); ++i) {
			if (!teapotUseTexture_[i]) {
				teapot_[i]->Draw(*teapotWorldTransform_[i], viewProjection_, 10);
			}
			else {
				teapot_[i]->Draw(*teapotWorldTransform_[i], viewProjection_);
			}
		}
	}
	if (!bunny_.empty()) {
		for (auto i = 0; i < bunny_.size(); ++i) {
			if (!bunnyUseTexture_[i]) {
				bunny_[i]->Draw(*bunnyWorldTransform_[i], viewProjection_, 10);
			}
			else {
				bunny_[i]->Draw(*bunnyWorldTransform_[i], viewProjection_);
			}
		}
	}
	if (!multiMesh_.empty()) {
		for (auto i = 0; i < multiMesh_.size(); ++i) {
			if (!multiMeshUseTexture_[i]) {
				multiMesh_[i]->Draw(*multiMeshWorldTransform_[i], viewProjection_, 10);
			}
			else {
				multiMesh_[i]->Draw(*multiMeshWorldTransform_[i], viewProjection_);
			}
		}
	}
	if (!multiMaterial_.empty()) {
		for (auto i = 0; i < multiMaterial_.size(); ++i) {
			if (multiMaterialUseTexture_[i] == 0) {
				multiMaterial_[i]->Draw(*multiMaterialWorldTransform_[i], viewProjection_, 10);
			}
			else {
				multiMaterial_[i]->Draw(*multiMaterialWorldTransform_[i], viewProjection_);
			}
		}
	}
	if (!suzanne_.empty()) {
		for (auto i = 0; i < suzanne_.size(); ++i) {
			if (suzanneUseTexture_[i] == 0) {
				suzanne_[i]->Draw(*suzanneWorldTransform_[i], viewProjection_, 10);
			}
			else {
				suzanne_[i]->Draw(*suzanneWorldTransform_[i], viewProjection_);
			}
		}
	}
	Line::GetInstance()->Line::Draw();
	// 3Dオブジェクト描画後処理
	Plane::PostDraw();
	Line::PostDraw();
	Model::PostDraw();
	Sphere::PostDraw();
	OBJ::PostDraw();
	Cube::PostDraw();
#pragma endregion



#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Release() {
	SafeDelete(debugCamera_);
	SafeDelete(directionalLight_);
}
