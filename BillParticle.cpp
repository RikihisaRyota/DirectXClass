#include "BillParticle.h"
#include <cstdlib>
#include <ctime>

#include "ImGuiManager.h"
#include "MyMath.h"

BillParticle::BillParticle() {
	Initilaize();
}

BillParticle::~BillParticle() {
	for (auto& particle : particle_) {
		if (particle.sprite_ != nullptr) {
		}
	}
}

void BillParticle::Initilaize() {
	// 現在の時刻をシード値として設定
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	emitter_ = Vector3(0.0f, 0.0f, 0.0f);
	for (auto& particle : particle_) {
		particle.velocity_ = Vector3(0.0f, 0.0f, 0.0f);
		particle.worldTransform_.Initialize();
		particle.time_ = 0;
		particle.IsAlive_ = false;
		particle.sprite_ = nullptr;
	}
}

void BillParticle::Create() {
	// 何フレーム間隔で生成するか
	const uint32_t CreateParticleinterval = 15;

	for (auto& particle : particle_) {
		if (!particle.IsAlive_ && CreateParticleinterval <= createParticleCount) {
			// 座標
			particle.worldTransform_.translation_ = emitter_;
			// 速度と向き
			Vector3 velocity = Vector3(
				static_cast<float>(std::rand()) / RAND_MAX * 200.0f - 100.0f,
				static_cast<float>(std::rand()) / RAND_MAX * 200.0f - 100.0f,
				static_cast<float>(std::rand()) / RAND_MAX * 200.0f - 100.0f
			);
			particle.velocity_ = Normalize(velocity) * 0.5f;
			// 寿命
			particle.time_ = /*std::rand() % 15 + 15*/30;
			// フラグ
			particle.IsAlive_ = true;
			// Plane生成
			particle.sprite_ = PlaneRenderer::Create();
			// 生成カウントリセット
			createParticleCount = 0;
			break;
		}
	}
	createParticleCount++;
}

void BillParticle::Update(const ViewProjection& viewProjection) {
	uint32_t count = 0;
	for (auto& particles : particle_) {
		if (particles.IsAlive_) {
			particles.time_--;
			if (particles.time_ <= 0) {
				Destory(count);
				break;
			}
			particles.worldTransform_.translation_ += particles.velocity_;
			/*Matrix4x4 Bill = MakeBillboard(particles.worldTransform_.translation_, viewProjection.translation_, Vector3(0.0f, 1.0f, 0.0f));
			Matrix4x4 worldTransformAffin = MakeAffineMatrix(particles.worldTransform_.scale_, particles.worldTransform_.rotation_, particles.worldTransform_.translation_);
			particles.worldTransform_.matWorld_ = Bill * worldTransformAffin;
			particles.worldTransform_.TransferMatrix();*/
			particles.worldTransform_.UpdateMatrix();
			/*if (count % 2 == 0) {
				Matrix4x4 Bill = MakeBillboard(viewProjection.translation_, particles.worldTransform_.translation_, Vector3(0.0f, 1.0f, 0.0f));
				Matrix4x4 worldTransformAffin = MakeAffineMatrix(particles.worldTransform_.scale_, particles.worldTransform_.rotation_, particles.worldTransform_.translation_);
				particles.worldTransform_.matWorld_ = Bill * worldTransformAffin;
				particles.worldTransform_.TransferMatrix();
			}
			else {
				particles.worldTransform_.UpdateMatrix();
			}*/
		}
		count++;
	}
}

void BillParticle::Draw(const ViewProjection& viewProjection, uint32_t textureHadl) {
	for (auto& particles : particle_) {
		if (particles.IsAlive_) {
			particles.sprite_->Draw(particles.worldTransform_, viewProjection, textureHadl);
		}
	}
}

void BillParticle::Destory(uint32_t index) {
	particle_[index].worldTransform_.Initialize();
	particle_[index].velocity_ = Vector3(0.0f, 0.0f, 0.0f);
	particle_[index].time_ = 0;
	particle_[index].IsAlive_ = false;
}