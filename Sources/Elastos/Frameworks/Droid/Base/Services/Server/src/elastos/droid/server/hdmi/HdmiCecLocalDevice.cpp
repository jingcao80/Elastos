
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"
#include "elastos/droid/server/hdmi/HdmiCecKeycode.h"
#include <Elastos.Droid.Hardware.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Os::ILooper;
// using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {
//=============================================================================
// HdmiCecLocalDevice::ActiveSource
//=============================================================================
ECode HdmiCecLocalDevice::ActiveSource::constructor()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                invalidate();

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::constructor(
    /* [in] */ Int32 logical,
    /* [in] */ Int32 physical)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                logicalAddress = logical;
                physicalAddress = physical;

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::Of(
    /* [in] */ Int32 logical,
    /* [in] */ Int32 physical,
    /* [out] */ ActiveSource** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return new ActiveSource(logical, physical);

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::IsValid(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return HdmiUtils.isValidAddress(logicalAddress);

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::Invalidate()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                logicalAddress = Constants.ADDR_INVALID;
                physicalAddress = Constants.INVALID_PHYSICAL_ADDRESS;

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::Equals(
    /* [in] */ Int32 logical,
    /* [in] */ Int32 physical,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return logicalAddress == logical && physicalAddress == physical;

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::Equals(
    /* [in] */ IObject* obj,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                if (obj instanceof ActiveSource) {
                    ActiveSource that = (ActiveSource) obj;
                    return that.logicalAddress == logicalAddress &&
                           that.physicalAddress == physicalAddress;
                }
                return FALSE;

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::HashCode(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return logicalAddress * 29 + physicalAddress;

#endif
}

ECode HdmiCecLocalDevice::ActiveSource::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                StringBuffer s = new StringBuffer();
                String logicalAddressString = (logicalAddress == Constants.ADDR_INVALID)
                        ? "invalid" : String.format("0x%02x", logicalAddress);
                s.append("logical_address: ").append(logicalAddressString);
                String physicalAddressString = (physicalAddress == Constants.INVALID_PHYSICAL_ADDRESS)
                        ? "invalid" : String.format("0x%04x", physicalAddress);
                s.append(", physical_address: ").append(physicalAddressString);
                return s.toString();

#endif
}

//=============================================================================
// HdmiCecLocalDevice::InnerSub_Handler
//=============================================================================
ECode HdmiCecLocalDevice::InnerSub_Handler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            switch (msg.what) {
                case MSG_DISABLE_DEVICE_TIMEOUT:
                    handleDisableDeviceTimeout();
                    break;
                case MSG_USER_CONTROL_RELEASE_TIMEOUT:
                    handleUserControlReleased();
                    break;
            }

#endif
}

//=============================================================================
// HdmiCecLocalDevice
//=============================================================================
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
#if 0 // TODO: Translate codes below
    AutoPtr<ActiveSource> mActiveSource = new ActiveSource();
    mCecMessageCache = new HdmiCecMessageCache();
    LinkedList<HdmiCecFeatureAction> mActions = new LinkedList<>();
    mHandler = new InnerSub_Handler();
#endif
}

ECode HdmiCecLocalDevice::constructor(
    /* [in] */ HdmiControlService* service,
    /* [in] */ Int32 deviceType)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService = service;
        mDeviceType = deviceType;
        mAddress = Constants::ADDR_UNREGISTERED;
        mLock = service.getServiceLock();

#endif
}

ECode HdmiCecLocalDevice::Create(
    /* [in] */ HdmiControlService* service,
    /* [in] */ Int32 deviceType,
    /* [out] */ IHdmiCecLocalDevice** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (deviceType) {
        case HdmiDeviceInfo.DEVICE_TV:
            return new HdmiCecLocalDeviceTv(service);
        case HdmiDeviceInfo.DEVICE_PLAYBACK:
            return new HdmiCecLocalDevicePlayback(service);
        default:
            return NULL;
        }

#endif
}

ECode HdmiCecLocalDevice::Init()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mPreferredAddress = getPreferredAddress();

#endif
}

