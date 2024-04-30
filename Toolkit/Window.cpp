#include "pch.h"
#include "Window.h"

#include <SDL3/SDL.h>

Window::Window(std::string_view title, uint32_t width, uint32_t height)
{
	m_Window = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_VULKAN);
}

Window::~Window()
{
	SDL_DestroyWindow(m_Window);
}
