/**
 *  libOpenAl.h
 *
 *  Original Code : Van Stokes, Jr. (http://www.EvilMasterMindsInc.com) - Aug 05
 *  Modified Code : Steven Gay - mec3@bluewin.ch - Septembre 2005 - Jan 06
 *  Dll version   : Griffo Jean-Baptiste , crashy@antharia.org - Jan 06
 *
 *                    Partial Documentation
 *                    =====================
 *
 *  Very simple SoundManager using OpenAl.
 *
 *  For a more complete one, you should see :
 *  OpenAL++ http://alpp.sourceforge.net/
 *
 ****************************************************************************
 *  COMPILE
 *
 *  - Don't forget to link to : ALut.lib and OpenAL32.lib.
 *    The order is important.
 *
 *  - With CodeBlocks there is one warning I didn't resolve :
 *     "Warning: .drectve `/DEFAULTLIB:"uuid.lib" /DEFAULTLIB:"uuid.lib" ' unrecognized"
 *
 ****************************************************************************
 *  USAGE
 *
 * 1. Create the object from the class with createManager()
 *
 * 2. Call the init() function
 *
 * 3. Call the loadDefaultSounds() function to PRE-LOAD audio into the buffers.
 *    This is optional. Review the function to make changes that you need.
 *
 * 4. Set the Listener Location by calling setListenerPosition() function
 *    continually call this as your Listener (camera) position changes!
 *
 * 5. For each object that emits sound, call the loadSound() function.
 *    CAREFUL : The filename must be unique.
 *
 * 6. Optional : For each object you can set the all the parameters of the
 *    sound with setSound() or only the position, velocity and direction with
 *    setSoundPosition().
 *
 * 7. Call the playAudioSource() to play the sound at some event.
 *    This function will play the sound and then stop. It will NOT repeat playing.
 *    Use stopAudioSource() to stop a sound from playing if its still playing
 *
 * 8. Call pauseAudio() or pauseAllAudio() to pause one or all sound(s).
 *    Call resumeAudio() or resumeAllAudio() to resume one or all paused sound(s).
 *
 * 9. When your object is done emitting sounds (when out of range for example)
 *    call releaseAudioSource().
 *    It is important to release your source when you are no longer going to
 *    need it because you are limited in the number of sources you can have.
 *
 * 10. If your objects moves (other than the listener/camera) then
 *    continually update the objects position by calling setSourcePosition().
 *
 ******************************************************************************
 *
 * Additional informations :
 *
 * Ogg Vorbis - http://www.xiph.org/downloads/
 *            - http://www.illiminable.com/ogg/
 * Flac       - http://flac.sourceforge.net/features.html
 * Theora     - http://www.theora.org/
 *
 *
 ******************************************************************************
 *
 * TODO
 *
 * loadOGG()
 * alSourcePause()
 *
 * Use the EAX functions !
 *
 ******************************************************************************/


#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__



#if  defined(USE_OPAL_SOUND_MGR)
#if !defined(__MINGW32__) && !defined(__CYGWIN__)
#define OPAL_SOUND_MGR __declspec(dllimport)
#else
#define OPAL_SOUND_MGR
#endif
#elif defined(CMP_OPAL_SOUND_MGR)
#define OPAL_SOUND_MGR __declspec(dllexport)
#else
#define OPAL_SOUND_MGR
#endif 

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <stdio.h>
#include <string>

 // Comment this line if you don't have the EAX 2.0 SDK installed
 //#define _USEEAX

#ifdef _USEEAX
#include "eax.h"
#endif

// OpenAl version 1.0
// #include <al/alctypes.h>
// #include <al/altypes.h>
// #include <al/alc.h>
// #include <al/al.h>
// #include <alut/alut.h>
// OpenAl version 1.1
#include <al.h>
#include <alc.h>
#include <alut.h>

#include "vorbis/codec.h"

#include "vorbis/vorbisfile.h"


#include "OgreVector3.h" 
#include "OgreQuaternion.h" 
using namespace Ogre;




// Used to store sound filenames
#define MAX_FILENAME_LENGTH 40

class OPAL_SOUND_MGR SoundManager
{
private:
    // EAX related
    bool isEAXPresent;
#ifdef _USEEAX
    // EAX 2.0 GUIDs
    const GUID DSPROPSETID_EAX20_ListenerProperties
        = { 0x306a6a8, 0xb224, 0x11d2, { 0x99, 0xe5, 0x0, 0x0, 0xe8, 0xd8, 0xc7, 0x22 } };

