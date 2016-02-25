
#include "elastos/droid/server/hdmi/ActiveSourceHandler.h"

// using Elastos::Droid::Server::Hdmi::HdmiCecLocalDevice::ActiveSource;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String ActiveSourceHandler::TAG("ActiveSourceHandler");

ECode ActiveSourceHandler::Create(
    /* [in] */ HdmiCecLocalDeviceTv* source,
    /* [in] */ IIHdmiControlCallback* callback,
    /* [out] */ ActiveSourceHandler** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (source == NULL) {
            Slogger::E(TAG, "Wrong arguments");
            return NULL;
        }
        return new ActiveSourceHandler(source, callback);
#endif
}

ActiveSourceHandler::ActiveSourceHandler(
    /* [in] */ HdmiCecLocalDeviceTv* source,
    /* [in] */ IIHdmiControlCallback* callback)
{
#if 0 // TODO: Translate codes below
        mSource = source;
        mService = mSource.getService();
        mCallback = callback;
#endif
}

ECode ActiveSourceHandler::Process(
    /* [in] */ HdmiCecLocalDevice::ActiveSource* newActive)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Seq #17
        HdmiCecLocalDeviceTv tv = mSource;
        ActiveSource activeSource = tv.getActiveSource();
        if (activeSource.equals(newActive)) {
            invokeCallback(HdmiControlManager.RESULT_SUCCESS);
            return;
        }
        HdmiDeviceInfo device = mService.getDeviceInfo(newActive.logicalAddress);
        if (device == NULL) {
            tv.startNewDeviceAction(newActive);
        }
        if (!tv.isProhibitMode()) {
            tv.updateActiveSource(newActive);
            boolean notifyInputChange = (mCallback == NULL);
            tv.updateActiveInput(newActive.physicalAddress, notifyInputChange);
            invokeCallback(HdmiControlManager.RESULT_SUCCESS);
        } else {
            // TV is in a mode that should keep its current source/input from
            // being changed for its operation. Reclaim the active source
            // or switch the port back to the one used for the current mode.
            ActiveSource current = tv.getActiveSource();
            if (current.logicalAddress == getSourceAddress()) {
                HdmiCecMessage activeSourceCommand = HdmiCecMessageBuilder.buildActiveSource(
                        current.logicalAddress, current.physicalAddress);
                mService.sendCecCommand(activeSourceCommand);
                tv.updateActiveSource(current);
                invokeCallback(HdmiControlManager.RESULT_SUCCESS);
            } else {
                HdmiCecMessage routingChange = HdmiCecMessageBuilder.buildRoutingChange(
                        getSourceAddress(), newActive.physicalAddress, current.physicalAddress);
                mService.sendCecCommand(routingChange);
                tv.addAndStartAction(
                        new RoutingControlAction(tv, current.physicalAddress, true, mCallback));
            }
        }
#endif
}

ECode ActiveSourceHandler::GetSourceAddress(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSource.getDeviceInfo().getLogicalAddress();

#endif
}

ECode ActiveSourceHandler::InvokeCallback(
    /* [in] */ Int32 result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mCallback == NULL) {
            return;
        }
        try {
            mCallback.onComplete(result);
        } catch (RemoteException e) {
            Slogger::E(TAG, "Callback failed:" + e);
        }

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
