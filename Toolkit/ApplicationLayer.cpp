#include "pch.h"
#include "ApplicationLayer.h"

#include "Application.h"
#include "ScriptEngine.h"

ApplicationLayer::ApplicationLayer(Application* app)
{
	m_Owner = app;
}

void ApplicationLayer::LoadScriptAssembly(std::string_view path)
{
	m_Owner->m_ScriptEngine->LoadAssembly("game", path);
}

void* ApplicationLayer::GetScriptFunction(std::string_view dll, std::string_view type, std::string_view method)
{
	return m_Owner->m_ScriptEngine->GetFunction(dll, type, method);
}
