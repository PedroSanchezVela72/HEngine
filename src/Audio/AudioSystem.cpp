#include "AudioSystem.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include "LoadLua.h"
#include <Manager.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include "Transform.h"

// Definiciones de variables para el motor
#include "H_Defs.h"
#include "Debuglog.h"

using namespace H;

/// <summary>
/// Comprueba el resultado de una funcion, lanzando un error si no ha sido valido
/// </summary>
static void checkFMODResult(FMOD_RESULT result, const char* message) {
    if (result != FMOD_OK) {
        std::string error = "FMOD Error : ";
        error += message;
        error += " - ";
        error += FMOD_ErrorString(result);
        DebugLog::instance()->throwLog(error);
    }
}

/// <summary>
/// Convierte de nuestro Vector3 al Vector de Fmod
/// </summary>
static FMOD_VECTOR vectorToFmod(const Vector3<float>& vector)
{
    FMOD_VECTOR vec;
    vec.x = vector.x;
    vec.y = vector.y;
    vec.z = vector.z;
    return vec;
}

AudioSystem::AudioSystem() : _soundSystem(nullptr) {  }

AudioSystem::~AudioSystem()
{
    checkFMODResult(_soundSystem->close(), "Closing system");
    checkFMODResult(_soundSystem->release(), "Releasing system");

    // delete _soundSystem;
    _soundSystem = nullptr;
}

void AudioSystem::initSystem()
{
    checkFMODResult(FMOD::System_Create(&_soundSystem), "Failed to create system");
    checkFMODResult(_soundSystem->init(H_MAX_AUDIO_CHANNELS, FMOD_INIT_NORMAL, nullptr), "Failed to initialize system");
}


void AudioSystem::update(double deltaTime) {

    const std::list<Entity*>& audioSources = _mngr->getHandler(_hdlr_AUDIO);

    for (Entity* ent : audioSources) {
        if (_mngr->hasComponent<AudioSource>(ent)) {
          
            AudioSource* as = _mngr->getComponent<AudioSource>(ent);
            if (_mngr->hasComponent<Transform>(ent)) {
                Transform* tr = _mngr->getComponent<Transform>(ent);
                FMOD_VECTOR fmodPos = vectorToFmod(tr->position);
                as->_channel->set3DAttributes(&fmodPos, nullptr);
            }        
        }

        if (_mngr->hasComponent<AudioListener>(ent)) {
            AudioListener* al = _mngr->getComponent<AudioListener>(ent);
            FMOD_VECTOR fw = vectorToFmod(al->forward);
            FMOD_VECTOR up = vectorToFmod(al->up);
            FMOD_VECTOR vel = vectorToFmod(al->velocity);
            if (_mngr->hasComponent<Transform>(ent)) {
                Transform* tr = _mngr->getComponent<Transform>(ent);               
                FMOD_VECTOR pos = vectorToFmod(tr->position);
                _soundSystem->set3DListenerAttributes(0, &pos, &fw, &up, &vel);
            }
        }
    }

    checkFMODResult(_soundSystem->update(), "Sound System update");
}


void AudioSystem::receive(const Message* m)
{
    switch (m->id) {
        case _m_PLAY_SOUND:
            playSound(m->entity.entityPtr, m->sound.soundName);
            break;
        case _m_ADD_SOUND:
            addSound(m->entity.entityPtr, m->sound.soundName, m->sound.soundPath);
            break;
        case _m_PAUSE_SOUND:
            pauseChannel(m->entity.entityPtr, true);
            break;
        case _m_STOP_SOUND:
            stopChannel(m->entity.entityPtr);
            break;
        case _m_INIT_ENTITY:
            addEntity(m->entity.entityPtr, m->entity.entityName);
            break;
    }
}

void AudioSystem::playSound(Entity* ent, std::string soundName)
{
    if (_mngr->hasComponent<AudioSource>(ent)) {
        FMOD::Channel* channel = nullptr;
        AudioSource* as = _mngr->getComponent<AudioSource>(ent);
        if (as->_channel)channel = as->_channel;
        FMOD_RESULT result = _soundSystem->playSound(as->_sounds[soundName], nullptr, false, &channel);
        if (result == FMOD_OK) {
            if (!as->_channel)as->_channel = channel;
        }
        checkFMODResult(result, "Playing Sound");

        if (channel) {
            FMOD_MODE mode;
            as->_channel->getMode(&mode);
            if (mode == FMOD_3D && _mngr->hasComponent<Transform>(ent)) {
                Transform* tr = _mngr->getComponent<Transform>(ent);
                FMOD_VECTOR position = vectorToFmod(tr->position);
                checkFMODResult(channel->set3DAttributes(&position, nullptr), "Setting 3d attributes");
            }
        }
    }
    else {
        std::string error = "AudioSource not found";
        DebugLog::instance()->throwLog(error);
    }
}

void AudioSystem::stopChannel(Entity* ent)
{
    if (_mngr->hasComponent<AudioSource>(ent) && _mngr->isAlive(ent)) {
        FMOD::Channel* channel = _mngr->getComponent<AudioSource>(ent)->_channel;
        
        if (channel) {
            bool isValid = false;
            
            if (channel->isPlaying(&isValid) == FMOD_OK && isValid)checkFMODResult(channel->stop(), "Stopping channel");
        }       
    }
    else {
        std::string error = "AudioSource not found";
        DebugLog::instance()->throwLog(error);
    }
}

void AudioSystem::pauseChannel(Entity* ent, bool pause)
{
    if (_mngr->hasComponent<AudioSource>(ent)) {
        checkFMODResult(_mngr->getComponent<AudioSource>(ent)->_channel->stop(), "Pausing channel");
    }
    else {
        std::string error = "AudioSource not found";
        DebugLog::instance()->throwLog(error);
    }
}

