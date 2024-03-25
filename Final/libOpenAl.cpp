#include "pch.h"
#include "libOpenAl.h"

#define CMP_OPAL_SOUND_MGR

#include "libOpenAl.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"

/**
*  libOpenAl.cpp
*
*  Original Code : Van Stokes, Jr. (http://www.EvilMasterMindsInc.com) - Aug 05
*  Modified Code : Steven Gay - mec3@bluewin.ch - Septembre 2005
*  Dll version   : Griffo Jean-Baptiste , crashy@antharia.org - Jan 06
*/

SoundManager* SoundManager::mSoundManager = NULL;

/****************************************************************************/
OPAL_SOUND_MGR SoundManager::SoundManager(int max_buffers = 64, int max_sources = 16)
    : MAX_AUDIO_BUFFERS(max_buffers), MAX_AUDIO_SOURCES(max_sources)
{

    mSoundManager = this;

    isInitialised = false;
    isSoundOn = false;
    mSoundDevice = 0;
    mSoundContext = 0;

    mAudioPath = "";

    // EAX related
    isEAXPresent = false;

    // Initial position of the listener
    position[0] = 0.0;
    position[1] = 0.0;
    position[2] = 0.0;

    // Initial velocity of the listener
    velocity[0] = 0.0;
    velocity[1] = 0.0;
    velocity[2] = 0.0;

    // Initial orientation of the listener = direction + direction up
    orientation[0] = 0.0;
    orientation[1] = 0.0;
    orientation[2] = -1.0;
    orientation[3] = 0.0;
    orientation[4] = 1.0;
    orientation[5] = 0.0;

    // Needed because of hardware limitation
    mAudioBuffersInUseCount = 0;
    mAudioSourcesInUseCount = 0;
    mAudioSources = new unsigned int[MAX_AUDIO_SOURCES];
    mAudioSourceInUse = new bool[MAX_AUDIO_SOURCES];

    for (int i = 0; i < MAX_AUDIO_SOURCES; i++)
    {
        mAudioSources[i] = 0;
        mAudioSourceInUse[i] = false;
    }

    mAudioBuffers = new unsigned int[MAX_AUDIO_BUFFERS];
    mAudioBufferInUse = new bool[MAX_AUDIO_BUFFERS];
    for (int i = 0; i < MAX_AUDIO_BUFFERS; i++)
    {
        mAudioBuffers[i] = 0;

        mAudioBufferInUse[i] = false;
    }

    printf("SoudManager Created.\n");
}

/****************************************************************************/
OPAL_SOUND_MGR SoundManager::~SoundManager(void)
{
    // Delete the sources and buffers
    alDeleteSources(MAX_AUDIO_SOURCES, mAudioSources);
    alDeleteBuffers(MAX_AUDIO_BUFFERS, mAudioBuffers);

    // Destroy the sound context and device
    mSoundContext = alcGetCurrentContext();
    mSoundDevice = alcGetContextsDevice(mSoundContext);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(mSoundContext);
    if (mSoundDevice)
        alcCloseDevice(mSoundDevice);

    alutExit();

    printf("SoudManager Destroyed.\n");
    mAudioBufferFileName.clear();
    delete mAudioSources;
    delete mAudioSourceInUse;
}

/****************************************************************************/
OPAL_SOUND_MGR void SoundManager::selfDestruct(void)
{
    if (getSingletonPtr()) delete getSingletonPtr();
}

