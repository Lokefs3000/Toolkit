#include "pch.h"
#include "EventHandler.h"

#include <SDL3/SDL.h>

void EventHandler::PollEvents()
{
	SDL_Event Event{};
	while (SDL_PollEvent(&Event))
	{

	}
}
