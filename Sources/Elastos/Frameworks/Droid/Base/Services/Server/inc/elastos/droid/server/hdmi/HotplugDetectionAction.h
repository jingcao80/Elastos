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

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Server::Ihdmi.HdmiControlService.DevicePollingCallback;

using Elastos::Utility::IBitSet;
using Elastos::Utility::IList;

/**
 * Feature action that handles hot-plug detection mechanism.
 * Hot-plug event is initiated by timer after device discovery action.
 *
 * <p>Check all devices every 15 secs except for system audio.
 * If system audio is on, check hot-plug for audio system every 5 secs.
 * For other devices, keep 15 secs period.
 */
// Seq #3
final class HotplugDetectionAction extends HdmiCecFeatureAction {
    private static const String TAG = "HotPlugDetectionAction";

    private static const Int32 POLLING_INTERVAL_MS = 5000;
    private static const Int32 TIMEOUT_COUNT = 3;

    // State in which waits for next polling
    private static const Int32 STATE_WAIT_FOR_NEXT_POLLING = 1;

    // All addresses except for broadcast (unregistered address).
    private static const Int32 NUM_OF_ADDRESS = Constants.ADDR_SPECIFIC_USE
            - Constants.ADDR_TV + 1;

    private Int32 mTimeoutCount = 0;

    /**
     * Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     */
    HotplugDetectionAction(HdmiCecLocalDevice source) {
        Super(source);
    }

    //@Override
    Boolean Start() {
        Slogger::V(TAG, "Hot-plug dection started.");

        mState = STATE_WAIT_FOR_NEXT_POLLING;
        mTimeoutCount = 0;

        // Start timer without polling.
        // The first check for all devices will be initiated 15 seconds later.
        AddTimer(mState, POLLING_INTERVAL_MS);
        return TRUE;
    }

    //@Override
    Boolean ProcessCommand(HdmiCecMessage cmd) {
        // No-op
        return FALSE;
    }

    //@Override
    void HandleTimerEvent(Int32 state) {
        if (mState != state) {
            return;
        }

        if (mState == STATE_WAIT_FOR_NEXT_POLLING) {
            mTimeoutCount = (mTimeoutCount + 1) % TIMEOUT_COUNT;
            PollDevices();
        }
    }

    /**
     * Start device polling immediately.
     */
    void PollAllDevicesNow() {
        // Clear existing timer to avoid overlapped execution
        mActionTimer->ClearTimerMessage();

        mTimeoutCount = 0;
        mState = STATE_WAIT_FOR_NEXT_POLLING;
        PollAllDevices();

        AddTimer(mState, POLLING_INTERVAL_MS);
    }

    // This method is called every 5 seconds.
    private void PollDevices() {
        // All device check called every 15 seconds.
        if (mTimeoutCount == 0) {
            PollAllDevices();
        } else {
            if (Tv()->IsSystemAudioActivated()) {
                PollAudioSystem();
            }
        }

        AddTimer(mState, POLLING_INTERVAL_MS);
    }

    private void PollAllDevices() {
        Slogger::V(TAG, "Poll all devices.");

        PollDevices(new DevicePollingCallback() {
            //@Override
            CARAPI OnPollingFinished(List<Integer> ackedAddress) {
                CheckHotplug(ackedAddress, FALSE);
            }
        }, Constants.POLL_ITERATION_IN_ORDER
                | Constants.POLL_STRATEGY_REMOTES_DEVICES, HdmiConfig.HOTPLUG_DETECTION_RETRY);
    }

    private void PollAudioSystem() {
        Slogger::V(TAG, "Poll audio system.");

        PollDevices(new DevicePollingCallback() {
            //@Override
            CARAPI OnPollingFinished(List<Integer> ackedAddress) {
                CheckHotplug(ackedAddress, TRUE);
            }
        }, Constants.POLL_ITERATION_IN_ORDER
                | Constants.POLL_STRATEGY_SYSTEM_AUDIO, HdmiConfig.HOTPLUG_DETECTION_RETRY);
    }

