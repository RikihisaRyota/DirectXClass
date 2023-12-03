#pragma once

#include "ViewProjection.h"
#include "WorldTransform.h"

class Player {
public:
	Player();

	void Initialize();
	void Update();
	void Draw(const ViewProjection& ViewProjection);

private:
	WorldTransform worldTransform_;

};
