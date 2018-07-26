#include "Emitter.h"

Emitter::Emitter(int maxParticles, float lifeTime, float size, XMFLOAT4 color,
	SimpleVertexShader * particleVS, 
	SimplePixelShader * particlePS, 
	SimpleGeometryShader * particleGS, 
	SimpleComputeShader* particleCS, 
	ID3D11ShaderResourceView * particleTexture, 
	ID3D11SamplerState * particleSampler, 
	ID3D11BlendState * particleBlendState, 
	ID3D11DepthStencilState * particleDepthState, 
	ID3D11Device* device,
	int emitterType)
{
	this->maxParticles = maxParticles;
	this->lifeTime = lifeTime;
	this->particleVS = particleVS;
	this->particlePS = particlePS;
	this->particleGS = particleGS;
	this->particleCS = particleCS;
	this->particleTexture = particleTexture;
	this->particleSampler = particleSampler;
	this->particleBlendState = particleBlendState;
	this->particleDepthState = particleDepthState;

	
	// Initial Vertices
	vertices = new ParticleVertex[maxParticles];
	for (int i = 0;i < maxParticles;i++)
	{
		ParticleVertex v;

		v.Age = (i* lifeTime)/maxParticles;

		float randmax = 0.6f;
		float x = ((float)rand() / RAND_MAX) * randmax;
		float y = ((float)rand() / RAND_MAX) * randmax;
		float z = ((float)rand() / RAND_MAX) * randmax;
		v.StartPosition = XMFLOAT3(x, y, z);
		
		v.Position = v.StartPosition;
		v.Size = size;
		v.Color = color;

		if (emitterType == 1)
		{
			int randomInt = ((float)rand() / RAND_MAX) * 4;
			v.AngleOffset.x = randomInt * 1.57;
		}
		else if(emitterType == 2)
		{
			v.AngleOffset.x = ((float)rand() / RAND_MAX) * 4 * 1.57;
		}
		
		
		vertices[i] = v;
	}
	

	// Vertex Buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(ParticleVertex) * maxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;
	device->CreateBuffer(&vbd, nullptr, &vertexBuffer);


	// UAV buffer for compute shader
	D3D11_BUFFER_DESC cbd;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ParticleVertex) * maxParticles;
	cbd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = sizeof(ParticleVertex);
	cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	device->CreateBuffer(&cbd, &initialVertexData, &computeBuffer);


	// UAV

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

	device->CreateUnorderedAccessView(computeBuffer, &uavDesc, &computeUAV);
}

Emitter::~Emitter()
{
	computeUAV->Release();
	vertexBuffer->Release();
	computeBuffer->Release();
	
	particleVS = nullptr;
	particlePS = nullptr;
	particleGS = nullptr;
	particleCS = nullptr;
	particleTexture = nullptr;
	particleSampler = nullptr;
	particleBlendState = nullptr;
	particleTexture = nullptr;

	delete particleVS;
	delete particlePS;
	delete particleGS;
	delete particleCS;
	delete particleTexture;
	delete particleSampler;
	delete particleBlendState;
	delete particleTexture;

	delete[] vertices;
	vertices = nullptr;
	
}



void Emitter::Draw(ID3D11DeviceContext * context, Camera * camera, float deltaTime, float totalTime)
{
	
	particleCS->SetUnorderedAccessView("computeBuffer", computeUAV, 0);
	particleCS->SetFloat("deltaTime", deltaTime);
	particleCS->SetFloat("lifeTime", lifeTime);
	particleCS->SetShader();
	particleCS->CopyAllBufferData();
	//computeShader->DispatchByThreads(3, 1, 1);
	context->Dispatch(maxParticles/200, 1, 1);
	particleCS->SetUnorderedAccessView("computeBuffer", 0, 0);

	context->CopyResource(vertexBuffer, computeBuffer);
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	float factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(particleBlendState, factor, 0xffffffff);
	context->OMSetDepthStencilState(particleDepthState, 0);

	particleVS->CopyAllBufferData();
	particleVS->SetShader();

	particleGS->SetMatrix4x4("view", camera->getViewMatrix());
	particleGS->SetMatrix4x4("projection", camera->getProjectionMatrix());
	particleGS->CopyAllBufferData();
	particleGS->SetShader();

	particlePS->SetSamplerState("trilinear", particleSampler);
	particlePS->SetShaderResourceView("particleTexture", particleTexture);
	particlePS->CopyAllBufferData();
	particlePS->SetShader();
		
	
	context->Draw(maxParticles,0);
	
}
