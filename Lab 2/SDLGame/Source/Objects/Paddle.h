#pragma once
#include "GameObject2D.h"

class Paddle :
	public GameObject2D
{
public:
	Paddle();
	~Paddle();

	void Update();
	void ForcePaddleKey();

	void SetPlayerNumber(int number); 

	int playerNumber = 1;
};

