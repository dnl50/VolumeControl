#include "stdafx.h"
#include "AudioSessionManager.h"

#include <iostream>
#include <psapi.h>
#include <string>

#define STRING_BUFFER_SIZE 100U

#define SESSION_ENUM_DELETER [](IAudioSessionEnumerator* sessEnum) { \
	SAFERELEASE(sessEnum); \
}

#define SESSION_MANAGER_DELETER [this](IAudioSessionManager2* mngr) { \
	if (mngr) { \
		mngr->UnregisterSessionNotification(this); \
	} \
	SAFERELEASE(mngr); \
}

#define SESSION_CONTROL_DELETER [](IAudioSessionControl2* sessionCtrl) { \
	SAFERELEASE(sessionCtrl); \
}

using std::map;
using std::shared_ptr;
using std::vector;
using std::wstring;
using std::make_shared;
using std::pair;
using std::make_pair;

AudioSessionManager::AudioSessionManager(VolumeController& volController) : 
	volController(volController),	
	wrapperMap(make_shared<map<ULONG, shared_ptr<SessionWrapper>>>()),
	sessionManager(shared_ptr<IAudioSessionManager2>(nullptr, SESSION_MANAGER_DELETER)),
	sessionEnum(shared_ptr<IAudioSessionEnumerator>(nullptr, SESSION_ENUM_DELETER)),
	currentID(0UL)
{
	
}

AudioSessionManager::~AudioSessionManager() = default;

shared_ptr<vector<ULONG>> AudioSessionManager::getSessionIDs() const {
	
	/// create new vector with predefined size
	auto keySet = make_shared<vector<ULONG>>(wrapperMap->size());

	/// for each loop
	for(const auto& entry : *wrapperMap) {
		keySet->insert(keySet->end(), entry.first);
	}

	return keySet;
}

shared_ptr<wstring> AudioSessionManager::getSessionNameByID(const ULONG id) const {
	const auto find = wrapperMap->find(id);

	if (find == wrapperMap->end()) {
		return make_shared<wstring>(L"System");;
	}

	const auto wrapper = find->second;

	/// --- try to get the session name via the session control ---
	LPWSTR displayName = nullptr;
		
	auto hr = wrapper->getSessionControl()->GetDisplayName(&displayName);

	/// if the display name was retrieved successfully and has at least 1
	/// char return it
	if(SUCCEEDED(hr) && displayName && wcslen(displayName)) {
		auto str = make_shared<wstring>(displayName);
		
		/// free the memory
		CoTaskMemFree(displayName);
			
		return str;
	}

	/// free the memory
	CoTaskMemFree(displayName);		



	/// --- retrieve it via the PID ---
	DWORD pid;
	
	hr = wrapper->getSessionControl()->GetProcessId(&pid);

	if(SUCCEEDED(hr)) {
		return getProcessNameByPID(pid);
	}

	DEBUG_ERR_PRINT("Failed to get PID from audio session! Err code: " << std::hex << hr);

	return make_shared<wstring>(L"System");
}

shared_ptr<IAudioSessionControl2> AudioSessionManager::getSessionByID(const ULONG id) const {
	const auto searchIdMap = wrapperMap->find(id);

	if (searchIdMap == wrapperMap ->end()) {
		return nullptr;
	}

	return searchIdMap->second->getSessionControl();
}

void AudioSessionManager::removeSessionByID(const ULONG id) const {
	const auto searchWrapperMap = wrapperMap->find(id);

	if(searchWrapperMap == wrapperMap->end()) {
		return;
	}

	/// remove from map
	wrapperMap->erase(searchWrapperMap);
				
	/// notify listeners
	volController.getListenerNotifier().notifyOnSessionRemoved(id);		
}

const VolumeController& AudioSessionManager::getVolController() const {
	return volController;
}

ULONG AudioSessionManager::getNextID() {
	InterlockedIncrement(&currentID);

	return currentID;
}

HRESULT AudioSessionManager::init() {

	/// Activate Device to get IAudioSessionManager2
	/// and IAudioSessionEnumerator
	const auto defaultDevice = volController.getAudioDeviceManager().getCurrentDefaultDevice();

	if(!defaultDevice || !defaultDevice.get()) {
		DEBUG_PRINT("Failed to get Default device! Line " << __LINE__);

		return E_POINTER;
	}

	IAudioSessionManager2* mngrPtr = nullptr;

	auto hr = defaultDevice->Activate(
		__uuidof(IAudioSessionManager2),
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&mngrPtr)
	);

	if (FAILED(hr) || !mngrPtr) {
		DEBUG_PRINT("Failed to get IAudioSessionManager!");

		return hr;
	}

	sessionManager = shared_ptr<IAudioSessionManager2>(mngrPtr, SESSION_MANAGER_DELETER);

	/// get session enum
	IAudioSessionEnumerator* enumPtr = nullptr;

	hr = sessionManager->GetSessionEnumerator(
		&enumPtr
	);

	if (FAILED(hr) || !enumPtr) {
		DEBUG_PRINT("Failed to get IAudioSessionEnumerator!");

		return hr;
	}

	sessionEnum = shared_ptr<IAudioSessionEnumerator>(enumPtr, SESSION_ENUM_DELETER);

	// register as listener
	sessionManager->RegisterSessionNotification(this);

	// add all current sessions
	auto sessionCount = 0;
	hr = sessionEnum->GetCount(&sessionCount);

	if (FAILED(hr)) {
		DEBUG_PRINT("Failed to get Session count!");

		return hr;
	}		

	for (auto i = 0; i < sessionCount; i++) {
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

	return hr;
}

// -------------------------
// --- Private Functions ---
// -------------------------

shared_ptr<wstring> AudioSessionManager::getProcessNameByPID(DWORD pid) const {
	auto returnStr = make_shared<wstring>();

	const auto processName = new wchar_t[STRING_BUFFER_SIZE];

	const auto handle = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		false,
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

		CloseHandle(handle);
	}
	else {
		DEBUG_ERR_PRINT("Error getting ProcessHandle! Error: " << GetLastError());

		returnStr->append(L"System");
	}

	delete[] processName;

	/// cut the .exe extension
	/*const auto fileExtensionIndex = returnStr->find_last_of('.');

	if (fileExtensionIndex < wstring::npos) {
		const auto str = returnStr->substr(0, fileExtensionIndex);

		returnStr->clear();
		returnStr->append(str);
	}*/

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

		/// create shared pointer that releases the handle when going out of scope
		auto ptr = shared_ptr<IAudioSessionControl2>(sessionControl, SESSION_CONTROL_DELETER);

		/// create new session wrapper
		const auto wrapper = make_shared<SessionWrapper>(*this, insertId, ptr);	

		/// insert it into the map
		ASSERT(wrapperMap->insert(make_pair(insertId, wrapper)).second);		
			
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
	return addSession(NewSession);
}



