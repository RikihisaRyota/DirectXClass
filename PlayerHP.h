#pragma once

#include <cstdint>
#include <memory>

#include "Sprite.h"

class PlayerHP {
public:
	void Initialize(uint32_t now_TextureHandle, uint32_t delay_TextureHandle);
	void Update();
	void Draw();
	static void SetAdd(int32_t add) { add_HP_ = add; }

private:
	float Conversion(int32_t num);

private:
	const uint32_t kMax_HP_ = 1000;
	const Vector2 position_ = {60.0f, 680.0f};
	const float kWidth_ = 600.0f;
	const float kHeight_ = 30.0f;
	std::unique_ptr<Sprite> now_HP_Sprite_;
	std::unique_ptr<Sprite> delay_HP_Sprite_;
	int32_t now_HP_;
	int32_t delay_HP_;
	static int32_t add_HP_;
};
