#include "EventHandler.h"
#include <stdio.h>
#include <string.h>

bool EventHandler::events[];

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

bool EventHandler::Update()
{
	bool success = true;

	SDL_Event currEvents;
	// Check for events
	while(	SDL_PollEvent(&currEvents))
	{
		switch (currEvents.key.keysym.sym)
		{
		case SDLK_w:
		{
			SetButton(PongEvents::W_PRESSED, currEvents.key.type == SDL_KEYDOWN);
			break;
		}
		case SDLK_s:
		{
			SetButton(PongEvents::S_PRESSED, currEvents.key.type == SDL_KEYDOWN);
			break;
		}
		case SDLK_UP:
		{
			SetButton(PongEvents::UP_PRESSED, currEvents.key.type == SDL_KEYDOWN);
			break;
		}
		case SDLK_DOWN:
		{
			SetButton(PongEvents::DOWN_PRESSED, currEvents.key.type == SDL_KEYDOWN);
			break;
		}
		}
	}
	return success;
}

void EventHandler::SetButton(PongEvents eventNum, bool pressed)
{
	events[eventNum] = pressed;
}
