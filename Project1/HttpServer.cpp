#include "HttpServer.h"

void HttpServer::LoadHTML()
{
	HMODULE thisModule = NULL;
	HRSRC hHTMLIndexId = FindResource(
		thisModule, // TODO: check if this will stil work in a DLL
		MAKEINTRESOURCE(IDR_HTML1),
		RT_HTML
	);

	m_htmlIndexLength = SizeofResource(thisModule, hHTMLIndexId);
	if (m_htmlIndexLength == 0)
	{
		throw std::runtime_error("Error");
	}

	m_hHTMLIndex = throw_if_null(LoadResource(NULL, hHTMLIndexId));
}

void HttpServer::InitializeHTTP()
{
	throw_if_fail(HttpInitialize(HTTP_VERSION_2_0, HTTP_INITIALIZE_SERVER, NULL));

	// TODO: move into another function
	throw_if_fail(HttpCreateServerSession(
		HTTPAPI_VERSION_2,
		&m_sessionId,
		0x0
	));
}

void HttpServer::RegisterURLs()
{
	throw_if_fail(HttpCreateUrlGroup(
		m_sessionId,
		&m_urlGroupId,
		0x0
	));

	throw_if_fail(HttpAddUrlToUrlGroup(
		m_urlGroupId,
		L"http://localhost:8888/",
		0x0,
		0x0
	));

	SECURITY_ATTRIBUTES queueSecurityAttributes = {
	sizeof(SECURITY_ATTRIBUTES),
	NULL,
	false
	};

	throw_if_fail(HttpCreateRequestQueue(
		HTTP_VERSION_2_0,
		NULL,
		&queueSecurityAttributes,
		0x0,
		&m_hReqQueue
	));

	HTTP_PROPERTY_FLAGS bindFlags = { 1 };
	HTTP_BINDING_INFO bindInfo = {
		bindFlags,
		m_hReqQueue
	};
	throw_if_fail(HttpSetUrlGroupProperty(
		m_urlGroupId,
		HttpServerBindingProperty,
		&bindInfo,
		sizeof(HTTP_BINDING_INFO)
	));
}

HttpServer::HttpServer() : m_hReqQueue(0), m_sessionId(0), m_urlGroupId(0) {
	LoadHTML();
	InitializeHTTP();
	RegisterURLs();
}

void HttpServer::Listen()
{
	for (;;)
	{
		DWORD bytesRead;
		char requestBuffer[requestBufferLength];
		auto pRequest = reinterpret_cast<PHTTP_REQUEST>(requestBuffer);

		// TODO: use overlapped structure to do asynchronous
		throw_if_fail(HttpReceiveHttpRequest(
			m_hReqQueue,
			HTTP_NULL_ID,
			0,
			pRequest,
			requestBufferLength,
			&bytesRead,
			NULL
		));

		if (pRequest->Verb == HttpVerbGET)
		{
			auto path = pRequest->CookedUrl.pAbsPath;
			if (wcscmp(path, L"/") == 0)
			{
				// TODO: move this into a private method
				HTTP_RESPONSE   resp;
				memset(&resp, 0, sizeof(resp));
				HTTP_DATA_CHUNK dataChunk;
				DWORD           bytesSent;

				resp.StatusCode = 200;
				resp.pReason = OK_REASON.c_str();
				resp.ReasonLength = OK_REASON.size();

				resp.Headers.KnownHeaders[HttpHeaderContentType].pRawValue = HEADER_HTML_TYPE.c_str();
				resp.Headers.KnownHeaders[HttpHeaderContentType].RawValueLength = HEADER_HTML_TYPE.size();

				dataChunk.DataChunkType = HttpDataChunkFromMemory;
				dataChunk.FromMemory.pBuffer = m_hHTMLIndex;
				dataChunk.FromMemory.BufferLength = m_htmlIndexLength;

				resp.EntityChunkCount = 1;
				resp.pEntityChunks = &dataChunk;

				throw_if_fail(HttpSendHttpResponse(
					m_hReqQueue,
					pRequest->RequestId, // Request ID
					0,                   // Flags
					&resp,               // HTTP response
					NULL,                // pReserved1
					&bytesSent,          // bytes sent  (OPTIONAL)
					NULL,                // pReserved2  (must be NULL)
					0,                   // Reserved3   (must be 0)
					NULL,                // LPOVERLAPPED(OPTIONAL)
					NULL                 // pReserved4  (must be NULL)
				));
			}
			else if (wcscmp(path, L"/connect") == 0)
			{
				

			}
		}
		else
		{

		}
	}
}

