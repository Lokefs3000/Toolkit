#include "pch.h"
#include "Application.h"

#include "ApplicationLayer.h"

#include "ScriptEngine.h"
#include "Window.h"
#include "EventHandler.h"
#include "GraphicsEngine.h"
#include "Renderer.h"
#include "SceneEngine.h"

Application::Application()
{
	m_ScriptEngine = std::make_unique<ScriptEngine>();
	m_Window = std::make_unique<Window>("Application", 1280, 720);
	m_EventHandler = std::make_unique<EventHandler>();
	m_GraphicsEngine = std::make_unique<GraphicsEngine>(m_Window.get());
	m_Renderer = std::make_unique<Renderer>();
	m_SceneEngine = std::make_unique<SceneEngine>();

	m_ScriptEngine->RegisterRenderer(m_Renderer.get());
}

Application::~Application()
{
	m_Layers.clear();
}

void Application::Run()
{
	for (std::unique_ptr<ApplicationLayer>& layer : m_Layers)
		layer->Init();

	while (true)
	{
		m_EventHandler->PollEvents();

		for (std::unique_ptr<ApplicationLayer>& layer : m_Layers)
			layer->Update();

		m_Renderer->Render();
	}
}
