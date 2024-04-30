#pragma once

#include <vector>
#include <memory>

class ApplicationLayer;

class ScriptEngine;
class Window;
class EventHandler;
class GraphicsEngine;
class Renderer;
class SceneEngine;

class Application //aka Runtime
{
private:
	std::vector<std::unique_ptr<ApplicationLayer>> m_Layers;

	std::unique_ptr<ScriptEngine> m_ScriptEngine;
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<EventHandler> m_EventHandler;
	std::unique_ptr<GraphicsEngine> m_GraphicsEngine;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<SceneEngine> m_SceneEngine;

	friend class ApplicationLayer;
public:
	Application();
	~Application();

	void Run();

	template<class TLayer>
	void AddLayer();
};

template<class TLayer>
void Application::AddLayer()
{
	m_Layers.push_back(std::make_unique<TLayer>(this));
}