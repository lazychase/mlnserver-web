#include <cstdlib>
#include <filesystem>

#include <net/session.hpp>
#include <net/logger.hpp>


#ifdef _WIN32
#include <net/exceptionHandler.hpp>
#else//#ifdef _WIN32
#include <unistd.h>
#include <limits.h>
#include <libgen.h>	// for readlink(linux only)
#endif


#include <net/netService.hpp>
#include <net/packetJson/packetParser.hpp>

#include "asioContext.h"
#include "configuration.h"
#include "lobbyServerCoro.h"


#include "redisConnector.h"
#include "tableManager.h"

void setExecutablePath(const char* argv0);


int main(int argc, char* argv[])
{
	using namespace mln::net;

	setExecutablePath(argv[0]);

	// ready orm
	std::cout << "init orm" << std::endl;
	mapStruct::Config::initOrm();

	// load config
	std::cout << "loading configurations" << std::endl;
	auto& conf = Configuration::instance().load();

	// init logger
	Logger::createDefault();
	LOGD("logger initialized");

	// env variables.
	if (const char* MYAPP_HTTP_PORT = std::getenv("MYAPP_HTTP_PORT")) {
		LOGI("(env)MYAPP_HTTP_PORT : {}", MYAPP_HTTP_PORT);
	}

#ifdef _WIN32
	mln::net::ExceptionHandler::init();

	if (FALSE == SetConsoleTitleA("mln-server")) {
		LOGE("SetConsoleTitle failed {}", GetLastError());
	}
#endif

	if (0 != conf.redisConf.REDIS_HOST.length()) {
		RedisConnector::instance().init(conf.redisConf);
	}

	if (false == TableManager::instance().loadTables()) {
		LOGC("failed loading-tables");
		LOGC("Closing Server.");

		// !!
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::exit(0);
	}

	if (0 != conf.MYAPP_HTTP_PORT) {
		LobbyServerCoro::instance().run(conf.MYAPP_HTTP_PORT, 20);
	}


	// 작동중인 작업들이 종료되기를 기다립니다.
	std::this_thread::sleep_for(std::chrono::seconds(5));
}


#ifdef _WIN32
std::string executable_path(const char* /*argv0*/)
{
	typedef std::vector<char> char_vector;
	typedef std::vector<char>::size_type size_type;
	char_vector buf(1024, 0);
	size_type size = buf.size();
	bool havePath = false;
	bool shouldContinue = true;
	do
	{
		DWORD result = GetModuleFileNameA(nullptr, &buf[0], size);
		DWORD lastError = GetLastError();
		if (result == 0)
		{
			shouldContinue = false;
		}
		else if (result < size)
		{
			havePath = true;
			shouldContinue = false;
		}
		else if (
			result == size
			&& (lastError == ERROR_INSUFFICIENT_BUFFER || lastError == ERROR_SUCCESS)
			)
		{
			size *= 2;
			buf.resize(size);
		}
		else
		{
			shouldContinue = false;
		}
	} while (shouldContinue);
	if (!havePath)
	{
		throw std::runtime_error("failed getExecutablePath.");
	}
	// On Microsoft Windows, there is no need to call boost::filesystem::canonical or
	// boost::filesystem::path::make_preferred. The path returned by GetModuleFileNameA
	// is the one we want.
	std::string ret = &buf[0];
	return ret;
}
#endif//#ifdef _WIN32

void setExecutablePath(const char* argv0)
{
#ifdef _WIN32
	std::string exePath = std::move(executable_path(argv0));
	std::string exeDir = exePath.substr(0, exePath.find_last_of("\\"));
	std::filesystem::current_path(exeDir);
#endif//#ifdef _WIN32

#ifdef __linux__
	char executable_path[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", executable_path, PATH_MAX);
	const char* path = nullptr;
	if (-1 != count) {
		path = dirname(executable_path);
		chdir(path);
	}
#endif//__linux__
}
