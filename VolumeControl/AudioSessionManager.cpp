#include "stdafx.h"
#include "AudioSessionManager.h"

#include <iostream>
#include <Psapi.h>
#include <string>

#define STRING_BUFFER_SIZE 100U

using std::map;
using std::shared_ptr;
using std::vector;
using std::wstring;
using std::make_shared;
using std::pair;

AudioSessionManager::AudioSessionManager(VolumeController& volController): 
	volController(volController),
	idMap(make_shared<map<ULONG, IAudioSessionControl2*>>()),
	eventHandlerMap(make_shared<map<ULONG, AudioSessionEventHandler*>>()),
	currentID(0UL),
	sessionEnum(nullptr),
	sessionManager(nullptr) {
	
}

AudioSessionManager::~AudioSessionManager() = default;

shared_ptr<vector<ULONG>> AudioSessionManager::getSessionIDs() const {
	
	/// create new vector with predefined size
	shared_ptr<vector<ULONG>> keySet(new vector<ULONG>(idMap->size()));

	/// for each loop
	for(const auto& entry : *idMap) {
		keySet->push_back(entry.first);
	}

	return keySet;
}

shared_ptr<wstring> AudioSessionManager::getSessionNameByID(const ULONG id) const {
	const auto find = idMap->find(id);
	
	if(find != idMap->end()) {
		
		const auto audioSessionControl = find->second;

		ASSERT(audioSessionControl);

		DWORD pid;

		const auto hr = audioSessionControl->GetProcessId(&pid);

		if(SUCCEEDED(hr)) {
			return getProcessNameByPID(pid);
		}

		DEBUG_ERR_PRINT("Failed to get PID from audio session! Err code:" << hr);
	}

	return nullptr;
}

IAudioSessionControl2* AudioSessionManager::getSessionByID(const ULONG id) const {
	const auto searchIdMap = idMap->find(id);

	if(searchIdMap == idMap->end()) {
		return nullptr;
	}

	return searchIdMap->second;
}

void AudioSessionManager::removeSessionByID(const ULONG id) const {
	const auto searchIdMap = idMap->find(id);
	const auto searchHandlerMap = eventHandlerMap->find(id);

	/// found in map 1 gdw. found in map 2
	ASSERT((searchIdMap == idMap->end()) == (searchHandlerMap == eventHandlerMap->end()));

	if(searchIdMap == idMap->end() && searchHandlerMap == eventHandlerMap->end()) {
		return;
	}

	/// remove from map
	idMap->erase(searchIdMap);

	/// release event handler and session
	const auto sessionEventHandler = searchHandlerMap->second;
	const auto session = searchIdMap->second;

	ASSERT(session && sessionEventHandler);

	if(session && sessionEventHandler) {
		session->UnregisterAudioSessionNotification(sessionEventHandler);

		SAFERELEASE(sessionEventHandler);
		SAFERELEASE(session);

		volController.getListenerNotifier().notifyOnSessionRemoved(id);
	}
}

void AudioSessionManager::shutdown() const {
	volController.getListenerNotifier().unregisterListener(make_shared<Listener>(*this));

	SAFERELEASE(sessionEnum);
	SAFERELEASE(sessionManager);
}

ULONG AudioSessionManager::getNextID() {
	InterlockedIncrement(&currentID);

	return currentID;
}

/// -------------------------
/// --- Private Functions ---
/// -------------------------

