#pragma once
#include <list>
#include <memory>

#include "Collider.h"

#include "Block.h"
#include "Enemy.h"
#include "EnemyAttack.h"
#include "Player.h"

class CollisionManager {
public:
	void Update(Player* player, Block* block, Enemy* enemy, EnemyAttack* enemyAttack);

private:
	// メンバ関数
	void CheckAllCollisions(Player* player, Block* block,Enemy* enemy,EnemyAttack* enemyAttack);
	/// <summary>
	/// コライダー2つの衝突判定と応答
	/// </summary>
	/// <param name="colliderA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheakCollisionPair(Collider* colliderA, Collider* colliderB);
	// コライダー
	std::list<Collider*> colliders_;
};
