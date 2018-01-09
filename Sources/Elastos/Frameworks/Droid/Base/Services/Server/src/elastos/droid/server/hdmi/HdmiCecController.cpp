//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/server/hdmi/HdmiCecController.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <libcore/utility/EmptyArray.h>

#include <android/hardware/tv/cec/1.0/IHdmiCec.h>
#include <android/hardware/tv/cec/1.0/IHdmiCecCallback.h>
#include <android/hardware/tv/cec/1.0/types.h>

#include <cstring>
#include <hardware/hdmi_cec.h>
#include <sys/param.h>
#include <utils/Looper.h>
#include <utils/RefBase.h>
#include "elastos/droid/os/NativeMessageQueue.h"

using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Droid::Internal::Utility::EIID_IPredicate;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::MessageQueue;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISlog;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Libcore::Utility::EmptyArray;

using android::hardware::tv::cec::V1_0::CecLogicalAddress;
using android::hardware::tv::cec::V1_0::CecMessage;
using android::hardware::tv::cec::V1_0::HdmiPortInfo;
using android::hardware::tv::cec::V1_0::HotplugEvent;
using android::hardware::tv::cec::V1_0::IHdmiCec;
using android::hardware::tv::cec::V1_0::IHdmiCecCallback;
using android::hardware::tv::cec::V1_0::OptionKey;
using android::hardware::tv::cec::V1_0::Result;
using android::hardware::tv::cec::V1_0::SendMessageResult;
using android::hardware::Return;
using android::hardware::hidl_vec;

namespace android {

class HdmiCecController {
public:
    HdmiCecController(sp<IHdmiCec> hdmiCec, Elastos::Droid::Server::Hdmi::HdmiCecController* callbacksObj,
            const sp<Looper>& looper);

    // Send message to other device. Note that it runs in IO thread.
    int sendMessage(const CecMessage& message);
    // Add a logical address to device.
    int addLogicalAddress(CecLogicalAddress address);
    // Clear all logical address registered to the device.
    void clearLogicaladdress();
    // Get physical address of device.
    int getPhysicalAddress();
    // Get CEC version from driver.
    int getVersion();
    // Get vendor id used for vendor command.
    uint32_t getVendorId();
    // Get Port information on all the HDMI ports.
    AutoPtr<ArrayOf<IHdmiPortInfo*> > getPortInfos();
    // Set a flag and its value.
    void setOption(OptionKey key, bool enabled);
    // Set audio return channel status.
    void enableAudioReturnChannel(int port, bool flag);
    // Whether to hdmi device is connected to the given port.
    bool isConnected(int port);

    AutoPtr<Elastos::Droid::Server::Hdmi::HdmiCecController> getCallbacksObj() const {
        return mCallbacksObj;
    }

private:
    class HdmiCecCallback : public IHdmiCecCallback {
    public:
        HdmiCecCallback(HdmiCecController* controller) : mController(controller) {};
        Return<void> onCecMessage(const CecMessage& event)  override;
        Return<void> onHotplugEvent(const HotplugEvent& event)  override;
    private:
        HdmiCecController* mController;
    };

    static const int INVALID_PHYSICAL_ADDRESS = 0xFFFF;

    sp<IHdmiCec> mHdmiCec;
    Elastos::Droid::Server::Hdmi::HdmiCecController* mCallbacksObj;
    sp<IHdmiCecCallback> mHdmiCecCallback;
    sp<Looper> mLooper;
};

// Handler class to delegate incoming message to service thread.
class HdmiCecEventHandler : public MessageHandler {
public:
    enum EventType {
        CEC_MESSAGE,
        HOT_PLUG
    };

    HdmiCecEventHandler(HdmiCecController* controller, const CecMessage& cecMessage)
        : mController(controller),
          mCecMessage(cecMessage) {
    }

    HdmiCecEventHandler(HdmiCecController* controller, const HotplugEvent& hotplugEvent)
        : mController(controller),
          mHotplugEvent(hotplugEvent) {
    }

    virtual ~HdmiCecEventHandler() {}

    void handleMessage(const Message& message) {
        switch (message.what) {
        case EventType::CEC_MESSAGE:
            propagateCecCommand(mCecMessage);
            break;
        case EventType::HOT_PLUG:
            propagateHotplugEvent(mHotplugEvent);
            break;
        default:
            // TODO: add more type whenever new type is introduced.
            break;
        }
    }

private:
    // Propagate the message up to Java layer.
    void propagateCecCommand(const CecMessage& message) {
        Int32 srcAddr = static_cast<Int32>(message.initiator);
        Int32 dstAddr = static_cast<Int32>(message.destination);
        AutoPtr<ArrayOf<Byte> > body = ArrayOf<Byte>::Alloc(message.body.size());
        const Byte* bodyPtr = reinterpret_cast<const Byte *>(message.body.data());
        for (uint i = 0; i < message.body.size(); ++i)
            (*body)[i] = bodyPtr[i];

        mController->getCallbacksObj()->HandleIncomingCecCommand(srcAddr,
                dstAddr, body);

        checkAndClearExceptionFromCallback(__FUNCTION__);
    }

