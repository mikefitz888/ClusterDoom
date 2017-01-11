#include "../include/ResourceManager.h"

// Constructor
ResourceManager::ResourceManager(Manager* manager) {
	this->manager = manager;
}

////////////////////////////////////////////////
// Texture functions
sf::Texture* ResourceManager::textureLoad(sf::String resource_name, sf::String resource_filepath) {
	// Check if map already contains something under the desired resource_name key:
	sf::Texture* new_tex = this->getTexture(resource_name);
	if (new_tex != nullptr) {
		return new_tex;
	}
	else {
		new_tex = new sf::Texture();
		if (!new_tex->loadFromFile(resource_filepath)) {
			// Texture has failed to load
			std::cout << "[ERROR] TEXTURE " << resource_filepath.toAnsiString() << " FAILED TO LOAD!" << std::endl;
			delete new_tex;
			return nullptr;
		}

		// Texture has loaded successfully, store in map
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
	}
	else {
		return nullptr;
	}
}

////////////////////////////////////////////////
// Shader functions
sf::Shader* ResourceManager::shaderLoad(sf::String resource_name, sf::String resource_filepath_vert, sf::String resource_filepath_frag) {
	// Check if map already contains something under the desired resource_name key:
	sf::Shader* new_shader = this->getShader(resource_name);
	if (new_shader != nullptr) {
		return new_shader;
	}
	else {
		new_shader = manager->getRenderManager()->createShaderFromFile(resource_filepath_vert, resource_filepath_frag);
		if (new_shader == nullptr) {
			// Shader has failed to load
			std::cout << "[ERROR] SHADER " << resource_filepath_vert.toAnsiString() << " & " << 
											  resource_filepath_frag.toAnsiString()	<< " FAILED TO LOAD!" << 
											  std::endl;
			delete new_shader;
			return nullptr;
		}

		// Texture has loaded successfully, store in map
		this->shaderMap.insert(std::pair<sf::String, sf::Shader*>(resource_name, new_shader));
		return new_shader;
	}
	return nullptr;
}

bool ResourceManager::shaderExists(sf::String resource_name) {
	return (this->textureMap.find(resource_name) != this->textureMap.end());
}

sf::Shader* ResourceManager::getShader(sf::String resource_name) {
	if (this->shaderExists(resource_name)) {
		sf::Shader* texture = this->shaderMap[resource_name];
		return texture;
	}
	else {
		return nullptr;
	}
}