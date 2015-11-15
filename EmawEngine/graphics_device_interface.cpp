#include "stdafx.h"
#include "graphics_device_interface.h"
#include "Font.h"

//#include <DirectXColors.h>
#include <DirectXMath.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3DX11core.h>

#define _XM_NO_INTRINSICS_

GraphicsDeviceInterface::GraphicsDeviceInterface() {
	m_SceneGraphRoot = 0;
	m_Camera = 0;
	m_DepthStencilView = 0;
	m_DepthStencilState = 0;
	m_DepthStencilBuffer = 0;
	m_VertBuffer = 0;
}

GraphicsDeviceInterface::~GraphicsDeviceInterface() {}

void GraphicsDeviceInterface::SetSceneGraphRoot(GameNode* root){
	m_SceneGraphRoot = root;
}

void GraphicsDeviceInterface::SetCamera(Camera* camera){
	m_Camera = camera;
}

//
// FUNCTION: GraphicsDeviceInterface::Initialize()
//
// PURPOSE: Initializes Direct3D
// 
bool GraphicsDeviceInterface::Initialize(HWND hWnd, WindowSize* wind) {
	HRESULT hResult;

	// Clear the struct
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// Set the swap chain values
	scd.BufferCount = 1;								// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// use 32 bit color
	scd.BufferDesc.Width = wind->getWidth();			// set width using windowSize object
	scd.BufferDesc.Height = wind->getHeight();			// set height using windowSize object
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// swap chain is output
	scd.OutputWindow = hWnd;							// window to render into
	scd.SampleDesc.Count = 4;							// use 4 multisamples for antialiasing
	scd.Windowed = wind->getWindowed();					// Sets windowed mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// Allow full-screen switching


	// Create the device, context, and swap chain
	hResult = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL, //D3D_FEATURE_LEVEL_10_0,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_Swapchain,
		&m_Device,
		NULL,
		&m_Context);

	if (hResult != S_OK)
	{
		return FALSE;
	}

	// Retrieves the IDXGIFactory that created "m_Device"
	IDXGIDevice *pDXGIDevice;
	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter *pDXGIAdapter;
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	IDXGIFactory *pDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pDXGIFactory);

	// Disables the use of Alt-Enter to switch between fullscreen/windowed
	pDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	
	// Resized the target (window or screen resolution) and back buffers
	m_Swapchain->ResizeTarget(&scd.BufferDesc);
	m_Swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, scd.Flags);

	// Get the back buffer address
	ID3D11Texture1D *pBackBuffer;
	m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// Use the back buffer address to create a render target
	m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_BackBuffer);
	pBackBuffer->Release();

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = wind->getWidth();
	depthBufferDesc.Height = wind->getHeight();
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 4;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer);

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);

	m_Context->OMSetDepthStencilState(m_DepthStencilState, 1);
	
	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, NULL, &m_DepthStencilView);

	// set the render target as the back buffer
	m_Context->OMSetRenderTargets(1, &m_BackBuffer, m_DepthStencilView);

	// Set the viewport using windowSize object
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = (float)wind->getWidth();
	viewport.Height = (float)wind->getHeight();

	m_Context->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	float fieldOfView = (float)D3DX_PI / 4.0f;
	float screenAspect = (float)wind->getWidth() / (float)wind->getHeight();

	// TODO: Make constants for screen depth and screen near.
	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&m_projMatrix, fieldOfView, screenAspect, 0.1f, 1000.0f);

	InitPipeline();
	InitGraphics();


	return TRUE;
}

//Placeholder used for testing, loads the shaders into the Graphics Device Context.
void GraphicsDeviceInterface::InitPipeline()
{
	//load shaders
	shdrs = new ShaderAsset(this);

	m_VertexShader = new VertexShader();
	m_VertexShader->initializeShader(m_Device);
}

ShaderStruct* GraphicsDeviceInterface::loadShaders(std::string str)
{
	ShaderStruct* blah = (ShaderStruct*)shdrs->load(str);
	return blah;
}

void GraphicsDeviceInterface::setShaders(ShaderStruct* shader)
{
	m_Context->VSSetShader(shader->VertShader, 0, 0);
	m_Context->PSSetShader(shader->PixShader, 0, 0);
	m_Context->GSSetShader(shader->GeoShader, 0, 0);

	m_Context->IASetInputLayout(shader->InputLayout);
}

//Placeholder used for testing, manually creates a triangle and sends the vertices for the Graphics Device for rendering.
void GraphicsDeviceInterface::InitGraphics(void)
{
	/*
	std::vector<VERTEX> vertices = e->getModel()->getVertexBuffer();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	m_Device->CreateBuffer(&bd, NULL, &m_VertBuffer);       // create the buffer

	D3D11_MAPPED_SUBRESOURCE ms;
	m_Context->Map(m_VertBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, OurVertices, 3 * sizeof(VERTEX));                // copy the data
	m_Context->Unmap(m_VertBuffer, NULL);
	*/
}

//
// FUNCTION: GraphicsDeviceInterface::Shutdown()
//
// PURPOSE: Releases all resources held by the GraphicsDeviceInterface
// 
void GraphicsDeviceInterface::Shutdown() {

	m_Swapchain->SetFullscreenState(FALSE, NULL);	// switch to windowed mode

	// close and release all COM objects
	m_Swapchain->Release();
	m_BackBuffer->Release();
	m_Device->Release();
	m_Context->Release();
	//m_VertBuffer->Release();
	m_DepthStencilView->Release();
	m_DepthStencilBuffer->Release();
}


