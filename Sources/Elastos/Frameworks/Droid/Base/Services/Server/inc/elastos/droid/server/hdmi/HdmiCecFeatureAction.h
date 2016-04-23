
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECFEATUREACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECFEATUREACTION_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceDevicePollingCallback;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecMessageCache;
class HdmiCecLocalDevicePlayback;
class HdmiCecLocalDeviceTv;
class HdmiControlService;
/**
 * Encapsulates a sequence of CEC command exchange for a certain feature.
 * <p>
 * Many CEC features are accomplished by CEC devices on the bus exchanging more than one
 * command. {@link HdmiCecFeatureAction} represents the life cycle of the communication, manages the
 * state as the process progresses, and if necessary, returns the result to the caller which
 * initiates the action, through the callback given at the creation of the object. All the actual
 * action classes inherit FeatureAction.
 * <p>
 * More than one FeatureAction objects can be up and running simultaneously, maintained by
 * {@link HdmiCecLocalDevice}. Each action is passed a new command arriving from the bus, and either
 * consumes it if the command is what the action expects, or yields it to other action. Declared as
 * package private, accessed by {@link HdmiControlService} only.
 */
class HdmiCecFeatureAction
    : public Object
    , public IHdmiCecFeatureAction
{
private:
    class ActionTimerHandler
        : public Handler
        , public IHdmiCecFeatureActionActionTimer
    {
    public:
        TO_STRING_IMPL("HdmiCecFeatureAction::ActionTimerHandler")

        CAR_INTERFACE_DECL()

        ActionTimerHandler(
            /* [in] */ HdmiCecFeatureAction* host);

        CARAPI constructor(
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI SendTimerMessage(
            /* [in] */ Int32 state,
            /* [in] */ Int64 delayMillis);

        // @Override
        CARAPI ClearTimerMessage();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        HdmiCecFeatureAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiCecFeatureAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source);

    // @VisibleForTesting
    CARAPI SetActionTimer(
        /* [in] */ IHdmiCecFeatureActionActionTimer* actionTimer);

    /**
     * Called after the action is created. Initialization or first step to take
     * for the action can be done in this method. Shall update {@code mState} to
     * indicate that the action has started.
     *
     * @return true if the operation is successful; otherwise false.
     */
    virtual CARAPI Start(
        /* [out] */ Boolean* result) = 0;

    /**
     * Process the command. Called whenever a new command arrives.
     *
     * @param cmd command to process
     * @return true if the command was consumed in the process; Otherwise false, which
     *          indicates that the command shall be handled by other actions.
     */
    virtual CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result) = 0;

    /**
     * Called when the action should handle the timer event it created before.
     *
     * <p>CEC standard mandates each command transmission should be responded within
     * certain period of time. The method is called when the timer it created as it transmitted
     * a command gets expired. Inner logic should take an appropriate action.
     *
     * @param state the state associated with the time when the timer was created
     */
    virtual CARAPI HandleTimerEvent(
        /* [in] */ Int32 state) = 0;

    // delayMillis.
    CARAPI AddTimer(
        /* [in] */ Int32 state,
        /* [in] */ Int32 delayMillis);

    CARAPI Started(
        /* [out] */ Boolean* result);

    CARAPI SendCommand(
        /* [in] */ IHdmiCecMessage* cmd);

    CARAPI SendCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [in] */ IHdmiControlServiceSendMessageCallback* callback);

    CARAPI AddAndStartAction(
        /* [in] */ HdmiCecFeatureAction* action);

    CARAPI GetActions(
        /* [in] */ ClassID clazz,
        /* [out] */ IList** result);

    CARAPI GetCecMessageCache(
        /* [out] */ HdmiCecMessageCache** result);

    /**
     * Remove the action from the action queue. This is called after the action finishes
     * its role.
     *
     * @param action
     */
    CARAPI RemoveAction(
        /* [in] */ HdmiCecFeatureAction* action);

    CARAPI RemoveAction(
        /* [in] */ ClassID clazz);

    CARAPI RemoveActionExcept(
        /* [in] */ ClassID clazz,
        /* [in] */ HdmiCecFeatureAction* exception);

    CARAPI PollDevices(
        /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
        /* [in] */ Int32 pickStrategy,
        /* [in] */ Int32 retryCount);

    /**
     * Clean up action's state.
     *
     * <p>Declared as package-private. Only {@link HdmiControlService} can access it.
     */
    CARAPI Clear();

    /**
     * Finish up the action. Reset the state, and remove itself from the action queue.
     */
    CARAPI Finish();

    CARAPI Finish(
        /* [in] */ Boolean removeSelf);

    CARAPI LocalDevice(
        /* [out] */ IHdmiCecLocalDevice** result);

    CARAPI Playback(
        /* [out] */ HdmiCecLocalDevicePlayback** result);

    CARAPI Tv(
        /* [out] */ IHdmiCecLocalDeviceTv** result);

    CARAPI GetSourceAddress(
        /* [out] */ Int32* result);

    CARAPI GetSourcePath(
        /* [out] */ Int32* result);

    CARAPI SendUserControlPressedAndReleased(
        /* [in] */ Int32 targetAddress,
        /* [in] */ Int32 uiCommand);

    CARAPI AddOnFinishedCallback(
        /* [in] */ HdmiCecFeatureAction* action,
        /* [in] */ IRunnable* runnable);

private:
    CARAPI CreateActionTimer(
        /* [in] */ ILooper* looper,
        /* [out] */ IHdmiCecFeatureActionActionTimer** result);

public:
    // Timer handler message used for timeout event
    static const Int32 MSG_TIMEOUT;

    // Default state used in common by all the feature actions.
    static const Int32 STATE_NONE;

    // Internal state indicating the progress of action.
    Int32 mState;

    // Timer that manages timeout events.
    AutoPtr<IHdmiCecFeatureActionActionTimer> mActionTimer;

private:
    static const String TAG;

    AutoPtr<IHdmiControlService> mService;

    AutoPtr<IHdmiCecLocalDevice> mSource;

    AutoPtr<IArrayList> mOnFinishedCallbacks;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECFEATUREACTION_H__
