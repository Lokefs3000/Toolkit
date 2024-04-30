#pragma once

#include <vector>
#include <string_view>
#include <memory>
#include <unordered_map>

//absolute information glory!
//https://github.com/SanderMertens/ecs-faq?tab=readme-ov-file#what-is-a-cache-line

class IComponent
{

};

class CTransform : public IComponent
{
public:

};

struct CompVector
{
private:
	
	size_t m_Size;
	size_t m_Capacity;
public:
};

struct TableVector
{
private:
	IComponent**& m_Array;
	size_t m_Size;
	size_t m_Capacity;
public:
	TableVector(IComponent**& address);
	~TableVector();

	void push_back(IComponent* component);

	size_t size();
	size_t capacity();
	IComponent* at(size_t i);
};

struct EntityConstructer
{
	size_t Name;
	void* Constructer;
};

class EntityFactory
{
private:
	std::vector<EntityConstructer> m_Constructers;
	size_t m_Index;
public:
	EntityFactory(size_t max);
	~EntityFactory();

	void Register(size_t name, void* constructer);
};

class EntityEngine
{
private:
	std::vector<IComponent**> m_CompTable;
	std::vector<TableVector> m_Table;

	std::unique_ptr<EntityFactory> m_Factory;
public:
	EntityEngine();
	~EntityEngine();
};