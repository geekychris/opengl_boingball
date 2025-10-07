#pragma once

#include "common.h"
#include "physics.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

class SoundSystem {
public:
    SoundSystem();
    ~SoundSystem();
    
    bool initialize();
    void cleanup();
    
    void generateBounceSound();
    void playBounceSound(CollisionType collision);
    
    void setVolume(float volume);

private:
    ALCdevice* device;
    ALCcontext* context;
    
    ALuint bounceBuffer;
    ALuint bounceSource;
    
    bool initialized;
    float volume;
    
    void generateSineWave(std::vector<short>& data, float frequency, float duration, int sampleRate);
};
