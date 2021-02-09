#pragma once

#include <Runtime/CoreMacros.h>

#ifdef IE_PLATFORM_BUILD_WIN32
#	include "Platform/Win32/ConsoleWindow.h"
#endif 


/*
	Valid log categories for IE_LOG
*/
enum class ELogSeverity
{
	Log,
	Verbose,
	Warning,
	Error,
	Critical,
};

namespace Insight {

	namespace Debug {


		class INSIGHT_API Logger
		{
		public:
			static bool Initialize();
			~Logger();

			inline static void AppendMessageForCoreDump(const char* Message)
			{
				s_CoreDumpMessage.append(Message);
				s_CoreDumpMessage.append("\n");
			}

			static void InitiateCoreDump();

			inline static ConsoleWindow& GetConsoleWindow() { return s_ConsoleWindow; }

		private:
			static std::string s_CoreDumpMessage;
#	if defined (IE_DEBUG) && defined (IE_PLATFORM_BUILD_WIN32)
			static ConsoleWindow s_ConsoleWindow;
#	endif
		};



		// Logger that will log output to the console window. Recommended that you dont call directly. 
		// Instead, use IE_LOG so logs will be stripped from release builds.
		void LogHelper(ELogSeverity Severity, const char* fmt, const char* File, const char* Function, int Line, ...);

	} // end namespace Debug
} // end namespace Insight

#if defined (IE_DEBUG) || defined (IE_RELEASE)

// If Expr is true, a exception will be raised.
#	define IE_FATAL_ERROR(Expr, Message, Category) if( !(Expr) ) { throw ieException(Message, Category); }

// Log a message to the console.
#	define IE_LOG(Severity, fmt, ...) ::Insight::Debug::LogHelper(ELogSeverity::##Severity, fmt, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

#else
#	define IE_FATAL_ERROR(...)
#	define IE_LOG(LogSeverity, fmt, ...)
#endif
