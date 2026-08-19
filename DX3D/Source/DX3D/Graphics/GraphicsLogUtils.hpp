#pragma once
#include <DX3D/Core/WinSwitches.hpp>
#include <d3d11.h>
#include <string>
#include <DX3D/Core/Core.hpp>
#include <DX3D/Core/Logger.hpp>

namespace dx3d {

	namespace graphics_log_utils {

		inline std::string GetErrorMessageFromHResult(Logger& logger, const HRESULT hr, const char* message, const char* file, const int line) {
			char* hresult_error_message = nullptr;

			DWORD size = FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				hr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<LPSTR>(&hresult_error_message),
				0,
				nullptr);

			std::string error_message{};

			if (size > 0 && hresult_error_message) {
				error_message = hresult_error_message;

				// Trim trailing newline characters that FormatMessage adds
				while (!error_message.empty() && (error_message.back() == '\r' || error_message.back() == '\n')) {
					error_message.pop_back();
				}

				error_message = std::string(message) + " - " + error_message;

				LocalFree(hresult_error_message); // free buffer allocated by FormatMessage
			} else {
				error_message = std::string(message) + " - Unknown error code: " + std::to_string(hr);
			}

			return error_message;
		}

		inline void CheckHResult(Logger& logger, const HRESULT hr, const char* message, const char* file, const int line) {

			if (FAILED(hr)) {
				DX3DLogThrowUtil(
					logger,
					std::runtime_error,
					Logger::LogLevel::Error,
					GetErrorMessageFromHResult(logger, hr, message, file, line).c_str(),
					file, line
				);
			}
		}

		inline void CheckShaderCompile(Logger& logger, const HRESULT hr, ID3DBlob* error_blob, const char* file, const int line) {

			auto error_msg = error_blob ? static_cast<const char*>(error_blob->GetBufferPointer()) : nullptr;

			if (FAILED(hr)) {
				DX3DLogThrowUtil(
					logger,
					std::runtime_error,
					Logger::LogLevel::Error,
					error_msg ? error_msg : "Shader compilation failed",
					file, line
				);
			}

			if (error_msg) {
				DX3DLogUtil(logger, Logger::LogLevel::Warning, error_msg, file, line);
			}
		}
	} // namespace graphics_log_utils

} // namespace dx3d

#define DX3DGraphicsLogThrowOnFailUtil(hr, message, file, line) {\
	const auto res = (hr);\
	dx3d::graphics_log_utils::CheckHResult(GetLogger(), res, message, file, line);\
}

#define DX3DGraphicsLogThrowOnFail(hr, message)\
	DX3DGraphicsLogThrowOnFailUtil(hr, message, __FILE__, __LINE__)


#define DX3DGraphicsCheckShaderCompileUtil(hr, error_blob, file, line) {\
	const auto res = (hr);\
	dx3d::graphics_log_utils::CheckShaderCompile(GetLogger(), res, error_blob, file, line);\
}

#define DX3DGraphicsCheckShaderCompile(hr, error_blob)\
	DX3DGraphicsCheckShaderCompileUtil(hr, error_blob, __FILE__, __LINE__)
