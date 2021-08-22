#pragma once

#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"


class WebRTCConnection : public webrtc::PeerConnectionObserver,
                         public webrtc::CreateSessionDescriptionObserver,
                         public webrtc::SetSessionDescriptionObserver
{
    friend class WebRTCConnectionFactory;
public:    
    ~WebRTCConnection();
    // PeerConnectionObserver implementation
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;
    
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {};
    void OnRenegotiationNeeded() override {};
    void OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) override {};

    
    // CreateSessionDescriptionObserver implementation.
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;

    // SetSessionDescriptionObserver
    void OnSuccess() override {};
    void OnFailure(webrtc::RTCError error) override {};

    void SetRemoteDescription();

private:
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peerConnection;
    rtc::scoped_refptr<webrtc::DataChannelInterface> _dataChannel;
};
