
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDeviceTv.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageCache.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Hardware.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/server/hdmi/HdmiCecLocalDevicePlayback.h"
#include <elastos/droid/net/ReturnOutValue.h>
#include <Elastos.Droid.View.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.CoreLibrary.IO.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::Input::CInputManagerHelper;
using Elastos::Droid::Hardware::Input::IInputManagerHelper;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IInputEvent;
using Elastos::Core::StringBuffer;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEvent;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {
//=============================================================================
// HdmiCecLocalDevice::ActiveSource
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecLocalDevice::ActiveSource, Object, IHdmiCecLocalDeviceActiveSource)

ECode HdmiCecLocalDevice::ActiveSource::constructor()
{
    return Invalidate();
}

ECode HdmiCecLocalDevice::ActiveSource::constructor(
    /* [in] */ Int32 logical,
    /* [in] */ Int32 physical)
{
    mLogicalAddress = logical;
    mPhysicalAddress = physical;
    return NOERROR;
}

ECode HdmiCecLocalDevice::ActiveSource::Of(
    /* [in] */ Int32 logical,
    /* [in] */ Int32 physical,
    /* [out] */ IHdmiCecLocalDeviceActiveSource** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ActiveSource> rev = new ActiveSource();
    rev->constructor(logical, physical);
    *result = IHdmiCecLocalDeviceActiveSource::Probe(rev);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDevice::ActiveSource::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = HdmiUtils::IsValidAddress(mLogicalAddress);
    return NOERROR;
}

ECode HdmiCecLocalDevice::ActiveSource::Invalidate()
{
    mLogicalAddress = Constants::ADDR_INVALID;
    mPhysicalAddress = Constants::INVALID_PHYSICAL_ADDRESS;
    return NOERROR;
}

ECode HdmiCecLocalDevice::ActiveSource::IsEquals(
    /* [in] */ Int32 logical,
    /* [in] */ Int32 physical,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLogicalAddress == logical && mPhysicalAddress == physical;
    return NOERROR;
}

ECode HdmiCecLocalDevice::ActiveSource::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IHdmiCecLocalDeviceActiveSource::Probe(obj) != NULL) {
        AutoPtr<ActiveSource> that = (ActiveSource*) IHdmiCecLocalDeviceActiveSource::Probe(obj);
        *result = that->mLogicalAddress == mLogicalAddress &&
               that->mPhysicalAddress == mPhysicalAddress;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::ActiveSource::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLogicalAddress * 29 + mPhysicalAddress;
    return NOERROR;
}

ECode HdmiCecLocalDevice::ActiveSource::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuffer s;
    String logicalAddressString;
    if (mLogicalAddress == Constants::ADDR_INVALID)
        logicalAddressString = "invalid";
    else
        logicalAddressString.AppendFormat("0x%02x", mLogicalAddress);
    s.Append("logical_address: ");
    s.Append(logicalAddressString);
    String physicalAddressString;
    if (mPhysicalAddress == Constants::INVALID_PHYSICAL_ADDRESS)
        physicalAddressString = "invalid";
    else
        physicalAddressString.AppendFormat("0x%04x", mPhysicalAddress);
    s.Append(", physical_address: ");
    s.Append(physicalAddressString);
    return s.ToString(result);
}

//=============================================================================
// HdmiCecLocalDevice::InnerSub_Handler
//=============================================================================
HdmiCecLocalDevice::InnerSub_Handler::InnerSub_Handler(
    /* [in] */ HdmiCecLocalDevice* host)
    : mHost(host)
{}

ECode HdmiCecLocalDevice::InnerSub_Handler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == MSG_DISABLE_DEVICE_TIMEOUT)
        mHost->HandleDisableDeviceTimeout();
    else if (what == MSG_USER_CONTROL_RELEASE_TIMEOUT) {
        Boolean bNoUse;
        mHost->HandleUserControlReleased(&bNoUse);
    }
    return NOERROR;
}

//=============================================================================
// HdmiCecLocalDevice::InnerSub_PendingActionClearedCallback
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecLocalDevice::InnerSub_PendingActionClearedCallback, Object, IHdmiCecLocalDevicePendingActionClearedCallback)

HdmiCecLocalDevice::InnerSub_PendingActionClearedCallback::InnerSub_PendingActionClearedCallback(
    /* [in] */ HdmiCecLocalDevice* host,
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* origialCallback)
    : mHost(host)
    , mOrigialCallback(origialCallback)
{}

