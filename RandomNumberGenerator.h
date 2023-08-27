#pragma once

#include <cstdint>
#include <random>

constexpr int32_t Int32Min = INT_MIN;
constexpr int32_t Int32Max = INT_MAX;
constexpr uint32_t UInt32Min = 0u;
constexpr uint32_t UInt32Max = UINT_MAX;
constexpr float FloatMin = -FLT_MAX;
constexpr float FloatMax = FLT_MAX;

class RandomNumberGenerator {
public:
	RandomNumberGenerator(uint32_t seed = 0) : engine_((seed == 0) ? seedGenerator_() : seed) {}

	void SetSeed(uint32_t seed) { engine_.seed(seed); }

	int32_t NextIntRange(int32_t min, int32_t max) {
		return std::uniform_int_distribution<int32_t>(min, max)(engine_);
	}
	int32_t NextIntLimit() { return NextIntRange(Int32Min, Int32Max); }

	uint32_t NextUIntRange(uint32_t min, uint32_t max) {
		return std::uniform_int_distribution<uint32_t>(min, max)(engine_);
	}
	uint32_t NextUIntLimit() { return NextUIntRange(UInt32Min, UInt32Max); }

	float NextFloatRange(float min, float max) {
		return std::uniform_real_distribution<float>(min, max)(engine_);
	}
	float NextFloatLimit() { return NextFloatRange(FloatMin, FloatMax); }
	float NextFloatUnit() { return NextFloatRange(0.0f, 1.0f); }

private:
	std::random_device seedGenerator_;
	std::mt19937 engine_;
};