#include "PlayerHP.h"

#include "ImGuiManager.h"
#include "MyMath.h"

int32_t PlayerHP::add_HP_;

void PlayerHP::Initialize(uint32_t now_TextureHandle, uint32_t delay_TextureHandle) {
	now_HP_ = kMax_HP_;
	delay_HP_ = kMax_HP_;
	add_HP_ = 0;
	now_HP_Sprite_ = std::make_unique<Sprite>();
	delay_HP_Sprite_ = std::make_unique<Sprite>();
	now_HP_Sprite_.reset(
	    Sprite::Create(now_TextureHandle, position_, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	delay_HP_Sprite_.reset(
	    Sprite::Create(delay_TextureHandle, position_, Vector4(0.0f, 0.0f, 1.0f, 0.8f)));
	IsAlive_ = true;
}

void PlayerHP::Update() {
	if (now_HP_ > 0) {
		now_HP_ -= add_HP_;
	} else {
		now_HP_ = 0;
		IsAlive_ = false;
	}
	now_HP_Sprite_->SetSize(Vector2(Conversion(now_HP_), kHeight_));
	delay_HP_ = static_cast<int32_t>(
	    Lerp(static_cast<float>(delay_HP_), static_cast<float>(now_HP_), 0.02f));
	delay_HP_Sprite_->SetSize(Vector2(Conversion(delay_HP_), kHeight_));
	// リセット
	add_HP_ = 0;
	ImGui::Begin("playerHP");
	float now_HP_float = static_cast<float>(now_HP_);
	ImGui::SliderFloat("playerHP", &now_HP_float, 0.0f, static_cast<float>(kMax_HP_));
	now_HP_ = static_cast<int32_t>(now_HP_float);
	if (ImGui::Button("reset")) {
		now_HP_ = kMax_HP_;
		delay_HP_ = kMax_HP_;
	}
	ImGui::End();
}

void PlayerHP::Draw() {
	delay_HP_Sprite_->Draw();
	now_HP_Sprite_->Draw();
}

float PlayerHP::Conversion(int32_t num) {
	float hp_Max = static_cast<float>(kMax_HP_);
	float width = kWidth_;
	return static_cast<float>(num) * width / hp_Max;
}