ECode HdmiCecLocalDevice::InnerSub_PendingActionClearedCallback::OnCleared(
    /* [in] */ IHdmiCecLocalDevice* device)
{
    mHost->mHandler->RemoveMessages(MSG_DISABLE_DEVICE_TIMEOUT);
    mOrigialCallback->OnCleared(device);
    return NOERROR;
}

//=============================================================================
// HdmiCecLocalDevice
//=============================================================================
CAR_INTERFACE_IMPL(HdmiCecLocalDevice, Object, IHdmiCecLocalDevice)

const String HdmiCecLocalDevice::TAG("HdmiCecLocalDevice");
const Int32 HdmiCecLocalDevice::MSG_DISABLE_DEVICE_TIMEOUT = 1;
const Int32 HdmiCecLocalDevice::MSG_USER_CONTROL_RELEASE_TIMEOUT = 2;
const Int32 HdmiCecLocalDevice::DEVICE_CLEANUP_TIMEOUT = 5000;
const Int32 HdmiCecLocalDevice::FOLLOWER_SAFETY_TIMEOUT = 550;

HdmiCecLocalDevice::HdmiCecLocalDevice()
    : mDeviceType(0)
    , mAddress(0)
    , mPreferredAddress(0)
    , mLastKeycode(HdmiCecKeycode::UNSUPPORTED_KEYCODE)
    , mLastKeyRepeatCount(0)
    , mActiveRoutingPath(0)
{
    mActiveSource = IHdmiCecLocalDeviceActiveSource::Probe(new ActiveSource());
    mCecMessageCache = new HdmiCecMessageCache();
    CLinkedList::New((ILinkedList**)&mActions);
    mHandler = new InnerSub_Handler(this);
}

ECode HdmiCecLocalDevice::constructor(
    /* [in] */ IHdmiControlService* service,
    /* [in] */ Int32 deviceType)
{
    mService = service;
    mDeviceType = deviceType;
    mAddress = Constants::ADDR_UNREGISTERED;
    AutoPtr<IInterface> obj;
    ((HdmiControlService*) service)->GetServiceLock((IInterface**)&obj);
    mLock = IObject::Probe(obj);
    return NOERROR;
}

