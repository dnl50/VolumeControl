#pragma once
#include <map>
#include <Audiopolicy.h>
#include "AudioSessionEventHandler.h"
#include "VolumeController.h"
#include <memory>
#include <vector>
#include "AudioSessionNotificationHandler.h"

/// forward declare
class VolumeController;
class AudioSessionEventHandler;

// Manages the active audio sessions of the current default audio rendering device.
class AudioSessionManager : public AudioSessionNotificationHandler {

private:

	// reference to the main controller
	const VolumeController& volController;
	
	// maps to map a ID to a session / the corresponding event handler
	const std::shared_ptr<std::map<ULONG, IAudioSessionControl2*>> idMap;
	const std::shared_ptr<std::map<ULONG,AudioSessionEventHandler*>> eventHandlerMap;
	
	// pointer to the IAudioSessionManager2 interface for the current
	// default rendering device
	IAudioSessionManager2* sessionManager;
	
	// pointer to the IAudioSessionEnumerator interface for the current
	// default rendering device
	IAudioSessionEnumerator* sessionEnum;
	
	// stores the last id used in the maps as the key
	ULONG currentID;
	
	// returns a unused ID as the key for the maps
	ULONG getNextID();

	// --- Private Functions ---

	// returns a smart pointer to a string containing the process name
	// throws a exception when it's not able to retrieve the name
	std::shared_ptr<std::wstring> getProcessNameByPID(DWORD pid) const;
		
	// add a new session and notify listeners
	HRESULT addSession(IAudioSessionControl* newSession);

public:
	
	// constructor
	explicit AudioSessionManager(VolumeController& volController);
	
	// destructor
	virtual ~AudioSessionManager();

	// returns a list of all currently used IDs for the active
	// audio sessions
	std::shared_ptr<std::vector<ULONG>> getSessionIDs() const;

	// returns the name of the audio session. the string is
	// empty if it was not possible to retrieve the name
	std::shared_ptr<std::wstring> getSessionNameByID(const ULONG id) const;

	// returns the object to control the volume for a session
	IAudioSessionControl2* getSessionByID(ULONG id) const;

	// removes a audio session from all maps and releases the 
	// corresponding interface handlers
	void removeSessionByID(ULONG id) const;
	
	// get the IAudioSessionManager2 and the IAudioSessionEnumerator
	void init();
	
	// release all references
	void shutdown(void) const;
	
	/// ---------------------------------------
	/// --- AudioSessionNotificationHandler ---
	/// -------------- Methods ----------------
	
	// gets called when a new session gets created
	HRESULT OnSessionCreated(IAudioSessionControl* NewSession) override;

};