    void propagateHotplugEvent(const HotplugEvent& event) {
        // Note that this method should be called in service thread.
        Int32 port = static_cast<Int32>(event.portId);
        Boolean connected = (Boolean) event.connected;
        mController->getCallbacksObj()->HandleHotplug(port, connected);

        checkAndClearExceptionFromCallback(__FUNCTION__);
    }

    // static
    static void checkAndClearExceptionFromCallback(const char* methodName) {
    }

    HdmiCecController* mController;
    CecMessage mCecMessage;
    HotplugEvent mHotplugEvent;
};

HdmiCecController::HdmiCecController(sp<IHdmiCec> hdmiCec,
        Elastos::Droid::Server::Hdmi::HdmiCecController* callbacksObj, const sp<Looper>& looper) :
    mHdmiCec(hdmiCec),
    mCallbacksObj(callbacksObj),
    mLooper(looper) {
    mHdmiCecCallback = new HdmiCecCallback(this);
    Return<void> ret = mHdmiCec->setCallback(mHdmiCecCallback);
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to set a cec callback.");
    }
}

int HdmiCecController::sendMessage(const CecMessage& message) {
    // TODO: propagate send_message's return value.
    Return<SendMessageResult> ret = mHdmiCec->sendMessage(message);
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to send CEC message.");
        return static_cast<int>(SendMessageResult::FAIL);
    }
    return static_cast<int>((SendMessageResult) ret);
}

int HdmiCecController::addLogicalAddress(CecLogicalAddress address) {
    Return<Result> ret = mHdmiCec->addLogicalAddress(address);
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to add a logical address.");
        return static_cast<int>(Result::FAILURE_UNKNOWN);
    }
    return static_cast<int>((Result) ret);
}

void HdmiCecController::clearLogicaladdress() {
    Return<void> ret = mHdmiCec->clearLogicalAddress();
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to clear logical address.");
    }
}

int HdmiCecController::getPhysicalAddress() {
    Result result;
    uint16_t addr;
    Return<void> ret = mHdmiCec->getPhysicalAddress([&result, &addr](Result res, uint16_t paddr) {
            result = res;
            addr = paddr;
    });
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to get physical address.");
        return INVALID_PHYSICAL_ADDRESS;
    }
    return result == Result::SUCCESS ? addr : INVALID_PHYSICAL_ADDRESS;
}

int HdmiCecController::getVersion() {
    Return<int32_t> ret = mHdmiCec->getCecVersion();
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to get cec version.");
    }
    return ret;
}

uint32_t HdmiCecController::getVendorId() {
    Return<uint32_t> ret = mHdmiCec->getVendorId();
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to get vendor id.");
    }
    return ret;
}

AutoPtr<ArrayOf<IHdmiPortInfo*> > HdmiCecController::getPortInfos() {
    hidl_vec<HdmiPortInfo> ports;
    Return<void> ret = mHdmiCec->getPortInfo([&ports](hidl_vec<HdmiPortInfo> list) {
            ports = list;
    });
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to get port information.");
        return NULL;
    }
    AutoPtr<ArrayOf<IHdmiPortInfo*> > res = ArrayOf<IHdmiPortInfo*>::Alloc(ports.size());

    // MHL support field will be obtained from MHL HAL. Leave it to false.
    Boolean mhlSupported = (Boolean) 0;
    for (size_t i = 0; i < ports.size(); ++i) {
        Boolean cecSupported = (Boolean) ports[i].cecSupported;
        Boolean arcSupported = (Boolean) ports[i].arcSupported;
        AutoPtr<IHdmiPortInfo> infoObj;
        // TODO: Waiting for CHdmiPortInfo
        assert(0);
        // CHdmiPortInfo::New(info->port_id, info->type,
        //         info->physical_address, cecSupported, mhlSupported, arcSupported, (IHdmiPortInfo**)&infoObj);
        res->Set(i, infoObj);

    }
    return res;
}

void HdmiCecController::setOption(OptionKey key, bool enabled) {
    Return<void> ret = mHdmiCec->setOption(key, enabled);
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to set option.");
    }
}

// Set audio return channel status.
void HdmiCecController::enableAudioReturnChannel(int port, bool enabled) {
    Return<void> ret = mHdmiCec->enableAudioReturnChannel(port, enabled);
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to enable/disable ARC.");
    }
}

// Whether to hdmi device is connected to the given port.
bool HdmiCecController::isConnected(int port) {
    Return<bool> ret = mHdmiCec->isConnected(port);
    if (!ret.isOk()) {
        Logger::E("HdmiCecController", "Failed to get connection info.");
    }
    return ret;
}

