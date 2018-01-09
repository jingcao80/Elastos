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

#include "elastos/droid/server/hdmi/ActiveSourceHandler.h"
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/DeviceDiscoveryAction.h"
#include "elastos/droid/server/hdmi/DeviceSelectAction.h"
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageCache.h"
#include "elastos/droid/server/hdmi/HdmiCecStandbyModeHandler.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/HotplugDetectionAction.h"
#include "elastos/droid/server/hdmi/NewDeviceAction.h"
#include "elastos/droid/server/hdmi/OneTouchRecordAction.h"
#include "elastos/droid/server/hdmi/PowerStatusMonitorAction.h"
#include "elastos/droid/server/hdmi/RequestArcInitiationAction.h"
#include "elastos/droid/server/hdmi/RequestArcTerminationAction.h"
#include "elastos/droid/server/hdmi/RoutingControlAction.h"
#include "elastos/droid/server/hdmi/SendKeyAction.h"
#include "elastos/droid/server/hdmi/SetArcTransmissionStateAction.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromAvr.h"
#include "elastos/droid/server/hdmi/SystemAudioActionFromTv.h"
#include "elastos/droid/server/hdmi/SystemAudioAutoInitiationAction.h"
#include "elastos/droid/server/hdmi/TimerRecordingAction.h"
#include "elastos/droid/server/hdmi/VolumeControlAction.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/os/Process.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>

// using Elastos::Droid::Hardware::Hdmi::CHdmiDeviceInfo;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfoHelper;
using Elastos::Droid::Hardware::Hdmi::IHdmiRecordSourcesHelper;
using Elastos::Droid::Hardware::Hdmi::IHdmiTimerRecordSources;
using Elastos::Droid::Hardware::Hdmi::IHdmiTimerRecordSourcesHelper;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Server::Hdmi::IDeviceDiscoveryActionDeviceDiscoveryCallback;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiCecLocalDeviceTv::InnerSub_DeviceDiscoveryCallback
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecLocalDeviceTv::InnerSub_DeviceDiscoveryCallback, Object, IDeviceDiscoveryActionDeviceDiscoveryCallback)

HdmiCecLocalDeviceTv::InnerSub_DeviceDiscoveryCallback::InnerSub_DeviceDiscoveryCallback(
    /* [in] */ HdmiCecLocalDeviceTv* host)
    : mHost(host)
{}

ECode HdmiCecLocalDeviceTv::InnerSub_DeviceDiscoveryCallback::OnDeviceDiscoveryDone(
    /* [in] */ IList* deviceInfos)
{
    AutoPtr<IIterator> it;
    deviceInfos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        mHost->AddCecDevice(info);
    }
    // Since we removed all devices when it's start and
    // device discovery action does not poll local devices,
    // we should put device info of local device manually here
    AutoPtr<IList> devices;
    ((HdmiControlService*)mHost->mService.Get())->GetAllLocalDevices((IList**)&devices);
    it = NULL;
    devices->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiCecLocalDevice> device = IHdmiCecLocalDevice::Probe(obj);
        AutoPtr<IHdmiDeviceInfo> deviceInfo;
        ((HdmiCecLocalDevice*)device.Get())->GetDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
        mHost->AddCecDevice(deviceInfo);
    }
    AutoPtr<HotplugDetectionAction> newHotplugDetectionAction = new HotplugDetectionAction();
    newHotplugDetectionAction->constructor(mHost);
    mHost->AddAndStartAction(newHotplugDetectionAction);
    AutoPtr<PowerStatusMonitorAction> newPowerStatusMonitorAction = new PowerStatusMonitorAction();
    newPowerStatusMonitorAction->constructor(mHost);
    mHost->AddAndStartAction(newPowerStatusMonitorAction);
    // If there is AVR, initiate System Audio Auto initiation action,
    // which turns on and off system audio according to last system
    // audio setting.
    AutoPtr<IHdmiDeviceInfo> avr;
    mHost->GetAvrDeviceInfo((IHdmiDeviceInfo**)&avr);
    if (avr != NULL) {
        mHost->OnNewAvrAdded(avr);
    }
    return NOERROR;
}

//=============================================================================
// HdmiCecLocalDeviceTv::InnerSub_SendMessageCallback
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecLocalDeviceTv::InnerSub_SendMessageCallback, Object, IHdmiControlServiceSendMessageCallback)

HdmiCecLocalDeviceTv::InnerSub_SendMessageCallback::InnerSub_SendMessageCallback(
    /* [in] */ HdmiCecLocalDeviceTv* host)
    : mHost(host)
{}

ECode HdmiCecLocalDeviceTv::InnerSub_SendMessageCallback::OnSendCompleted(
    /* [in] */ Int32 error)
{
    if (error != Constants::SEND_RESULT_SUCCESS) {
        mHost->AnnounceClearTimerRecordingResult(
                IHdmiControlManager::CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
    }
    return NOERROR;
}

//=============================================================================
// HdmiCecLocalDeviceTv
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecLocalDeviceTv, HdmiCecLocalDevice, IHdmiCecLocalDeviceTv)

const String HdmiCecLocalDeviceTv::TAG("HdmiCecLocalDeviceTv");

HdmiCecLocalDeviceTv::HdmiCecLocalDeviceTv()
    : mArcEstablished(FALSE)
    , mArcFeatureEnabled(TRUE)
    , mSystemAudioActivated(FALSE)
    , mPrevPortId(0)
    , mSystemAudioVolume(Constants::UNKNOWN_VOLUME)
    , mSystemAudioMute(FALSE)
    , mAutoDeviceOff(FALSE)
    , mAutoWakeup(FALSE)
    , mSkipRoutingControl(FALSE)
{
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->GetEmptyList((IList**)&mSafeAllDeviceInfos);
    helper->GetEmptyList((IList**)&mSafeExternalInputs);
    CSparseArray::New((ISparseArray**)&mDeviceInfos);
    CArraySet::New((IArraySet**)&mCecSwitches);
}

