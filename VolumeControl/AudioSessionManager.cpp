#include "stdafx.h"
#include "AudioSessionManager.h"

#include <iostream>

using std::map;
using std::shared_ptr;
using std::vector;
using std::string;
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

shared_ptr<string> AudioSessionManager::getSessionNameByID(UINT id) const {
	const auto find = idMap->find(id);
	
	if(find != idMap->end()) {
		
		IAudioSessionControl2* audioSessionControl = find->second;

		ASSERT(audioSessionControl);

		DWORD pid;

		const auto hr = audioSessionControl->GetProcessId(&pid);

		if(SUCCEEDED(hr)) {
			return make_shared<string>("");


		}

		DEBUG_ERR_PRINT("Failed to get PID from audio session! Err code:" << hr);
	}

	return nullptr;
}