Return<void> HdmiCecController::HdmiCecCallback::onCecMessage(const CecMessage& message) {
    sp<HdmiCecEventHandler> handler(new HdmiCecEventHandler(mController, message));
    mController->mLooper->sendMessage(handler, HdmiCecEventHandler::EventType::CEC_MESSAGE);
    return android::hardware::Void();
}

Return<void> HdmiCecController::HdmiCecCallback::onHotplugEvent(const HotplugEvent& event) {
    sp<HdmiCecEventHandler> handler(new HdmiCecEventHandler(mController, event));
    mController->mLooper->sendMessage(handler, HdmiCecEventHandler::EventType::HOT_PLUG);
    return android::hardware::Void();
}

//------------------------------------------------------------------------------
static Int64 nativeInit(Elastos::Droid::Server::Hdmi::HdmiCecController* callbacksObj,
        IMessageQueue* messageQueueObj) {
    android::sp<IHdmiCec> hdmiCec = IHdmiCec::getService();
    if (hdmiCec == nullptr) {
        ALOGE("Couldn't get tv.cec service.");
        return 0;
    }

    Handle64 ptr;
    messageQueueObj->GetNativeMessageQueue(&ptr);
    AutoPtr<MessageQueue> messageQueue = reinterpret_cast<NativeMessageQueue*>(ptr);

    HdmiCecController* controller = new HdmiCecController(
            hdmiCec,
            callbacksObj,
            messageQueue->GetLooper());

    return reinterpret_cast<Int64>(controller);
}

static Int32 nativeSendCecCommand(Int64 controllerPtr,
        Int32 srcAddr, Int32 dstAddr, AutoPtr<ArrayOf<Byte> > body) {
    CecMessage message;
    message.initiator = static_cast<CecLogicalAddress>(srcAddr);
    message.destination = static_cast<CecLogicalAddress>(dstAddr);

    int len = body->GetLength();
    size_t bodyLength = MIN(len, CEC_MESSAGE_BODY_MAX_LENGTH);
    message.body.resize(bodyLength);
    for (size_t i = 0; i < bodyLength; ++i) {
        message.body[i] = static_cast<uint8_t>((*body)[i]);
    }

    HdmiCecController* controller =
            reinterpret_cast<HdmiCecController*>(controllerPtr);
    return controller->sendMessage(message);
}

static Int32 nativeAddLogicalAddress(Int64 controllerPtr,
        Int32 logicalAddress) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    return controller->addLogicalAddress(static_cast<CecLogicalAddress>(logicalAddress));
}

static void nativeClearLogicalAddress(Int64 controllerPtr) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    controller->clearLogicaladdress();
}

static Int32 nativeGetPhysicalAddress(Int64 controllerPtr) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    return controller->getPhysicalAddress();
}

static Int32 nativeGetVersion(Int64 controllerPtr) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    return controller->getVersion();
}

static Int32 nativeGetVendorId(Int64 controllerPtr) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    return controller->getVendorId();
}

static AutoPtr<ArrayOf<IHdmiPortInfo*> > nativeGetPortInfos(Int64 controllerPtr) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    return controller->getPortInfos();
}

static void nativeSetOption(Int64 controllerPtr, Int32 flag, Int32 value) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    controller->setOption(static_cast<OptionKey>(flag), value > 0 ? true : false);
}

static void nativeEnableAudioReturnChannel(Int64 controllerPtr, Int32 port,
        Boolean enabled) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    controller->enableAudioReturnChannel(port, enabled == TRUE);
}

static Boolean nativeIsConnected(Int64 controllerPtr, Int32 port) {
    HdmiCecController* controller = reinterpret_cast<HdmiCecController*>(controllerPtr);
    return controller->isConnected(port) ? TRUE : FALSE ;
}

} // namespace android

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiCecController::RemoteDevicePredicate
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecController::RemoteDevicePredicate, Object, IPredicate)

HdmiCecController::RemoteDevicePredicate::RemoteDevicePredicate(
    /* [in] */ HdmiCecController* host)
    : mHost(host)
{}

ECode HdmiCecController::RemoteDevicePredicate::Apply(
    /* [in] */ IInterface* address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 iAddress;
    IInteger32::Probe(address)->GetValue(&iAddress);
    mHost->IsAllocatedLocalDeviceAddress(iAddress, result);
    *result = !(*result);
    return NOERROR;
}

//=============================================================================
// HdmiCecController::SystemAudioPredicate
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecController::SystemAudioPredicate, Object, IPredicate)

ECode HdmiCecController::SystemAudioPredicate::Apply(
    /* [in] */ IInterface* address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 iAddress;
    IInteger32::Probe(address)->GetValue(&iAddress);
    *result = HdmiUtils::GetTypeFromAddress(iAddress) == Constants::ADDR_AUDIO_SYSTEM;
    return NOERROR;
}

