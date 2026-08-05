#include <iostream>
#include <DX3D/Core/Logger.hpp>

dx3d::Logger::Logger(const LogLevel log_level): _log_level(log_level) {

	std::clog << "[DX3D - Logger] C++ 3D Engine\n";
	std::clog << "[DX3D - Logger] Initialized with log level: " << static_cast<int>(_log_level) << "\n";
	std::clog << "----------------------------------------\n";
}

dx3d::Logger::~Logger() {
	std::clog << "----------------------------------------\n";
}

void dx3d::Logger::Log(const LogLevel log_level, const char* message) const {

	if (log_level <= _log_level) {
		std::clog << "[DX11 Engine] - ";

		switch (log_level) {
			case LogLevel::Error:
				std::clog << "Error";
				break;
			case LogLevel::Warning:
				std::clog << "Warning";
				break;
			case LogLevel::Info:
				std::clog << "Info";
				break;
			default:
				std::clog << "Log";
				break;
		}

		std::clog << " : " << message << "\n";
	}
}
