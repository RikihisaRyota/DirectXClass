struct Particle
{
    float4 position;
    float3 velocity;
};

RWStructuredBuffer<Particle> gParticle : register(u0);
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
     // パーティクルデータの読み取り
    Particle particleData = gParticle[DTid.x];

    // パーティクルの位置を更新
    particleData.position.xyz += particleData.velocity;

    // 更新されたパーティクルデータを書き戻し
    gParticle[DTid.x] = particleData;

}