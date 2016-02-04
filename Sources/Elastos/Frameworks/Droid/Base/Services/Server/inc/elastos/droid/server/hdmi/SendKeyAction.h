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

import static com.android.server.hdmi.Constants.IRT_MS;

using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IKeyEvent;

/**
 * Feature action that transmits remote control key command (User Control Press/
 * User Control Release) to CEC bus.
 *
 * <p>This action is created when a new key event is passed to CEC service. It optionally
 * does key repeat (a.k.a. press-and-hold) operation until it receives a key release event.
 * If another key press event is received before the key in use is released, CEC service
 * does not create a new action but recycles the current one by updating the key used
 * for press-and-hold operation.
 *
 * <p>Package-private, accessed by {@link HdmiControlService} only.
 */
final class SendKeyAction extends HdmiCecFeatureAction {
    private static const String TAG = "SendKeyAction";

    // State in which the action is at work. The state is set in {@link #Start()} and
    // persists throughout the process till it is set back to {@code STATE_NONE} at the end.
    private static const Int32 STATE_PROCESSING_KEYCODE = 1;

    // Logical address of the device to which the UCP/UCP commands are sent.
    private final Int32 mTargetAddress;

    // The key code of the last key press event the action is passed via processKeyEvent.
    private Int32 mLastKeycode;

    /**
     * Constructor.
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param targetAddress logical address of the device to send the keys to
     * @param keycode remote control key code as defined in {@link KeyEvent}
     */
    SendKeyAction(HdmiCecLocalDevice source, Int32 targetAddress, Int32 keycode) {
        Super(source);
        mTargetAddress = targetAddress;
        mLastKeycode = keycode;
    }

    //@Override
    public Boolean Start() {
        SendKeyDown(mLastKeycode);
        // finish action for non-repeatable key.
        if (!HdmiCecKeycode->IsRepeatableKey(mLastKeycode)) {
            SendKeyUp();
            Finish();
            return TRUE;
        }
        mState = STATE_PROCESSING_KEYCODE;
        AddTimer(mState, IRT_MS);
        return TRUE;
    }

    /**
     * Called when a key event should be handled for the action.
     *
     * @param keycode key code of {@link KeyEvent} object
     * @param isPressed TRUE if the key event is of {@link KeyEvent#ACTION_DOWN}
     */
    void ProcessKeyEvent(Int32 keycode, Boolean isPressed) {
        if (mState != STATE_PROCESSING_KEYCODE) {
            Slogger::W(TAG, "Not in a valid state");
            return;
        }
        // A new key press event that comes in with a key code different from the last
        // one sets becomes a new key code to be used for press-and-hold operation.
        // Removes any pending timer and starts a new timer for itself.
        // Key release event indicates that the action shall be finished. Send UCR
        // command and terminate the action. Other release events are ignored.
        if (isPressed) {
            if (keycode != mLastKeycode) {
                SendKeyDown(keycode);
                if (!HdmiCecKeycode->IsRepeatableKey(keycode)) {
                    SendKeyUp();
                    Finish();
                    return;
                }
                mActionTimer->ClearTimerMessage();
                AddTimer(mState, IRT_MS);
                mLastKeycode = keycode;
            }
        } else {
            if (keycode == mLastKeycode) {
                SendKeyUp();
                Finish();
            }
        }
    }

    private void SendKeyDown(Int32 keycode) {
        Int32 cecKeycode = HdmiCecKeycode->AndroidKeyToCecKey(keycode);
        if (cecKeycode == HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            return;
        }
        SendCommand(HdmiCecMessageBuilder->BuildUserControlPressed(GetSourceAddress(),
                mTargetAddress, new Byte[] { (Byte) (cecKeycode & 0xFF) }));
    }

    private void SendKeyUp() {
        SendCommand(HdmiCecMessageBuilder->BuildUserControlReleased(GetSourceAddress(),
                mTargetAddress));
    }

    //@Override
    public Boolean ProcessCommand(HdmiCecMessage cmd) {
        // Send key action doesn't need any incoming CEC command, hence does not consume it.
        return FALSE;
    }

    //@Override
    CARAPI HandleTimerEvent(Int32 state) {
        // Timer event occurs every IRT_MS milliseconds to perform key-repeat (or press-and-hold)
        // operation. If the last received key code is as same as the one with which the action
        // is started, plus there was no key release event in last IRT_MS timeframe, send a UCP
        // command and start another timer to schedule the next press-and-hold command.
        if (mState != STATE_PROCESSING_KEYCODE) {
            Slogger::W(TAG, "Not in a valid state");
            return;
        }
        SendKeyDown(mLastKeycode);
        AddTimer(mState, IRT_MS);
    }
}
