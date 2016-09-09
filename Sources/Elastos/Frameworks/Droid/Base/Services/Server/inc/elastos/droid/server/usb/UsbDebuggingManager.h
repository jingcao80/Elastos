
#ifndef __ELASTOS_DROID_SERVER_USB_USBDEBUGGINGMANAGER_H__
#define __ELASTOS_DROID_SERVER_USB_USBDEBUGGINGMANAGER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Core::IThread;
using Elastos::IO::IFile;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

class UsbDebuggingManager
    : public Runnable
{
private:
    class UsbDebuggingHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("UsbDebuggingManager::UsbDebuggingHandler")

        UsbDebuggingHandler(
            /* [in] */ UsbDebuggingManager* host,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI_(void) HandleAdbEnable();

        CARAPI_(void) HandleAdbDisable();

        CARAPI_(void) HandleAdbAllow(
            /* [in] */ Boolean alwaysAllow,
            /* [in] */ const String& key);

        CARAPI_(void) HandleAdbDeny();

        CARAPI_(void) HandleAdbConfirm(
            /* [in] */ const String& key);

    public:
        static const Int32 MESSAGE_ADB_ENABLED = 1;
        static const Int32 MESSAGE_ADB_DISABLED = 2;
        static const Int32 MESSAGE_ADB_ALLOW = 3;
        static const Int32 MESSAGE_ADB_DENY = 4;
        static const Int32 MESSAGE_ADB_CONFIRM = 5;
        static const Int32 MESSAGE_ADB_CLEAR = 6;

    private:
        UsbDebuggingManager* mHost;
    };

public:
    UsbDebuggingManager(
        /* [in] */ IContext* context);

    /**
     * Starts executing the active part of the class' code. This method is
     * called when a thread is started that has been created with a class which
     * implements {@code Runnable}.
     */
    CARAPI Run();

    CARAPI_(void) SetAdbEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(void) AllowUsbDebugging(
        /* [in] */ Boolean alwaysAllow,
        /* [in] */ const String& publicKey);

    CARAPI_(void) DenyUsbDebugging();

    CARAPI_(void) ClearUsbDebuggingKeys();

    // public void dump(FileDescriptor fd, PrintWriter pw)

    TO_STRING_IMPL("UsbDebuggingManager")

private:
    CARAPI ListenToSocket();

    CARAPI_(void) CloseSocket();

    CARAPI_(void) SendResponse(
        /* [in] */ const String& msg);

    CARAPI_(String) GetFingerprints(
        /* [in] */ const String& key);

    CARAPI_(void) StartConfirmation(
        /* [in] */ const String& key,
        /* [in] */ const String& fingerprints);

    /**
     * @returns true if the componentName led to an Activity that was started.
     */
    CARAPI_(Boolean) StartConfirmationActivity(
        /* [in] */ IComponentName* componentName,
        /* [in] */ const String& key,
        /* [in] */ const String& fingerprints);

    /**
     * @returns true if the componentName led to a Service that was started.
     */
    CARAPI_(Boolean) StartConfirmationService(
        /* [in] */ IComponentName* componentName,
        /* [in] */ const String& key,
        /* [in] */ const String& fingerprints);

    CARAPI_(AutoPtr<IIntent>) CreateConfirmationIntent(
        /* [in] */ IComponentName* componentName,
        /* [in] */ const String& key,
        /* [in] */ const String& fingerprints);

    CARAPI_(AutoPtr<IFile>) GetUserKeyFile();

    CARAPI_(void) WriteKey(
        /* [in] */ const String& key);

    CARAPI_(void) DeleteKeyFile();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String ADBD_SOCKET;
    static const String ADB_DIRECTORY;
    static const String ADB_KEYS_FILE;
    static const Int32 BUFFER_SIZE;

    AutoPtr<IContext> mContext;
    AutoPtr<UsbDebuggingHandler> mHandler;
    AutoPtr<IThread> mThread;
    Boolean mAdbEnabled;
    String mFingerprints;
    AutoPtr<ILocalSocket> mSocket;
    AutoPtr<IOutputStream> mOutputStream;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_USBDEBUGGINGMANAGER_H__
