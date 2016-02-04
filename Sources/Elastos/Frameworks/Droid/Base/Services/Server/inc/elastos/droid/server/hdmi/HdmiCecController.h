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

using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Droid::internal.util.IndentingPrintWriter;
using Elastos::Droid::internal.util.Predicate;
using Elastos::Droid::Server::Ihdmi.HdmiAnnotations.IoThreadOnly;
using Elastos::Droid::Server::Ihdmi.HdmiAnnotations.ServiceThreadOnly;
using Elastos::Droid::Server::Ihdmi.HdmiControlService.DevicePollingCallback;

import libcore.util.EmptyArray;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;

/**
 * Manages HDMI-CEC command and behaviors. It converts user's command into CEC command
 * and pass it to CEC HAL so that it sends message to other device. For incoming
 * message it translates the message and delegates it to proper module.
 *
 * <p>It should be careful to access member variables on IO thread because
 * it can be accessed from system thread as well.
 *
 * <p>It can be created only by {@link HdmiCecController#create}
 *
 * <p>Declared as package-private, accessed by {@link HdmiControlService} only.
 */
final class HdmiCecController {
    private static const String TAG = "HdmiCecController";

    /**
     * Interface to report allocated logical address.
     */
    interface AllocateAddressCallback {
        /**
         * Called when a new logical address is allocated.
         *
         * @param deviceType requested device type to allocate logical address
         * @param logicalAddress allocated logical address. If it is
         *                       {@link Constants#ADDR_UNREGISTERED}, it means that
         *                       it failed to allocate logical address for the given device type
         */
        void OnAllocated(Int32 deviceType, Int32 logicalAddress);
    }

    private static const Byte[] EMPTY_BODY = EmptyArray.BYTE;

    private static const Int32 NUM_LOGICAL_ADDRESS = 16;

    // Predicate for whether the given logical address is remote device's one or not.
    private final Predicate<Integer> mRemoteDeviceAddressPredicate = new Predicate<Integer>() {
        //@Override
        public Boolean Apply(Integer address) {
            return !IsAllocatedLocalDeviceAddress(address);
        }
    };

    // Predicate whether the given logical address is system audio's one or not
    private final Predicate<Integer> mSystemAudioAddressPredicate = new Predicate<Integer>() {
        //@Override
        public Boolean Apply(Integer address) {
            return HdmiUtils->GetTypeFromAddress(address) == Constants.ADDR_AUDIO_SYSTEM;
        }
    };

    // Handler instance to process synchronous I/O (mainly send) message.
    private Handler mIoHandler;

    // Handler instance to process various messages coming from other CEC
    // device or issued by internal state change.
    private Handler mControlHandler;

    // Stores the pointer to the native implementation of the service that
    // interacts with HAL.
    private volatile Int64 mNativePtr;

    private final HdmiControlService mService;

    // Stores the local CEC devices in the system. Device type is used for key.
    private final SparseArray<HdmiCecLocalDevice> mLocalDevices = new SparseArray<>();

    // Private constructor.  Use HdmiCecController->Create().
    private HdmiCecController(HdmiControlService service) {
        mService = service;
    }

    /**
     * A factory method to get {@link HdmiCecController}. If it fails to initialize
     * inner device or has no device it will return {@code NULL}.
     *
     * <p>Declared as package-private, accessed by {@link HdmiControlService} only.
     * @param service {@link HdmiControlService} instance used to create internal handler
     *                and to pass callback for incoming message or event.
     * @return {@link HdmiCecController} if device is initialized successfully. Otherwise,
     *         returns {@code NULL}.
     */
    static HdmiCecController Create(HdmiControlService service) {
        HdmiCecController controller = new HdmiCecController(service);
        Int64 nativePtr = NativeInit(controller, service->GetServiceLooper()->GetQueue());
        if (nativePtr == 0L) {
            controller = NULL;
            return NULL;
        }

        controller->Init(nativePtr);
        return controller;
    }

    private void Init(Int64 nativePtr) {
        mIoHandler = new Handler(mService->GetServiceLooper());
        mControlHandler = new Handler(mService->GetServiceLooper());
        mNativePtr = nativePtr;
    }

    @ServiceThreadOnly
    void AddLocalDevice(Int32 deviceType, HdmiCecLocalDevice device) {
        AssertRunOnServiceThread();
        mLocalDevices->Put(deviceType, device);
    }

