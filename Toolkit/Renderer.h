#pragma once

#include "StandardTypes.h"
#include "GraphicsEngine.h"

struct RenderConfig
{
	Color4 ClearColor;
};

class Renderer
{
private:
	std::unique_ptr<CommandBuffer> m_PrimaryCommandBuffer;
public:
	RenderConfig Config;

	Renderer();

	void Render();
};