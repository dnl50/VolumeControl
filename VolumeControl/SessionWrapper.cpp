#include "stdafx.h"
#include "SessionWrapper.h"

using std::shared_ptr;

SessionWrapper::SessionWrapper(AudioSessionManager& mngr, unsigned int id, shared_ptr<IAudioSessionControl2> sessionCtrl) :
	sessionManager(mngr),
	sessionID(id),
	sessionControl(sessionCtrl),
	eventHandler(new AudioSessionEventHandler(*this))
{
	sessionControl->RegisterAudioSessionNotification(eventHandler.get());
}


SessionWrapper::~SessionWrapper() {
	sessionControl->UnregisterAudioSessionNotification(eventHandler.get());
};

unsigned int SessionWrapper::getSessionID() const {
	return sessionID;
}

shared_ptr<IAudioSessionControl2> SessionWrapper::getSessionControl() const {
	return sessionControl;
}

const AudioSessionManager& SessionWrapper::getAudioSessionManager() const {
	return sessionManager;
}

shared_ptr<AudioSessionEventHandler> SessionWrapper::getEventHandler() const {
	return eventHandler;
}

