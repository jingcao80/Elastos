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

using Elastos::Droid::annotation.Nullable;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.SendMessageCallback;

/**
 * Feature action for routing control. Exchanges routing-related commands with other devices
 * to determine the new active source.
 *
 * <p>This action is initiated by various cases:
 * <ul>
 * <li> Manual TV input switching
 * <li> Routing change of a CEC switch other than TV
 * <li> New CEC device at the tail of the active routing path
 * <li> Removed CEC device from the active routing path
 * <li> Routing at CEC enable time
 * </ul>
 */
final class RoutingControlAction extends HdmiCecFeatureAction {
    private static const String TAG = "RoutingControlAction";

    // State in which we wait for <Routing Information> to arrive. If timed out, we use the
    // latest routing path to set the new active source.
    private static const Int32 STATE_WAIT_FOR_ROUTING_INFORMATION = 1;

    // State in which we wait for <Report Power Status> in response to <Give Device Power Status>
    // we have sent. If the response tells us the device power is on, we send <Set Stream Path>
    // to make it the active source. Otherwise we do not send <Set Stream Path>, and possibly
    // just show the blank screen.
    private static const Int32 STATE_WAIT_FOR_REPORT_POWER_STATUS = 2;

    // Time out in millseconds used for <Routing Information>
    private static const Int32 TIMEOUT_ROUTING_INFORMATION_MS = 1000;

    // Time out in milliseconds used for <Report Power Status>
    private static const Int32 TIMEOUT_REPORT_POWER_STATUS_MS = 1000;

    // TRUE if <Give Power Status> should be sent once the new active routing path is determined.
    private final Boolean mQueryDevicePowerStatus;

    // If set to TRUE, call {@link HdmiControlService#InvokeInputChangeListener()} when
    // the routing control/active source change happens. The listener should be called if
    // the events are triggered by external events such as manual switch port change or incoming
    // <Inactive Source> command.
    private final Boolean mNotifyInputChange;

    @Nullable private final IHdmiControlCallback mCallback;

    // The latest routing path. Updated by each <Routing Information> from CEC switches.
    private Int32 mCurrentRoutingPath;

    RoutingControlAction(HdmiCecLocalDevice localDevice, Int32 path, Boolean queryDevicePowerStatus,
            IHdmiControlCallback callback) {
        Super(localDevice);
        mCallback = callback;
        mCurrentRoutingPath = path;
        mQueryDevicePowerStatus = queryDevicePowerStatus;
        // Callback is non-NULL when routing control action is brought up by binder API. Use
        // this as an indicator for the input change notification. These API calls will get
        // the result through this callback, not through notification. Any other events that
        // trigger the routing control is external, for which notifcation is used.
        mNotifyInputChange = (callback == NULL);
    }

    //@Override
    public Boolean Start() {
        mState = STATE_WAIT_FOR_ROUTING_INFORMATION;
        AddTimer(mState, TIMEOUT_ROUTING_INFORMATION_MS);
        return TRUE;
    }

    //@Override
    public Boolean ProcessCommand(HdmiCecMessage cmd) {
        Int32 opcode = cmd->GetOpcode();
        Byte[] params = cmd->GetParams();
        if (mState == STATE_WAIT_FOR_ROUTING_INFORMATION
                && opcode == Constants.MESSAGE_ROUTING_INFORMATION) {
            // Keep updating the physicalAddress as we receive <Routing Information>.
            // If the routing path doesn't belong to the currently active one, we should
            // ignore it since it might have come from other routing change sequence.
            Int32 routingPath = HdmiUtils->TwoBytesToInt(params);
            if (!HdmiUtils->IsInActiveRoutingPath(mCurrentRoutingPath, routingPath)) {
                return TRUE;
            }
            mCurrentRoutingPath = routingPath;
            // Stop possible previous routing change sequence if in progress.
            RemoveActionExcept(RoutingControlAction.class, this);
            AddTimer(mState, TIMEOUT_ROUTING_INFORMATION_MS);
            return TRUE;
        } else if (mState == STATE_WAIT_FOR_REPORT_POWER_STATUS
                  && opcode == Constants.MESSAGE_REPORT_POWER_STATUS) {
            HandleReportPowerStatus(cmd->GetParams()[0]);
            return TRUE;
        }
        return FALSE;
    }

    private void HandleReportPowerStatus(Int32 devicePowerStatus) {
        if (IsPowerOnOrTransient(GetTvPowerStatus())) {
            if (IsPowerOnOrTransient(devicePowerStatus)) {
                SendSetStreamPath();
            } else {
                Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
            }
        }
        FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
    }

    private Int32 GetTvPowerStatus() {
        return Tv()->GetPowerStatus();
    }

    private static Boolean IsPowerOnOrTransient(Int32 status) {
        return status == HdmiControlManager.POWER_STATUS_ON
                || status == HdmiControlManager.POWER_STATUS_TRANSIENT_TO_ON;
    }

    private void SendSetStreamPath() {
        SendCommand(HdmiCecMessageBuilder->BuildSetStreamPath(GetSourceAddress(),
                mCurrentRoutingPath));
    }

    private void FinishWithCallback(Int32 result) {
        InvokeCallback(result);
        Finish();
    }

    //@Override
    CARAPI HandleTimerEvent(Int32 timeoutState) {
        if (mState != timeoutState || mState == STATE_NONE) {
            Slogger::W("CEC", "Timer in a wrong state. Ignored.");
            return;
        }
        switch (timeoutState) {
            case STATE_WAIT_FOR_ROUTING_INFORMATION:
                HdmiDeviceInfo device = Tv()->GetDeviceInfoByPath(mCurrentRoutingPath);
                if (device != NULL && mQueryDevicePowerStatus) {
                    Int32 deviceLogicalAddress = device->GetLogicalAddress();
                    QueryDevicePowerStatus(deviceLogicalAddress, new SendMessageCallback() {
                        //@Override
                        CARAPI OnSendCompleted(Int32 error) {
                            HandlDevicePowerStatusAckResult(
                                    error == HdmiControlManager.RESULT_SUCCESS);
                        }
                    });
                } else {
                    Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
                    FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
                }
                return;
            case STATE_WAIT_FOR_REPORT_POWER_STATUS:
                if (IsPowerOnOrTransient(GetTvPowerStatus())) {
                    Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
                    SendSetStreamPath();
                }
                FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
                return;
        }
    }

    private void QueryDevicePowerStatus(Int32 address, SendMessageCallback callback) {
        SendCommand(HdmiCecMessageBuilder->BuildGiveDevicePowerStatus(GetSourceAddress(), address),
                callback);
    }

    private void HandlDevicePowerStatusAckResult(Boolean acked) {
        if (acked) {
            mState = STATE_WAIT_FOR_REPORT_POWER_STATUS;
            AddTimer(mState, TIMEOUT_REPORT_POWER_STATUS_MS);
        } else {
            Tv()->UpdateActiveInput(mCurrentRoutingPath, mNotifyInputChange);
            SendSetStreamPath();
            FinishWithCallback(HdmiControlManager.RESULT_SUCCESS);
        }
    }

    private void InvokeCallback(Int32 result) {
        if (mCallback == NULL) {
            return;
        }
        try {
            mCallback->OnComplete(result);
        } catch (RemoteException e) {
            // Do nothing.
        }
    }
}
