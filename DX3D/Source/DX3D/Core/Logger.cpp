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

void dx3d::Logger::Log(LogLevel level, const char* message, const char* file, const int line) {

	auto LogLevel2String = [](LogLevel log_level) -> const char* {
		switch (log_level) {
		case LogLevel::Error: return "Error";
		case LogLevel::Warning: return "Warning";
		case LogLevel::Info: return "Info";
		default: return "Unknown";
		}
		};

	if (level > _log_level) return;

	std::clog << "[DX3D - " << LogLevel2String(level) << "] " << message << "\n";

#ifdef _DEBUG
	if (LogLevel::Warning == level || LogLevel::Error == level) {
		std::clog << file << " : line " << line << "\n";
	}
#endif // _DEBUG
}
