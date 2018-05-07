#include "stdafx.h"
#include "Logger.h"
#include <iostream>


Logger::Logger() = default;

Logger::~Logger() = default;

void Logger::OnDefaultDeviceChanged() {
	std::cout << "new Default Device" << std::endl;
}

void Logger::OnSessionCreated(unsigned id) {
}

void Logger::OnSessionRemoved(unsigned id) {
}

void Logger::OnVolumeChanged(unsigned id, const float newVolume, const bool newMute) {
	std::cout << "Session with id " << id << " changed volume to " << newVolume << ". muted: " << newMute << std::endl;
}

void Logger::OnDefaultDevicePropertyChanged(PROPERTYKEY key) {

}

