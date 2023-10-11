#include "CollisionManager.h"

#include "Collision.h"
#include "MyMath.h"

#include "ImGuiManager.h"

void CollisionManager::Update(Player* player, Block* block, Enemy* enemy, EnemyAttack* enemyAttack) {
	colliders_.clear();
	CheckAllCollisions(player, block, enemy, enemyAttack);
}

void CollisionManager::CheckAllCollisions(Player* player, Block* block, Enemy* enemy, EnemyAttack* enemyAttack) {
	// プレイヤーをリストに追加
	colliders_.emplace_back(player);
	// ブロックのリストを追加
	colliders_.emplace_back(block);
	// 敵
	colliders_.emplace_back(enemy);
	if (enemy->GetBehavior() == Enemy::Behavior::kAttack) {
		colliders_.emplace_back(enemyAttack);
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
			AABB* aabbA;
			AABB* aabbB;
			if (colliderA->GetWorldTransform().parent_) {
				Matrix4x4 current = colliderA->GetWorldTransform().matWorld_;
				Matrix4x4 parent = colliderA->GetWorldTransform().parent_->matWorld_;
				Matrix4x4 colliderAMatrix = current * parent;
				colliderA->SetTranslation(MakeTranslateMatrix(colliderAMatrix));
				colliderA->HitBoxUpdate();
				if (colliderB->GetWorldTransform().parent_) {
					current = colliderB->GetWorldTransform().matWorld_;
					parent = colliderB->GetWorldTransform().parent_->matWorld_;
					colliderAMatrix = current * parent;
					colliderB->SetTranslation(MakeTranslateMatrix(colliderAMatrix));
					colliderB->HitBoxUpdate();
				}
				aabbA = colliderA->GetAABB(a);
				aabbB = colliderB->GetAABB(b);
			}
			else {
				aabbA = colliderA->GetAABB(a);
				aabbB = colliderB->GetAABB(b);
			}

			// AABBで当たり判定
			if (IsCollision(*aabbA, *aabbB)) {
				OBB* obbA;
				OBB* obbB;
				if (colliderA->GetWorldTransform().parent_) {
					Matrix4x4 current = colliderA->GetWorldTransform().matWorld_;
					Matrix4x4 parent = colliderA->GetWorldTransform().parent_->matWorld_;
					Matrix4x4 colliderAMatrix = current * parent;
					colliderA->SetTranslation(MakeTranslateMatrix(colliderAMatrix));
					colliderA->HitBoxUpdate();
					if (colliderB->GetWorldTransform().parent_) {
						current = colliderB->GetWorldTransform().matWorld_;
						parent = colliderB->GetWorldTransform().parent_->matWorld_;
						colliderAMatrix = current * parent;
						colliderB->SetTranslation(MakeTranslateMatrix(colliderAMatrix));
						colliderB->HitBoxUpdate();
					}
					obbA = colliderA->GetOBB(a);
					obbB = colliderB->GetOBB(b);
				}
				else {
					obbA = colliderA->GetOBB(a);
					obbB = colliderB->GetOBB(b);
				}

				// OBBで当たり判定
				if (IsCollision(*obbA, *obbB)) {
					// プレイヤーと敵の攻撃
					if ((collisionAttributeA & kCollisionAttributePlayer) &&
						(collisionAttributeB & kCollisionAttributeEnemyAttack)) {
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::PlayerToEnemyAttack));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::PlayerToEnemyAttack));
					}
					// 敵とプレイヤーの攻撃
					else if (
						(collisionAttributeA & kCollisionAttributeEnemy) &&
						(collisionAttributeB & kCollisionAttributePlayerAttack)) {
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::EnemyToPlayerAttack));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::EnemyToPlayerAttack));
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
					else {
						// プレイヤーと敵の衝突
						colliderA->OnCollision(
							*obbB, colliderB->GetWorldTransform(b), static_cast<uint32_t>(Collider::Type::PlayerToEnemy));
						colliderB->OnCollision(
							*obbA, colliderA->GetWorldTransform(a), static_cast<uint32_t>(Collider::Type::PlayerToEnemy));
					}
				}
			}
		}
	}
}