ECode HdmiCecLocalDevice::DispatchMessage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int dest = message.getDestination();
        if (dest != mAddress && dest != Constants::ADDR_BROADCAST) {
            return FALSE;
        }
        // Cache incoming message. Note that it caches only white-listed one.
        mCecMessageCache.cacheMessage(message);
        return onMessage(message);

#endif
}

ECode HdmiCecLocalDevice::OnMessage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        if (dispatchMessageToAction(message)) {
            return true;
        }
        switch (message.getOpcode()) {
            case Constants::MESSAGE_ACTIVE_SOURCE:
                return handleActiveSource(message);
            case Constants::MESSAGE_INACTIVE_SOURCE:
                return handleInactiveSource(message);
            case Constants::MESSAGE_REQUEST_ACTIVE_SOURCE:
                return handleRequestActiveSource(message);
            case Constants::MESSAGE_GET_MENU_LANGUAGE:
                return handleGetMenuLanguage(message);
            case Constants::MESSAGE_GIVE_PHYSICAL_ADDRESS:
                return handleGivePhysicalAddress();
            case Constants::MESSAGE_GIVE_OSD_NAME:
                return handleGiveOsdName(message);
            case Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID:
                return handleGiveDeviceVendorId();
            case Constants::MESSAGE_GET_CEC_VERSION:
                return handleGetCecVersion(message);
            case Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS:
                return handleReportPhysicalAddress(message);
            case Constants::MESSAGE_ROUTING_CHANGE:
                return handleRoutingChange(message);
            case Constants::MESSAGE_ROUTING_INFORMATION:
                return handleRoutingInformation(message);
            case Constants::MESSAGE_INITIATE_ARC:
                return handleInitiateArc(message);
            case Constants::MESSAGE_TERMINATE_ARC:
                return handleTerminateArc(message);
            case Constants::MESSAGE_SET_SYSTEM_AUDIO_MODE:
                return handleSetSystemAudioMode(message);
            case Constants::MESSAGE_SYSTEM_AUDIO_MODE_STATUS:
                return handleSystemAudioModeStatus(message);
            case Constants::MESSAGE_REPORT_AUDIO_STATUS:
                return handleReportAudioStatus(message);
            case Constants::MESSAGE_STANDBY:
                return handleStandby(message);
            case Constants::MESSAGE_TEXT_VIEW_ON:
                return handleTextViewOn(message);
            case Constants::MESSAGE_IMAGE_VIEW_ON:
                return handleImageViewOn(message);
            case Constants::MESSAGE_USER_CONTROL_PRESSED:
                return handleUserControlPressed(message);
            case Constants::MESSAGE_USER_CONTROL_RELEASED:
                return handleUserControlReleased();
            case Constants::MESSAGE_SET_STREAM_PATH:
                return handleSetStreamPath(message);
            case Constants::MESSAGE_GIVE_DEVICE_POWER_STATUS:
                return handleGiveDevicePowerStatus(message);
            case Constants::MESSAGE_MENU_REQUEST:
                return handleMenuRequest(message);
            case Constants::MESSAGE_MENU_STATUS:
                return handleMenuStatus(message);
            case Constants::MESSAGE_VENDOR_COMMAND:
                return handleVendorCommand(message);
            case Constants::MESSAGE_VENDOR_COMMAND_WITH_ID:
                return handleVendorCommandWithId(message);
            case Constants::MESSAGE_SET_OSD_NAME:
                return handleSetOsdName(message);
            case Constants::MESSAGE_RECORD_TV_SCREEN:
                return handleRecordTvScreen(message);
            case Constants::MESSAGE_TIMER_CLEARED_STATUS:
                return handleTimerClearedStatus(message);
            case Constants::MESSAGE_REPORT_POWER_STATUS:
                return handleReportPowerStatus(message);
            case Constants::MESSAGE_TIMER_STATUS:
                return handleTimerStatus(message);
            case Constants::MESSAGE_RECORD_STATUS:
                return handleRecordStatus(message);
            default:
                return FALSE;
        }

#endif
}