//
// FUNCTION: GraphicsDeviceInterface::NextFrame()
//
// PURPOSE: Prepares the next frame to be rendered to screen
//
void GraphicsDeviceInterface::NextFrame()
{
	Render();
}

//
// FUNCTION: GraphicsDeviceInterface::IsWindowed()
//
// PURPOSE: returns false if fullscreen, true if windowed;
//
BOOL GraphicsDeviceInterface::IsWindowed()
{
	if (m_Swapchain == NULL) // this occurs when program is starting up
		return TRUE;

	BOOL fullscreen;
	m_Swapchain->GetFullscreenState(&fullscreen, NULL);

	return !fullscreen;
}

//
// FUNCTION: GraphicsDeviceInterface::Render()
//
// PURPOSE: Renders the next frame
//
bool GraphicsDeviceInterface::Render()
{

	// Default color
	float color[4] = { .5, .65, 1.0, 1.0 };
	
	// Clear the back buffer
	m_Context->ClearRenderTargetView(m_BackBuffer, color);
	m_Context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Render Camera
	m_Camera->Render();


	// Render SceneGraph
	m_SceneGraphRoot->render();

	// select which primtive type we are using
//	m_Context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// draw the vertex buffer to the back buffer
//	m_Context->Draw(36, 0);


	// TODO: Clear the depth buffer

	// TODO: Render game world

	// Swap buffers - waits for vsync
	//m_Swapchain->Present(1, 0);

	// Swap buffers - unlocked framerate
	m_Swapchain->Present(0, 0);

	return true;
}

bool GraphicsDeviceInterface::RenderModel(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer){

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	m_Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// select which primtive type we are using
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	return true;

}


bool GraphicsDeviceInterface::RenderVoxel(ID3D11Buffer* vertexBuffer){

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// select which primtive type we are using
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	return true;

}

int numIndicies = 2;

bool GraphicsDeviceInterface::Update(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, std::vector<VERTEX>* vertices, std::vector<int>* indicies){

	D3D11_MAPPED_SUBRESOURCE ms;
	m_Context->Map(vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, vertices->data(), vertices->size() * sizeof(VERTEX));    // copy the data
	m_Context->Unmap(vertexBuffer, NULL);
	
	m_Context->Map(indexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, indicies->data(), indicies->size() * sizeof(int));
	m_Context->Unmap(indexBuffer, NULL);

	// Render the triangle.
	m_Context->DrawIndexed(indicies->size(), 0, 0);
	//m_Context->DrawIndexed(numIndicies * 4, 0, 0);

	return true;
}

bool GraphicsDeviceInterface::Update(ID3D11Buffer* vertexBuffer, VERTEX* vertices, int size){

	D3D11_MAPPED_SUBRESOURCE ms;
	m_Context->Map(vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, vertices, size * sizeof(VERTEX));                // copy the data
	m_Context->Unmap(vertexBuffer, NULL);


	m_Context->Draw(size*8, 0);

	return true;
}

void GraphicsDeviceInterface::RenderShader(){
	
	// Set the vertex input layout.
	m_Context->IASetInputLayout(shdrs->Shaders.InputLayout);
	
	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_Context->VSSetShader(shdrs->Shaders.VertShader, NULL, 0);
	m_Context->PSSetShader(shdrs->Shaders.PixShader, NULL, 0);

	return;
}

void GraphicsDeviceInterface::VertexPipeline(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, std::vector<VERTEX>* vertices, std::vector<int>* indicies, D3DXMATRIX* transform, ID3D11ShaderResourceView* texture){
	
	//I want to rename these so they make a little more sense.
	RenderModel(vertexBuffer, indexBuffer);
//	m_VertexShader->initializeShader(m_Device);
	m_VertexShader->setParameters(m_Context, *transform, m_Camera->GetViewMatrix(), m_projMatrix);
	m_Context->PSSetShaderResources(1, 1, &texture);
	Update(vertexBuffer, indexBuffer, vertices, indicies);
	//RenderShader();
}

void GraphicsDeviceInterface::VoxelPipeline(ID3D11Buffer* vertexBuffer, VERTEX* vertices, int size, D3DXMATRIX* transform, ID3D11ShaderResourceView* texture){

	//I want to rename these so they make a little more sense.
	RenderVoxel(vertexBuffer);
//	m_VertexShader->initializeShader(m_Device);
	m_VertexShader->setParameters(m_Context, *transform, m_Camera->GetViewMatrix(), m_projMatrix);
	m_Context->PSSetShaderResources(0, 1, &texture);
	Update(vertexBuffer, vertices, size);
	//RenderShader();
}

ID3D11Buffer* GraphicsDeviceInterface::CreateVertexBuffer(int numOfVerticies){

	ID3D11Buffer* vertexBuffer;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * numOfVerticies;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	m_Device->CreateBuffer(&bd, NULL, &vertexBuffer);       // create the buffer

	return vertexBuffer;
}

ID3D11Buffer* GraphicsDeviceInterface::CreateIndexBuffer(int numOfVerticies){

	ID3D11Buffer* indexBuffer;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access for both CPU and GPU
	bd.ByteWidth = sizeof(int) * numOfVerticies;             // size is the number of vertices in the model, I think.
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;       // use as an index buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   // let CPU write to buffer

	m_Device->CreateBuffer(&bd, NULL, &indexBuffer);       // create the buffer

	return indexBuffer;
}

