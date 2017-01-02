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

#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICETV_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICETV_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IArraySet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecStandbyModeHandler;
/**
 * Represent a logical device of type TV residing in Android system.
 */
class HdmiCecLocalDeviceTv
    : public HdmiCecLocalDevice
    , public IHdmiCecLocalDeviceTv
{
private:
    class InnerSub_DeviceDiscoveryCallback
        : public Object
        , public IDeviceDiscoveryActionDeviceDiscoveryCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_DeviceDiscoveryCallback(
            /* [in] */ HdmiCecLocalDeviceTv* host);

        // @Override
        CARAPI OnDeviceDiscoveryDone(
            /* [in] */ IList* deviceInfos);

    private:
        HdmiCecLocalDeviceTv* mHost;
    };

    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ HdmiCecLocalDeviceTv* host);

        // @Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        HdmiCecLocalDeviceTv* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiCecLocalDeviceTv();

    CARAPI constructor(
        /* [in] */ IHdmiControlService* service);

    // @Override
    // @ServiceThreadOnly
    CARAPI OnAddressAllocated(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 reason);

    // @Override
    // @ServiceThreadOnly
    CARAPI GetPreferredAddress(
        /* [out] */ Int32* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI SetPreferredAddress(
        /* [in] */ Int32 addr);

    // @Override
    // @ServiceThreadOnly
    CARAPI DispatchMessage(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    /**
     * Performs the action 'device select', or 'one touch play' initiated by TV.
     *
     * @param id id of HDMI device to select
     * @param callback callback object to report the result with
     */
    // @ServiceThreadOnly
    CARAPI DeviceSelect(
        /* [in] */ Int32 id,
        /* [in] */ IIHdmiControlCallback* callback);

    // @ServiceThreadOnly
    CARAPI UpdateActiveSource(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 physicalAddress);

    // @ServiceThreadOnly
    CARAPI UpdateActiveSource(
        /* [in] */ IHdmiCecLocalDeviceActiveSource* newActive);

    CARAPI GetPortId(
        /* [in] */ Int32 physicalAddress,
        /* [out] */ Int32* result);

    /**
     * Returns the previous port id kept to handle input switching on <Inactive Source>.
     */
    CARAPI GetPrevPortId(
        /* [out] */ Int32* result);

    /**
     * Sets the previous port id. INVALID_PORT_ID invalidates it, hence no actions will be
     * taken for <Inactive Source>.
     */
    CARAPI SetPrevPortId(
        /* [in] */ Int32 portId);

    // @ServiceThreadOnly
    CARAPI UpdateActiveInput(
        /* [in] */ Int32 path,
        /* [in] */ Boolean notifyInputChange);

    // @ServiceThreadOnly
    CARAPI DoManualPortSwitching(
        /* [in] */ Int32 portId,
        /* [in] */ IIHdmiControlCallback* callback);

    // @ServiceThreadOnly
    CARAPI GetPowerStatus(
        /* [out] */ Int32* result);

    /**
     * Sends key to a target CEC device.
     *
     * @param keyCode key code to send. Defined in {@link android.view.KeyEvent}.
     * @param isPressed true if this is key press event
     */
    // @Override
    // @ServiceThreadOnly
    CARAPI SendKeyEvent(
        /* [in] */ Int32 keyCode,
        /* [in] */ Boolean isPressed);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleActiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleInactiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleRequestActiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleGetMenuLanguage(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI BroadcastMenuLanguage(
        /* [in] */ const String& language,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleReportPhysicalAddress(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleReportPowerStatus(
        /* [in] */ IHdmiCecMessage* command,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleTimerStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleRecordStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI UpdateCecSwitchInfo(
        /* [in] */ Int32 address,
        /* [in] */ Int32 type,
        /* [in] */ Int32 path,
        /* [out] */ Boolean* result);

    CARAPI StartNewDeviceAction(
        /* [in] */ IHdmiCecLocalDeviceActiveSource* activeSource);

    /**
     * Whether the given path is located in the tail of current active path.
     *
     * @param path to be tested
     * @param activePath current active path
     * @return true if the given path is located in the tail of current active path; otherwise,
     *         false
     */
    static CARAPI IsTailOfActivePath(
        /* [in] */ Int32 path,
        /* [in] */ Int32 activePath,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleRoutingChange(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleReportAudioStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleTextViewOn(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleImageViewOn(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleSetOsdName(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI OnNewAvrAdded(
        /* [in] */ IHdmiDeviceInfo* avr);

    // Seq #32
    CARAPI ChangeSystemAudioMode(
        /* [in] */ Boolean enabled,
        /* [in] */ IIHdmiControlCallback* callback);

    // # Seq 25
    CARAPI SetSystemAudioMode(
        /* [in] */ Boolean on,
        /* [in] */ Boolean updateSetting);

    CARAPI IsSystemAudioActivated(
        /* [out] */ Boolean* result);

    CARAPI GetSystemAudioModeSetting(
        /* [out] */ Boolean* result);

    /**
     * Change ARC status into the given {@code enabled} status.
     *
     * @return {@code true} if ARC was in "Enabled" status
     */
    // @ServiceThreadOnly
    CARAPI SetArcStatus(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* result);

    /**
     * Returns whether ARC is enabled or not.
     */
    // @ServiceThreadOnly
    CARAPI IsArcEstabilished(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI ChangeArcFeatureEnabled(
        /* [in] */ Boolean enabled);

    // @ServiceThreadOnly
    CARAPI IsArcFeatureEnabled(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI StartArcAction(
        /* [in] */ Boolean enabled);

    CARAPI SetAudioStatus(
        /* [in] */ Boolean mute,
        /* [in] */ Int32 volume);

    // @ServiceThreadOnly
    CARAPI ChangeVolume(
        /* [in] */ Int32 curVolume,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 maxVolume);

    // @ServiceThreadOnly
    CARAPI ChangeMute(
        /* [in] */ Boolean mute);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleInitiateArc(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleTerminateArc(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleSetSystemAudioMode(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleSystemAudioModeStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // Seq #53
    // @Override
    // @ServiceThreadOnly
    CARAPI HandleRecordTvScreen(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleTimerClearedStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI AnnounceOneTouchRecordResult(
        /* [in] */ Int32 result);

    CARAPI AnnounceTimerRecordingResult(
        /* [in] */ Int32 result);

    CARAPI AnnounceClearTimerRecordingResult(
        /* [in] */ Int32 result);

    /**
     * Return a list of all {@link HdmiDeviceInfo}.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     * This is not thread-safe. For thread safety, call {@link #getSafeExternalInputsLocked} which
     * does not include local device.
     */
    // @ServiceThreadOnly
    CARAPI GetDeviceInfoList(
        /* [in] */ Boolean includeLocalDevice,
        /* [out] */ IList** result);

    /**
     * Return external input devices.
     */
    CARAPI GetSafeExternalInputsLocked(
        /* [out] */ IList** result);

    // @ServiceThreadOnly
    CARAPI GetAvrDeviceInfo(
        /* [out] */ IHdmiDeviceInfo** result);

    /**
     * Return a {@link HdmiDeviceInfo} corresponding to the given {@code logicalAddress}.
     *
     * This is not thread-safe. For thread safety, call {@link #getSafeCecDeviceInfo(int)}.
     *
     * @param logicalAddress logical address of the device to be retrieved
     * @return {@link HdmiDeviceInfo} matched with the given {@code logicalAddress}.
     *         Returns null if no logical address matched
     */
    // @ServiceThreadOnly
    CARAPI GetCecDeviceInfo(
        /* [in] */ Int32 logicalAddress,
        /* [out] */ IHdmiDeviceInfo** result);

    CARAPI HasSystemAudioDevice(
        /* [out] */ Boolean* result);

    CARAPI GetSafeAvrDeviceInfo(
        /* [out] */ IHdmiDeviceInfo** result);

    /**
     * Thread safe version of {@link #getCecDeviceInfo(int)}.
     *
     * @param logicalAddress logical address to be retrieved
     * @return {@link HdmiDeviceInfo} matched with the given {@code logicalAddress}.
     *         Returns null if no logical address matched
     */
    CARAPI GetSafeCecDeviceInfo(
        /* [in] */ Int32 logicalAddress,
        /* [out] */ IHdmiDeviceInfo** result);

    /**
     * Called when a device is newly added or a new device is detected or
     * existing device is updated.
     *
     * @param info device info of a new device.
     */
    // @ServiceThreadOnly
    CARAPI AddCecDevice(
        /* [in] */ IHdmiDeviceInfo* info);

    /**
     * Called when a device is removed or removal of device is detected.
     *
     * @param address a logical address of a device to be removed
     */
    // @ServiceThreadOnly
    CARAPI RemoveCecDevice(
        /* [in] */ Int32 address);

    // @ServiceThreadOnly
    CARAPI HandleRemoveActiveRoutingPath(
        /* [in] */ Int32 path);

    /**
     * Launch routing control process.
     *
     * @param routingForBootup true if routing control is initiated due to One Touch Play
     *        or TV power on
     */
    // @ServiceThreadOnly
    CARAPI LaunchRoutingControl(
        /* [in] */ Boolean routingForBootup);

    /**
     * Returns the {@link HdmiDeviceInfo} instance whose physical address matches
     * the given routing path. CEC devices use routing path for its physical address to
     * describe the hierarchy of the devices in the network.
     *
     * @param path routing path or physical address
     * @return {@link HdmiDeviceInfo} if the matched info is found; otherwise null
     */
    // @ServiceThreadOnly
    CARAPI GetDeviceInfoByPath(
        /* [in] */ Int32 path,
        /* [out] */ IHdmiDeviceInfo** result);

    // @Override
    // @ServiceThreadOnly
    CARAPI OnHotplug(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean connected);

    // @ServiceThreadOnly
    CARAPI SetAutoDeviceOff(
        /* [in] */ Boolean enabled);

    // @ServiceThreadOnly
    CARAPI SetAutoWakeup(
        /* [in] */ Boolean enabled);

    // @ServiceThreadOnly
    CARAPI GetAutoWakeup(
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI DisableDevice(
        /* [in] */ Boolean initiatedByCec,
        /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback);

    // @Override
    // @ServiceThreadOnly
    CARAPI OnStandby(
        /* [in] */ Boolean initiatedByCec);

    CARAPI IsProhibitMode(
        /* [out] */ Boolean* result);

    CARAPI IsPowerStandbyOrTransient(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI DisplayOsd(
        /* [in] */ Int32 messageId);

    // @ServiceThreadOnly
    CARAPI DisplayOsd(
        /* [in] */ Int32 messageId,
        /* [in] */ Int32 extra);

    // Seq #54 and #55
    // @ServiceThreadOnly
    CARAPI StartOneTouchRecord(
        /* [in] */ Int32 recorderAddress,
        /* [in] */ ArrayOf<Byte>* recordSource,
        /* [out] */ Int32* result);

    // @ServiceThreadOnly
    CARAPI StopOneTouchRecord(
        /* [in] */ Int32 recorderAddress);

    // @ServiceThreadOnly
    CARAPI StartTimerRecording(
        /* [in] */ Int32 recorderAddress,
        /* [in] */ Int32 sourceType,
        /* [in] */ ArrayOf<Byte>* recordSource);

    // @ServiceThreadOnly
    CARAPI ClearTimerRecording(
        /* [in] */ Int32 recorderAddress,
        /* [in] */ Int32 sourceType,
        /* [in] */ ArrayOf<Byte>* recordSource);

    CARAPI UpdateDevicePowerStatus(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 newPowerStatus);

    // @Override
    CARAPI HandleMenuStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SendStandby(
        /* [in] */ Int32 deviceId);

    // @Override
    CARAPI Dump(
        /* [in] */ IIndentingPrintWriter* pw);

private:
    // @ServiceThreadOnly
    CARAPI HandleSelectInternalSource();

    static CARAPI InvokeCallback(
        /* [in] */ IIHdmiControlCallback* callback,
        /* [in] */ Int32 result);

    CARAPI HandleNewDeviceAtTheTailOfActivePath(
        /* [in] */ Int32 path);

    // @ServiceThreadOnly
    CARAPI LaunchDeviceDiscovery();

    // Clear all device info.
    // @ServiceThreadOnly
    CARAPI ClearDeviceInfoList();

    CARAPI UpdateAudioManagerForSystemAudio(
        /* [in] */ Boolean on);

    CARAPI NotifyArcStatusToAudioService(
        /* [in] */ Boolean enabled);

    CARAPI IsDirectConnectAddress(
        /* [in] */ Int32 physicalAddress,
        /* [out] */ Boolean* result);

    CARAPI CanStartArcUpdateAction(
        /* [in] */ Int32 avrAddress,
        /* [in] */ Boolean shouldCheckArcFeatureEnabled,
        /* [out] */ Boolean* result);

    CARAPI IsMessageForSystemAudio(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    /**
     * Add a new {@link HdmiDeviceInfo}. It returns old device info which has the same
     * logical address as new device info's.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param deviceInfo a new {@link HdmiDeviceInfo} to be added.
     * @return {@code null} if it is new device. Otherwise, returns old {@HdmiDeviceInfo}
     *         that has the same logical address as new one has.
     */
    // @ServiceThreadOnly
    CARAPI AddDeviceInfo(
        /* [in] */ IHdmiDeviceInfo* deviceInfo,
        /* [out] */ IHdmiDeviceInfo** result);

    /**
     * Remove a device info corresponding to the given {@code logicalAddress}.
     * It returns removed {@link HdmiDeviceInfo} if exists.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param id id of device to be removed
     * @return removed {@link HdmiDeviceInfo} it exists. Otherwise, returns {@code null}
     */
    // @ServiceThreadOnly
    CARAPI RemoveDeviceInfo(
        /* [in] */ Int32 id,
        /* [out] */ IHdmiDeviceInfo** result);

    // @ServiceThreadOnly
    CARAPI UpdateSafeDeviceInfoList();

    /**
     * Return a list of external cec input (source) devices.
     *
     * <p>Note that this effectively excludes non-source devices like system audio,
     * secondary TV.
     */
    CARAPI GetInputDevices(
        /* [out] */ IList** result);

    // a parent CEC device (which should be the CEC-enabled switch) in the list.
    CARAPI HideDevicesBehindLegacySwitch(
        /* [in] */ IHdmiDeviceInfo* info,
        /* [out] */ Boolean* result);

    static CARAPI IsConnectedToCecSwitch(
        /* [in] */ Int32 path,
        /* [in] */ ICollection* switches,
        /* [out] */ Boolean* result);

    static CARAPI IsParentPath(
        /* [in] */ Int32 parentPath,
        /* [in] */ Int32 childPath,
        /* [out] */ Boolean* result);

    CARAPI InvokeDeviceEventListener(
        /* [in] */ IHdmiDeviceInfo* info,
        /* [in] */ Int32 status);

    // @ServiceThreadOnly
    CARAPI IsLocalDeviceAddress(
        /* [in] */ Int32 address,
        /* [out] */ Boolean* result);

    /**
     * Whether a device of the specified physical address and logical address exists
     * in a device info list. However, both are minimal condition and it could
     * be different device from the original one.
     *
     * @param logicalAddress logical address of a device to be searched
     * @param physicalAddress physical address of a device to be searched
     * @return true if exist; otherwise false
     */
    // @ServiceThreadOnly
    CARAPI IsInDeviceList(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 physicalAddress,
        /* [out] */ Boolean* result);

    CARAPI RemoveCecSwitches(
        /* [in] */ Int32 portId);

    // @ServiceThreadOnly
    CARAPI DisableSystemAudioIfExist();

    // @ServiceThreadOnly
    CARAPI DisableArcIfExist();

    CARAPI CheckRecorder(
        /* [in] */ Int32 recorderAddress,
        /* [out] */ Boolean* result);

    CARAPI CheckRecordSource(
        /* [in] */ ArrayOf<Byte>* recordSource,
        /* [out] */ Boolean* result);

    CARAPI CheckTimerRecordingSource(
        /* [in] */ Int32 sourceType,
        /* [in] */ ArrayOf<Byte>* recordSource,
        /* [out] */ Boolean* result);

    CARAPI SendClearTimerMessage(
        /* [in] */ Int32 recorderAddress,
        /* [in] */ Int32 sourceType,
        /* [in] */ ArrayOf<Byte>* recordSource);

private:
    static const String TAG;

    // Whether ARC is available or not. "true" means that ARC is established between TV and
    // AVR as audio receiver.
    // @ServiceThreadOnly
    Boolean mArcEstablished;

    // Whether ARC feature is enabled or not. The default value is true.
    // TODO: once adding system setting for it, read the value to it.
    Boolean mArcFeatureEnabled;

    // Whether System audio mode is activated or not.
    // This becomes true only when all system audio sequences are finished.
    // @GuardedBy("mLock")
    Boolean mSystemAudioActivated;

    // The previous port id (input) before switching to the new one. This is remembered in order to
    // be able to switch to it upon receiving <Inactive Source> from currently active source.
    // This remains valid only when the active source was switched via one touch play operation
    // (either by TV or source device). Manual port switching invalidates this value to
    // Constants.PORT_INVALID, for which case <Inactive Source> does not do anything.
    // @GuardedBy("mLock")
    Int32 mPrevPortId;

    // @GuardedBy("mLock")
    Int32 mSystemAudioVolume;

    // @GuardedBy("mLock")
    Boolean mSystemAudioMute;

    // Copy of mDeviceInfos to guarantee thread-safety.
    // @GuardedBy("mLock")
    AutoPtr<IList> mSafeAllDeviceInfos;
    // All external cec input(source) devices. Does not include system audio device.
    // @GuardedBy("mLock")
    AutoPtr<IList> mSafeExternalInputs;

    // Map-like container of all cec devices including local ones.
    // device id is used as key of container.
    // This is not thread-safe. For external purpose use mSafeDeviceInfos.
    AutoPtr<ISparseArray> mDeviceInfos;

    // If true, TV going to standby mode puts other devices also to standby.
    Boolean mAutoDeviceOff;

    // If true, TV wakes itself up when receiving <Text/Image View On>.
    Boolean mAutoWakeup;

    AutoPtr<HdmiCecStandbyModeHandler> mStandbyHandler;

    // If true, do not do routing control/send active source for internal source.
    // Set to true when the device was woken up by <Text/Image View On>.
    Boolean mSkipRoutingControl;

    // Set of physical addresses of CEC switches on the CEC bus. Managed independently from
    // other CEC devices since they might not have logical address.
    AutoPtr<IArraySet> mCecSwitches;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICETV_H__
