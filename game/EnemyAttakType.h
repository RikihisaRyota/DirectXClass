#pragma once

#include "WorldTransform.h"

class Enemy;
class Player;
class EnemyAttack;
class EnemyAttackType {
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void SetPlayerEnemy(Player* player, Enemy* enemy, EnemyAttack* enemyAttack) {
		player_ = player, enemy_ = enemy, enemyAttack_=enemyAttack;
	}
	virtual bool GetWorking() { return IsWorking_; }
	virtual void SetWorking(bool working) { IsWorking_ = working; }
	virtual bool GetAttack() { return IsAttack_; }
	virtual void SetAttack(bool Attack) { IsAttack_ = Attack; }
	virtual bool GetHit() { return IsHit_; }
	virtual void SetHit(bool IsHit) { IsHit_ = IsHit; }
	
	virtual void UpdateHit() { 
		if (!IsHit_) {
			IsAttack_ = true;
		} else {
			IsAttack_ = false;
		}
	}

	virtual Player* GetPlayer() { return player_; }
	virtual Enemy* GetEnemy() { return enemy_; }
	virtual EnemyAttack* GetEnemyAttack() { return enemyAttack_; }

private:
	bool IsWorking_ = false;
	bool IsAttack_ = false;
	bool IsHit_ = false;
	Enemy* enemy_;
	Player* player_;
	EnemyAttack* enemyAttack_;
};