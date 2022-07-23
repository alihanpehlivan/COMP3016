#pragma once

#define SPDLOG_CLOCK_COARSE
#define SPDLOG_NO_THREAD_ID
#define SPDLOG_DISABLE_DEFAULT_LOGGER
#define SPDLOG_FMT_EXTERNAL
#define SPDLOG_NO_ATOMIC_LEVELS
#define SPDLOG_NO_EXCEPTIONS
#define SPDLOG_COMPILED_LIB

#include <spdlog/spdlog.h>

class Log
{
public:
	static bool Init(const std::string& processFileName);
	static void Destroy();

	static std::shared_ptr<spdlog::logger>& GetLogger() { return m_sLogger; }
private:
	static std::shared_ptr<spdlog::logger> m_sLogger;
};

// Core log macros
#define LOG_TRACE(...)    Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    Log::GetLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) Log::GetLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::critical, __VA_ARGS__)
