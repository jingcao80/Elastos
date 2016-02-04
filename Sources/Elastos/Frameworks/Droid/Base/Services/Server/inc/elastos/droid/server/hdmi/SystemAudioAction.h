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

using Elastos::Utility::IList;

/**
 * Base feature action class for SystemAudioActionFromTv and SystemAudioActionFromAvr.
 */
abstract class SystemAudioAction extends HdmiCecFeatureAction {
    private static const String TAG = "SystemAudioAction";

    // Transient state to differentiate with STATE_NONE where the on-finished callback
    // will not be called.
    private static const Int32 STATE_CHECK_ROUTING_IN_PRGRESS = 1;

    // State in which waits for <SetSystemAudioMode>.
    private static const Int32 STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE = 2;

    private static const Int32 MAX_SEND_RETRY_COUNT = 2;

    private static const Int32 ON_TIMEOUT_MS = 5000;
    private static const Int32 OFF_TIMEOUT_MS = HdmiConfig.TIMEOUT_MS;

    // Logical address of AV Receiver.
    protected final Int32 mAvrLogicalAddress;

    // The target audio status of the action, whether to enable the system audio mode or not.
    protected Boolean mTargetAudioStatus;

    @Nullable private final IHdmiControlCallback mCallback;

    private Int32 mSendRetryCount = 0;

    /**
     * Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param avrAddress logical address of AVR device
     * @param targetStatus Whether to enable the system audio mode or not
     * @param callback callback interface to be notified when it's done
     * @throw IllegalArugmentException if device type of sourceAddress and avrAddress is invalid
     */
    SystemAudioAction(HdmiCecLocalDevice source, Int32 avrAddress, Boolean targetStatus,
            IHdmiControlCallback callback) {
        Super(source);
        HdmiUtils->VerifyAddressType(avrAddress, HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM);
        mAvrLogicalAddress = avrAddress;
        mTargetAudioStatus = targetStatus;
        mCallback = callback;
    }

    // Seq #27
    protected void SendSystemAudioModeRequest() {
        List<RoutingControlAction> routingActions = GetActions(RoutingControlAction.class);
        if (!routingActions->IsEmpty()) {
            mState = STATE_CHECK_ROUTING_IN_PRGRESS;
            // Should have only one Routing Control Action
            RoutingControlAction routingAction = routingActions->Get(0);
            routingAction->AddOnFinishedCallback(this, new Runnable() {
                //@Override
                CARAPI Run() {
                    SendSystemAudioModeRequestInternal();
                }
            });
            return;
        }
        SendSystemAudioModeRequestInternal();
    }

    private void SendSystemAudioModeRequestInternal() {
        Int32 avrPhysicalAddress = Tv()->GetAvrDeviceInfo().GetPhysicalAddress();
        HdmiCecMessage command = HdmiCecMessageBuilder->BuildSystemAudioModeRequest(
                GetSourceAddress(),
                mAvrLogicalAddress, avrPhysicalAddress, mTargetAudioStatus);
        SendCommand(command, new HdmiControlService->SendMessageCallback() {
            //@Override
            CARAPI OnSendCompleted(Int32 error) {
                if (error != Constants.SEND_RESULT_SUCCESS) {
                    HdmiLogger->Debug("Failed to send <System Audio Mode Request>:" + error);
                    SetSystemAudioMode(FALSE);
                    FinishWithCallback(HdmiControlManager.RESULT_COMMUNICATION_FAILED);
                }
            }
        });
        mState = STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE;
        AddTimer(mState, mTargetAudioStatus ? ON_TIMEOUT_MS : OFF_TIMEOUT_MS);
    }

    private void HandleSendSystemAudioModeRequestTimeout() {
        if (!mTargetAudioStatus  // Don't retry for Off case.
                || mSendRetryCount++ >= MAX_SEND_RETRY_COUNT) {
            HdmiLogger->Debug("[T]:wait for <Set System Audio Mode>.");
            SetSystemAudioMode(FALSE);
            FinishWithCallback(HdmiControlManager.RESULT_TIMEOUT);
            return;
        }
        SendSystemAudioModeRequest();
    }

    protected void SetSystemAudioMode(Boolean mode) {
        Tv()->SetSystemAudioMode(mode, TRUE);
    }

    //@Override
    final Boolean ProcessCommand(HdmiCecMessage cmd) {
        switch (mState) {
            case STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE:
                if (cmd->GetOpcode() == Constants.MESSAGE_FEATURE_ABORT
                        && (cmd->GetParams()[0] & 0xFF)
                                == Constants.MESSAGE_SYSTEM_AUDIO_MODE_REQUEST) {
                    HdmiLogger->Debug("Failed to start system audio mode request.");
                    SetSystemAudioMode(FALSE);
                    FinishWithCallback(HdmiControlManager.RESULT_EXCEPTION);
                    return TRUE;
                }
                if (cmd->GetOpcode() != Constants.MESSAGE_SET_SYSTEM_AUDIO_MODE
                        || !HdmiUtils->CheckCommandSource(cmd, mAvrLogicalAddress, TAG)) {
                    return FALSE;
                }
                Boolean receivedStatus = HdmiUtils->ParseCommandParamSystemAudioStatus(cmd);
                if (receivedStatus == mTargetAudioStatus) {
                    SetSystemAudioMode(receivedStatus);
                    StartAudioStatusAction();
                    return TRUE;
                } else {
                    HdmiLogger->Debug("Unexpected system audio mode request:" + receivedStatus);
                    // Unexpected response, consider the request is newly initiated by AVR.
                    // To return 'FALSE' will initiate new SystemAudioActionFromAvr by the control
                    // service.
                    FinishWithCallback(HdmiControlManager.RESULT_EXCEPTION);
                    return FALSE;
                }
            default:
                return FALSE;
        }
    }

    protected void StartAudioStatusAction() {
        AddAndStartAction(new SystemAudioStatusAction(Tv(), mAvrLogicalAddress, mCallback));
        Finish();
    }

    protected void RemoveSystemAudioActionInProgress() {
        RemoveActionExcept(SystemAudioActionFromTv.class, this);
        RemoveActionExcept(SystemAudioActionFromAvr.class, this);
    }

    //@Override
    final void HandleTimerEvent(Int32 state) {
        if (mState != state) {
            return;
        }
        switch (mState) {
            case STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE:
                HandleSendSystemAudioModeRequestTimeout();
                return;
        }
    }

    // TODO: if IHdmiControlCallback is general to other FeatureAction,
    //       move it into FeatureAction.
    protected void FinishWithCallback(Int32 returnCode) {
        if (mCallback != NULL) {
            try {
                mCallback->OnComplete(returnCode);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Failed to invoke callback.", e);
            }
        }
        Finish();
    }
}
