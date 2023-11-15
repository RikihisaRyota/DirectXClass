#include "CollisionManager.h"

#include "Collision.h"
#include "MyMath.h"

#include "ImGuiManager.h"

void CollisionManager::Update(Player* player, PlayerAttack* PlayerAttack, Block* block, std::vector<Enemy*> enemy, std::vector<EnemyAttack*> enemyAttack) {
	colliders_.clear();
	CheckAllCollisions(player, PlayerAttack, block, enemy, enemyAttack);
}

void CollisionManager::CheckAllCollisions(Player* player, PlayerAttack* PlayerAttack, Block* block, std::vector<Enemy*> enemy, std::vector<EnemyAttack*> enemyAttack) {
	// プレイヤーをリストに追加
	colliders_.emplace_back(player);
	if (player->GetBehavior() == Player::Behavior::kAttack) {
		colliders_.emplace_back(PlayerAttack);
	}
	// ブロックのリストを追加
	colliders_.emplace_back(block);
	// 敵
	for (size_t i = 0; i < enemy.size(); i++) {
		if (enemy.at(i)->GetIsAlive()) {
			colliders_.emplace_back(enemy.at(i));
			if (enemy.at(i)->GetBehavior() == Enemy::Behavior::kAttack) {
				colliders_.emplace_back(enemyAttack.at(i));
			}
		}
	}
	// リスト内総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		Collider* colliderA = *itrA;
		// イテレータBはイテレータAから回す
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			// ペアの当たり判定
			CheakCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheakCollisionPair(Collider* colliderA, Collider* colliderB) {
	uint32_t collisionAttributeA = colliderA->GetCollisionAttribute();
	uint32_t collisionAttributeB = colliderB->GetCollisionAttribute();
	// 衝突フィルタリング
	if (// 敵と敵の攻撃
		(collisionAttributeA & kCollisionAttributeEnemy &&
			collisionAttributeB & kCollisionAttributeEnemyAttack) ||

		(collisionAttributeB & kCollisionAttributeEnemy &&
			collisionAttributeA & kCollisionAttributeEnemyAttack) ||
		// プレイヤーとプレイヤーの攻撃
		(collisionAttributeA & kCollisionAttributePlayer &&
			collisionAttributeB & kCollisionAttributePlayerAttack) ||

		(collisionAttributeB & kCollisionAttributePlayer &&
			collisionAttributeA & kCollisionAttributePlayerAttack) ||
		// ブロックとブロック
		(collisionAttributeA & kCollisionAttributeBlock &&
			collisionAttributeB & kCollisionAttributeBlock)) {
		return;
	}
	for (uint32_t a = 0; a < colliderA->GetAABBSize(); a++) {
		for (uint32_t b = 0; b < colliderB->GetAABBSize(); b++) {
			AABB* aabbA = colliderA->GetAABB(a);
			AABB* aabbB = colliderB->GetAABB(b);
			// AABBで当たり判定
			if (IsCollision(*aabbA, *aabbB)) {
				OBB* obbA = colliderA->GetOBB(a);
				OBB* obbB = colliderB->GetOBB(b);
				// OBBで当たり判定
				if (IsCollision(*obbA, *obbB)) {
					// プレイヤーと敵
					if ((collisionAttributeA & kCollisionAttributePlayer) &&
						(collisionAttributeB & kCollisionAttributeEnemy)) {
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::PlayerToEnemy));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::PlayerToEnemy));
					}
					// プレイヤーと敵の攻撃
					else if ((collisionAttributeA & kCollisionAttributePlayer) &&
						(collisionAttributeB & kCollisionAttributeEnemyAttack)) {
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::PlayerToEnemyAttack));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::PlayerToEnemyAttack));
					}
					// 敵とプレイヤーの攻撃
					else if (
						(collisionAttributeA & kCollisionAttributePlayerAttack) &&
						(collisionAttributeB & kCollisionAttributeEnemy)) {
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::PlayerAttackToEnemy));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::PlayerAttackToEnemy));
					}
					else if ((collisionAttributeA & kCollisionAttributePlayer) &&
						(collisionAttributeB & kCollisionAttributeBlock)) {
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::PlayerToBlock));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::PlayerToBlock));
					}
					else if ((collisionAttributeA & kCollisionAttributeBlock) &&
						(collisionAttributeB & kCollisionAttributeEnemy)) {
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::EnemyToBlock));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::EnemyToBlock));
					}
				}
			}
		}
	}
}
