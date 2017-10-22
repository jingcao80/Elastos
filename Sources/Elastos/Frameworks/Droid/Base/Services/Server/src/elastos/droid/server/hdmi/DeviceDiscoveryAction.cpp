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

#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/DeviceDiscoveryAction.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageCache.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/core/Object.h>
#include <elastos/utility/logging/Slogger.h>

// using Elastos::Droid::Hardware::Hdmi::CHdmiDeviceInfo;
using Elastos::Core::IInteger32;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// DeviceDiscoveryAction::DeviceInfo
//=============================================================================
DeviceDiscoveryAction::DeviceInfo::DeviceInfo(
    /* [in] */ Int32 logicalAddress)
    : mLogicalAddress(logicalAddress)
    , mPhysicalAddress(Constants::INVALID_PHYSICAL_ADDRESS)
    , mPortId(Constants::INVALID_PORT_ID)
    , mVendorId(Constants::UNKNOWN_VENDOR_ID)
    , mDisplayName("")
    , mDeviceType(IHdmiDeviceInfo::DEVICE_INACTIVE)
{}

ECode DeviceDiscoveryAction::DeviceInfo::ToHdmiDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)

    // TODO: Waiting for CHdmiDeviceInfo
    assert(0);
    // return CHdmiDeviceInfo::New(mLogicalAddress, mPhysicalAddress, mPortId, mDeviceType,
    //         mVendorId, mDisplayName, result);
    return NOERROR;
}

//=============================================================================
// DeviceDiscoveryAction::InnerSub_DevicePollingCallback
//=============================================================================
CAR_INTERFACE_IMPL(DeviceDiscoveryAction::InnerSub_DevicePollingCallback, Object, IDeviceDiscoveryActionDeviceDiscoveryCallback)

DeviceDiscoveryAction::InnerSub_DevicePollingCallback::InnerSub_DevicePollingCallback(
    /* [in] */ DeviceDiscoveryAction* host)
    : mHost(host)
{}

ECode DeviceDiscoveryAction::InnerSub_DevicePollingCallback::OnPollingFinished(
    /* [in] */ IList* ackedAddress)
{
    Boolean isEmpty;
    ackedAddress->IsEmpty(&isEmpty);
    if (isEmpty) {
        Slogger::V(TAG, "No device is detected.");
        mHost->WrapUpAndFinish();
        return NOERROR;
    }
    Slogger::V(TAG, "Device detected: %s", Object::ToString(ackedAddress).string());
    mHost->AllocateDevices(ackedAddress);
    mHost->StartPhysicalAddressStage();
    return NOERROR;
}

//=============================================================================
// DeviceDiscoveryAction
//=============================================================================
CAR_INTERFACE_IMPL(DeviceDiscoveryAction, HdmiCecFeatureAction, IDeviceDiscoveryAction)

const String DeviceDiscoveryAction::TAG("DeviceDiscoveryAction");
const Int32 DeviceDiscoveryAction::STATE_WAITING_FOR_DEVICE_POLLING = 1;
const Int32 DeviceDiscoveryAction::STATE_WAITING_FOR_PHYSICAL_ADDRESS = 2;
const Int32 DeviceDiscoveryAction::STATE_WAITING_FOR_OSD_NAME = 3;
const Int32 DeviceDiscoveryAction::STATE_WAITING_FOR_VENDOR_ID = 4;

DeviceDiscoveryAction::DeviceDiscoveryAction()
    : mProcessedDeviceCount(0)
{
    CArrayList::New((IArrayList**)&mDevices);
}

ECode DeviceDiscoveryAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ IDeviceDiscoveryActionDeviceDiscoveryCallback* callback)
{
    HdmiCecFeatureAction::constructor(source);
    AutoPtr<IPreconditions> helper;
    CPreconditions::AcquireSingleton((IPreconditions**)&helper);
    ECode ec = helper->CheckNotNull(callback);
    if (FAILED(ec)) return ec;
    mCallback = callback;
    return NOERROR;
}

ECode DeviceDiscoveryAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mDevices->Clear();
    mState = STATE_WAITING_FOR_DEVICE_POLLING;
    PollDevices(new InnerSub_DevicePollingCallback(this) , Constants::POLL_ITERATION_REVERSE_ORDER
        | Constants::POLL_STRATEGY_REMOTES_DEVICES, HdmiConfig::DEVICE_POLLING_RETRY);
    *result = TRUE;
    return NOERROR;
}

