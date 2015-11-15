#pragma once
#include "Asset.h"
#include "Entity.h"
#include "texture.h"

class Model : public Asset
{
public:
	Model();
	Model(std::vector<VERTEX>);
	~Model();
	void* load(std::string);
	void* getData();
	bool unload();
	std::vector<VERTEX> getVertexBuffer();
	std::vector<int>	getIndexBuffer();
	void LoadTexture(ID3D11Device*, string);
	ID3D11ShaderResourceView* GetTexture();
	

private:
	void fixForQuad(int index);

	std::vector<VERTEX> vertexBuffer;
	std::vector<int>	indexBuffer;

	Texture* m_Texture;

};

