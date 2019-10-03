#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// for loading sprites
#include "Utils.h"

// processing events
#include "EventHandler.h"

// creating a basic gameobject
#include "Objects\GameObject2D.h"

#include "Objects\Paddle.h"
#include "Objects\ball.h"

#include "NetworkManager.h"

#include "SDL.h"
// GameState
bool GameRunning = true;

void DisplaySplashScreen(SDL_Window* mainWindow, SDL_Renderer* mainRenderer);

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;

	// create our basic SDL window, this is why we have both a console window and a window for our graphical game.
	// we will use this for both 2D and 3D rendering.
	window = SDL_CreateWindow("Basic SDL Project",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN);

	if (!window) 
	{
		return false;
	}

	// Creates the 2D renderer used to display SDL sprites.
	// We will use this for 2D rendering in front of the screen.
	// OpenGL however will use it's own rendered but share the same window as SDL.
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) 
	{
		return false;
	}

	NetworkManager::GetInstance()->Init();					//init (instantiate) our network manager singleton

	// Initialize the sprite positions
	srand(time(NULL));

	// this will run for 1500ms before allowing input
	DisplaySplashScreen(window, renderer);

	// create and initialize both paddles
	Paddle Paddles[2];
	Paddles[0].Init("Data\\Art\\paddle.bmp", renderer);
	Paddles[1].Init("Data\\Art\\paddle.bmp", renderer);

	// set the playernumbers of each paddle, this also positions them correctly.
	Paddles[0].SetPlayerNumber(0);
	Paddles[1].SetPlayerNumber(1);

	// create our Ball
	ball Ball;
	Ball.Init("Data\\Art\\ball.bmp", renderer);

	// Main game loop
	while (GameRunning) 
	{
		// The order of everything within this loop is very important.
		// Draw a black background
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);

		// handle button events
		GameRunning = EventHandler::Update();
		Paddles[0].Update();
		Paddles[1].Update();

		Ball.Update();
		Ball.CheckWallCollision();
		Ball.CheckGOCollision(Paddles[0]);
		Ball.CheckGOCollision(Paddles[1]);

		// update the drawn paddles
		Paddles[0].Draw(window, renderer);
		Paddles[1].Draw(window, renderer);

		Ball.Draw(window, renderer);

		// Update the screen!
		SDL_RenderPresent(renderer);

		// pause to control framerate
		SDL_Delay(2);
	}

	SDL_Quit();

	return true;
}

void DisplaySplashScreen(SDL_Window* mainWindow, SDL_Renderer* mainRenderer)
{
	// this object is defined inside the GameObject2D file.
	GameObject2D SplashScreen;
	SplashScreen.Init("Data\\Art\\SplashScreen.bmp", mainRenderer);

	// Draw a black background
	SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(mainRenderer);
	
	// draw our example game object
	SplashScreen.Draw(mainWindow, mainRenderer);

	// Update the screen!
	SDL_RenderPresent(mainRenderer);

	// pause to control framerate
	SDL_Delay(1500);
}