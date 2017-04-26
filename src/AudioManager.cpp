#include "../include/AudioManager.h"
#include "../include/manager.h"
#include "../include/ResourceManager.h"

// Constructor
AudioManager::AudioManager(manager::Manager* manager) {
    this->manager = manager;

    loadSoundAssets();
}

/////////////////////////////////////////////////////
// LOADING OF ASSETS HERE
void AudioManager::loadSoundAssets() {
    // Get resource manager
    ResourceManager *resource_manager = manager->getResourceManager();

    // Load Sound assets
    resource_manager->soundBufferLoad("bomb", "src/Resources/SoundEffects/bomb_explode.ogg");
    resource_manager->soundBufferLoad("pirate_death_1", "src/Resources/SoundEffects/snd_pirate_death.ogg");
    resource_manager->soundBufferLoad("pirate_death_2", "src/Resources/SoundEffects/snd_pirate_death_2.wav");
    resource_manager->soundBufferLoad("wizard_death", "src/Resources/SoundEffects/snd_wizard_death.wav");
    resource_manager->soundBufferLoad("laser", "src/Resources/SoundEffects/snd_laser.ogg");
    resource_manager->soundBufferLoad("robot_death", "src/Resources/SoundEffects/snd_robot_death.ogg");

        
    // Load Music assets
    sf::Music* music = resource_manager->musicLoad("main_music", "src/Resources/Music/music_3.ogg");
    music->play();
    music->setLoop(true);
}
/////////////////////////////////////////////////////

/*
    We need to ensure all sound instances that have finished playing
    get discarded correctly.

*/
void AudioManager::stepSounds() {

    for (auto sound_instance : sound_instances) {
        if (sound_instance->getStatus() == sf::Sound::Stopped) {
            // Mark sound instance for delete and remove it from main list
            delete_list.push_back(sound_instance);
        }
    }
    for (auto sound_instance : delete_list) {

        auto it = std::find(sound_instances.begin(), sound_instances.end(), sound_instance);
        if (it != sound_instances.end()) {
            sound_instances.erase(it);
        }
        delete sound_instance;
    }
    delete_list.clear();
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
sf::Sound* AudioManager::playSound(sf::SoundBuffer* buffer) {
    sf::Sound *new_sound = new sf::Sound();
    new_sound->setBuffer(*buffer);
    new_sound->play();

    sound_instances.push_back(new_sound);
    return new_sound;
}
sf::Sound* AudioManager::playSound(sf::String sound_resource_name) {

    if (manager->getResourceManager()->soundBufferExists(sound_resource_name)) {
        sf::SoundBuffer *sound_buffer;
        sound_buffer = manager->getResourceManager()->getSoundBuffer(sound_resource_name);
        if (sound_buffer != nullptr) {
            return this->playSound(sound_buffer);
        }
    }
    return nullptr;
}

// Stop all music that is currently playing
void AudioManager::stopAllMusic() {
    map<sf::String, sf::Music*> *music_map = this->manager->getResourceManager()->getMusicMap();
    for (auto it = music_map->begin(); it != music_map->end(); it++) {
        sf::Music *music = this->manager->getResourceManager()->getMusic(it->first);
        music->stop();
    }
}

// Stop all sound that is currently playing
void AudioManager::stopAllSound() {
    for (auto sound_instance : sound_instances) {
        sound_instance->stop();
    }
}
