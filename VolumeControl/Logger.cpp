#include "stdafx.h"
#include "Logger.h"
#include <iostream>


Logger::Logger(VolumeController& volCon) : volController(volCon) {
	
}

Logger::~Logger() = default;

void Logger::OnDefaultDeviceChanged() {
	std::cout << "Default Audio Endpoint Device changed!" << std::endl;
}

void Logger::OnSessionCreated(unsigned id) {
	std::cout << "Session created! ID: " << id << std::endl;;
}

void Logger::OnSessionRemoved(unsigned id) {
	std::cout << "Session removed! ID: " << id << std::endl;
}

void Logger::OnVolumeChanged(unsigned id, const float newVolume, const bool newMute) {
	std::wcout << volController.getAudioSessionManager().getSessionNameByID(id)->c_str() << std::endl;
	//std::cout << " audio session with id " << id << " changed volume to " << newVolume << " | Muted: " << newMute << std::endl;
}

void Logger::OnDefaultDevicePropertyChanged(PROPERTYKEY key) {
	std::cout << "Default Device property changed!" << std::endl;
}

void Logger::OnEndpointVolumeChanged(float newVolume, bool newMute) {
	std::cout << "Endpoint volume changed! Volume: " << newVolume << " | Muted: " << newMute << std::endl;
}

