#pragma once

#include <string_view>

struct SDL_Window;

class Window
{
private:
	SDL_Window* m_Window;

	friend class GraphicsEngine;
public:
	Window(std::string_view title, uint32_t width, uint32_t height);
	~Window();
};