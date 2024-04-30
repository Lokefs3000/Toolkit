#include "pch.h"
#include "Renderer.h"

Renderer::Renderer()
{
	GraphicsEngine* engine = GraphicsEngine::GetInstance();

	m_PrimaryCommandBuffer = std::unique_ptr<CommandBuffer>(engine->AllocateNewCommandBuffer());
}

void Renderer::Render()
{
	RenderConfig preservedConfig = RenderConfig(Config);
}
