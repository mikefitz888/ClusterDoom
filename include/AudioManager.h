#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include "gamecore.h"

//#include <map>
//#include <SFML/Main.hpp>
//#include <SFML/System.hpp>
//#include <SFML/Audio.hpp>
//#include <iostream>

//namespace manager {
//    class Manager;
//}


class AudioManager {
    using manager::Manager;

    friend class Manager;

private:
    Manager* manager;
    std::vector<sf::Sound*> sound_instances;
    std::vector<sf::Sound*> delete_list;

    void loadSoundAssets(); // Function to give us an opportunity to load in sound assets
    void stepSounds();     // Keep sounds up-to-date, and delete sounds that have finished playing.
    void release();        // release memory

public:
    AudioManager(Manager* manager);
    sf::Sound* playSound(sf::SoundBuffer* buffer);
    sf::Sound* playSound(sf::String sound_resource_name);
    void stopAllMusic();
    void stopAllSound();
};

#endif
