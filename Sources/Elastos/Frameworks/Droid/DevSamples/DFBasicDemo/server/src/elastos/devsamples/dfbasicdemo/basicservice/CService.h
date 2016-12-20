
#ifndef __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CSERVICE_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_DevSamples_DFBasicDemo_BasicService_CService.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>
#include <_Org.Alljoyn.Bus.h>
#include <org/alljoyn/bus/SessionPortListener.h>

using Elastos::Droid::App::Activity;
// using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Os::Handler;
// using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
// using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IArrayAdapter;
// using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IListView;
// using Elastos::Droid::Widget::ITextView;
// using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Core::Object;
using Elastos::Utility::IMap;

using Org::Alljoyn::Bus::IVariant;
// using Org::Alljoyn::Bus::BusListener;
using Org::Alljoyn::Bus::IBusAttachment;
using Org::Alljoyn::Bus::IBusObject;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::SessionPortListener;

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {
namespace BasicService {

CarClass(CService)
    , public Activity
{
public:
    class BasicService
        : public Object
        , public IBasicInterface
        , public IBusObject
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI constructor(
            /* [in] */ IObject* host);

        CARAPI Cat(
            /* [in] */ const String& inStr1,
            /* [in] */ const String& inStr2,
            /* [out] */ String* result);

        CARAPI Test(
            /* [in] */ Byte ip1,
            /* [in] */ Boolean ip2,
            /* [in] */ Int16 ip3,
            /* [in] */ Int32 ip4,
            /* [in] */ Int64 ip5,
            /* [in] */ Double ip6,
            /* [in] */ const String& ip7,
            /* [in] */ ArrayOf<String>* ip8,
            /* [out] */ Byte* op1,
            /* [out] */ Boolean* op2,
            /* [out] */ Int16* op3,
            /* [out] */ Int32* op4,
            /* [out] */ Int64* op5,
            /* [out] */ Double* op6,
            /* [out] */ String* op7,
            /* [out, callee] */ ArrayOf<String>** op8);

        CARAPI TestArrayOf(
            /* [in] */ ArrayOf<Int32>* inarray,
            /* [out, callee] */ ArrayOf<Int32>** outarray);

        CARAPI TestMapOne(
            /* [in] */ IMap* bytebooleanMap,      //<Byte, Boolean>
            /* [in] */ IMap* int16int32Map,       //<Int16, Int32>
            /* [in] */ IMap* int64doubleMap,      //<Int64, Double>
            /* [in] */ IMap* strstrMap);          //<String, String>

        CARAPI TestMapTwo(
            /* [in] */ IMap* strint32arrayMap,    //<String, ArrayOf<Int32> >
            /* [in] */ IMap* strstrarrayMap,      //<String, ArrayOf<String> >
            /* [in] */ IMap* strstrint32mapMap);  //<String, Map<String, Int32> >

        CARAPI TestOutMap(
            /* [out] */ IMap** strint32arrayMap,    //<String, ArrayOf<Int32> >
            /* [out] */ IMap** strstrarrayMap,      //<String, ArrayOf<String> >
            /* [out] */ IMap** strstrint32mapMap);  //<String, Map<String, Int32> >

        CARAPI TestVariant(
            /* [in] */ IVariant* v);

        CARAPI TestOutVariant(
            /* [out] */ IVariant** v);
    private:
        /* Helper function to send a message to the UI thread. */
        CARAPI_(void) SendUiMessage(
            /* [in] */ Int32 what,
            /* [in] */ const String& str);

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
    private:
        class InnerSessionPortListener : public SessionPortListener
        {
        public:
            InnerSessionPortListener(
                /* [in] */ BusHandler* host)
                : mHost(host)
            {}

            CARAPI AcceptSessionJoiner(
                /* [in] */ Int16 sessionPort,
                /* [in] */ const String& joiner,
                /* [in] */ ISessionOpts* opts,
                /* [out] */ Boolean* accepted);

        private:
            BusHandler* mHost;
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

        AutoPtr<IBusAttachment> mBus;
        CService* mHost;
    };

public:
    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnResume();

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
    CARAPI OnStop();

    // @Override
    CARAPI OnDestroy();

private:
    CARAPI_(void) LogStatus(
        /* [in] */ const String& msg,
        /* [in] */ ECode status);

private:
    static const String TAG;

    static const Int32 MESSAGE_CAT = 1;
    static const Int32 MESSAGE_CAT_REPLY = 2;
    static const Int32 MESSAGE_POST_TOAST = 3;

    AutoPtr<IArrayAdapter> mListViewArrayAdapter;
    AutoPtr<IListView> mListView;
    AutoPtr<IMenu> mMenu;

    AutoPtr<IHandler> mHandler;

    /* The AllJoyn object that is our service. */
    AutoPtr<BasicService> mBasicService;

    /* Handler used to make calls to AllJoyn methods. See onCreate(). */
    AutoPtr<BusHandler> mBusHandler;
};

} // namespace BasicService
} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CSERVICE_H__
