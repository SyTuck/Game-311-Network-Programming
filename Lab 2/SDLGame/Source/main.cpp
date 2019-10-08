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
	char header[2] = { 'a', 'A' };			//header and footer two bytes long to make sure there's no padding issues
	unsigned short ballXPos;
	unsigned short ballYPos;
	unsigned short paddle0Pos;
	unsigned short paddle1Pos;
	unsigned char  score;
	unsigned char  gameState;
	char footer[2] = { 'z', 'Z' };
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
			if (EventHandler::newEvent)														//"update" flag added to the event handle to only send data when something has changed
			{																				//(removing this if and the "listenSocket" to "peersocket" change in AcceptConnections )
				if (EventHandler::events[UP_PRESSED])										//(might fix the receiving continously bad data issue?                                 )
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
					char clientSend[5] = { 'F', 'F', ' ', ' ', '\n' };						//initialize transmit data (all keys released, no user command, last bytes don't care for now)
					if (EventHandler::events[W_PRESSED])									//Send the keystates, with a filter for simultaneous key presses (W overrides S)
					{
						clientSend[0] = 'T';
					}
					else if (EventHandler::events[S_PRESSED] == true)
					{
						clientSend[1] = 'T';
					}
					NetworkManager::GetInstance()->SendData(clientSend);
				}
			}
		}

		Paddles[0].Update();																//local/left paddle is always updated from key controls regardless of game mode	

		if (networked)
		{
			char *DataFromOpponent = NetworkManager::GetInstance()->ReceiveData();			//Receive keypresses from the client
			if (&DataFromOpponent != 0)
			{
				if (server)																	//if we're the server, get the second player keypresses from the network
				{
					EventHandler::events[UP_PRESSED] = false;
					EventHandler::events[DOWN_PRESSED] = false;

					if (*DataFromOpponent == 'T')
					{
						EventHandler::events[UP_PRESSED] = true;
					}
					else if (*(DataFromOpponent + 1) == 'T')
					{
						EventHandler::events[DOWN_PRESSED] = true;
					}
					Paddles[1].Update();																	
				}
				else 
				{	
					ServerDataStruct *tmpSrvDat;											//if we're the client, receive the paddle and ball position data
					tmpSrvDat = reinterpret_cast<ServerDataStruct *>(DataFromOpponent);		//from the server 
					if ((tmpSrvDat->header[0] == 'a') && (tmpSrvDat->footer[0] == 'z'))		//only process our data (no longer able to troubleshoot to see if this helps)
					{																		//(I seem to receive continous bad data even when the server is not sending)
						unsigned short tmpDat = ntohs(*(DataFromOpponent+2));				//This should be reworked to use tmpSrvDat but I can't troubleshoot any changes
						Ball.SetXPosition(float(WINDOW_WIDTH - tmpDat));					//so I'm leaving it at the last known state
						tmpDat = ntohs(*(DataFromOpponent + 4));							//(note that the direction of the ball is flipped for the reversed prespective of the client)
						Ball.SetYPosition(float(tmpDat)); 
						tmpDat = ntohs(*(DataFromOpponent + 6));
						Paddles[1].SetYPosition(float(tmpDat)); 							//also flip paddle locations
						tmpDat = ntohs(*(DataFromOpponent + 8));							//(opponent is on the right side, local player is on the left side)
						Paddles[0].SetYPosition(float(tmpDat)); 											
					}
				}
			}
		}
		else
		{																					//non network games have a regular paddle update
			Paddles[1].Update();
		}

		if ((!networked) || (server))														//only check for collisions if we're the server or standalone
		{																				
			Ball.Update();
			Ball.CheckWallCollision();
			Ball.CheckGOCollision(Paddles[0]);
			Ball.CheckGOCollision(Paddles[1]);
		}

		if (networked)																		//once all is processed, update the client with the new game data
		{
			if (server)
			{
				unsigned short tmpDat = unsigned short(Ball.GetTransform().position.x);		
				serverData.ballXPos = htons(tmpDat);
				tmpDat = unsigned short(Ball.GetTransform().position.y);
				serverData.ballYPos = htons(tmpDat);
				serverData.paddle0Pos = htons(unsigned short(Paddles[0].GetTransform().position.y));
				serverData.paddle1Pos = htons(unsigned short(Paddles[1].GetTransform().position.y));
				serverData.score = 0;
				serverData.gameState = 0;
				NetworkManager::GetInstance()->SendData(reinterpret_cast<char *>(&serverData));
			}
		}
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
	SDL_Delay(15);
}