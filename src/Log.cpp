#include "StdAfx.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

std::shared_ptr<spdlog::logger> Log::m_sLogger;

bool Log::Init(const std::string& processFileName)
{
	try
	{
		AllocConsole();

		//Log console sink
		auto log_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		log_console_sink->set_pattern("%v%$");
		
		//Log file sink
		auto log_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fmt::format("{}.log", processFileName), true);
		log_file_sink->set_pattern("[%d/%m/%C %T.%e] %v");
		
		std::vector<spdlog::sink_ptr> log_sinks{ log_console_sink, log_file_sink };

		m_sLogger = std::make_shared<spdlog::logger>("log", log_sinks.begin(), log_sinks.end());
		spdlog::register_logger(m_sLogger);

		//Set flush settings on the sinks
		spdlog::flush_on(spdlog::level::trace);
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		fprintf(stderr, "log init failed: %s\n", ex.what());
		return false;
	}

	return true;
}

void Log::Destroy()
{
	spdlog::drop_all();
}
