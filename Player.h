#pragma once

#include <memory>

#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CubeRenderer.h"

class Player {
public:
	Player();

	void Initialize();
	void Update();
	void Draw(const ViewProjection& ViewProjection);

	void MoveRight();
	void MoveLeft();
private:
	WorldTransform worldTransform_;
	float speed_;
	std::unique_ptr<CubeRenderer> cube_;
};
