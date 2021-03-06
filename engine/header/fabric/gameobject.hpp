#ifndef  GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <vector>
#include <string>
#include <fabric/types.hpp>
#include <fabric/mesh.hpp>
#include <typeinfo>
#include <exception>
#include <memory>
#include <iostream>
#include <bitset>
#include <Windows.h>
#include <fabric/dataCore.hpp>
#include <fabric/engineObj.hpp>

namespace fabric {


	class Attribute: public EngineObject{
	public:

		int isValid() {
			int out = true;
			if (name == "0$")
				out = false;
			if (content == 0)
				out = false;
			if (hash == 0)
				out = false;
			return out;
		}

		void* content = 0;
		size_t hash = 0;
		std::string name = "";
	};

	class  GameObject: public EngineObject {
	public:

		std::vector<HINSTANCE> dllHandles;
		int priority = 0;
		
		std::vector<void(*)()> updatePointers;
		std::vector<void(*)()> setupPointers;

		GameObject();
		~GameObject();

		int render();
		int setMesh(Mesh _mesh);
		void free();
		std::vector<Attribute> getAttributeVector();
		Attribute getAttribute(std::string name);
		template<typename T> void addAttribute(std::string name, T* content);
		template<typename T> void setAttribute(std::string name, T content);
		template<typename T> void removeAttribute(std::string name);
		template<typename T> Attribute findAttribute(std::string name);
		template<typename T> Attribute findAttribute(std::string name, bool* error, unsigned int* idx);

	private:
		std::string removeNameMangling(std::string name);
		std::vector<Attribute> attributes;
		Mesh mesh;
	};
}


template<typename T>
inline fabric::Attribute fabric::GameObject::findAttribute(std::string name)
{
	return GameObject::findAttribute<T>(name, 0, 0);
}

template<typename T> fabric::Attribute fabric::GameObject::findAttribute(std::string name, bool* error, unsigned int* idx)
{

	name = removeNameMangling(name);

	for (unsigned int i = 0; i < GameObject::attributes.size(); i++) {
		
		std::string atrName = removeNameMangling(GameObject::attributes.at(i).name);

		if (atrName == name) {

			if (GameObject::attributes.at(i).hash != typeid(T).hash_code()) {
				throw std::runtime_error("Type dont match!");
			}
			if (idx != 0) 
				*idx = i;
			
			if (error != 0) 
				*error = false;

			return GameObject::attributes.at(i);
		}
	}
	if (error != 0) 
		*error = true;

	return GameObject::findAttribute<int>("");
}

template<typename T>
inline void fabric::GameObject::setAttribute(std::string name, T content)
{
	Attribute attr = findAttribute<T>(name);
	*reinterpret_cast<T*>(attr.content) = content;
}

template<typename T>
inline void fabric::GameObject::removeAttribute(std::string name)
{
	unsigned int i = 0;
	Attribute attr = findAttribute<T>(name, 0, &i);
	delete reinterpret_cast<T*>(attr.content);
	GameObject::attributes.erase(GameObject::attributes.begin() + i);
	
}

template<typename T>
inline void fabric::GameObject::addAttribute(std::string name, T* content)
{

	for (unsigned int i = 0; i < GameObject::attributes.size(); i++) {
		if (GameObject::attributes.at(i).name == name) {
			throw std::runtime_error("Such element allrady exists");
		}
	}

	Attribute attrib;
	attrib.name = name;
	attrib.content = content;
	attrib.hash = typeid(T).hash_code();

	GameObject::attributes.push_back(attrib);

}

#endif // ! GAMEOBJECT_HPP