#include "../include/manager.h"

// Constructor
ResourceManager::ResourceManager(Manager* manager) {
	this->manager = manager;
}

////////////////////////////////////////////////
// Texture functions
sf::Texture* ResourceManager::textureLoad(sf::String resource_name, sf::String resource_filepath) {
	// Check if map already contains something under the desired resource_name key:
	sf::Texture* new_tex;
	if (this->textureExists(resource_name)) {
		return this->getTexture(resource_name);
	}
	else {
		new_tex = new sf::Texture();
		if (!new_tex->loadFromFile(resource_filepath)) {
			// Texture has failed to load
			std::cout << "[ERROR] TEXTURE' " << resource_filepath.toAnsiString() << "' FAILED TO LOAD!" << std::endl;
			delete new_tex;
			return nullptr;
		}

		// Texture has loaded successfully, store in map
		std::cout << "TEXTURE '" << resource_filepath.toAnsiString() << "' LOADED into resource slot '" << resource_name.toAnsiString() << "'!" << std::endl;
		this->textureMap.insert(std::pair<sf::String, sf::Texture*>(resource_name, new_tex));
		return new_tex;
	}
	return nullptr;
}

bool ResourceManager::textureExists(sf::String resource_name) {
	return (this->textureMap.find(resource_name) != this->textureMap.end());
}

sf::Texture* ResourceManager::getTexture(sf::String resource_name) {
	if (this->textureExists(resource_name)) {
		sf::Texture* texture = this->textureMap[resource_name];
		return texture;
	} else {
		std::cout << "[ERROR] Texture '" << resource_name.toAnsiString() << "' Does not exist!" << std::endl;
		return nullptr;
	}
}

void ResourceManager::textureUnload(sf::String resource_name) {
	if (this->textureExists(resource_name)) {
		sf::Texture* tx = this->getTexture(resource_name);
		delete tx;
		this->textureMap.erase(resource_name);
	}
}

////////////////////////////////////////////////
// Shader functions
sf::Shader* ResourceManager::shaderLoad(sf::String resource_name, sf::String resource_filepath_vert, sf::String resource_filepath_frag) {
	// Check if map already contains something under the desired resource_name key:
	sf::Shader* new_shader;
	if (this->shaderExists(resource_name)) {
		return this->getShader(resource_name);;
	}
	else {
		new_shader = manager->getRenderManager()->createShaderFromFile(resource_filepath_vert, resource_filepath_frag);
		if (new_shader == nullptr) {
			// Shader has failed to load
			std::cout << "[ERROR] SHADER '" << resource_filepath_vert.toAnsiString() << "' & '" << 
											  resource_filepath_frag.toAnsiString()	<< "' FAILED TO LOAD!" << 
											  std::endl;
			delete new_shader;
			return nullptr;
		}

		// Texture has loaded successfully, store in map
		std::cout << "SHADER '" << resource_filepath_vert.toAnsiString() << "' & '" <<
			resource_filepath_frag.toAnsiString() << "' LOADED into resource slot '" << resource_name.toAnsiString() << "'!" <<
			std::endl;
		this->shaderMap.insert(std::pair<sf::String, sf::Shader*>(resource_name, new_shader));
		return new_shader;
	}
	return nullptr;
}

bool ResourceManager::shaderExists(sf::String resource_name) {
	return (this->shaderMap.find(resource_name) != this->shaderMap.end());
}

sf::Shader* ResourceManager::getShader(sf::String resource_name) {
	if (this->shaderExists(resource_name)) {
		sf::Shader* texture = this->shaderMap[resource_name];
		return texture;
	} else {
		std::cout << "[ERROR] Shader '" << resource_name.toAnsiString() << "' Does not exist!" << std::endl;
		return nullptr;
	}
}

void ResourceManager::shaderUnload(sf::String resource_name) {
	if (this->shaderExists(resource_name)) {
		sf::Shader* sh = this->getShader(resource_name);
		delete sh;
		this->shaderMap.erase(resource_name);
	}
}


////////////////////////////////////////////////
// Release all resources
void ResourceManager::release() {

	// Texture map
	for (auto it = textureMap.begin(); it != textureMap.end(); it++) {
		this->textureUnload(it->first);
	}

	// Shader map
	for (auto it = shaderMap.begin(); it != shaderMap.end(); it++) {
		this->shaderUnload(it->first);
	}

	// TODO: Unload mesh map
}