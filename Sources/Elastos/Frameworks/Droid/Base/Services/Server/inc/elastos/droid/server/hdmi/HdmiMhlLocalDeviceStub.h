package com.android.server.hdmi;

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

/**
 * Stub class that models a logical mhl device hosted in this system.
 */
final class HdmiMhlLocalDeviceStub {

    private static const HdmiDeviceInfo INFO = new HdmiDeviceInfo(
            Constants.INVALID_PHYSICAL_ADDRESS, Constants.INVALID_PORT_ID, -1, -1);
    private final HdmiControlService mService;
    private final Int32 mPortId;

    protected HdmiMhlLocalDeviceStub(HdmiControlService service, Int32 portId) {
        mService = service;
        mPortId = portId;
    }

    void OnDeviceRemoved() {
    }

    HdmiDeviceInfo GetInfo() {
        return INFO;
    }

    void SetBusMode(Int32 cbusmode) {
    }

    void OnBusOvercurrentDetected(Boolean on) {
    }

    void SetDeviceStatusChange(Int32 adopterId, Int32 deviceId) {
    }

    Int32 GetPortId() {
        return mPortId;
    }

    void TurnOn(IHdmiControlCallback callback) {
    }

    void SendKeyEvent(Int32 keycode, Boolean isPressed) {
    }
}
