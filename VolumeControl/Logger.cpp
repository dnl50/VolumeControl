#include "stdafx.h"
#include "Logger.h"
#include <iostream>


using std::cout;
using std::wcout;
using std::endl;

Logger::Logger(VolumeController& volCon) : volController(volCon) {
	
}

Logger::~Logger() = default;

void Logger::OnDefaultDeviceChanged() {
	cout << "Default Audio Endpoint Device changed!" << endl;
}

void Logger::OnSessionCreated(unsigned id) {
	cout << "Session created! ID: " << id << endl;;
}

void Logger::OnSessionRemoved(unsigned id) {
	cout << "Session removed! ID: " << id << endl;
}

void Logger::OnVolumeChanged(unsigned id, const float newVolume, const bool newMute) {
	wcout << volController.getAudioSessionManager().getSessionNameByID(id)->c_str() << " Volume changed! New Vol: " << newVolume << " | Muted: " << newMute << endl;
}

void Logger::OnDefaultDevicePropertyChanged(PROPERTYKEY key) {
	cout << "Default Device property changed!" << endl;
}

void Logger::OnEndpointVolumeChanged(float newVolume, bool newMute) {
	cout << "Endpoint volume changed! Volume: " << newVolume << " | Muted: " << newMute << endl;
}

