
#include "elastos/droid/server/hdmi/HdmiCecController.h"
#include <libcore/utility/EmptyArray.h>
#include <Elastos.Droid.Os.h>

using Elastos::Utility::IArrayList;
using Elastos::Utility::ILinkedList;

using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::IPredicate;

using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiCecController::RemoteDevicePredicate
//=============================================================================
ECode HdmiCecController::RemoteDevicePredicate::Apply(
    /* [in] */ IInteger32* address,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            return !isAllocatedLocalDeviceAddress(address);
#endif
}

//=============================================================================
// HdmiCecController::SystemAudioPredicate
//=============================================================================
ECode HdmiCecController::SystemAudioPredicate::Apply(
    /* [in] */ IInteger32* address,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            return HdmiUtils.getTypeFromAddress(address) == Constants.ADDR_AUDIO_SYSTEM;
#endif
}

//=============================================================================
// HdmiCecController
//=============================================================================
const String HdmiCecController::TAG("HdmiCecController");
const AutoPtr<ArrayOf<Byte> > HdmiCecController::EMPTY_BODY = EmptyArray::BYTE;
const Int32 HdmiCecController::NUM_LOGICAL_ADDRESS = 16;

#if 0
HdmiCecController::HdmiCecController(
    /* [in] */ HdmiControlService* service)
{
#if 0 // TODO: Translate codes below
        mService = service;
    mRemoteDeviceAddressPredicate = new RemoteDevicePredicate();
    mSystemAudioAddressPredicate = new SystemAudioPredicate();
    CSparseArray::New((ISparseArray**)&mLocalDevices);
#endif
}

ECode HdmiCecController::Create(
    /* [in] */ HdmiControlService* service,
    /* [out] */ HdmiCecController** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiCecController controller = new HdmiCecController(service);
        long nativePtr = nativeInit(controller, service.getServiceLooper().getQueue());
        if (nativePtr == 0L) {
            controller = NULL;
            return NULL;
        }
        controller.init(nativePtr);
        return controller;

#endif
}
#endif

ECode HdmiCecController::Init(
    /* [in] */ Int64 nativePtr)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mIoHandler = new Handler(mService.getServiceLooper());
        mControlHandler = new Handler(mService.getServiceLooper());
        mNativePtr = nativePtr;

#endif
}

ECode HdmiCecController::AddLocalDevice(
    /* [in] */ Int32 deviceType,
    /* [in] */ IHdmiCecLocalDevice* device)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mLocalDevices.put(deviceType, device);

#endif
}

ECode HdmiCecController::AllocateLogicalAddress(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 preferredAddress,
    /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        runOnIoThread(new Runnable() {
            @Override
            public void run() {
                handleAllocateLogicalAddress(deviceType, preferredAddress, callback);
            }
        });

#endif
}

