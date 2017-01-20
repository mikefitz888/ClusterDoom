#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

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
#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using graphics::RenderManager;

// Forward manager declaration
namespace manager { 
	class Manager;
}
namespace graphics {
	class AnimatedTexture;
}

using std::map;
using graphics::VertexBuffer;
using graphics::AnimatedTexture;
using manager::Manager;

class ResourceManager {
	friend class AudioManager;

private:
	map<sf::String, sf::Texture*>     textureMap;
	map<sf::String, sf::Shader*>	  shaderMap;
	map<sf::String, VertexBuffer*>	  meshMap;
	map<sf::String, AnimatedTexture*> animatedTextureMap;
	map<sf::String, sf::SoundBuffer*> soundBufferMap;
	map<sf::String, sf::Music*>	      musicMap;
	map<sf::String, sf::Font*>		  fontMap;

	Manager *manager; // ptr to manager

	map<sf::String, sf::Music*> *getMusicMap();

	// SoundBuffer functions
	/*
		Note: Sound data is stored in soundbuffer, but we need to use sf::Sound to play an instance of a sound.
		- Sounds are for sound effects that end are short and multiple instances of them may play.

		- The SoundBuffer functions are private, and only accessible by friend class AudioManager, as normal
		instances should not be dealing with sound buffers directly. Sound effects are abstracted away
		through the audio manager.

		- Sound instances however are fine (these are returned by AudioManager's play function and allow
		manipulation of volume, pitch etc;)
	*/
	sf::SoundBuffer* soundBufferLoad(sf::String resource_name, sf::String resource_filepath);
	sf::SoundBuffer* getSoundBuffer(sf::String resource_name);
	bool			 soundBufferExists(sf::String resource_name);
	void			 soundBufferUnload(sf::String resource_name);

public:
	// Constructor
	ResourceManager(Manager* manager);

	// Texture functions
	sf::Texture* textureLoad(sf::String resource_name, sf::String resource_filepath);
	sf::Texture* getTexture(sf::String resource_name);
	bool         textureExists(sf::String resource_name);
	void         textureUnload(sf::String resource_name);

	// Shader functions
	sf::Shader* shaderLoad(sf::String resource_name, sf::String resource_filepath_vert, sf::String resource_filepath_frag);
	sf::Shader* getShader(sf::String resource_name);
	bool		shaderExists(sf::String resource_name);
	void        shaderUnload(sf::String resource_name);

	// Mesh functions
	VertexBuffer* meshLoad(sf::String resource_name, sf::String resource_filepath);
	VertexBuffer* getMesh(sf::String resource_name);
	bool	      meshExists(sf::String resource_name);
	void		  meshUnload(sf::String resource_name);

	// AnimatedTexture functions
	AnimatedTexture* animatedTextureLoad(sf::String resource_name, sf::String resource_filepath, bool split_both_ways, int frame_count_h, int frame_count_v, int total_frames);
	AnimatedTexture* getAnimatedTexture(sf::String resource_name);
	bool		     animatedTextureExists(sf::String resource_name);
	void		     animatedTextureUnload(sf::String resource_name);

	// Music functions
	/*
		Music is streamed, and each music track will be associated with one instance stored in this map.
		Fetching that instance will allow you to start and stop that music from playing. (By using
		its resource name as a reference).

		- The audio manager will also be able to do this.
	*/
	sf::Music* musicLoad(sf::String resource_name, sf::String resource_filepath);
	sf::Music* getMusic(sf::String resource_name);
	bool       musicExists(sf::String resource_name);
	void       musicUnload(sf::String resource_name);

	// Font functions
	sf::Font* fontLoad(sf::String resource_name, sf::String resource_filepath);
	sf::Font* getFont(sf::String resource_name);
	bool      fontExists(sf::String resource_name);
	void      fontUnload(sf::String resource_name);

	// Text functions
	sf::Text* createTextFromFont(sf::String resource_name, sf::String default_text);

	// < UNLOAD ALL with release() ~ To be called at end >
	void release();
};

#endif