#pragma once

#include <DX3D/Core/Common.hpp>

namespace dx3d {

	class Base {
	public:
		explicit Base(const BaseDesc& desc);
		virtual ~Base();

		virtual Logger& GetLogger() noexcept final;

	protected:
		Base(const Base&) = delete;
		Base(Base&&) = delete;
		Base& operator = (const Base&) = delete;
		Base& operator = (Base&&) = delete;
	
	protected:
		Logger& _logger;
	};

} // namespace dx3d

#define DX3DLogInfoUtil(message, file, line)\
	DX3DLogUtil(GetLogger(), Logger::LogLevel::Info, message, file, line)

#define DX3DLogInfo(message)\
	DX3DLogInfoUtil(message, __FILE__, __LINE__)

#define DX3DLogWarningUtil(message, file, line)\
	DX3DLogUtil(GetLogger(), Logger::LogLevel::Warning, message, file, line)

#define DX3DLogWarning(message)\
	DX3DLogWarningUtil(message, __FILE__, __LINE__)

#define DX3DLogErrorUtil(message, file, line)\
	DX3DLogUtil(GetLogger(), Logger::LogLevel::Error, message, file, line)

#define DX3DLogError(message)\
	DX3DLogErrorUtil(message, __FILE__, __LINE__)

#define DX3DLogThrowErrorUtil(message, file, line)\
	DX3DLogThrowUtil(GetLogger(), std::runtime_error, Logger::LogLevel::Error, message, file, line)

#define DX3DLogThrowError(message)\
	DX3DLogThrowErrorUtil(message, __FILE__, __LINE__)

#define DX3DLogThrowInvalidArgUtil(message, file, line)\
	DX3DLogThrowUtil(GetLogger(), std::invalid_argument, Logger::LogLevel::Error, message, file, line)

#define DX3DLogThrowInvalidArg(message)\
	DX3DLogThrowInvalidArgUtil(message, __FILE__, __LINE__)