shared_ptr<wstring> AudioSessionManager::getProcessNameByPID(DWORD pid) const {
	auto returnStr = make_shared<wstring>();

	const auto processName = static_cast<LPTSTR>(malloc(STRING_BUFFER_SIZE));

	const auto handle = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		pid
	);

	if (handle) {

		/// try to get the process name
		const auto copiedChars = GetModuleBaseName(
			handle,
			nullptr,
			processName,
			STRING_BUFFER_SIZE
		);

		/// if GetModuleBaseName returns 0 a error occurred
		/// print it
		if (!copiedChars) {
			const auto errorCode = GetLastError();
			DEBUG_PRINT("Error retrieving the Process name of process with PID " << pid << ". Error Code: " << std::hex << HRESULT_FROM_WIN32(errorCode));

			returnStr->append(L"System");
		}
		else {
			returnStr->append(processName, copiedChars);
		}

	}
	else {
		DEBUG_ERR_PRINT("Error getting ProcessHandle");

		returnStr->append(L"System");
	}

	CloseHandle(handle);

	free(processName);

	/// cut the .exe extension
	const auto fileExtensionIndex = returnStr->find_last_of('.');

	if (fileExtensionIndex < wstring::npos) {
		const auto str = returnStr->substr(0, fileExtensionIndex);

		returnStr->clear();
		returnStr->append(str);
	}

	return returnStr;
}

void AudioSessionManager::resetSessionDataAndNotify() {
	removeAllSessions();

	auto sessionCount = 0;
	auto hr = sessionEnum->GetCount(&sessionCount);

	if(FAILED(hr)) {
		DEBUG_PRINT("Failed to get Session count!");

		return;
	}

	for(int i = 0; i < sessionCount; i++) {
		IAudioSessionControl* sessionControl = nullptr;

		hr = sessionEnum->GetSession(
			i,
			&sessionControl
		);

		if(FAILED(hr)) {
			DEBUG_PRINT("Failed to get session control!");
		} else
		{
			addSession(sessionControl);
		}
	}
}

void AudioSessionManager::removeAllSessions() const {
	for (const auto id : *getSessionIDs()) {
		removeSessionByID(id);
	}
}

HRESULT AudioSessionManager::addSession(IAudioSessionControl* newSession) {
	IAudioSessionControl2* sessionControl = nullptr;

	const auto hr = newSession->QueryInterface(
		__uuidof(IAudioSessionControl2),
		reinterpret_cast<void**>(&sessionControl)
	);

	if (SUCCEEDED(hr) && sessionControl) {
		ULONG insertId = getNextID();

		const auto handler = new AudioSessionEventHandler(volController, insertId);

		/// insert into the id map
		idMap->insert(pair<ULONG, IAudioSessionControl2*>(insertId, sessionControl));

		/// register listener and insert it in the map
		sessionControl->RegisterAudioSessionNotification(handler);
		eventHandlerMap->insert(pair<ULONG, AudioSessionEventHandler*>(insertId, handler));

		/// notify the listeners
		volController.getListenerNotifier().notifyOnSessionCreated(insertId);
	} else {
		DEBUG_PRINT("Error getting IAudioSessionControl2!");
	}

	return hr;
}

// ----------------------------------
// --- Listener Interface Methods ---
// ----------------------------------

void AudioSessionManager::OnDefaultDeviceChanged() {
	DEBUG_PRINT("LALALALALAL");

	/// release old handles
	SAFERELEASE(sessionManager);
	SAFERELEASE(sessionEnum);

	sessionManager = nullptr;
	sessionEnum = nullptr;

	/// Activate Device to get IAudioSessionManager2
	/// and IAudioSessionEnumerator

	const auto defaultDevice = volController.getAudioDeviceManager().getCurrentDefaultDevice();

	auto hr = defaultDevice->Activate(
		__uuidof(IAudioSessionManager2),
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&sessionManager)
	);

	if(FAILED(hr) || !sessionManager) {
		DEBUG_PRINT("Failed to get IAudioSessionManager!");

		return;
	}

	hr = sessionManager->GetSessionEnumerator(
		&sessionEnum
	);

	if(FAILED(hr) || !sessionEnum) {
		DEBUG_PRINT("Failed to get IAudioSessionEnumerator!");

		return;
	}

	resetSessionDataAndNotify();
}

/// --------------------------------------
/// --- AudiSessionNotificationHandler ---
/// ------------- Method -----------------

HRESULT AudioSessionManager::OnSessionCreated(IAudioSessionControl* NewSession) {
	return addSession(NewSession);
}



