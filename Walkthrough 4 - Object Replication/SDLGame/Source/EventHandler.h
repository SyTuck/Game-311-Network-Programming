#pragma once
#include "SDL.h"

enum PongEvents
{
	W_PRESSED,
	S_PRESSED,
	UP_PRESSED,
	DOWN_PRESSED,

	NUM_PONG_EVENTS

};

//struct Event
//{
//	PongEvents key;
//	bool state;
//};

class EventHandler
{
public:
	EventHandler();
	~EventHandler();

	static bool Update();

	static const int NUM_EVENTS = 10;
 
	static bool events[PongEvents::NUM_PONG_EVENTS];
	static bool newEvent;									

private:

	static void SetButton(PongEvents eventNum, bool pressed);
};

