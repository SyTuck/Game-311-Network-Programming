#include "ball.h"
#include "..\Utils.h"

ball::ball()
{
	Reset();
}

ball::~ball()
{
}

void ball::Reset()
{
	Velocity.x = 0.2f;
	Velocity.y = 0.2f;

	transform.position.x = WINDOW_WIDTH / 2;
	transform.position.y = WINDOW_HEIGHT / 2;

	collisionCount = 0;
}

void ball::Update()
{
	transform.position.y += Velocity.y;
	transform.position.x += Velocity.x;
}

// returns true on back wall collision for goal
bool ball::CheckWallCollision()
{
	// side hits which result in a goal
	if (transform.position.x + Dimensions.x >= WINDOW_WIDTH)
	{
		transform.position.x = (float) (WINDOW_WIDTH - (Dimensions.x+1));
		// goal scored, reset
		Reset();
	}
	else if (transform.position.x <= 0)
	{
		transform.position.x = 1;
		// goal scored, reset
		Reset();
	}

	// top or bottom hit reflection
	if (transform.position.y + Dimensions.y >= WINDOW_HEIGHT)
	{
		transform.position.y = (float) (WINDOW_HEIGHT - (Dimensions.y + 1));
		Velocity.y *= -1.0f;
	}
	else if (transform.position.y <= 0)
	{
		transform.position.y = 1;
		Velocity.y *= -1.0f;
	}
	return false;
}

bool ball::CheckGOCollision(GameObject2D GO)
{
	if (collisionCount > 0)
	{
		collisionCount--;
		return false;
	}

	// check if outside X low
	if (transform.position.x + Dimensions.x < GO.GetTransform().position.x)
	{
		return false;
	}
	// check if outside X High
	if (transform.position.x > GO.GetTransform().position.x + GO.GetDimensions().x)
	{
		return false;
	}
	// check if outside Y low
	if (transform.position.y + Dimensions.y < GO.GetTransform().position.y)
	{
		return false;
	}
	// check if outside X High
	if (transform.position.y > GO.GetTransform().position.y + GO.GetDimensions().y)
	{
		return false;
	}

	// 15% increase to speed on paddle hit.	
	Velocity.x *= -1.15f;
	Velocity.y *= 1.15f;

	// i frames, prevent multiple collision detections
	collisionCount = 120;

	return true;
}
