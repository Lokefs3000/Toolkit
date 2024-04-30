#pragma once

#include <string>

class Application;

class ApplicationLayer
{
private:
	Application* m_Owner;
protected:
	//interface functions
	void LoadScriptAssembly(std::string_view path);
	void* GetScriptFunction(std::string_view dll, std::string_view type, std::string_view method);
public:
	ApplicationLayer(Application* app);
	virtual ~ApplicationLayer() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
};