#include <stdlib.h>
#include <iostream>
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

struct ServerDataStruct
{
	unsigned short ballPos;
	unsigned short paddle0Pos;
	unsigned short paddle1Pos;
	unsigned char  score;
	unsigned char  gameState;
} serverData;

struct ClientData
{
	  signed char keyPress;
	unsigned char userCmd;
} clientData;

#include "SDL.h"
// GameState
bool GameRunning = true;
bool networked = true;
bool server = true;

void DisplaySplashScreen(SDL_Window* mainWindow, SDL_Renderer* mainRenderer);

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;

	char sChoice;

	std::cout << "Is this a network game? (Y/n)? ";
	std::cin >> sChoice;
	std::cout << endl;
	if ((sChoice == 'n') || (sChoice == 'N'))
	{
		networked = false;
	}
	else
	{
		std::cout << endl;
		std::cout << "Are you the server (Y/n)? ";
		std::cin >> sChoice;
		std::cout << endl;

		if ((sChoice == 'n') || (sChoice == 'N'))
		{
			server = false;
		}
	}
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

	if (networked)
	{
		NetworkManager::GetInstance()->Init();					//init (instantiate) our network manager singleton
		if (server)
		{
			NetworkManager::GetInstance()->BindSockets();
		}
	}
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

	if (networked)
	{
		if (server)
		{
			NetworkManager::GetInstance()->AcceptConnections();
		}

		else
		{
			NetworkManager::GetInstance()->ConnectSockets();
		}
	}



	// Main game loop
	while (GameRunning)
	{
		// The order of everything within this loop is very important.
		// Draw a black background
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);

		GameRunning = EventHandler::Update();												// handle button events

		if (networked)																		//if running networked, remap all keys to player 0
		{																					//so everyone plays from only one side of the screen
			if (EventHandler::newEvent)
			{
				if (EventHandler::events[UP_PRESSED])
				{
					EventHandler::events[W_PRESSED] = true;
					EventHandler::events[UP_PRESSED] = false;
				}
				if (EventHandler::events[DOWN_PRESSED])
				{
					EventHandler::events[S_PRESSED] = true;
					EventHandler::events[DOWN_PRESSED] = false;
				}
				if (!server)
				{
					char clientSend[4] = { '1', '2', '3', '4' };								//initialize transmit data (all keys released, no user command, last byte don't care)
					if (EventHandler::events[W_PRESSED])									//Send the keystates, with a filter for simultaneous key presses (W overrides S)
					{
						clientSend[0] = -1;
					}
					else if (EventHandler::events[S_PRESSED] == true)
					{
						clientSend[1] = -1;
					}
					//NetworkManager::GetInstance()->SendData("Hello from client world");
					NetworkManager::GetInstance()->SendData(clientSend);
				}
			}
		}

		Paddles[0].Update();


		if (networked)
		{
			char *DataFromOpponent = NetworkManager::GetInstance()->ReceiveData();							//Receive keypresses from the client
			if (&DataFromOpponent != 0)
			{
				//std::cout << DataFromOpponent;
				if (server)																					//if we're the server, get the second player keypresses from the network
				{
					EventHandler::events[UP_PRESSED] = (bool)*DataFromOpponent;

					if (EventHandler::events[UP_PRESSED])
					{
						EventHandler::events[DOWN_PRESSED] = false;
					}
					else
					{
						EventHandler::events[DOWN_PRESSED] = (bool)*(DataFromOpponent + 1);
					}
					Paddles[1].Update();
				}
			}
		}
		else
		{
			Paddles[1].Update();
		}

		Ball.Update();
		Ball.CheckWallCollision();
		Ball.CheckGOCollision(Paddles[0]);
		Ball.CheckGOCollision(Paddles[1]);

		if (networked)
		{
/*			if (server)
			{
				NetworkManager::GetInstance()->SendData("Hello from Server world");


			}
			else
			{
				NetworkManager::GetInstance()->SendData("Hello from client world");
			}
*/		}
		// update the drawn paddles
		Paddles[0].Draw(window, renderer);
		Paddles[1].Draw(window, renderer);

		Ball.Draw(window, renderer);

		// Update the screen!
		SDL_RenderPresent(renderer);

		// pause to control framerate
		SDL_Delay(15);
	}

	SDL_Quit();

	if (networked)
	{
		NetworkManager::GetInstance()->Shutdown();									//tidy up network resources before quiting
	}

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