
#ifndef __ELASTOS_DROID_SYSTEMUI_USB_STORAGENOTIFICATION_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_STORAGENOTIFICATION_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/systemui/SystemUI.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::Storage::IStorageEventListener;
using Elastos::Droid::Os::Storage::IStorageManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

class StorageNotification
    : public SystemUI
    , public IStorageNotification
{
private:
    class R1
        : public Runnable
    {
    public:
        R1(
            /* [in] */ Boolean connected,
            /* [in] */ StorageNotification* host);

        CARAPI Run();

    private:
        Boolean mConnected;
        StorageNotification* mHost;
    };
    class R2
        : public Runnable
    {
    public:
        R2(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState,
            /* [in] */ StorageNotification* host);

        CARAPI Run();

    private:
        String mPath;
        String mOldState;
        String mNewState;
        StorageNotification* mHost;
    };

    class StorageNotificationEventListener
        : public Object
        , public IStorageEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        StorageNotificationEventListener(
            /* [in] */ StorageNotification* host);

        CARAPI OnUsbMassStorageConnectionChanged(
            /* [in] */ Boolean connected);

        CARAPI OnStorageStateChanged(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState);

    private:
        StorageNotification* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    StorageNotification();

    CARAPI constructor();

    // @Override
    CARAPI Start();

    /**
     * Update the state of the USB mass storage notification
     */
    CARAPI UpdateUsbMassStorageNotification(
        /* [in] */ Boolean available);

private:
    CARAPI_(void) OnUsbMassStorageConnectionChangedAsync(
        /* [in] */ Boolean connected);

    CARAPI_(void) OnStorageStateChangedAsync(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState);

    /**
     * Sets the USB storage notification.
     */
    CARAPI_(void) SetUsbStorageNotification(
        /* [in] */ Int32 titleId,
        /* [in] */ Int32 messageId,
        /* [in] */ Int32 icon,
        /* [in] */ Boolean sound,
        /* [in] */ Boolean visible,
        /* [in] */ IPendingIntent* pi);

    CARAPI_(Boolean) GetMediaStorageNotificationDismissable();

    /**
     * Sets the media storage notification.
     */
    CARAPI_(void) SetMediaStorageNotification(
        /* [in] */ Int32 titleId,
        /* [in] */ Int32 messageId,
        /* [in] */ Int32 icon,
        /* [in] */ Boolean visible,
        /* [in] */ Boolean dismissable,
        /* [in] */ IPendingIntent* pi);

private:
    const static String TAG;
    const static Boolean DEBUG;

    const static Boolean POP_UMS_ACTIVITY_ON_CONNECT;

    /**
     * The notification that is shown when a USB mass storage host
     * is connected.
     * <p>
     * This is lazily created, so use {@link #setUsbStorageNotification()}.
     */
    AutoPtr<INotification> mUsbStorageNotification;

    /**
     * The notification that is shown when the following media events occur:
     *     - Media is being checked
     *     - Media is blank (or unknown filesystem)
     *     - Media is corrupt
     *     - Media is safe to unmount
     *     - Media is missing
     * <p>
     * This is lazily created, so use {@link #setMediaStorageNotification()}.
     */
    AutoPtr<INotification>   mMediaStorageNotification;
    Boolean mUmsAvailable;
    AutoPtr<IStorageManager> mStorageManager;

    AutoPtr<IHandler> mAsyncEventHandler;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_STORAGENOTIFICATION_H__