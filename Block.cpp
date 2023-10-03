//#include "Block.h"
//
//#include "Model.h"
//
//Block::Block(Model* model) {
//	model_ = std::make_unique<Model>();
//	model_.reset(model);
//}
//
//Block::~Block() {
//	model_.reset();
//}
//
//void Block::Initialize() {
//	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
//}
//
//void Block::Update() {}
//
//void Block::Draw(const ViewProjection& viewProjection) {
//	model_->Draw(worldTransform_, viewProjection);
//}
