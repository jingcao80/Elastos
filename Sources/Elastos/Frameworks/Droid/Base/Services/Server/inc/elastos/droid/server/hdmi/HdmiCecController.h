
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECCONTROLLER_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Internal.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Core::IInteger32;
using Elastos::Core::IRunnable;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::IPredicate;
using Elastos::Droid::Os::IHandler;
using Elastos::Utility::IList;
using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiControlService;
/**
 * Manages HDMI-CEC command and behaviors. It converts user's command into CEC command
 * and pass it to CEC HAL so that it sends message to other device. For incoming
 * message it translates the message and delegates it to proper module.
 *
 * <p>It should be careful to access member variables on IO thread because
 * it can be accessed from system thread as well.
 *
 * <p>It can be created only by {@link HdmiCecController#create}
 *
 * <p>Declared as package-private, accessed by {@link HdmiControlService} only.
 */
class HdmiCecController
    : public Object
{
private:
    class RemoteDevicePredicate
        : public Object
        , public IPredicate
    {
    public:
        // @Override
        CARAPI Apply(
            /* [in] */ IInteger32* address,
            /* [out] */ Boolean* result);
    };

    class SystemAudioPredicate
        : public Object
        , public IPredicate
    {
    public:
        // @Override
        CARAPI Apply(
            /* [in] */ IInteger32* address,
            /* [out] */ Boolean* result);
    };

public:
    /**
     * A factory method to get {@link HdmiCecController}. If it fails to initialize
     * inner device or has no device it will return {@code null}.
     *
     * <p>Declared as package-private, accessed by {@link HdmiControlService} only.
     * @param service {@link HdmiControlService} instance used to create internal handler
     *                and to pass callback for incoming message or event.
     * @return {@link HdmiCecController} if device is initialized successfully. Otherwise,
     *         returns {@code null}.
     */
    static CARAPI Create(
        /* [in] */ HdmiControlService* service,
        /* [out] */ HdmiCecController** result);

    // @ServiceThreadOnly
    CARAPI AddLocalDevice(
        /* [in] */ Int32 deviceType,
        /* [in] */ IHdmiCecLocalDevice* device);

    /**
     * Allocate a new logical address of the given device type. Allocated
     * address will be reported through {@link AllocateAddressCallback}.
     *
     * <p> Declared as package-private, accessed by {@link HdmiControlService} only.
     *
     * @param deviceType type of device to used to determine logical address
     * @param preferredAddress a logical address preferred to be allocated.
     *                         If sets {@link Constants#ADDR_UNREGISTERED}, scans
     *                         the smallest logical address matched with the given device type.
     *                         Otherwise, scan address will start from {@code preferredAddress}
     * @param callback callback interface to report allocated logical address to caller
     */
    // @ServiceThreadOnly
    CARAPI AllocateLogicalAddress(
        /* [in] */ Int32 deviceType,
        /* [in] */ Int32 preferredAddress,
        /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback);

    CARAPI GetPortInfos(
        /* [out, callee] */ ArrayOf<IHdmiPortInfo*>* result);

    /**
     * Return the locally hosted logical device of a given type.
     *
     * @param deviceType logical device type
     * @return {@link HdmiCecLocalDevice} instance if the instance of the type is available;
     *          otherwise null.
     */
    CARAPI GetLocalDevice(
        /* [in] */ Int32 deviceType,
        /* [out] */ IHdmiCecLocalDevice** result);

    /**
     * Add a new logical address to the device. Device's HW should be notified
     * when a new logical address is assigned to a device, so that it can accept
     * a command having available destinations.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param newLogicalAddress a logical address to be added
     * @return 0 on success. Otherwise, returns negative value
     */
    // @ServiceThreadOnly
    CARAPI AddLogicalAddress(
        /* [in] */ Int32 newLogicalAddress,
        /* [out] */ Int32* result);

    /**
     * Clear all logical addresses registered in the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    // @ServiceThreadOnly
    CARAPI ClearLogicalAddress();

    // @ServiceThreadOnly
    CARAPI ClearLocalDevices();

    /**
     * Return the physical address of the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @return CEC physical address of the device. The range of success address
     *         is between 0x0000 and 0xFFFF. If failed it returns -1
     */
    // @ServiceThreadOnly
    CARAPI GetPhysicalAddress(
        /* [out] */ Int32* result);

    /**
     * Return CEC version of the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    // @ServiceThreadOnly
    CARAPI GetVersion(
        /* [out] */ Int32* result);

    /**
     * Return vendor id of the device.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    // @ServiceThreadOnly
    CARAPI GetVendorId(
        /* [out] */ Int32* result);

    /**
     * Set an option to CEC HAL.
     *
     * @param flag key of option
     * @param value value of option
     */
    // @ServiceThreadOnly
    CARAPI SetOption(
        /* [in] */ Int32 flag,
        /* [in] */ Int32 value);

    /**
     * Configure ARC circuit in the hardware logic to start or stop the feature.
     *
     * @param enabled whether to enable/disable ARC
     */
    // @ServiceThreadOnly
    CARAPI SetAudioReturnChannel(
        /* [in] */ Boolean enabled);

    /**
     * Return the connection status of the specified port
     *
     * @param port port number to check connection status
     * @return true if connected; otherwise, return false
     */
    // @ServiceThreadOnly
    CARAPI IsConnected(
        /* [in] */ Int32 port,
        /* [out] */ Boolean* result);

    /**
     * Poll all remote devices. It sends &lt;Polling Message&gt; to all remote
     * devices.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     *
     * @param callback an interface used to get a list of all remote devices' address
     * @param sourceAddress a logical address of source device where sends polling message
     * @param pickStrategy strategy how to pick polling candidates
     * @param retryCount the number of retry used to send polling message to remote devices
     */
    // @ServiceThreadOnly
    CARAPI PollDevices(
        /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
        /* [in] */ Int32 sourceAddress,
        /* [in] */ Int32 pickStrategy,
        /* [in] */ Int32 retryCount);

    /**
     * Return a list of all {@link HdmiCecLocalDevice}s.
     *
     * <p>Declared as package-private. accessed by {@link HdmiControlService} only.
     */
    // @ServiceThreadOnly
    CARAPI GetLocalDeviceList(
        /* [out] */ IList** result);

    // @ServiceThreadOnly
    CARAPI MaySendFeatureAbortCommand(
        /* [in] */ IHdmiCecMessage* message,
        /* [in] */ Int32 reason);

    // @ServiceThreadOnly
    CARAPI SendCommand(
        /* [in] */ IHdmiCecMessage* cecMessage);

    // @ServiceThreadOnly
    CARAPI SendCommand(
        /* [in] */ IHdmiCecMessage* cecMessage,
        /* [in] */ IHdmiControlServiceSendMessageCallback* callback);

    CARAPI Dump(
        /* [in] */ IIndentingPrintWriter* pw);