void AudioSystem::setVolume(Entity* ent, float volume)
{
    if (_mngr->hasComponent<AudioSource>(ent)) {
        FMOD::Channel* channel = _mngr->getComponent<AudioSource>(ent)->_channel;
        if (channel) {
            bool isValid = false;

            if (channel->isPlaying(&isValid) == FMOD_OK && isValid)checkFMODResult(channel->setVolume(volume), "Setting volume");
        }       
    }
    else {
        std::string error = "AudioSource not found";
        DebugLog::instance()->throwLog(error);
    }
}

FMOD::Sound* AudioSystem::getSound(Entity* ent, std::string soundName) const
{
    if (_mngr->hasComponent<AudioSource>(ent)) {
        return _mngr->getComponent<AudioSource>(ent)->_sounds[soundName];
    }
    else {
        std::string error = "AudioSource not found";
        DebugLog::instance()->throwLog(error);
        return nullptr;
    }
}

void H::AudioSystem::addSound(Entity* ent, std::string soundName, std::string filePath)
{
    if (_mngr->hasComponent<AudioSource>(ent)) {
        AudioSource* as = _mngr->getComponent<AudioSource>(ent);
        FMOD::Sound* sound = nullptr;
        FMOD_RESULT result = _soundSystem->createSound(filePath.c_str(), as->_mode, nullptr, &sound);
        checkFMODResult(result, "Loading sound");

        as->_sounds[soundName] = sound;
        as->_soundNames.push_back(soundName);
    }
    else {
        std::string error = "AudioSource not found";
        DebugLog::instance()->throwLog(error);
    }
}

FMOD::Channel* AudioSystem::getChannel(Entity* ent) const
{
    if (_mngr->hasComponent<AudioSource>(ent)) {
        return _mngr->getComponent<AudioSource>(ent)->_channel;
    }
    else {
        std::string error = "AudioSource not found";
        DebugLog::instance()->throwLog(error);
        return nullptr;
    }
}



void AudioSystem::addEntity(Entity* entity, std::string name)
{
    LoadLua* lua = LoadLua::instance();

    if (lua->hasComponent(name, "AudioListener")) {
        Vector3F pos = { 0,0,0 };

        Vector3F forward = { 0,0,0 };

        forward = {
            lua->loadFloatValue(name, {"AudioListener","forward", "x"}),
            lua->loadFloatValue(name, {"AudioListener","forward", "y"}),
            lua->loadFloatValue(name, {"AudioListener","forward", "z"})
        };

        Vector3F up = { 0,0,0 };

        forward = {
            lua->loadFloatValue(name, {"AudioListener","up", "x"}),
            lua->loadFloatValue(name, {"AudioListener","up", "y"}),
            lua->loadFloatValue(name, {"AudioListener","up", "z"})
        };

        Vector3F vel = { 0,0,0 };

        forward = {
            lua->loadFloatValue(name, {"AudioListener","velocity", "x"}),
            lua->loadFloatValue(name, {"AudioListener","velocity", "y"}),
            lua->loadFloatValue(name, {"AudioListener","velocity", "z"})
        };
        FMOD_VECTOR Fpos = vectorToFmod(pos);
        FMOD_VECTOR Ffor = vectorToFmod(forward);
        FMOD_VECTOR Fup = vectorToFmod(up);
        FMOD_VECTOR Fvel = vectorToFmod(vel);
        if (_mngr->hasComponent<Transform>(entity)) {
            Transform* tr = _mngr->getComponent<Transform>(entity);
            Fpos = vectorToFmod(tr->position);
        }

        _mngr->addComponent<AudioListener>(entity, forward, up, vel);

        _soundSystem->set3DListenerAttributes(0, &Fpos, &Ffor, &Fup, &Fvel);
        _mngr->setHandler(_hdlr_AUDIO, entity);
    }

    if (lua->hasComponent(name, "AudioSource")) {
        int numSounds = lua->loadIntValue(name, { "AudioSource", "numSounds" });
        std::string modeStr = lua->loadStrValue(name, { "AudioSource", "mode" });
        FMOD_MODE mode = modeStr == "3D" ? FMOD_3D : FMOD_2D;
        std::list<std::string> soundNames;
        std::unordered_map<std::string, FMOD::Sound*> sounds;
        std::unordered_map<std::string, std::string> soundFilePaths;
        for (int i = 0; i < numSounds; i++) {
            std::string filePath = lua->loadStrValue(name, { "AudioSource", "filePaths", "filePath"+std::to_string(i)});
            filePath = "Assets/Sounds/" + filePath;
            std::string soundName = lua->loadStrValue(name, { "AudioSource", "soundNames", "soundName"+std::to_string(i)});
            bool loop = false;
            if (lua->hasProperty(name, { "AudioSource", "loop" }))loop = lua->loadBoolValue(name, { "AudioSource", "loop" });
            FMOD::Sound* sound = nullptr;
            if(loop) mode = mode | FMOD_LOOP_NORMAL;
            FMOD_RESULT result = _soundSystem->createSound(filePath.c_str(), mode, nullptr, &sound);
            checkFMODResult(result, "Loading sound");
            sounds[soundName] = sound;
            soundFilePaths[soundName] = filePath;
            soundNames.push_back(soundName);
        }
       
        float volume = lua->loadFloatValue(name, { "AudioSource", "volume" });     

        AudioSource* as = _mngr->addComponent<AudioSource>(entity, soundNames, soundFilePaths, sounds, volume, mode);
        _mngr->setHandler(_hdlr_AUDIO, entity);
    }
}
