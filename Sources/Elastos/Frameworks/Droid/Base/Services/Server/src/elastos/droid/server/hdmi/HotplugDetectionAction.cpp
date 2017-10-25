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
#include "elastos/droid/server/hdmi/DeviceSelectAction.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/HotplugDetectionAction.h"
#include "elastos/droid/server/hdmi/OneTouchRecordAction.h"
#include "elastos/droid/server/hdmi/RequestArcTerminationAction.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::IInteger32;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceDevicePollingCallback;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CBitSet;
using Elastos::Utility::IIterator;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HotplugDetectionAction::AllDevicesDevicePollingCallback
//=============================================================================
CAR_INTERFACE_IMPL(HotplugDetectionAction::AllDevicesDevicePollingCallback, Object, IHdmiControlServiceDevicePollingCallback)

HotplugDetectionAction::AllDevicesDevicePollingCallback::AllDevicesDevicePollingCallback(
    /* [in] */ HotplugDetectionAction* host)
    : mHost(host)
{}

ECode HotplugDetectionAction::AllDevicesDevicePollingCallback::OnPollingFinished(
    /* [in] */ IList* ackedAddress)
{
    return mHost->CheckHotplug(ackedAddress, FALSE);
}

//=============================================================================
// HotplugDetectionAction::AudioSystemDevicePollingCallback
//=============================================================================
CAR_INTERFACE_IMPL(HotplugDetectionAction::AudioSystemDevicePollingCallback, Object, IHdmiControlServiceDevicePollingCallback)

HotplugDetectionAction::AudioSystemDevicePollingCallback::AudioSystemDevicePollingCallback(
    /* [in] */ HotplugDetectionAction* host)
    : mHost(host)
{}

ECode HotplugDetectionAction::AudioSystemDevicePollingCallback::OnPollingFinished(
    /* [in] */ IList* ackedAddress)
{
    return mHost->CheckHotplug(ackedAddress, TRUE);
}

//=============================================================================
// HotplugDetectionAction
//=============================================================================
CAR_INTERFACE_IMPL(HotplugDetectionAction, HdmiCecFeatureAction, IHotplugDetectionAction)

const String HotplugDetectionAction::TAG("HotPlugDetectionAction");
const Int32 HotplugDetectionAction::POLLING_INTERVAL_MS = 5000;
const Int32 HotplugDetectionAction::TIMEOUT_COUNT = 3;
const Int32 HotplugDetectionAction::STATE_WAIT_FOR_NEXT_POLLING = 1;
const Int32 HotplugDetectionAction::NUM_OF_ADDRESS = Constants::ADDR_SPECIFIC_USE - Constants::ADDR_TV + 1;

HotplugDetectionAction::HotplugDetectionAction()
    : mTimeoutCount(0)
{}

ECode HotplugDetectionAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source)
{
    return HdmiCecFeatureAction::constructor(source);
}

ECode HotplugDetectionAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Slogger::V(TAG, "Hot-plug dection started.");

    mState = STATE_WAIT_FOR_NEXT_POLLING;
    mTimeoutCount = 0;

    // Start timer without polling.
    // The first check for all devices will be initiated 15 seconds later.
    AddTimer(mState, POLLING_INTERVAL_MS);
    *result = TRUE;
    return NOERROR;
}

ECode HotplugDetectionAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // No-op
    *result = FALSE;
    return NOERROR;
}

ECode HotplugDetectionAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        return NOERROR;
    }

    if (mState == STATE_WAIT_FOR_NEXT_POLLING) {
        mTimeoutCount = (mTimeoutCount + 1) % TIMEOUT_COUNT;
        PollDevices();
    }
    return NOERROR;
}

ECode HotplugDetectionAction::PollAllDevicesNow()
{
    // Clear existing timer to avoid overlapped execution
    mActionTimer->ClearTimerMessage();

    mTimeoutCount = 0;
    mState = STATE_WAIT_FOR_NEXT_POLLING;
    PollAllDevices();

    AddTimer(mState, POLLING_INTERVAL_MS);
    return NOERROR;
}

