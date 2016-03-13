
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/DevicePowerStatusAction.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevicePlayback.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageCache.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/OneTouchPlayAction.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Utility::ISlog;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String HdmiCecLocalDevicePlayback::TAG("HdmiCecLocalDevicePlayback");

HdmiCecLocalDevicePlayback::HdmiCecLocalDevicePlayback()
    : mIsActiveSource(FALSE)
{}

ECode HdmiCecLocalDevicePlayback::constructor(
    /* [in] */ IHdmiControlService* service)
{
    return HdmiCecLocalDevice::constructor(service, IHdmiDeviceInfo::DEVICE_PLAYBACK);
}

ECode HdmiCecLocalDevicePlayback::Init()
{
    HdmiCecLocalDevice::Init();
    mIsActiveSource = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::OnAddressAllocated(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 reason)
{
    AssertRunOnServiceThread();
    Int32 physicalAddr;
    ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildReportPhysicalAddressCommand(mAddress, physicalAddr, mDeviceType, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    StartQueuedActions();
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::GetPreferredAddress(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AssertRunOnServiceThread();
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    return helper->GetInt32(Constants::PROPERTY_PREFERRED_ADDRESS_PLAYBACK,
            Constants::ADDR_UNREGISTERED, result);
}

ECode HdmiCecLocalDevicePlayback::SetPreferredAddress(
    /* [in] */ Int32 addr)
{
    AssertRunOnServiceThread();
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    helper->Set(Constants::PROPERTY_PREFERRED_ADDRESS_PLAYBACK,
            StringUtils::ToString(addr));
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::OneTouchPlay(
    /* [in] */ IIHdmiControlCallback* callback)
{
    AssertRunOnServiceThread();
    Boolean hasAction;
    HasAction(ECLSID_COneTouchPlayAction, &hasAction);
    if (hasAction) {
        Slogger::W(TAG, "oneTouchPlay already in progress");
        InvokeCallback(callback, IHdmiControlManager::RESULT_ALREADY_IN_PROGRESS);
        return NOERROR;
    }
    // TODO: Consider the case of multiple TV sets. For now we always direct the command
    //       to the primary one.
    AutoPtr<OneTouchPlayAction> action;
    OneTouchPlayAction::Create(this, Constants::ADDR_TV,
            callback, (OneTouchPlayAction**)&action);
    if (action == NULL) {
        Slogger::W(TAG, "Cannot initiate oneTouchPlay");
        InvokeCallback(callback, IHdmiControlManager::RESULT_EXCEPTION);
        return NOERROR;
    }
    AddAndStartAction(action);
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::QueryDisplayStatus(
    /* [in] */ IIHdmiControlCallback* callback)
{
    AssertRunOnServiceThread();
    Boolean hasAction;
    HasAction(ECLSID_CDevicePowerStatusAction, &hasAction);
    if (hasAction) {
        Slogger::W(TAG, "queryDisplayStatus already in progress");
        InvokeCallback(callback, IHdmiControlManager::RESULT_ALREADY_IN_PROGRESS);
        return NOERROR;
    }
    AutoPtr<DevicePowerStatusAction> action;
    DevicePowerStatusAction::Create(this, Constants::ADDR_TV, callback, (DevicePowerStatusAction**)&action);
    if (action == NULL) {
        Slogger::W(TAG, "Cannot initiate queryDisplayStatus");
        InvokeCallback(callback, IHdmiControlManager::RESULT_EXCEPTION);
        return NOERROR;
    }
    AddAndStartAction(action);
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::InvokeCallback(
    /* [in] */ IIHdmiControlCallback* callback,
    /* [in] */ Int32 result)
{
    AssertRunOnServiceThread();
    // try {
    ECode ec = callback->OnComplete(result);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Invoking callback failed:%d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    AssertRunOnServiceThread();
    mCecMessageCache->FlushAll();
    // We'll not clear mIsActiveSource on the hotplug event to pass CETC 11.2.2-2 ~ 3.
    Boolean isPowerStandbyOrTransient;
    ((HdmiControlService*)mService.Get())->IsPowerStandbyOrTransient(&isPowerStandbyOrTransient);
    if (connected && isPowerStandbyOrTransient) {
        ((HdmiControlService*)mService.Get())->WakeUp();
    }
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::MarkActiveSource()
{
    AssertRunOnServiceThread();
    mIsActiveSource = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::HandleActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 physicalAddress = HdmiUtils::TwoBytesToInt32(params);
    MayResetActiveSource(physicalAddress);
    *result = TRUE;
    return NOERROR;  // Broadcast message.
}

ECode HdmiCecLocalDevicePlayback::MayResetActiveSource(
    /* [in] */ Int32 physicalAddress)
{
    Int32 physicalAddr;
    ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
    if (physicalAddress != physicalAddr) {
        mIsActiveSource = FALSE;
    }
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::HandleSetStreamPath(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 physicalAddress = HdmiUtils::TwoBytesToInt32(params);
    MaySetActiveSource(physicalAddress);
    MaySendActiveSource();
    WakeUpIfActiveSource();
    *result = TRUE;
    return NOERROR;  // Broadcast message.
}

ECode HdmiCecLocalDevicePlayback::HandleRoutingChange(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 newPath = HdmiUtils::TwoBytesToInt32(params, 2);
    MaySetActiveSource(newPath);
    *result = TRUE;
    return NOERROR;  // Broadcast message.
}

ECode HdmiCecLocalDevicePlayback::HandleRoutingInformation(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 physicalAddress = HdmiUtils::TwoBytesToInt32(params);
    MaySetActiveSource(physicalAddress);
    *result = TRUE;
    return NOERROR;  // Broadcast message.
}

ECode HdmiCecLocalDevicePlayback::MaySetActiveSource(
    /* [in] */ Int32 physicalAddress)
{
    Int32 physicalAddr;
    ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
    if (physicalAddress == physicalAddr) {
        mIsActiveSource = TRUE;
    } else {
        mIsActiveSource = FALSE;
    }
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::WakeUpIfActiveSource()
{
    Boolean isPowerStandbyOrTransient;
    ((HdmiControlService*)mService.Get())->IsPowerStandbyOrTransient(&isPowerStandbyOrTransient);
    if (mIsActiveSource && isPowerStandbyOrTransient) {
        ((HdmiControlService*)mService.Get())->WakeUp();
    }
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::MaySendActiveSource()
{
    if (mIsActiveSource) {
        Int32 physicalAddr;
        ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
        AutoPtr<IHdmiCecMessage> activeSource;
        HdmiCecMessageBuilder::BuildActiveSource(mAddress, physicalAddr, (IHdmiCecMessage**)&activeSource);
        ((HdmiControlService*)mService.Get())->SendCecCommand(activeSource);
    }
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::HandleRequestActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    MaySendActiveSource();
    *result = TRUE;
    return NOERROR;  // Broadcast message.
}

ECode HdmiCecLocalDevicePlayback::DisableDevice(
    /* [in] */ Boolean initiatedByCec,
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback)
{
    HdmiCecLocalDevice::DisableDevice(initiatedByCec, callback);
    AssertRunOnServiceThread();
    if (!initiatedByCec && mIsActiveSource) {
        Int32 physicalAddr;
        ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddr);
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildInactiveSource(mAddress, physicalAddr, (IHdmiCecMessage**)&cmd);
        ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    }
    mIsActiveSource = FALSE;
    CheckIfPendingActionsCleared();
    return NOERROR;
}

ECode HdmiCecLocalDevicePlayback::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    HdmiCecLocalDevice::Dump(pw);
    String s;
    s.AppendFormat("mIsActiveSource: %s", mIsActiveSource ? "true" : "false");
    IPrintWriter::Probe(pw)->Println(s);
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