Boolean HdmiCecLocalDevice::DispatchMessageToAction(
    /* [in] */ IHdmiCecMessage* message)
{
    return FALSE;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiCecFeatureAction action : mActions) {
            if (action.processCommand(message)) {
                return true;
            }
        }
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleGivePhysicalAddress(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int physicalAddress = mService.getPhysicalAddress();
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder.buildReportPhysicalAddressCommand(
                mAddress, physicalAddress, mDeviceType);
        mService.sendCecCommand(cecMessage);
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleGiveDeviceVendorId(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int vendorId = mService.getVendorId();
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder.buildDeviceVendorIdCommand(
                mAddress, vendorId);
        mService.sendCecCommand(cecMessage);
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleGetCecVersion(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        int version = mService.getCecVersion();
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder.buildCecVersion(message.getDestination(),
                message.getSource(), version);
        mService.sendCecCommand(cecMessage);
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleInactiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleRequestActiveSource(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleGetMenuLanguage(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        Slogger::W(TAG, "Only TV can handle <Get Menu Language>:" + message.toString());
        // 'return false' will cause to reply with <Feature Abort>.
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleGiveOsdName(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Note that since this method is called after logical address allocation is done,
        // mDeviceInfo should not be null.
        HdmiCecMessage cecMessage = HdmiCecMessageBuilder.buildSetOsdNameCommand(
                mAddress, message.getSource(), mDeviceInfo.getDisplayName());
        if (cecMessage != NULL) {
            mService.sendCecCommand(cecMessage);
        } else {
            Slogger::W(TAG, "Failed to build <Get Osd Name>:" + mDeviceInfo.getDisplayName());
        }
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleRoutingChange(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleRoutingInformation(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleReportPhysicalAddress(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleSystemAudioModeStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleSetSystemAudioMode(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleTerminateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleInitiateArc(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleReportAudioStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleStandby(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // Seq #12
        if (mService.isControlEnabled() && !mService.isProhibitMode()
                && mService.isPowerOnOrTransient()) {
            mService.standby();
            return true;
        }
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleUserControlPressed(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mHandler.removeMessages(MSG_USER_CONTROL_RELEASE_TIMEOUT);
        if (mService.isPowerOnOrTransient() && isPowerOffOrToggleCommand(message)) {
            mService.standby();
            return true;
        } else if (mService.isPowerStandbyOrTransient() && isPowerOnOrToggleCommand(message)) {
            mService.wakeUp();
            return true;
        }
        final long downTime = SystemClock.uptimeMillis();
        final byte[] params = message.getParams();
        // Note that we don't support parameterized keycode now.
        // TODO: translate parameterized keycode as well.
        final int keycode = HdmiCecKeycode.cecKeyToAndroidKey(params[0]);
        int keyRepeatCount = 0;
        if (mLastKeycode != HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            if (keycode == mLastKeycode) {
                keyRepeatCount = mLastKeyRepeatCount + 1;
            } else {
                injectKeyEvent(downTime, KeyEvent.ACTION_UP, mLastKeycode, 0);
            }
        }
        mLastKeycode = keycode;
        mLastKeyRepeatCount = keyRepeatCount;
        if (keycode != HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            injectKeyEvent(downTime, KeyEvent.ACTION_DOWN, keycode, keyRepeatCount);
            mHandler.sendMessageDelayed(Message.obtain(mHandler, MSG_USER_CONTROL_RELEASE_TIMEOUT),
                    FOLLOWER_SAFETY_TIMEOUT);
            return true;
        }
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleUserControlReleased(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mHandler.removeMessages(MSG_USER_CONTROL_RELEASE_TIMEOUT);
        mLastKeyRepeatCount = 0;
        if (mLastKeycode != HdmiCecKeycode.UNSUPPORTED_KEYCODE) {
            final long upTime = SystemClock.uptimeMillis();
            injectKeyEvent(upTime, KeyEvent.ACTION_UP, mLastKeycode, 0);
            mLastKeycode = HdmiCecKeycode.UNSUPPORTED_KEYCODE;
            return true;
        }
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::InjectKeyEvent(
    /* [in] */ Int64 time,
    /* [in] */ Int32 action,
    /* [in] */ Int32 keycode,
    /* [in] */ Int32 repeat)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        KeyEvent keyEvent = KeyEvent.obtain(time, time, action, keycode,
                repeat, 0, KeyCharacterMap.VIRTUAL_KEYBOARD, 0, KeyEvent.FLAG_FROM_SYSTEM,
                InputDevice.SOURCE_HDMI, NULL);
        InputManager.getInstance().injectInputEvent(keyEvent,
                InputManager.INJECT_INPUT_EVENT_MODE_ASYNC);
        keyEvent.recycle();

#endif
}

ECode HdmiCecLocalDevice::IsPowerOnOrToggleCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = message.getParams();
        return message.getOpcode() == Constants::MESSAGE_USER_CONTROL_PRESSED
                && (params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_ON_FUNCTION
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_TOGGLE_FUNCTION);

#endif
}

ECode HdmiCecLocalDevice::IsPowerOffOrToggleCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = message.getParams();
        return message.getOpcode() == Constants::MESSAGE_USER_CONTROL_PRESSED
                && (params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_OFF_FUNCTION
                        || params[0] == HdmiCecKeycode.CEC_KEYCODE_POWER_TOGGLE_FUNCTION);

#endif
}

ECode HdmiCecLocalDevice::HandleTextViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleImageViewOn(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleSetStreamPath(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleGiveDevicePowerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.sendCecCommand(HdmiCecMessageBuilder.buildReportPowerStatus(
                mAddress, message.getSource(), mService.getPowerStatus()));
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleMenuRequest(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Always report menu active to receive Remote Control.
        mService.sendCecCommand(HdmiCecMessageBuilder.buildReportMenuStatus(
                mAddress, message.getSource(), Constants::MENU_STATE_ACTIVATED));
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleMenuStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleVendorCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!mService.invokeVendorCommandListeners(mDeviceType, message.getSource(),
                message.getParams(), FALSE)) {
            // Vendor command listener may not have been registered yet. Respond with
            // <Feature Abort> [NOT_IN_CORRECT_MODE] so that the sender can try again later.
            mService.maySendFeatureAbortCommand(message, Constants::ABORT_NOT_IN_CORRECT_MODE);
        }
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleVendorCommandWithId(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        byte[] params = message.getParams();
        int vendorId = HdmiUtils.threeBytesToInt(params);
        if (vendorId == mService.getVendorId()) {
            if (!mService.invokeVendorCommandListeners(mDeviceType, message.getSource(), params,
                    true)) {
                mService.maySendFeatureAbortCommand(message, Constants::ABORT_NOT_IN_CORRECT_MODE);
            }
        } else if (message.getDestination() != Constants::ADDR_BROADCAST &&
                message.getSource() != Constants::ADDR_UNREGISTERED) {
            Slogger::V(TAG, "Wrong direct vendor command. Replying with <Feature Abort>");
            mService.maySendFeatureAbortCommand(message, Constants::ABORT_UNRECOGNIZED_OPCODE);
        } else {
            Slogger::V(TAG, "Wrong broadcast vendor command. Ignoring");
        }
        return true;

#endif
}

ECode HdmiCecLocalDevice::SendStandby(
    /* [in] */ Int32 deviceId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Do nothing.

#endif
}

ECode HdmiCecLocalDevice::HandleSetOsdName(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // The default behavior of <Set Osd Name> is doing nothing.
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleRecordTvScreen(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // The default behavior of <Record TV Screen> is replying <Feature Abort> with
        // "Cannot provide source".
        mService.maySendFeatureAbortCommand(message, Constants::ABORT_CANNOT_PROVIDE_SOURCE);
        return true;

#endif
}

ECode HdmiCecLocalDevice::HandleTimerClearedStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleReportPowerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleTimerStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleRecordStatus(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::HandleAddressAllocated(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mAddress = mPreferredAddress = logicalAddress;
        onAddressAllocated(logicalAddress, reason);
        setPreferredAddress(logicalAddress);

#endif
}

ECode HdmiCecLocalDevice::GetType(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDeviceType;

#endif
}

ECode HdmiCecLocalDevice::GetDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mDeviceInfo;

#endif
}

ECode HdmiCecLocalDevice::SetDeviceInfo(
    /* [in] */ IHdmiDeviceInfo* info)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mDeviceInfo = info;

#endif
}

ECode HdmiCecLocalDevice::IsAddressOf(
    /* [in] */ Int32 addr,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return addr == mAddress;

#endif
}

ECode HdmiCecLocalDevice::ClearAddress()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mAddress = Constants::ADDR_UNREGISTERED;

#endif
}

ECode HdmiCecLocalDevice::AddAndStartAction(
    /* [in] */  )
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        mActions.add(action);
        if (mService.isPowerStandbyOrTransient()) {
            Slogger::I(TAG, "Not ready to start action. Queued for deferred start:" + action);
            return;
        }
        action.start();

#endif
}

ECode HdmiCecLocalDevice::StartQueuedActions()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiCecFeatureAction action : mActions) {
            if (!action.started()) {
                Slogger::I(TAG, "Starting queued action:" + action);
                action.start();
            }
        }

#endif
}

ECode HdmiCecLocalDevice::HasAction(
    /* [in] */ ClassID clazz,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        for (HdmiCecFeatureAction action : mActions) {
            if (action.getClass().equals(clazz)) {
                return true;
            }
        }
        return FALSE;

#endif
}

ECode HdmiCecLocalDevice::GetActions(
    /* [in] */ ClassID clazz,
    /* [out] */ IList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        List<T> actions = Collections.<T>emptyList();
        for (HdmiCecFeatureAction action : mActions) {
            if (action.getClass().equals(clazz)) {
                if (actions.isEmpty()) {
                    actions = new ArrayList<T>();
                }
                actions.add((T) action);
            }
        }
        return actions;

#endif
}

ECode HdmiCecLocalDevice::RemoveAction(
    /* [in] */ HdmiCecFeatureAction* action)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        action.finish(FALSE);
        mActions.remove(action);
        checkIfPendingActionsCleared();

#endif
}

ECode HdmiCecLocalDevice::RemoveAction(
    /* [in] */ ClassID clazz,
    /* [out] */ HdmiCecFeatureAction** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        removeActionExcept(clazz, NULL);

#endif
}

ECode HdmiCecLocalDevice::RemoveActionExcept(
    /* [in] */ ClassID clazz,
    /* [in] */ HdmiCecFeatureAction* exception,
    /* [out] */ HdmiCecFeatureAction** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        Iterator<HdmiCecFeatureAction> iter = mActions.iterator();
        while (iter.hasNext()) {
            HdmiCecFeatureAction action = iter.next();
            if (action != exception && action.getClass().equals(clazz)) {
                action.finish(FALSE);
                iter.remove();
            }
        }
        checkIfPendingActionsCleared();

#endif
}

ECode HdmiCecLocalDevice::CheckIfPendingActionsCleared()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mActions.isEmpty() && mPendingActionClearedCallback != NULL) {
            PendingActionClearedCallback callback = mPendingActionClearedCallback;
            // To prevent from calling the callback again during handling the callback itself.
            mPendingActionClearedCallback = NULL;
            callback.onCleared(this);
        }

#endif
}

ECode HdmiCecLocalDevice::AssertRunOnServiceThread()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (Looper.myLooper() != mService.getServiceLooper()) {
            throw new IllegalStateException("Should run on service thread.");
        }

#endif
}

ECode HdmiCecLocalDevice::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiCecLocalDevice::GetService(
    /* [out] */ HdmiControlService** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mService;

#endif
}

ECode HdmiCecLocalDevice::IsConnectedToArcPort(
    /* [in] */ Int32 path,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mService.isConnectedToArcPort(path);

#endif
}

ECode HdmiCecLocalDevice::GetActiveSource(
    /* [out] */ ActiveSource** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            return mActiveSource;
        }

#endif
}

ECode HdmiCecLocalDevice::SetActiveSource(
    /* [in] */ ActiveSource* newActive)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        setActiveSource(newActive.logicalAddress, newActive.physicalAddress);

#endif
}

ECode HdmiCecLocalDevice::SetActiveSource(
    /* [in] */ IHdmiDeviceInfo* info)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        setActiveSource(info.getLogicalAddress(), info.getPhysicalAddress());

#endif
}

ECode HdmiCecLocalDevice::SetActiveSource(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 physicalAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            mActiveSource.logicalAddress = logicalAddress;
            mActiveSource.physicalAddress = physicalAddress;
        }
        mService.setLastInputForMhl(Constants::INVALID_PORT_ID);

#endif
}

ECode HdmiCecLocalDevice::GetActivePath(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            return mActiveRoutingPath;
        }

#endif
}

