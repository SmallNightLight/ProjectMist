#pragma once

#include "Settings.h"
#include "System.h"

#include <memory>
#include <iostream>
#include <unordered_map>
#include <cassert>

class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(_systems.find(typeName) == _systems.end() && "System already registered");

		auto system = std::make_shared<T>();
		_systems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(_systems.find(typeName) != _systems.end() && "System not yet registered");

		_signatures.insert({ typeName, signature });
	}

	void DestroyEntity(Entity entity)
	{
		for (auto const& pair : _systems)
		{
			auto const& system = pair.second;

			system->Entities.erase(entity);
		}
	}

	void EntitySignatureChanged(Entity entity, Signature newSignature)
	{
		for (auto const& pair : _systems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = _signatures[type];

			//Entity signature matches system signature - insert into set
			if ((newSignature & systemSignature) == systemSignature)
			{
				system->Entities.insert(entity);
			}
			//Entity signature does not match system signature - erase from set
			else
			{
				system->Entities.erase(entity);
			}
		}
	}

private:
	std::unordered_map<const char*, Signature> _signatures;
	std::unordered_map<const char*, std::shared_ptr<System>> _systems{};
};