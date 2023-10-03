#pragma once

#include <memory>

class Model;
struct ViewProjection;
struct WorldTransform;
class Block {
public:
	Block(Model* model);
	~Block();

	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);
private:
	std::unique_ptr<Model> model_;
	WorldTransform worldTransform_;
};

