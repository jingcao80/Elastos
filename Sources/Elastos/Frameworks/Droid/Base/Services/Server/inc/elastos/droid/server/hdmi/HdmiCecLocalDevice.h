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

#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICE_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICE_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiControlService;
class HdmiCecFeatureAction;
class HdmiCecMessageCache;
/**
 * Class that models a logical CEC device hosted in this system. Handles initialization,
 * CEC commands that call for actions customized per device type.
 */
class HdmiCecLocalDevice
    : public Object
    , public IHdmiCecLocalDevice
{
public:
    class ActiveSource
        : public Object
        , public IHdmiCecLocalDeviceActiveSource
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ Int32 logical,
            /* [in] */ Int32 physical);

        static CARAPI Of(
            /* [in] */ Int32 logical,
            /* [in] */ Int32 physical,
            /* [out] */ IHdmiCecLocalDeviceActiveSource** result);

        CARAPI IsValid(
            /* [out] */ Boolean* result);

        CARAPI Invalidate();

        CARAPI IsEquals(
            /* [in] */ Int32 logical,
            /* [in] */ Int32 physical,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 mLogicalAddress;

        Int32 mPhysicalAddress;
    };

private:
    class InnerSub_Handler
        : public Handler
    {
    public:
        TO_STRING_IMPL("HdmiCecLocalDevice::InnerSub_Handler")

        InnerSub_Handler(
            /* [in] */ HdmiCecLocalDevice* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        HdmiCecLocalDevice* mHost;
    };

    class InnerSub_PendingActionClearedCallback
        : public Object
        , public IHdmiCecLocalDevicePendingActionClearedCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_PendingActionClearedCallback(
            /* [in] */ HdmiCecLocalDevice* host,
            /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* origialCallback);

        // @Override
        CARAPI OnCleared(
            /* [in] */ IHdmiCecLocalDevice* device);

    private:
        HdmiCecLocalDevice* mHost;
        AutoPtr<IHdmiCecLocalDevicePendingActionClearedCallback> mOrigialCallback;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiCecLocalDevice();

    CARAPI constructor(
        /* [in] */ IHdmiControlService* service,
        /* [in] */ Int32 deviceType);

    // Factory method that returns HdmiCecLocalDevice of corresponding type.
    static CARAPI Create(
        /* [in] */ IHdmiControlService* service,
        /* [in] */ Int32 deviceType,
        /* [out] */ IHdmiCecLocalDevice** result);

    // @ServiceThreadOnly
    CARAPI Init();

    /**
     * Called once a logical address of the local device is allocated.
     */
    virtual CARAPI OnAddressAllocated(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 reason) = 0;

    /**
     * Get the preferred logical address from system properties.
     */
    virtual CARAPI GetPreferredAddress(
        /* [out] */ Int32* result) = 0;

    /**
     * Set the preferred logical address to system properties.
     */
    virtual CARAPI SetPreferredAddress(
        /* [in] */ Int32 addr) = 0;

    /**
     * Dispatch incoming message.
     *
     * @param message incoming message
     * @return true if consumed a message; otherwise, return false.
     */
    // @ServiceThreadOnly
    CARAPI DispatchMessage(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI OnMessage(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleGivePhysicalAddress(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleGiveDeviceVendorId(
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleGetCecVersion(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleActiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleInactiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleRequestActiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleGetMenuLanguage(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleGiveOsdName(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleRoutingChange(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleRoutingInformation(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleReportPhysicalAddress(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleSystemAudioModeStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleSetSystemAudioMode(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleTerminateArc(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleInitiateArc(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleReportAudioStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleStandby(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleUserControlPressed(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleUserControlReleased(
        /* [out] */ Boolean* result);

    static CARAPI InjectKeyEvent(
        /* [in] */ Int64 time,
        /* [in] */ Int32 action,
        /* [in] */ Int32 keycode,
        /* [in] */ Int32 repeat);

    static CARAPI IsPowerOnOrToggleCommand(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    static CARAPI IsPowerOffOrToggleCommand(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleTextViewOn(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleImageViewOn(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleSetStreamPath(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleGiveDevicePowerStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleMenuRequest(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleMenuStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleVendorCommand(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleVendorCommandWithId(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI SendStandby(
        /* [in] */ Int32 deviceId);

    CARAPI HandleSetOsdName(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleRecordTvScreen(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleTimerClearedStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleReportPowerStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleTimerStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    CARAPI HandleRecordStatus(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI HandleAddressAllocated(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 reason);

    CARAPI GetType(
        /* [out] */ Int32* result);

    // @ServiceThreadOnly
    CARAPI GetDeviceInfo(
        /* [out] */ IHdmiDeviceInfo** result);

    // @ServiceThreadOnly
    CARAPI SetDeviceInfo(
        /* [in] */ IHdmiDeviceInfo* info);

    // Returns true if the logical address is same as the argument.
    // @ServiceThreadOnly
    CARAPI IsAddressOf(
        /* [in] */ Int32 addr,
        /* [out] */ Boolean* result);

    // Resets the logical address to unregistered(15), meaning the logical device is invalid.
    // @ServiceThreadOnly
    CARAPI ClearAddress();

    // @ServiceThreadOnly
    CARAPI AddAndStartAction(
        /* [in] */ HdmiCecFeatureAction* action);

    // @ServiceThreadOnly
    CARAPI StartQueuedActions();

    // See if we have an action of a given type in progress.
    // @ServiceThreadOnly
    CARAPI HasAction(
        /* [in] */ ClassID clazz,
        /* [out] */ Boolean* result);

    // Returns all actions matched with given class type.
    // @ServiceThreadOnly
    CARAPI GetActions(
        /* [in] */ ClassID clazz,
        /* [out] */ IList** result);

    /**
     * Remove the given {@link HdmiCecFeatureAction} object from the action queue.
     *
     * @param action {@link HdmiCecFeatureAction} to remove
     */
    // @ServiceThreadOnly
    CARAPI RemoveAction(
        /* [in] */ HdmiCecFeatureAction* action);

    // Remove all actions matched with the given Class type.
    // @ServiceThreadOnly
    CARAPI RemoveAction(
        /* [in] */ ClassID clazz);

    // Remove all actions matched with the given Class type besides |exception|.
    // @ServiceThreadOnly
    CARAPI RemoveActionExcept(
        /* [in] */ ClassID clazz,
        /* [in] */ HdmiCecFeatureAction* exception);

    CARAPI CheckIfPendingActionsCleared();

    CARAPI AssertRunOnServiceThread();

    /**
     * Called when a hot-plug event issued.
     *
     * @param portId id of port where a hot-plug event happened
     * @param connected whether to connected or not on the event
     */
    CARAPI OnHotplug(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean connected);

    CARAPI GetService(
        /* [out] */ IHdmiControlService** result);

    // @ServiceThreadOnly
    CARAPI IsConnectedToArcPort(
        /* [in] */ Int32 path,
        /* [out] */ Boolean* result);

    CARAPI GetActiveSource(
        /* [out] */ IHdmiCecLocalDeviceActiveSource** result);

    CARAPI SetActiveSource(
        /* [in] */ IHdmiCecLocalDeviceActiveSource* newActive);

    CARAPI SetActiveSource(
        /* [in] */ IHdmiDeviceInfo* info);

    CARAPI SetActiveSource(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 physicalAddress);

    CARAPI GetActivePath(
        /* [out] */ Int32* result);

    CARAPI SetActivePath(
        /* [in] */ Int32 path);

    /**
     * Returns the ID of the active HDMI port. The active port is the one that has the active
     * routing path connected to it directly or indirectly under the device hierarchy.
     */
    CARAPI GetActivePortId(
        /* [out] */ Int32* result);

    /**
     * Update the active port.
     *
     * @param portId the new active port id
     */
    CARAPI SetActivePortId(
        /* [in] */ Int32 portId);

    // @ServiceThreadOnly
    CARAPI GetCecMessageCache(
        /* [out] */ HdmiCecMessageCache** result);

    // @ServiceThreadOnly
    CARAPI PathToPortId(
        /* [in] */ Int32 newPath,
        /* [out] */ Int32* result);

    /**
     * Called when the system goes to standby mode.
     *
     * @param initiatedByCec true if this power sequence is initiated
     *        by the reception the CEC messages like &lt;Standby&gt;
     */
    CARAPI OnStandby(
        /* [in] */ Boolean initiatedByCec);

    /**
     * Disable device. {@code callback} is used to get notified when all pending
     * actions are completed or timeout is issued.
     *
     * @param initiatedByCec true if this sequence is initiated
     *        by the reception the CEC messages like &lt;Standby&gt;
     * @param origialCallback callback interface to get notified when all pending actions are
     *        cleared
     */
    CARAPI DisableDevice(
        /* [in] */ Boolean initiatedByCec,
        /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* origialCallback);

    /**
     * Send a key event to other device.
     *
     * @param keyCode key code defined in {@link android.view.KeyEvent}
     * @param isPressed {@code true} for key down event
     */
    CARAPI SendKeyEvent(
        /* [in] */ Int32 keyCode,
        /* [in] */ Boolean isPressed);

    CARAPI SendUserControlPressedAndReleased(
        /* [in] */ Int32 targetAddress,
        /* [in] */ Int32 cecKeycode);

    /**
     * Dump internal status of HdmiCecLocalDevice object.
     */
    CARAPI Dump(
        /* [in] */ IIndentingPrintWriter* pw);

private:
    // @ServiceThreadOnly
    CARAPI_(Boolean) DispatchMessageToAction(
        /* [in] */ IHdmiCecMessage* message);

    // @ServiceThreadOnly
    CARAPI HandleDisableDeviceTimeout();

public:
    AutoPtr<IHdmiControlService> mService;

    Int32 mDeviceType;

    Int32 mAddress;

    Int32 mPreferredAddress;

    AutoPtr<IHdmiDeviceInfo> mDeviceInfo;

    Int32 mLastKeycode;

    Int32 mLastKeyRepeatCount;

    AutoPtr<IObject> mLock;

    AutoPtr<IHdmiCecLocalDevicePendingActionClearedCallback> mPendingActionClearedCallback;

protected:
    // Logical address of the active source.
    // @GuardedBy("mLock")
    AutoPtr<IHdmiCecLocalDeviceActiveSource> mActiveSource;

    AutoPtr<HdmiCecMessageCache> mCecMessageCache;

private:
    static const String TAG;

    static const Int32 MSG_DISABLE_DEVICE_TIMEOUT;

    static const Int32 MSG_USER_CONTROL_RELEASE_TIMEOUT;

    // Normal actions timeout is 2s but some of them would have several sequence of timeout.
    static const Int32 DEVICE_CLEANUP_TIMEOUT;

    // When it expires, we can assume <User Control Release> is received.
    static const Int32 FOLLOWER_SAFETY_TIMEOUT;


    // Active routing path. Physical address of the active source but not all the time, such as
    // when the new active source does not claim itself to be one. Note that we don't keep
    // the active port id (or active input) since it can be gotten by {@link #pathToPortId(int)}.
    // @GuardedBy("mLock")
    Int32 mActiveRoutingPath;

    // A collection of FeatureAction.
    // Note that access to this collection should happen in service thread.
    AutoPtr<ILinkedList> mActions;

    AutoPtr<IHandler> mHandler;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICE_H__
