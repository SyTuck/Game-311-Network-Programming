#pragma once
#include "GameObject2D.h"

class ball :
	public GameObject2D
{
public:
	ball();
	~ball();

	void Update();

	void Reset();

	bool CheckWallCollision(); // returns true on back wall collision for goal
	bool CheckGOCollision(GameObject2D GO);

	int collisionCount; // i frames, prevent multiple collision detections
	Vec2D Velocity;
};

