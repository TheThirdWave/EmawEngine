#pragma once

#include "stdafx.h"
#include "Transform.h"
#include "SceneGraphNode.h"
#include <string>

class ModelNode : public SceneGraphNode {

private:

	std::vector<SceneGraphNode*> children;
	Transform* m_Transform;
	Model* m_Model;
	ID3D11Buffer *m_VertBuffer;
	ID3D11Buffer* m_IndexBuffer;
	GraphicsDeviceInterface* gdi;

	float *posX, *posY, *posZ;
	float *scaleX, *scaleY, *scaleZ;
	float *rotX, *rotY, *rotZ;

public:
	ModelNode();
	ModelNode(Model*);
	~ModelNode();

	void update(D3DXMATRIX*);
	void render();
	void addChild(SceneGraphNode*);
	void setGraphicsDeviceInterface(GraphicsDeviceInterface*);
	void initializeVertexBuffer();
	void initializeIndexBuffer();

	void setPosition(float, float, float);
	void setRotation(float, float, float);
	void setScale(float, float, float);

	void rotateX(float);
	void rotateY(float);
	void rotateZ(float);
	void scale(float, float, float);
	void translate(float, float, float);

	void initializePosition();
	void initializeScale();
	void initializeRotation();

	void resetTransformMatrix();

};