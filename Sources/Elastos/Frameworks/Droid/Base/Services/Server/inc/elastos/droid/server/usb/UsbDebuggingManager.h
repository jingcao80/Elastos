
#ifndef __ELASTOS_DROID_SERVER_USB_USBDEBUGGINGMANAGER_H__
#define __ELASTOS_DROID_SERVER_USB_USBDEBUGGINGMANAGER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::IO::IOutputStream;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

class UsbDebuggingManager
    : public ElRefBase
    , public IRunnable
{
public:
    UsbDebuggingManager(
        /* [in] */ IContext* context);

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* pObject,
        /* [out] */ InterfaceID* pIID);

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

    // public void dump(FileDescriptor fd, PrintWriter pw)

private:
    class UsbDebuggingHandler
        : public HandlerBase
    {
        friend class UsbDebuggingManager;
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

    private:
        UsbDebuggingManager* mHost;
    };

private:
    CARAPI ListenToSocket();

    CARAPI_(void) CloseSocket();

    CARAPI_(void) SendResponse(
        /* [in] */ const String& msg);

    CARAPI_(String) GetFingerprints(
        /* [in] */ const String& key);

    CARAPI_(void) ShowConfirmationDialog(
        /* [in] */ const String& key,
        /* [in] */ const String& fingerprints);

    CARAPI_(void) WriteKey(
        /* [in] */ const String& key);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String ADBD_SOCKET;
    static const String ADB_DIRECTORY;
    static const String ADB_KEYS_FILE;
    static const Int32 BUFFER_SIZE;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IHandlerThread> mHandlerThread;
    AutoPtr<IThread> mThread;
    Boolean mAdbEnabled;
    String mFingerprints;
    AutoPtr<ILocalSocket> mSocket;
    AutoPtr<IOutputStream> mOutputStream;
    AutoPtr<UsbDebuggingHandler> mUsbDebuggingHandler;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_USBDEBUGGINGMANAGER_H__
