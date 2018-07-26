#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "GameEntity.h"
#include <memory>
#include <iostream>
#include "Camera.h"
#include "Lights.h"
#include "WICTextureLoader.h"
#include "Emitter.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void LoadTexture(ID3D11Device*, ID3D11DeviceContext*, const wchar_t*, ID3D11Resource**, ID3D11ShaderResourceView**);
	void CreateMatrices();

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	//Shaders
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	SimpleGeometryShader* particleGS;
	SimpleComputeShader* particleCS;

	ID3D11BlendState* particleBlendState;
	ID3D11DepthStencilState* particleDepthState;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	POINT prevMousePos;

	DirectionalLight light1,light2;

	Camera camera;

	Emitter* emitter;
	Emitter* emitter2;
	Emitter* emitter3;

	int i = 0;
};