    private void CheckHotplug(List<Integer> ackedAddress, Boolean audioOnly) {
        BitSet currentInfos = InfoListToBitSet(Tv()->GetDeviceInfoList(FALSE), audioOnly);
        BitSet polledResult = AddressListToBitSet(ackedAddress);

        // At first, check removed devices.
        BitSet removed = Complement(currentInfos, polledResult);
        Int32 index = -1;
        while ((index = removed->NextSetBit(index + 1)) != -1) {
            Slogger::V(TAG, "Remove device by hot-plug detection:" + index);
            RemoveDevice(index);
        }

        // Next, check added devices.
        BitSet added = Complement(polledResult, currentInfos);
        index = -1;
        while ((index = added->NextSetBit(index + 1)) != -1) {
            Slogger::V(TAG, "Add device by hot-plug detection:" + index);
            AddDevice(index);
        }
    }

    private static BitSet InfoListToBitSet(List<HdmiDeviceInfo> infoList, Boolean audioOnly) {
        BitSet set = new BitSet(NUM_OF_ADDRESS);
        for (HdmiDeviceInfo info : infoList) {
            if (audioOnly) {
                if (info->GetDeviceType() == HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM) {
                    set->Set(info->GetLogicalAddress());
                }
            } else {
                set->Set(info->GetLogicalAddress());
            }
        }
        return set;
    }

    private static BitSet AddressListToBitSet(List<Integer> list) {
        BitSet set = new BitSet(NUM_OF_ADDRESS);
        for (Integer value : list) {
            set->Set(value);
        }
        return set;
    }

    // A - B = A & ~B
    private static BitSet Complement(BitSet first, BitSet second) {
        // Need to clone it so that it doesn't touch original set.
        BitSet clone = (BitSet) first->Clone();
        clone->AndNot(second);
        return clone;
    }

    private void AddDevice(Int32 addedAddress) {
        // Sending <Give Physical Address> will initiate new device action.
        SendCommand(HdmiCecMessageBuilder->BuildGivePhysicalAddress(GetSourceAddress(),
                addedAddress));
    }

    private void RemoveDevice(Int32 removedAddress) {
        MayChangeRoutingPath(removedAddress);
        MayCancelDeviceSelect(removedAddress);
        MayCancelOneTouchRecord(removedAddress);
        MayDisableSystemAudioAndARC(removedAddress);

        Tv()->RemoveCecDevice(removedAddress);
    }

    private void MayChangeRoutingPath(Int32 address) {
        HdmiDeviceInfo info = Tv()->GetCecDeviceInfo(address);
        if (info != NULL) {
            Tv()->HandleRemoveActiveRoutingPath(info->GetPhysicalAddress());
        }
    }

    private void MayCancelDeviceSelect(Int32 address) {
        List<DeviceSelectAction> actions = GetActions(DeviceSelectAction.class);
        if (actions->IsEmpty()) {
            return;
        }

        // Should have only one Device Select Action
        DeviceSelectAction action = actions->Get(0);
        if (action->GetTargetAddress() == address) {
            RemoveAction(DeviceSelectAction.class);
        }
    }

    private void MayCancelOneTouchRecord(Int32 address) {
        List<OneTouchRecordAction> actions = GetActions(OneTouchRecordAction.class);
        for (OneTouchRecordAction action : actions) {
            if (action->GetRecorderAddress() == address) {
                RemoveAction(action);
            }
        }
    }

    private void MayDisableSystemAudioAndARC(Int32 address) {
        if (HdmiUtils->GetTypeFromAddress(address) != HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM) {
            return;
        }

        // Turn off system audio mode and update settings.
        Tv()->SetSystemAudioMode(FALSE, TRUE);
        if (Tv()->IsArcEstabilished()) {
            AddAndStartAction(new RequestArcTerminationAction(LocalDevice(), address));
        }
    }
}
