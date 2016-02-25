
#include "elastos/droid/server/hdmi/HdmiCecLocalDevicePlayback.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String HdmiCecLocalDevicePlayback::TAG("HdmiCecLocalDevicePlayback");

HdmiCecLocalDevicePlayback::HdmiCecLocalDevicePlayback()
    : mIsActiveSource(FALSE)
{}

ECode HdmiCecLocalDevicePlayback::constructor(
    /* [in] */ HdmiControlService* service)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(service, HdmiDeviceInfo.DEVICE_PLAYBACK);

#endif
}

ECode HdmiCecLocalDevicePlayback::Init()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super.init();
        mIsActiveSource = FALSE;

#endif
}

ECode HdmiCecLocalDevicePlayback::OnAddressAllocated(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mService.sendCecCommand(HdmiCecMessageBuilder.buildReportPhysicalAddressCommand(
                mAddress, mService.getPhysicalAddress(), mDeviceType));
        startQueuedActions();

#endif
}

ECode HdmiCecLocalDevicePlayback::GetPreferredAddress(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return SystemProperties.getInt(Constants::PROPERTY_PREFERRED_ADDRESS_PLAYBACK,
                Constants::ADDR_UNREGISTERED);

#endif
}

ECode HdmiCecLocalDevicePlayback::SetPreferredAddress(
    /* [in] */ Int32 addr)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        SystemProperties.set(Constants::PROPERTY_PREFERRED_ADDRESS_PLAYBACK,
                String.valueOf(addr));

#endif
}

ECode HdmiCecLocalDevicePlayback::OneTouchPlay(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (hasAction(OneTouchPlayAction.class)) {
            Slogger::W(TAG, "oneTouchPlay already in progress");
            invokeCallback(callback, HdmiControlManager.RESULT_ALREADY_IN_PROGRESS);
            return;
        }
        // TODO: Consider the case of multiple TV sets. For now we always direct the command
        //       to the primary one.
        OneTouchPlayAction action = OneTouchPlayAction.create(this, Constants::ADDR_TV,
                callback);
        if (action == NULL) {
            Slogger::W(TAG, "Cannot initiate oneTouchPlay");
            invokeCallback(callback, HdmiControlManager.RESULT_EXCEPTION);
            return;
        }
        addAndStartAction(action);

#endif
}

ECode HdmiCecLocalDevicePlayback::QueryDisplayStatus(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (hasAction(DevicePowerStatusAction.class)) {
            Slogger::W(TAG, "queryDisplayStatus already in progress");
            invokeCallback(callback, HdmiControlManager.RESULT_ALREADY_IN_PROGRESS);
            return;
        }
        DevicePowerStatusAction action = DevicePowerStatusAction.create(this,
                Constants::ADDR_TV, callback);
        if (action == NULL) {
            Slogger::W(TAG, "Cannot initiate queryDisplayStatus");
            invokeCallback(callback, HdmiControlManager.RESULT_EXCEPTION);
            return;
        }
        addAndStartAction(action);

#endif
}

ECode HdmiCecLocalDevicePlayback::InvokeCallback(
    /* [in] */ IIHdmiControlCallback* callback,
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        try {
            callback.onComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Invoking callback failed:" + e);
        }

#endif
}

ECode HdmiCecLocalDevicePlayback::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mCecMessageCache.flushAll();
        // We'll not clear mIsActiveSource on the hotplug event to pass CETC 11.2.2-2 ~ 3.
        if (connected && mService.isPowerStandbyOrTransient()) {
            mService.wakeUp();
        }

#endif
}

ECode HdmiCecLocalDevicePlayback::MarkActiveSource()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mIsActiveSource = true;

#endif
}

ECode HdmiCecLocalDevicePlayback::HandleActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int physicalAddress = HdmiUtils.twoBytesToInt(message.getParams());
        mayResetActiveSource(physicalAddress);
        return true;  // Broadcast message.

#endif
}

ECode HdmiCecLocalDevicePlayback::MayResetActiveSource(
    /* [in] */ Int32 physicalAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (physicalAddress != mService.getPhysicalAddress()) {
            mIsActiveSource = FALSE;
        }

#endif
}

ECode HdmiCecLocalDevicePlayback::HandleSetStreamPath(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int physicalAddress = HdmiUtils.twoBytesToInt(message.getParams());
        maySetActiveSource(physicalAddress);
        maySendActiveSource();
        wakeUpIfActiveSource();
        return true;  // Broadcast message.

#endif
}

ECode HdmiCecLocalDevicePlayback::HandleRoutingChange(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int newPath = HdmiUtils.twoBytesToInt(message.getParams(), 2);
        maySetActiveSource(newPath);
        return true;  // Broadcast message.

#endif
}

ECode HdmiCecLocalDevicePlayback::HandleRoutingInformation(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int physicalAddress = HdmiUtils.twoBytesToInt(message.getParams());
        maySetActiveSource(physicalAddress);
        return true;  // Broadcast message.

#endif
}

ECode HdmiCecLocalDevicePlayback::MaySetActiveSource(
    /* [in] */ Int32 physicalAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (physicalAddress == mService.getPhysicalAddress()) {
            mIsActiveSource = true;
        } else {
            mIsActiveSource = FALSE;
        }

#endif
}

ECode HdmiCecLocalDevicePlayback::WakeUpIfActiveSource()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mIsActiveSource && mService.isPowerStandbyOrTransient()) {
            mService.wakeUp();
        }

#endif
}

ECode HdmiCecLocalDevicePlayback::MaySendActiveSource()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mIsActiveSource) {
            mService.sendCecCommand(HdmiCecMessageBuilder.buildActiveSource(
                    mAddress, mService.getPhysicalAddress()));
        }

#endif
}

ECode HdmiCecLocalDevicePlayback::HandleRequestActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        maySendActiveSource();
        return true;  // Broadcast message.

#endif
}

ECode HdmiCecLocalDevicePlayback::DisableDevice(
    /* [in] */ Boolean initiatedByCec,
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super.disableDevice(initiatedByCec, callback);
        assertRunOnServiceThread();
        if (!initiatedByCec && mIsActiveSource) {
            mService.sendCecCommand(HdmiCecMessageBuilder.buildInactiveSource(
                    mAddress, mService.getPhysicalAddress()));
        }
        mIsActiveSource = FALSE;
        checkIfPendingActionsCleared();

#endif
}

ECode HdmiCecLocalDevicePlayback::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super.dump(pw);
        pw.println("mIsActiveSource: " + mIsActiveSource);

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