    const GUID DSPROPSETID_EAX20_BufferProperties
        = { 0x306a6a7, 0xb224, 0x11d2, {0x99, 0xe5, 0x0, 0x0, 0xe8, 0xd8, 0xc7, 0x22 } };

    EAXSet eaxSet; // EAXSet function, retrieved if EAX Extension is supported
    EAXGet eaxGet; // EAXGet function, retrieved if EAX Extension is supported
#endif // _USEEAX

    bool isInitialised;
    ALCdevice* mSoundDevice;
    ALCcontext* mSoundContext;

    std::string mAudioPath;

    bool isSoundOn;

    ALfloat position[3];
    ALfloat velocity[3];
    ALfloat orientation[6];

    // Needed because of hardware limitation
    // Audio sources
    unsigned const short MAX_AUDIO_SOURCES;
    unsigned int mAudioSourcesInUseCount;
    unsigned int* mAudioSources;
    bool* mAudioSourceInUse;

    // Audio buffers
    unsigned const short int MAX_AUDIO_BUFFERS;
    unsigned int mAudioBuffersInUseCount;
    unsigned int* mAudioBuffers;
    bool* mAudioBufferInUse;
    std::map<int, std::string, std::less<int> > mAudioBufferFileName;

    // Function to check if the soundFile is already loaded into a buffer
    int locateAudioBuffer(std::string filename);
    int loadAudioInToSystem(std::string filename);
    bool loadWAV(std::string filename, ALuint pDestAudioBuffer);
    bool loadOGG(std::string filename, ALuint pDestAudioBuffer);
    bool loadOGGFile(std::string fileName, std::vector<char>& buffer,
        ALenum& format, ALsizei& freq);
    // TODO bool loadAU( std::string filename, ALuint pDestAudioBuffer );

    // Ogg Vorbis extension
    bool bOggExtensionPresent;

public:
    static SoundManager* mSoundManager;
    // SoundManager(void);
    SoundManager(int max_buffers, int max_sources);
    virtual ~SoundManager(void);
    void selfDestruct(void);
    static SoundManager* createManager(void);
    static SoundManager* createManager(int, int);
    static SoundManager* getSingletonPtr(void) { return mSoundManager; };

    bool init(void);
    bool getIsSoundOn(void) { return isSoundOn; };
    void setAudioPath(char* path) { mAudioPath = std::string(path); };

    bool checkALError(void);
    bool checkALError(std::string pMsg);

    /** See http://www.openal.org/windows_enumeration.html for installing other
    *   devices. You should at least have "Generic Hardware".
    */
    std::string listAvailableDevices(void);

    // careful : testSound should not be used it doesn't use the resource limitation
    void testSound(const char* wavFile);

    // Aquire an Audio Source
    // filename = pass in the sound file to play for this source (ex. "myfile.wav")
    // audioId   = returns the AudioSource identifier you will need for the PlayAudioSource();
    bool loadAudio(std::string filename, unsigned int* audioId, bool loop);
    bool releaseAudio(unsigned int audioID);

    // deprecated
    bool aquireAudioSource(char* file, unsigned int* audioId)
    {
        return loadAudio(std::string(file), audioId, false);
    };

    // Returns true if the audio is started from the beginning
    // false if error or if already playing
    bool playAudio(unsigned int audioId, bool forceRestart);
    bool stopAudio(unsigned int audioID);
    bool stopAllAudio(void);

    bool pauseAudio(unsigned int audioID);
    bool pauseAllAudio(void);
    bool resumeAudio(unsigned int audioID);
    bool resumeAllAudio(void);

    bool setSoundPosition(unsigned int audioID, Vector3 position);

    bool setSoundPosition(unsigned int audioID, Vector3 position,
        Vector3 velocity, Vector3 direction);

    bool setSound(unsigned int audioID, Vector3 position,
        Vector3 velocity, Vector3 direction, float maxDistance,
        bool playNow, bool forceRestart, float minGain);

    bool setListenerPosition(Vector3 position, Vector3 velocity,
        Quaternion orientation);

    bool isOggExtensionPresent(void);

    bool setGain(unsigned int, float gain);

    /**
     * Preload audio files into the system.
     * Not obligatory, the files can also be loaded on the fly.
     */
    bool loadDefaultSounds(std::string filename);
    // Function to trim the trailing crap from a string.
    void trimTrailingSpace(char* s);

};

#endif /*__SOUNDMANAGER_H__*/