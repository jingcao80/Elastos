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

using Elastos::Droid::Utility::ISparseArray;

/**
 * This class handles the incoming messages when HdmiCecService is in the standby mode.
 */
public class HdmiCecStandbyModeHandler {

    private interface CecMessageHandler {
        Boolean Handle(HdmiCecMessage message);
    }

    private static const class Bystander implements CecMessageHandler {
        //@Override
        public Boolean Handle(HdmiCecMessage message) {
            return TRUE;
        }
    }

    private static const class Bypasser implements CecMessageHandler {
        //@Override
        public Boolean Handle(HdmiCecMessage message) {
            return FALSE;
        }
    }

    private final class Aborter implements CecMessageHandler {
        private final Int32 mReason;
        public Aborter(Int32 reason) {
            mReason = reason;
        }
        //@Override
        public Boolean Handle(HdmiCecMessage message) {
            mService->MaySendFeatureAbortCommand(message, mReason);
            return TRUE;
        }
    }

    private final class AutoOnHandler implements CecMessageHandler {
        //@Override
        public Boolean Handle(HdmiCecMessage message) {
            if (!mTv->GetAutoWakeup()) {
                mAborterRefused->Handle(message);
                return TRUE;
            }
            return FALSE;
        }
    }

    private final class UserControlProcessedHandler implements CecMessageHandler {
        //@Override
        public Boolean Handle(HdmiCecMessage message) {
            // The power status here is always standby.
            if (HdmiCecLocalDevice->IsPowerOnOrToggleCommand(message)) {
                return FALSE;
            } else if (HdmiCecLocalDevice->IsPowerOffOrToggleCommand(message)) {
                return TRUE;
            }
            return mAborterIncorrectMode->Handle(message);
        }
    }

    private final HdmiControlService mService;
    private final HdmiCecLocalDeviceTv mTv;

    private final SparseArray<CecMessageHandler> mCecMessageHandlers = new SparseArray<>();
    private final CecMessageHandler mDefaultHandler = new Aborter(
            Constants.ABORT_UNRECOGNIZED_OPCODE);
    private final CecMessageHandler mAborterIncorrectMode = new Aborter(
            Constants.ABORT_NOT_IN_CORRECT_MODE);
    private final CecMessageHandler mAborterRefused = new Aborter(Constants.ABORT_REFUSED);
    private final CecMessageHandler mAutoOnHandler = new AutoOnHandler();
    private final CecMessageHandler mBypasser = new Bypasser();
    private final CecMessageHandler mBystander = new Bystander();
    private final UserControlProcessedHandler
            mUserControlProcessedHandler = new UserControlProcessedHandler();

    public HdmiCecStandbyModeHandler(HdmiControlService service, HdmiCecLocalDeviceTv tv) {
        mService = service;
        mTv = tv;

        AddHandler(Constants.MESSAGE_IMAGE_VIEW_ON, mAutoOnHandler);
        AddHandler(Constants.MESSAGE_TEXT_VIEW_ON, mAutoOnHandler);

        AddHandler(Constants.MESSAGE_ACTIVE_SOURCE, mBystander);
        AddHandler(Constants.MESSAGE_REQUEST_ACTIVE_SOURCE, mBystander);
        AddHandler(Constants.MESSAGE_ROUTING_CHANGE, mBystander);
        AddHandler(Constants.MESSAGE_ROUTING_INFORMATION, mBystander);
        AddHandler(Constants.MESSAGE_SET_STREAM_PATH, mBystander);
        AddHandler(Constants.MESSAGE_STANDBY, mBystander);
        AddHandler(Constants.MESSAGE_SET_MENU_LANGUAGE, mBystander);
        AddHandler(Constants.MESSAGE_DEVICE_VENDOR_ID, mBystander);
        AddHandler(Constants.MESSAGE_USER_CONTROL_RELEASED, mBystander);
        AddHandler(Constants.MESSAGE_REPORT_POWER_STATUS, mBystander);
        AddHandler(Constants.MESSAGE_FEATURE_ABORT, mBystander);
        AddHandler(Constants.MESSAGE_INACTIVE_SOURCE, mBystander);
        AddHandler(Constants.MESSAGE_SYSTEM_AUDIO_MODE_STATUS, mBystander);
        AddHandler(Constants.MESSAGE_REPORT_AUDIO_STATUS, mBystander);

        // If TV supports the following messages during power-on, ignore them and do nothing,
        // else reply with <Feature Abort>["Unrecognized Opcode"]
        // <Deck Status>, <Tuner Device Status>, <Tuner Cleared Status>, <Timer Status>
        AddHandler(Constants.MESSAGE_RECORD_STATUS, mBystander);

        // If TV supports the following messages during power-on, reply with <Feature Abort>["Not
        // in correct mode to respond"], else reply with <Feature Abort>["Unrecognized Opcode"]
        // <Give Tuner Device Status>, <Select Digital Service>, <Tuner Step Decrement>,
        // <Tuner Stem Increment>, <Menu Status>.
        AddHandler(Constants.MESSAGE_RECORD_TV_SCREEN, mAborterIncorrectMode);
        AddHandler(Constants.MESSAGE_INITIATE_ARC, mAborterIncorrectMode);
        AddHandler(Constants.MESSAGE_TERMINATE_ARC, mAborterIncorrectMode);

        AddHandler(Constants.MESSAGE_GIVE_PHYSICAL_ADDRESS, mBypasser);
        AddHandler(Constants.MESSAGE_GET_MENU_LANGUAGE, mBypasser);
        AddHandler(Constants.MESSAGE_REPORT_PHYSICAL_ADDRESS, mBypasser);
        AddHandler(Constants.MESSAGE_GIVE_DEVICE_VENDOR_ID, mBypasser);
        AddHandler(Constants.MESSAGE_GIVE_OSD_NAME, mBypasser);
        AddHandler(Constants.MESSAGE_SET_OSD_NAME, mBypasser);

        AddHandler(Constants.MESSAGE_USER_CONTROL_PRESSED, mUserControlProcessedHandler);

        AddHandler(Constants.MESSAGE_GIVE_DEVICE_POWER_STATUS, mBypasser);
        AddHandler(Constants.MESSAGE_ABORT, mBypasser);
        AddHandler(Constants.MESSAGE_GET_CEC_VERSION, mBypasser);

        AddHandler(Constants.MESSAGE_VENDOR_COMMAND_WITH_ID, mAborterIncorrectMode);
        AddHandler(Constants.MESSAGE_SET_SYSTEM_AUDIO_MODE, mAborterIncorrectMode);
    }

    private void AddHandler(Int32 opcode, CecMessageHandler handler) {
        mCecMessageHandlers->Put(opcode, handler);
    }

    /**
     * Handles the CEC message in the standby mode.
     *
     * @param message {@link HdmiCecMessage} to be processed
     * @return TRUE if the message is handled in the handler, FALSE means that the message is need
     *         to be dispatched to the local device.
     */
    Boolean HandleCommand(HdmiCecMessage message) {
        CecMessageHandler handler = mCecMessageHandlers->Get(message->GetOpcode());
        if (handler != NULL) {
            return handler->Handle(message);
        }
        return mDefaultHandler->Handle(message);
    }
}