ECode HdmiCecController::HandleAllocateLogicalAddress(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 preferredAddress,
    /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnIoThread();
        int startAddress = preferredAddress;
        // If preferred address is "unregistered", start address will be the smallest
        // address matched with the given device type.
        if (preferredAddress == Constants::ADDR_UNREGISTERED) {
            for (int i = 0; i < NUM_LOGICAL_ADDRESS; ++i) {
                if (deviceType == HdmiUtils.getTypeFromAddress(i)) {
                    startAddress = i;
                    break;
                }
            }
        }
        int logicalAddress = Constants::ADDR_UNREGISTERED;
        // Iterates all possible addresses which has the same device type.
        for (int i = 0; i < NUM_LOGICAL_ADDRESS; ++i) {
            int curAddress = (startAddress + i) % NUM_LOGICAL_ADDRESS;
            if (curAddress != Constants::ADDR_UNREGISTERED
                    && deviceType == HdmiUtils.getTypeFromAddress(curAddress)) {
                int failedPollingCount = 0;
                for (int j = 0; j < HdmiConfig.ADDRESS_ALLOCATION_RETRY; ++j) {
                    if (!sendPollMessage(curAddress, curAddress, 1)) {
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
        final int assignedAddress = logicalAddress;
        HdmiLogger.debug("New logical address for device [%d]: [preferred:%d, assigned:%d]",
                        deviceType, preferredAddress, assignedAddress);
        if (callback != NULL) {
            runOnServiceThread(new Runnable() {
                @Override
                public void run() {
                    callback.onAllocated(deviceType, assignedAddress);
                }
            });
        }

#endif
}

ECode HdmiCecController::BuildBody(
    /* [in] */ Int32 opcode,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out, callee] */ ArrayOf<Byte>* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] body = new byte[params.length + 1];
        body[0] = (byte) opcode;
        System.arraycopy(params, 0, body, 1, params.length);
        return body;

#endif
}

ECode HdmiCecController::GetPortInfos(
    /* [out, callee] */ ArrayOf<IHdmiPortInfo*>* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return nativeGetPortInfos(mNativePtr);

#endif
}

ECode HdmiCecController::GetLocalDevice(
    /* [in] */ Int32 deviceType,
    /* [out] */ IHdmiCecLocalDevice** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mLocalDevices.get(deviceType);

#endif
}

ECode HdmiCecController::AddLogicalAddress(
    /* [in] */ Int32 newLogicalAddress,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (HdmiUtils.isValidAddress(newLogicalAddress)) {
            return nativeAddLogicalAddress(mNativePtr, newLogicalAddress);
        } else {
            return -1;
        }

#endif
}

ECode HdmiCecController::ClearLogicalAddress()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (int i = 0; i < mLocalDevices.size(); ++i) {
            mLocalDevices.valueAt(i).clearAddress();
        }
        nativeClearLogicalAddress(mNativePtr);

#endif
}

ECode HdmiCecController::ClearLocalDevices()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mLocalDevices.clear();

#endif
}

ECode HdmiCecController::GetPhysicalAddress(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return nativeGetPhysicalAddress(mNativePtr);

#endif
}

ECode HdmiCecController::GetVersion(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return nativeGetVersion(mNativePtr);

#endif
}

ECode HdmiCecController::GetVendorId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return nativeGetVendorId(mNativePtr);

#endif
}

ECode HdmiCecController::SetOption(
    /* [in] */ Int32 flag,
    /* [in] */ Int32 value)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        nativeSetOption(mNativePtr, flag, value);

#endif
}

ECode HdmiCecController::SetAudioReturnChannel(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        nativeSetAudioReturnChannel(mNativePtr, enabled);

#endif
}

ECode HdmiCecController::IsConnected(
    /* [in] */ Int32 port,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return nativeIsConnected(mNativePtr, port);

#endif
}

ECode HdmiCecController::PollDevices(
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ Int32 sourceAddress,
    /* [in] */ Int32 pickStrategy,
    /* [in] */ Int32 retryCount)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Extract polling candidates. No need to poll against local devices.
        List<Integer> pollingCandidates = pickPollCandidates(pickStrategy);
        ArrayList<Integer> allocated = new ArrayList<>();
        runDevicePolling(sourceAddress, pollingCandidates, retryCount, callback, allocated);

#endif
}

ECode HdmiCecController::GetLocalDeviceList(
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return HdmiUtils.sparseArrayToList(mLocalDevices);

#endif
}

ECode HdmiCecController::PickPollCandidates(
    /* [in] */ Int32 pickStrategy,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int strategy = pickStrategy & Constants::POLL_STRATEGY_MASK;
        Predicate<Integer> pickPredicate = NULL;
        switch (strategy) {
            case Constants::POLL_STRATEGY_SYSTEM_AUDIO:
                pickPredicate = mSystemAudioAddressPredicate;
                break;
            case Constants::POLL_STRATEGY_REMOTES_DEVICES:
            default:  // The default is POLL_STRATEGY_REMOTES_DEVICES.
                pickPredicate = mRemoteDeviceAddressPredicate;
                break;
        }
        int iterationStrategy = pickStrategy & Constants::POLL_ITERATION_STRATEGY_MASK;
        LinkedList<Integer> pollingCandidates = new LinkedList<>();
        switch (iterationStrategy) {
            case Constants::POLL_ITERATION_IN_ORDER:
                for (int i = Constants::ADDR_TV; i <= Constants::ADDR_SPECIFIC_USE; ++i) {
                    if (pickPredicate.apply(i)) {
                        pollingCandidates.add(i);
                    }
                }
                break;
            case Constants::POLL_ITERATION_REVERSE_ORDER:
            default:  // The default is reverse order.
                for (int i = Constants::ADDR_SPECIFIC_USE; i >= Constants::ADDR_TV; --i) {
                    if (pickPredicate.apply(i)) {
                        pollingCandidates.add(i);
                    }
                }
                break;
        }
        return pollingCandidates;

#endif
}

