#pragma once
#include "SDL.h"
#include <string>

using namespace std;

struct Vec2D
{
	float x;
	float y;
};

struct Transform2D
{
	Vec2D position;
	Vec2D scale;
	Vec2D rotation;
};

class GameObject2D
{
public:
	GameObject2D();
	~GameObject2D();

	// used for any variables and data that needs to be setup on the object before we try to use it.
	void Init(string TextureFile, SDL_Renderer* renderer);

	// used to render the object to the screen
	void Draw(SDL_Window* window, SDL_Renderer* renderer);

	// used to update processing on the object (in case it's moving or has some processing to compute)
	void Update();

	// when the object is destroyed to ensure any memory allocated during it's lifetime is cleaned up.
	void Destroy();

	Transform2D GetTransform() { return transform; }
	SDL_Point GetDimensions() { return Dimensions; }
	void SetYPosition(float ypos) { transform.position.y = ypos; }			//direct access for position to put server in charge of the client's ball (and opponent paddle)
	void SetXPosition(float xpos) { transform.position.x = xpos; }

protected:
	// pos, rot, scale
	Transform2D transform;

	// width and height
	SDL_Point Dimensions;

	// loaded up sprite with information about the texture and it's data
	SDL_Texture* sprite;
};

