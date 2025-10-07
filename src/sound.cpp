#include "sound.h"

SoundSystem::SoundSystem() 
    : device(nullptr), context(nullptr), bounceBuffer(0), bounceSource(0), 
      initialized(false), volume(0.7f) {
}

SoundSystem::~SoundSystem() {
    cleanup();
}

bool SoundSystem::initialize() {
    // Open the default audio device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return false;
    }
    
    // Create audio context
    context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        alcCloseDevice(device);
        device = nullptr;
        return false;
    }
    
    // Make context current
    if (!alcMakeContextCurrent(context)) {
        std::cerr << "Failed to make OpenAL context current" << std::endl;
        alcDestroyContext(context);
        alcCloseDevice(device);
        context = nullptr;
        device = nullptr;
        return false;
    }
    
    // Generate buffers and sources
    alGenBuffers(1, &bounceBuffer);
    alGenSources(1, &bounceSource);
    
    // Check for OpenAL errors
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error during initialization: " << error << std::endl;
        cleanup();
        return false;
    }
    
    // Generate the bounce sound
    generateBounceSound();
    
    initialized = true;
    return true;
}

void SoundSystem::cleanup() {
    if (initialized) {
        // Stop and delete sources
        if (bounceSource != 0) {
            alSourceStop(bounceSource);
            alDeleteSources(1, &bounceSource);
            bounceSource = 0;
        }
        
        // Delete buffers
        if (bounceBuffer != 0) {
            alDeleteBuffers(1, &bounceBuffer);
            bounceBuffer = 0;
        }
        
        initialized = false;
    }
    
    // Clean up context and device
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }
    
    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
}

void SoundSystem::generateBounceSound() {
    const int sampleRate = 22050;
    const float duration = 0.15f; // Short bounce sound
    std::vector<short> soundData;
    
    // Generate a bounce sound (frequency sweep from high to low)
    const float startFreq = 800.0f;
    const float endFreq = 200.0f;
    const int numSamples = static_cast<int>(sampleRate * duration);
    
    soundData.resize(numSamples);
    
    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / static_cast<float>(numSamples - 1);
        float freq = startFreq + (endFreq - startFreq) * t;
        float envelope = (1.0f - t) * 0.8f; // Decay envelope
        
        float sample = sin(2.0f * M_PI * freq * t * duration) * envelope;
        soundData[i] = static_cast<short>(sample * 32767.0f);
    }
    
    // Upload to OpenAL buffer
    alBufferData(bounceBuffer, AL_FORMAT_MONO16, soundData.data(), 
                soundData.size() * sizeof(short), sampleRate);
    
    // Set up the source
    alSourcei(bounceSource, AL_BUFFER, bounceBuffer);
    alSourcef(bounceSource, AL_GAIN, volume);
    alSourcef(bounceSource, AL_PITCH, 1.0f);
    alSource3f(bounceSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
}

void SoundSystem::playBounceSound(CollisionType collision) {
    if (!initialized || collision == CollisionType::NONE) {
        return;
    }
    
    // Stop any currently playing sound
    alSourceStop(bounceSource);
    
    // Modify pitch based on collision type
    float pitch = 1.0f;
    switch (collision) {
        case CollisionType::FLOOR:
            pitch = 0.8f; // Lower pitch for floor hits
            break;
        case CollisionType::CEILING:
            pitch = 1.2f; // Higher pitch for ceiling hits
            break;
        default:
            pitch = 1.0f; // Normal pitch for walls
            break;
    }
    
    alSourcef(bounceSource, AL_PITCH, pitch);
    alSourcePlay(bounceSource);
}

void SoundSystem::setVolume(float volume) {
    this->volume = std::max(0.0f, std::min(1.0f, volume));
    if (initialized) {
        alSourcef(bounceSource, AL_GAIN, this->volume);
    }
}

void SoundSystem::generateSineWave(std::vector<short>& data, float frequency, float duration, int sampleRate) {
    int numSamples = static_cast<int>(sampleRate * duration);
    data.resize(numSamples);
    
    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / static_cast<float>(sampleRate);
        float sample = sin(2.0f * M_PI * frequency * t);
        data[i] = static_cast<short>(sample * 32767.0f);
    }
}
