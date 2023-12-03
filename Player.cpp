#include "Player.h"

Player::Player() {
	cube_.reset(CubeRenderer::Create());
	worldTransform_.Initialize();
}

void Player::Initialize() {
	speed_ = 1.0f;
	worldTransform_.translation_.z = 10.0f;
	worldTransform_.UpdateMatrix();
}

void Player::Update() {
	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& ViewProjection) {
	cube_->Draw(worldTransform_,ViewProjection);
}

void Player::MoveRight() {
	worldTransform_.translation_.x += speed_;
}

void Player::MoveLeft() {
	worldTransform_.translation_.x -= speed_;
}
