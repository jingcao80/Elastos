
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERAMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_CAMERAMANAGER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::IICameraService;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class CameraManager
    : public Object
    , public ICameraManager
{
public:
    class AvailabilityCallback
        : public Object
        , public ICameraManagerAvailabilityCallback
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * A new camera has become available to use.
         *
         * <p>The default implementation of this method does nothing.</p>
         *
         * @param cameraId The unique identifier of the new camera.
         */
        CARAPI OnCameraAvailable(
            /* [in] */ const String& cameraId);

        /**
         * A previously-available camera has become unavailable for use.
         *
         * <p>If an application had an active CameraDevice instance for the
         * now-disconnected camera, that application will receive a
         * {@link CameraDevice.StateCallback#onDisconnected disconnection error}.</p>
         *
         * <p>The default implementation of this method does nothing.</p>
         *
         * @param cameraId The unique identifier of the disconnected camera.
         */
        CARAPI OnCameraUnavailable(
            /* [in] */ const String& cameraId);
    };

    /**
     * Temporary for migrating to Callback naming
     * @hide
     */
    class AvailabilityListener
        : public AvailabilityCallback
    {
    };

private:
    class MyRunnableAvailable
        : public Runnable
    {
    public:
        MyRunnableAvailable(
            /* [in] */ ICameraManagerAvailabilityCallback* _callback,
            /* [in] */ const String& id);

        CARAPI Run();

    private:
        AutoPtr<ICameraManagerAvailabilityCallback> mCallback;
        String mId;
    };

    class MyRunnableUnavailable
        : public Runnable
    {
    public:
        MyRunnableUnavailable(
            /* [in] */ ICameraManagerAvailabilityCallback* _callback,
            /* [in] */ const String& id);

        CARAPI Run();

    private:
        AutoPtr<ICameraManagerAvailabilityCallback> mCallback;
        String mId;
    };

    /**
     * Listener for camera service death.
     *
     * <p>The camera service isn't supposed to die under any normal circumstances, but can be turned
     * off during debug, or crash due to bugs.  So detect that and null out the interface object, so
     * that the next calls to the manager can try to reconnect.</p>
     */
    class CameraServiceDeathListener
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        CameraServiceDeathListener(
            /* [in] */ CameraManager* host);

        CARAPI ProxyDied();

    private:
        CameraManager* mHost;
    };

    // TODO: this class needs unit tests
    // TODO: extract class into top level
    class CameraServiceListener
        : public Object
        , public IICameraServiceListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CameraServiceListener(
            /* [in] */ CameraManager* host);

        virtual ~CameraServiceListener() {}

        //@Override
        CARAPI AsBinder(
            /* [out] */ IBinder** binder);

        /**
         * Send the state of all known cameras to the provided listener, to initialize
         * the listener's knowledge of camera state.
         */
        CARAPI UpdateCallbackLocked(
            /* [in] */ ICameraManagerAvailabilityCallback* _callback,
            /* [in] */ IHandler* handler);

        //@Override
        CARAPI OnStatusChanged(
            /* [in] */ Int32 status,
            /* [in] */ Int32 cameraId);

        CARAPI OnStatusChangedLocked(
            /* [in] */ Int32 status,
            /* [in] */ const String& id);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CARAPI_(Boolean) IsAvailable(
            /* [in] */ Int32 status);

        CARAPI_(Boolean) ValidStatus(
            /* [in] */ Int32 status);

        CARAPI PostSingleUpdate(
            /* [in] */ ICameraManagerAvailabilityCallback* _callback,
            /* [in] */ IHandler* handler,
            /* [in] */ const String& id,
            /* [in] */ Int32 status);

    public:
        // Keep up-to-date with ICameraServiceListener.h

        // Device physically unplugged
        static const Int32 STATUS_NOT_PRESENT;
        // Device physically has been plugged in
        // and the camera can be used exclusively
        static const Int32 STATUS_PRESENT;
        // Device physically has been plugged in
        // but it will not be connect-able until enumeration is complete
        static const Int32 STATUS_ENUMERATING;
        // Camera is in use by another app and cannot be used exclusively
        static const Int32 STATUS_NOT_AVAILABLE;

    private:
        // Camera ID -> Status map
        AutoPtr<IArrayMap> mDeviceStatus;

        static const String TAG;

        CameraManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CameraManager();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Return the list of currently connected camera devices by
     * identifier.
     *
     * <p>Non-removable cameras use integers starting at 0 for their
     * identifiers, while removable cameras have a unique identifier for each
     * individual device, even if they are the same model.</p>
     *
     * @return The list of currently connected camera devices.
     */
    CARAPI GetCameraIdList(
        /* [out, callee] */ ArrayOf<String>** outarr);

    /**
     * Register a callback to be notified about camera device availability.
     *
     * <p>Registering the same callback again will replace the handler with the
     * new one provided.</p>
     *
     * <p>The first time a callback is registered, it is immediately called
     * with the availability status of all currently known camera devices.</p>
     *
     * @param callback the new callback to send camera availability notices to
     * @param handler The handler on which the callback should be invoked, or
     * {@code null} to use the current thread's {@link android.os.Looper looper}.
     */
    CARAPI RegisterAvailabilityCallback(
        /* [in] */ ICameraManagerAvailabilityCallback* ccallback,
        /* [in] */ IHandler* handler);

    /**
     * Remove a previously-added callback; the callback will no longer receive connection and
     * disconnection callbacks.
     *
     * <p>Removing a callback that isn't registered has no effect.</p>
     *
     * @param callback The callback to remove from the notification list
     */
    CARAPI UnregisterAvailabilityCallback(
        /* [in] */ ICameraManagerAvailabilityCallback* ccallback);

    /**
     * <p>Query the capabilities of a camera device. These capabilities are
     * immutable for a given camera.</p>
     *
     * @param cameraId The id of the camera device to query
     * @return The properties of the given camera
     *
     * @throws IllegalArgumentException if the cameraId does not match any
     *         known camera device.
     * @throws CameraAccessException if the camera is disabled by device policy, or
     *         the camera device has been disconnected.
     * @throws SecurityException if the application does not have permission to
     *         access the camera
     *
     * @see #getCameraIdList
     * @see android.app.admin.DevicePolicyManager#setCameraDisabled
     */
    CARAPI GetCameraCharacteristics(
        /* [in] */ const String& cameraId,
        /* [out] */ ICameraCharacteristics** outcc);

    /**
     * Open a connection to a camera with the given ID.
     *
     * <p>Use {@link #getCameraIdList} to get the list of available camera
     * devices. Note that even if an id is listed, open may fail if the device
     * is disconnected between the calls to {@link #getCameraIdList} and
     * {@link #openCamera}.</p>
     *
     * <p>Once the camera is successfully opened, {@link CameraDevice.StateCallback#onOpened} will
     * be invoked with the newly opened {@link CameraDevice}. The camera device can then be set up
     * for operation by calling {@link CameraDevice#createCaptureSession} and
     * {@link CameraDevice#createCaptureRequest}</p>
     *
     * <!--
     * <p>Since the camera device will be opened asynchronously, any asynchronous operations done
     * on the returned CameraDevice instance will be queued up until the device startup has
     * completed and the callback's {@link CameraDevice.StateCallback#onOpened onOpened} method is
     * called. The pending operations are then processed in order.</p>
     * -->
     * <p>If the camera becomes disconnected during initialization
     * after this function call returns,
     * {@link CameraDevice.StateCallback#onDisconnected} with a
     * {@link CameraDevice} in the disconnected state (and
     * {@link CameraDevice.StateCallback#onOpened} will be skipped).</p>
     *
     * <p>If opening the camera device fails, then the device callback's
     * {@link CameraDevice.StateCallback#onError onError} method will be called, and subsequent
     * calls on the camera device will throw a {@link CameraAccessException}.</p>
     *
     * @param cameraId
     *             The unique identifier of the camera device to open
     * @param callback
     *             The callback which is invoked once the camera is opened
     * @param handler
     *             The handler on which the callback should be invoked, or
     *             {@code null} to use the current thread's {@link android.os.Looper looper}.
     *
     * @throws CameraAccessException if the camera is disabled by device policy,
     * or the camera has become or was disconnected.
     *
     * @throws IllegalArgumentException if cameraId or the callback was null,
     * or the cameraId does not match any currently or previously available
     * camera device.
     *
     * @throws SecurityException if the application does not have permission to
     * access the camera
     *
     * @see #getCameraIdList
     * @see android.app.admin.DevicePolicyManager#setCameraDisabled
     */
    CARAPI OpenCamera(
        /* [in] */ const String& cameraId,
        /* [in] */ ICameraDeviceStateCallback* ccallback,
        /* [in] */ IHandler* handler);

