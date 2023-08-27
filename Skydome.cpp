#include "Skydome.h"
#include <assert.h>

#include "ImGuiManager.h"

void Skydome::Initialize(std::unique_ptr<Model> model) {
	// NULLポインタチェック
	assert(model);
	model_ = std::move(model);
	worldTransform_.Initialize();
	scale_ = 51.0f;
	worldTransform_.scale_ = {51.0f, 51.0f, 51.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransform_.UpdateMatrix();
}

void Skydome::Update() {
	ImGui::Begin("skydome");
	ImGui::SliderFloat("scale", &scale_, 0.0f, 100.0f);
	ImGui::End();
	worldTransform_.scale_ = {scale_, scale_ ,scale_};
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_,viewProjection);
}
