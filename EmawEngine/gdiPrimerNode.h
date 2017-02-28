#pragma once
#include "SceneGraphNode.h"
class gdiPrimerNode :
	public SceneGraphNode
{
public:
	gdiPrimerNode();
	~gdiPrimerNode();

	void update(D3DXMATRIX*);
	void render();
	void addChild(SceneGraphNode*);
	void setGraphicsDeviceInterface(GraphicsDeviceInterface*);
	void setShader(ShaderAsset*);

private:
	ShaderAsset* shader;
};
