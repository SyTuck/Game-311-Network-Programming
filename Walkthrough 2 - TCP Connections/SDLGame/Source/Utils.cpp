/*
This file contains functions which can be used for simple file I/O and functions which apply to many different uses.
*/

#include "Utils.h"
#include <stdio.h>

int Utils::LoadSprite(const char *file, SDL_Renderer *renderer, SDL_Point* dimensions, SDL_Texture** sprite)
{
	SDL_Surface *temp;

	// Load the sprite image
	temp = SDL_LoadBMP(file);
	if (temp == NULL)
	{
		fprintf(stderr, "Couldn't load %s: %s", file, SDL_GetError());
		return (-1);
	}

	dimensions->x = temp->w;
	dimensions->y = temp->h;

	// Set transparent pixel as the pixel at (0,0)
	if (temp->format->palette)
	{
		SDL_SetColorKey(temp, SDL_TRUE, *(Uint8 *)temp->pixels);
	}
	else
	{
		switch (temp->format->BitsPerPixel)
		{
		case 15:
			SDL_SetColorKey(temp, SDL_TRUE, (*(Uint16 *)temp->pixels) & 0x00007FFF);
			break;
		case 16:
			SDL_SetColorKey(temp, SDL_TRUE, *(Uint16 *)temp->pixels);
			break;
		case 24:
			SDL_SetColorKey(temp, SDL_TRUE, (*(Uint32 *)temp->pixels) & 0x00FFFFFF);
			break;
		case 32:
			SDL_SetColorKey(temp, SDL_TRUE, *(Uint32 *)temp->pixels);
			break;
		}
	}

	// Create textures from the image
	*sprite = SDL_CreateTextureFromSurface(renderer, temp);
	if (!sprite) 
	{
		fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
		SDL_FreeSurface(temp);
		return (-1);
	}

	SDL_FreeSurface(temp);

	return (0);
}