    /**
     * Allocate a new logical address of the given device type. Allocated
     * address will be reported through {@link AllocateAddressCallback}.
     *
     * <p> Declared as package-private, accessed by {@link HdmiControlService} only.
     *
     * @param deviceType type of device to used to determine logical address
     * @param preferredAddress a logical address preferred to be allocated.
     *                         If sets {@link Constants#ADDR_UNREGISTERED}, scans
     *                         the smallest logical address matched with the given device type.
     *                         Otherwise, scan address will start from {@code preferredAddress}
     * @param callback callback interface to report allocated logical address to caller
     */
    @ServiceThreadOnly
    void AllocateLogicalAddress(final Int32 deviceType, final Int32 preferredAddress,
            final AllocateAddressCallback callback) {
        AssertRunOnServiceThread();

        RunOnIoThread(new Runnable() {
            //@Override
            CARAPI Run() {
                HandleAllocateLogicalAddress(deviceType, preferredAddress, callback);
            }
        });
    }

    @IoThreadOnly
    private void HandleAllocateLogicalAddress(final Int32 deviceType, Int32 preferredAddress,
            final AllocateAddressCallback callback) {
        AssertRunOnIoThread();
        Int32 startAddress = preferredAddress;
        // If preferred address is "unregistered", start address will be the smallest
        // address matched with the given device type.
        if (preferredAddress == Constants.ADDR_UNREGISTERED) {
            for (Int32 i = 0; i < NUM_LOGICAL_ADDRESS; ++i) {
                if (deviceType == HdmiUtils->GetTypeFromAddress(i)) {
                    startAddress = i;
                    break;
                }
            }
        }

        Int32 logicalAddress = Constants.ADDR_UNREGISTERED;
        // Iterates all possible addresses which has the same device type.
        for (Int32 i = 0; i < NUM_LOGICAL_ADDRESS; ++i) {
            Int32 curAddress = (startAddress + i) % NUM_LOGICAL_ADDRESS;
            if (curAddress != Constants.ADDR_UNREGISTERED
                    && deviceType == HdmiUtils->GetTypeFromAddress(curAddress)) {
                Int32 failedPollingCount = 0;
                for (Int32 j = 0; j < HdmiConfig.ADDRESS_ALLOCATION_RETRY; ++j) {
                    if (!SendPollMessage(curAddress, curAddress, 1)) {
                        failedPollingCount++;
                    }
                }

                // Pick logical address if failed ratio is more than a half of all retries.
                if (failedPollingCount * 2 >  HdmiConfig.ADDRESS_ALLOCATION_RETRY) {
                    logicalAddress = curAddress;
                    break;
                }
            }
        }

        final Int32 assignedAddress = logicalAddress;
        HdmiLogger->Debug("New logical address for device [%d]: [preferred:%d, assigned:%d]",
                        deviceType, preferredAddress, assignedAddress);
        if (callback != NULL) {
            RunOnServiceThread(new Runnable() {
                //@Override
                CARAPI Run() {
                    callback->OnAllocated(deviceType, assignedAddress);
                }
            });
        }
    }

    private static Byte[] BuildBody(Int32 opcode, Byte[] params) {
        Byte[] body = new Byte[params.length + 1];
        body[0] = (Byte) opcode;
        System->Arraycopy(params, 0, body, 1, params.length);
        return body;
    }


    HdmiPortInfo[] GetPortInfos() {
        return NativeGetPortInfos(mNativePtr);
    }

    /**
     * Return the locally hosted logical device of a given type.
     *
     * @param deviceType logical device type
     * @return {@link HdmiCecLocalDevice} instance if the instance of the type is available;
     *          otherwise NULL.
     */
    HdmiCecLocalDevice GetLocalDevice(Int32 deviceType) {
        return mLocalDevices->Get(deviceType);
    }

    /**
     * Add a new logical address to the device. Device's HW should be notified
     * when a new logical address is assigned to a device, so that it can accept
     * a command having available destinations.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param newLogicalAddress a logical address to be added
     * @return 0 on success. Otherwise, returns negative value
     */
    @ServiceThreadOnly
    Int32 AddLogicalAddress(Int32 newLogicalAddress) {
        AssertRunOnServiceThread();
        if (HdmiUtils->IsValidAddress(newLogicalAddress)) {
            return NativeAddLogicalAddress(mNativePtr, newLogicalAddress);
        } else {
            return -1;
        }
    }

