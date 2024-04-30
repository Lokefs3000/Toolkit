#pragma once

#include <vector>
#include <memory>
#include <queue>

class SceneTree
{
private:
	std::vector<size_t> m_Entities;
public:

};

class SceneEngine
{
private:
	std::vector<std::unique_ptr<SceneTree>> m_SceneTrees;
	size_t m_IdCounter; //overflow and collision is technicly possible but unlikely
public:
	SceneEngine();
	~SceneEngine();
};