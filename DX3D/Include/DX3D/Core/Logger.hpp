#pragma once

namespace dx3d {
	class Logger {
	public:

		enum class LogLevel {
			Info = 0,
			Warning,
			Error
		};

		explicit Logger(const LogLevel log_level = LogLevel::Error);
		~Logger();

		void Log(const LogLevel log_level, const char* message) const;

	private:
		const LogLevel _log_level;
	};

} // namespace dx3d
