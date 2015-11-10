#include "stdafx.h"
#include "CollisionManager.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool CollisionManager::instanceFlag = false;
CollisionManager* CollisionManager::instance = NULL;

CollisionManager::CollisionManager(){
	xzCounter = 0;
	xzWalls = NULL;
	level = -1;
	floor = 1;
	ceiling = -1;
}


CollisionManager::~CollisionManager(){
	for (int i = 0; i < xzCounter; i++){
		delete xzWalls[i];
	}
	delete[] xzWalls;

	list<MovingCollidable*>::iterator i;
	for (i = movingCollidables.begin(); i != movingCollidables.end(); ++i){
		delete *i;
	}
}

// Returns the instance of our CollisionManager
CollisionManager* CollisionManager::getInstance() {
	if (!instanceFlag)
	{
		instance = new CollisionManager();
		instanceFlag = true;
	}
	return instance;
}

// Adding moving collidable
void CollisionManager::addMovingCollidable(MovingCollidable* mc){
	movingCollidables.push_back(mc);
}

// Removing moving collidable
void CollisionManager::removeMovingCollidable(MovingCollidable* mc){
	movingCollidables.remove(mc);
}

// Checking of collisions for single moving collidable
// Collisions between moving collidables are skipped
bool CollisionManager::checkCollisions(){
	return checkXZCollisions() || checkYCollisions();
}

// Checking of collisions for all moving collidables
// Collisions between moving collidables are skipped
bool CollisionManager::checkCollisions(MovingCollidable* mc){
	return checkXZCollisions(mc) || checkYCollisions(mc);
}

// Checking of wall collisions for single moving collidable
bool CollisionManager::checkXZCollisions(MovingCollidable* mc){
	for (int j = 0; j < xzCounter; j++){
		if (xzWalls[j]->collide(mc)){
			return true;
		}
	}
	return false;
}

// Checking of wall collisions for all moving collidables
bool CollisionManager::checkXZCollisions(){
	bool res = false;
	list<MovingCollidable*>::iterator i;
	for (i = movingCollidables.begin(); i != movingCollidables.end(); ++i){
		if (checkXZCollisions(*i)){
			res = true;
		}
	}
	return res;
}

// Checking of floor/ceiling collisions for single moving collidable
bool CollisionManager::checkYCollisions(MovingCollidable* mc){
	if (mc->getY() >= ceiling){
		mc->pushBack(mc->getX(), mc->getLastY() - 0.0002f, mc->getZ());
		return true;
	}
	if ((mc->getY() - mc->getHeight()) <= floor){
		mc->pushBack(mc->getX(), mc->getLastY() + 0.0002f, mc->getZ());
		return true;
	}
	return false;
}

// Checking of floor/ceiling collisions for all moving collidables
bool CollisionManager::checkYCollisions(){
	bool res = false;
	list<MovingCollidable*>::iterator i;
	for (i = movingCollidables.begin(); i != movingCollidables.end(); ++i){
		if (checkYCollisions(*i)){
			res = true;
		}
	}
	return res;
}

// Checking of collisions between moving collidables
bool CollisionManager::checkMovingCollisions(){
	while (checkMovingCollisionsInner());
	return false;
}

// Checking collisions between moving collidables, supporting inner function
bool CollisionManager::checkMovingCollisionsInner(){
	bool res = false;
	list<MovingCollidable*>::iterator i;
	list<MovingCollidable*>::iterator j;
	for (i = movingCollidables.begin(); i != movingCollidables.end(); ++i){
		for (j = ++i; j != movingCollidables.end(); ++j){
			res = res || ((*i)->collide(*j));
		}
	}
	return res;
}

