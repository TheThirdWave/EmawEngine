#pragma once
#include "Model.h"

class Entity
{
public:
	Entity();
	~Entity();
	Vector orientation;
	Model* getModel();
	Vector position;
	DirectX::XMMATRIX* getMatrix();
	void update();
private:
	
	Vector scale;
	DirectX::XMMATRIX worldMatrix;
	void updateMatrix();
protected:
	Model* model;
};

