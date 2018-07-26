struct ParticleVertex
{
	float3 position;
	float3 startPosition;
	float4 color;
	float age;
	float size;
	float4 angleOffset;
};


cbuffer time : register(b1)
{
	float deltaTime;
	float lifeTime;
};


RWStructuredBuffer<ParticleVertex> computeBuffer: register(u0);

[numthreads(200, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	ParticleVertex v = computeBuffer[DTid.x];

	if (v.age > lifeTime)
		v.age -= lifeTime;
	v.age += deltaTime;

	float t = v.age;
	float theta = v.angleOffset.x;
	v.position = float3(t*sin(t + theta), t*cos(t + theta), 0) + v.startPosition;
	
	v.color.a = 1 - (t / lifeTime)*(t / lifeTime);

	computeBuffer[DTid.x] = v;
}