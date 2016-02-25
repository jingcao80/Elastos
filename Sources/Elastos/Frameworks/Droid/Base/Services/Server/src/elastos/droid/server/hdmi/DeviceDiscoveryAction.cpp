
#include "elastos/droid/server/hdmi/DeviceDiscoveryAction.h"
#include "elastos/droid/server/hdmi/Constants.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Utility::CArrayList;

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
#if 0
    , mDeviceType(HdmiDeviceInfo::DEVICE_INACTIVE)
#endif
{}

ECode DeviceDiscoveryAction::DeviceInfo::ToHdmiDeviceInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return new HdmiDeviceInfo(mLogicalAddress, mPhysicalAddress, mPortId, mDeviceType,
                        mVendorId, mDisplayName);
#endif
}

//=============================================================================
// DeviceDiscoveryAction
//=============================================================================
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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(source);
        mCallback = Preconditions.checkNotNull(callback);

#endif
}

ECode DeviceDiscoveryAction::Start(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDevices.clear();
        mState = STATE_WAITING_FOR_DEVICE_POLLING;
        pollDevices(new DevicePollingCallback() {
            @Override
            public void onPollingFinished(List<Integer> ackedAddress) {
                if (ackedAddress.isEmpty()) {
                    Slogger::V(TAG, "No device is detected.");
                    wrapUpAndFinish();
                    return;
                }
                Slogger::V(TAG, "Device detected: " + ackedAddress);
                allocateDevices(ackedAddress);
                startPhysicalAddressStage();
            }
        }, Constants::POLL_ITERATION_REVERSE_ORDER
            | Constants::POLL_STRATEGY_REMOTES_DEVICES, HdmiConfig.DEVICE_POLLING_RETRY);
        return true;

#endif
}

ECode DeviceDiscoveryAction::AllocateDevices(
    /* [in] */ IList* addresses)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (Integer i : addresses) {
            DeviceInfo info = new DeviceInfo(i);
            mDevices.add(info);
        }

#endif
}

ECode DeviceDiscoveryAction::StartPhysicalAddressStage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::V(TAG, "Start [Physical Address Stage]:" + mDevices.size());
        mProcessedDeviceCount = 0;
        mState = STATE_WAITING_FOR_PHYSICAL_ADDRESS;
        checkAndProceedStage();

#endif
}

ECode DeviceDiscoveryAction::VerifyValidLogicalAddress(
    /* [in] */ Int32 address,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return address >= Constants::ADDR_TV && address < Constants::ADDR_UNREGISTERED;

#endif
}

ECode DeviceDiscoveryAction::QueryPhysicalAddress(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!verifyValidLogicalAddress(address)) {
            checkAndProceedStage();
            return;
        }
        mActionTimer.clearTimerMessage();
        // Check cache first and send request if not exist.
        if (mayProcessMessageIfCached(address, Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS)) {
            return;
        }
        sendCommand(HdmiCecMessageBuilder.buildGivePhysicalAddress(getSourceAddress(), address));
        addTimer(mState, HdmiConfig.TIMEOUT_MS);

#endif
}

ECode DeviceDiscoveryAction::StartOsdNameStage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::V(TAG, "Start [Osd Name Stage]:" + mDevices.size());
        mProcessedDeviceCount = 0;
        mState = STATE_WAITING_FOR_OSD_NAME;
        checkAndProceedStage();

#endif
}

ECode DeviceDiscoveryAction::QueryOsdName(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!verifyValidLogicalAddress(address)) {
            checkAndProceedStage();
            return;
        }
        mActionTimer.clearTimerMessage();
        if (mayProcessMessageIfCached(address, Constants::MESSAGE_SET_OSD_NAME)) {
            return;
        }
        sendCommand(HdmiCecMessageBuilder.buildGiveOsdNameCommand(getSourceAddress(), address));
        addTimer(mState, HdmiConfig.TIMEOUT_MS);

#endif
}

ECode DeviceDiscoveryAction::StartVendorIdStage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::V(TAG, "Start [Vendor Id Stage]:" + mDevices.size());
        mProcessedDeviceCount = 0;
        mState = STATE_WAITING_FOR_VENDOR_ID;
        checkAndProceedStage();

#endif
}

ECode DeviceDiscoveryAction::QueryVendorId(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!verifyValidLogicalAddress(address)) {
            checkAndProceedStage();
            return;
        }
        mActionTimer.clearTimerMessage();
        if (mayProcessMessageIfCached(address, Constants::MESSAGE_DEVICE_VENDOR_ID)) {
            return;
        }
        sendCommand(
                HdmiCecMessageBuilder.buildGiveDeviceVendorIdCommand(getSourceAddress(), address));
        addTimer(mState, HdmiConfig.TIMEOUT_MS);

#endif
}

ECode DeviceDiscoveryAction::MayProcessMessageIfCached(
    /* [in] */ Int32 address,
    /* [in] */ Int32 opcode,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiCecMessage message = getCecMessageCache().getMessage(address, opcode);
        if (message != NULL) {
            processCommand(message);
            return true;
        }
        return FALSE;

#endif
}

ECode DeviceDiscoveryAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        switch (mState) {
            case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                if (cmd.getOpcode() == Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS) {
                    handleReportPhysicalAddress(cmd);
                    return true;
                }
                return FALSE;
            case STATE_WAITING_FOR_OSD_NAME:
                if (cmd.getOpcode() == Constants::MESSAGE_SET_OSD_NAME) {
                    handleSetOsdName(cmd);
                    return true;
                }
                return FALSE;
            case STATE_WAITING_FOR_VENDOR_ID:
                if (cmd.getOpcode() == Constants::MESSAGE_DEVICE_VENDOR_ID) {
                    handleVendorId(cmd);
                    return true;
                }
                return FALSE;
            case STATE_WAITING_FOR_DEVICE_POLLING:
                // Fall through.
            default:
                return FALSE;
        }