ECode HdmiCecController::IsAllocatedLocalDeviceAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (int i = 0; i < mLocalDevices.size(); ++i) {
            if (mLocalDevices.valueAt(i).isAddressOf(address)) {
                return true;
            }
        }
        return FALSE;

#endif
}

ECode HdmiCecController::RunDevicePolling(
    /* [in] */ Int32 sourceAddress,
    /* [in] */ IList* candidates,
    /* [in] */ Int32 retryCount,
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ IList* allocated)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (candidates.isEmpty()) {
            if (callback != NULL) {
                HdmiLogger.debug("[P]:AllocatedAddress=%s", allocated.toString());
                callback.onPollingFinished(allocated);
            }
            return;
        }
        final Integer candidate = candidates.remove(0);
        // Proceed polling action for the next address once polling action for the
        // previous address is done.
        runOnIoThread(new Runnable() {
            @Override
            public void run() {
                if (sendPollMessage(sourceAddress, candidate, retryCount)) {
                    allocated.add(candidate);
                }
                runOnServiceThread(new Runnable() {
                    @Override
                    public void run() {
                        runDevicePolling(sourceAddress, candidates, retryCount, callback,
                                allocated);
                    }
                });
            }
        });

#endif
}

ECode HdmiCecController::SendPollMessage(
    /* [in] */ Int32 sourceAddress,
    /* [in] */ Int32 destinationAddress,
    /* [in] */ Int32 retryCount,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnIoThread();
        for (int i = 0; i < retryCount; ++i) {
            // <Polling Message> is a message which has empty body.
            // If sending <Polling Message> failed (NAK), it becomes
            // new logical address for the device because no device uses
            // it as logical address of the device.
            if (nativeSendCecCommand(mNativePtr, sourceAddress, destinationAddress, EMPTY_BODY)
                    == Constants::SEND_RESULT_SUCCESS) {
                return true;
            }
        }
        return FALSE;

#endif
}

ECode HdmiCecController::AssertRunOnIoThread()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (Looper.myLooper() != mIoHandler.getLooper()) {
            throw new IllegalStateException("Should run on io thread.");
        }

#endif
}

ECode HdmiCecController::AssertRunOnServiceThread()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (Looper.myLooper() != mControlHandler.getLooper()) {
            throw new IllegalStateException("Should run on service thread.");
        }

#endif
}

ECode HdmiCecController::RunOnIoThread(
    /* [in] */ IRunnable* runnable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mIoHandler.post(runnable);

#endif
}

ECode HdmiCecController::RunOnServiceThread(
    /* [in] */ IRunnable* runnable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mControlHandler.post(runnable);

#endif
}

ECode HdmiCecController::IsAcceptableAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Can access command targeting devices available in local device or broadcast command.
        if (address == Constants::ADDR_BROADCAST) {
            return true;
        }
        return isAllocatedLocalDeviceAddress(address);

#endif
}

ECode HdmiCecController::OnReceiveCommand(
    /* [in] */ IHdmiCecMessage* message)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (isAcceptableAddress(message.getDestination()) && mService.handleCecCommand(message)) {
            return;
        }
        // Not handled message, so we will reply it with <Feature Abort>.
        maySendFeatureAbortCommand(message, Constants::ABORT_UNRECOGNIZED_OPCODE);

#endif
}