//=============================================================================
// HdmiCecController::InnerSub_Runnable
//=============================================================================
HdmiCecController::InnerSub_Runnable::InnerSub_Runnable(
    /* [in] */ HdmiCecController* host,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 preferredAddress,
    /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback)
    : mHost(host)
    , mDeviceType(deviceType)
    , mPreferredAddress(preferredAddress)
    , mCallback(callback)
{}

ECode HdmiCecController::InnerSub_Runnable::Run()
{
    return mHost->HandleAllocateLogicalAddress(mDeviceType, mPreferredAddress, mCallback);
}

//=============================================================================
// HdmiCecController::HandleRunnable
//=============================================================================
HdmiCecController::HandleRunnable::HandleRunnable(
    /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 assignedAddress)
    : mCallback(callback)
    , mDeviceType(deviceType)
    , mAssignedAddress(assignedAddress)
{}

ECode HdmiCecController::HandleRunnable::Run()
{
    return mCallback->OnAllocated(mDeviceType, mAssignedAddress);
}

//=============================================================================
// HdmiCecController::DevRunnable::ServiceRunnable
//=============================================================================
HdmiCecController::DevRunnable::ServiceRunnable::ServiceRunnable(
    /* [in] */ DevRunnable* host,
    /* [in] */ Int32 sourceAddress,
    /* [in] */ IList* candidates,
    /* [in] */ Int32 retryCount,
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ IList* allocated)
    : mHost(host)
    , mSourceAddress(sourceAddress)
    , mCandidates(candidates)
    , mRetryCount(retryCount)
    , mCallback(callback)
    , mAllocated(allocated)
{}

ECode HdmiCecController::DevRunnable::ServiceRunnable::Run()
{
    return mHost->mHost->RunDevicePolling(mSourceAddress, mCandidates, mRetryCount, mCallback,
            mAllocated);
}

//=============================================================================
// HdmiCecController::DevRunnable
//=============================================================================
HdmiCecController::DevRunnable::DevRunnable(
    /* [in] */ HdmiCecController* host,
    /* [in] */ Int32 sourceAddress,
    /* [in] */ IInteger32* candidate,
    /* [in] */ Int32 retryCount,
    /* [in] */ IList* allocated,
    /* [in] */ IList* candidates,
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback)
    : mHost(host)
    , mSourceAddress(sourceAddress)
    , mCandidate(candidate)
    , mRetryCount(retryCount)
    , mAllocated(allocated)
    , mCandidates(candidates)
    , mCallback(callback)
{}

ECode HdmiCecController::DevRunnable::Run()
{
    Boolean isSendMsgOk;
    Int32 iCandidate;
    mCandidate->GetValue(&iCandidate);
    mHost->SendPollMessage(mSourceAddress, iCandidate, mRetryCount, &isSendMsgOk);
    if (isSendMsgOk) {
        mAllocated->Add(mCandidate);
    }
    mHost->RunOnServiceThread(new ServiceRunnable(this, mSourceAddress, mCandidates, mRetryCount, mCallback, mAllocated));
    return NOERROR;
}

//=============================================================================
// HdmiCecController::IoRunnable::ServiceThreadRunnable
//=============================================================================
HdmiCecController::IoRunnable::ServiceThreadRunnable::ServiceThreadRunnable(
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback,
    /* [in] */ Int32 finalError)
    : mCallback(callback)
    , mFinalError(finalError)
{}

ECode HdmiCecController::IoRunnable::ServiceThreadRunnable::Run()
{
    return mCallback->OnSendCompleted(mFinalError);
}

