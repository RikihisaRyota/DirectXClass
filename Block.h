#pragma once

#include "Collider.h"
#include "BaseCharacter.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Block : public BaseCharacter, public Collider {
public:
	void Initialize(std::vector<std::unique_ptr<Model>> model) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;
	void HitBoxDraw(const ViewProjection& viewProjection) override;
	void OnCollision(const OBB& obb, uint32_t type) override;
private:
	void HitBoxInitialize(uint32_t collisionMask) override;
	void HitBoxUpdate() override;
};

