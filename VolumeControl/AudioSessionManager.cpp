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
	shared_ptr<vector<ULONG>> keySet = make_shared<vector<ULONG>>(idMap->size());

	/// for each loop
	for(const auto& entry : *idMap) {
		keySet->insert(keySet->end(), entry.first);
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

	return make_shared<wstring>(L"System");
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
	
	/// release event handler and session
	auto sessionEventHandler = searchHandlerMap->second;
	auto session = searchIdMap->second;

	ASSERT(session && sessionEventHandler);

	if(session && sessionEventHandler) {
		session->UnregisterAudioSessionNotification(sessionEventHandler);

		/// remove from map
		idMap->erase(searchIdMap);
		eventHandlerMap->erase(searchHandlerMap);

		SAFERELEASE(sessionEventHandler);
		SAFERELEASE(session);

		

		/// notify listeners
		volController.getListenerNotifier().notifyOnSessionRemoved(id);
	}	
}

void AudioSessionManager::shutdown() const {
	if(sessionManager) {
		sessionManager->UnregisterSessionNotification((IAudioSessionNotification*) this);
	}

	SAFERELEASE(sessionEnum);
	SAFERELEASE(sessionManager);
}

ULONG AudioSessionManager::getNextID() {
	InterlockedIncrement(&currentID);

	return currentID;
}

void AudioSessionManager::init() {

	/// do something only on first call
	if (sessionManager && sessionEnum) {
		return;
	}

	/// release old handles
	SAFERELEASE(sessionManager);
	SAFERELEASE(sessionEnum);

	sessionManager = nullptr;
	sessionEnum = nullptr;

	/// Activate Device to get IAudioSessionManager2
	/// and IAudioSessionEnumerator
	const auto defaultDevice = volController.getAudioDeviceManager().getCurrentDefaultDevice();

	if(!defaultDevice) {
		DEBUG_PRINT("Failed to get Default device! Line " << __LINE__);

		return;
	}

	auto hr = defaultDevice->Activate(
		__uuidof(IAudioSessionManager2),
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&sessionManager)
	);

	/// release the handle again
	defaultDevice->Release();

	if (FAILED(hr) || !sessionManager) {
		DEBUG_PRINT("Failed to get IAudioSessionManager!");

		return;
	}

	hr = sessionManager->GetSessionEnumerator(
		&sessionEnum
	);

	if (FAILED(hr) || !sessionEnum) {
		DEBUG_PRINT("Failed to get IAudioSessionEnumerator!");

		return;
	}

	// register as listener
	sessionManager->RegisterSessionNotification(this);


	// add all current sessions
	auto sessionCount = 0;
	hr = sessionEnum->GetCount(&sessionCount);

	if (FAILED(hr)) {
		DEBUG_PRINT("Failed to get Session count!");

		return;
	}
		

	for (int i = 0; i < sessionCount; i++) {
		IAudioSessionControl* sessionControl = nullptr;

		hr = sessionEnum->GetSession(
			i,
			&sessionControl
		);

		if (FAILED(hr)) {
			DEBUG_PRINT("Failed to get session control!");
		}
		else
		{
			addSession(sessionControl);
		}
	}
}

// -------------------------
// --- Private Functions ---
// -------------------------

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

HRESULT AudioSessionManager::addSession(IAudioSessionControl* newSession) {
	IAudioSessionControl2* sessionControl = nullptr;

	const auto hr = newSession->QueryInterface(
		__uuidof(IAudioSessionControl2),
		reinterpret_cast<void**>(&sessionControl)
	);

	if (SUCCEEDED(hr) && sessionControl) {
		/// add ref
		sessionControl->AddRef();
		
		ULONG insertId = getNextID();

		const auto handler = new AudioSessionEventHandler(volController, insertId);

		/// insert into the id map
		idMap->insert(pair<ULONG, IAudioSessionControl2*>(insertId, sessionControl));

		/// register listener and insert it in the map
		sessionControl->RegisterAudioSessionNotification(handler);
		const auto insertPair = std::make_pair(insertId, handler);
		
		ASSERT(eventHandlerMap->insert(insertPair).second)
			
		/// notify the listeners
		volController.getListenerNotifier().notifyOnSessionAdded(insertId);	

		
	} else {
		DEBUG_PRINT("Error getting IAudioSessionControl2!");
	}

	return hr;
}

// --------------------------------------
// --- AudiSessionNotificationHandler ---
// ------------- Method -----------------

HRESULT AudioSessionManager::OnSessionCreated(IAudioSessionControl* NewSession) {
	DEBUG_PRINT("OnSessionCreated called in line " << __LINE__);
	
	return addSession(NewSession);
}



