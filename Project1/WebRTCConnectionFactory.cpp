#include "WebRTCConnectionFactory.h"

#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

WebRTCConnectionFactory::WebRTCConnectionFactory()
{
	rtc::InitializeSSL();

	_networkThread = rtc::Thread::CreateWithSocketServer();
	_networkThread->SetName("pc_network_thread", nullptr);
	_networkThread->Start();

	_workerThread = rtc::Thread::Create();
	_workerThread->SetName("pc_worker_thread", nullptr);
	_workerThread->Start();

	_signalingThread = rtc::Thread::Create();
	_signalingThread->SetName("pc_signaling_thread", nullptr);
	_signalingThread->Start();

	_peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
		_networkThread.get(), _workerThread.get(), _signalingThread.get(), nullptr,
		webrtc::CreateBuiltinAudioEncoderFactory(), webrtc::CreateBuiltinAudioDecoderFactory(),
		webrtc::CreateBuiltinVideoEncoderFactory(), webrtc::CreateBuiltinVideoDecoderFactory(),
		nullptr, nullptr);
}

rtc::scoped_refptr<WebRTCConnection> WebRTCConnectionFactory::CreatePeerConnection()
{
	webrtc::PeerConnectionInterface::RTCConfiguration config;
	config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
	config.enable_dtls_srtp = true;
	/*
	webrtc::PeerConnectionInterface::IceServer server;
	server.uri = "stun:stun.l.google.com:19302";
	config.servers.push_back(server);
	*/

	rtc::scoped_refptr<WebRTCConnection> rtcConnection(
		new rtc::RefCountedObject<WebRTCConnection>());

	auto peerConnection = _peerConnectionFactory->CreatePeerConnection(
		config, nullptr, nullptr, rtcConnection);

	rtcConnection->_peerConnection = peerConnection;

	return rtcConnection;
}

WebRTCConnectionFactory::~WebRTCConnectionFactory()
{
	_networkThread = nullptr;
	_workerThread = nullptr;
	_signalingThread = nullptr;

	rtc::CleanupSSL();
}