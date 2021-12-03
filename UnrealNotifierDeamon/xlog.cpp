#include "xlog.h"
#include "wx/string.h"
#include "wx/log.h"

namespace
{
	auto logger{ wxLogStderr() };
}

void xlog(const ELogType logType, const wxString& message)
{
	const wxLogLevel logLevel = [&logType]() -> wxLogLevel
	{
		switch (logType)
		{
		case ELogType::warning:
			return wxLOG_Warning;
		case ELogType::error:
			return wxLOG_Error;
		case ELogType::info:
			return wxLOG_Info;
		default:
			wxASSERT(false);
			return wxLOG_Error;
		}
	}();
	logger.LogTextAtLevel(logLevel, message);
}
