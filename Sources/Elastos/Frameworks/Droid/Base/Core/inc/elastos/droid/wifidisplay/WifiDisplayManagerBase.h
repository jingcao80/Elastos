#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace WifiDisplay {

class WifiDisplayManagerBase
{
private:
    class H
        : public HandlerBase
    {
    public:
        H(
            /* [in] */ WifiDisplayManagerBase* host);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        static String CodeToString(
            /* [in] */ Int32 code);
    public:
        static const Int32 EXIT_WIFIDISPLAY_RECEIVE         ;//= 100;
        static const Int32 EXIT_WIFIDISPLAY_SEND            ;//= 101;
        static const Int32 START_WIFIDISPLAY_SEND           ;//= 102;
        static const Int32 DISPATCH_WIFIDISPLAY_EVEVT       ;//= 103;
    private:
        WifiDisplayManagerBase* mHost;
    };

    class WifiDisplayThread
        : public ElRefBase
        , public IWifiDisplayThread
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        WifiDisplayThread(
            /* [in] */ WifiDisplayManagerBase* host);

        CARAPI ExitWifiDisplayReceive();

        CARAPI ExitWifiDisplaySend();

        CARAPI StartWifiDisplaySend();

        CARAPI DispatchMotionEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI GetHashCode(
             /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* info);
    private:
        WifiDisplayManagerBase* mHost;
    };

public:
    WifiDisplayManagerBase();

    virtual ~WifiDisplayManagerBase();

    virtual CARAPI StartWifiDisplaySend(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    virtual CARAPI EndWifiDisplaySend(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    virtual CARAPI StartWifiDisplayReceive(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    virtual CARAPI EndWifiDisplayReceive(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

protected:
    virtual CARAPI OnExitWifiDisplayReceive() = 0;

    virtual CARAPI OnExitWifiDisplaySend() = 0;

    virtual CARAPI OnStartWifiDisplaySend() = 0;

    virtual CARAPI OnDispatchMotionEvent(
        /* [in] */ IMotionEvent* motionevent) = 0;

private:
    CARAPI_(void) QueueOrSendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI_(void) QueueOrSendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 arg1);

    CARAPI_(void) QueueOrSendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

public:
    static const Int32 EXIT_WIFIDISPLAY_RECEIVE         = 100;
    static const Int32 EXIT_WIFIDISPLAY_SEND            = 101;
    static const Int32 START_WIFIDISPLAY_SEND           = 102;
    static const Int32 DISPATCH_WIFIDISPLAY_EVEVT       = 103;

    static const String TAG;
private:
    static const Boolean DEBUG_MESSAGES;// = false;
    static const Boolean DEBUG_REMOTE;// = true;

    AutoPtr<IIWindowManager> mWindowManager;
    AutoPtr<IWifiDisplayManager> mWifiDisplayManager;
    AutoPtr<IWifiDisplayThread> mWifiDisplayThread;

    AutoPtr<IHandler> mH;
    Object mThisLock;
};

} // namespace WifiDisplay
} // namespace Droid
} // namespace Elastos