    /**
     * Clear all logical addresses registered in the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    @ServiceThreadOnly
    void ClearLogicalAddress() {
        AssertRunOnServiceThread();
        for (Int32 i = 0; i < mLocalDevices->Size(); ++i) {
            mLocalDevices->ValueAt(i).ClearAddress();
        }
        NativeClearLogicalAddress(mNativePtr);
    }

    @ServiceThreadOnly
    void ClearLocalDevices() {
        AssertRunOnServiceThread();
        mLocalDevices->Clear();
    }

    /**
     * Return the physical address of the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @return CEC physical address of the device. The range of success address
     *         is between 0x0000 and 0xFFFF. If failed it returns -1
     */
    @ServiceThreadOnly
    Int32 GetPhysicalAddress() {
        AssertRunOnServiceThread();
        return NativeGetPhysicalAddress(mNativePtr);
    }

    /**
     * Return CEC version of the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    @ServiceThreadOnly
    Int32 GetVersion() {
        AssertRunOnServiceThread();
        return NativeGetVersion(mNativePtr);
    }

    /**
     * Return vendor id of the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    @ServiceThreadOnly
    Int32 GetVendorId() {
        AssertRunOnServiceThread();
        return NativeGetVendorId(mNativePtr);
    }

    /**
     * Set an option to CEC HAL.
     *
     * @param flag key of option
     * @param value value of option
     */
    @ServiceThreadOnly
    void SetOption(Int32 flag, Int32 value) {
        AssertRunOnServiceThread();
        NativeSetOption(mNativePtr, flag, value);
    }

    /**
     * Configure ARC circuit in the hardware logic to start or stop the feature.
     *
     * @param enabled whether to enable/disable ARC
     */
    @ServiceThreadOnly
    void SetAudioReturnChannel(Boolean enabled) {
        AssertRunOnServiceThread();
        NativeSetAudioReturnChannel(mNativePtr, enabled);
    }

    /**
     * Return the connection status of the specified port
     *
     * @param port port number to check connection status
     * @return TRUE if connected; otherwise, return FALSE
     */
    @ServiceThreadOnly
    Boolean IsConnected(Int32 port) {
        AssertRunOnServiceThread();
        return NativeIsConnected(mNativePtr, port);
    }

    /**
     * Poll all remote devices. It sends &lt;Polling Message&gt; to all remote
     * devices.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param callback an interface used to get a list of all remote devices' address
     * @param sourceAddress a logical address of source device where sends polling message
     * @param pickStrategy strategy how to pick polling candidates
     * @param retryCount the number of retry used to send polling message to remote devices
     */
    @ServiceThreadOnly
    void PollDevices(DevicePollingCallback callback, Int32 sourceAddress, Int32 pickStrategy,
            Int32 retryCount) {
        AssertRunOnServiceThread();

        // Extract polling candidates. No need to poll against local devices.
        List<Integer> pollingCandidates = PickPollCandidates(pickStrategy);
        ArrayList<Integer> allocated = new ArrayList<>();
        RunDevicePolling(sourceAddress, pollingCandidates, retryCount, callback, allocated);
    }