bool CollisionManager::checkVMapCollision(BoxCollidable* obj, VoxelMap* map)
{
	Vector curPos;
	curPos.x = obj->getX();
	curPos.y = obj->getY();
	curPos.z = obj->getZ();
	Vector lastPos;
	lastPos.x = obj->getLastX();
	lastPos.y = obj->getLastY();
	lastPos.z = obj->getLastZ();
	Vector dimensions = obj->getDimensions();
	Vector movVec = VectorSubtract(curPos, lastPos);

	Chunk* chunk = map->GetChunk(curPos.x / CHUNK_SIZE, curPos.z / CHUNK_SIZE);
	int chunkX = chunk->coord_X;
	int chunkY = chunk->coord_Y;

	for (int i = (int)(curPos.x - dimensions.x * 2) % CHUNK_SIZE; i <= (int)(curPos.x + dimensions.x * 2) % (CHUNK_SIZE + 1); i++)
	{
		for (int j = (int)(curPos.y - dimensions.y * 2) % CHUNK_HEIGHT; j <= (int)(curPos.y + dimensions.y * 2) % (CHUNK_HEIGHT + 1); j++)
		{
			for (int k = (int)(curPos.z - dimensions.z * 2) % CHUNK_SIZE; k <= (int)(curPos.z + dimensions.z * 2) % (CHUNK_SIZE + 1); k++)
			{
				if (chunk->chunk[i][j][k] == 1)
				{
					//Get actual location of box.
					float boxX = (int)(curPos.x / CHUNK_SIZE) * CHUNK_SIZE + i + VOXELSIZE;
					float boxY = (int)(curPos.y / CHUNK_HEIGHT) * CHUNK_HEIGHT + j + VOXELSIZE;
					float boxZ = (int)(curPos.z / CHUNK_SIZE) * CHUNK_SIZE + k + VOXELSIZE;
					//first check to see if the current position is actually colliding with the block. If it isn't, continue to the next block.
					float closeX = findMinDiff(curPos.x - dimensions.x, curPos.x + dimensions.x, boxX);
					float closeY = findMinDiff(curPos.y - dimensions.y, curPos.y + dimensions.y, boxY);
					float closeZ = findMinDiff(curPos.z - dimensions.z, curPos.z + dimensions.z, boxZ);
					if (!(closeX >= boxX - VOXELSIZE && closeX <= boxX + VOXELSIZE) || !(closeY >= boxY - VOXELSIZE && closeY <= boxY + VOXELSIZE) || !(closeZ >= boxZ - VOXELSIZE && closeZ <= boxZ + VOXELSIZE)) continue;

					//then get the sides of the box closest to the collision point.
					closeX = findMinDiff(lastPos.x - dimensions.x, lastPos.x + dimensions.x, boxX);
					closeY = findMinDiff(lastPos.y - dimensions.y, lastPos.y + dimensions.y, boxY);
					closeZ = findMinDiff(lastPos.z - dimensions.z, lastPos.z + dimensions.z, boxZ);

					//Now get the direction the box was moving and use that vector to move the box to the correct side of the block...I think, it's been awhile since I've looked at this.
					float coeff;
					(movVec.x != 0) ? coeff = (findMinDiff(boxX - VOXELSIZE, boxX + VOXELSIZE, closeX) - closeX) / movVec.x : coeff = 0;
					Vector midway = VectorAdd(lastPos, ScalarMultiply(movVec, coeff));
					if (coeff >= 0 && (midway.y >= boxY - VOXELSIZE && midway.y <= boxY + VOXELSIZE && midway.z >= boxZ - VOXELSIZE && midway.z <= boxZ + VOXELSIZE))
					{
						obj->pushBack(midway.x, curPos.y, curPos.z);
						continue;
					}
					(movVec.y != 0) ? coeff = (findMinDiff(boxY - VOXELSIZE, boxY + VOXELSIZE, closeY) - closeY) / movVec.y : coeff = 0;
					midway = VectorAdd(lastPos, ScalarMultiply(movVec, coeff));
					if (coeff >= 0 && (midway.x >= boxX - VOXELSIZE && midway.x <= boxX + VOXELSIZE && midway.z >= boxZ - VOXELSIZE && midway.z <= boxZ + VOXELSIZE))
					{
						obj->pushBack(curPos.x, midway.y, curPos.z);
						continue;
					}
					(movVec.z != 0) ? coeff = (findMinDiff(boxZ - VOXELSIZE, boxZ + VOXELSIZE, closeZ) - closeZ) / movVec.z : coeff = 0;
					midway = VectorAdd(lastPos, ScalarMultiply(movVec, coeff));
					if (coeff >= 0 && (midway.x >= boxX - VOXELSIZE && midway.x <= boxX + VOXELSIZE && midway.y >= boxY - VOXELSIZE && midway.y <= boxY + VOXELSIZE))
					{
						obj->pushBack(curPos.x, curPos.y, midway.z);
						continue;
					}
				}
			}
		}
	}
	return false;
}

// Loading of static collision setting for level
void CollisionManager::loadLevel(int id){
	float a, b, c, d;

	level = id;

	fstream xzfile("collision/level" + to_string(id) + ".txt", ios_base::in);
	if (xzfile.is_open()){
		for (int i = 0; i < xzCounter; i++){
			delete xzWalls[i];
		}
		delete[] xzWalls;

		xzfile >> ceiling;
		xzfile >> floor;
		xzfile >> xzCounter;

		xzWalls = new Wall2D*[xzCounter];
		for (int i = 0; i < xzCounter; i++){
			xzfile >> a;
			xzfile >> b;
			xzfile >> c;
			xzfile >> d;
			xzWalls[i] = new Wall2D(a, b, c, d);
		}
		xzfile.close();
	}
}