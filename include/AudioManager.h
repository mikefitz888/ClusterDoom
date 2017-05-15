#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

/*
    AUDIO MANAGER
    -------------

    The audio manager controls the playing of sounds by maintaining a list of live sound instances.
    - A sound instance is a currently playing sound effect.

    The audio manager ensures that no memory is leaked by having other components of the game create
    sound instances. All instances for sound should be instantiated by calling the playSound method.


*/

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