/****************************************************************************/
OPAL_SOUND_MGR SoundManager* SoundManager::createManager(void)
{
    if (mSoundManager == 0)
        mSoundManager = new SoundManager();
    return mSoundManager;
}
OPAL_SOUND_MGR SoundManager* SoundManager::createManager(int max_sources, int max_buffers)
{
    if (mSoundManager == 0)
        mSoundManager = new SoundManager(max_sources, max_buffers);
    return mSoundManager;
}
/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::init(void)
{
    // It's an error to initialise twice OpenAl
    if (isInitialised) return true;

    // Open an audio device
    mSoundDevice = alcOpenDevice(NULL); // TODO ((ALubyte*) "DirectSound3D");
    // mSoundDevice = alcOpenDevice( "DirectSound3D" );

    // Check for errors
    if (!mSoundDevice)
    {
        printf("SoundManager::init error : No sound device.\n");
        return false;
    }

    mSoundContext = alcCreateContext(mSoundDevice, NULL);
    //   if ( checkAlError() || !mSoundContext ) // TODO seems not to work! why ?
    if (!mSoundContext)
    {
        printf("SoundManager::init error : No sound context.\n");
        return false;
    }

    // Make the context current and active
    alcMakeContextCurrent(mSoundContext);
    if (checkALError("Init()"))
    {
        printf("SoundManager::init error : could not make sound context current and active.\n");
        return false;
    }

    // Check for EAX 2.0 support and
    // Retrieves function entry addresses to API ARB extensions, in this case,
    // for the EAX extension. See Appendix 1 (Extensions) of
    // http://www.openal.org/openal_webstf/specs/OpenAL1-1Spec_html/al11spec7.html
    //
    // TODO EAX fct not used anywhere in the code ... !!!
    isEAXPresent = alIsExtensionPresent("EAX2.0");
    if (isEAXPresent)
    {
        printf("EAX 2.0 Extension available\n");

#ifdef _USEEAX
        eaxSet = (EAXSet)alGetProcAddress("EAXSet");
        if (eaxSet == NULL)
            isEAXPresent = false;

        eaxGet = (EAXGet)alGetProcAddress("EAXGet");
        if (eaxGet == NULL)
            isEAXPresent = false;

        if (!isEAXPresent)
            checkALError("Failed to get the EAX extension functions adresses.\n");
#else
        isEAXPresent = false;
        printf("... but not used.\n");
#endif // _USEEAX

    }

    // Test if Ogg Vorbis extension is present
    isOggExtensionPresent();

    // Create the Audio Buffers
    alGenBuffers(MAX_AUDIO_BUFFERS, mAudioBuffers);
    if (checkALError("init::alGenBuffers:"))
        return false;

    // Generate Sources
    alGenSources(MAX_AUDIO_SOURCES, mAudioSources);
    if (checkALError("init::alGenSources :"))
        return false;


    // Setup the initial listener parameters
    // -> location
    alListenerfv(AL_POSITION, position);

    // -> velocity
    alListenerfv(AL_VELOCITY, velocity);

    // -> orientation
    alListenerfv(AL_ORIENTATION, orientation);

    // Gain
    alListenerf(AL_GAIN, 1.0);

    // Initialise Doppler
    alDopplerFactor(1.0); // 1.2 = exaggerate the pitch shift by 20%
    alDopplerVelocity(343.0f); // m/s this may need to be scaled at some point

    // Ok
    isInitialised = true;
    isSoundOn = true;

    printf("SoundManager initialised.\n\n");

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::checkALError(void)
{
    ALenum errCode;
    if ((errCode = alGetError()) != AL_NO_ERROR)
    {
        std::string err = "ERROR SoundManager:: ";
        err += (char*)alGetString(errCode);

        printf("%s\n", err.c_str());
        return true;
    }
    return false;
}

/****************************************************************************/
OPAL_SOUND_MGR std::string SoundManager::listAvailableDevices(void)
{
    std::string str = "Sound Devices available : ";

    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)
    {
        str = "List of Devices : ";
        str += (char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        str += "\n";
    }
    else
        str += " ... eunmeration error.\n";

    return str;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::checkALError(std::string pMsg)
{
    ALenum error = 0;

    if ((error = alGetError()) == AL_NO_ERROR)
        return false;

    char mStr[256];
    switch (error)
    {
    case AL_INVALID_NAME:
        sprintf(mStr, "ERROR SoundManager::%s Invalid Name", pMsg.c_str());
        break;
    case AL_INVALID_ENUM:
        sprintf(mStr, "ERROR SoundManager::%s Invalid Enum", pMsg.c_str());
        break;
    case AL_INVALID_VALUE:
        sprintf(mStr, "ERROR SoundManager::%s Invalid Value", pMsg.c_str());
        break;
    case AL_INVALID_OPERATION:
        sprintf(mStr, "ERROR SoundManager::%s Invalid Operation", pMsg.c_str());
        break;
    case AL_OUT_OF_MEMORY:
        sprintf(mStr, "ERROR SoundManager::%s Out Of Memory", pMsg.c_str());
        break;
    default:
        sprintf(mStr, "ERROR SoundManager::%s Unknown error (%i) case in testALError()",
            pMsg.c_str(), error);
        break;
    };

    printf("%s\n", mStr);

    return true;
}

/*****************************************************************************/
OPAL_SOUND_MGR void SoundManager::testSound(const char* wavFile)
{
    ALuint buffer;
    ALuint source;
    ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;
    alGenBuffers(1, &buffer);
    if (checkALError("testSound()"))
        return;

    // This is the same for alutLoadWAVMemory
#ifndef MACINTOSH_AL
#pragma warning(suppress : 4996)
    alutLoadWAVFile((ALbyte*)wavFile, &format, &data, &size, &freq, &loop);
#else
    alutLoadWAVFile((ALbyte*)wavFile, &format, &data, &size, &freq);
#endif

    alBufferData(buffer, format, data, size, freq);
#pragma warning(suppress : 4996)
    alutUnloadWAV(format, data, size, freq);
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);

#ifndef MACINTOSH_AL
    alSourcei(source, AL_LOOPING, loop);
#else
    alSourcei(source, AL_LOOPING, false); // TODO ! But how to get from file?
#endif

    alSourcePlay(source);

    //Or else we risk to destroy the manager too quickly to here anything !
    for (int i = 0; i < 50000; i++) { printf("."); };
}

// Attempts to aquire an empty audio source and assign it back to the caller
// via AudioSourceID. This will lock the source
/*****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::loadAudio(std::string filename, unsigned int* audioId,
    bool loop)
{
    if (filename.empty() || filename.length() > MAX_FILENAME_LENGTH)
        return false;

    if (mAudioSourcesInUseCount == MAX_AUDIO_SOURCES)
        return false;   // out of Audio Source slots!

    int bufferID = -1;   // Identity of the Sound Buffer to use
    int sourceID = -1;   // Identity of the Source Buffer to use

    alGetError();    // Clear Error Code

    // Check and see if the pSoundFile is already loaded into a buffer
    bufferID = locateAudioBuffer(filename);
    if (bufferID < 0)
    {
        // The sound file isn't loaded in a buffer, lets attempt to load it on the fly
        bufferID = loadAudioInToSystem(filename);
        if (bufferID < 0) return false;   // failed!
    }

    // If you are here, the sound the requester wants to reference is in a buffer.
    // Now, we need to find a free Audio Source slot in the sound system
    sourceID = 0;

    while (mAudioSourceInUse[sourceID] == true) sourceID++;

    // When you are here, 'mSourceID' now represents a free Audio Source slot
    // The free slot may not be at the end of the array but in the middle of it.
    *audioId = sourceID;  // return the Audio Source ID to the caller
    mAudioSourceInUse[sourceID] = true; // mark this Source slot as in use
    mAudioSourcesInUseCount++;    // bump the 'in use' counter

    // Now inform OpenAL of the sound assignment and attach the audio buffer
    // to the audio source
    alSourcei(mAudioSources[sourceID], AL_BUFFER, mAudioBuffers[bufferID]);

    // Steven : Not in the original code !!!!!
    alSourcei(mAudioSources[sourceID], AL_LOOPING, loop);

    if (checkALError("loadSource()::alSourcei"))
        return false;

    return true;
}

// Function to check and see if the pSoundFile is already loaded into a buffer
/*****************************************************************************/
OPAL_SOUND_MGR int SoundManager::locateAudioBuffer(std::string filename)
{
    for (unsigned int b = 0; b < MAX_AUDIO_BUFFERS; b++)
    {
        if (filename == mAudioBufferFileName[b]) return (int)b;
        //Surely a way to optimize it with the MAP
    }
    return -1;      // not found
}

// Function to load a sound file into an AudioBuffer
/*****************************************************************************/
OPAL_SOUND_MGR int SoundManager::loadAudioInToSystem(std::string filename)
{
    if (filename.empty())
        return -1;

    // Make sure we have audio buffers available
    if (mAudioBuffersInUseCount == MAX_AUDIO_BUFFERS) return -1;

    // Find a free Audio Buffer slot
    int bufferID = 0;      // Identity of the Sound Buffer to use

    while (mAudioBufferInUse[bufferID] == true) bufferID++;

    // load .wav, .ogg or .au
    if (filename.find(".wav", 0) != std::string::npos)
    {
        printf(" ---> found Wav\n");
        // When you are here, bufferID now represents a free Audio Buffer slot
          // Attempt to load the SoundFile into the buffer
        if (!loadWAV(filename, mAudioBuffers[bufferID])) return -1;
    }
    else if (filename.find(".ogg", 0) != std::string::npos)
    {
        printf(" ---> found ogg\n");
        std::cout << filename << std::endl;
        if (!loadOGG(filename, mAudioBuffers[bufferID])) return -1;

    }
    else if (filename.find(".au", 0) != std::string::npos)
    {
        printf(" ---> found au\n");
        // TODO if ( !loadAU( filename, mAudioBuffers[mBufferID]) ) return -1;
    }

    // Successful load of the file into the Audio Buffer.
    mAudioBufferInUse[bufferID] = true;      // Buffer now in use
    mAudioBufferFileName[bufferID] = filename;   // save the file descriptor


    mAudioBuffersInUseCount++;               // bump the 'in use' counter

    return bufferID;
}

// Function to load a wave file and assigned it to a buffer
/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::loadWAV(std::string filename, ALuint pDestAudioBuffer)
{
    ALenum      format;         //for the buffer format
    ALsizei      size;         //the bit depth
    ALsizei      freq;         //for the frequency of the buffer
    ALboolean   loop;         //looped
    ALvoid* data;         //data for the buffer

    std::string mFullPath = mAudioPath;

    alGetError();   // Clear Error Code

    // Load in the WAV file from disk
    //mFullPath += "\\";
    mFullPath += filename;

#ifndef MACINTOSH_AL
#pragma warning(suppress : 4996)
    alutLoadWAVFile((ALbyte*)mFullPath.c_str(), &format, &data, &size, &freq, &loop);
#else
    alutLoadWAVFile((ALbyte*)mFullPath.c_str(), &format, &data, &size, &freq);
#endif

    if (checkALError("loadWAV::alutLoadWAVFile: "))
        return false;

    // Copy the new WAV data into the buffer
    alBufferData(pDestAudioBuffer, format, data, size, freq);
    if (checkALError("loadWAV::alBufferData: "))
        return false;

    // Unload the WAV file
#pragma warning(suppress : 4996)
    alutUnloadWAV(format, data, size, freq);
    if (checkALError("loadWAV::alutUnloadWAV: "))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::playAudio(unsigned int audioID, bool forceRestart)
{
    // Make sure the audio source ident is valid and usable
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    int sourceAudioState = 0;

    alGetError();

    // Are we currently playing the audio source?
    alGetSourcei(mAudioSources[audioID], AL_SOURCE_STATE, &sourceAudioState);

    if (sourceAudioState == AL_PLAYING)
    {
        if (forceRestart)
            stopAudio(audioID);
        else
            return false; // Not forced, so we don't do anything
    }

    alSourcePlay(mAudioSources[audioID]);
    if (checkALError("playAudio::alSourcePlay: "))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::pauseAudio(unsigned int audioID)
{
    // Make sure the audio source ident is valid and usable
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    alGetError();

    alSourcePause(mAudioSources[audioID]);

    if (checkALError("pauseAudio::alSourceStop "))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::pauseAllAudio(void)
{
    if (mAudioSourcesInUseCount >= MAX_AUDIO_SOURCES)
        return false;

    alGetError();

    alSourcePausev(mAudioSourcesInUseCount, mAudioSources);

    if (checkALError("pauseAllAudio::alSourceStop "))
        return false;

    return true;
}

// We could use playAudio instead !
/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::resumeAudio(unsigned int audioID)
{
    // Make sure the audio source ident is valid and usable
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    alGetError();

    // If the sound was paused the sound will resume, else it will play from
    // the beginning !
    // TODO No check for forced restart. Verify if it is what you want ?
    alSourcePlay(mAudioSources[audioID]);

    if (checkALError("resumeAudio::alSourceStop "))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::resumeAllAudio(void)
{
    if (mAudioSourcesInUseCount >= MAX_AUDIO_SOURCES)
        return false;

    alGetError();

    int sourceAudioState = 0;

    for (int i = 0; i < mAudioSourcesInUseCount; i++)
    {
        // Are we currently playing the audio source?
        alGetSourcei(mAudioSources[i], AL_SOURCE_STATE, &sourceAudioState);

        if (sourceAudioState == AL_PAUSED)
        {
            resumeAudio(i);
        }
    }

    if (checkALError("resumeAllAudio::alSourceStop "))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::stopAudio(unsigned int audioID)
{
    // Make sure the audio source ident is valid and usable
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    alGetError();

    alSourceStop(mAudioSources[audioID]);

    if (checkALError("stopAudio::alSourceStop "))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::stopAllAudio(void)
{
    if (mAudioSourcesInUseCount >= MAX_AUDIO_SOURCES)
        return false;

    alGetError();

    for (int i = 0; i < mAudioSourcesInUseCount; i++)
    {
        stopAudio(i);
    }

    if (checkALError("stopAllAudio::alSourceStop "))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::releaseAudio(unsigned int audioID)
{
    if (audioID >= MAX_AUDIO_SOURCES)
        return false;
    alSourceStop(mAudioSources[audioID]);
    mAudioSourceInUse[audioID] = false;
    mAudioSourcesInUseCount--;
    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::setSound(unsigned int audioID, Vector3 position,
    Vector3 velocity, Vector3 direction, float maxDistance,
    bool playNow, bool forceRestart, float minGain)
{
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    // Set the position
    ALfloat pos[] = { position.x, position.y, position.z };

    alSourcefv(mAudioSources[audioID], AL_POSITION, pos);

    if (checkALError("setSound::alSourcefv:AL_POSITION"))
        return false;

    // Set the veclocity
    ALfloat vel[] = { velocity.x, velocity.y, velocity.z };

    alSourcefv(mAudioSources[audioID], AL_VELOCITY, vel);

    if (checkALError("setSound::alSourcefv:AL_VELOCITY"))
        return false;

    // Set the direction
    ALfloat dir[] = { velocity.x, velocity.y, velocity.z };

    alSourcefv(mAudioSources[audioID], AL_DIRECTION, dir);

    if (checkALError("setSound::alSourcefv:AL_DIRECTION"))
        return false;

    // Set the max audible distance
    alSourcef(mAudioSources[audioID], AL_MAX_DISTANCE, maxDistance);

    // Set the MIN_GAIN ( IMPORTANT - if not set, nothing audible! )
    alSourcef(mAudioSources[audioID], AL_MIN_GAIN, minGain);

    // Set the max gain
    alSourcef(mAudioSources[audioID], AL_MAX_GAIN, 1.0f); // TODO as parameter ? global ?

    // Set the rollof factor
    alSourcef(mAudioSources[audioID], AL_ROLLOFF_FACTOR, 1.0f); // TODO as parameter ?

    // Do we play the sound now ?
    if (playNow) return playAudio(audioID, forceRestart); // TODO bof... not in this fct

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::setSoundPosition(unsigned int audioID, Vector3 position)
{
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    // Set the position
    ALfloat pos[] = { position.x, position.y, position.z };

    alSourcefv(mAudioSources[audioID], AL_POSITION, pos);

    if (checkALError("setSound::alSourcefv:AL_POSITION"))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::setSoundPosition(unsigned int audioID, Vector3 position,
    Vector3 velocity, Vector3 direction)
{
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    // Set the position
    ALfloat pos[] = { position.x, position.y, position.z };

    alSourcefv(mAudioSources[audioID], AL_POSITION, pos);

    if (checkALError("setSound::alSourcefv:AL_POSITION"))
        return false;

    // Set the veclocity
    ALfloat vel[] = { velocity.x, velocity.y, velocity.z };

    alSourcefv(mAudioSources[audioID], AL_VELOCITY, vel);

    if (checkALError("setSound::alSourcefv:AL_VELOCITY"))
        return false;

    // Set the direction
    ALfloat dir[] = { velocity.x, velocity.y, velocity.z };

    alSourcefv(mAudioSources[audioID], AL_DIRECTION, dir);

    if (checkALError("setSound::alSourcefv:AL_DIRECTION"))
        return false;

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::setListenerPosition(Vector3 position, Vector3 velocity,
    Quaternion orientation)
{
    Vector3 axis;

    // Set the position
    ALfloat pos[] = { position.x, position.y, position.z };

    alListenerfv(AL_POSITION, pos);

    if (checkALError("setListenerPosition::alListenerfv:AL_POSITION"))
        return false;

    // Set the veclocity
    ALfloat vel[] = { velocity.x, velocity.y, velocity.z };

    alListenerfv(AL_VELOCITY, vel);

    if (checkALError("setListenerPosition::alListenerfv:AL_VELOCITY"))
        return false;

    // Orientation of the listener : look at then look up
    axis = Vector3::ZERO;
    axis.x = orientation.getYaw().valueRadians();
    axis.y = orientation.getPitch().valueRadians();
    axis.z = orientation.getRoll().valueRadians();

    // Set the direction
    ALfloat dir[] = { axis.x, axis.y, axis.z };

    alListenerfv(AL_ORIENTATION, dir);

    if (checkALError("setListenerPosition::alListenerfv:AL_DIRECTION"))
        return false;

    // TODO as parameters ?
    alListenerf(AL_MAX_DISTANCE, 10000.0f);
    alListenerf(AL_MIN_GAIN, 0.0f);
    alListenerf(AL_MAX_GAIN, 1.0f);
    alListenerf(AL_GAIN, 1.0f);

    return true;
}

/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::isOggExtensionPresent(void)
{
    if (alIsExtensionPresent("AL_EXT_vorbis") != AL_TRUE)
    {
        printf("ERROR: SoundManager::isOggExtensionPresent : Ogg Vorbis extension not   availablee!\n");
        bOggExtensionPresent = false;
        return false;
    }
    return true;
}

// Function to load a wave file and assigned it to a buffer
// This code was taken from the plugin found on this forum :
// http://www.ogre3d.org/phpBB2/viewtopic.php?t=7234
//
// TODO I didn't integrate it fow now .. because I don't need it now :-)
//
/****************************************************************************/

OPAL_SOUND_MGR bool SoundManager::loadOGGFile(std::string fileName, std::vector<char>& buffer,
    ALenum& format, ALsizei& freq)
{
#define BUFFER_SIZE     32768       // 32 KB buffers
    int endian = 0;                         // 0 for Little-Endian, 1 for Big-Endian
    int bitStream;
    long bytes;
    char array[BUFFER_SIZE];                // Local fixed size array
    FILE* f;

    // Open for binary reading
    f = fopen(fileName.c_str(), "rb");

    if (f == NULL)
    {
        std::cout << "Cannot open " << fileName << " for reading..." << std::endl;
        exit(-1);
    }


    vorbis_info* pInfo;
    OggVorbis_File oggFile;

    // Try opening the given file
    if (ov_open(f, &oggFile, NULL, 0) != 0)
    {
        std::cout << "Error opening " << fileName << " for decoding..." << std::endl;
        exit(-1);
    }


    // Get some information about the OGG file
    pInfo = ov_info(&oggFile, -1);

    // Check the number of channels... always use 16-bit samples
    if (pInfo->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;


    // The frequency of the sampling rate
    freq = pInfo->rate;

    // Keep reading until all is read
    do
    {
        // Read up to a buffer's worth of decoded sound data
        bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);

        if (bytes < 0)
        {
            ov_clear(&oggFile);
            std::cout << "Error decoding " << fileName << "..." << std::endl;
            exit(-1);
        }


        // Append to end of buffer
        buffer.insert(buffer.end(), array, array + bytes);
    } while (bytes > 0);

    // Clean up!
    ov_clear(&oggFile);
    return true;
}
OPAL_SOUND_MGR bool SoundManager::loadOGG(std::string filename, ALuint pDestAudioBuffer)
{
    ALenum       format;         //for the buffer format
    ALsizei      size;         //the bit depth
    ALsizei      freq;         //for the frequency of the buffer
    ALboolean    loop;         //looped

    std::vector<char> bufferData;                // The sound buffer data from file

    std::string mFullPath = mAudioPath;

    alGetError();   // Clear Error Code

    mFullPath += filename;


    loadOGGFile(mFullPath, bufferData, format, freq);


    alBufferData(pDestAudioBuffer, format, &bufferData[0], static_cast<ALsizei>(bufferData.size())
        , freq);




    return true;
}
/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::setGain(unsigned int audioID, float gain)
{
    if (audioID >= MAX_AUDIO_SOURCES || !mAudioSourceInUse[audioID])
        return false;

    alSourcef(mAudioSources[audioID], AL_GAIN, gain);
    return true;
}






/****************************************************************************/
OPAL_SOUND_MGR bool SoundManager::loadDefaultSounds(std::string filename)
{
    FILE* myfile;
    unsigned linecount = 0;
    char key[255], buff[512];

    if ((myfile = fopen(filename.c_str(), "r")) == NULL)
    {
        sprintf(buff, "---> Can't Open File: %s\n", filename.c_str());
        printf("SoundManager::loadDefaultSounds : %s\n", buff);
        return false;
    }
    fseek(myfile, 0L, SEEK_SET);    // Make sure we are at the begining of the file

    while (!feof(myfile))
    {
        fgets(buff, sizeof(buff), myfile);   // Read a line from the file.
        linecount++;

        if (strncmp(buff, "#", 1) &&        // Is this a comment line?
            strncmp(buff, "", 1) &&
            strncmp(buff, "/", 1))
        {
            // We have some data, attempt to load it
            strcpy(key, buff);
            trimTrailingSpace(key);

            // First, make sure it isn't already loaded
            if (locateAudioBuffer(key) < 0)
            {
                // Nope, its not already loaded
                if (loadAudioInToSystem(key) < 0)
                {
                    sprintf(buff, "Can't load audio file: %s\n", key);
                    printf("SoundManager::loadDefaultSounds() : %s\n", buff);
                }
            }
        }
    }

    // Were done
    fclose(myfile);
    return true;
}

// Function to trim the trailing crap from a string.
/****************************************************************************/
OPAL_SOUND_MGR void SoundManager::trimTrailingSpace(char* s)
{
    char* p;
    p = s;
    if (p == NULL) return;
    for (unsigned i = 0; i < (strlen(s) + 1); i++)
    {
        if (__iscsym(*p) == 0 && *p != '.' && *p != '-')
        {
            *p = '\0';
            break;
        }
        p++;
    }
}