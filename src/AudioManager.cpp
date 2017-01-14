#include "../include/AudioManager.h"
#include "../include/manager.h"


// Constructor
AudioManager::AudioManager(Manager* manager) {
	this->manager = manager;

	loadSoundAssets();
}

/////////////////////////////////////////////////////
// LOADING OF ASSETS HERE
void AudioManager::loadSoundAssets() {
	// Get resource manager
	ResourceManager *resource_manager = manager->getResourceManager();

	// Load Sound assets
	// ...

	// Load Music assets
	// ...
	sf::Music* music = resource_manager->musicLoad("main_music", "src/Resources/Music/sndMusic.ogg");
	music->play();
}
/////////////////////////////////////////////////////

/*
	We need to ensure all sound instances that have finished playing
	get discarded correctly.

*/
void AudioManager::stepSounds() {
	for (auto sound_instance : sound_instances) {
		if (sound_instance->getStatus() == sf::Sound::Stopped) {
			// .. Delete sound
		}
	}
}

/*
	Unload any sound instances that are still playing
*/
void AudioManager::release() {
	for (auto sound_instance : sound_instances) {
		delete sound_instance;
	}
	sound_instances.clear();
}

// Sound playing functions
/*
	These functions create new sound instances to play the sound
	stored in the associated sound buffers.
*/
sf::Sound* AudioManager::playSoundFromBuffer(sf::SoundBuffer* buffer) {
	sf::Sound *new_sound = new sf::Sound();
	new_sound->setBuffer(*buffer);
	new_sound->play();

	sound_instances.push_back(new_sound);
}
sf::Sound* AudioManager::playSoundFromBuffer(sf::String sound_resource_name) {
	
	if (manager->getResourceManager()->soundBufferExists(sound_resource_name)) {
		sf::SoundBuffer *sound_buffer;
		sound_buffer = manager->getResourceManager()->getSoundBuffer(sound_resource_name);
		if (sound_buffer != nullptr) {
			return this->playSoundFromBuffer(sound_buffer);
		}
	}
	return nullptr;
}