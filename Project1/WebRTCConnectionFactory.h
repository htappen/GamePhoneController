#pragma once

#include "WebRTCConnection.h"
#include "rtc_base/ssl_adapter.h"

class WebRTCConnectionFactory
{
public:
	WebRTCConnectionFactory();
	~WebRTCConnectionFactory();

	rtc::scoped_refptr<WebRTCConnection> CreatePeerConnection();
private:
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peerConnectionFactory;
	std::unique_ptr<rtc::Thread> _networkThread;
	std::unique_ptr<rtc::Thread> _workerThread;
	std::unique_ptr<rtc::Thread> _signalingThread;
};

