
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageCache.h"
#include "elastos/droid/server/hdmi/HdmiConfig.h"
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/NewDeviceAction.h"
#include <elastos/utility/logging/Slogger.h>

// using Elastos::Droid::Hardware::Hdmi::CHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

CAR_INTERFACE_IMPL(NewDeviceAction, HdmiCecFeatureAction, INewDeviceAction)

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
    HdmiCecFeatureAction::constructor(source);
    mDeviceLogicalAddress = deviceLogicalAddress;
    mDevicePhysicalAddress = devicePhysicalAddress;
    mVendorId = Constants::UNKNOWN_VENDOR_ID;
    return NOERROR;
}

ECode NewDeviceAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mState = STATE_WAITING_FOR_SET_OSD_NAME;
    if (MayProcessCommandIfCached(mDeviceLogicalAddress, Constants::MESSAGE_SET_OSD_NAME)) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGiveOsdNameCommand(srcAddr, mDeviceLogicalAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    *result = TRUE;
    return NOERROR;
}

ECode NewDeviceAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // For the logical device in interest, we want two more pieces of information -
    // osd name and vendor id. They are requested in sequence. In case we don't
    // get the expected responses (either by timeout or by receiving <feature abort> command),
    // set them to a default osd name and unknown vendor id respectively.
    Int32 opcode;
    cmd->GetOpcode(&opcode);
    Int32 src;
    cmd->GetSource(&src);
    AutoPtr<ArrayOf<Byte> > params;
    cmd->GetParams((ArrayOf<Byte>**)&params);

    if (mDeviceLogicalAddress != src) {
        *result = FALSE;
        return NOERROR;
    }

    if (mState == STATE_WAITING_FOR_SET_OSD_NAME) {
        if (opcode == Constants::MESSAGE_SET_OSD_NAME) {
            // try {
            mDisplayName = String(*params/* , "US-ASCII" */);
            // } catch (UnsupportedEncodingException e) {
                // Slogger::E(TAG, "Failed to get OSD name: " + e->GetMessage());
            // }
            RequestVendorId();
            *result = TRUE;
            return NOERROR;
        } else if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
            Int32 requestOpcode = (*params)[0] & 0xFF;
            if (requestOpcode == Constants::MESSAGE_GIVE_OSD_NAME) {
                RequestVendorId();
                *result = TRUE;
                return NOERROR;
            }
        }
    } else if (mState == STATE_WAITING_FOR_DEVICE_VENDOR_ID) {
        if (opcode == Constants::MESSAGE_DEVICE_VENDOR_ID) {
            mVendorId = HdmiUtils::ThreeBytesToInt32(params);
            AddDeviceInfo();
            Finish();
            *result = TRUE;
            return NOERROR;
        } else if (opcode == Constants::MESSAGE_FEATURE_ABORT) {
            Int32 requestOpcode = (*params)[0] & 0xFF;
            if (requestOpcode == Constants::MESSAGE_GIVE_DEVICE_VENDOR_ID) {
                AddDeviceInfo();
                Finish();
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

Boolean NewDeviceAction::MayProcessCommandIfCached(
    /* [in] */ Int32 destAddress,
    /* [in] */ Int32 opcode)
{
    AutoPtr<HdmiCecMessageCache> cache;
    GetCecMessageCache((HdmiCecMessageCache**)&cache);
    AutoPtr<IHdmiCecMessage> message;
    cache->GetMessage(destAddress, opcode, (IHdmiCecMessage**)&message);
    if (message != NULL) {
        Boolean rev;
        ProcessCommand(message, &rev);
        return rev;
    }
    return FALSE;
}

ECode NewDeviceAction::RequestVendorId()
{
    // At first, transit to waiting status for <Device Vendor Id>.
    mState = STATE_WAITING_FOR_DEVICE_VENDOR_ID;
    // If the message is already in cache, process it.
    if (MayProcessCommandIfCached(mDeviceLogicalAddress,
            Constants::MESSAGE_DEVICE_VENDOR_ID)) {
        return NOERROR;
    }
    Int32 srcAddr;
    GetSourceAddress(&srcAddr);
    AutoPtr<IHdmiCecMessage> cmd;
    HdmiCecMessageBuilder::BuildGiveDeviceVendorIdCommand(srcAddr,
            mDeviceLogicalAddress, (IHdmiCecMessage**)&cmd);
    SendCommand(cmd);
    AddTimer(mState, HdmiConfig::TIMEOUT_MS);
    return NOERROR;
}

ECode NewDeviceAction::AddDeviceInfo()
{
    if (mDisplayName == NULL) {
        mDisplayName = HdmiUtils::GetDefaultDeviceName(mDeviceLogicalAddress);
    }
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    Int32 portId;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetPortId(mDevicePhysicalAddress, &portId);
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    // TODO: Waiting for CHdmiDeviceInfo
    assert(0);
    // CHdmiDeviceInfo::New(
    //         mDeviceLogicalAddress, mDevicePhysicalAddress,
    //         portId,
    //         HdmiUtils::GetTypeFromAddress(mDeviceLogicalAddress),
    //         mVendorId, mDisplayName, (IHdmiDeviceInfo**)&deviceInfo);
    ((HdmiCecLocalDeviceTv*) tv.Get())->AddCecDevice(deviceInfo);

    if (HdmiUtils::GetTypeFromAddress(mDeviceLogicalAddress)
            == IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM) {
        ((HdmiCecLocalDeviceTv*) tv.Get())->OnNewAvrAdded(deviceInfo);
    }
    return NOERROR;
}

ECode NewDeviceAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    if (mState == STATE_NONE || mState != state) {
        return NOERROR;
    }
    if (state == STATE_WAITING_FOR_SET_OSD_NAME) {
        // Osd name request timed out. Try vendor id
        RequestVendorId();
    } else if (state == STATE_WAITING_FOR_DEVICE_VENDOR_ID) {
        // vendor id timed out. Go ahead creating the device info what we've got so far.
        AddDeviceInfo();
        Finish();
    }
    return NOERROR;
}

ECode NewDeviceAction::IsActionOf(
    /* [in] */ IHdmiCecLocalDeviceActiveSource* activeSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = (mDeviceLogicalAddress == ((HdmiCecLocalDevice::ActiveSource*) activeSource)->mLogicalAddress)
            && (mDevicePhysicalAddress == ((HdmiCecLocalDevice::ActiveSource*) activeSource)->mPhysicalAddress);
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
