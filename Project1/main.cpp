//

#include <iostream>
#include "HttpServer.h"
#include "WebRTCConnectionFactory.h"

int main()
{
	WebRTCConnectionFactory web;
	auto conn = web.CreatePeerConnection();

	HttpServer http;
	http.Listen();
}