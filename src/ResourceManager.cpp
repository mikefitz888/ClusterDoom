#include "../include/ResourceManager.h"
#include "../include/manager.h"
#include "../include/AnimatedTexture.h"
#include "../include/RenderManager.h"

// Constructor
ResourceManager::ResourceManager(manager::Manager* manager) {
    this->manager = manager;
}

////////////////////////////////////////////////
// Texture functions
graphics::Texture* ResourceManager::textureLoad(sf::String resource_name, sf::String resource_filepath) {
    // Check if map already contains something under the desired resource_name key:
    graphics::Texture* new_tex;
    if (this->textureExists(resource_name)) {
        return this->getTexture(resource_name);
    }
    else {
        new_tex = new graphics::Texture(this->manager->getRenderManager());
        if (!new_tex->loadFromFile(resource_filepath)) {
            // Texture has failed to load
            std::cout << "[ERROR] TEXTURE' " << resource_filepath.toAnsiString() << "' FAILED TO LOAD!" << std::endl;
            delete new_tex;
            return nullptr;
        }

        // Texture has loaded successfully, store in map
        std::cout << "TEXTURE '" << resource_filepath.toAnsiString() << "' LOADED into resource slot '" << resource_name.toAnsiString() << "'!" << std::endl;
        this->textureMap.insert(std::pair<sf::String, graphics::Texture*>(resource_name, new_tex));
        return new_tex;
    }
    return nullptr;
}

bool ResourceManager::textureExists(sf::String resource_name) {
    return (this->textureMap.find(resource_name) != this->textureMap.end());
}