ECode HdmiCecLocalDeviceTv::constructor(
    /* [in] */ IHdmiControlService* service)
{
    HdmiCecLocalDevice::constructor(service, IHdmiDeviceInfo::DEVICE_TV);
    mPrevPortId = Constants::INVALID_PORT_ID;
    ((HdmiControlService*)mService.Get())->ReadBooleanSetting(ISettingsGlobal::HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED,
            TRUE, &mAutoDeviceOff);
    ((HdmiControlService*)mService.Get())->ReadBooleanSetting(ISettingsGlobal::HDMI_CONTROL_AUTO_WAKEUP_ENABLED, TRUE, &mAutoWakeup);
    mStandbyHandler = new HdmiCecStandbyModeHandler();
    mStandbyHandler->constructor(service, this);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::OnAddressAllocated(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 reason)
{
    AssertRunOnServiceThread();
    Int32 physicalAddr;
    ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildReportPhysicalAddressCommand(mAddress, physicalAddr, mDeviceType, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    cmd = NULL;
    Int32 vendorId;
    ((HdmiControlService*)mService.Get())->GetVendorId(&vendorId);
    HdmiCecMessageBuilder::BuildDeviceVendorIdCommand(mAddress, vendorId, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    AutoPtr<IInteger32> iPhysicalAddr;
    CInteger32::New(physicalAddr, (IInteger32**)&iPhysicalAddr);
    ISet::Probe(mCecSwitches)->Add(iPhysicalAddr);  // TV is a CEC switch too.
    mSkipRoutingControl = (reason == HdmiControlService::INITIATED_BY_WAKE_UP_MESSAGE);
    LaunchRoutingControl(reason != HdmiControlService::INITIATED_BY_ENABLE_CEC &&
            reason != HdmiControlService::INITIATED_BY_BOOT_UP);
    LaunchDeviceDiscovery();
    StartQueuedActions();
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetPreferredAddress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AssertRunOnServiceThread();
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    return helper->GetInt32(Constants::PROPERTY_PREFERRED_ADDRESS_TV,
            Constants::ADDR_UNREGISTERED, result);
}

ECode HdmiCecLocalDeviceTv::SetPreferredAddress(
    /* [in] */ Int32 addr)
{
    AssertRunOnServiceThread();
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    helper->Set(Constants::PROPERTY_PREFERRED_ADDRESS_TV, StringUtils::ToString(addr));
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::DispatchMessage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Boolean isPowerStandby;
    ((HdmiControlService*)mService.Get())->IsPowerStandby(&isPowerStandby);
    Boolean isHandleCommandOk;
    mStandbyHandler->HandleCommand(message, &isHandleCommandOk);
    if (isPowerStandby && isHandleCommandOk) {
        *result = TRUE;
        return NOERROR;
    }
    return HdmiCecLocalDevice::OnMessage(message, result);
}

ECode HdmiCecLocalDeviceTv::DeviceSelect(
    /* [in] */ Int32 id,
    /* [in] */ IIHdmiControlCallback* callback)
{
    AssertRunOnServiceThread();
    AutoPtr<IInterface> obj;
    mDeviceInfos->Get(id, (IInterface**)&obj);
    AutoPtr<IHdmiDeviceInfo> targetDevice = IHdmiDeviceInfo::Probe(obj);
    if (targetDevice == NULL) {
        InvokeCallback(callback, IHdmiControlManager::RESULT_TARGET_NOT_AVAILABLE);
        return NOERROR;
    }
    Int32 targetAddress;
    targetDevice->GetLogicalAddress(&targetAddress);
    AutoPtr<IHdmiCecLocalDeviceActiveSource> active;
    GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&active);
    Boolean isValid;
    active->IsValid(&isValid);
    if (isValid && targetAddress == ((HdmiCecLocalDevice::ActiveSource*)active.Get())->mLogicalAddress) {
        InvokeCallback(callback, IHdmiControlManager::RESULT_SUCCESS);
        return NOERROR;
    }
    if (targetAddress == Constants::ADDR_INTERNAL) {
        HandleSelectInternalSource();
        // Switching to internal source is always successful even when CEC control is disabled.
        Int32 physicalAddr;
        ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
        SetActiveSource(targetAddress, physicalAddr);
        SetActivePath(physicalAddr);
        InvokeCallback(callback, IHdmiControlManager::RESULT_SUCCESS);
        return NOERROR;
    }
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        SetActiveSource(targetDevice);
        InvokeCallback(callback, IHdmiControlManager::RESULT_INCORRECT_MODE);
        return NOERROR;
    }
    RemoveAction(ECLSID_CDeviceSelectAction);
    AutoPtr<DeviceSelectAction> action = new DeviceSelectAction();
    action->constructor(this, targetDevice, callback);
    AddAndStartAction(action);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleSelectInternalSource()
{
    AssertRunOnServiceThread();
    // Seq #18
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (isControlEnabled && ((HdmiCecLocalDevice::ActiveSource*) mActiveSource.Get())->mLogicalAddress != mAddress) {
        Int32 physicalAddr;
        ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
        UpdateActiveSource(mAddress, physicalAddr);
        if (mSkipRoutingControl) {
            mSkipRoutingControl = FALSE;
            return NOERROR;
        }
        AutoPtr<IHdmiCecMessage> activeSource;
        HdmiCecMessageBuilder::BuildActiveSource(mAddress, physicalAddr, (IHdmiCecMessage**)&activeSource);
        ((HdmiControlService*)mService.Get())->SendCecCommand(activeSource);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::UpdateActiveSource(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress)
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
    HdmiCecLocalDevice::ActiveSource::Of(logicalAddress, physicalAddress, (IHdmiCecLocalDeviceActiveSource**)&activeSource);
    UpdateActiveSource(activeSource);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::UpdateActiveSource(
    /* [in] */ IHdmiCecLocalDeviceActiveSource* newActive)
{
    AssertRunOnServiceThread();
    // Seq #14
    Boolean isEquals;
    ((HdmiCecLocalDevice::ActiveSource*) mActiveSource.Get())->Equals(newActive, &isEquals);
    if (isEquals) {
        return NOERROR;
    }
    SetActiveSource(newActive);
    Int32 logicalAddress = ((HdmiCecLocalDevice::ActiveSource*)newActive)->mLogicalAddress;
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    GetCecDeviceInfo(logicalAddress, (IHdmiDeviceInfo**)&deviceInfo);
    if (deviceInfo != NULL && logicalAddress != mAddress) {
        Int32 portId;
        ((HdmiControlService*)mService.Get())->PathToPortId(((HdmiCecLocalDevice::ActiveSource*)newActive)->mPhysicalAddress, &portId);
        Int32 activePortId;
        GetActivePortId(&activePortId);
        if (portId == activePortId) {
            SetPrevPortId(activePortId);
        }
        // TODO: Show the OSD banner related to the new active source device.
    } else {
        // TODO: If displayed, remove the OSD banner related to the previous
        //       active source device.
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetPortId(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Int32* result)
{
    return ((HdmiControlService*)mService.Get())->PathToPortId(physicalAddress, result);
}

ECode HdmiCecLocalDeviceTv::GetPrevPortId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mLock);
        *result = mPrevPortId;
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::SetPrevPortId(
    /* [in] */ Int32 portId)
{
    {    AutoLock syncLock(mLock);
        mPrevPortId = portId;
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::UpdateActiveInput(
    /* [in] */ Int32 path,
    /* [in] */ Boolean notifyInputChange)
{
    AssertRunOnServiceThread();
    // Seq #15
    Int32 activePath;
    GetActivePath(&activePath);
    if (path == activePath) {
        return NOERROR;
    }
    Int32 activePortId;
    GetActivePortId(&activePortId);
    SetPrevPortId(activePortId);
    SetActivePath(path);
    // TODO: Handle PAP/PIP case.
    // Show OSD port change banner
    if (notifyInputChange) {
        AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
        GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&activeSource);
        AutoPtr<IHdmiDeviceInfo> info;
        GetCecDeviceInfo(((HdmiCecLocalDevice::ActiveSource*)activeSource.Get())->mLogicalAddress, (IHdmiDeviceInfo**)&info);
        if (info == NULL) {
            Int32 activePortId;
            GetActivePortId(&activePortId);
#if 0 // TODO: Waiting for CHdmiDeviceInfo
            CHdmiDeviceInfo::New(Constants::ADDR_INVALID, path, activePortId,
                    IHdmiDeviceInfo::DEVICE_RESERVED, 0, NULL, (IHdmiDeviceInfo**)&info);
#else
            assert(0);
#endif
        }
        ((HdmiControlService*)mService.Get())->InvokeInputChangeListener(info);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::DoManualPortSwitching(
    /* [in] */ Int32 portId,
    /* [in] */ IIHdmiControlCallback* callback)
{
    AssertRunOnServiceThread();
    // Seq #20
    Boolean isValidPortId;
    ((HdmiControlService*)mService.Get())->IsValidPortId(portId, &isValidPortId);
    if (!isValidPortId) {
        InvokeCallback(callback, IHdmiControlManager::RESULT_INCORRECT_MODE);
        return NOERROR;
    }
    Int32 activePortId;
    GetActivePortId(&activePortId);
    if (portId == activePortId) {
        InvokeCallback(callback, IHdmiControlManager::RESULT_SUCCESS);
        return NOERROR;
    }
    ((HdmiCecLocalDevice::ActiveSource*) mActiveSource.Get())->Invalidate();
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        SetActivePortId(portId);
        InvokeCallback(callback, IHdmiControlManager::RESULT_INCORRECT_MODE);
        return NOERROR;
    }
    Int32 oldPath;
    if (activePortId != Constants::INVALID_PORT_ID) {
        ((HdmiControlService*)mService.Get())->PortIdToPath(activePortId, &oldPath);
    }
    else {
        AutoPtr<IHdmiDeviceInfo> deviceInfo;
        GetDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
        deviceInfo->GetPhysicalAddress(&oldPath);
    }
    SetActivePath(oldPath);
    if (mSkipRoutingControl) {
        mSkipRoutingControl = FALSE;
        return NOERROR;
    }
    Int32 newPath;
    ((HdmiControlService*)mService.Get())->PortIdToPath(portId, &newPath);
    AutoPtr<IHdmiCecMessage> routingChange;
    HdmiCecMessageBuilder::BuildRoutingChange(mAddress, oldPath, newPath, (IHdmiCecMessage**)&routingChange);
    ((HdmiControlService*)mService.Get())->SendCecCommand(routingChange);
    RemoveAction(ECLSID_CRoutingControlAction);
    AutoPtr<RoutingControlAction> controlAction = new RoutingControlAction();
    controlAction->constructor(this, newPath, TRUE, callback);
    AddAndStartAction(controlAction);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetPowerStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AssertRunOnServiceThread();
    return ((HdmiControlService*)mService.Get())->GetPowerStatus(result);
}

ECode HdmiCecLocalDeviceTv::SendKeyEvent(
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    AssertRunOnServiceThread();
    Boolean isSupportedKeycode;
    HdmiCecKeycode::IsSupportedKeycode(keyCode, &isSupportedKeycode);
    if (!isSupportedKeycode) {
        Slogger::W(TAG, "Unsupported key: %d", keyCode);
        return NOERROR;
    }
    AutoPtr<IList> action;
    GetActions(ECLSID_CSendKeyAction, (IList**)&action);
    Boolean isEmpty;
    action->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<IInterface> obj;
        action->Get(0, (IInterface**)&obj);
        ((SendKeyAction*)ISendKeyAction::Probe(obj))->ProcessKeyEvent(keyCode, isPressed);
    } else {
        Boolean isValid;
        AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
        GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&activeSource);
        ((HdmiCecLocalDevice::ActiveSource*)activeSource.Get())->IsValid(&isValid);
        if (isPressed && isValid) {
            Int32 logicalAddress = ((HdmiCecLocalDevice::ActiveSource*)activeSource.Get())->mLogicalAddress;
            AutoPtr<SendKeyAction> newSendKeyAction = new SendKeyAction();
            newSendKeyAction->constructor(this, logicalAddress, keyCode);
            AddAndStartAction(newSendKeyAction);
        } else {
            Slogger::W(TAG, "Discard key event: %d pressed:%s", keyCode, isPressed ? "true" : "false");
        }
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::InvokeCallback(
    /* [in] */ IIHdmiControlCallback* callback,
    /* [in] */ Int32 result)
{
    if (callback == NULL) {
        return NOERROR;
    }
    // try {
    ECode ec = callback->OnComplete(result);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Invoking callback failed:%d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Int32 logicalAddress;
    message->GetSource(&logicalAddress);
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 physicalAddress = HdmiUtils::TwoBytesToInt32(params);
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    GetCecDeviceInfo(logicalAddress, (IHdmiDeviceInfo**)&deviceInfo);
    if (deviceInfo == NULL) {
        HandleNewDeviceAtTheTailOfActivePath(physicalAddress);
    } else {
        AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
        HdmiCecLocalDevice::ActiveSource::Of(logicalAddress, physicalAddress, (IHdmiCecLocalDeviceActiveSource**)&activeSource);
        AutoPtr<ActiveSourceHandler> activeSourceHandler;
        ActiveSourceHandler::Create(this, NULL, (ActiveSourceHandler**)&activeSourceHandler);
        activeSourceHandler->Process(activeSource);
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleInactiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    // Seq #10
    // Ignore <Inactive Source> from non-active source device.
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
    GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&activeSource);
    if (((HdmiCecLocalDevice::ActiveSource*)activeSource.Get())->mLogicalAddress != srcAddr) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean isProhibitMode;
    IsProhibitMode(&isProhibitMode);
    if (isProhibitMode) {
        *result = TRUE;
        return NOERROR;
    }
    Int32 portId;
    GetPrevPortId(&portId);
    if (portId != Constants::INVALID_PORT_ID) {
        // TODO: Do this only if TV is not showing multiview like PIP/PAP.
        Int32 srcAddr;
        message->GetSource(&srcAddr);
        AutoPtr<IHdmiDeviceInfo> inactiveSource;
        GetCecDeviceInfo(srcAddr, (IHdmiDeviceInfo**)&inactiveSource);
        if (inactiveSource == NULL) {
            *result = TRUE;
            return NOERROR;
        }
        Int32 physicalAddr;
        inactiveSource->GetPhysicalAddress(&physicalAddr);
        Int32 servicePortId;
        ((HdmiControlService*)mService.Get())->PathToPortId(physicalAddr, &servicePortId);
        if (servicePortId == portId) {
            *result = TRUE;
            return NOERROR;
        }
        // TODO: Switch the TV freeze mode off
        DoManualPortSwitching(portId, NULL);
        SetPrevPortId(Constants::INVALID_PORT_ID);
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleRequestActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    // Seq #19
    AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
    GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&activeSource);
    if (mAddress == ((HdmiCecLocalDevice::ActiveSource*)activeSource.Get())->mLogicalAddress) {
        AutoPtr<IHdmiCecMessage> activeSource;
        Int32 activePath;
        GetActivePath(&activePath);
        HdmiCecMessageBuilder::BuildActiveSource(mAddress, activePath, (IHdmiCecMessage**)&activeSource);
        ((HdmiControlService*) mService.Get())->SendCecCommand(activeSource);
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleGetMenuLanguage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    String language;
    ((HdmiControlService*)mService.Get())->GetLanguage(&language);
    Boolean isBroadcastMenuLanguage;
    BroadcastMenuLanguage(language, &isBroadcastMenuLanguage);
    if (!isBroadcastMenuLanguage) {
        Slogger::W(TAG, "Failed to respond to <Get Menu Language>: %s", TO_CSTR(message));
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::BroadcastMenuLanguage(
    /* [in] */ const String& language,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<IHdmiCecMessage> command;
    HdmiCecMessageBuilder::BuildSetMenuLanguageCommand(mAddress, language, (IHdmiCecMessage**)&command);
    if (command != NULL) {
        ((HdmiControlService*)mService.Get())->SendCecCommand(command);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleReportPhysicalAddress(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 path = HdmiUtils::TwoBytesToInt32(params);
    Int32 address;
    message->GetSource(&address);
    Int32 type = (*params)[2];
    Boolean isUpdateCecSwitchInfoOk;
    UpdateCecSwitchInfo(address, type, path, &isUpdateCecSwitchInfoOk);
    if (isUpdateCecSwitchInfoOk) {
        *result = TRUE;
        return NOERROR;
    }
    // Ignore if [Device Discovery Action] is going on.
    Boolean hasAction;
    HasAction(ECLSID_CDeviceDiscoveryAction, &hasAction);
    if (hasAction) {
        Slogger::I(TAG, "Ignored while Device Discovery Action is in progress: %s", TO_CSTR(message));
        *result = TRUE;
        return NOERROR;
    }
    Boolean isInDeviceList;
    IsInDeviceList(address, path, &isInDeviceList);
    if (!isInDeviceList) {
        HandleNewDeviceAtTheTailOfActivePath(path);
    }
    AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
    ActiveSource::Of(address, path, (IHdmiCecLocalDeviceActiveSource**)&activeSource);
    StartNewDeviceAction(activeSource);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleReportPowerStatus(
    /* [in] */ IHdmiCecMessage* command,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params;
    command->GetParams((ArrayOf<Byte>**)&params);
    Int32 newStatus = (*params)[0] & 0xFF;
    Int32 srcAddr;
    command->GetSource(&srcAddr);
    UpdateDevicePowerStatus(srcAddr, newStatus);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleTimerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Do nothing.
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleRecordStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Do nothing.
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::UpdateCecSwitchInfo(
    /* [in] */ Int32 address,
    /* [in] */ Int32 type,
    /* [in] */ Int32 path,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInteger32> iPath;
    CInteger32::New(path, (IInteger32**)&iPath);
    if (address == Constants::ADDR_UNREGISTERED
            && type == IHdmiDeviceInfo::DEVICE_PURE_CEC_SWITCH) {
        ISet::Probe(mCecSwitches)->Add(iPath);
        UpdateSafeDeviceInfoList();
        *result = TRUE;
        return NOERROR;  // Pure switch does not need further processing. Return here.
    }
    if (type == IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM) {
        ISet::Probe(mCecSwitches)->Add(iPath);
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::StartNewDeviceAction(
    /* [in] */ IHdmiCecLocalDeviceActiveSource* activeSource)
{
    AutoPtr<IList> actions;
    GetActions(ECLSID_CNewDeviceAction, (IList**)&actions);
    AutoPtr<IIterator> it;
    actions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<NewDeviceAction> action = (NewDeviceAction*) IObject::Probe(obj);
        // If there is new device action which has the same logical address and path
        // ignore new request.
        // NewDeviceAction is created whenever it receives <Report Physical Address>.
        // And there is a chance starting NewDeviceAction for the same source.
        // Usually, new device sends <Report Physical Address> when it's plugged
        // in. However, TV can detect a new device from HotPlugDetectionAction,
        // which sends <Give Physical Address> to the source for newly detected
        // device.
        Boolean isActionOf;
        action->IsActionOf(activeSource, &isActionOf);
        if (isActionOf) {
            return NOERROR;
        }
    }
    AutoPtr<NewDeviceAction> deviceAction = new NewDeviceAction();
    deviceAction->constructor(this, ((HdmiCecLocalDevice::ActiveSource*)activeSource)->mLogicalAddress,
            ((HdmiCecLocalDevice::ActiveSource*)activeSource)->mPhysicalAddress);
    AddAndStartAction(deviceAction);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleNewDeviceAtTheTailOfActivePath(
    /* [in] */ Int32 path)
{
    // Seq #22
    Int32 activePath;
    GetActivePath(&activePath);
    Boolean isTailOfActivePath;
    IsTailOfActivePath(path, activePath, &isTailOfActivePath);
    if (isTailOfActivePath) {
        RemoveAction(ECLSID_CRoutingControlAction);
        Int32 activePortId;
        GetActivePortId(&activePortId);
        Int32 newPath;
        ((HdmiControlService*)mService.Get())->PortIdToPath(activePortId, &newPath);
        SetActivePath(newPath);
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildRoutingChange(mAddress, activePath, newPath, (IHdmiCecMessage**)&cmd);
        ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
        AutoPtr<RoutingControlAction> routingControlAction = new RoutingControlAction();
        routingControlAction->constructor(this, newPath, FALSE, NULL);
        AddAndStartAction(routingControlAction);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsTailOfActivePath(
    /* [in] */ Int32 path,
    /* [in] */ Int32 activePath,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // If active routing path is internal source, return false.
    if (activePath == 0) {
        *result = FALSE;
        return NOERROR;
    }
    for (Int32 i = 12; i >= 0; i -= 4) {
        Int32 curActivePath = (activePath >> i) & 0xF;
        if (curActivePath == 0) {
            *result = TRUE;
            return NOERROR;
        } else {
            Int32 curPath = (path >> i) & 0xF;
            if (curPath != curActivePath) {
                *result = FALSE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleRoutingChange(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    // Seq #21
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 currentPath = HdmiUtils::TwoBytesToInt32(params);
    Int32 activePath;
    GetActivePath(&activePath);
    if (HdmiUtils::IsAffectingActiveRoutingPath(activePath, currentPath)) {
        ((HdmiCecLocalDevice::ActiveSource*) mActiveSource.Get())->Invalidate();
        RemoveAction(ECLSID_CRoutingControlAction);
        Int32 newPath = HdmiUtils::TwoBytesToInt32(params, 2);
        AutoPtr<RoutingControlAction> action = new RoutingControlAction();
        action->constructor(this, newPath, TRUE, NULL);
        AddAndStartAction(action);
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 mute = (*params)[0] & 0x80;
    Int32 volume = (*params)[0] & 0x7F;
    SetAudioStatus(mute == 0x80, volume);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleTextViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Boolean isPowerStandbyOrTransient;
    ((HdmiControlService*)mService.Get())->IsPowerStandbyOrTransient(&isPowerStandbyOrTransient);
    if (isPowerStandbyOrTransient && mAutoWakeup) {
        ((HdmiControlService*)mService.Get())->WakeUp();
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleImageViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    // Currently, it's the same as <Text View On>.
    return HandleTextViewOn(message, result);
}

ECode HdmiCecLocalDeviceTv::HandleSetOsdName(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 source;
    message->GetSource(&source);
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    GetCecDeviceInfo(source, (IHdmiDeviceInfo**)&deviceInfo);
    // If the device is not in device list, ignore it.
    if (deviceInfo == NULL) {
        Slogger::E(TAG, "No source device info for <Set Osd Name>.%s", TO_CSTR(message));
        *result = TRUE;
        return NOERROR;
    }
    String osdName(NULL);
    // try {
    ECode ec;
    do {
        AutoPtr<ArrayOf<Byte> > params;
        ec = message->GetParams((ArrayOf<Byte>**)&params);
        if (FAILED(ec)) break;
        osdName = String(*(params.Get()));
    } while(FALSE);
    // } catch (UnsupportedEncodingException e) {
    if (FAILED(ec)) {
        if ((ECode)E_UNSUPPORTED_ENCODING_EXCEPTION == ec) {
            Slogger::E(TAG, "Invalid <Set Osd Name> request:%s %d", TO_CSTR(message), ec);
            *result = TRUE;
            return NOERROR;
        }
        return ec;
    }
    // }
    String displayName;
    deviceInfo->GetDisplayName(&displayName);
    if (displayName.Equals(osdName)) {
        Slogger::I(TAG, "Ignore incoming <Set Osd Name> having same osd name:%s", TO_CSTR(message));
        *result = TRUE;
        return NOERROR;
    }
    Int32 logicalAddr;
    deviceInfo->GetLogicalAddress(&logicalAddr);
    Int32 physicalAddr;
    deviceInfo->GetPhysicalAddress(&physicalAddr);
    Int32 portId;
    deviceInfo->GetPortId(&portId);
    Int32 deviceType;
    deviceInfo->GetDeviceType(&deviceType);
    Int32 vendorId;
    deviceInfo->GetVendorId(&vendorId);
    deviceInfo = NULL;
#if 0 // TODO: Waiting for CHdmiDeviceInfo
    CHdmiDeviceInfo::New(logicalAddr, physicalAddr, portId, deviceType,
            vendorId, osdName, (IHdmiDeviceInfo**)&deviceInfo);
#else
    assert(0);
#endif
    AddCecDevice(deviceInfo);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::LaunchDeviceDiscovery()
{
    AssertRunOnServiceThread();
    ClearDeviceInfoList();
    AutoPtr<DeviceDiscoveryAction> action = new DeviceDiscoveryAction();
    action->constructor(this, new InnerSub_DeviceDiscoveryCallback(this));
    AddAndStartAction(action);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::OnNewAvrAdded(
    /* [in] */ IHdmiDeviceInfo* avr)
{
    AssertRunOnServiceThread();
    Boolean systemAudioModeSetting;
    GetSystemAudioModeSetting(&systemAudioModeSetting);
    if (systemAudioModeSetting) {
        Int32 logicalAddr;
        avr->GetLogicalAddress(&logicalAddr);
        AutoPtr<SystemAudioAutoInitiationAction> systemAudioAutoInitiationAction = new SystemAudioAutoInitiationAction();
        systemAudioAutoInitiationAction->constructor(this, logicalAddr);
        AddAndStartAction(systemAudioAutoInitiationAction);
    }
    Boolean isArcFeatureEnabled;
    IsArcFeatureEnabled(&isArcFeatureEnabled);
    if (isArcFeatureEnabled) {
        StartArcAction(TRUE);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::ClearDeviceInfoList()
{
    AssertRunOnServiceThread();
    AutoPtr<IIterator> it;
    mSafeExternalInputs->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        InvokeDeviceEventListener(info, IHdmiControlManager::DEVICE_EVENT_REMOVE_DEVICE);
    }
    mDeviceInfos->Clear();
    UpdateSafeDeviceInfoList();
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::ChangeSystemAudioMode(
    /* [in] */ Boolean enabled,
    /* [in] */ IIHdmiControlCallback* callback)
{
    AssertRunOnServiceThread();
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    Boolean hasAction;
    HasAction(ECLSID_CDeviceDiscoveryAction, &hasAction);
    if (!isControlEnabled || hasAction) {
        SetSystemAudioMode(FALSE, TRUE);
        InvokeCallback(callback, IHdmiControlManager::RESULT_INCORRECT_MODE);
        return NOERROR;
    }
    AutoPtr<IHdmiDeviceInfo> avr;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&avr);
    if (avr == NULL) {
        SetSystemAudioMode(FALSE, TRUE);
        InvokeCallback(callback, IHdmiControlManager::RESULT_TARGET_NOT_AVAILABLE);
        return NOERROR;
    }
    Int32 logicalAddr;
    avr->GetLogicalAddress(&logicalAddr);
    AutoPtr<SystemAudioActionFromTv> newSystemAudioActionFromTv = new SystemAudioActionFromTv();
    newSystemAudioActionFromTv->constructor(this, logicalAddr, enabled, callback);
    AddAndStartAction(newSystemAudioActionFromTv);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::SetSystemAudioMode(
    /* [in] */ Boolean on,
    /* [in] */ Boolean updateSetting)
{
    HdmiLogger::Debug("System Audio Mode change[old:%s new:%s]", mSystemAudioActivated ? "true" : "false", on ? "true" : "false");
    if (updateSetting) {
        ((HdmiControlService*)mService.Get())->WriteBooleanSetting(ISettingsGlobal::HDMI_SYSTEM_AUDIO_ENABLED, on);
    }
    UpdateAudioManagerForSystemAudio(on);
    {    AutoLock syncLock(mLock);
        if (mSystemAudioActivated != on) {
            mSystemAudioActivated = on;
            ((HdmiControlService*)mService.Get())->AnnounceSystemAudioModeChange(on);
        }
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::UpdateAudioManagerForSystemAudio(
    /* [in] */ Boolean on)
{
    AutoPtr<IAudioManager> audioManager;
    ((HdmiControlService*)mService.Get())->GetAudioManager((IAudioManager**)&audioManager);
    Int32 device;
    audioManager->SetHdmiSystemAudioSupported(on, &device);
    HdmiLogger::Debug("[A]UpdateSystemAudio mode[on=%s] output=[%X]", on ? "true" : "false", device);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsSystemAudioActivated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
    if (deviceInfo == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    {    AutoLock syncLock(mLock);
        *result = mSystemAudioActivated;
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetSystemAudioModeSetting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return ((HdmiControlService*)mService.Get())->ReadBooleanSetting(ISettingsGlobal::HDMI_SYSTEM_AUDIO_ENABLED, FALSE, result);
}

ECode HdmiCecLocalDeviceTv::SetArcStatus(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    HdmiLogger::Debug("Set Arc Status[old:%s new:%s]", mArcEstablished ? "true" : "false", enabled ? "true" : "false");
    Boolean oldStatus = mArcEstablished;
    // 1. Enable/disable ARC circuit.
    EnableAudioReturnChannel(enabled);
    // 2. Notify arc status to audio service.
    NotifyArcStatusToAudioService(enabled);
    // 3. Update arc status;
    mArcEstablished = enabled;
    *result = oldStatus;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::EnableAudioReturnChannel(
    /* [in] */ Boolean enabled)
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiDeviceInfo> avr;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&avr);
    if (avr != NULL) {
        Int32 portId;
        avr->GetPortId(&portId);
        ((HdmiControlService*)mService.Get())->EnableAudioReturnChannel(portId, enabled);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::NotifyArcStatusToAudioService(
    /* [in] */ Boolean enabled)
{
    // Note that we don't set any name to ARC.
    AutoPtr<IAudioManager> audioManager;
    ((HdmiControlService*)mService.Get())->GetAudioManager((IAudioManager**)&audioManager);
    audioManager->SetWiredDeviceConnectionState(
            IAudioSystem::DEVICE_OUT_HDMI_ARC,
            enabled ? 1 : 0, String(""));
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsArcEstabilished(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    *result = mArcFeatureEnabled && mArcEstablished;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::ChangeArcFeatureEnabled(
    /* [in] */ Boolean enabled)
{
    AssertRunOnServiceThread();
    if (mArcFeatureEnabled != enabled) {
        mArcFeatureEnabled = enabled;
        if (enabled) {
            if (!mArcEstablished) {
                StartArcAction(TRUE);
            }
        } else {
            if (mArcEstablished) {
                StartArcAction(FALSE);
            }
        }
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsArcFeatureEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    *result = mArcFeatureEnabled;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::StartArcAction(
    /* [in] */ Boolean enabled)
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiDeviceInfo> info;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&info);
    if (info == NULL) {
        Slogger::W(TAG, "Failed to start arc action; No AVR device.");
        return NOERROR;
    }
    Int32 logicalAddr;
    info->GetLogicalAddress(&logicalAddr);
    Boolean canStartArcUpdateAction;
    CanStartArcUpdateAction(logicalAddr, enabled, &canStartArcUpdateAction);
    if (!canStartArcUpdateAction) {
        Slogger::W(TAG, "Failed to start arc action; ARC configuration check failed.");
        Int32 physicalAddr;
        info->GetPhysicalAddress(&physicalAddr);
        Boolean isConnectedToArcPort;
        IsConnectedToArcPort(physicalAddr, &isConnectedToArcPort);
        if (enabled && !isConnectedToArcPort) {
            DisplayOsd(IHdmiControlManager::OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT);
        }
        return NOERROR;
    }
    // Terminate opposite action and start action if not exist.
    if (enabled) {
        RemoveAction(ECLSID_CRequestArcTerminationAction);
        Boolean hasAction;
        HasAction(ECLSID_CRequestArcInitiationAction, &hasAction);
        if (!hasAction) {
            Int32 logicalAddr;
            info->GetLogicalAddress(&logicalAddr);
            AutoPtr<RequestArcInitiationAction> newRequestArcInitiationAction = new RequestArcInitiationAction();
            newRequestArcInitiationAction->constructor(this, logicalAddr);
            AddAndStartAction(newRequestArcInitiationAction);
        }
    } else {
        RemoveAction(ECLSID_CRequestArcInitiationAction);
        Boolean hasAction;
        HasAction(ECLSID_CRequestArcTerminationAction, &hasAction);
        if (!hasAction) {
            Int32 logicalAddr;
            info->GetLogicalAddress(&logicalAddr);
            AutoPtr<RequestArcTerminationAction> newRequestArcTerminationAction = new RequestArcTerminationAction();
            newRequestArcTerminationAction->constructor(this, logicalAddr);
            AddAndStartAction(newRequestArcTerminationAction);
        }
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsDirectConnectAddress(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (physicalAddress & Constants::ROUTING_PATH_TOP_MASK) == physicalAddress;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::SetAudioStatus(
    /* [in] */ Boolean mute,
    /* [in] */ Int32 volume)
{
    {    AutoLock syncLock(mLock);
        mSystemAudioMute = mute;
        mSystemAudioVolume = volume;
        AutoPtr<IAudioManager> audioManager;
        ((HdmiControlService*)mService.Get())->GetAudioManager((IAudioManager**)&audioManager);
        Int32 maxVolume;
        audioManager->GetStreamMaxVolume(
                IAudioManager::STREAM_MUSIC, &maxVolume);
        Int32 customVolume;
        VolumeControlAction::ScaleToCustomVolume(volume, maxVolume, &customVolume);
        ((HdmiControlService*)mService.Get())->SetAudioStatus(mute, customVolume);
        DisplayOsd(IHdmiControlManager::OSD_MESSAGE_AVR_VOLUME_CHANGED,
                mute ? IHdmiControlManager::AVR_VOLUME_MUTED : volume);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::ChangeVolume(
    /* [in] */ Int32 curVolume,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 maxVolume)
{
    AssertRunOnServiceThread();
    Boolean isSystemAudioActivated;
    IsSystemAudioActivated(&isSystemAudioActivated);
    if (delta == 0 || !isSystemAudioActivated) {
        return NOERROR;
    }
    Int32 targetVolume = curVolume + delta;
    Int32 cecVolume;
    VolumeControlAction::ScaleToCecVolume(targetVolume, maxVolume, &cecVolume);
    {    AutoLock syncLock(mLock);
        // If new volume is the same as current system audio volume, just ignore it.
        // Note that UNKNOWN_VOLUME is not in range of cec volume scale.
        if (cecVolume == mSystemAudioVolume) {
            // Update tv volume with system volume value.
            Int32 customVolume;
            VolumeControlAction::ScaleToCustomVolume(mSystemAudioVolume, maxVolume, &customVolume);
            ((HdmiControlService*)mService.Get())->SetAudioStatus(FALSE, customVolume);
            return NOERROR;
        }
    }
    AutoPtr<IList> actions;
    GetActions(ECLSID_CVolumeControlAction, (IList**)&actions);
    Boolean isEmpty;
    actions->IsEmpty(&isEmpty);
    if (isEmpty) {
        AutoPtr<VolumeControlAction> newVolumeControlAction = new VolumeControlAction();
        AutoPtr<IHdmiDeviceInfo> info;
        GetAvrDeviceInfo((IHdmiDeviceInfo**)&info);
        Int32 address;
        info->GetLogicalAddress(&address);
        newVolumeControlAction->constructor(this, address, delta > 0);
        AddAndStartAction(newVolumeControlAction);
    } else {
        AutoPtr<IInterface> obj;
        actions->Get(0, (IInterface**)&obj);
        ((VolumeControlAction*)IObject::Probe(obj))->HandleVolumeChange(delta > 0);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::ChangeMute(
    /* [in] */ Boolean mute)
{
    AssertRunOnServiceThread();
    HdmiLogger::Debug("[A]:Change mute:%s", mute ? "true" : "false");
    {    AutoLock syncLock(mLock);
        if (mSystemAudioMute == mute) {
            HdmiLogger::Debug("No need to change mute.");
            return NOERROR;
        }
    }
    Boolean isSystemAudioActivated;
    IsSystemAudioActivated(&isSystemAudioActivated);
    if (!isSystemAudioActivated) {
        HdmiLogger::Debug("[A]:System audio is not activated.");
        return NOERROR;
    }
    // Remove existing volume action.
    RemoveAction(ECLSID_CVolumeControlAction);
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
    Int32 logicalAddress;
    deviceInfo->GetLogicalAddress(&logicalAddress);
    SendUserControlPressedAndReleased(logicalAddress,
            mute ? HdmiCecKeycode::CEC_KEYCODE_MUTE_FUNCTION :
                    HdmiCecKeycode::CEC_KEYCODE_RESTORE_VOLUME_FUNCTION);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleInitiateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    Boolean canStartArcUpdateAction;
    CanStartArcUpdateAction(srcAddr, TRUE, &canStartArcUpdateAction);
    if (!canStartArcUpdateAction) {
        ((HdmiControlService*)mService.Get())->MaySendFeatureAbortCommand(message, Constants::ABORT_REFUSED);
        Boolean isConnectedToArcPort;
        IsConnectedToArcPort(srcAddr, &isConnectedToArcPort);
        if (!isConnectedToArcPort) {
            DisplayOsd(IHdmiControlManager::OSD_MESSAGE_ARC_CONNECTED_INVALID_PORT);
        }
        *result = TRUE;
        return NOERROR;
    }
    // In case where <Initiate Arc> is started by <Request ARC Initiation>
    // need to clean up RequestArcInitiationAction.
    RemoveAction(ECLSID_CRequestArcInitiationAction);
    AutoPtr<SetArcTransmissionStateAction> action = new SetArcTransmissionStateAction();
    action->constructor(this, srcAddr, TRUE);
    AddAndStartAction(action);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::CanStartArcUpdateAction(
    /* [in] */ Int32 avrAddress,
    /* [in] */ Boolean shouldCheckArcFeatureEnabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiDeviceInfo> avr;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&avr);
    Int32 physicalAddr;
    avr->GetPhysicalAddress(&physicalAddr);
    Int32 logicalAddr;
    avr->GetLogicalAddress(&logicalAddr);
    Boolean isConnectedToArcPort;
    IsConnectedToArcPort(physicalAddr, &isConnectedToArcPort);
    Boolean isDirectConnectAddress;
    IsDirectConnectAddress(physicalAddr, &isDirectConnectAddress);
    if (avr != NULL
            && (avrAddress == logicalAddr)
            && isConnectedToArcPort
            && isDirectConnectAddress) {
        if (shouldCheckArcFeatureEnabled) {
            return IsArcFeatureEnabled(result);
        } else {
            *result = TRUE;
            return NOERROR;
        }
    } else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleTerminateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    // In cast of termination, do not check ARC configuration in that AVR device
    // might be removed already.
    // In case where <Terminate Arc> is started by <Request ARC Termination>
    // need to clean up RequestArcInitiationAction.
    RemoveAction(ECLSID_CRequestArcTerminationAction);
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    AutoPtr<SetArcTransmissionStateAction> action = new SetArcTransmissionStateAction();
    action->constructor(this, srcAddr, FALSE);
    AddAndStartAction(action);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleSetSystemAudioMode(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Boolean isMessageForSystemAudio;
    IsMessageForSystemAudio(message, &isMessageForSystemAudio);
    if (!isMessageForSystemAudio) {
        HdmiLogger::Warning("Invalid <Set System Audio Mode> message:%s", TO_CSTR(message));
        ((HdmiControlService*)mService.Get())->MaySendFeatureAbortCommand(message, Constants::ABORT_REFUSED);
        *result = TRUE;
        return NOERROR;
    }
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    AutoPtr<SystemAudioActionFromAvr> action = new SystemAudioActionFromAvr();
    action->constructor(this, srcAddr, HdmiUtils::ParseCommandParamSystemAudioStatus(message), NULL);
    AddAndStartAction(action);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleSystemAudioModeStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Boolean isMessageForSystemAudio;
    IsMessageForSystemAudio(message, &isMessageForSystemAudio);
    if (!isMessageForSystemAudio) {
        HdmiLogger::Warning("Invalid <System Audio Mode Status> message:%s", TO_CSTR(message));
        // Ignore this message.
        *result = TRUE;
        return NOERROR;
    }
    SetSystemAudioMode(HdmiUtils::ParseCommandParamSystemAudioStatus(message), TRUE);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleRecordTvScreen(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> actions;
    GetActions(ECLSID_COneTouchRecordAction, (IList**)&actions);
    Boolean isEmpty;
    actions->IsEmpty(&isEmpty);
    if (!isEmpty) {
        // Assumes only one OneTouchRecordAction.
        AutoPtr<IInterface> obj;
        actions->Get(0, (IInterface**)&obj);
        AutoPtr<OneTouchRecordAction> action = (OneTouchRecordAction*)IObject::Probe(obj);
        Int32 srcAddr;
        message->GetSource(&srcAddr);
        Int32 recorderAddress;
        action->GetRecorderAddress(&recorderAddress);
        if (recorderAddress != srcAddr) {
            AnnounceOneTouchRecordResult(
                    IHdmiControlManager::ONE_TOUCH_RECORD_PREVIOUS_RECORDING_IN_PROGRESS);
        }
        return HdmiCecLocalDevice::HandleRecordTvScreen(message, result);
    }
    Int32 recorderAddress;
    message->GetSource(&recorderAddress);
    AutoPtr<ArrayOf<Byte> > recordSource;
    ((HdmiControlService*)mService.Get())->InvokeRecordRequestListener(recorderAddress, (ArrayOf<Byte>**)&recordSource);
    Int32 reason;
    StartOneTouchRecord(recorderAddress, recordSource, &reason);
    if (reason != Constants::ABORT_NO_ERROR) {
        ((HdmiControlService*)mService.Get())->MaySendFeatureAbortCommand(message, reason);
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleTimerClearedStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 timerClearedStatusData = (*params)[0] & 0xFF;
    AnnounceTimerRecordingResult(timerClearedStatusData);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::AnnounceOneTouchRecordResult(
    /* [in] */ Int32 result)
{
    ((HdmiControlService*)mService.Get())->InvokeOneTouchRecordResult(result);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::AnnounceTimerRecordingResult(
    /* [in] */ Int32 result)
{
    ((HdmiControlService*)mService.Get())->InvokeTimerRecordingResult(result);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::AnnounceClearTimerRecordingResult(
    /* [in] */ Int32 result)
{
    ((HdmiControlService*)mService.Get())->InvokeClearTimerRecordingResult(result);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsMessageForSystemAudio(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 dest;
    message->GetDestination(&dest);
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
    *result = isControlEnabled
            && srcAddr == Constants::ADDR_AUDIO_SYSTEM
            && (dest == Constants::ADDR_TV || dest == Constants::ADDR_BROADCAST)
            && deviceInfo != NULL;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::AddDeviceInfo(
    /* [in] */ IHdmiDeviceInfo* deviceInfo,
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    Int32 logicalAddr;
    deviceInfo->GetLogicalAddress(&logicalAddr);
    AutoPtr<IHdmiDeviceInfo> oldDeviceInfo;
    GetCecDeviceInfo(logicalAddr, (IHdmiDeviceInfo**)&oldDeviceInfo);
    Int32 id;
    deviceInfo->GetId(&id);
    if (oldDeviceInfo != NULL) {
        AutoPtr<IHdmiDeviceInfo> iNoUse;
        RemoveDeviceInfo(id, (IHdmiDeviceInfo**)&iNoUse);
    }
    mDeviceInfos->Append(id, deviceInfo);
    UpdateSafeDeviceInfoList();
    *result = oldDeviceInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::RemoveDeviceInfo(
    /* [in] */ Int32 id,
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    AutoPtr<IInterface> obj;
    mDeviceInfos->Get(id, (IInterface**)&obj);
    AutoPtr<IHdmiDeviceInfo> deviceInfo = IHdmiDeviceInfo::Probe(obj);
    if (deviceInfo != NULL) {
        mDeviceInfos->Remove(id);
    }
    UpdateSafeDeviceInfoList();
    *result = deviceInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetDeviceInfoList(
    /* [in] */ Boolean includeLocalDevice,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    if (includeLocalDevice) {
        AutoPtr<IList> temp = HdmiUtils::SparseArrayToList(mDeviceInfos);
        *result = temp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IArrayList> infoList;
        CArrayList::New((IArrayList**)&infoList);
        Int32 size;
        mDeviceInfos->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            mDeviceInfos->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
            Int32 logicalAddr;
            info->GetLogicalAddress(&logicalAddr);
            Boolean isLocalDeviceAddress;
            IsLocalDeviceAddress(logicalAddr, &isLocalDeviceAddress);
            if (!isLocalDeviceAddress) {
                infoList->Add(info);
            }
        }
        *result = IList::Probe(infoList);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetSafeExternalInputsLocked(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSafeExternalInputs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::UpdateSafeDeviceInfoList()
{
    AssertRunOnServiceThread();
    AutoPtr<IList> copiedDevices = HdmiUtils::SparseArrayToList(mDeviceInfos);
    AutoPtr<IList> externalInputs;
    GetInputDevices((IList**)&externalInputs);
    {    AutoLock syncLock(mLock);
        mSafeAllDeviceInfos = copiedDevices;
        mSafeExternalInputs = externalInputs;
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetInputDevices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> infoList;
    CArrayList::New((IArrayList**)&infoList);
    Int32 size;
    mDeviceInfos->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mDeviceInfos->ValueAt(i, (IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        Int32 logicalAddr;
        info->GetLogicalAddress(&logicalAddr);
        Boolean isLocalDeviceAddress;
        IsLocalDeviceAddress(logicalAddr, &isLocalDeviceAddress);
        if (isLocalDeviceAddress) {
            continue;
        }
        Boolean isSourceType;
        info->IsSourceType(&isSourceType);
        Boolean isHideDevicesBehindLegacySwitchOk;
        HideDevicesBehindLegacySwitch(info, &isHideDevicesBehindLegacySwitchOk);
        if (isSourceType && !isHideDevicesBehindLegacySwitchOk) {
            infoList->Add(info);
        }
    }
    *result = IList::Probe(infoList);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HideDevicesBehindLegacySwitch(
    /* [in] */ IHdmiDeviceInfo* info,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 physicalAddr;
    info->GetPhysicalAddress(&physicalAddr);
    Boolean isConnectedToCecSwitch;
    IsConnectedToCecSwitch(physicalAddr, ICollection::Probe(mCecSwitches), &isConnectedToCecSwitch);
    *result = HdmiConfig::HIDE_DEVICES_BEHIND_LEGACY_SWITCH
            && !isConnectedToCecSwitch;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsConnectedToCecSwitch(
    /* [in] */ Int32 path,
    /* [in] */ ICollection* switches,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIterator> it;
    switches->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int32 switchPath;
        IInteger32::Probe(obj)->GetValue(&switchPath);
        Boolean isParentPath;
        IsParentPath(switchPath, path, &isParentPath);
        if (isParentPath) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsParentPath(
    /* [in] */ Int32 parentPath,
    /* [in] */ Int32 childPath,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // (A000, AB00) (AB00, ABC0), (ABC0, ABCD)
    // If child's last non-zero nibble is removed, the result equals to the parent.
    for (Int32 i = 0; i <= 12; i += 4) {
        Int32 nibble = (childPath >> i) & 0xF;
        if (nibble != 0) {
            Int32 parentNibble = (parentPath >> i) & 0xF;
            *result = parentNibble == 0 && (childPath >> (i+4)) == (parentPath >> (i+4));
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::InvokeDeviceEventListener(
    /* [in] */ IHdmiDeviceInfo* info,
    /* [in] */ Int32 status)
{
    Boolean isSourceType;
    info->IsSourceType(&isSourceType);
    Boolean isHideDevicesBehindLegacySwitch;
    HideDevicesBehindLegacySwitch(info, &isHideDevicesBehindLegacySwitch);
    if (isSourceType && !isHideDevicesBehindLegacySwitch) {
        ((HdmiControlService*)mService.Get())->InvokeDeviceEventListeners(info, status);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsLocalDeviceAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<IList> devices;
    ((HdmiControlService*)mService.Get())->GetAllLocalDevices((IList**)&devices);
    AutoPtr<IIterator> it;
    devices->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<HdmiCecLocalDevice> device = (HdmiCecLocalDevice*) IObject::Probe(obj);
        Boolean isAddressOf;
        device->IsAddressOf(address, &isAddressOf);
        if (isAddressOf) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetAvrDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    AssertRunOnServiceThread();
    return GetCecDeviceInfo(Constants::ADDR_AUDIO_SYSTEM, result);
}

ECode HdmiCecLocalDeviceTv::GetCecDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    AutoPtr<IInterface> obj;
    Int32 device;
    AutoPtr<IHdmiDeviceInfoHelper> helper;
#if 0 // TODO: Waiting for CHdmiDeviceInfoHelper
    CHdmiDeviceInfoHelper::AcquireSingleton((IHdmiDeviceInfoHelper**)&helper);
#else
    assert(0);
#endif
    helper->IdForCecDevice(logicalAddress, &device);
    mDeviceInfos->Get(device, (IInterface**)&obj);
    *result = IHdmiDeviceInfo::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HasSystemAudioDevice(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiDeviceInfo> info;
    GetSafeAvrDeviceInfo((IHdmiDeviceInfo**)&info);
    *result = info != NULL;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetSafeAvrDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    return GetSafeCecDeviceInfo(Constants::ADDR_AUDIO_SYSTEM, result);
}

ECode HdmiCecLocalDeviceTv::GetSafeCecDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock syncLock(mLock);
        AutoPtr<IIterator> it;
        mSafeAllDeviceInfos->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
            Int32 logicalAddr;
            info->GetLogicalAddress(&logicalAddr);
            Boolean isCecDevice;
            info->IsCecDevice(&isCecDevice);
            if (isCecDevice && logicalAddr == logicalAddress) {
                *result = info;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
        *result = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::AddCecDevice(
    /* [in] */ IHdmiDeviceInfo* info)
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiDeviceInfo> iNoUse;
    AddDeviceInfo(info, (IHdmiDeviceInfo**)&iNoUse);
    Int32 logicalAddr;
    info->GetLogicalAddress(&logicalAddr);
    if (logicalAddr == mAddress) {
        // The addition of TV device itself should not be notified.
        return NOERROR;
    }
    InvokeDeviceEventListener(info, IHdmiControlManager::DEVICE_EVENT_ADD_DEVICE);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::RemoveCecDevice(
    /* [in] */ Int32 address)
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiDeviceInfoHelper> helper;
#if 0 // TODO: Waiting for CHdmiDeviceInfoHelper
    CHdmiDeviceInfoHelper::AcquireSingleton((IHdmiDeviceInfoHelper**)&helper);
#else
    assert(0);
#endif
    Int32 device;
    helper->IdForCecDevice(address, &device);
    AutoPtr<IHdmiDeviceInfo> info;
    RemoveDeviceInfo(device, (IHdmiDeviceInfo**)&info);
    mCecMessageCache->FlushMessagesFrom(address);
    InvokeDeviceEventListener(info, IHdmiControlManager::DEVICE_EVENT_REMOVE_DEVICE);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleRemoveActiveRoutingPath(
    /* [in] */ Int32 path)
{
    AssertRunOnServiceThread();
    // Seq #23
    Int32 activePath;
    GetActivePath(&activePath);
    Boolean isTailOfActivePath;
    IsTailOfActivePath(path, activePath, &isTailOfActivePath);
    if (isTailOfActivePath) {
        RemoveAction(ECLSID_CRoutingControlAction);
        Int32 newPath;
        Int32 activePortId;
        GetActivePortId(&activePortId);
        ((HdmiControlService*)mService.Get())->PortIdToPath(activePortId, &newPath);
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildRoutingChange(mAddress, activePath, newPath, (IHdmiCecMessage**)&cmd);
        ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
        ((HdmiCecLocalDevice::ActiveSource*) mActiveSource.Get())->Invalidate();
        AutoPtr<RoutingControlAction> newRoutingControlAction = new RoutingControlAction();
        newRoutingControlAction->constructor(this, activePath, TRUE, NULL);
        AddAndStartAction(newRoutingControlAction);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::LaunchRoutingControl(
    /* [in] */ Boolean routingForBootup)
{
    AssertRunOnServiceThread();
    // Seq #24
    Int32 activePortId;
    GetActivePortId(&activePortId);
    if (activePortId != Constants::INVALID_PORT_ID) {
        Boolean isProhibitMode;
        IsProhibitMode(&isProhibitMode);
        if (!routingForBootup && !isProhibitMode) {
            RemoveAction(ECLSID_CRoutingControlAction);
            Int32 newPath;
            Int32 activePortId;
            GetActivePortId(&activePortId);
            ((HdmiControlService*)mService.Get())->PortIdToPath(activePortId, &newPath);
            SetActivePath(newPath);
            Int32 activePath;
            GetActivePath(&activePath);
            AutoPtr<IHdmiCecMessage> cmd;
            HdmiCecMessageBuilder::BuildRoutingChange(mAddress, activePath, newPath, (IHdmiCecMessage**)&cmd);
            ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
            AutoPtr<RoutingControlAction> routingControlAction = new RoutingControlAction();
            routingControlAction->constructor(this, activePortId,routingForBootup, NULL);
            AddAndStartAction(routingControlAction);
        }
    } else {
        Int32 activePath;
        ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&activePath);
        SetActivePath(activePath);
        if (!routingForBootup) {
            AutoPtr<IHdmiCecMessage> activeSource;
            HdmiCecMessageBuilder::BuildActiveSource(mAddress, activePath, (IHdmiCecMessage**)&activeSource);
            ((HdmiControlService*)mService.Get())->SendCecCommand(activeSource);
        }
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetDeviceInfoByPath(
    /* [in] */ Int32 path,
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    AutoPtr<IList> infos;
    GetDeviceInfoList(FALSE, (IList**)&infos);
    AutoPtr<IIterator> it;
    infos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        Int32 physicalAddr;
        info->GetPhysicalAddress(&physicalAddr);
        if (physicalAddr == path) {
            *result = info;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsInDeviceList(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<IHdmiDeviceInfo> device;
    GetCecDeviceInfo(logicalAddress, (IHdmiDeviceInfo**)&device);
    if (device == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 physicalAddr;
    device->GetPhysicalAddress(&physicalAddr);
    *result = physicalAddr == physicalAddress;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    AssertRunOnServiceThread();
    if (!connected) {
        RemoveCecSwitches(portId);
    }
    // Tv device will have permanent HotplugDetectionAction.
    AutoPtr<IList> hotplugActions;
    GetActions(ECLSID_CHotplugDetectionAction, (IList**)&hotplugActions);
    Boolean isEmpty;
    hotplugActions->IsEmpty(&isEmpty);
    if (!isEmpty) {
        // Note that hotplug action is single action running on a machine.
        // "pollAllDevicesNow" cleans up timer and start poll action immediately.
        // It covers seq #40, #43.
        AutoPtr<IInterface> obj;
        hotplugActions->Get(0, (IInterface**)&obj);
        ((HotplugDetectionAction*)IObject::Probe(obj))->PollAllDevicesNow();
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::RemoveCecSwitches(
    /* [in] */ Int32 portId)
{
    AutoPtr<IIterator> it;
    ISet::Probe(mCecSwitches)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), !hasNext) {
        Int32 path;
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&path);
        Int32 thisPortId;
        PathToPortId(path, &thisPortId);
        if (thisPortId == portId) {
            it->Remove();
        }
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::SetAutoDeviceOff(
    /* [in] */ Boolean enabled)
{
    AssertRunOnServiceThread();
    mAutoDeviceOff = enabled;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::SetAutoWakeup(
    /* [in] */ Boolean enabled)
{
    AssertRunOnServiceThread();
    mAutoWakeup = enabled;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::GetAutoWakeup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    *result = mAutoWakeup;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::DisableDevice(
    /* [in] */ Boolean initiatedByCec,
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback)
{
    HdmiCecLocalDevice::DisableDevice(initiatedByCec, callback);
    AssertRunOnServiceThread();
    // Remove any repeated working actions.
    // HotplugDetectionAction will be reinstated during the wake up process.
    // HdmiControlService.onWakeUp() -> initializeLocalDevices() ->
    //     LocalDeviceTv.onAddressAllocated() -> launchDeviceDiscovery().
    RemoveAction(ECLSID_CDeviceDiscoveryAction);
    RemoveAction(ECLSID_CHotplugDetectionAction);
    RemoveAction(ECLSID_CPowerStatusMonitorAction);
    // Remove recording actions.
    RemoveAction(ECLSID_COneTouchRecordAction);
    RemoveAction(ECLSID_CTimerRecordingAction);
    DisableSystemAudioIfExist();
    DisableArcIfExist();
    ClearDeviceInfoList();
    CheckIfPendingActionsCleared();
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::DisableSystemAudioIfExist()
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
    if (deviceInfo == NULL) {
        return NOERROR;
    }
    // Seq #31.
    RemoveAction(ECLSID_CSystemAudioActionFromAvr);
    RemoveAction(ECLSID_CSystemAudioActionFromTv);
    RemoveAction(ECLSID_CSystemAudioAutoInitiationAction);
    RemoveAction(ECLSID_CSystemAudioStatusAction);
    RemoveAction(ECLSID_CVolumeControlAction);
    // Turn off the mode but do not write it the settings, so that the next time TV powers on
    // the system audio mode setting can be restored automatically.
    SetSystemAudioMode(FALSE, FALSE);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::DisableArcIfExist()
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiDeviceInfo> avr;
    GetAvrDeviceInfo((IHdmiDeviceInfo**)&avr);
    if (avr == NULL) {
        return NOERROR;
    }
    // Seq #44.
    RemoveAction(ECLSID_CRequestArcInitiationAction);
    Boolean hasAction;
    HasAction(ECLSID_CRequestArcTerminationAction, &hasAction);
    Boolean isArcEstabilished;
    IsArcEstabilished(&isArcEstabilished);
    if (!hasAction && isArcEstabilished) {
        Int32 logicalAddr;
        avr->GetLogicalAddress(&logicalAddr);
        AutoPtr<RequestArcTerminationAction> newRequestArcTerminationAction = new RequestArcTerminationAction();
        newRequestArcTerminationAction->constructor(this, logicalAddr);
        AddAndStartAction(newRequestArcTerminationAction);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::OnStandby(
    /* [in] */ Boolean initiatedByCec)
{
    AssertRunOnServiceThread();
    // Seq #11
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        return NOERROR;
    }
    if (!initiatedByCec && mAutoDeviceOff) {
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildStandby(mAddress, Constants::ADDR_BROADCAST, (IHdmiCecMessage**)&cmd);
        ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    }
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::IsProhibitMode(
    /* [out] */ Boolean* result)
{
    return ((HdmiControlService*)mService.Get())->IsProhibitMode(result);
}

ECode HdmiCecLocalDeviceTv::IsPowerStandbyOrTransient(
    /* [out] */ Boolean* result)
{
    return ((HdmiControlService*)mService.Get())->IsPowerStandbyOrTransient(result);
}

ECode HdmiCecLocalDeviceTv::DisplayOsd(
    /* [in] */ Int32 messageId)
{
    AssertRunOnServiceThread();
    ((HdmiControlService*)mService.Get())->DisplayOsd(messageId);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::DisplayOsd(
    /* [in] */ Int32 messageId,
    /* [in] */ Int32 extra)
{
    AssertRunOnServiceThread();
    ((HdmiControlService*)mService.Get())->DisplayOsd(messageId, extra);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::StartOneTouchRecord(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
        AnnounceOneTouchRecordResult(IHdmiControlManager::ONE_TOUCH_RECORD_CEC_DISABLED);
        *result = Constants::ABORT_NOT_IN_CORRECT_MODE;
        return NOERROR;
    }
    Boolean isCheckRecorderOk;
    CheckRecorder(recorderAddress, &isCheckRecorderOk);
    if (!isCheckRecorderOk) {
        Slogger::W(TAG, "Invalid recorder address:%d", recorderAddress);
        AnnounceOneTouchRecordResult(IHdmiControlManager::ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
        *result = Constants::ABORT_NOT_IN_CORRECT_MODE;
        return NOERROR;
    }
    CheckRecordSource(recordSource, &isCheckRecorderOk);
    if (!isCheckRecorderOk) {
        Slogger::W(TAG, "Invalid record source.%s", Arrays::ToString(recordSource).string());
        AnnounceOneTouchRecordResult(IHdmiControlManager::ONE_TOUCH_RECORD_FAIL_TO_RECORD_DISPLAYED_SCREEN);
        *result = Constants::ABORT_UNABLE_TO_DETERMINE;
        return NOERROR;
    }
    AutoPtr<OneTouchRecordAction> newOneTouchRecordAction = new OneTouchRecordAction();
    newOneTouchRecordAction->constructor(this, recorderAddress, recordSource);
    AddAndStartAction(newOneTouchRecordAction);
    Slogger::I(TAG, "Start new [One Touch Record]-Target:%d, recordSource:%s",
            recorderAddress, Arrays::ToString(recordSource).string());
    *result = Constants::ABORT_NO_ERROR;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::StopOneTouchRecord(
    /* [in] */ Int32 recorderAddress)
{
    AssertRunOnServiceThread();
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        Slogger::W(TAG, "Can not stop one touch record. CEC control is disabled.");
        AnnounceOneTouchRecordResult(IHdmiControlManager::ONE_TOUCH_RECORD_CEC_DISABLED);
        return NOERROR;
    }
    Boolean isCheckRecorderOk;
    CheckRecorder(recorderAddress, &isCheckRecorderOk);
    if (!isCheckRecorderOk) {
        Slogger::W(TAG, "Invalid recorder address:%d", recorderAddress);
        AnnounceOneTouchRecordResult(IHdmiControlManager::ONE_TOUCH_RECORD_CHECK_RECORDER_CONNECTION);
        return NOERROR;
    }
    // Remove one touch record action so that other one touch record can be started.
    RemoveAction(ECLSID_COneTouchRecordAction);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildRecordOff(mAddress, recorderAddress, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    Slogger::I(TAG, "Stop [One Touch Record]-Target:%s", recorderAddress);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::CheckRecorder(
    /* [in] */ Int32 recorderAddress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiDeviceInfo> device;
    GetCecDeviceInfo(recorderAddress, (IHdmiDeviceInfo**)&device);
    *result = (device != NULL)
            && (HdmiUtils::GetTypeFromAddress(recorderAddress)
                    == IHdmiDeviceInfo::DEVICE_RECORDER);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::CheckRecordSource(
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiRecordSourcesHelper> helper;
#if 0 // TODO: Waiting for CHdmiRecordSources
    CHdmiRecordSources::AcquireSingleton((IHdmiRecordSourcesHelper**)&helper);
#else
    assert(0);
#endif
    Boolean isCheckRecordSourceOk;
    helper->CheckRecordSource(recordSource, &isCheckRecordSourceOk);
    *result = (recordSource != NULL) && isCheckRecordSourceOk;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::StartTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    AssertRunOnServiceThread();
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
        AnnounceTimerRecordingResult(IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_CEC_DISABLED);
        return NOERROR;
    }
    Boolean isCheckRecorderOk;
    CheckRecorder(recorderAddress, &isCheckRecorderOk);
    if (!isCheckRecorderOk) {
        Slogger::W(TAG, "Invalid recorder address:%d", recorderAddress);
        AnnounceTimerRecordingResult(
                IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_CHECK_RECORDER_CONNECTION);
        return NOERROR;
    }
    Boolean isCheckTimerRecordingSourceOk;
    CheckTimerRecordingSource(sourceType, recordSource, &isCheckTimerRecordingSourceOk);
    if (!isCheckTimerRecordingSourceOk) {
        Slogger::W(TAG, "Invalid record source.%s", Arrays::ToString(recordSource).string());
        AnnounceTimerRecordingResult(
                IHdmiControlManager::TIMER_RECORDING_RESULT_EXTRA_FAIL_TO_RECORD_SELECTED_SOURCE);
        return NOERROR;
    }
    AutoPtr<TimerRecordingAction> newTimerRecordingAction = new TimerRecordingAction();
    newTimerRecordingAction->constructor(this, recorderAddress, sourceType, recordSource);
    AddAndStartAction(newTimerRecordingAction);
    Slogger::I(TAG, "Start [Timer Recording]-Target:%d, SourceType:%d, RecordSource:%s",
            recorderAddress, sourceType, Arrays::ToString(recordSource).string());
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::CheckTimerRecordingSource(
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiTimerRecordSourcesHelper> helper;
#if 0 // TODO: Waiting for CHdmiTimerRecordSourcesHelper
    CHdmiTimerRecordSourcesHelper::AcquireSingleton((IHdmiTimerRecordSourcesHelper**)&helper);
#else
    assert(0);
#endif
    Boolean isCheckTimerRecordSourceOk;
    helper->CheckTimerRecordSource(sourceType, recordSource, &isCheckTimerRecordSourceOk);
    *result = (recordSource != NULL)
            && isCheckTimerRecordSourceOk;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::ClearTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    AssertRunOnServiceThread();
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        Slogger::W(TAG, "Can not start one touch record. CEC control is disabled.");
        AnnounceClearTimerRecordingResult(IHdmiControlManager::CLEAR_TIMER_STATUS_CEC_DISABLE);
        return NOERROR;
    }
    Boolean isCheckRecorderOk;
    CheckRecorder(recorderAddress, &isCheckRecorderOk);
    if (!isCheckRecorderOk) {
        Slogger::W(TAG, "Invalid recorder address:%d", recorderAddress);
        AnnounceClearTimerRecordingResult(IHdmiControlManager::CLEAR_TIMER_STATUS_CHECK_RECORDER_CONNECTION);
        return NOERROR;
    }
    Boolean isCheckTimerRecordingSourceOk;
    CheckTimerRecordingSource(sourceType, recordSource, &isCheckTimerRecordingSourceOk);
    if (!isCheckTimerRecordingSourceOk) {
        Slogger::W(TAG, "Invalid record source.%s", Arrays::ToString(recordSource).string());
        AnnounceClearTimerRecordingResult(IHdmiControlManager::CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
        return NOERROR;
    }
    SendClearTimerMessage(recorderAddress, sourceType, recordSource);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::SendClearTimerMessage(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    AutoPtr<IHdmiCecMessage> message = NULL;
    switch (sourceType) {
        case IHdmiControlManager::TIMER_RECORDING_TYPE_DIGITAL:
            HdmiCecMessageBuilder::BuildClearDigitalTimer(
                    mAddress, recorderAddress, recordSource, (IHdmiCecMessage**)&message);
            break;
        case IHdmiControlManager::TIMER_RECORDING_TYPE_ANALOGUE:
            HdmiCecMessageBuilder::BuildClearAnalogueTimer(
                    mAddress, recorderAddress, recordSource, (IHdmiCecMessage**)&message);
            break;
        case IHdmiControlManager::TIMER_RECORDING_TYPE_EXTERNAL:
            HdmiCecMessageBuilder::BuildClearExternalTimer(
                    mAddress, recorderAddress, recordSource, (IHdmiCecMessage**)&message);
            break;
        default:
            Slogger::W(TAG, "Invalid source type:%d", recorderAddress);
            AnnounceClearTimerRecordingResult(IHdmiControlManager::CLEAR_TIMER_STATUS_FAIL_TO_CLEAR_SELECTED_SOURCE);
            return NOERROR;
    }
    ((HdmiControlService*)mService.Get())->SendCecCommand(message, new InnerSub_SendMessageCallback(this));
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::UpdateDevicePowerStatus(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 newPowerStatus)
{
    AutoPtr<IHdmiDeviceInfo> info;
    GetCecDeviceInfo(logicalAddress, (IHdmiDeviceInfo**)&info);
    if (info == NULL) {
        Slogger::W(TAG, "Can not update power status of non-existing device:%d", logicalAddress);
        return NOERROR;
    }
    Int32 devicePowerStatus;
    info->GetDevicePowerStatus(&devicePowerStatus);
    if (devicePowerStatus == newPowerStatus) {
        return NOERROR;
    }
    AutoPtr<IHdmiDeviceInfo> newInfo = HdmiUtils::CloneHdmiDeviceInfo(info, newPowerStatus);
    // addDeviceInfo replaces old device info with new one if exists.
    AutoPtr<IHdmiDeviceInfo> iNoUse;
    AddDeviceInfo(newInfo, (IHdmiDeviceInfo**)&iNoUse);
    InvokeDeviceEventListener(newInfo, IHdmiControlManager::DEVICE_EVENT_UPDATE_DEVICE);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::HandleMenuStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Do nothing and just return true not to prevent from responding <Feature Abort>.
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::SendStandby(
    /* [in] */ Int32 deviceId)
{
    AutoPtr<IInterface> obj;
    mDeviceInfos->Get(deviceId, (IInterface**)&obj);
    AutoPtr<IHdmiDeviceInfo> targetDevice = IHdmiDeviceInfo::Probe(obj);
    if (targetDevice == NULL) {
        return NOERROR;
    }
    Int32 targetAddress;
    targetDevice->GetLogicalAddress(&targetAddress);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildStandby(mAddress, targetAddress, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    return NOERROR;
}

ECode HdmiCecLocalDeviceTv::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    HdmiCecLocalDevice::Dump(pw);
    String s;
    s.AppendFormat("mArcEstablished: %s", mArcEstablished ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    s = "mArcFeatureEnabled: ";
    s.AppendFormat("%s", mArcFeatureEnabled ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    s = "mSystemAudioActivated: ";
    s.AppendFormat("%s", mSystemAudioActivated ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    s = "mSystemAudioMute: ";
    s.AppendFormat("%s", mSystemAudioMute ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    s = "mAutoDeviceOff: ";
    s.AppendFormat("%s", mAutoDeviceOff ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    s = "mAutoWakeup: ";
    s.AppendFormat("%s", mAutoWakeup ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    s = "mSkipRoutingControl: ";
    s.AppendFormat("%s", mSkipRoutingControl ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    IPrintWriter::Probe(pw)->Println(String("CEC devices:"));
    pw->IncreaseIndent();
    AutoPtr<IIterator> it;
    mSafeAllDeviceInfos->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
        IPrintWriter::Probe(pw)->Println(info);
    }
    pw->DecreaseIndent();
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
