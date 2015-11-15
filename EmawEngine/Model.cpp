#include "stdafx.h"
#include "Model.h"

Model::Model()
{
	m_Texture = NULL;
}

Model::Model(std::vector<VERTEX> vBuffer){
	this->vertexBuffer = vBuffer;
	m_Texture = NULL;
}

Model::~Model()
{
}

void* Model::load(std::string str) {
	// obj model loading
	ifstream fin;
	fin.open(str);
	if (!fin.good()) {
		OutputDebugString(L"model file not found\n");
		return NULL;
	}

	// index buffer positions.
	int ind = 0;
	int otherInd = 0;

	// create buffer to hold all vertices
	const int MAX_VERTICES = 15000;
	VERTEX vertices[MAX_VERTICES];
	int num_vertices = 0;

	// create buffer to hold uv coordinates
	const int MAX_UV = 40000;
	float us[MAX_UV];
	float vs[MAX_UV];
	int num_uv = 0;

	const int MAX_CHARS = 500;
	while (!fin.eof()) {
		char buf[MAX_CHARS];
		fin.getline(buf, MAX_CHARS);
		if (strlen(buf) == 0) {
			continue;
		}
		string prefix = strtok(buf, " ");

		// vertex
		if (prefix.compare("v") == 0) {
			
			vertices[num_vertices].X = stof(strtok(0, " "));
			vertices[num_vertices].Y = stof(strtok(0, " "));
			vertices[num_vertices].Z = stof(strtok(0, " "));
			vertices[num_vertices].W = 1.0f;

			if (strtok(0, " ")) {
				OutputDebugString(L"too many vertices in face; we need exactly three\n");
			}

			// generate random color
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			vertices[num_vertices].Color = DirectX::XMFLOAT4(r, g, b, 1.0f);

			num_vertices++;
			if (num_vertices >= MAX_VERTICES) {
				OutputDebugString(L"too many vertices in model\n");
				break;
			}
		}
		else if (prefix.compare("vt") == 0) {
			us[num_uv] = stof(strtok(0, " "));
			vs[num_uv] = stof(strtok(0, " "));

			if (strtok(0, " ")) {
				OutputDebugString(L"too many coordinates in texture vector. I don't know what to make of this, but it's bad.\n");
			}

			num_uv++;
			if (num_uv >= MAX_UV) {
				OutputDebugString(L"too many uv coordinates in model\n");
				break;
			}
		}
		else if (prefix.compare("f") == 0) {
			string entry = strtok(0, " ");
			int isQuad = 0;
			while (!entry.empty()){		

				// stoi will properly ignore the '/'
				int index = stoi(entry) - 1;
				// add uv data to vertex
				int uv_index = stoi(entry.substr(entry.find('/')+1)) - 1;
				vertices[index].U = us[uv_index];
				vertices[index].V = vs[uv_index];
				vertexBuffer.push_back(vertices[index]);
				indexBuffer.push_back(ind++);
				isQuad++;
				char* hold = strtok(0, " ");
				if (hold != NULL) entry = hold;
				else entry.clear();
			}
			if (isQuad == 4) fixForQuad(ind + otherInd);
			otherInd++;
			indexBuffer.push_back(-1);
		}
	}

	return NULL;
}

// If there were four face vertices it's a quad and we need
// to move around some of the indices for it to show up right.
void Model::fixForQuad(int index)
{
	int hold = indexBuffer.at(index - 4);
	indexBuffer.at(index - 4) = indexBuffer.at(index - 3);
	indexBuffer.at(index - 3) = indexBuffer.at(index - 2);
	indexBuffer.at(index - 2) = hold;
}

void Model::LoadTexture(ID3D11Device* device, string filename){

	m_Texture = new Texture(device);
	m_Texture->load(filename);

}

ID3D11ShaderResourceView* Model::GetTexture(){
	return m_Texture->getTexture();
}

void* Model::getData() {
	return NULL;
}

bool Model::unload() {
	vertexBuffer.clear();
	indexBuffer.clear();
	m_Texture = NULL;
	return true;
}

std::vector<VERTEX> Model::getVertexBuffer() {
	return vertexBuffer;
}

std::vector<int> Model::getIndexBuffer()
{
	return indexBuffer;
}