//=============================================================================
// HdmiCecController::IoRunnable
//=============================================================================
HdmiCecController::IoRunnable::IoRunnable(
    /* [in] */ HdmiCecController* host,
    /* [in] */ IHdmiCecMessage* cecMessage,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
    : mHost(host)
    , mCecMessage(cecMessage)
    , mCallback(callback)
{}

ECode HdmiCecController::IoRunnable::Run()
{
    HdmiLogger::Debug("[S]:%s", TO_CSTR(mCecMessage));
    Int32 opcode;
    mCecMessage->GetOpcode(&opcode);
    AutoPtr<ArrayOf<Byte> > body;
    AutoPtr<ArrayOf<Byte> > params;
    mCecMessage->GetParams((ArrayOf<Byte>**)&params);
    BuildBody(opcode, params, (ArrayOf<Byte>**)&body);
    Int32 i = 0;
    Int32 errorCode = Constants::SEND_RESULT_SUCCESS;
    do {
        Int32 dest;
        mCecMessage->GetDestination(&dest);
        Int32 srcAddr;
        mCecMessage->GetSource(&srcAddr);
        NativeSendCecCommand(mHost->mNativePtr, srcAddr,
                dest, body, &errorCode);
        if (errorCode == Constants::SEND_RESULT_SUCCESS) {
            break;
        }
    } while (i++ < HdmiConfig::RETRANSMISSION_COUNT);
    const Int32 finalError = errorCode;
    if (finalError != Constants::SEND_RESULT_SUCCESS) {
        Slogger::W(TAG, "Failed to send %s", Object::ToString(mCecMessage).string());
    }
    if (mCallback != NULL) {
        mHost->RunOnServiceThread(new ServiceThreadRunnable(mCallback, finalError));
    }
    return NOERROR;
}

//=============================================================================
// HdmiCecController
//=============================================================================
const String HdmiCecController::TAG("HdmiCecController");
const AutoPtr<ArrayOf<Byte> > HdmiCecController::EMPTY_BODY = EmptyArray::BYTE;
const Int32 HdmiCecController::NUM_LOGICAL_ADDRESS = 16;

HdmiCecController::HdmiCecController(
    /* [in] */ IHdmiControlService* service)
    : mService(service)
{
    mRemoteDeviceAddressPredicate = new RemoteDevicePredicate(this);
    mSystemAudioAddressPredicate = new SystemAudioPredicate();
    CSparseArray::New((ISparseArray**)&mLocalDevices);
}

ECode HdmiCecController::Create(
    /* [in] */ IHdmiControlService* service,
    /* [out] */ HdmiCecController** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<HdmiCecController> controller = new HdmiCecController(service);
    AutoPtr<ILooper> serviceLooper;
    ((HdmiControlService*)service)->GetServiceLooper((ILooper**)&serviceLooper);
    AutoPtr<IMessageQueue> msgQueue;
    serviceLooper->GetQueue((IMessageQueue**)&msgQueue);
    Int64 nativePtr;
    NativeInit(controller, msgQueue, &nativePtr);
    if (nativePtr == 0L) {
        controller = NULL;
        *result = NULL;
        return NOERROR;
    }
    controller->Init(nativePtr);
    *result = controller;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecController::Init(
    /* [in] */ Int64 nativePtr)
{
    AutoPtr<ILooper> serviceLooper;
    ((HdmiControlService*)mService.Get())->GetServiceLooper((ILooper**)&serviceLooper);
    CHandler::New(serviceLooper, (IHandler**)&mIoHandler);
    CHandler::New(serviceLooper, (IHandler**)&mControlHandler);
    mNativePtr = nativePtr;
    return NOERROR;
}

ECode HdmiCecController::AddLocalDevice(
    /* [in] */ Int32 deviceType,
    /* [in] */ IHdmiCecLocalDevice* device)
{
    AssertRunOnServiceThread();
    mLocalDevices->Put(deviceType, device);
    return NOERROR;
}

ECode HdmiCecController::AllocateLogicalAddress(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 preferredAddress,
    /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback)
{
    AssertRunOnServiceThread();
    RunOnIoThread(new InnerSub_Runnable(this, deviceType, preferredAddress, callback));
    return NOERROR;
}

ECode HdmiCecController::HandleAllocateLogicalAddress(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 preferredAddress,
    /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback)
{
    AssertRunOnIoThread();
    Int32 startAddress = preferredAddress;
    // If preferred address is "unregistered", start address will be the smallest
    // address matched with the given device type.
    if (preferredAddress == Constants::ADDR_UNREGISTERED) {
        for (Int32 i = 0; i < NUM_LOGICAL_ADDRESS; ++i) {
            if (deviceType == HdmiUtils::GetTypeFromAddress(i)) {
                startAddress = i;
                break;
            }
        }
    }
    Int32 logicalAddress = Constants::ADDR_UNREGISTERED;
    // Iterates all possible addresses which has the same device type.
    for (Int32 i = 0; i < NUM_LOGICAL_ADDRESS; ++i) {
        Int32 curAddress = (startAddress + i) % NUM_LOGICAL_ADDRESS;
        if (curAddress != Constants::ADDR_UNREGISTERED
                && deviceType == HdmiUtils::GetTypeFromAddress(curAddress)) {
            Int32 failedPollingCount = 0;
            for (Int32 j = 0; j < HdmiConfig::ADDRESS_ALLOCATION_RETRY; ++j) {
                Boolean isSendPollMessageOk;
                SendPollMessage(curAddress, curAddress, 1, &isSendPollMessageOk);
                if (!isSendPollMessageOk) {
                    failedPollingCount++;
                }
            }
            // Pick logical address if failed ratio is more than a half of all retries.
            if (failedPollingCount * 2 >  HdmiConfig::ADDRESS_ALLOCATION_RETRY) {
                logicalAddress = curAddress;
                break;
            }
        }
    }
    const Int32 assignedAddress = logicalAddress;
    HdmiLogger::Debug("New logical address for device [%d]: [preferred:%d, assigned:%d]", deviceType, preferredAddress, assignedAddress);
    if (callback != NULL) {
        RunOnServiceThread(new HandleRunnable(callback, deviceType, assignedAddress));
    }
    return NOERROR;
}

ECode HdmiCecController::BuildBody(
    /* [in] */ Int32 opcode,
    /* [in] */ ArrayOf<Byte>* params,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    AutoPtr<ArrayOf<Byte> > body = ArrayOf<Byte>::Alloc(params->GetLength() + 1);
    (*body)[0] = (Byte) opcode;
    body->Copy(1, params, 0, params->GetLength());
    *result = body;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecController::GetPortInfos(
    /* [out, callee] */ ArrayOf<IHdmiPortInfo*>** result)
{
    return NativeGetPortInfos(mNativePtr, result);
}

ECode HdmiCecController::GetLocalDevice(
    /* [in] */ Int32 deviceType,
    /* [out] */ IHdmiCecLocalDevice** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mLocalDevices->Get(deviceType, (IInterface**)&obj);
    *result = IHdmiCecLocalDevice::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecController::AddLogicalAddress(
    /* [in] */ Int32 newLogicalAddress,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    if (HdmiUtils::IsValidAddress(newLogicalAddress)) {
        return NativeAddLogicalAddress(mNativePtr, newLogicalAddress, result);
    } else {
        *result = -1;
        return NOERROR;
    }
    return NOERROR;
}

ECode HdmiCecController::ClearLogicalAddress()
{
    AssertRunOnServiceThread();
    Int32 size;
    mLocalDevices->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mLocalDevices->ValueAt(i, (IInterface**)&obj);
        ((HdmiCecLocalDevice*)IHdmiCecLocalDevice::Probe(obj))->ClearAddress();
    }
    NativeClearLogicalAddress(mNativePtr);
    return NOERROR;
}

ECode HdmiCecController::ClearLocalDevices()
{
    AssertRunOnServiceThread();
    mLocalDevices->Clear();
    return NOERROR;
}

ECode HdmiCecController::GetPhysicalAddress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    return NativeGetPhysicalAddress(mNativePtr, result);
}

ECode HdmiCecController::GetVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    return NativeGetVersion(mNativePtr, result);
}

ECode HdmiCecController::GetVendorId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    return NativeGetVendorId(mNativePtr, result);
}

