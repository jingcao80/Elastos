/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.server.hdmi;

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::internal.annotations.VisibleForTesting;
using Elastos::Droid::Server::Ihdmi.HdmiControlService.DevicePollingCallback;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

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
abstract class HdmiCecFeatureAction {
    private static const String TAG = "HdmiCecFeatureAction";

    // Timer handler message used for timeout event
    protected static const Int32 MSG_TIMEOUT = 100;

    // Default state used in common by all the feature actions.
    protected static const Int32 STATE_NONE = 0;

    // Internal state indicating the progress of action.
    protected Int32 mState = STATE_NONE;

    private final HdmiControlService mService;
    private final HdmiCecLocalDevice mSource;

    // Timer that manages timeout events.
    protected ActionTimer mActionTimer;

    private ArrayList<Pair<HdmiCecFeatureAction, Runnable>> mOnFinishedCallbacks;

    HdmiCecFeatureAction(HdmiCecLocalDevice source) {
        mSource = source;
        mService = mSource->GetService();
        mActionTimer = CreateActionTimer(mService->GetServiceLooper());
    }

    @VisibleForTesting
    void SetActionTimer(ActionTimer actionTimer) {
        mActionTimer = actionTimer;
    }

    /**
     * Called after the action is created. Initialization or first step to take
     * for the action can be done in this method. Shall update {@code mState} to
     * indicate that the action has started.
     *
     * @return TRUE if the operation is successful; otherwise FALSE.
     */
    abstract Boolean Start();

    /**
     * Process the command. Called whenever a new command arrives.
     *
     * @param cmd command to process
     * @return TRUE if the command was consumed in the process; Otherwise FALSE, which
     *          indicates that the command shall be handled by other actions.
     */
    abstract Boolean ProcessCommand(HdmiCecMessage cmd);

    /**
     * Called when the action should handle the timer event it created before.
     *
     * <p>CEC standard mandates each command transmission should be responded within
     * certain period of time. The method is called when the timer it created as it transmitted
     * a command gets expired. Inner logic should take an appropriate action.
     *
     * @param state the state associated with the time when the timer was created
     */
    abstract void HandleTimerEvent(Int32 state);

    /**
     * Timer handler interface used for FeatureAction classes.
     */
    interface ActionTimer {
        /**
         * Send a timer message.
         *
         * Also carries the state of the action when the timer is created. Later this state is
         * compared to the one the action is in when it receives the timer to let the action tell
         * the right timer to handle.
         *
         * @param state state of the action is in
         * @param delayMillis amount of delay for the timer
         */
        void SendTimerMessage(Int32 state, Int64 delayMillis);

        /**
         * Removes any pending timer message.
         */
        void ClearTimerMessage();
    }

    private class ActionTimerHandler extends Handler implements ActionTimer {

        public ActionTimerHandler(Looper looper) {
            Super(looper);
        }

        //@Override
        CARAPI SendTimerMessage(Int32 state, Int64 delayMillis) {
            // The third Argument(0) is not used.
            SendMessageDelayed(ObtainMessage(MSG_TIMEOUT, state, 0), delayMillis);
        }

        //@Override
        CARAPI ClearTimerMessage() {
            RemoveMessages(MSG_TIMEOUT);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
            case MSG_TIMEOUT:
                HandleTimerEvent(msg.arg1);
                break;
            default:
                Slogger::W(TAG, "Unsupported message:" + msg.what);
                break;
            }
        }
    }

    private ActionTimer CreateActionTimer(Looper looper) {
        return new ActionTimerHandler(looper);
    }

    // Add a new timer. The timer event will come to mActionTimer->HandleMessage() in
    // delayMillis.
    protected void AddTimer(Int32 state, Int32 delayMillis) {
        mActionTimer->SendTimerMessage(state, delayMillis);
    }

    Boolean Started() {
        return mState != STATE_NONE;
    }

    protected final void SendCommand(HdmiCecMessage cmd) {
        mService->SendCecCommand(cmd);
    }

    protected final void SendCommand(HdmiCecMessage cmd,
            HdmiControlService.SendMessageCallback callback) {
        mService->SendCecCommand(cmd, callback);
    }

    protected final void AddAndStartAction(HdmiCecFeatureAction action) {
        mSource->AddAndStartAction(action);
    }

    protected final <T extends HdmiCecFeatureAction> List<T> GetActions(final Class<T> clazz) {
        return mSource->GetActions(clazz);
    }

    protected final HdmiCecMessageCache GetCecMessageCache() {
        return mSource->GetCecMessageCache();
    }

    /**
     * Remove the action from the action queue. This is called after the action finishes
     * its role.
     *
     * @param action
     */
    protected final void RemoveAction(HdmiCecFeatureAction action) {
        mSource->RemoveAction(action);
    }

    protected final <T extends HdmiCecFeatureAction> void RemoveAction(final Class<T> clazz) {
        mSource->RemoveActionExcept(clazz, NULL);
    }

    protected final <T extends HdmiCecFeatureAction> void RemoveActionExcept(final Class<T> clazz,
            final HdmiCecFeatureAction exception) {
        mSource->RemoveActionExcept(clazz, exception);
    }

    protected final void PollDevices(DevicePollingCallback callback, Int32 pickStrategy,
            Int32 retryCount) {
        mService->PollDevices(callback, GetSourceAddress(), pickStrategy, retryCount);
    }

    /**
     * Clean up action's state.
     *
     * <p>Declared as package-private. Only {@link HdmiControlService} can access it.
     */
    void Clear() {
        mState = STATE_NONE;
        // Clear all timers.
        mActionTimer->ClearTimerMessage();
    }

    /**
     * Finish up the action. Reset the state, and remove itself from the action queue.
     */
    protected void Finish() {
        Finish(TRUE);
    }

    void Finish(Boolean removeSelf) {
        Clear();
        if (removeSelf) {
            RemoveAction(this);
        }
        if (mOnFinishedCallbacks != NULL) {
            for (Pair<HdmiCecFeatureAction, Runnable> actionCallbackPair: mOnFinishedCallbacks) {
                if (actionCallbackPair.first.mState != STATE_NONE) {
                    actionCallbackPair.second->Run();
                }
            }
            mOnFinishedCallbacks = NULL;
        }
    }

    protected final HdmiCecLocalDevice LocalDevice() {
        return mSource;
    }

    protected final HdmiCecLocalDevicePlayback Playback() {
        return (HdmiCecLocalDevicePlayback) mSource;
    }

    protected final HdmiCecLocalDeviceTv Tv() {
        return (HdmiCecLocalDeviceTv) mSource;
    }

    protected final Int32 GetSourceAddress() {
        return mSource->GetDeviceInfo()->GetLogicalAddress();
    }

    protected final Int32 GetSourcePath() {
        return mSource->GetDeviceInfo()->GetPhysicalAddress();
    }

    protected final void SendUserControlPressedAndReleased(Int32 targetAddress, Int32 uiCommand) {
        mSource->SendUserControlPressedAndReleased(targetAddress, uiCommand);
    }

    protected final void AddOnFinishedCallback(HdmiCecFeatureAction action, Runnable runnable) {
        if (mOnFinishedCallbacks == NULL) {
            mOnFinishedCallbacks = new ArrayList<>();
        }
        mOnFinishedCallbacks->Add(Pair->Create(action, runnable));
    }
}