ECode HdmiCecLocalDevice::Create(
    /* [in] */ IHdmiControlService* service,
    /* [in] */ Int32 deviceType,
    /* [out] */ IHdmiCecLocalDevice** result)
{
    VALIDATE_NOT_NULL(result)

    switch (deviceType) {
    case IHdmiDeviceInfo::DEVICE_TV:
        {
            AutoPtr<IHdmiCecLocalDeviceTv> tv;
            ((HdmiCecLocalDeviceTv*) tv.Get())->constructor(service);
            *result = IHdmiCecLocalDevice::Probe(tv);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    case IHdmiDeviceInfo::DEVICE_PLAYBACK:
        *result = IHdmiCecLocalDevice::Probe(new HdmiCecLocalDevicePlayback());
        ((HdmiCecLocalDevicePlayback*)(*result))->constructor(service);
        REFCOUNT_ADD(*result)
        return NOERROR;
    default:
        *result = NULL;
        return NOERROR;
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::Init()
{
    AssertRunOnServiceThread();
    GetPreferredAddress(&mPreferredAddress);
    return NOERROR;
}

ECode HdmiCecLocalDevice::DispatchMessage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Int32 dest;
    message->GetDestination(&dest);
    if (dest != mAddress && dest != Constants::ADDR_BROADCAST) {
        *result = FALSE;
        return NOERROR;
    }
    // Cache incoming message. Note that it caches only white-listed one.
    mCecMessageCache->CacheMessage(message);
    return OnMessage(message, result);
}

ECode HdmiCecLocalDevice::OnMessage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    if (DispatchMessageToAction(message)) {
        *result = TRUE;
        return NOERROR;
    }
    Int32 opcode;
    message->GetOpcode(&opcode);
    if (opcode == Constants::MESSAGE_ACTIVE_SOURCE)
        return HandleActiveSource(message, result);
    else if (opcode == Constants::MESSAGE_INACTIVE_SOURCE)
        return HandleInactiveSource(message, result);
    else if (opcode == Constants::MESSAGE_REQUEST_ACTIVE_SOURCE)
        return HandleRequestActiveSource(message, result);
    else if (opcode == Constants::MESSAGE_GET_MENU_LANGUAGE)
        return HandleGetMenuLanguage(message, result);
    else if (opcode == Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS)
        return HandleGivePhysicalAddress(result);
    else if (opcode == Constants::MESSAGE_GIVE_OSD_NAME)
        return HandleGiveOsdName(message, result);
    else if (opcode == Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID)
        return HandleGiveDeviceVendorId(result);
    else if (opcode == Constants::MESSAGE_GET_CEC_VERSION)
        return HandleGetCecVersion(message, result);
    else if (opcode == Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS)
        return HandleReportPhysicalAddress(message, result);
    else if (opcode == Constants::MESSAGE_ROUTING_CHANGE)
        return HandleRoutingChange(message, result);
    else if (opcode == Constants::MESSAGE_ROUTING_INFORMATION)
        return HandleRoutingInformation(message, result);
    else if (opcode == Constants::MESSAGE_INITIATE_ARC)
        return HandleInitiateArc(message, result);
    else if (opcode == Constants::MESSAGE_TERMINATE_ARC)
        return HandleTerminateArc(message, result);
    else if (opcode == Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE)
        return HandleSetSystemAudioMode(message, result);
    else if (opcode == Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS)
        return HandleSystemAudioModeStatus(message, result);
    else if (opcode == Constants::MESSAGE_REPORT_AUDIO_STATUS)
        return HandleReportAudioStatus(message, result);
    else if (opcode == Constants::MESSAGE_STANDBY)
        return HandleStandby(message, result);
    else if (opcode == Constants::MESSAGE_TEXT_VIEW_ON)
        return HandleTextViewOn(message, result);
    else if (opcode == Constants::MESSAGE_IMAGE_VIEW_ON)
        return HandleImageViewOn(message, result);
    else if (opcode == Constants::MESSAGE_USER_CONTROL_PRESSED)
        return HandleUserControlPressed(message, result);
    else if (opcode == Constants::MESSAGE_USER_CONTROL_RELEASED)
        return HandleUserControlReleased(result);
    else if (opcode == Constants::MESSAGE_SET_STREAM_PATH)
        return HandleSetStreamPath(message, result);
    else if (opcode == Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS)
        return HandleGiveDevicePowerStatus(message, result);
    else if (opcode == Constants::MESSAGE_MENU_REQUEST)
        return HandleMenuRequest(message, result);
    else if (opcode == Constants::MESSAGE_MENU_STATUS)
        return HandleMenuStatus(message, result);
    else if (opcode == Constants::MESSAGE_VENDOR_COMMAND)
        return HandleVendorCommand(message, result);
    else if (opcode == Constants::MESSAGE_VENDOR_COMMAND_WITH_ID)
        return HandleVendorCommandWithId(message, result);
    else if (opcode == Constants::MESSAGE_SET_OSD_NAME)
        return HandleSetOsdName(message, result);
    else if (opcode == Constants::MESSAGE_RECORD_TV_SCREEN)
        return HandleRecordTvScreen(message, result);
    else if (opcode == Constants::MESSAGE_TIMER_CLEARED_STATUS)
        return HandleTimerClearedStatus(message, result);
    else if (opcode == Constants::MESSAGE_REPORT_POWER_STATUS)
        return HandleReportPowerStatus(message, result);
    else if (opcode == Constants::MESSAGE_TIMER_STATUS)
        return HandleTimerStatus(message, result);
    else if (opcode == Constants::MESSAGE_RECORD_STATUS)
        return HandleRecordStatus(message, result);
    else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

Boolean HdmiCecLocalDevice::DispatchMessageToAction(
    /* [in] */ IHdmiCecMessage* message)
{
    AssertRunOnServiceThread();
    FOR_EACH(it, mActions) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<HdmiCecFeatureAction> action = (HdmiCecFeatureAction*) IObject::Probe(obj);
        Boolean isProcessCommandOk;
        action->ProcessCommand(message, &isProcessCommandOk);
        if (isProcessCommandOk) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode HdmiCecLocalDevice::HandleGivePhysicalAddress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Int32 physicalAddress;
    ((HdmiControlService*)mService.Get())->GetPhysicalAddress(&physicalAddress);
    AutoPtr<IHdmiCecMessage> cecMessage;
    HdmiCecMessageBuilder::BuildReportPhysicalAddressCommand(
            mAddress, physicalAddress, mDeviceType, (IHdmiCecMessage**)&cecMessage);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cecMessage);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleGiveDeviceVendorId(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Int32 vendorId;
    ((HdmiControlService*)mService.Get())->GetVendorId(&vendorId);
    AutoPtr<IHdmiCecMessage> cecMessage;
    HdmiCecMessageBuilder::BuildDeviceVendorIdCommand(
            mAddress, vendorId, (IHdmiCecMessage**)&cecMessage);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cecMessage);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleGetCecVersion(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Int32 version;
    ((HdmiControlService*)mService.Get())->GetCecVersion(&version);
    Int32 dest;
    message->GetDestination(&dest);
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    AutoPtr<IHdmiCecMessage> cecMessage;
    HdmiCecMessageBuilder::BuildCecVersion(dest,
            srcAddr, version, (IHdmiCecMessage**)&cecMessage);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cecMessage);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleInactiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleRequestActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleGetMenuLanguage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    Slogger::W(TAG, "Only TV can handle <Get Menu Language>:%s", Object::ToString(message).string());
    // 'return false' will cause to reply with <Feature Abort>.
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleGiveOsdName(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    // Note that since this method is called after logical address allocation is done,
    // mDeviceInfo should not be null.
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    String displayName;
    mDeviceInfo->GetDisplayName(&displayName);
    AutoPtr<IHdmiCecMessage> cecMessage;
    HdmiCecMessageBuilder::BuildSetOsdNameCommand(mAddress, srcAddr, displayName, (IHdmiCecMessage**)&cecMessage);
    if (cecMessage != NULL) {
        ((HdmiControlService*)mService.Get())->SendCecCommand(cecMessage);
    } else {
        Slogger::W(TAG, "Failed to build <Get Osd Name>:%s", displayName.string());
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleRoutingChange(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleRoutingInformation(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleReportPhysicalAddress(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleSystemAudioModeStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleSetSystemAudioMode(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleTerminateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleInitiateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleStandby(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    // Seq #12
    Boolean isControlEnabled;
    ((HdmiControlService*)mService.Get())->IsControlEnabled(&isControlEnabled);
    Boolean isProhibitMode;
    ((HdmiControlService*)mService.Get())->IsProhibitMode(&isProhibitMode);
    Boolean isPowerOnOrTransient;
    ((HdmiControlService*)mService.Get())->IsPowerOnOrTransient(&isPowerOnOrTransient);
    if (isControlEnabled && !isProhibitMode && isPowerOnOrTransient) {
        ((HdmiControlService*)mService.Get())->Standby();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleUserControlPressed(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    mHandler->RemoveMessages(MSG_USER_CONTROL_RELEASE_TIMEOUT);
    Boolean isPowerStandbyOrTransient;
    ((HdmiControlService*)mService.Get())->IsPowerStandbyOrTransient(&isPowerStandbyOrTransient);
    Boolean isPowerOnOrTransient;
    ((HdmiControlService*)mService.Get())->IsPowerOnOrTransient(&isPowerOnOrTransient);
    Boolean isPowerOffOrToggleCommand;
    IsPowerOffOrToggleCommand(message, &isPowerOffOrToggleCommand);
    if (isPowerOnOrTransient && isPowerOffOrToggleCommand) {
        ((HdmiControlService*)mService.Get())->Standby();
        *result = TRUE;
        return NOERROR;
    } else if (isPowerStandbyOrTransient && isPowerOffOrToggleCommand) {
        ((HdmiControlService*)mService.Get())->WakeUp();
        *result = TRUE;
        return NOERROR;
    }
    const Int64 downTime = SystemClock::GetUptimeMillis();
    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    // Note that we don't support parameterized keycode now.
    // TODO: translate parameterized keycode as well.
    Int32 keycode;
    HdmiCecKeycode::CecKeyToElastosKey((*params)[0], &keycode);
    Int32 keyRepeatCount = 0;
    if (mLastKeycode != HdmiCecKeycode::UNSUPPORTED_KEYCODE) {
        if (keycode == mLastKeycode) {
            keyRepeatCount = mLastKeyRepeatCount + 1;
        } else {
            InjectKeyEvent(downTime, IKeyEvent::ACTION_UP, mLastKeycode, 0);
        }
    }
    mLastKeycode = keycode;
    mLastKeyRepeatCount = keyRepeatCount;
    if (keycode != HdmiCecKeycode::UNSUPPORTED_KEYCODE) {
        InjectKeyEvent(downTime, IKeyEvent::ACTION_DOWN, keycode, keyRepeatCount);
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> msg;
        helper->Obtain(mHandler, MSG_USER_CONTROL_RELEASE_TIMEOUT, (IMessage**)&msg);
        Boolean bNoUse;
        mHandler->SendMessageDelayed(msg, FOLLOWER_SAFETY_TIMEOUT, &bNoUse);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleUserControlReleased(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    mHandler->RemoveMessages(MSG_USER_CONTROL_RELEASE_TIMEOUT);
    mLastKeyRepeatCount = 0;
    if (mLastKeycode != HdmiCecKeycode::UNSUPPORTED_KEYCODE) {
        const Int64 upTime = SystemClock::GetUptimeMillis();
        InjectKeyEvent(upTime, IKeyEvent::ACTION_UP, mLastKeycode, 0);
        mLastKeycode = HdmiCecKeycode::UNSUPPORTED_KEYCODE;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::InjectKeyEvent(
    /* [in] */ Int64 time,
    /* [in] */ Int32 action,
    /* [in] */ Int32 keycode,
    /* [in] */ Int32 repeat)
{
    AutoPtr<IKeyEventHelper> helper;
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
    AutoPtr<IKeyEvent> keyEvent;
    helper->Obtain(time, time, action, keycode,
            repeat, 0, IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FROM_SYSTEM,
            IInputDevice::SOURCE_HDMI, String(NULL), (IKeyEvent**)&keyEvent);
    AutoPtr<IInputManagerHelper> inputManagerHelper;
    CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&inputManagerHelper);
    AutoPtr<IInputManager> inputManager;
    inputManagerHelper->GetInstance((IInputManager**)&inputManager);
    Boolean bNoUse;
    inputManager->InjectInputEvent(IInputEvent::Probe(keyEvent),
            IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, &bNoUse);
    IInputEvent::Probe(keyEvent)->Recycle();
    return NOERROR;
}

ECode HdmiCecLocalDevice::IsPowerOnOrToggleCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 opcode;
    message->GetOpcode(&opcode);
    *result = opcode == Constants::MESSAGE_USER_CONTROL_PRESSED
            && ((*params)[0] == HdmiCecKeycode::CEC_KEYCODE_POWER
                    || (*params)[0] == HdmiCecKeycode::CEC_KEYCODE_POWER_ON_FUNCTION
                    || (*params)[0] == HdmiCecKeycode::CEC_KEYCODE_POWER_TOGGLE_FUNCTION);
    return NOERROR;
}

ECode HdmiCecLocalDevice::IsPowerOffOrToggleCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 opcode;
    message->GetOpcode(&opcode);
    *result = opcode == Constants::MESSAGE_USER_CONTROL_PRESSED
            && ((*params)[0] == HdmiCecKeycode::CEC_KEYCODE_POWER
                    || (*params)[0] == HdmiCecKeycode::CEC_KEYCODE_POWER_OFF_FUNCTION
                    || (*params)[0] == HdmiCecKeycode::CEC_KEYCODE_POWER_TOGGLE_FUNCTION);
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleTextViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleImageViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleSetStreamPath(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleGiveDevicePowerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 srcAddr;
    message->GetSource(&srcAddr);
    Int32 powerStatus;
    ((HdmiControlService*)mService.Get())->GetPowerStatus(&powerStatus);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildReportPowerStatus(mAddress, srcAddr, powerStatus, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleMenuRequest(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Always report menu active to receive Remote Control.
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildReportMenuStatus(mAddress, srcAddr, Constants::MENU_STATE_ACTIVATED, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*) mService.Get())->SendCecCommand(cmd);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleMenuStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleVendorCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    Boolean isInvokeVendorCommandListenersOk;
    ((HdmiControlService*)mService.Get())->InvokeVendorCommandListeners(mDeviceType, srcAddr, params, FALSE, &isInvokeVendorCommandListenersOk);
    if (!isInvokeVendorCommandListenersOk) {
        // Vendor command listener may not have been registered yet. Respond with
        // <Feature Abort> [NOT_IN_CORRECT_MODE] so that the sender can try again later.
        ((HdmiControlService*)mService.Get())->MaySendFeatureAbortCommand(message, Constants::ABORT_NOT_IN_CORRECT_MODE);
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleVendorCommandWithId(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > params;
    message->GetParams((ArrayOf<Byte>**)&params);
    Int32 vendorId = HdmiUtils::ThreeBytesToInt32(params);
    Int32 dest;
    message->GetDestination(&dest);
    Int32 srcAddr;
    message->GetSource(&srcAddr);
    Int32 serviceVendorId;
    ((HdmiControlService*)mService.Get())->GetVendorId(&serviceVendorId);
    if (vendorId == serviceVendorId) {
        Boolean isInvokeVendorCommandListenersOk;
        ((HdmiControlService*)mService.Get())->InvokeVendorCommandListeners(mDeviceType, srcAddr, params, TRUE, &isInvokeVendorCommandListenersOk);
        if (!isInvokeVendorCommandListenersOk) {
            ((HdmiControlService*)mService.Get())->MaySendFeatureAbortCommand(message, Constants::ABORT_NOT_IN_CORRECT_MODE);
        }
    } else if (dest != Constants::ADDR_BROADCAST &&
            srcAddr != Constants::ADDR_UNREGISTERED) {
        Slogger::V(TAG, "Wrong direct vendor command. Replying with <Feature Abort>");
        ((HdmiControlService*)mService.Get())->MaySendFeatureAbortCommand(message, Constants::ABORT_UNRECOGNIZED_OPCODE);
    } else {
        Slogger::V(TAG, "Wrong broadcast vendor command. Ignoring");
    }
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::SendStandby(
    /* [in] */ Int32 deviceId)
{
    // Do nothing.
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleSetOsdName(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // The default behavior of <Set Osd Name> is doing nothing.
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleRecordTvScreen(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // The default behavior of <Record TV Screen> is replying <Feature Abort> with
    // "Cannot provide source".
    ((HdmiControlService*)mService.Get())->MaySendFeatureAbortCommand(message, Constants::ABORT_CANNOT_PROVIDE_SOURCE);
    *result = TRUE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleTimerClearedStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleReportPowerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleTimerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleRecordStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleAddressAllocated(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 reason)
{
    AssertRunOnServiceThread();
    mAddress = mPreferredAddress = logicalAddress;
    OnAddressAllocated(logicalAddress, reason);
    SetPreferredAddress(logicalAddress);
    return NOERROR;
}

ECode HdmiCecLocalDevice::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return mDeviceType;
}

ECode HdmiCecLocalDevice::GetDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    *result = mDeviceInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDevice::SetDeviceInfo(
    /* [in] */ IHdmiDeviceInfo* info)
{
    AssertRunOnServiceThread();
    mDeviceInfo = info;
    return NOERROR;
}

ECode HdmiCecLocalDevice::IsAddressOf(
    /* [in] */ Int32 addr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    *result = addr == mAddress;
    return NOERROR;
}

ECode HdmiCecLocalDevice::ClearAddress()
{
    AssertRunOnServiceThread();
    mAddress = Constants::ADDR_UNREGISTERED;
    return NOERROR;
}

ECode HdmiCecLocalDevice::AddAndStartAction(
    /* [in] */ HdmiCecFeatureAction* action)
{
    AssertRunOnServiceThread();
    mActions->Add(TO_IINTERFACE(action));
    Boolean isPowerStandbyOrTransient;
    ((HdmiControlService*)mService.Get())->IsPowerStandbyOrTransient(&isPowerStandbyOrTransient);
    if (isPowerStandbyOrTransient) {
        Slogger::I(TAG, "Not ready to start action. Queued for deferred start:%s", Object::ToString(TO_IINTERFACE(action)).string());
        return NOERROR;
    }
    Boolean bNoUse;
    action->Start(&bNoUse);
    return NOERROR;
}

ECode HdmiCecLocalDevice::StartQueuedActions()
{
    AssertRunOnServiceThread();
    FOR_EACH(it, mActions) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<HdmiCecFeatureAction> action = (HdmiCecFeatureAction*) IObject::Probe(obj);
        Boolean isStartedOk;
        action->Started(&isStartedOk);
        if (!isStartedOk) {
            Slogger::I(TAG, "Starting queued action:%s", Object::ToString(TO_IINTERFACE(action)).string());
            Boolean bNoUse;
            action->Start(&bNoUse);
        }
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::HasAction(
    /* [in] */ ClassID clazz,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    FOR_EACH(it, mActions) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<HdmiCecFeatureAction> action = (HdmiCecFeatureAction*) IObject::Probe(obj);
        ClassID clsid;
        IObject::Probe(TO_IINTERFACE(action))->GetClassID(&clsid);
        if (clsid == clazz) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiCecLocalDevice::GetActions(
    /* [in] */ ClassID clazz,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    AutoPtr<IList> actions;
    helper->GetEmptyList((IList**)&actions);
    FOR_EACH(it, mActions) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<HdmiCecFeatureAction> action = (HdmiCecFeatureAction*) IObject::Probe(obj);
        ClassID clsid;
        IObject::Probe(TO_IINTERFACE(action))->GetClassID(&clsid);
        if (clsid == clazz) {
            Boolean isEmpty;
            actions->IsEmpty(&isEmpty);
            if (isEmpty) {
                CArrayList::New((IList**)&actions);
            }
            actions->Add(TO_IINTERFACE(action));
        }
    }
    *result = actions;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDevice::RemoveAction(
    /* [in] */ HdmiCecFeatureAction* action)
{
    AssertRunOnServiceThread();
    action->Finish(FALSE);
    mActions->Remove(TO_IINTERFACE(action));
    CheckIfPendingActionsCleared();
    return NOERROR;
}

ECode HdmiCecLocalDevice::RemoveAction(
    /* [in] */ ClassID clazz)
{
    AssertRunOnServiceThread();
    RemoveActionExcept(clazz, NULL);
    return NOERROR;
}

ECode HdmiCecLocalDevice::RemoveActionExcept(
    /* [in] */ ClassID clazz,
    /* [in] */ HdmiCecFeatureAction* exception)
{
    AssertRunOnServiceThread();
    AutoPtr<IIterator> iter;
    mActions->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<HdmiCecFeatureAction> action = (HdmiCecFeatureAction*) IObject::Probe(obj);
        ClassID clsid;
        IObject::Probe(TO_IINTERFACE(action))->GetClassID(&clsid);
        if (action.Get() != exception && clsid == clazz) {
            action->Finish(FALSE);
            iter->Remove();
        }
    }
    CheckIfPendingActionsCleared();
    return NOERROR;
}

ECode HdmiCecLocalDevice::CheckIfPendingActionsCleared()
{
    Boolean isEmpty;
    mActions->IsEmpty(&isEmpty);
    if (isEmpty && mPendingActionClearedCallback != NULL) {
        AutoPtr<IHdmiCecLocalDevicePendingActionClearedCallback> callback = mPendingActionClearedCallback;
        // To prevent from calling the callback again during handling the callback itself.
        mPendingActionClearedCallback = NULL;
        callback->OnCleared(this);
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::AssertRunOnServiceThread()
{
    AutoPtr<ILooper> serviceLooper;
    ((HdmiControlService*)mService.Get())->GetServiceLooper((ILooper**)&serviceLooper);
    if (Looper::GetMyLooper() != serviceLooper) {
        Logger::E(TAG, "Should run on service thread.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    return NOERROR;
}

ECode HdmiCecLocalDevice::GetService(
    /* [out] */ IHdmiControlService** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mService;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDevice::IsConnectedToArcPort(
    /* [in] */ Int32 path,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    return ((HdmiControlService*)mService.Get())->IsConnectedToArcPort(path, result);
}

ECode HdmiCecLocalDevice::GetActiveSource(
    /* [out] */ IHdmiCecLocalDeviceActiveSource** result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mLock);
        *result = mActiveSource;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::SetActiveSource(
    /* [in] */ IHdmiCecLocalDeviceActiveSource* newActive)
{
    return SetActiveSource(((ActiveSource*)newActive)->mLogicalAddress, ((ActiveSource*)newActive)->mPhysicalAddress);
}

ECode HdmiCecLocalDevice::SetActiveSource(
    /* [in] */ IHdmiDeviceInfo* info)
{
    Int32 logicalAddr;
    info->GetLogicalAddress(&logicalAddr);
    Int32 physicalAddr;
    info->GetPhysicalAddress(&physicalAddr);
    SetActiveSource(logicalAddr, physicalAddr);
    return NOERROR;
}

ECode HdmiCecLocalDevice::SetActiveSource(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress)
{
    {    AutoLock syncLock(mLock);
        ((ActiveSource*) mActiveSource.Get())->mLogicalAddress = logicalAddress;
        ((ActiveSource*) mActiveSource.Get())->mPhysicalAddress = physicalAddress;
    }
    ((HdmiControlService*)mService.Get())->SetLastInputForMhl(Constants::INVALID_PORT_ID);
    return NOERROR;
}

ECode HdmiCecLocalDevice::GetActivePath(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mLock);
        *result = mActiveRoutingPath;
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::SetActivePath(
    /* [in] */ Int32 path)
{
    {    AutoLock syncLock(mLock);
        mActiveRoutingPath = path;
    }
    Int32 portId;
    PathToPortId(path, &portId);
    ((HdmiControlService*)mService.Get())->SetActivePortId(portId);
    return NOERROR;
}

ECode HdmiCecLocalDevice::GetActivePortId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mLock);
        return ((HdmiControlService*)mService.Get())->PathToPortId(mActiveRoutingPath, result);
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::SetActivePortId(
    /* [in] */ Int32 portId)
{
    // We update active routing path instead, since we get the active port id from
    // the active routing path.
    Int32 path;
    ((HdmiControlService*)mService.Get())->PortIdToPath(portId, &path);
    SetActivePath(path);
    return NOERROR;
}

ECode HdmiCecLocalDevice::GetCecMessageCache(
    /* [out] */ HdmiCecMessageCache** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    *result = mCecMessageCache;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecLocalDevice::PathToPortId(
    /* [in] */ Int32 newPath,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AssertRunOnServiceThread();
    return ((HdmiControlService*)mService.Get())->PathToPortId(newPath, result);
}

ECode HdmiCecLocalDevice::OnStandby(
    /* [in] */ Boolean initiatedByCec)
{
    return NOERROR;
}

ECode HdmiCecLocalDevice::DisableDevice(
    /* [in] */ Boolean initiatedByCec,
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* origialCallback)
{
    mPendingActionClearedCallback = new InnerSub_PendingActionClearedCallback(this, origialCallback);
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain(mHandler, MSG_DISABLE_DEVICE_TIMEOUT, (IMessage**)&msg);
    Boolean bNoUse;
    mHandler->SendMessageDelayed(msg, DEVICE_CLEANUP_TIMEOUT, &bNoUse);
    return NOERROR;
}

ECode HdmiCecLocalDevice::HandleDisableDeviceTimeout()
{
    AssertRunOnServiceThread();
    // If all actions are not cleared in DEVICE_CLEANUP_TIMEOUT, enforce to finish them.
    // onCleard will be called at the last action's finish method.
    AutoPtr<IIterator> iter;
    mActions->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<HdmiCecFeatureAction> action = (HdmiCecFeatureAction*) IObject::Probe(obj);
        action->Finish(FALSE);
        iter->Remove();
    }
    return NOERROR;
}

ECode HdmiCecLocalDevice::SendKeyEvent(
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    Slogger::W(TAG, "sendKeyEvent not implemented");
    return NOERROR;
}

ECode HdmiCecLocalDevice::SendUserControlPressedAndReleased(
    /* [in] */ Int32 targetAddress,
    /* [in] */ Int32 cecKeycode)
{
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildUserControlPressed(
            mAddress, targetAddress, cecKeycode, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    cmd = NULL;
    HdmiCecMessageBuilder::BuildUserControlReleased(
            mAddress, targetAddress, (IHdmiCecMessage**)&cmd);
    ((HdmiControlService*)mService.Get())->SendCecCommand(cmd);
    return NOERROR;
}

ECode HdmiCecLocalDevice::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    String s;
    s.AppendFormat("mDeviceType: %d", mDeviceType);
    IPrintWriter::Probe(pw)->Println(s);
    s = "mAddress: ";
    s.AppendFormat("%d", mAddress);
    IPrintWriter::Probe(pw)->Println(s);
    s = "mPreferredAddress: ";
    s.AppendFormat("%d", mPreferredAddress);
    IPrintWriter::Probe(pw)->Println(s);
    s = "mDeviceInfo: ";
    s.AppendFormat("%s", TO_CSTR(mDeviceInfo));
    IPrintWriter::Probe(pw)->Println(s);
    s = "mActiveSource: ";
    s.AppendFormat("%d", TO_CSTR(mActiveSource));
    IPrintWriter::Probe(pw)->Println(s);
    s = "mActiveRoutingPath: ";
    s.AppendFormat("0x%04x", mActiveRoutingPath);
    IPrintWriter::Probe(pw)->Println(s);
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