ECode HotplugDetectionAction::PollDevices()
{
    // All device check called every 15 seconds.
    if (mTimeoutCount == 0) {
        PollAllDevices();
    } else {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean isSystemAudioActivated;
        ((HdmiCecLocalDeviceTv*) tv.Get())->IsSystemAudioActivated(&isSystemAudioActivated);
        if (isSystemAudioActivated) {
            PollAudioSystem();
        }
    }

    AddTimer(mState, POLLING_INTERVAL_MS);
    return NOERROR;
}

ECode HotplugDetectionAction::PollAllDevices()
{
    Slogger::V(TAG, "Poll all devices.");

    HdmiCecFeatureAction::PollDevices(new AllDevicesDevicePollingCallback(this), Constants::POLL_ITERATION_IN_ORDER
            | Constants::POLL_STRATEGY_REMOTES_DEVICES, HdmiConfig::HOTPLUG_DETECTION_RETRY);
    return NOERROR;
}

ECode HotplugDetectionAction::PollAudioSystem()
{
    Slogger::V(TAG, "Poll audio system.");

    HdmiCecFeatureAction::PollDevices(new AudioSystemDevicePollingCallback(this), Constants::POLL_ITERATION_IN_ORDER
            | Constants::POLL_STRATEGY_SYSTEM_AUDIO, HdmiConfig::HOTPLUG_DETECTION_RETRY);
    return NOERROR;
}

ECode HotplugDetectionAction::CheckHotplug(
    /* [in] */ IList* ackedAddress,
    /* [in] */ Boolean audioOnly)
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    AutoPtr<IList> infoList;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetDeviceInfoList(FALSE, (IList**)&infoList);
    AutoPtr<IBitSet> currentInfos = InfoListToBitSet(infoList, audioOnly);
    AutoPtr<IBitSet> polledResult = AddressListToBitSet(ackedAddress);

    // At first, check removed devices.
    AutoPtr<IBitSet> removed = Complement(currentInfos, polledResult);
    Int32 index = -1;
    Int32 nextSetBit;
    while (removed->NextSetBit(index + 1, &nextSetBit), (index = nextSetBit) != -1) {
        Slogger::V(TAG, "Remove device by hot-plug detection:%d", index);
        RemoveDevice(index);
    }

    // Next, check added devices.
    AutoPtr<IBitSet> added = Complement(polledResult, currentInfos);
    index = -1;
    while (added->NextSetBit(index + 1, &nextSetBit), (index = nextSetBit) != -1) {
        Slogger::V(TAG, "Add device by hot-plug detection:%d", index);
        AddDevice(index);
    }
    return NOERROR;
}

AutoPtr<IBitSet> HotplugDetectionAction::InfoListToBitSet(
    /* [in] */ IList* infoList,
    /* [in] */ Boolean audioOnly)
{
    AutoPtr<IBitSet> set;
    CBitSet::New(NUM_OF_ADDRESS, (IBitSet**)&set);
    AutoPtr<IIterator> it;
    infoList->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        if (audioOnly) {
            Int32 deviceType;
            info->GetDeviceType(&deviceType);
            if (deviceType == IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM) {
                Int32 logicalAddr;
                info->GetLogicalAddress(&logicalAddr);
                set->Set(logicalAddr);
            }
        } else {
            Int32 logicalAddr;
            info->GetLogicalAddress(&logicalAddr);
            set->Set(logicalAddr);
        }
    }
    return set;
}

AutoPtr<IBitSet> HotplugDetectionAction::AddressListToBitSet(
    /* [in] */ IList* list)
{
    AutoPtr<IBitSet> set;
    CBitSet::New(NUM_OF_ADDRESS, (IBitSet**)&set);
    AutoPtr<IIterator> it;
    list->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int32 value;
        IInteger32::Probe(obj)->GetValue(&value);
        set->Set(value);
    }
    return set;
}

