#include "Paddle.h"
#include "..\Utils.h"

// processing events
#include "..\EventHandler.h"

Paddle::Paddle()
{
}

Paddle::~Paddle()
{
}

void Paddle::Update()
{

	if (playerNumber == 0)
	{
		if (EventHandler::events[PongEvents::W_PRESSED])
		{
			transform.position.y -= 1;
		}
		if (EventHandler::events[PongEvents::S_PRESSED])
		{
			transform.position.y += 1;
		}
	}
	else
	{
		if (EventHandler::events[PongEvents::UP_PRESSED])
		{
			transform.position.y -= 1;
		}
		if (EventHandler::events[PongEvents::DOWN_PRESSED])
		{
			transform.position.y += 1;
		}
	}

	if (transform.position.y + Dimensions.y >= WINDOW_HEIGHT)
	{
		transform.position.y = WINDOW_HEIGHT - (Dimensions.y + 1);
	}
	else if (transform.position.y <= 0)
	{
		transform.position.y = 1;
	}
}

void Paddle::SetPlayerNumber(int number)
{ 
	playerNumber = number;

	if (number == 0)
	{
		transform.position.x = 15;
	}
	else
	{
		transform.position.x = WINDOW_WIDTH - 30;
	}

	transform.position.y = 250;
}