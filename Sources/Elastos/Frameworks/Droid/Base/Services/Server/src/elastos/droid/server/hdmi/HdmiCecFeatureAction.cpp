
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.CoreLibrary.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiCecFeatureAction::ActionTimerHandler
//=============================================================================
ECode HdmiCecFeatureAction::ActionTimerHandler::constructor(
    /* [in] */ ILooper* looper)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                super(looper);

#endif
}

ECode HdmiCecFeatureAction::ActionTimerHandler::SendTimerMessage(
    /* [in] */ Int32 state,
    /* [in] */ Int64 delayMillis)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                // The third argument(0) is not used.
                sendMessageDelayed(obtainMessage(MSG_TIMEOUT, state, 0), delayMillis);

#endif
}

ECode HdmiCecFeatureAction::ActionTimerHandler::ClearTimerMessage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                removeMessages(MSG_TIMEOUT);

#endif
}

ECode HdmiCecFeatureAction::ActionTimerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch (msg.what) {
                case MSG_TIMEOUT:
                    handleTimerEvent(msg.arg1);
                    break;
                default:
                    Slog.w(TAG, "Unsupported message:" + msg.what);
                    break;
                }

#endif
}

//=============================================================================
// HdmiCecFeatureAction
//=============================================================================
const String HdmiCecFeatureAction::TAG("HdmiCecFeatureAction");
const Int32 HdmiCecFeatureAction::MSG_TIMEOUT = 100;
const Int32 HdmiCecFeatureAction::STATE_NONE = 0;

HdmiCecFeatureAction::HdmiCecFeatureAction()
    : mState(STATE_NONE)
{}

ECode HdmiCecFeatureAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mSource = source;
        mService = mSource.getService();
        mActionTimer = createActionTimer(mService.getServiceLooper());

#endif
}

ECode HdmiCecFeatureAction::SetActionTimer(
    /* [in] */ IHdmiCecFeatureActionActionTimer* actionTimer)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mActionTimer = actionTimer;

#endif
}

ECode HdmiCecFeatureAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecFeatureAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecFeatureAction::CreateActionTimer(
    /* [in] */ ILooper* looper,
    /* [out] */ IHdmiCecFeatureActionActionTimer** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return new ActionTimerHandler(looper);

#endif
}

ECode HdmiCecFeatureAction::AddTimer(
    /* [in] */ Int32 state,
    /* [in] */ Int32 delayMillis)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mActionTimer.sendTimerMessage(state, delayMillis);

#endif
}

ECode HdmiCecFeatureAction::Started(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mState != STATE_NONE;

#endif
}

ECode HdmiCecFeatureAction::SendCommand(
    /* [in] */ IHdmiCecMessage* cmd)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.sendCecCommand(cmd);

#endif
}

ECode HdmiCecFeatureAction::SendCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.sendCecCommand(cmd, callback);

#endif
}

ECode HdmiCecFeatureAction::AddAndStartAction(
    /* [in] */ HdmiCecFeatureAction* action)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mSource.addAndStartAction(action);

#endif
}

ECode HdmiCecFeatureAction::GetActions(
    /* [in] */ ClassID clazz,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSource.getActions(clazz);

#endif
}

ECode HdmiCecFeatureAction::GetCecMessageCache(
    /* [out] */ HdmiCecMessageCache** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSource.getCecMessageCache();

#endif
}

ECode HdmiCecFeatureAction::RemoveAction(
    /* [in] */ HdmiCecFeatureAction* action)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mSource.removeAction(action);

#endif
}

ECode HdmiCecFeatureAction::RemoveAction(
    /* [in] */ ClassID clazz,
    /* [out] */ HdmiCecFeatureAction** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mSource.removeActionExcept(clazz, NULL);

#endif
}

ECode HdmiCecFeatureAction::RemoveActionExcept(
    /* [in] */ ClassID clazz,
    /* [in] */ HdmiCecFeatureAction* exception,
    /* [out] */ HdmiCecFeatureAction** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mSource.removeActionExcept(clazz, exception);

#endif
}

ECode HdmiCecFeatureAction::PollDevices(
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ Int32 pickStrategy,
    /* [in] */ Int32 retryCount)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.pollDevices(callback, getSourceAddress(), pickStrategy, retryCount);

#endif
}

ECode HdmiCecFeatureAction::Clear()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_NONE;
        // Clear all timers.
        mActionTimer.clearTimerMessage();

#endif
}

ECode HdmiCecFeatureAction::Finish()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        finish(true);

#endif
}

ECode HdmiCecFeatureAction::Finish(
    /* [in] */ Boolean removeSelf)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        clear();
        if (removeSelf) {
            removeAction(this);
        }
        if (mOnFinishedCallbacks != NULL) {
            for (Pair<HdmiCecFeatureAction, Runnable> actionCallbackPair: mOnFinishedCallbacks) {
                if (actionCallbackPair.first.mState != STATE_NONE) {
                    actionCallbackPair.second.run();
                }
            }
            mOnFinishedCallbacks = NULL;
        }

#endif
}

ECode HdmiCecFeatureAction::LocalDevice(
    /* [out] */ IHdmiCecLocalDevice** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSource;

#endif
}

ECode HdmiCecFeatureAction::Playback(
    /* [out] */ HdmiCecLocalDevicePlayback** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (HdmiCecLocalDevicePlayback) mSource;

#endif
}

ECode HdmiCecFeatureAction::Tv(
    /* [out] */ HdmiCecLocalDeviceTv** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (HdmiCecLocalDeviceTv) mSource;

#endif
}

ECode HdmiCecFeatureAction::GetSourceAddress(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSource.getDeviceInfo().getLogicalAddress();

#endif
}

ECode HdmiCecFeatureAction::GetSourcePath(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mSource.getDeviceInfo().getPhysicalAddress();

#endif
}

ECode HdmiCecFeatureAction::SendUserControlPressedAndReleased(
    /* [in] */ Int32 targetAddress,
    /* [in] */ Int32 uiCommand)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mSource.sendUserControlPressedAndReleased(targetAddress, uiCommand);

#endif
}

ECode HdmiCecFeatureAction::AddOnFinishedCallback(
    /* [in] */ HdmiCecFeatureAction* action,
    /* [in] */ IRunnable* runnable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mOnFinishedCallbacks == NULL) {
            mOnFinishedCallbacks = new ArrayList<>();
        }
        mOnFinishedCallbacks.add(Pair.create(action, runnable));

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

