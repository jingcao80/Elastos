
#include "elastos/droid/server/hdmi/HotplugDetectionAction.h"
#include "elastos/droid/server/hdmi/Constants.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceDevicePollingCallback;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super::constructor(source);

#endif
}

ECode HotplugDetectionAction::Start(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::V(TAG, "Hot-plug dection started.");

        mState = STATE_WAIT_FOR_NEXT_POLLING;
        mTimeoutCount = 0;

        // Start timer without polling.
        // The first check for all devices will be initiated 15 seconds later.
        AddTimer(mState, POLLING_INTERVAL_MS);
        *result = TRUE;
        return NOERROR;
#endif
}

ECode HotplugDetectionAction::ProcessCommand(
    /* [in] */ IHdmiCecMessage* cmd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // No-op
        *result = FALSE;
        return NOERROR;
#endif
}

ECode HotplugDetectionAction::HandleTimerEvent(
    /* [in] */ Int32 state)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mState != state) {
            return NOERROR;
        }

        if (mState == STATE_WAIT_FOR_NEXT_POLLING) {
            mTimeoutCount = (mTimeoutCount + 1) % TIMEOUT_COUNT;
            PollDevices();
        }
#endif
}

ECode HotplugDetectionAction::PollAllDevicesNow()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Clear existing timer to avoid overlapped execution
        mActionTimer->ClearTimerMessage();

        mTimeoutCount = 0;
        mState = STATE_WAIT_FOR_NEXT_POLLING;
        PollAllDevices();

        AddTimer(mState, POLLING_INTERVAL_MS);
#endif
}

ECode HotplugDetectionAction::PollDevices()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // All device check called every 15 seconds.
        if (mTimeoutCount == 0) {
            PollAllDevices();
        } else {
            AutoPtr<IHdmiCecLocalDeviceTv> tv;
            Tv((IHdmiCecLocalDeviceTv**)&tv);
            Boolean isSystemAudioActivated;
            tv->IsSystemAudioActivated(&isSystemAudioActivated);
            if (isSystemAudioActivated) {
                PollAudioSystem();
            }
        }

        AddTimer(mState, POLLING_INTERVAL_MS);
#endif
}

ECode HotplugDetectionAction::PollAllDevices()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::V(TAG, "Poll all devices.");

        PollDevices(new DevicePollingCallback() {
            //@Override
            CARAPI OnPollingFinished(List<Integer> ackedAddress) {
                CheckHotplug(ackedAddress, FALSE);
            }
        }, Constants::POLL_ITERATION_IN_ORDER
                | Constants::POLL_STRATEGY_REMOTES_DEVICES, HdmiConfig::HOTPLUG_DETECTION_RETRY);
#endif
}

ECode HotplugDetectionAction::PollAudioSystem()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Slogger::V(TAG, "Poll audio system.");

        PollDevices(new DevicePollingCallback() {
            //@Override
            CARAPI OnPollingFinished(List<Integer> ackedAddress) {
                CheckHotplug(ackedAddress, TRUE);
            }
        }, Constants::POLL_ITERATION_IN_ORDER
                | Constants::POLL_STRATEGY_SYSTEM_AUDIO, HdmiConfig::HOTPLUG_DETECTION_RETRY);
#endif
}

ECode HotplugDetectionAction::CheckHotplug(
    /* [in] */ IList* ackedAddress,
    /* [in] */ Boolean audioOnly)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        BitSet currentInfos = InfoListToBitSet(tv->GetDeviceInfoList(FALSE), audioOnly);
        BitSet polledResult = AddressListToBitSet(ackedAddress);

        // At first, check removed devices.
        BitSet removed = Complement(currentInfos, polledResult);
        Int32 index = -1;
        while ((index = removed->NextSetBit(index + 1)) != -1) {
            Slogger::V(TAG, "Remove device by hot-plug detection:" + index);
            RemoveDevice(index);
        }

        // Next, check added devices.
        BitSet added = Complement(polledResult, currentInfos);
        index = -1;
        while ((index = added->NextSetBit(index + 1)) != -1) {
            Slogger::V(TAG, "Add device by hot-plug detection:" + index);
            AddDevice(index);
        }
