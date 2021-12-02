#pragma once

enum class ELogType
{
	warning,
	error,
	info
};

class wxString;

void xlog(const ELogType logType, const wxString& message);