#include "Block.h"

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

Block::Block(Model* model) {
	model_.reset(model);
	Initialize();
}

Block::~Block() {}

void Block::Initialize() {
	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
}

void Block::Update() {}

void Block::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}
