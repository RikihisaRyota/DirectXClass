#include "Ground.h"

#include <cassert>

#include "ImGuiManager.h"

float Ground::scale_;

void Ground::Initialize(std::unique_ptr<Model> model) {
	// NULLポインタチェック
	assert(model);
	model_ = std::move(model);
	worldTransform_.Initialize();
	scale_ = 30.0f;
	worldTransform_.scale_ = {30.0f, 30.0f, 30.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransform_.UpdateMatrix();
}

void Ground::Update() {
	ImGui::Begin("ground");
	ImGui::SliderFloat("scale", &scale_, 0.0f, 100.0f);
	ImGui::End();
	worldTransform_.scale_ = {scale_, scale_, scale_};

	worldTransform_.UpdateMatrix();
}

void Ground::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection);
}
