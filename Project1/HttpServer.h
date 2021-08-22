#pragma once

#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP

#include <string>
#include <stdexcept>
#include <vector>
#include <map>

#include <http.h>
//#include <websocket.h>
#include <windows.h>
#include "resource.h"

constexpr int requestBufferLength = sizeof(HTTP_REQUEST) + 2048;
const auto OK_REASON = std::string("OK");
const auto HEADER_HTML_TYPE = std::string("text/html");
const auto HEADER_JSON_TYPE = std::string("application/json");

const auto PROTOCOL_REASON = std::string("Switching Protocols");

const auto UPGRADE = std::string("Upgrade");
const auto WEBSOCKET = std::string("websocket");

struct KnownHeaderTuple {
	int knownHeaderEnum;
	std::string headerName;
};

const std::vector<KnownHeaderTuple> KNOWN_HEADERS = {
	{ HttpHeaderHost, "Host" },
	{ HttpHeaderConnection, "Connection"},
	{ HttpHeaderUpgrade, "Upgrade" }
};

// TODO: I probably shouldn't throw by just disable the thread. Maybe catch it and kill if it doesn't work?
inline void throw_if_fail(ULONG error_code)
{
	if (error_code)
	{
		// TODO: format an error here using sprintf
		throw std::runtime_error("Error encountered");
	}
}

inline void throw_if_fail(HRESULT error_code)
{
	if (error_code != S_OK)
	{
		// TODO: format an error here using sprintf
		throw std::runtime_error("Error encountered");
	}
}

inline HANDLE throw_if_null(HANDLE handle)
{
	if (handle == NULL)
	{
		throw std::runtime_error("Handle is null");
	}

	return handle;
}

class HttpServer
{
public:
	HttpServer();
	~HttpServer();
	void Listen();

private:
	void LoadHTML();
	void InitializeHTTP();
	void RegisterURLs();	

	HGLOBAL m_hHTMLIndex;
	DWORD m_htmlIndexLength;
	HANDLE m_hReqQueue;
	HTTP_SERVER_SESSION_ID m_sessionId;
	HTTP_URL_GROUP_ID m_urlGroupId;
	//WEB_SOCKET_HANDLE m_hSocket;
	std::vector<std::string> m_urls;
};

