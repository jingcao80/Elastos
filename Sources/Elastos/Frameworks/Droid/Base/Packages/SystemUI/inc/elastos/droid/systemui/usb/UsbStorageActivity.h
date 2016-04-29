
#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBSTORAGEACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBSTORAGEACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Storage::IStorageEventListener;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IProgressBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

/**
 * This activity is shown to the user for him/her to enable USB mass storage
 * on-demand (that is, when the USB cable is connected). It uses the alert
 * dialog style. It will be launched from a notification.
 */
class UsbStorageActivity
    : public Activity
    , public IViewOnClickListener
    , public IDialogInterfaceOnCancelListener
    , public IUsbStorageActivity
{
private:
    class MyUsbStateReceiver
        : public BroadcastReceiver
    {
    public:
        MyUsbStateReceiver(
            /* [in] */ UsbStorageActivity* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("UsbStorageActivity.MyStorageListener");
            return NOERROR;
        }

    private:
        UsbStorageActivity* mHost;
    };


    class MyStorageListener
        : public Object
        , public IStorageEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyStorageListener(
            /* [in] */ UsbStorageActivity* host);

        // @Override
        CARAPI OnStorageStateChanged(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState);

        // @Override
        CARAPI OnUsbMassStorageConnectionChanged(
            /* [in] */ Boolean connected);

    private:
        UsbStorageActivity* mHost;
    };

    class R1
        : public Runnable
    {
    public:
        R1(
            /* [in] */ Boolean usbStorageInUse,
            /* [in] */ UsbStorageActivity* host);

        //@Override
        CARAPI Run();

    private:
        UsbStorageActivity* mHost;
        Boolean mUsbStorageInUse;
    };

    class R2
        : public Runnable
    {
    public:
        R2(
            /* [in] */ UsbStorageActivity* host);

        //@Override
        CARAPI Run();

    private:
        UsbStorageActivity* mHost;
    };

    class R3
        : public Runnable
    {
    public:
        R3(
            /* [in] */ Boolean on,
            /* [in] */ UsbStorageActivity* host);

        //@Override
        CARAPI Run();

    private:
        UsbStorageActivity* mHost;
        Boolean mOn;
    };

    class R4
        : public Runnable
    {
    public:
        R4(
            /* [in] */ UsbStorageActivity* host);

        //@Override
        CARAPI Run();

    private:
        UsbStorageActivity* mHost;
    };

    class R5
        : public Runnable
    {
    public:
        R5(
            /* [in] */ Int32 id,
            /* [in] */ UsbStorageActivity* host);

        //@Override
        CARAPI Run();

    private:
        UsbStorageActivity* mHost;
        Int32 mId;
    };

    class R6
        : public Runnable
    {
    public:
        R6(
            /* [in] */ UsbStorageActivity* host);

        //@Override
        CARAPI Run();

    private:
        UsbStorageActivity* mHost;
    };

    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener(
            /* [in] */ UsbStorageActivity* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        UsbStorageActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    UsbStorageActivity();

    CARAPI constructor();

    // @Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args);

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    //@Override
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnPause();

private:
    CARAPI_(void) SwitchDisplay(
        /* [in] */ Boolean usbStorageInUse);

    CARAPI_(void) SwitchDisplayAsync(
        /* [in] */ Boolean usbStorageInUse);

    CARAPI_(void) HandleUsbStateChanged(
        /* [in] */ IIntent* intent);

    CARAPI_(AutoPtr<IIMountService>) GetMountService();

    CARAPI_(void) ScheduleShowDialog(
        /* [in] */ const Int32 id);

    CARAPI_(void) SwitchUsbMassStorage(
        /* [in] */ const Boolean on);

    CARAPI_(void) CheckStorageUsers();

    CARAPI_(void) CheckStorageUsersAsync();

private:
    const static String TAG;

    AutoPtr<IButton> mMountButton;
    AutoPtr<IButton> mUnmountButton;
    AutoPtr<IProgressBar> mProgressBar;
    AutoPtr<ITextView> mBanner;
    AutoPtr<ITextView> mMessage;
    AutoPtr<IImageView> mIcon;
    AutoPtr<IStorageManager> mStorageManager;
    const static Int32 DLG_CONFIRM_KILL_STORAGE_USERS = 1;
    const static Int32 DLG_ERROR_SHARING = 2;
    const static Boolean localLOGV;
    Boolean mDestroyed;

    // UI thread
    AutoPtr<IHandler> mUIHandler;

    // thread for working with the storage services, which can be slow
    AutoPtr<IHandler> mAsyncStorageHandler;

    /** Used to detect when the USB cable is unplugged, so we can call finish() */
    AutoPtr<MyUsbStateReceiver> mUsbStateReceiver;

    AutoPtr<MyStorageListener> mStorageListener;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBSTORAGEACTIVITY_H__