#include "pch.h"
#include "AudioEngine.h"



    Implementation* sgpImplementation = nullptr;

    Implementation::Implementation() {
        mpStudioSystem = NULL;
        CAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
        CAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

        mpSystem = NULL;
        CAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
    }


    Implementation::~Implementation() {
        CAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
        CAudioEngine::ErrorCheck(mpStudioSystem->release());
    }


    void Implementation::Update() {
        std::vector<ChannelMap::iterator> pStoppedChannels;
        for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
        {
            bool bIsPlaying = false;
            it->second->isPlaying(&bIsPlaying);
            if (!bIsPlaying)
            {
                pStoppedChannels.push_back(it);
            }
        }
        for (auto& it : pStoppedChannels)
        {
            mChannels.erase(it);
        }
        CAudioEngine::ErrorCheck(mpStudioSystem->update());
    }


    void CAudioEngine::Init() {
        sgpImplementation = new Implementation;
    }

    void CAudioEngine::Update() {
        sgpImplementation->Update();
    }


    void CAudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
    {
        auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt != sgpImplementation->mSounds.end())
            return;

        FMOD_MODE eMode = FMOD_DEFAULT;
        eMode |= b3d ? FMOD_3D : FMOD_2D;
        eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

        FMOD::Sound* pSound = nullptr;
        CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
        if (pSound) {
            sgpImplementation->mSounds[strSoundName] = pSound;
        }

    }


    void CAudioEngine::UnLoadSound(const std::string& strSoundName)
    {
        auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
            return;

        CAudioEngine::ErrorCheck(tFoundIt->second->release());
        sgpImplementation->mSounds.erase(tFoundIt);
    }

    void CAudioEngine::Set3DListenerAndOrientationDoppler(int listener, Ogre::Vector3f listener_pos, Ogre::Vector3f listener_vel,
        Ogre::Vector3f listener_forward, Ogre::Vector3f listener_up)
    {
        const FMOD_VECTOR f_list_pos = VectorToFmod(listener_pos);
        const FMOD_VECTOR f_list_up = VectorToFmod(listener_up);
        const FMOD_VECTOR f_list_forw = VectorToFmod(listener_forward);
        const FMOD_VECTOR f_list_vel = VectorToFmod(listener_vel);

        CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(listener, &f_list_pos, &f_list_vel, &f_list_forw, &f_list_up));
    }

    void CAudioEngine::Set3DListenerAndOrientation(int listener, Ogre::Vector3f listener_pos,
        Ogre::Vector3f listener_forward, Ogre::Vector3f listener_up)
    {
        const FMOD_VECTOR f_list_pos = VectorToFmod(listener_pos);
        const FMOD_VECTOR f_list_up = VectorToFmod(listener_up);
        const FMOD_VECTOR f_list_forw = VectorToFmod(listener_forward);

        CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->set3DListenerAttributes(listener, &f_list_pos, NULL, &f_list_forw, &f_list_up));
    }

    int CAudioEngine::PlaySoundAtPosition(const std::string& strSoundName, const Ogre::Vector3f& vPosition, float fVolumedB)
    {
        int nChannelId = sgpImplementation->mnNextChannelId++;
        auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            LoadSound(strSoundName);
            tFoundIt = sgpImplementation->mSounds.find(strSoundName);
            if (tFoundIt == sgpImplementation->mSounds.end())
            {
                return nChannelId;
            }
        }
        FMOD::Channel* pChannel = nullptr;
        CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
        if (pChannel)
        {
            FMOD_MODE currMode;
            tFoundIt->second->getMode(&currMode);
            if (currMode & FMOD_3D) {
                FMOD_VECTOR position = VectorToFmod(vPosition);
                CAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
            }
            CAudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
            CAudioEngine::ErrorCheck(pChannel->setPaused(false));
            sgpImplementation->mChannels[nChannelId] = pChannel;
        }
        return nChannelId;
    }


    void CAudioEngine::SetChannel3dPosition(int nChannelId, const Ogre::Vector3f& vPosition)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundIt == sgpImplementation->mChannels.end())
            return;

        FMOD_VECTOR position = VectorToFmod(vPosition);
        CAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
    }

    void CAudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundIt == sgpImplementation->mChannels.end())
            return;

        CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
    }

    void CAudioEngine::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {

        auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
        if (tFoundIt != sgpImplementation->mBanks.end())
            return;
        FMOD::Studio::Bank* pBank;
        CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
        if (pBank) {
            sgpImplementation->mBanks[strBankName] = pBank;
        }
    }


    void CAudioEngine::LoadEvent(const std::string& strEventName) {
        auto tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit != sgpImplementation->mEvents.end())
            return;
        FMOD::Studio::EventDescription* pEventDescription = NULL;
        CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
        if (pEventDescription) {
            FMOD::Studio::EventInstance* pEventInstance = NULL;
            CAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
            if (pEventInstance) {
                sgpImplementation->mEvents[strEventName] = pEventInstance;
            }
        }
    }


    void CAudioEngine::PlayEvent(const std::string& strEventName) {
        auto tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit == sgpImplementation->mEvents.end()) {
            LoadEvent(strEventName);
            tFoundit = sgpImplementation->mEvents.find(strEventName);
            if (tFoundit == sgpImplementation->mEvents.end())
                return;
        }
        tFoundit->second->start();
    }


    void CAudioEngine::StopEvent(const std::string& strEventName, bool bImmediate) {
        auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
        if (tFoundIt == sgpImplementation->mEvents.end())
            return;

        FMOD_STUDIO_STOP_MODE eMode;
        eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
        CAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
    }


    bool CAudioEngine::IsEventPlaying(const std::string& strEventName) const {
        auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
        if (tFoundIt == sgpImplementation->mEvents.end())
            return false;

        FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
        if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
            return true;
        }
        return false;
    }


    void CAudioEngine::GetEventParameter(const std::string& strEventName, const std::string& strParameterName, float* parameter) {
        auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
        if (tFoundIt == sgpImplementation->mEvents.end())
            return;

        //float pParameter;
        CAudioEngine::ErrorCheck(tFoundIt->second->getParameterByName(strParameterName.c_str(), parameter));
        //CAudioEngine::ErrorCheck(pParameter->getValue(parameter));

    }

    void CAudioEngine::SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue) {
        auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
        if (tFoundIt == sgpImplementation->mEvents.end())
            return;

        //FMOD::Studio::ParameterInstance* pParameter = NULL;
        //CAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
        //CAudioEngine::ErrorCheck(pParameter->setValue(fValue));
        CAudioEngine::ErrorCheck(tFoundIt->second->setParameterByName(strEventName.c_str(), fValue));

    }

    FMOD_VECTOR CAudioEngine::VectorToFmod(const Ogre::Vector3f& vPosition) {
        FMOD_VECTOR fVec;
        fVec.x = vPosition.x;
        fVec.y = vPosition.y;
        fVec.z = vPosition.z;
        return fVec;
    }

    float  CAudioEngine::dbToVolume(float dB)
    {
        return powf(10.0f, 0.05f * dB);
    }

    float  CAudioEngine::VolumeTodB(float volume)
    {
        return 20.0f * log10f(volume);
    }

    int CAudioEngine::ErrorCheck(FMOD_RESULT result) {
        if (result != FMOD_OK) {
            std::cout << "FMOD ERROR " << result << std::endl;
            return 1;
        }
        // cout << "FMOD all good" << endl;
        return 0;
    }


    void CAudioEngine::Shutdown() {
        delete sgpImplementation;
    }



