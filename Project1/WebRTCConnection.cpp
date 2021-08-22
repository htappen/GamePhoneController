#include <iostream>

#include "WebRTCConnection.h"

/*
https://webrtc.googlesource.com/src/+/refs/heads/main/examples/peerconnection/client/conductor.cc
https://developer.mozilla.org/en-US/docs/Web/API/WebRTC_API/Simple_RTCDataChannel_sample
*/

void WebRTCConnection::MakeOffer()
{
	_peerConnection->CreateOffer(
		this,
		webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
}

void WebRTCConnection::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel)
{
	_dataChannel = channel;
}

void WebRTCConnection::OnSuccess(webrtc::SessionDescriptionInterface* desc) 
{
	std::string out;
	desc->ToString(&out);
	std::cout << out;
};

WebRTCConnection::~WebRTCConnection()
{	
	_peerConnection = nullptr;
	_dataChannel = nullptr;
	
}

void WebRTCConnection::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
	std::string out;
	candidate->ToString(&out);
	std::cout << out;
}