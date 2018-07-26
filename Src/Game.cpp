#include "Game.h"
#include "Vertex.h"


using namespace DirectX;


Game::Game(HINSTANCE hInstance)
	: DXCore(hInstance,	"GPU Particle Demo",1280,720,true)			   
{
	vertexBuffer = 0;
	indexBuffer = 0;

#if defined(DEBUG) || defined(_DEBUG)
	CreateConsoleWindow(500, 120, 32, 120);
#endif
	
}


Game::~Game()
{
	delete emitter;
	delete emitter2;
	delete emitter3;

	delete particleVS;
	delete particleGS;
	delete particlePS;

	srv->Release();
	sampler->Release();
	
	if (vertexBuffer) { vertexBuffer->Release(); }
	if (indexBuffer) { indexBuffer->Release(); }

	particleBlendState->Release();
	particleDepthState->Release();
}


void Game::Init()
{
	LoadShaders();

	LoadTexture(device, context, L"../../Textures/StarParticle.jpg", 0, &srv);

	D3D11_SAMPLER_DESC sd = {}; // Zeros it out
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Tri-linear filtering
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = 16;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&sd, &sampler);

	CreateMatrices();
	
	
	light1.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
	light1.DiffuseColor = XMFLOAT4(0, 0, 1, 1);
	light1.Direction = XMFLOAT3(1,-1,0);

	light2.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
	light2.DiffuseColor = XMFLOAT4(0.8f, 1, 0, 1);
	light2.Direction = XMFLOAT3(-1, 0, 0);
	
	
	printf("Use WASD to move forward and strafe camera. Hold left mouse button and drag to rotate.\nSpace and X raise and lower the camera respectively");

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &particleBlendState);

	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	device->CreateDepthStencilState(&depthDesc, &particleDepthState);

	emitter = new Emitter(500000,12,0.1f,XMFLOAT4(1,0.2f,0.1f,1),particleVS,particlePS,particleGS,particleCS,srv,sampler, particleBlendState, particleDepthState,device,1);
	emitter2 = new Emitter(500000, 12, 0.1f, XMFLOAT4(0.2f, 0.2f, 1, 1), particleVS, particlePS, particleGS, particleCS, srv, sampler, particleBlendState, particleDepthState, device, 2);
	emitter3 = new Emitter(1000, 12, 0.5f, XMFLOAT4(1, 1, 1, 1), particleVS, particlePS, particleGS, particleCS, srv, sampler, particleBlendState, particleDepthState, device, 2);
		
	
}


void Game::LoadShaders()
{
	particleVS = new SimpleVertexShader(device, context);
	particleVS->LoadShaderFile(L"ParticleVertexShader.cso");

	particlePS = new SimplePixelShader(device, context);
	particlePS->LoadShaderFile(L"ParticlePixelShader.cso");

	particleGS = new SimpleGeometryShader(device, context);
	particleGS->LoadShaderFile(L"ParticleGeometryShader.cso");

	particleCS = new SimpleComputeShader(device, context);
	particleCS->LoadShaderFile(L"ParticleComputeShader.cso");
}

void Game::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext,const wchar_t* fileName, ID3D11Resource** resource, ID3D11ShaderResourceView** srv)
{


	CreateWICTextureFromFile(device, deviceContext, fileName, resource, srv);
}



void Game::CreateMatrices()
{

	camera.update();
	viewMatrix = camera.getViewMatrix();
	
	camera.updateProjectionMatrix((float)width / height);
	projectionMatrix = camera.getProjectionMatrix();

}





void Game::OnResize()
{
	DXCore::OnResize();

	camera.updateProjectionMatrix((float)width / height);
	projectionMatrix = camera.getProjectionMatrix();
}


void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
	else if (GetAsyncKeyState('A') & 0x8000 )
	{
		camera.strafeCamera(1, deltaTime);
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		camera.strafeCamera(-1, deltaTime);
	}
	else if (GetAsyncKeyState('W') & 0x8000)
	{
		camera.moveCameraForward(1,deltaTime);
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		camera.moveCameraForward(-1, deltaTime);
	}
	else if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		camera.raiseCamera(1, deltaTime);
	}
	else if (GetAsyncKeyState('X') & 0x8000)
	{
		camera.raiseCamera(-1, deltaTime);
	}
	
	
	viewMatrix = camera.getViewMatrix();



}


void Game::Draw(float deltaTime, float totalTime)
{	
	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	emitter->Draw(context,&camera,deltaTime,totalTime);
	emitter2->Draw(context, &camera, deltaTime, totalTime);
	emitter3->Draw(context, &camera, deltaTime, totalTime);

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hWnd);
}


void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{

	ReleaseCapture();
}

void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0001)
	{
		camera.setRotation(camera.getXRotation() + (x - prevMousePos.x)*0.001f, camera.getYRotation()+ (y - prevMousePos.y)*0.001f);
	}
	prevMousePos.x = x;
	prevMousePos.y = y;
}

void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
}
#pragma endregion