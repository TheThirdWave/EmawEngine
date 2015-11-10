#include "stdafx.h"
#include "VoxelChunkNode.h"

Texture* VoxelChunkNode::grass;

VoxelChunkNode::VoxelChunkNode(int inx, int iny)
{
	gridx = inx;
	gridy = iny;
	voxSize = VOXELSIZE;
	memset(ChunkBuffer, 0, sizeof(VERTEX) * 65025);
	transform = new Transform();
	float x = gridx * (2 * voxSize) * CHUNK_SIZE;
	float z = gridy * (2 * voxSize) * CHUNK_SIZE;
	float y = 0;
	transform->translate(x, y, z);
	length = 0;
}


VoxelChunkNode::~VoxelChunkNode()
{

}

void VoxelChunkNode::update(D3DXMATRIX* higherTransform)
{
	// Apply transform
	transform->createTransform();
	transform->applyTransformation(higherTransform);

	// Update children
	for (int i = 0; i < children.size(); i++){
		children.at(i)->update(transform->getTransformMatrix());
	}
}

void VoxelChunkNode::setGraphicsDeviceInterface(GraphicsDeviceInterface* graphicsDeviceInterface){
	gdi = graphicsDeviceInterface;
}

void VoxelChunkNode::render(){

	gdi->VoxelPipeline(vertBuffer, ChunkBuffer, length, transform->getTransformMatrix(), grass->getTexture());

	//Render children.
	for (int i = 0; i < children.size(); i++){
		children.at(i)->render();
	}
}

void VoxelChunkNode::loadTextures()
{
	grass = new Texture(gdi->m_Device);
	grass->load("models\\not_minecraft\\textures\\grass.png");
}

void VoxelChunkNode::loadChunkBuffer(VoxelMap* mapGenerator)
{
	Chunk* hold = mapGenerator->GetChunk(gridx, gridy);
	Chunk* other;
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			int center = hold->height_map[i][j] - 1;
			int lowest = center;
			if (i > 0 && hold->height_map[i - 1][j] - 1 < lowest) lowest = hold->height_map[i - 1][j] - 1;
			else if (i == 0 && gridx > 0)
			{
				other = mapGenerator->GetChunk(gridx - 1, gridy);
				if (other->height_map[CHUNK_SIZE-1][j] - 1 < lowest) lowest = other->height_map[CHUNK_SIZE-1][j] - 1;
			}
			if (i < CHUNK_SIZE-1 && hold->height_map[i + 1][j] - 1 < lowest) lowest = hold->height_map[i + 1][j] - 1;
			else if (i == CHUNK_SIZE-1 && gridx < 4)
			{
				other = mapGenerator->GetChunk(gridx + 1, gridy);
				if (other->height_map[0][j] - 1 < lowest) lowest = other->height_map[0][j] - 1;
			}
			if (j > 0 && hold->height_map[i][j - 1] - 1 < lowest) lowest = hold->height_map[i][j - 1] - 1;
			else if (j == 0 && gridy > 0)
			{
				other = mapGenerator->GetChunk(gridx, gridy - 1);
				if (other->height_map[i][CHUNK_SIZE-1] - 1 < lowest) lowest = other->height_map[i][CHUNK_SIZE-1] - 1;
			}
			if (j < CHUNK_SIZE-1 && hold->height_map[i][j + 1] - 1 < lowest) lowest = hold->height_map[i][j + 1] - 1;
			else if (j == CHUNK_SIZE-1 && gridy < 4)
			{
				other = mapGenerator->GetChunk(gridx, gridy + 1);
				if (other->height_map[i][0] - 1 < lowest) lowest = other->height_map[i][0] - 1;
			}
			for (int k = lowest; k <= center; k++)
			{
				ChunkBuffer[length].X = (float)i + voxSize;
				ChunkBuffer[length].Y = (float)k + voxSize;
				ChunkBuffer[length].Z = (float)j + voxSize;
				ChunkBuffer[length].W = voxSize;
				float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				ChunkBuffer[length].Color = { r, g, b, 1.0f };
				length++;
			}
		}
	}
	vertBuffer = gdi->CreateVertexBuffer(length);
}


//Old chunk loader, unoptimized, runs longer. probably because it creeps upwards slower (why is that happening?)

/*
void VoxelChunkNode::loadChunkBuffer(VoxelMap* mapGenerator)
{

	for (int i = 0; i < CHUNK_SIZE-1; i++)
	{
		for (int j = 0; j < CHUNK_SIZE-1; j++)
		{
			for (int k = 0; k < 256; k++)
			{
				if (mapGenerator->GetChunkValue(gridx, gridy, i, j, k) == 1)
				{
					ChunkBuffer[length].X = (float)i;
					ChunkBuffer[length].Y = (float)k;
					ChunkBuffer[length].Z = (float)j;
					ChunkBuffer[length].W = voxSize;
					float r = 0;// static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					float g = static_cast <float> (50 + 4*k) / static_cast <float> (255);
					float b = 0;// static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					ChunkBuffer[length].Color = { r, g, b, 1.0f };
					length++;
				}
			}
		}
	}
	vertBuffer = gdi->CreateVertexBuffer(length);
}*/