ECode DeviceDiscoveryAction::AllocateDevices(
    /* [in] */ IList* addresses)
{
    AutoPtr<IIterator> it;
    addresses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IInteger32> i32 = IInteger32::Probe(obj);
        Int32 i;
        i32->GetValue(&i);
        AutoPtr<DeviceInfo> info = new DeviceInfo(i);
        mDevices->Add(TO_IINTERFACE(info));
    }
    return NOERROR;
}

ECode DeviceDiscoveryAction::StartPhysicalAddressStage()
{
    Int32 size;
    mDevices->GetSize(&size);
    Slogger::V(TAG, "Start [Physical Address Stage]:%d", size);
    mProcessedDeviceCount = 0;
    mState = STATE_WAITING_FOR_PHYSICAL_ADDRESS;
    CheckAndProceedStage();
    return NOERROR;
}

Boolean DeviceDiscoveryAction::VerifyValidLogicalAddress(
    /* [in] */ Int32 address)
{
    return address >= Constants::ADDR_TV && address < Constants::ADDR_UNREGISTERED;
}

ECode DeviceDiscoveryAction::QueryPhysicalAddress(
    /* [in] */ Int32 address)
{
    if (!VerifyValidLogicalAddress(address)) {
        CheckAndProceedStage();
        return NOERROR;
    }
    mActionTimer->ClearTimerMessage();
    // Check cache first and send request if not exist.
    if (MayProcessMessageIfCached(address, Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS)) {
        return NOERROR;
    }
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> msg;
    HdmiCecMessageBuilder::BuildGivePhysicalAddress(srcAddr, address, (IHdmiCecMessage**)&msg);
    SendCommand(msg);
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    return NOERROR;
}

ECode DeviceDiscoveryAction::StartOsdNameStage()
{
    Int32 size;
    mDevices->GetSize(&size);
    Slogger::V(TAG, "Start [Osd Name Stage]:%d", size);
    mProcessedDeviceCount = 0;
    mState = STATE_WAITING_FOR_OSD_NAME;
    CheckAndProceedStage();
    return NOERROR;
}

ECode DeviceDiscoveryAction::QueryOsdName(
    /* [in] */ Int32 address)
{
    if (!VerifyValidLogicalAddress(address)) {
        CheckAndProceedStage();
        return NOERROR;
    }
    mActionTimer->ClearTimerMessage();
    if (MayProcessMessageIfCached(address, Constants::MESSAGE_SET_OSD_NAME)) {
        return NOERROR;
    }
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> msg;
    HdmiCecMessageBuilder::BuildGivePhysicalAddress(srcAddr, address, (IHdmiCecMessage**)&msg);
    SendCommand(msg);
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    return NOERROR;
}

ECode DeviceDiscoveryAction::StartVendorIdStage()
{
    Int32 size;
    mDevices->GetSize(&size);
    Slogger::V(TAG, "Start [Vendor Id Stage]:%d", size);
    mProcessedDeviceCount = 0;
    mState = STATE_WAITING_FOR_VENDOR_ID;
    CheckAndProceedStage();
    return NOERROR;
}

ECode DeviceDiscoveryAction::QueryVendorId(
    /* [in] */ Int32 address)
{
    if (!VerifyValidLogicalAddress(address)) {
        CheckAndProceedStage();
        return NOERROR;
    }
    mActionTimer->ClearTimerMessage();
    if (MayProcessMessageIfCached(address, Constants::MESSAGE_DEVICE_VENDOR_ID)) {
        return NOERROR;
    }
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> msg;
    HdmiCecMessageBuilder::BuildGiveDeviceVendorIdCommand(srcAddr, address, (IHdmiCecMessage**)&msg);
    SendCommand(msg);
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    return NOERROR;
}

Boolean DeviceDiscoveryAction::MayProcessMessageIfCached(
    /* [in] */ Int32 address,
    /* [in] */ Int32 opcode)
{
    AutoPtr<IHdmiCecMessage> message;
    AutoPtr<HdmiCecMessageCache> cache;
    GetCecMessageCache((HdmiCecMessageCache**)&cache);
    cache->GetMessage(address, opcode, (IHdmiCecMessage**)&message);
    if (message != NULL) {
        Boolean bNoUse;
        ProcessCommand(message, &bNoUse);
        return TRUE;
    }
    return FALSE;
}

