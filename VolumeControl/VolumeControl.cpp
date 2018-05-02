// VolumeControl.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

// Audio Stuff
#include <windows.h>
#include <Mmdeviceapi.h>
#include <comdef.h>
#include <Audioclient.h>
#include <iostream>
#include <assert.h>
#include <Psapi.h>
#include <Endpointvolume.h>
#include <Audiopolicy.h>
#include "DeviceEnumNotificationHandler.h"


// DEBUG ONLY
#include <chrono>
#include <thread>
#include <Psapi.h>
#include "AudioSessionEventHandler.h"

// Other Preprocessor directives
#define SAFERELEASE(ptr) \
	if(ptr) { \
		ptr->Release(); \
	} else { }

#define CHECK_HR(hr) \
	if(FAILED(hr)) { \
		cout << "Error in Line " << __LINE__ << ". Error code: " << hex << hr << endl; \
		goto exit; \
	} else { }

// Function declarations
void init(void); // init the use of COM interfaces
void uninit(void); // uninit the use of COM interfaces


HRESULT getDeviceEnum(OUT IMMDeviceEnumerator** pDeviceEnum);
void getValue(IAudioSessionControl** param);

// using directives


using std::cout;
using std::endl;
using std::hex;
using std::string;

int main() {	
	init();
	
	IAudioSessionControl* audioSessionControl = nullptr;
	getValue(&audioSessionControl);

	uninit();
}


void getValue(IAudioSessionControl** param) {
	
	IMMDeviceEnumerator* pDeviceEnum = nullptr;
	IMMDevice* pDefaultDevice = nullptr;
	IAudioSessionManager* audioSessionMgr = nullptr;
	IAudioSessionManager2* audioSessionMgr2 = nullptr;
	IAudioSessionEnumerator* audioSessionEnum = nullptr;
	IAudioSessionControl* audioSessionControl = nullptr;
	IAudioSessionControl2* audioSessionControl2 = nullptr;
	ISimpleAudioVolume* simpleAudioVol = nullptr;

	LPWSTR name;
	
	DWORD pid = 0UL;
	auto sessionCount = 0;
	LPTSTR processName = static_cast<LPTSTR>(malloc(200));

	DeviceEnumNotificationHandler* deviceHandler = new DeviceEnumNotificationHandler();

	// -----------------------
	// --- Get Device Enum ---
	// -----------------------
	CHECK_HR(getDeviceEnum(&pDeviceEnum));

	assert(pDeviceEnum);


	/// --- Register Device Handler ---
	pDeviceEnum->RegisterEndpointNotificationCallback(deviceHandler);

	// --------------------------
	// --- Get default device ---
	// --------------------------
	CHECK_HR(pDeviceEnum->GetDefaultAudioEndpoint(
		EDataFlow::eRender,
		ERole::eMultimedia,
		&pDefaultDevice
	));	
		
	assert(pDefaultDevice);		

	/// --- Get IAudioSessionManager2 ---

	CHECK_HR(pDefaultDevice->Activate(
		__uuidof(IAudioSessionManager2),
		CLSCTX_ALL,
		nullptr,
		reinterpret_cast<void**>(&audioSessionMgr2)
	));

	assert(audioSessionMgr2);

	/// --- Get IAudioSessionEnumerator ---
	CHECK_HR(audioSessionMgr2->GetSessionEnumerator(
		&audioSessionEnum
	));

	assert(audioSessionEnum);

	/// --- Get a IAudioSessionControl Object ---	
	CHECK_HR(audioSessionEnum->GetCount(&sessionCount));	

	for (int i = 0; i < sessionCount; i++) {
		/// --- Get IAudioSessionControl ---
		CHECK_HR(audioSessionEnum->GetSession(i, &audioSessionControl));

		assert(audioSessionControl);
				
		/// --- Get IAudioSessionControl2 ---
		CHECK_HR(audioSessionControl->QueryInterface(
			_uuidof(IAudioSessionControl2),
			reinterpret_cast<void**>(&audioSessionControl2)
		));

		assert(audioSessionControl2);

		/// --- Get ISimpleAudioVolume ---
		CHECK_HR(audioSessionControl->QueryInterface(
			_uuidof(ISimpleAudioVolume),
			reinterpret_cast<void**>(&simpleAudioVol)
		));

		assert(simpleAudioVol);


		/// --- Get the session PID name ---
		CHECK_HR(audioSessionControl2->GetProcessId(&pid));
				
		cout << "SessionID: " << name << endl;

		HANDLE handle = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE,
			pid
		);


		if (handle) {

			GetModuleBaseName(
				handle,
				nullptr,
				processName,
				200
			);

			std::wcout << processName << endl;

		}
		else
		{
			cout << "Error getting Handle!" << endl;
		}

		CloseHandle(handle);


		if(i == 100)
		{
			AudioSessionEventHandler* eventHandler = new AudioSessionEventHandler();

			audioSessionControl2->RegisterAudioSessionNotification(eventHandler);


			std::this_thread::sleep_for(std::chrono::seconds(50));


			audioSessionControl2->UnregisterAudioSessionNotification(eventHandler);

			delete eventHandler;
		}	


	}
	
exit:

	pDeviceEnum->UnregisterEndpointNotificationCallback(deviceHandler);
	delete deviceHandler;

	SAFERELEASE(pDeviceEnum);
	SAFERELEASE(pDefaultDevice);
	SAFERELEASE(audioSessionMgr);
	SAFERELEASE(audioSessionMgr2);
	SAFERELEASE(audioSessionControl);
	SAFERELEASE(audioSessionControl2);
	free(processName);
	
}

void init(void) {
	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(hr)) {
		cout << "Failed to init COM library. Error code 0x" << hex << hr << endl;
	}

	hr = CoInitializeSecurity(
		nullptr,                        // Security descriptor    
		-1,                          // COM negotiates authentication service
		nullptr,                        // Authentication services
		nullptr,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication level for proxies
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
		nullptr,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities of the client or server
		nullptr);

	if (FAILED(hr)) {
		cout << "Failed to initialize security. Error code = 0x" << hex << hr << endl;

		uninit();
	}
}

void uninit(void) {
	CoUninitialize();
}

HRESULT getDeviceEnum(_Out_ IMMDeviceEnumerator** pDeviceEnum) {

	return CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		nullptr,
		CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		reinterpret_cast<void**>(pDeviceEnum)
	);
	

}