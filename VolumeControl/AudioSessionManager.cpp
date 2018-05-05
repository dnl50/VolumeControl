#include "stdafx.h"
#include "AudioSessionManager.h"

#include <iostream>
#include <Psapi.h>
#include <string>

#define BUFFER_STRING_SIZE 100U

using std::map;
using std::shared_ptr;
using std::vector;
using std::wstring;
using std::make_shared;

AudioSessionManager::AudioSessionManager(VolumeController& volController): 
	volController(volController),
	idMap(new map<UINT, IAudioSessionControl2*>), 
	eventHandlerMap(new map<UINT, AudioSessionEventHandler&>),
	currentID(0U)
{
	
}

AudioSessionManager::~AudioSessionManager() = default;

shared_ptr<vector<UINT>> AudioSessionManager::getSessionIDs() const {
	
	/// create new vector with predefined size
	shared_ptr<vector<UINT>> keySet(new vector<UINT>(idMap->size()));

	/// for each loop
	for(const auto& entry : *idMap) {
		keySet->push_back(entry.first);
	}

	return keySet;
}

shared_ptr<wstring> AudioSessionManager::getSessionNameByID(const UINT id) const {
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

IAudioSessionControl2* AudioSessionManager::getSessionByID(UINT id) const {

}

void AudioSessionManager::removeSessionByID(const UINT id) const {
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
	}
}

shared_ptr<wstring> AudioSessionManager::getProcessNameByPID(DWORD pid) const {
	auto returnStr = make_shared<wstring>();
	
	const auto processName = static_cast<LPTSTR>(malloc(BUFFER_STRING_SIZE));

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
			BUFFER_STRING_SIZE
		);

		/// if GetModuleBaseName returns 0 a error occurred
		/// print it
		if(!copiedChars) {
			const auto errorCode = GetLastError();
			DEBUG_PRINT("Error retrieving the Process name of process with PID " << pid << ". Error Code: " << std::hex << HRESULT_FROM_WIN32(errorCode));

			returnStr->append(L"System");
		} else {
			returnStr->append(processName, copiedChars);
		}		

	} else {
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