ECode HdmiCecLocalDevice::SetActivePath(
    /* [in] */ Int32 path)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            mActiveRoutingPath = path;
        }
        mService.setActivePortId(pathToPortId(path));

#endif
}

ECode HdmiCecLocalDevice::GetActivePortId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mLock) {
            return mService.pathToPortId(mActiveRoutingPath);
        }

#endif
}

ECode HdmiCecLocalDevice::SetActivePortId(
    /* [in] */ Int32 portId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // We update active routing path instead, since we get the active port id from
        // the active routing path.
        setActivePath(mService.portIdToPath(portId));

#endif
}

ECode HdmiCecLocalDevice::GetCecMessageCache(
    /* [out] */ HdmiCecMessageCache** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mCecMessageCache;

#endif
}

ECode HdmiCecLocalDevice::PathToPortId(
    /* [in] */ Int32 newPath,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        return mService.pathToPortId(newPath);

#endif
}

ECode HdmiCecLocalDevice::OnStandby(
    /* [in] */ Boolean initiatedByCec)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
#endif
}

ECode HdmiCecLocalDevice::DisableDevice(
    /* [in] */ Boolean initiatedByCec,
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* origialCallback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPendingActionClearedCallback = new PendingActionClearedCallback() {
            @Override
            public void onCleared(HdmiCecLocalDevice device) {
                mHandler.removeMessages(MSG_DISABLE_DEVICE_TIMEOUT);
                origialCallback.onCleared(device);
            }
        };
        mHandler.sendMessageDelayed(Message.obtain(mHandler, MSG_DISABLE_DEVICE_TIMEOUT),
                DEVICE_CLEANUP_TIMEOUT);

#endif
}

