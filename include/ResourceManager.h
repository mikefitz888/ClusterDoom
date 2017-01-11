#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER

/*
	Resource Manager
	==========================
	 - Resource manager contains a centralised list of resources such as textures, sounds, shaders, models. (Any externally loaded resource).
	 This means that objects themselves should not be responsible for loading in resources.

	 - The system provides a means of both loading in resources to add the the list, and fetching ones that already exist. To do this, 
	 a specific string is associated with that particular resource. If you attempt to load a resource under an already existing entry, 
	 it will assume the resource already exists (and will return the existing one).

	 - Otherwise, resources can simply be queried from their known resource name. Existance checks will also be provided. If you attempt
	 to fetch a resource that doesn't exist, you will get an error and the program will terminate. (Therefore in these situations, you
	 should use the load function as this takes a filepath and loads if it is not already loaded.)

	 - There will be specific functions for each unique type of resource.

	 - I have provided the unload functions as well, however these don't really need to be used. They will be called when the release() function is called.
*/

#include "../include/VertexBuffer.h"
#include "../include/RenderManager.h"
#include <map>
#include <SFML\Main.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\Window.hpp>
#include <SFML\System.hpp>
#include <iostream>

using graphics::RenderManager;

// Forward manager declaration
namespace manager { 
	class Manager {
	public:
		RenderManager* getRenderManager() const;
	};
}

using std::map;
using graphics::VertexBuffer;
using manager::Manager;

class ResourceManager {

private:
	map<sf::String, sf::Texture*>  textureMap;
	map<sf::String, sf::Shader*>   shaderMap;
	//map<sf::String, VertexBuffer*> meshMap;

	Manager *manager; // ptr to manager

	
public:
	// Constructor
	ResourceManager(Manager* manager);

	// Texture functions
	sf::Texture* textureLoad(sf::String resource_name, sf::String resource_filepath);
	sf::Texture* getTexture(sf::String resource_name);
	bool         textureExists(sf::String resource_name);
	bool         textureUnload(sf::String resource_name);

	// Shader functions
	sf::Shader* shaderLoad(sf::String resource_name, sf::String resource_filepath_vert, sf::String resource_filepath_frag);
	sf::Shader* getShader(sf::String resource_name);
	bool		shaderExists(sf::String resource_name);
	bool        shaderUnload(sf::String resource_name);

	// Mesh functions
	VertexBuffer* meshLoad(sf::String resource_name, sf::String resource_filepath);
	VertexBuffer* getMesh(sf::String resource_name);
	bool	      meshExists(sf::String resource_name);
	bool		  meshUnload(sf::String resource_name);

	// < UNLOAD ALL with release() ~ To be called at end >
	void release();
};

#endif