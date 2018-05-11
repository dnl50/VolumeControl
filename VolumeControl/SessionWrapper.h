#pragma once
#include <memory>
#include "AudioSessionManager.h"
#include "AudioSessionEventHandler.h"
#include "Audiopolicy.h"

// forward declare
class AudioSessionManager;
class AudioSessionEventHandler;

class SessionWrapper {

private:
	const AudioSessionManager& sessionManager;

	const unsigned int sessionID;

	const std::shared_ptr<IAudioSessionControl2> sessionControl;

	const std::shared_ptr<AudioSessionEventHandler> eventHandler;

public:
	explicit SessionWrapper(AudioSessionManager& mngr, unsigned int id, std::shared_ptr<IAudioSessionControl2> sessionCtrl);
	
	~SessionWrapper();

	std::shared_ptr<AudioSessionEventHandler> getEventHandler() const;

	std::shared_ptr<IAudioSessionControl2> getSessionControl() const;

	const AudioSessionManager& getAudioSessionManager() const;

	unsigned int getSessionID() const;

};