ECode HdmiCecLocalDevice::HandleDisableDeviceTimeout()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        assertRunOnServiceThread();
        // If all actions are not cleared in DEVICE_CLEANUP_TIMEOUT, enforce to finish them.
        // onCleard will be called at the last action's finish method.
        Iterator<HdmiCecFeatureAction> iter = mActions.iterator();
        while (iter.hasNext()) {
            HdmiCecFeatureAction action = iter.next();
            action.finish(FALSE);
            iter.remove();
        }

#endif
}

ECode HdmiCecLocalDevice::SendKeyEvent(
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::W(TAG, "sendKeyEvent not implemented");

#endif
}

ECode HdmiCecLocalDevice::SendUserControlPressedAndReleased(
    /* [in] */ Int32 targetAddress,
    /* [in] */ Int32 cecKeycode)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService.sendCecCommand(HdmiCecMessageBuilder.buildUserControlPressed(
                mAddress, targetAddress, cecKeycode));
        mService.sendCecCommand(HdmiCecMessageBuilder.buildUserControlReleased(
                mAddress, targetAddress));

#endif
}

ECode HdmiCecLocalDevice::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        pw.println("mDeviceType: " + mDeviceType);
        pw.println("mAddress: " + mAddress);
        pw.println("mPreferredAddress: " + mPreferredAddress);
        pw.println("mDeviceInfo: " + mDeviceInfo);
        pw.println("mActiveSource: " + mActiveSource);
        pw.println(String.format("mActiveRoutingPath: 0x%04x", mActiveRoutingPath));

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