HttpServer::~HttpServer()
{
	HttpCloseUrlGroup(m_urlGroupId);
	HttpCloseServerSession(m_sessionId);
	HttpCloseRequestQueue(m_hReqQueue);
	//WebSocketEndServerHandshake(m_hSocket);
	//WebSocketDeleteHandle(m_hSocket);
}


// TODO: get player number from query string
				/*
				throw_if_fail(WebSocketCreateServerHandle(
					NULL,
					0,
					&m_hSocket
				));

				// TODO: should I just grab the array instead?
				auto webSocketReqHeaders = std::vector<WEB_SOCKET_HTTP_HEADER>();
				const auto& rawReqHeaders = pRequest->Headers;
				for (int i = 0; i < rawReqHeaders.UnknownHeaderCount; i++)
				{
					const auto& header = rawReqHeaders.pUnknownHeaders[i];
					WEB_SOCKET_HTTP_HEADER newHeader = {
						(PCHAR)header.pName,
						header.NameLength,
						(PCHAR)header.pRawValue,
						header.RawValueLength
					};
					webSocketReqHeaders.push_back(newHeader);
				}
				// TODO: move this into a method

				for (auto it = KNOWN_HEADERS.begin(); it != KNOWN_HEADERS.end(); ++it)
				{
					const auto& knownHeaderTuple = *it;
					const auto& header = rawReqHeaders.KnownHeaders[knownHeaderTuple.knownHeaderEnum];

					WEB_SOCKET_HTTP_HEADER newHeader = {
						(PCHAR) knownHeaderTuple.headerName.c_str(),
						knownHeaderTuple.headerName.size(),
						(PCHAR) header.pRawValue,
						header.RawValueLength
					};
					webSocketReqHeaders.push_back(newHeader);
				}
				// TODO: Save the request ID so I can

				WEB_SOCKET_HTTP_HEADER* pRespHeaders;
				ULONG responseHeaderCount;
				throw_if_fail(WebSocketBeginServerHandshake(
					m_hSocket,
					"json",
					NULL,
					0,
					&webSocketReqHeaders[0],
					webSocketReqHeaders.size(),
					&pRespHeaders,
					&responseHeaderCount
				));

				// TODO: move this into a private method
				HTTP_RESPONSE   resp;
				memset(&resp, 0, sizeof(resp));
				HTTP_DATA_CHUNK dataChunk;
				DWORD           bytesSent;

				resp.StatusCode = 101;
				resp.pReason = PROTOCOL_REASON.c_str();
				resp.ReasonLength = PROTOCOL_REASON.size();

				resp.Headers.KnownHeaders[HttpHeaderUpgrade].pRawValue = WEBSOCKET.c_str();
				resp.Headers.KnownHeaders[HttpHeaderUpgrade].RawValueLength = WEBSOCKET.size();
				resp.Headers.KnownHeaders[HttpHeaderConnection].pRawValue = UPGRADE.c_str();
				resp.Headers.KnownHeaders[HttpHeaderConnection].RawValueLength = UPGRADE.size();

				auto respHeaders = std::vector<HTTP_UNKNOWN_HEADER>();
				for (int i = 0; i < responseHeaderCount; i++)
				{
					const auto& webSocketHeader = pRespHeaders[i];
					if (strncmp(webSocketHeader.pcName, "Sec", 3))
					{
						HTTP_UNKNOWN_HEADER newHeader = {
							webSocketHeader.ulNameLength,
							webSocketHeader.ulValueLength,
							webSocketHeader.pcName,
							webSocketHeader.pcValue
						};
						respHeaders.push_back(newHeader);
					}
				}

				resp.Headers.pUnknownHeaders = &respHeaders[0];
				resp.Headers.UnknownHeaderCount = respHeaders.size();

				throw_if_fail(HttpSendHttpResponse(
					m_hReqQueue,
					pRequest->RequestId, // Request ID
					HTTP_SEND_RESPONSE_FLAG_MORE_DATA,
					&resp,               // HTTP response
					NULL,                // pReserved1
					&bytesSent,          // bytes sent  (OPTIONAL)
					NULL,                // pReserved2  (must be NULL)
					0,                   // Reserved3   (must be 0)
					NULL,                // LPOVERLAPPED(OPTIONAL)
					NULL                 // pReserved4  (must be NULL)
				));

				// TODO: should this also be in the destructor?
				throw_if_fail(WebSocketEndServerHandshake(
					m_hSocket
				));
				*/