ECode HdmiCecController::MaySendFeatureAbortCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [in] */ Int32 reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Swap the source and the destination.
        int src = message.getDestination();
        int dest = message.getSource();
        if (src == Constants::ADDR_BROADCAST || dest == Constants::ADDR_UNREGISTERED) {
            // Don't reply <Feature Abort> from the unregistered devices or for the broadcasted
            // messages. See CEC 12.2 Protocol General Rules for detail.
            return;
        }
        int originalOpcode = message.getOpcode();
        if (originalOpcode == Constants::MESSAGE_FEATURE_ABORT) {
            return;
        }
        sendCommand(
                HdmiCecMessageBuilder.buildFeatureAbortCommand(src, dest, originalOpcode, reason));

#endif
}

ECode HdmiCecController::SendCommand(
    /* [in] */ IHdmiCecMessage* cecMessage)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        sendCommand(cecMessage, NULL);

#endif
}

ECode HdmiCecController::SendCommand(
    /* [in] */ IHdmiCecMessage* cecMessage,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        runOnIoThread(new Runnable() {
            @Override
            public void run() {
                HdmiLogger.debug("[S]:" + cecMessage);
                byte[] body = buildBody(cecMessage.getOpcode(), cecMessage.getParams());
                int i = 0;
                int errorCode = Constants::SEND_RESULT_SUCCESS;
                do {
                    errorCode = nativeSendCecCommand(mNativePtr, cecMessage.getSource(),
                            cecMessage.getDestination(), body);
                    if (errorCode == Constants::SEND_RESULT_SUCCESS) {
                        break;
                    }
                } while (i++ < HdmiConfig.RETRANSMISSION_COUNT);
                final int finalError = errorCode;
                if (finalError != Constants::SEND_RESULT_SUCCESS) {
                    Slogger::W(TAG, "Failed to send " + cecMessage);
                }
                if (callback != NULL) {
                    runOnServiceThread(new Runnable() {
                        @Override
                        public void run() {
                            callback.onSendCompleted(finalError);
                        }
                    });
                }
            }
        });

#endif
}

ECode HdmiCecController::HandleIncomingCecCommand(
    /* [in] */ Int32 srcAddress,
    /* [in] */ Int32 dstAddress,
    /* [in] */ ArrayOf<Byte>* body)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiCecMessage command = HdmiCecMessageBuilder.of(srcAddress, dstAddress, body);
        HdmiLogger.debug("[R]:" + command);
        onReceiveCommand(command);

#endif
}

ECode HdmiCecController::HandleHotplug(
    /* [in] */ Int32 port,
    /* [in] */ Boolean connected)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        HdmiLogger.debug("Hotplug event:[port:%d, connected:%b]", port, connected);
        mService.onHotplug(port, connected);

#endif
}

ECode HdmiCecController::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (int i = 0; i < mLocalDevices.size(); ++i) {
            pw.println("HdmiCecLocalDevice #" + i + ":");
            pw.increaseIndent();
            mLocalDevices.valueAt(i).dump(pw);
            pw.decreaseIndent();
        }

#endif
}

ECode HdmiCecController::NativeInit(
    /* [in] */ HdmiCecController* handler,
    /* [in] */ IMessageQueue* messageQueue,
    /* [out] */ Int64* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeSendCecCommand(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 srcAddress,
    /* [in] */ Int32 dstAddress,
    /* [in] */ ArrayOf<Byte>* body,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeAddLogicalAddress(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 logicalAddress,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeClearLogicalAddress(
    /* [in] */ Int64 controllerPtr)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeGetPhysicalAddress(
    /* [in] */ Int64 controllerPtr,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeGetVersion(
    /* [in] */ Int64 controllerPtr,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeGetVendorId(
    /* [in] */ Int64 controllerPtr,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeGetPortInfos(
    /* [in] */ Int64 controllerPtr,
    /* [out, callee] */ ArrayOf<IHdmiPortInfo*>* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeSetOption(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 flag,
    /* [in] */ Int32 value)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeSetAudioReturnChannel(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Boolean flag)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecController::NativeIsConnected(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 port,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

