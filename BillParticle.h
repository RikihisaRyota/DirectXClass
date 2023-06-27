#pragma once
#include "WorldTransform.h"
#include "Vector3.h"
#include "Sprite.h"
#include <array>
class BillParticle {
	static const int kPaticles = 50;
	struct Particle {
		WorldTransform worldTransform_;
		Vector3 velocity_;
		int time_;
		bool IsAlive_;
		Sprite* sprite_;
	};
public:
	BillParticle();
	~BillParticle();
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initilaize();

	/// <summary>
	/// 生成
	/// </summary>
	/// <returns></returns>
	void Create();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="viewProjection"></param>
	/// <param name="flag">false:NotBillBoard</param>
	void Update(const ViewProjection& viewProjection, bool flag);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection, uint32_t textureHadl = 0);

	/// <summary>
	/// 消す
	/// </summary>
	void Destory(uint32_t index);
public:
	/// <summary>
	/// エミッターセッター
	/// </summary>
	/// <param name="position">position</param>
	void SetEmitter(const Vector3& position) { emitter_ = position; }
private:
	Vector3 emitter_;
	std::array<Particle, kPaticles> particle_;
	uint32_t createParticleCount;
};

