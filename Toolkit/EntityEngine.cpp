#include "pch.h"
#include "EntityEngine.h"

#define DefineConstructerPointer(object) \
	IComponent* Create_##object() { return new object(); }
#define GetConstructerPointer(object) reinterpret_cast<void*>(Create_##object)

DefineConstructerPointer(CTransform);

TableVector::TableVector(IComponent**& address)
	: m_Array(address), m_Capacity(1), m_Size(0)
{
}

TableVector::~TableVector()
{
	m_Capacity = 0;
	m_Size = 0;

	delete[] m_Array;
	m_Array = nullptr;
}

void TableVector::push_back(IComponent* component)
{
	if (m_Size + 1 <= m_Capacity)
	{
		m_Capacity *= 2;
		IComponent** newArray = new IComponent*[m_Capacity];
		for (size_t i = 0; i < m_Size; i++) //eww
			newArray[i] = m_Array[i];
		delete[] m_Array;
		m_Array = newArray;
	}

	m_Array[m_Size] = component;
	m_Size++;
}

size_t TableVector::size()
{
	return m_Size;
}

size_t TableVector::capacity()
{
	return m_Capacity;
}

IComponent* TableVector::at(size_t i)
{
	if (i >= m_Size)
		return nullptr;
	return m_Array[i];
}

EntityFactory::EntityFactory(size_t max)
{
	m_Constructers.resize(max);
	m_Index = 0;
}

EntityFactory::~EntityFactory()
{
}

void EntityFactory::Register(size_t name, void* constructer)
{
	m_Constructers[m_Index] = {
		name,
		constructer
	};

	m_Index++;
}

EntityEngine::EntityEngine()
{
	m_Factory = std::make_unique<EntityFactory>(1);
	m_Factory->Register(0, GetConstructerPointer(CTransform));
}

EntityEngine::~EntityEngine()
{
	for (size_t i = 0; i < m_Table.size(); i++)
	{
		TableVector& table = m_Table.at(i);
		for (size_t j = 0; j < table.size(); j++)
		{
			delete table.at(j);
		}
	}

	m_Table.clear();
}