graphics::Texture* ResourceManager::getTexture(sf::String resource_name) {
    if (this->textureExists(resource_name)) {
        graphics::Texture* texture = this->textureMap[resource_name];
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
                                              resource_filepath_frag.toAnsiString()    << "' FAILED TO LOAD!" << 
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
// AnimatedTexture functions
AnimatedTexture* ResourceManager::animatedTextureLoad(sf::String resource_name, sf::String resource_filepath, bool split_both_ways, int frame_count_h, int frame_count_v, int total_frames) {
    if (this->animatedTextureExists(resource_name)) {
        AnimatedTexture *new_anim_tex = getAnimatedTexture(resource_name);
        return new_anim_tex;
    } else {
        AnimatedTexture *new_anim_tex = new AnimatedTexture(resource_filepath, split_both_ways, frame_count_h, frame_count_v, total_frames);
        if (new_anim_tex == nullptr) {
            std::cout << "[ERROR] Failed to create Animated Texture '" << resource_filepath.toAnsiString() << "'!" << std::endl;
            delete new_anim_tex;
            return nullptr;
        }

        // Animated texture has loaded successfully
        this->animatedTextureMap.insert(std::pair<sf::String, AnimatedTexture*>(resource_name, new_anim_tex));
        return new_anim_tex;
    }
}

AnimatedTexture* ResourceManager::getAnimatedTexture(sf::String resource_name) {
    if (this->animatedTextureExists(resource_name)) {
        AnimatedTexture* texture = this->animatedTextureMap[resource_name];
        return texture;
    } else {
        std::cout << "[ERROR] Animated Texture with '" << resource_name.toAnsiString() << "' Does not exist!" << std::endl;
        return nullptr;
    }
}

bool ResourceManager::animatedTextureExists(sf::String resource_name) {
    return (this->animatedTextureMap.find(resource_name) != this->animatedTextureMap.end());
}

void ResourceManager::animatedTextureUnload(sf::String resource_name) {
    if (this->animatedTextureExists(resource_name)) {
        AnimatedTexture* at = this->getAnimatedTexture(resource_name);
        delete at;
        this->animatedTextureMap.erase(resource_name);
    }
}

////////////////////////////////////////////////
// SoundBuffer functions
sf::SoundBuffer* ResourceManager::soundBufferLoad(sf::String resource_name, sf::String resource_filepath) {
    if (this->soundBufferExists(resource_name)) {
        sf::SoundBuffer *sound = this->getSoundBuffer(resource_name);
        return sound;
    } else {
        sf::SoundBuffer *sound_buffer = new sf::SoundBuffer();
        sound_buffer->loadFromFile(resource_filepath);
        if (sound_buffer == nullptr) {
            std::cout << "[ERROR] Failed to load sound: '" << resource_filepath.toAnsiString() << "!" << std::endl;
            delete sound_buffer;
        }
        
        // Store buffer in map
        this->soundBufferMap.insert(std::pair<sf::String, sf::SoundBuffer*>(resource_name, sound_buffer));
        return sound_buffer;
    }
}

sf::SoundBuffer* ResourceManager::getSoundBuffer(sf::String resource_name) {
    if (this->soundBufferExists(resource_name)) {
        sf::SoundBuffer* sound_buffer = this->soundBufferMap[resource_name];
        return sound_buffer;
    } else {
        std::cout << "[ERROR] Sound Buffer with '" << resource_name.toAnsiString() << "' Does not exist!" << std::endl;
        return nullptr;
    }
}

bool ResourceManager::soundBufferExists(sf::String resource_name) {
    return (this->soundBufferMap.find(resource_name) != this->soundBufferMap.end());
}

void ResourceManager::soundBufferUnload(sf::String resource_name) {
    if (this->soundBufferExists(resource_name)) {
        sf::SoundBuffer* sound_buffer = this->getSoundBuffer(resource_name);
        delete sound_buffer;
        this->soundBufferMap.erase(resource_name);
    }
}

////////////////////////////////////////////////
// Music functions
sf::Music* ResourceManager::musicLoad(sf::String resource_name, sf::String resource_filepath) {
    if (this->musicExists(resource_name)) {
        sf::Music *music = this->getMusic(resource_name);
        return music;
    } else {
        sf::Music *music = new sf::Music();
        bool success = music->openFromFile(resource_filepath);
        if (!success) {
            std::cout << "[ERROR] Music '" << resource_filepath.toAnsiString() << "' Failed to load!" << std::endl;
            delete music;
            return nullptr;
        }

        // Music has loaded successfully! (Add to map)
        this->musicMap.insert(std::pair<sf::String, sf::Music*>(resource_name, music));
        return music;
    }
}

sf::Music* ResourceManager::getMusic(sf::String resource_name) {
    if (this->musicExists(resource_name)) {
        sf::Music* music = this->musicMap[resource_name];
        return music;
    } else {
        std::cout << "[ERROR] Music with the name '" << resource_name.toAnsiString() << "' does not exist!" << std::endl;
        return nullptr;
    }
}

bool ResourceManager::musicExists(sf::String resource_name) {
    return (this->musicMap.find(resource_name) != this->musicMap.end());
}

void ResourceManager::musicUnload(sf::String resource_name) {
    if (this->musicExists(resource_name)) {
        sf::Music *music = this->getMusic(resource_name);
        delete music;
        this->musicMap.erase(resource_name);
    }
}

map<sf::String, sf::Music*>* ResourceManager::getMusicMap() {
    return &musicMap;
}
////////////////////////////////////////////////
// Font functions
sf::Font* ResourceManager::fontLoad(sf::String resource_name, sf::String resource_filepath) {
    if (fontExists(resource_name)) {
        return getFont(resource_name);
    } else {
        // load font
        sf::Font* new_font = new sf::Font();
        if (!new_font->loadFromFile(resource_filepath)) {
            std::cout << "[ERROR] FONT '" << resource_filepath.toAnsiString() << "' Failed to load!" << std::endl;
            delete new_font;
            return nullptr;
        }

        // Font loaded sucecssfully (add to map)
        fontMap.insert(std::pair<sf::String, sf::Font*>(resource_name, new_font));
        return getFont(resource_name);
    }
}

sf::Font* ResourceManager::getFont(sf::String resource_name) {
    if (fontExists(resource_name)) {
        return this->fontMap[resource_name];
    }
    else {
        std::cout << "[ERROR] Font with name '" << resource_name.toAnsiString() << "' does not exist" << std::endl;
        return nullptr;
    }
}

bool ResourceManager::fontExists(sf::String resource_name) {
    return (this->fontMap.find(resource_name) != this->fontMap.end());
}

void ResourceManager::fontUnload(sf::String resource_name) {
    sf::Font* font = this->getFont(resource_name);
    if (font != nullptr) {
        delete font;
        this->fontMap.erase(resource_name);
    }
}

sf::Text* ResourceManager::createTextFromFont(sf::String resource_name, sf::String default_text) {
    if (fontExists(resource_name)) {
        sf::Font *font = getFont(resource_name);
        if (font != nullptr) {
            sf::Text *new_text = new sf::Text();
            new_text->setFont(*font);
            new_text->setString(default_text);
            return new_text;
        }
    }
    return nullptr;
}

////////////////////////////////////////////////
// Release all resources
void ResourceManager::release() {

    // Texture map
    for (auto it = textureMap.begin(); it != textureMap.end(); it++) {
        this->textureUnload(it->first);
    }
    textureMap.clear();

    // Shader map
    for (auto it = shaderMap.begin(); it != shaderMap.end(); it++) {
        this->shaderUnload(it->first);
    }
    shaderMap.clear();

    // TODO: Unload Mesh Map

    // Unload AnimatedTexture
    for (auto it = animatedTextureMap.begin(); it != animatedTextureMap.end(); it++) {
        this->animatedTextureUnload(it->first);
    }
    animatedTextureMap.clear();

    // Unload SoundBuffers
    for (auto it = soundBufferMap.begin(); it != soundBufferMap.end(); it++) {
        this->soundBufferUnload(it->first);
    }
    soundBufferMap.clear();

    // Unload Music
    for (auto it = musicMap.begin(); it != musicMap.end(); it++) {
        this->musicUnload(it->first);
    }

    // Unload fonts
    for (auto it = fontMap.begin(); it != fontMap.end(); it++) {
        this->fontUnload(it->first);
    }

    musicMap.clear();
}