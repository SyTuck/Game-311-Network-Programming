#pragma once
#include "SDL.h"

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

class Utils
{
public:
	static int LoadSprite(const char *file, SDL_Renderer *renderer, SDL_Point* dimensions, SDL_Texture** sprite);
};

