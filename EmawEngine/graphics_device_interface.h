/* grahpics_device_interface. h */
#ifndef GRAPHICS_DEVICE_INTERFACE_H
#define GRAPHICS_DEVICE_INTERFACE_H

#include <d3d11.h>
#include <D3DX10.h>
#include "ShaderAsset.h"
#include "VertexShader.h"
#include "GameNode.h"
#include "Camera.h"

/* Globals */

/* GraphicsDeviceInterface */
class GraphicsDeviceInterface
{
	friend class ShaderAsset;
public: 
	GraphicsDeviceInterface();
	~GraphicsDeviceInterface();

	bool Initialize(HWND, WindowSize*);
	void SetSceneGraphRoot(GameNode*);
	void SetCamera(Camera* camera);
	void Shutdown();
	void NextFrame();
	void InitPipeline();
	void InitGraphics();
	ShaderStruct* loadShaders(std::string str);
	void setShaders(ShaderStruct*);
	bool Update(ID3D11Buffer*, VERTEX*, int);
	bool Update(ID3D11Buffer*, ID3D11Buffer*, std::vector<VERTEX>*, std::vector<int>*);
	void VoxelPipeline(ID3D11Buffer*, VERTEX*, int, D3DXMATRIX*, ID3D11ShaderResourceView*);
	void VertexPipeline(ID3D11Buffer*, ID3D11Buffer*, std::vector<VERTEX>*, std::vector<int>*, D3DXMATRIX*, ID3D11ShaderResourceView*);
	ID3D11Buffer* CreateVertexBuffer(int numOfVerticies);
	ID3D11Buffer* CreateIndexBuffer(int numOfVerticies);
	BOOL IsWindowed();

	ID3D11Device *m_Device;

private:
	bool Render();
	bool RenderModel();
	bool RenderModel(ID3D11Buffer*, ID3D11Buffer*);
	bool RenderVoxel(ID3D11Buffer*);
	void RenderShader();

	DXGI_SWAP_CHAIN_DESC	scd;
	ShaderAsset				*shdrs;
	IDXGISwapChain			*m_Swapchain;
	ID3D11DeviceContext		*m_Context;
	ID3D11RenderTargetView  *m_BackBuffer;
	ID3D11Buffer			*m_VertBuffer;
	ID3D11DepthStencilView	*m_DepthStencilView;
	ID3D11DepthStencilState *m_DepthStencilState;
	ID3D11Texture2D			*m_DepthStencilBuffer;

	VertexShader			*m_VertexShader;
	GameNode				*m_SceneGraphRoot;
	Camera					*m_Camera;
	D3DXMATRIX				m_projMatrix;
};

#endif