    /**
     * Return a list of all {@link HdmiCecLocalDevice}s.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    @ServiceThreadOnly
    List<HdmiCecLocalDevice> GetLocalDeviceList() {
        AssertRunOnServiceThread();
        return HdmiUtils->SparseArrayToList(mLocalDevices);
    }

    private List<Integer> PickPollCandidates(Int32 pickStrategy) {
        Int32 strategy = pickStrategy & Constants.POLL_STRATEGY_MASK;
        Predicate<Integer> pickPredicate = NULL;
        switch (strategy) {
            case Constants.POLL_STRATEGY_SYSTEM_AUDIO:
                pickPredicate = mSystemAudioAddressPredicate;
                break;
            case Constants.POLL_STRATEGY_REMOTES_DEVICES:
            default:  // The default is POLL_STRATEGY_REMOTES_DEVICES.
                pickPredicate = mRemoteDeviceAddressPredicate;
                break;
        }

        Int32 iterationStrategy = pickStrategy & Constants.POLL_ITERATION_STRATEGY_MASK;
        LinkedList<Integer> pollingCandidates = new LinkedList<>();
        switch (iterationStrategy) {
            case Constants.POLL_ITERATION_IN_ORDER:
                for (Int32 i = Constants.ADDR_TV; i <= Constants.ADDR_SPECIFIC_USE; ++i) {
                    if (pickPredicate->Apply(i)) {
                        pollingCandidates->Add(i);
                    }
                }
                break;
            case Constants.POLL_ITERATION_REVERSE_ORDER:
            default:  // The default is reverse order.
                for (Int32 i = Constants.ADDR_SPECIFIC_USE; i >= Constants.ADDR_TV; --i) {
                    if (pickPredicate->Apply(i)) {
                        pollingCandidates->Add(i);
                    }
                }
                break;
        }
        return pollingCandidates;
    }

    @ServiceThreadOnly
    private Boolean IsAllocatedLocalDeviceAddress(Int32 address) {
        AssertRunOnServiceThread();
        for (Int32 i = 0; i < mLocalDevices->Size(); ++i) {
            if (mLocalDevices->ValueAt(i).IsAddressOf(address)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    @ServiceThreadOnly
    private void RunDevicePolling(final Int32 sourceAddress,
            final List<Integer> candidates, final Int32 retryCount,
            final DevicePollingCallback callback, final List<Integer> allocated) {
        AssertRunOnServiceThread();
        if (candidates->IsEmpty()) {
            if (callback != NULL) {
                HdmiLogger->Debug("[P]:AllocatedAddress=%s", allocated->ToString());
                callback->OnPollingFinished(allocated);
            }
            return;
        }

        final Integer candidate = candidates->Remove(0);
        // Proceed polling action for the next address once polling action for the
        // previous address is done.
        RunOnIoThread(new Runnable() {
            //@Override
            CARAPI Run() {
                if (SendPollMessage(sourceAddress, candidate, retryCount)) {
                    allocated->Add(candidate);
                }
                RunOnServiceThread(new Runnable() {
                    //@Override
                    CARAPI Run() {
                        RunDevicePolling(sourceAddress, candidates, retryCount, callback,
                                allocated);
                    }
                });
            }
        });
    }

    @IoThreadOnly
    private Boolean SendPollMessage(Int32 sourceAddress, Int32 destinationAddress, Int32 retryCount) {
        AssertRunOnIoThread();
        for (Int32 i = 0; i < retryCount; ++i) {
            // <Polling Message> is a message which has empty body.
            // If sending <Polling Message> failed (NAK), it becomes
            // new logical address for the device because no device uses
            // it as logical address of the device.
            if (NativeSendCecCommand(mNativePtr, sourceAddress, destinationAddress, EMPTY_BODY)
                    == Constants.SEND_RESULT_SUCCESS) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private void AssertRunOnIoThread() {
        if (Looper->MyLooper() != mIoHandler->GetLooper()) {
            throw new IllegalStateException("Should run on io thread.");
        }
    }

    private void AssertRunOnServiceThread() {
        if (Looper->MyLooper() != mControlHandler->GetLooper()) {
            throw new IllegalStateException("Should run on service thread.");
        }
    }

    // Run a Runnable on IO thread.
    // It should be careful to access member variables on IO thread because
    // it can be accessed from system thread as well.
    private void RunOnIoThread(Runnable runnable) {
        mIoHandler->Post(runnable);
    }

    private void RunOnServiceThread(Runnable runnable) {
        mControlHandler->Post(runnable);
    }

    private Boolean IsAcceptableAddress(Int32 address) {
        // Can access command targeting devices available in local device or broadcast command.
        if (address == Constants.ADDR_BROADCAST) {
            return TRUE;
        }
        return IsAllocatedLocalDeviceAddress(address);
    }

    @ServiceThreadOnly
    private void OnReceiveCommand(HdmiCecMessage message) {
        AssertRunOnServiceThread();
        if (IsAcceptableAddress(message->GetDestination()) && mService->HandleCecCommand(message)) {
            return;
        }
        // Not handled message, so we will reply it with <Feature Abort>.
        MaySendFeatureAbortCommand(message, Constants.ABORT_UNRECOGNIZED_OPCODE);
    }

    @ServiceThreadOnly
    void MaySendFeatureAbortCommand(HdmiCecMessage message, Int32 reason) {
        AssertRunOnServiceThread();
        // Swap the source and the destination.
        Int32 src = message->GetDestination();
        Int32 dest = message->GetSource();
        if (src == Constants.ADDR_BROADCAST || dest == Constants.ADDR_UNREGISTERED) {
            // Don't reply <Feature Abort> from the unregistered devices or for the broadcasted
            // messages. See CEC 12.2 Protocol General Rules for detail.
            return;
        }
        Int32 originalOpcode = message->GetOpcode();
        if (originalOpcode == Constants.MESSAGE_FEATURE_ABORT) {
            return;
        }
        SendCommand(
                HdmiCecMessageBuilder->BuildFeatureAbortCommand(src, dest, originalOpcode, reason));
    }

    @ServiceThreadOnly
    void SendCommand(HdmiCecMessage cecMessage) {
        AssertRunOnServiceThread();
        SendCommand(cecMessage, NULL);
    }

    @ServiceThreadOnly
    void SendCommand(final HdmiCecMessage cecMessage,
            final HdmiControlService.SendMessageCallback callback) {
        AssertRunOnServiceThread();
        RunOnIoThread(new Runnable() {
            //@Override
            CARAPI Run() {
                HdmiLogger->Debug("[S]:" + cecMessage);
                Byte[] body = BuildBody(cecMessage->GetOpcode(), cecMessage->GetParams());
                Int32 i = 0;
                Int32 errorCode = Constants.SEND_RESULT_SUCCESS;
                do {
                    errorCode = NativeSendCecCommand(mNativePtr, cecMessage->GetSource(),
                            cecMessage->GetDestination(), body);
                    if (errorCode == Constants.SEND_RESULT_SUCCESS) {
                        break;
                    }
                } while (i++ < HdmiConfig.RETRANSMISSION_COUNT);

                final Int32 finalError = errorCode;
                if (finalError != Constants.SEND_RESULT_SUCCESS) {
                    Slogger::W(TAG, "Failed to send " + cecMessage);
                }
                if (callback != NULL) {
                    RunOnServiceThread(new Runnable() {
                        //@Override
                        CARAPI Run() {
                            callback->OnSendCompleted(finalError);
                        }
                    });
                }
            }
        });
    }

    /**
     * Called by native when incoming CEC message arrived.
     */
    @ServiceThreadOnly
    private void HandleIncomingCecCommand(Int32 srcAddress, Int32 dstAddress, Byte[] body) {
        AssertRunOnServiceThread();
        HdmiCecMessage command = HdmiCecMessageBuilder->Of(srcAddress, dstAddress, body);
        HdmiLogger->Debug("[R]:" + command);
        OnReceiveCommand(command);
    }