ECode HdmiCecController::SetOption(
    /* [in] */ Int32 flag,
    /* [in] */ Int32 value)
{
    AssertRunOnServiceThread();
    NativeSetOption(mNativePtr, flag, value);
    return NOERROR;
}

ECode HdmiCecController::EnableAudioReturnChannel(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean enabled)
{
    AssertRunOnServiceThread();
    NativeEnableAudioReturnChannel(mNativePtr, portId, enabled);
    return NOERROR;
}

ECode HdmiCecController::IsConnected(
    /* [in] */ Int32 port,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    return NativeIsConnected(mNativePtr, port, result);
}

ECode HdmiCecController::PollDevices(
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ Int32 sourceAddress,
    /* [in] */ Int32 pickStrategy,
    /* [in] */ Int32 retryCount)
{
    AssertRunOnServiceThread();
    // Extract polling candidates. No need to poll against local devices.
    AutoPtr<IList> pollingCandidates;
    PickPollCandidates(pickStrategy, (IList**)&pollingCandidates);
    AutoPtr<IArrayList> allocated;
    CArrayList::New((IArrayList**)&allocated);
    RunDevicePolling(sourceAddress, pollingCandidates, retryCount, callback, IList::Probe(allocated));
    return NOERROR;
}

ECode HdmiCecController::GetLocalDeviceList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    AutoPtr<IList> temp = HdmiUtils::SparseArrayToList(mLocalDevices);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecController::PickPollCandidates(
    /* [in] */ Int32 pickStrategy,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 strategy = pickStrategy & Constants::POLL_STRATEGY_MASK;
    AutoPtr<IPredicate> pickPredicate;
    if (strategy == Constants::POLL_STRATEGY_SYSTEM_AUDIO) {
        pickPredicate = mSystemAudioAddressPredicate;
    }
    else {
        // The default is POLL_STRATEGY_REMOTES_DEVICES.
        pickPredicate = mRemoteDeviceAddressPredicate;
    }
    Int32 iterationStrategy = pickStrategy & Constants::POLL_ITERATION_STRATEGY_MASK;
    AutoPtr<ILinkedList> pollingCandidates;
    CLinkedList::New((ILinkedList**)&pollingCandidates);
    if (iterationStrategy == Constants::POLL_ITERATION_IN_ORDER) {
        for (Int32 i = Constants::ADDR_TV; i <= Constants::ADDR_SPECIFIC_USE; ++i) {
            AutoPtr<IInteger32> i32;
            CInteger32::New(i, (IInteger32**)&i32);
            Boolean bApply;
            pickPredicate->Apply(i32, &bApply);
            if (bApply) {
                pollingCandidates->Add(i32);
            }
        }
    }
    else {
        // The default is reverse order.
        for (Int32 i = Constants::ADDR_SPECIFIC_USE; i >= Constants::ADDR_TV; --i) {
            AutoPtr<IInteger32> i32;
            CInteger32::New(i, (IInteger32**)&i32);
            Boolean bApply;
            pickPredicate->Apply(i32, &bApply);
            if (bApply) {
                pollingCandidates->Add(i32);
            }
        }
    }
    *result = IList::Probe(pollingCandidates);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecController::IsAllocatedLocalDeviceAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    Int32 size;
    mLocalDevices->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mLocalDevices->ValueAt(i, (IInterface**)&obj);
        Boolean isAddressOf;
        ((HdmiCecLocalDevice*)IHdmiCecLocalDevice::Probe(obj))->IsAddressOf(address, &isAddressOf);
        if (isAddressOf) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecController::RunDevicePolling(
    /* [in] */ Int32 sourceAddress,
    /* [in] */ IList* candidates,
    /* [in] */ Int32 retryCount,
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ IList* allocated)
{
    AssertRunOnServiceThread();
    Boolean isEmpty;
    candidates->IsEmpty(&isEmpty);
    if (isEmpty) {
        if (callback != NULL) {
            HdmiLogger::Debug("[P]:AllocatedAddress=%s", TO_CSTR(allocated));
            callback->OnPollingFinished(allocated);
        }
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    candidates->Remove(0, (IInterface**)&obj);
    AutoPtr<IInteger32> candidate = IInteger32::Probe(obj);
    // Proceed polling action for the next address once polling action for the
    // previous address is done.
    RunOnIoThread(new DevRunnable(this, sourceAddress, candidate, retryCount, allocated, candidates, callback));
    return NOERROR;
}

ECode HdmiCecController::SendPollMessage(
    /* [in] */ Int32 sourceAddress,
    /* [in] */ Int32 destinationAddress,
    /* [in] */ Int32 retryCount,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnIoThread();
    for (Int32 i = 0; i < retryCount; ++i) {
        // <Polling Message> is a message which has empty body.
        // If sending <Polling Message> failed (NAK), it becomes
        // new logical address for the device because no device uses
        // it as logical address of the device.
        Int32 cmd;
        NativeSendCecCommand(mNativePtr, sourceAddress, destinationAddress, EMPTY_BODY, &cmd);
        if (cmd == Constants::SEND_RESULT_SUCCESS) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecController::AssertRunOnIoThread()
{
    AutoPtr<ILooper> looper;
    mIoHandler->GetLooper((ILooper**)&looper);
    if (Looper::GetMyLooper() != looper) {
        Logger::E(TAG, "Should run on io thread.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiCecController::AssertRunOnServiceThread()
{
    AutoPtr<ILooper> looper;
    mControlHandler->GetLooper((ILooper**)&looper);
    if (Looper::GetMyLooper() != looper) {
        Logger::E(TAG, "Should run on service thread.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiCecController::RunOnIoThread(
    /* [in] */ IRunnable* runnable)
{
    Boolean bNoUse;
    return mIoHandler->Post(runnable, &bNoUse);
}

ECode HdmiCecController::RunOnServiceThread(
    /* [in] */ IRunnable* runnable)
{
    Boolean bNoUse;
    return mControlHandler->Post(runnable, &bNoUse);
}

ECode HdmiCecController::IsAcceptableAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Can access command targeting devices available in local device or broadcast command.
    if (address == Constants::ADDR_BROADCAST) {
        *result = TRUE;
        return NOERROR;
    }
    return IsAllocatedLocalDeviceAddress(address, result);
}

ECode HdmiCecController::OnReceiveCommand(
    /* [in] */ IHdmiCecMessage* message)
{
    AssertRunOnServiceThread();
    Int32 dest;
    message->GetDestination(&dest);
    Boolean isAcceptableAddress;
    IsAcceptableAddress(dest, &isAcceptableAddress);
    Boolean isHandleCecCommandOk;
    ((HdmiControlService*)mService.Get())->HandleCecCommand(message, &isHandleCecCommandOk);
    if (isAcceptableAddress && isHandleCecCommandOk) {
        return NOERROR;
    }
    // Not handled message, so we will reply it with <Feature Abort>.
    MaySendFeatureAbortCommand(message, Constants::ABORT_UNRECOGNIZED_OPCODE);
    return NOERROR;
}

ECode HdmiCecController::MaySendFeatureAbortCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [in] */ Int32 reason)
{
    AssertRunOnServiceThread();
    // Swap the source and the destination.
    Int32 src;
    message->GetDestination(&src);
    Int32 dest;
    message->GetSource(&dest);
    if (src == Constants::ADDR_BROADCAST || dest == Constants::ADDR_UNREGISTERED) {
        // Don't reply <Feature Abort> from the unregistered devices or for the broadcasted
        // messages. See CEC 12.2 Protocol General Rules for detail.
        return NOERROR;
    }
    Int32 originalOpcode;
    message->GetOpcode(&originalOpcode);
    if (originalOpcode == Constants::MESSAGE_FEATURE_ABORT) {
        return NOERROR;
    }
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildFeatureAbortCommand(src, dest, originalOpcode, reason, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    return NOERROR;
}

ECode HdmiCecController::SendCommand(
    /* [in] */ IHdmiCecMessage* cecMessage)
{
    AssertRunOnServiceThread();
    SendCommand(cecMessage, NULL);
    return NOERROR;
}

ECode HdmiCecController::SendCommand(
    /* [in] */ IHdmiCecMessage* cecMessage,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    AssertRunOnServiceThread();
    RunOnIoThread(new IoRunnable(this, cecMessage, callback));
    return NOERROR;
}

ECode HdmiCecController::HandleIncomingCecCommand(
    /* [in] */ Int32 srcAddress,
    /* [in] */ Int32 dstAddress,
    /* [in] */ ArrayOf<Byte>* body)
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiCecMessage> command;
    HdmiCecMessageBuilder::Of(srcAddress, dstAddress, body, (IHdmiCecMessage**)&command);
    HdmiLogger::Debug("[R]:%s", TO_CSTR(command));
    OnReceiveCommand(command);
    return NOERROR;
}

ECode HdmiCecController::HandleHotplug(
    /* [in] */ Int32 port,
    /* [in] */ Boolean connected)
{
    AssertRunOnServiceThread();
    HdmiLogger::Debug("Hotplug event:[port:%d, connected:%b]", port, connected);
    ((HdmiControlService*)mService.Get())->OnHotplug(port, connected);
    return NOERROR;
}

ECode HdmiCecController::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    Int32 size;
    mLocalDevices->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        String s;
        s.AppendFormat("HdmiCecLocalDevice #%d:", i);
        IPrintWriter::Probe(pw)->Println(s);
        pw->IncreaseIndent();
        AutoPtr<IInterface> obj;
        mLocalDevices->ValueAt(i, (IInterface**)&obj);
        ((HdmiCecLocalDevice*) IHdmiCecLocalDevice::Probe(obj))->Dump(pw);
        pw->DecreaseIndent();
    }
    return NOERROR;
}

ECode HdmiCecController::NativeInit(
    /* [in] */ HdmiCecController* handler,
    /* [in] */ IMessageQueue* messageQueue,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = android::nativeInit(handler, messageQueue);
    return NOERROR;
}

ECode HdmiCecController::NativeSendCecCommand(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 srcAddress,
    /* [in] */ Int32 dstAddress,
    /* [in] */ ArrayOf<Byte>* body,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = android::nativeSendCecCommand(controllerPtr, srcAddress, dstAddress, body);
    return NOERROR;
}

ECode HdmiCecController::NativeAddLogicalAddress(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 logicalAddress,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = android::nativeAddLogicalAddress(controllerPtr, logicalAddress);
    return NOERROR;
}

ECode HdmiCecController::NativeClearLogicalAddress(
    /* [in] */ Int64 controllerPtr)
{
    android::nativeClearLogicalAddress(controllerPtr);
    return NOERROR;
}

ECode HdmiCecController::NativeGetPhysicalAddress(
    /* [in] */ Int64 controllerPtr,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = android::nativeGetPhysicalAddress(controllerPtr);
    return NOERROR;
}

ECode HdmiCecController::NativeGetVersion(
    /* [in] */ Int64 controllerPtr,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = android::nativeGetVersion(controllerPtr);
    return NOERROR;
}

ECode HdmiCecController::NativeGetVendorId(
    /* [in] */ Int64 controllerPtr,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = android::nativeGetVendorId(controllerPtr);
    return NOERROR;
}

ECode HdmiCecController::NativeGetPortInfos(
    /* [in] */ Int64 controllerPtr,
    /* [out, callee] */ ArrayOf<IHdmiPortInfo*>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr< ArrayOf<IHdmiPortInfo*> > portInfos = android::nativeGetPortInfos(controllerPtr);
    *result = portInfos;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode HdmiCecController::NativeSetOption(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 flag,
    /* [in] */ Int32 value)
{
    android::nativeSetOption(controllerPtr, flag, value);
    return NOERROR;
}

ECode HdmiCecController::NativeEnableAudioReturnChannel(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 portId,
    /* [in] */ Boolean flag)
{
    android::nativeEnableAudioReturnChannel(controllerPtr, portId, flag);
    return NOERROR;
}

ECode HdmiCecController::NativeIsConnected(
    /* [in] */ Int64 controllerPtr,
    /* [in] */ Int32 port,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = android::nativeIsConnected(controllerPtr, port);
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

