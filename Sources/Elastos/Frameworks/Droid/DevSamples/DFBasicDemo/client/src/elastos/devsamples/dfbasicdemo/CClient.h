
#ifndef __ELASTOS_DEVSAMPLES_DFBASICDEMO_CCLIENT_H__
#define __ELASTOS_DEVSAMPLES_DFBASICDEMO_CCLIENT_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_DevSamples_DFBasicDemo_CClient.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Core::Object;

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {

CarClass(CClient)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ CClient* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CClient* mHost;
    };

    class EditorListener
        : public Object
        , public IOnEditorActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        EditorListener(
            /* [in] */ CClient* host);

        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    private:
        CClient* mHost;
    };

    /* This class will handle all AllJoyn calls. See onCreate(). */
    class BusHandler
        : Handler
    {
    public:
        CARAPI constructor(
            /* [in] */ ILooper* looper,
            /* [in] */ CClient* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        /* These are the messages sent to the BusHandler from the UI. */
        static const Int32 CONNECT;
        static const Int32 JOIN_SESSION;
        static const Int32 DISCONNECT;
        static const Int32 CAT;

    private:
        /*
         * Name used as the well-known name and the advertised name of the service this client is
         * interested in.  This name must be a unique name both to the bus and to the network as a
         * whole.
         *
         * The name uses reverse URL style of naming, and matches the name used by the service.
         */
        static const String SERVICE_NAME;
        static const Int16 CONTACT_PORT;

        // BusAttachment mBus;
        // ProxyBusObject mProxyObj;
        // BasicInterface mBasicInterface;

        Int32 mSessionId;
        Boolean mIsInASession;
        Boolean mIsConnected;
        Boolean mIsStoppingDiscovery;
        CClient* mHost;
    };

public:
    CARAPI constructor();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    static const Int32 MESSAGE_PING;
    static const Int32 MESSAGE_PING_REPLY;
    static const Int32 MESSAGE_POST_TOAST;
    static const Int32 MESSAGE_START_PROGRESS_DIALOG;
    static const Int32 MESSAGE_STOP_PROGRESS_DIALOG;

    static const String TAG;

    AutoPtr<IEditText> mEditText;
    AutoPtr<IArrayAdapter> mListViewArrayAdapter;
    AutoPtr<IListView> mListView;
    // private Menu menu;

    /* Handler used to make calls to AllJoyn methods. See onCreate(). */
    // private BusHandler mBusHandler;

    // private ProgressDialog mDialog;

    AutoPtr<IHandler> mHandler;
};

} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFBASICDEMO_CCLIENT_H__
