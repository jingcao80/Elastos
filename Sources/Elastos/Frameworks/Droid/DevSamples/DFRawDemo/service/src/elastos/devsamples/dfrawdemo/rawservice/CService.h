
#ifndef __ELASTOS_DEVSAMPLES_DFRAWDEMO_RAWSERVICE_CSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFRAWDEMO_RAWSERVICE_CSERVICE_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_DevSamples_DFRawDemo_RawService_CService.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <elastos/core/Runnable.h>
#include <_Org.Alljoyn.Bus.h>
#include <org/alljoyn/bus/SessionPortListener.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::Object;
using Elastos::Core::Runnable;
using Elastos::Core::IThread;
using Elastos::IO::IReader;

using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IBusObject;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::SessionPortListener;

namespace Elastos {
namespace DevSamples {
namespace DFRawDemo {
namespace RawService {

CarClass(CService)
    , public Activity
{
public:
    class RawService
        : public Object
        , public IRawInterface
        , public IBusObject
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor(
            /* [in] */ IObject* host);

        CARAPI RequestRawSession(
            /* [out] */ Int16* port);

    private:
        CService* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CService::MyHandler")

        MyHandler(
            /* [in] */ CService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CService* mHost;
    };

    /* This class will handle all AllJoyn calls. See onCreate(). */
    class BusHandler
        : public Handler
    {
        friend class RawService;
    private:
        class InnerSessionPortListener : public SessionPortListener
        {
        public:
            InnerSessionPortListener(
                /* [in] */ BusHandler* host,
                /* [in] */ Boolean another = FALSE)
                : mHost(host)
                , mAnother(another)
            {}

            CARAPI AcceptSessionJoiner(
                /* [in] */ Int16 sessionPort,
                /* [in] */ const String& joiner,
                /* [in] */ ISessionOpts* opts,
                /* [out] */ Boolean* accepted);

            // @Override
            CARAPI SessionJoined(
                /* [in] */ Int16 sessionPort,
                /* [in] */ Int32 sessionId,
                /* [in] */ const String& joiner);

        private:
            BusHandler* mHost;
            Boolean mAnother;
        };

        class ReaderRunnable : public Runnable
        {
        public:
            ReaderRunnable(
                /* [in] */ IReader* reader,
                /* [in] */ IHandler* handler);

            CARAPI Run();

        private:
            AutoPtr<IReader> mReader;
            AutoPtr<IHandler> mHandler;
        };

    public:
        TO_STRING_IMPL("CService::BusHandler")

        CARAPI constructor(
            /* [in] */ ILooper* looper,
            /* [in] */ CService* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        /* These are the messages sent to the BusHandler from the UI. */
        static const Int32 CONNECT;
        static const Int32 DISCONNECT;
        static const Int32 JOINED;

        Boolean mStreamUp;

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

        /*
         * TODO: Remove this when we ephemeral session ports is fully implemented.
         */
        static const String RAW_SERVICE_NAME;
        static const Int16 RAW_PORT;

        AutoPtr<IBusAttachment> mBus;
        Int32 mRawSessionId;
        AutoPtr<IThread> mReader;
        CService* mHost;
    };

public:
    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnDestroy();

private:
    CARAPI_(void) LogStatus(
        /* [in] */ const String& msg,
        /* [in] */ ECode status);

private:
    static const String TAG;

    static const Int32 MESSAGE_PING = 1;
    static const Int32 MESSAGE_PING_REPLY = 2;
    static const Int32 MESSAGE_POST_TOAST = 3;

    AutoPtr<IArrayAdapter> mListViewArrayAdapter;
    AutoPtr<IListView> mListView;
    AutoPtr<IMenu> mMenu;

    AutoPtr<IHandler> mHandler;

    /* The AllJoyn object that is our service. */
    AutoPtr<RawService> mRawService;

    /* Handler used to make calls to AllJoyn methods. See onCreate(). */
    AutoPtr<BusHandler> mBusHandler;
};

} // namespace RawService
} // namespace DFRawDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFRAWDEMO_RAWSERVICE_CSERVICE_H__
