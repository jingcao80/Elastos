
#include "elastos/droid/server/hdmi/NewDeviceAction.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String NewDeviceAction::TAG("NewDeviceAction");
const Int32 NewDeviceAction::STATE_WAITING_FOR_SET_OSD_NAME = 1;
const Int32 NewDeviceAction::STATE_WAITING_FOR_DEVICE_VENDOR_ID = 2;

NewDeviceAction::NewDeviceAction()
    : mDeviceLogicalAddress(0)
    , mDevicePhysicalAddress(0)
    , mVendorId(0)
{}

ECode NewDeviceAction::constructor(
    /* [in] */ IHdmiCecLocalDevice* source,
    /* [in] */ Int32 deviceLogicalAddress,
    /* [in] */ Int32 devicePhysicalAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Super(source);
        mDeviceLogicalAddress = deviceLogicalAddress;
        mDevicePhysicalAddress = devicePhysicalAddress;
        mVendorId = Constants::UNKNOWN_VENDOR_ID;
#endif
}

ECode NewDeviceAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mState = STATE_WAITING_FOR_SET_OSD_NAME;
        if (MayProcessCommandIfCached(mDeviceLogicalAddress, Constants::MESSAGE_SET_OSD_NAME)) {
            return TRUE;
        }

        SendCommand(HdmiCecMessageBuilder->BuildGiveOsdNameCommand(GetSourceAddress(),
                mDeviceLogicalAddress));
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
        return TRUE;
#endif
}

ECode NewDeviceAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // For the logical device in interest, we want two more pieces of information -
        // osd name and vendor id. They are requested in sequence. In case we don't
        // get the expected responses (either by timeout or by receiving <feature abort> command),
        // set them to a default osd name and unknown vendor id respectively.
        Int32 opcode = cmd->GetOpcode();
        Int32 src = cmd->GetSource();
        Byte[] params = cmd->GetParams();

        if (mDeviceLogicalAddress != src) {
            return FALSE;
        }

        if (mState == STATE_WAITING_FOR_SET_OSD_NAME) {
            if (opcode == Constants::MESSAGE_SET_OSD_NAME) {
                try {
                    mDisplayName = new String(params, "US-ASCII");
                } catch (UnsupportedEncodingException e) {
                    Slogger::E(TAG, "Failed to get OSD name: " + e->GetMessage());
                }
                RequestVendorId();
                return TRUE;
            } else if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
                Int32 requestOpcode = params[0] & 0xFF;
                if (requestOpcode == Constants::MESSAGE_GIVE_OSD_NAME) {
                    RequestVendorId();
                    return TRUE;
                }
            }
        } else if (mState == STATE_WAITING_FOR_DEVICE_VENDOR_ID) {
            if (opcode == Constants::MESSAGE_DEVICE_VENDOR_ID) {
                mVendorId = HdmiUtils->ThreeBytesToInt(params);
                AddDeviceInfo();
                Finish();
                return TRUE;
            } else if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
                Int32 requestOpcode = params[0] & 0xFF;
                if (requestOpcode == Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID) {
                    AddDeviceInfo();
                    Finish();
                    return TRUE;
                }
            }
        }
        return FALSE;
#endif
}

ECode NewDeviceAction::MayProcessCommandIfCached(
    /* [in] */ Int32 destAddress,
    /* [in] */ Int32 opcode,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiCecMessage message = GetCecMessageCache()->GetMessage(destAddress, opcode);
        if (message != NULL) {
            return ProcessCommand(message);
        }
        return FALSE;
#endif
}

ECode NewDeviceAction::RequestVendorId()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // At first, transit to waiting status for <Device Vendor Id>.
        mState = STATE_WAITING_FOR_DEVICE_VENDOR_ID;
        // If the message is already in cache, process it.
        if (MayProcessCommandIfCached(mDeviceLogicalAddress,
                Constants::MESSAGE_DEVICE_VENDOR_ID)) {
            return;
        }
        SendCommand(HdmiCecMessageBuilder->BuildGiveDeviceVendorIdCommand(GetSourceAddress(),
                mDeviceLogicalAddress));
        AddTimer(mState, HdmiConfig.TIMEOUT_MS);
#endif
}

ECode NewDeviceAction::AddDeviceInfo()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mDisplayName == NULL) {
            mDisplayName = HdmiUtils->GetDefaultDeviceName(mDeviceLogicalAddress);
        }
        HdmiDeviceInfo deviceInfo = new HdmiDeviceInfo(
                mDeviceLogicalAddress, mDevicePhysicalAddress,
                Tv()->GetPortId(mDevicePhysicalAddress),
                HdmiUtils->GetTypeFromAddress(mDeviceLogicalAddress),
                mVendorId, mDisplayName);
        Tv()->AddCecDevice(deviceInfo);

        if (HdmiUtils->GetTypeFromAddress(mDeviceLogicalAddress)
                == HdmiDeviceInfo.DEVICE_AUDIO_SYSTEM) {
            Tv()->OnNewAvrAdded(deviceInfo);
        }
#endif
}

ECode NewDeviceAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState == STATE_NONE || mState != state) {
            return;
        }
        if (state == STATE_WAITING_FOR_SET_OSD_NAME) {
            // Osd name request timed out. Try vendor id
            RequestVendorId();
        } else if (state == STATE_WAITING_FOR_DEVICE_VENDOR_ID) {
            // vendor id timed out. Go ahead creating the device info what we've got so far.
            AddDeviceInfo();
            Finish();
        }
#endif
}

ECode NewDeviceAction::IsActionOf(
    /* [in] */ HdmiCecLocalDevice::ActiveSource* activeSource,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return (mDeviceLogicalAddress == activeSource.logicalAddress)
                && (mDevicePhysicalAddress == activeSource.physicalAddress);
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
