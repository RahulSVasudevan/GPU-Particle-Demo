#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "SimpleShader.h"

struct ParticleVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 StartPosition;
	DirectX::XMFLOAT4 Color;
	float Age;
	float Size;
	DirectX::XMFLOAT4 AngleOffset = {0,0,0,0};
};

class Emitter
{
	int maxParticles;
	float lifeTime;

	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	SimpleGeometryShader* particleGS;
	SimpleComputeShader* particleCS;

	ID3D11ShaderResourceView* particleTexture;
	ID3D11SamplerState* particleSampler;
	ID3D11BlendState* particleBlendState;
	ID3D11DepthStencilState* particleDepthState;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* computeBuffer;

	ID3D11UnorderedAccessView* computeUAV;

	ParticleVertex* vertices;

	public:
		Emitter(int maxParticles, float lifeTime, float size, XMFLOAT4 color,
			SimpleVertexShader* particleVS,
			SimplePixelShader* particlePS,
			SimpleGeometryShader* particleGS,
			SimpleComputeShader* particleCS,
			ID3D11ShaderResourceView* particleTexture,
			ID3D11SamplerState* particleSampler,
			ID3D11BlendState* particleBlendState,
			ID3D11DepthStencilState* particleDepthState,
			ID3D11Device* device, 
			int emitterType = 1);
		~Emitter();
		void Draw(ID3D11DeviceContext * context, Camera* camera, float deltaTime, float totalTime);
		
};