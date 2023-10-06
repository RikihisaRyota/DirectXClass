#pragma once

#include "Collider.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Player;
class Block : public Collider {
public:
	void Initialize(std::vector<std::unique_ptr<Model>> model) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;
	void HitBoxDraw(const ViewProjection& viewProjection) override;
	void OnCollision(const OBB& obb, const WorldTransform& worldTransform, uint32_t type) override;
	void SetPlayer(Player* player) { player_ = player; }
private:
	void HitBoxInitialize(uint32_t collisionMask) override;
	void HitBoxUpdate() override;
	Player* player_;
};