    /**
     * Called by native when a hotplug event issues.
     */
    @ServiceThreadOnly
    private void HandleHotplug(Int32 port, Boolean connected) {
        AssertRunOnServiceThread();
        HdmiLogger->Debug("Hotplug event:[port:%d, connected:%b]", port, connected);
        mService->OnHotplug(port, connected);
    }

    void Dump(final IndentingPrintWriter pw) {
        for (Int32 i = 0; i < mLocalDevices->Size(); ++i) {
            pw->Println("HdmiCecLocalDevice #" + i + ":");
            pw->IncreaseIndent();
            mLocalDevices->ValueAt(i).Dump(pw);
            pw->DecreaseIndent();
        }
    }

    private static native Int64 NativeInit(HdmiCecController handler, MessageQueue messageQueue);
    private static native Int32 NativeSendCecCommand(Int64 controllerPtr, Int32 srcAddress,
            Int32 dstAddress, Byte[] body);
    private static native Int32 NativeAddLogicalAddress(Int64 controllerPtr, Int32 logicalAddress);
    private static native void NativeClearLogicalAddress(Int64 controllerPtr);
    private static native Int32 NativeGetPhysicalAddress(Int64 controllerPtr);
    private static native Int32 NativeGetVersion(Int64 controllerPtr);
    private static native Int32 NativeGetVendorId(Int64 controllerPtr);
    private static native HdmiPortInfo[] NativeGetPortInfos(Int64 controllerPtr);
    private static native void NativeSetOption(Int64 controllerPtr, Int32 flag, Int32 value);
    private static native void NativeSetAudioReturnChannel(Int64 controllerPtr, Boolean flag);
    private static native Boolean NativeIsConnected(Int64 controllerPtr, Int32 port);
}