private:
    /**
     * Helper for openning a connection to a camera with the given ID.
     *
     * @param cameraId The unique identifier of the camera device to open
     * @param callback The callback for the camera. Must not be null.
     * @param handler  The handler to invoke the callback on. Must not be null.
     *
     * @throws CameraAccessException if the camera is disabled by device policy,
     * or too many camera devices are already open, or the cameraId does not match
     * any currently available camera device.
     *
     * @throws SecurityException if the application does not have permission to
     * access the camera
     * @throws IllegalArgumentException if callback or handler is null.
     * @return A handle to the newly-created camera device.
     *
     * @see #getCameraIdList
     * @see android.app.admin.DevicePolicyManager#setCameraDisabled
     */
    CARAPI OpenCameraDeviceUserAsync(
        /* [in] */ const String& cameraId,
        /* [in] */ ICameraDeviceStateCallback* _callback,
        /* [in] */ IHandler* handler,
        /* [out] */ ICameraDevice** device);

    /**
     * Return or create the list of currently connected camera devices.
     *
     * <p>In case of errors connecting to the camera service, will return an empty list.</p>
     */
    CARAPI GetOrCreateDeviceIdListLocked(
        /* [out] */ IArrayList** list);

    CARAPI HandleRecoverableSetupErrors(
        /* [in] */ Int32 e,
        /* [in] */ const String& msg);

    /**
     * Queries the camera service if it supports the camera2 api directly, or needs a shim.
     *
     * @param cameraId a non-{@code null} camera identifier
     * @return {@code false} if the legacy shim needs to be used, {@code true} otherwise.
     */
    CARAPI SupportsCamera2ApiLocked(
        /* [in] */ const String& cameraId,
        /* [out] */ Boolean* result);

    /**
     * Queries the camera service if it supports a camera api directly, or needs a shim.
     *
     * @param cameraId a non-{@code null} camera identifier
     * @param apiVersion the version, i.e. {@code API_VERSION_1} or {@code API_VERSION_2}
     * @return {@code true} if connecting will work for that device version.
     */
    CARAPI SupportsCameraApiLocked(
        /* [in] */ const String& cameraId,
        /* [in] */ Int32 apiVersion,
        /* [out] */ Boolean* result);

    /**
     * Connect to the camera service if it's available, and set up listeners.
     *
     * <p>Sets mCameraService to a valid pointer or null if the connection does not succeed.</p>
     */
    CARAPI ConnectCameraServiceLocked();

    /**
     * Return a best-effort ICameraService.
     *
     * <p>This will be null if the camera service
     * is not currently available. If the camera service has died since the last
     * use of the camera service, will try to reconnect to the service.</p>
     */
    CARAPI GetCameraServiceLocked(
        /* [out] */ IICameraService** service);

private:
    static const String TAG;
    const Boolean DEBUG;

    /**
     * This should match the ICameraService definition
     */
    static const String CAMERA_SERVICE_BINDER_NAME;
    static const Int32 USE_CALLING_UID;

    //@SuppressWarnings("unused")
    static const Int32 API_VERSION_1;
    static const Int32 API_VERSION_2;

    // Access only through getCameraServiceLocked to deal with binder death
    AutoPtr<IICameraService> mCameraService;

    AutoPtr<IArrayList> mDeviceIdList;

    AutoPtr<IArrayMap> mCallbackMap;

    AutoPtr<IContext> mContext;
    Object mLock;

    AutoPtr<CameraServiceListener> mServiceListener;
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_CAMERAMANAGER_H__