private:
    // Private constructor.  Use HdmiCecController.create().
    HdmiCecController(
        /* [in] */ HdmiControlService* service);

    CARAPI Init(
        /* [in] */ Int64 nativePtr);

    // @IoThreadOnly
    CARAPI HandleAllocateLogicalAddress(
        /* [in] */ Int32 deviceType,
        /* [in] */ Int32 preferredAddress,
        /* [in] */ IHdmiCecControllerAllocateAddressCallback* callback);

    static CARAPI BuildBody(
        /* [in] */ Int32 opcode,
        /* [in] */ ArrayOf<Byte>* params,
        /* [out, callee] */ ArrayOf<Byte>* result);

    CARAPI PickPollCandidates(
        /* [in] */ Int32 pickStrategy,
        /* [out] */ IList** result);

    // @ServiceThreadOnly
    CARAPI IsAllocatedLocalDeviceAddress(
        /* [in] */ Int32 address,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI RunDevicePolling(
        /* [in] */ Int32 sourceAddress,
        /* [in] */ IList* candidates,
        /* [in] */ Int32 retryCount,
        /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
        /* [in] */ IList* allocated);

    // @IoThreadOnly
    CARAPI SendPollMessage(
        /* [in] */ Int32 sourceAddress,
        /* [in] */ Int32 destinationAddress,
        /* [in] */ Int32 retryCount,
        /* [out] */ Boolean* result);

    CARAPI AssertRunOnIoThread();

    CARAPI AssertRunOnServiceThread();

    // it can be accessed from system thread as well.
    CARAPI RunOnIoThread(
        /* [in] */ IRunnable* runnable);

    CARAPI RunOnServiceThread(
        /* [in] */ IRunnable* runnable);

    CARAPI IsAcceptableAddress(
        /* [in] */ Int32 address,
        /* [out] */ Boolean* result);

    // @ServiceThreadOnly
    CARAPI OnReceiveCommand(
        /* [in] */ IHdmiCecMessage* message);

    /**
     * Called by native when incoming CEC message arrived.
     */
    // @ServiceThreadOnly
    CARAPI HandleIncomingCecCommand(
        /* [in] */ Int32 srcAddress,
        /* [in] */ Int32 dstAddress,
        /* [in] */ ArrayOf<Byte>* body);

    /**
     * Called by native when a hotplug event issues.
     */
    // @ServiceThreadOnly
    CARAPI HandleHotplug(
        /* [in] */ Int32 port,
        /* [in] */ Boolean connected);

    static CARAPI NativeInit(
        /* [in] */ HdmiCecController* handler,
        /* [in] */ IMessageQueue* messageQueue,
        /* [out] */ Int64* result);

    static CARAPI NativeSendCecCommand(
        /* [in] */ Int64 controllerPtr,
        /* [in] */ Int32 srcAddress,
        /* [in] */ Int32 dstAddress,
        /* [in] */ ArrayOf<Byte>* body,
        /* [out] */ Int32* result);

    static CARAPI NativeAddLogicalAddress(
        /* [in] */ Int64 controllerPtr,
        /* [in] */ Int32 logicalAddress,
        /* [out] */ Int32* result);

    static CARAPI NativeClearLogicalAddress(
        /* [in] */ Int64 controllerPtr);

    static CARAPI NativeGetPhysicalAddress(
        /* [in] */ Int64 controllerPtr,
        /* [out] */ Int32* result);

    static CARAPI NativeGetVersion(
        /* [in] */ Int64 controllerPtr,
        /* [out] */ Int32* result);

    static CARAPI NativeGetVendorId(
        /* [in] */ Int64 controllerPtr,
        /* [out] */ Int32* result);

    static CARAPI NativeGetPortInfos(
        /* [in] */ Int64 controllerPtr,
        /* [out, callee] */ ArrayOf<IHdmiPortInfo*>* result);

    static CARAPI NativeSetOption(
        /* [in] */ Int64 controllerPtr,
        /* [in] */ Int32 flag,
        /* [in] */ Int32 value);

    static CARAPI NativeSetAudioReturnChannel(
        /* [in] */ Int64 controllerPtr,
        /* [in] */ Boolean flag);

    static CARAPI NativeIsConnected(
        /* [in] */ Int64 controllerPtr,
        /* [in] */ Int32 port,
        /* [out] */ Boolean* result);

private:
    static const String TAG;

    static const AutoPtr<ArrayOf<Byte> > EMPTY_BODY;

    static const Int32 NUM_LOGICAL_ADDRESS;

    // Predicate for whether the given logical address is remote device's one or not.
    AutoPtr<IPredicate> mRemoteDeviceAddressPredicate;

    // Predicate whether the given logical address is system audio's one or not
    AutoPtr<IPredicate> mSystemAudioAddressPredicate;

    // Handler instance to process synchronous I/O (mainly send) message.
    AutoPtr<IHandler> mIoHandler;

    // Handler instance to process various messages coming from other CEC
    // device or issued by internal state change.
    AutoPtr<IHandler> mControlHandler;

    // Stores the pointer to the native implementation of the service that
    // interacts with HAL.
    /* volatile */ Int64 mNativePtr;

    AutoPtr<HdmiControlService> mService;

    // Stores the local CEC devices in the system. Device type is used for key.
    AutoPtr<ISparseArray> mLocalDevices;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECCONTROLLER_H__
