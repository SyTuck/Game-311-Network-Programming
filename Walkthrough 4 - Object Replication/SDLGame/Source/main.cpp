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
bool minData = true;

void DisplaySplashScreen(SDL_Window* mainWindow, SDL_Renderer* mainRenderer);

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;

	int b = sizeof(ball);									//***  Part 1: 
	int p = sizeof(Paddle);
	
	cout << "sizeof(ball): " << b << endl;
	cout << "sizeof(Paddle): " << p << endl;
	cout << "sizeof(Game Content): " << p * 2 + b << endl;
	cout << endl;

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
		std::cout << "Are you running in Minimum Data mode? (Y/n)? ";
		std::cin >> sChoice;
		std::cout << endl;
		if ((sChoice == 'n') || (sChoice == 'N'))
		{
			 minData = false;
		}

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
	ball *Ball = new ball();
	Ball->Init("Data\\Art\\ball.bmp", renderer);

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
			if (&DataFromOpponent != '\0')
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

						unsigned short tmpDat = WINDOW_WIDTH - tmpSrvDat->ballXPos - Ball->GetDimensions().x;		
						Ball->SetXPosition(float(tmpDat));									//get the ball X and flip it's direction to be relevant to the client's view
						Ball->SetYPosition(float(tmpSrvDat->ballYPos)); 
						Paddles[1].SetYPosition(float(tmpSrvDat->paddle0Pos));				//also flip paddle locations
						Paddles[0].SetYPosition(float(tmpSrvDat->paddle1Pos)); 				//(opponent is on the right side, local player is on the left side)

						if (!minData)														//if not using Minimum Data Mode, then get the ball data from 
						{																	//the ball object that has been sent to us
							char recieveBall[sizeof(ball)];
							memcpy(recieveBall, DataFromOpponent + sizeof(serverData) + 1, sizeof(recieveBall));

							ball *ballData = reinterpret_cast<ball *>(&recieveBall);
							Ball->SetXPosition(WINDOW_WIDTH - ballData->GetTransform().position.x - Ball->GetDimensions().x);
							Ball->SetYPosition(ballData->GetTransform().position.y);
						}
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
			Ball->Update();
			Ball->CheckWallCollision();
			Ball->CheckGOCollision(Paddles[0]);
			Ball->CheckGOCollision(Paddles[1]);
		}

		if (networked)																		//once all is processed, update the client with the new game data
		{
			if (server)
			{
				serverData.ballXPos = unsigned short(Ball->GetTransform().position.x);
				serverData.ballYPos = unsigned short(Ball->GetTransform().position.y);
				serverData.paddle0Pos = unsigned short(Paddles[0].GetTransform().position.y);
				serverData.paddle1Pos = unsigned short(Paddles[1].GetTransform().position.y);
				serverData.score = 0;
				serverData.gameState = 0;

				char dataToSend[sizeof(serverData)];

				if (minData)																//if only sending the minimum required amount of data
				{																			//only serialize the ServerData set above
					memcpy(dataToSend, &serverData, sizeof(serverData));					
				}
				else																		//otherwise send the ServerData (because it has the paddle positions)
				{																			//and all of the ball object as well
					char dataToSend[sizeof(serverData) + sizeof(ball)];
					memcpy(dataToSend, &serverData, sizeof(serverData));

					char* ballbuffer = new char[sizeof(ball)];
					ballbuffer = reinterpret_cast<char *>(Ball);

					memcpy(dataToSend, &serverData, sizeof(serverData));
					memcpy(dataToSend + sizeof(serverData) + 1, ballbuffer, sizeof(ball));
				}
				NetworkManager::GetInstance()->SendData(dataToSend);

//				delete ballbuffer;
			}
		}
		// update the drawn paddles
		Paddles[0].Draw(window, renderer);
		Paddles[1].Draw(window, renderer);

		Ball->Draw(window, renderer);

		// Update the screen!
		SDL_RenderPresent(renderer);

		// pause to control framerate
		SDL_Delay(30);
	}

	SDL_Quit();

	if (networked)
	{
		NetworkManager::GetInstance()->Shutdown();									//tidy up network resources before quiting
	}

	delete Ball;

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