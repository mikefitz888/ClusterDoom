#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include "gamecore.h"

class AudioManager {

    friend class manager::Manager;

private:
    manager::Manager* manager;
    std::vector<sf::Sound*> sound_instances;
    std::vector<sf::Sound*> delete_list;

    void loadSoundAssets(); // Function to give us an opportunity to load in sound assets
    void stepSounds();     // Keep sounds up-to-date, and delete sounds that have finished playing.
    void release();        // release memory

public:
    AudioManager(manager::Manager* manager);
    sf::Sound* playSound(sf::SoundBuffer* buffer);
    sf::Sound* playSound(sf::String sound_resource_name);
    void stopAllMusic();
    void stopAllSound();
};

#endif