#endif
}

ECode HotplugDetectionAction::InfoListToBitSet(
    /* [in] */ IList* infoList,
    /* [in] */ Boolean audioOnly,
    /* [out] */ IBitSet** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        BitSet set = new BitSet(NUM_OF_ADDRESS);
        for (HdmiDeviceInfo info : infoList) {
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
#endif
}

ECode HotplugDetectionAction::AddressListToBitSet(
    /* [in] */ IList* list,
    /* [out] */ IBitSet** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        BitSet set = new BitSet(NUM_OF_ADDRESS);
        for (Integer value : list) {
            set->Set(value);
        }
        return set;
#endif
}

ECode HotplugDetectionAction::Complement(
    /* [in] */ IBitSet* first,
    /* [in] */ IBitSet* second,
    /* [out] */ IBitSet** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Need to clone it so that it doesn't touch original set.
        BitSet clone = (BitSet) first->Clone();
        clone->AndNot(second);
        return clone;
#endif
}

ECode HotplugDetectionAction::AddDevice(
    /* [in] */ Int32 addedAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // Sending <Give Physical Address> will initiate new device action.
        Int32 srcAddr;
        GetSourceAddress(&srcAddr);
        SendCommand(HdmiCecMessageBuilder->BuildGivePhysicalAddress(srcAddr,
                addedAddress));
#endif
}

ECode HotplugDetectionAction::RemoveDevice(
    /* [in] */ Int32 removedAddress)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        MayChangeRoutingPath(removedAddress);
        MayCancelDeviceSelect(removedAddress);
        MayCancelOneTouchRecord(removedAddress);
        MayDisableSystemAudioAndARC(removedAddress);

        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        tv->RemoveCecDevice(removedAddress);
#endif
}

ECode HotplugDetectionAction::MayChangeRoutingPath(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        AutoPtr<IHdmiDeviceInfo> info;
        tv->GetCecDeviceInfo(address, (IHdmiDeviceInfo**)&info);
        if (info != NULL) {
            Int32 physicalAddr;
            info->GetPhysicalAddress(&physicalAddr);
            tv->HandleRemoveActiveRoutingPath(physicalAddr);
        }
#endif
}

ECode HotplugDetectionAction::MayCancelDeviceSelect(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        List<DeviceSelectAction> actions = GetActions(ECLSID_CDeviceSelectAction);
        Boolean isEmpty;
        actions->IsEmpty(&isEmpty);
        if (isEmpty) {
            return NOERROR;
        }

        // Should have only one Device Select Action
        AutoPtr<IInterface> obj;
        actions->Get(0, (IInterface**)&obj);
        DeviceSelectAction action = I::Probe(obj);
        Int32 targetAddr;
        action->GetTargetAddress(&targetAddr);
        if (targetAddr == address) {
            RemoveAction(ECLSID_CDeviceSelectAction);
        }
#endif
}

ECode HotplugDetectionAction::MayCancelOneTouchRecord(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        List<OneTouchRecordAction> actions = GetActions(ECLSID_COneTouchRecordAction);
        for (OneTouchRecordAction action : actions) {
            if (action->GetRecorderAddress() == address) {
                RemoveAction(action);
            }
        }
#endif
}

ECode HotplugDetectionAction::MayDisableSystemAudioAndARC(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (HdmiUtils->GetTypeFromAddress(address) != IHdmiDeviceInfo::DEVICE_AUDIO_SYSTEM) {
            return NOERROR;
        }

        // Turn off system audio mode and update settings.
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        tv->SetSystemAudioMode(FALSE, TRUE);
        Boolean isArcEstabilished;
        tv->IsArcEstabilished(&isArcEstabilished);
        if (isArcEstabilished) {
            AutoPtr<RequestArcTerminationAction> newRequestArcTerminationAction = new RequestArcTerminationAction();
            newRequestArcTerminationAction->constructor(LocalDevice(), address);
            AddAndStartAction(newRequestArcTerminationAction);
        }
#endif
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