ECode DeviceDiscoveryAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 opcode;
    cmd->GetOpcode(&opcode);
    switch (mState) {
        case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
            if (opcode == Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS) {
                HandleReportPhysicalAddress(cmd);
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;
        case STATE_WAITING_FOR_OSD_NAME:
            if (opcode == Constants::MESSAGE_SET_OSD_NAME) {
                HandleSetOsdName(cmd);
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;
        case STATE_WAITING_FOR_VENDOR_ID:
            if (opcode == Constants::MESSAGE_DEVICE_VENDOR_ID) {
                HandleVendorId(cmd);
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;
        case STATE_WAITING_FOR_DEVICE_POLLING:
            // Fall through.
        default:
            *result = FALSE;
            return NOERROR;
    }
    return NOERROR;
}

ECode DeviceDiscoveryAction::HandleReportPhysicalAddress(
    /* [in] */ IHdmiCecMessage* cmd)
{
    Int32 size;
    mDevices->GetSize(&size);
        AutoPtr<IPreconditions> helper;
        CPreconditions::AcquireSingleton((IPreconditions**)&helper);
        helper->CheckState(mProcessedDeviceCount < size);
        AutoPtr<IInterface> obj;
        mDevices->Get(mProcessedDeviceCount, (IInterface**)&obj);
        AutoPtr<DeviceInfo> current = (DeviceInfo*) IObject::Probe(obj);
        Int32 srcAddr;
        cmd->GetSource(&srcAddr);
        if (current->mLogicalAddress != srcAddr) {
            Slogger::W(TAG, "Unmatched address[expected:%d, actual:%d",
                    current->mLogicalAddress, srcAddr);
            return NOERROR;
        }
        AutoPtr<ArrayOf<Byte> > params;
        cmd->GetParams((ArrayOf<Byte>**)&params);
        current->mPhysicalAddress = HdmiUtils::TwoBytesToInt32(params);
        GetPortId(current->mPhysicalAddress, &current->mPortId);
        current->mDeviceType = (*params)[2] & 0xFF;
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean bNoUse;
        ((HdmiCecLocalDeviceTv*) tv.Get())->UpdateCecSwitchInfo(current->mLogicalAddress, current->mDeviceType,
                    current->mPhysicalAddress, &bNoUse);
        IncreaseProcessedDeviceCount();
        CheckAndProceedStage();
        return NOERROR;
}

ECode DeviceDiscoveryAction::GetPortId(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    return ((HdmiCecLocalDeviceTv*) tv.Get())->GetPortId(physicalAddress, result);
}

ECode DeviceDiscoveryAction::HandleSetOsdName(
    /* [in] */ IHdmiCecMessage* cmd)
{
    Int32 size;
    mDevices->GetSize(&size);
    AutoPtr<IPreconditions> helper;
    CPreconditions::AcquireSingleton((IPreconditions**)&helper);
    ECode ec = helper->CheckState(mProcessedDeviceCount < size);
    if (FAILED(ec)) return ec;
    AutoPtr<IInterface> obj;
    mDevices->Get(mProcessedDeviceCount, (IInterface**)&obj);
    AutoPtr<DeviceInfo> current = (DeviceInfo*) IObject::Probe(obj);
    Int32 srcAddr;
    cmd->GetSource(&srcAddr);
    if (current->mLogicalAddress != srcAddr) {
        Slogger::W(TAG, "Unmatched address[expected:%d, actual:%d", current->mLogicalAddress, srcAddr);
        return NOERROR;
    }
    String displayName(NULL);
    // try {
    do {
        AutoPtr<ArrayOf<Byte> > params;
        ec = cmd->GetParams((ArrayOf<Byte>**)&params);
        if (FAILED(ec)) break;
        // displayName = new String(params, "US-ASCII");
        displayName = String(*params);
    } while(FALSE);
    // } catch (UnsupportedEncodingException e) {
    if (FAILED(ec)) {
        if ((ECode) E_UNSUPPORTED_ENCODING_EXCEPTION == ec) {
            Slogger::W(TAG, "Failed to decode display name: %s", Object::ToString(cmd).string());
            // If failed to get display name, use the default name of device.
            displayName = HdmiUtils::GetDefaultDeviceName(current->mLogicalAddress);
        }
        else
            return ec;
    }
    // }
    current->mDisplayName = displayName;
    IncreaseProcessedDeviceCount();
    CheckAndProceedStage();
    return NOERROR;
}

ECode DeviceDiscoveryAction::HandleVendorId(
    /* [in] */ IHdmiCecMessage* cmd)
{
    Int32 size;
    mDevices->GetSize(&size);
    AutoPtr<IPreconditions> helper;
    CPreconditions::AcquireSingleton((IPreconditions**)&helper);
    ECode ec = helper->CheckState(mProcessedDeviceCount < size);
    if (FAILED(ec)) return ec;
    AutoPtr<IInterface> obj;
    mDevices->Get(mProcessedDeviceCount, (IInterface**)&obj);
    AutoPtr<DeviceInfo> current = (DeviceInfo*) IObject::Probe(obj);
    Int32 srcAddr;
    cmd->GetSource(&srcAddr);
    if (current->mLogicalAddress != srcAddr) {
        Slogger::W(TAG, "Unmatched address[expected:%d, actual:%d", current->mLogicalAddress, srcAddr);
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);
    Int32 vendorId = HdmiUtils::ThreeBytesToInt32(params);
    current->mVendorId = vendorId;
    IncreaseProcessedDeviceCount();
    CheckAndProceedStage();
    return NOERROR;
}

ECode DeviceDiscoveryAction::IncreaseProcessedDeviceCount()
{
    mProcessedDeviceCount++;
    return NOERROR;
}

ECode DeviceDiscoveryAction::RemoveDevice(
    /* [in] */ Int32 index)
{
    mDevices->Remove(index);
    return NOERROR;
}

ECode DeviceDiscoveryAction::WrapUpAndFinish()
{
    Int32 size;
    mDevices->GetSize(&size);
    Slogger::V(TAG, "---------Wrap up Device Discovery:[%d]---------", size);
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    AutoPtr<IIterator> it;
    mDevices->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<DeviceInfo> info = (DeviceInfo*) IObject::Probe(obj);
        AutoPtr<IHdmiDeviceInfo> cecDeviceInfo;
        info->ToHdmiDeviceInfo((IHdmiDeviceInfo**)&cecDeviceInfo);
        Slogger::V(TAG, " DeviceInfo: %s", Object::ToString(cecDeviceInfo).string());
        result->Add(cecDeviceInfo);
    }
    Slogger::V(TAG, "--------------------------------------------");
    mCallback->OnDeviceDiscoveryDone(IList::Probe(result));
    Finish();
    return NOERROR;
}

ECode DeviceDiscoveryAction::CheckAndProceedStage()
{
    Boolean isEmpty;
    mDevices->IsEmpty(&isEmpty);
    if (isEmpty) {
        WrapUpAndFinish();
        return NOERROR;
    }
    // If finished current stage, move on to next stage.
    Int32 size;
    mDevices->GetSize(&size);
    if (mProcessedDeviceCount == size) {
        mProcessedDeviceCount = 0;
        switch (mState) {
            case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                StartOsdNameStage();
                return NOERROR;
            case STATE_WAITING_FOR_OSD_NAME:
                StartVendorIdStage();
                return NOERROR;
            case STATE_WAITING_FOR_VENDOR_ID:
                WrapUpAndFinish();
                return NOERROR;
            default:
                return NOERROR;
        }
    } else {
        AutoPtr<IInterface> obj;
        mDevices->Get(mProcessedDeviceCount, (IInterface**)&obj);
        AutoPtr<DeviceInfo> info = (DeviceInfo*) IObject::Probe(obj);
        Int32 address = info->mLogicalAddress;
        switch (mState) {
            case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                QueryPhysicalAddress(address);
                return NOERROR;
            case STATE_WAITING_FOR_OSD_NAME:
                QueryOsdName(address);
                return NOERROR;
            case STATE_WAITING_FOR_VENDOR_ID:
                QueryVendorId(address);
            default:
                return NOERROR;
        }
    }
    return NOERROR;
}

ECode DeviceDiscoveryAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState == STATE_NONE || mState != state) {
        return NOERROR;
    }
    Slogger::V(TAG, "Timeout[State=%d, Processed=%d", mState, mProcessedDeviceCount);
    RemoveDevice(mProcessedDeviceCount);
    CheckAndProceedStage();
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