AutoPtr<IBitSet> HotplugDetectionAction::Complement(
    /* [in] */ IBitSet* first,
    /* [in] */ IBitSet* second)
{
    // Need to clone it so that it doesn't touch original set.
    AutoPtr<IInterface> obj;
    ICloneable::Probe(first)->Clone((IInterface**)&obj);
    AutoPtr<IBitSet> clone = IBitSet::Probe(obj);
    clone->AndNot(second);
    return clone;
}

ECode HotplugDetectionAction::AddDevice(
    /* [in] */ Int32 addedAddress)
{
    // Sending <Give Physical Address> will initiate new device action.
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGivePhysicalAddress(srcAddr,
            addedAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    return NOERROR;
}

ECode HotplugDetectionAction::RemoveDevice(
    /* [in] */ Int32 removedAddress)
{
    MayChangeRoutingPath(removedAddress);
    MayCancelDeviceSelect(removedAddress);
    MayCancelOneTouchRecord(removedAddress);
    MayDisableSystemAudioAndARC(removedAddress);

    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->RemoveCecDevice(removedAddress);
    return NOERROR;
}

ECode HotplugDetectionAction::MayChangeRoutingPath(
    /* [in] */ Int32 address)
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    AutoPtr<IHdmiDeviceInfo> info;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetCecDeviceInfo(address, (IHdmiDeviceInfo**)&info);
    if (info != NULL) {
        Int32 physicalAddr;
        info->GetPhysicalAddress(&physicalAddr);
        ((HdmiCecLocalDeviceTv*) tv.Get())->HandleRemoveActiveRoutingPath(physicalAddr);
    }
    return NOERROR;
}

ECode HotplugDetectionAction::MayCancelDeviceSelect(
    /* [in] */ Int32 address)
{
    AutoPtr<IList> actions;
    GetActions(ECLSID_CDeviceSelectAction, (IList**)&actions);
    Boolean isEmpty;
    actions->IsEmpty(&isEmpty);
    if (isEmpty) {
        return NOERROR;
    }

    // Should have only one Device Select Action
    AutoPtr<IInterface> obj;
    actions->Get(0, (IInterface**)&obj);
    AutoPtr<DeviceSelectAction> action = (DeviceSelectAction*) IObject::Probe(obj);
    Int32 targetAddr;
    action->GetTargetAddress(&targetAddr);
    if (targetAddr == address) {
        RemoveAction(ECLSID_CDeviceSelectAction);
    }
    return NOERROR;
}

ECode HotplugDetectionAction::MayCancelOneTouchRecord(
    /* [in] */ Int32 address)
{
    AutoPtr<IList> actions;
    GetActions(ECLSID_COneTouchRecordAction, (IList**)&actions);
    AutoPtr<IIterator> it;
    actions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<OneTouchRecordAction> action = (OneTouchRecordAction*) IOneTouchRecordAction::Probe(obj);
        Int32 recorderAddress;
        action->GetRecorderAddress(&recorderAddress);
        if (recorderAddress == address) {
            RemoveAction(action);
        }
    }
    return NOERROR;
}

ECode HotplugDetectionAction::MayDisableSystemAudioAndARC(
    /* [in] */ Int32 address)
{
    if (HdmiUtils::GetTypeFromAddress(address) != IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM) {
        return NOERROR;
    }

    // Turn off system audio mode and update settings.
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->SetSystemAudioMode(FALSE, TRUE);
    Boolean isArcEstabilished;
    ((HdmiCecLocalDeviceTv*) tv.Get())->IsArcEstabilished(&isArcEstabilished);
    if (isArcEstabilished) {
        AutoPtr<RequestArcTerminationAction> newRequestArcTerminationAction = new RequestArcTerminationAction();
        AutoPtr<IHdmiCecLocalDevice> localDevice;
        LocalDevice((IHdmiCecLocalDevice**)&localDevice);
        newRequestArcTerminationAction->constructor(localDevice, address);
        AddAndStartAction(newRequestArcTerminationAction);
    }
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
