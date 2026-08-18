#pragma once

namespace dx3d {
	class Logger final {
	public:

		enum class LogLevel {
			Error = 0,
			Warning,
			Info,
		};

		explicit Logger(const LogLevel log_level = LogLevel::Error);
		~Logger();

		void Log(LogLevel level, const char* message, const char* file, const int line);

	private:
		const LogLevel _log_level;
	};

} // namespace dx3d

#define DX3DLogUtil(logger, type, message, file, line)\
	logger.Log((type), message, file, line)

#define DX3DLog(logger, type, message)\
	DX3DLogUtil(logger, (type), message, __FILE__, __LINE__)

#define DX3DLogThrowUtil(logger, exception, type, message, file, line) {\
	DX3DLogUtil(logger, type, message, file, line);\
	throw exception(message);\
}

#define DX3DLogThrow(logger, exception, type, message)\
	DX3DLogThrowUtil(logger, exception, type, message, __FILE__, __LINE__)