#endif
}

ECode DeviceDiscoveryAction::HandleReportPhysicalAddress(
    /* [in] */ IHdmiCecMessage* cmd)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Preconditions.checkState(mProcessedDeviceCount < mDevices.size());
        DeviceInfo current = mDevices.get(mProcessedDeviceCount);
        if (current.mLogicalAddress != cmd.getSource()) {
            Slogger::W(TAG, "Unmatched address[expected:" + current.mLogicalAddress + ", actual:" +
                    cmd.getSource());
            return;
        }
        byte params[] = cmd.getParams();
        current.mPhysicalAddress = HdmiUtils.twoBytesToInt(params);
        current.mPortId = getPortId(current.mPhysicalAddress);
        current.mDeviceType = params[2] & 0xFF;
        tv().updateCecSwitchInfo(current.mLogicalAddress, current.mDeviceType,
                    current.mPhysicalAddress);
        increaseProcessedDeviceCount();
        checkAndProceedStage();

#endif
}

ECode DeviceDiscoveryAction::GetPortId(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return tv().getPortId(physicalAddress);

#endif
}

ECode DeviceDiscoveryAction::HandleSetOsdName(
    /* [in] */ IHdmiCecMessage* cmd)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Preconditions.checkState(mProcessedDeviceCount < mDevices.size());
        DeviceInfo current = mDevices.get(mProcessedDeviceCount);
        if (current.mLogicalAddress != cmd.getSource()) {
            Slogger::W(TAG, "Unmatched address[expected:" + current.mLogicalAddress + ", actual:" +
                    cmd.getSource());
            return;
        }
        String displayName = NULL;
        try {
            displayName = new String(cmd.getParams(), "US-ASCII");
        } catch (UnsupportedEncodingException e) {
            Slogger::W(TAG, "Failed to decode display name: " + cmd.toString());
            // If failed to get display name, use the default name of device.
            displayName = HdmiUtils.getDefaultDeviceName(current.mLogicalAddress);
        }
        current.mDisplayName = displayName;
        increaseProcessedDeviceCount();
        checkAndProceedStage();

#endif
}

ECode DeviceDiscoveryAction::HandleVendorId(
    /* [in] */ IHdmiCecMessage* cmd)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Preconditions.checkState(mProcessedDeviceCount < mDevices.size());
        DeviceInfo current = mDevices.get(mProcessedDeviceCount);
        if (current.mLogicalAddress != cmd.getSource()) {
            Slogger::W(TAG, "Unmatched address[expected:" + current.mLogicalAddress + ", actual:" +
                    cmd.getSource());
            return;
        }
        byte[] params = cmd.getParams();
        int vendorId = HdmiUtils.threeBytesToInt(params);
        current.mVendorId = vendorId;
        increaseProcessedDeviceCount();
        checkAndProceedStage();

#endif
}

ECode DeviceDiscoveryAction::IncreaseProcessedDeviceCount()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mProcessedDeviceCount++;

#endif
}

ECode DeviceDiscoveryAction::RemoveDevice(
    /* [in] */ Int32 index)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDevices.remove(index);

#endif
}

ECode DeviceDiscoveryAction::WrapUpAndFinish()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::V(TAG, "---------Wrap up Device Discovery:[" + mDevices.size() + "]---------");
        ArrayList<HdmiDeviceInfo> result = new ArrayList<>();
        for (DeviceInfo info : mDevices) {
            HdmiDeviceInfo cecDeviceInfo = info.toHdmiDeviceInfo();
            Slogger::V(TAG, " DeviceInfo: " + cecDeviceInfo);
            result.add(cecDeviceInfo);
        }
        Slogger::V(TAG, "--------------------------------------------");
        mCallback.onDeviceDiscoveryDone(result);
        finish();

#endif
}

ECode DeviceDiscoveryAction::CheckAndProceedStage()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mDevices.isEmpty()) {
            wrapUpAndFinish();
            return;
        }
        // If finished current stage, move on to next stage.
        if (mProcessedDeviceCount == mDevices.size()) {
            mProcessedDeviceCount = 0;
            switch (mState) {
                case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                    startOsdNameStage();
                    return;
                case STATE_WAITING_FOR_OSD_NAME:
                    startVendorIdStage();
                    return;
                case STATE_WAITING_FOR_VENDOR_ID:
                    wrapUpAndFinish();
                    return;
                default:
                    return;
            }
        } else {
            int address = mDevices.get(mProcessedDeviceCount).mLogicalAddress;
            switch (mState) {
                case STATE_WAITING_FOR_PHYSICAL_ADDRESS:
                    queryPhysicalAddress(address);
                    return;
                case STATE_WAITING_FOR_OSD_NAME:
                    queryOsdName(address);
                    return;
                case STATE_WAITING_FOR_VENDOR_ID:
                    queryVendorId(address);
                default:
                    return;
            }
        }

#endif
}

ECode DeviceDiscoveryAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState == STATE_NONE || mState != state) {
            return;
        }
        Slogger::V(TAG, "Timeout[State=" + mState + ", Processed=" + mProcessedDeviceCount);
        removeDevice(mProcessedDeviceCount);
        checkAndProceedStage();

#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
