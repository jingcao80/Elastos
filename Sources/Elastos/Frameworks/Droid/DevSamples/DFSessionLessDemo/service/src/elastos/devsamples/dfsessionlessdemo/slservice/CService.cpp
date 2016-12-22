
#include "CService.h"
#include "CSimpleService.h"
#include "R.h"
#include <elastos/droid/os/Looper.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CBusListener;
using Org::Alljoyn::Bus::EIID_IBusObject;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFSessionLessDemo {
namespace SLService {

//======================================================================
//  CService::MyHander
//======================================================================
CService::MyHandler::MyHandler(
    /* [in] */ CService* host)
    : Handler(FALSE)
    , mHost(host)
{}

ECode CService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
    case MESSAGE_PING: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String cat = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Ping:  ") + cat));
        break;
    }
    case MESSAGE_PING_REPLY: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String reply = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Reply:  ") + reply));
        break;
    }
    case MESSAGE_POST_TOAST: {
        AutoPtr<IContext> context;
        mHost->GetApplicationContext((IContext**)&context);
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(context, ICharSequence::Probe(obj), IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
        break;
    }
    default:
        break;
    }
    return NOERROR;
}

//======================================================================
//  CService::SimpleService
//======================================================================
CAR_INTERFACE_IMPL_2(CService::SimpleService, Object, ISimpleInterface, IBusObject)
CAR_OBJECT_IMPL(CSimpleService)

ECode CService::SimpleService::constructor(
    /* [in] */ IObject* host)
{
    mHost = (CService*)host;
    return NOERROR;
}

ECode CService::SimpleService::Chat(
    /* [in] */ const String& inStr)
{
    Logger::I(TAG, "Signal  : %s", inStr.string());
    SendUiMessage(MESSAGE_PING, inStr);
    return NOERROR;
}

void CService::SimpleService::SendUiMessage(
    /* [in] */ Int32 what,
    /* [in] */ const String& str)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(what, CoreUtils::Convert(str), (IMessage**)&msg);
    Boolean result;
    mHost->mHandler->SendMessage(msg, &result);
}

//======================================================================
//  CService::BusHandler
//======================================================================
const Int32 CService::BusHandler::ADDMATCH = 1;
const Int32 CService::BusHandler::DISCONNECT = 2;

const String CService::BusHandler::SERVICE_NAME("org.alljoyn.bus.test.sessions");
const Int16 CService::BusHandler::CONTACT_PORT = 42;

ECode CService::BusHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ CService* host)
{
    Handler::constructor(looper);
    mHost = host;
    return NOERROR;
}

ECode CService::BusHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
    /* Connect to the bus and start our service. */
    case ADDMATCH: {
        AutoPtr<IContext> context;
        mHost->GetApplicationContext((IContext**)&context);
        AutoPtr<IDaemonInit> di;
        CDaemonInit::AcquireSingleton((IDaemonInit**)&di);
        Boolean result;
        di->PrepareDaemon(context, &result);
        /*
         * All communication through AllJoyn begins with a BusAttachment.
         *
         * A BusAttachment needs a name. The actual name is unimportant except for internal
         * security. As a default we use the class name as the name.
         *
         * By default AllJoyn does not allow communication between devices (i.e. bus to bus
         * communication). The second argument must be set to Receive to allow communication
         * between devices.
         */
        String pName;
        mHost->GetPackageName(&pName);
        mBus = NULL;
        CBusAttachment::New(pName, RemoteMessage_Receive, (IBusAttachment**)&mBus);

        /*
         * Create a bus listener class
         */
        AutoPtr<IBusListener> bl;
        CBusListener::New((IBusListener**)&bl);
        mBus->RegisterBusListener(bl);

        /*
         * To make a service available to other AllJoyn peers, first
         * register a BusObject with the BusAttachment at a specific
         * object path.  Our service is implemented by the SimpleService
         * BusObject found at the "/SimpleService" object path.
         */
        ECode ec = mBus->RegisterBusObject(mHost->mSimpleService, String("/SimpleService"));
        mHost->LogStatus(String("BusAttachment.registerBusObject()"), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * The next step in making a service available to other AllJoyn peers is to connect the
         * BusAttachment to the bus with a well-known name.
         */
        /*
         * connect the BusAttachement to the bus
         */
        ec = mBus->Connect();
        mHost->LogStatus(String("BusAttachment.connect()"), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         *  We register our signal handler which is implemented inside the SimpleService
         */
        ec = mBus->RegisterSignalHandlers((ISimpleInterface*)mHost->mSimpleService);
        if (ec != (ECode)E_STATUS_OK) {
            Logger::I(TAG, "Problem while registering signal handler");
            return NOERROR;
        }

        ec = mBus->AddMatch(String("sessionless='t'"));
        if (ec == (ECode)E_STATUS_OK) {
            Logger::I(TAG, "AddMatch was called successfully");
        }
        break;
    }
    /* Release all resources acquired in connect. */
    case DISCONNECT: {
        /*
         * It is important to unregister the BusObject before disconnecting from the bus.
         * Failing to do so could result in a resource leak.
         */
        mBus->UnregisterBusObject(mHost->mSimpleService);
        mBus->Disconnect();
        AutoPtr<ILooper> looper;
        mHost->mBusHandler->GetLooper((ILooper**)&looper);
        looper->Quit();
        break;
    }

    default:
        break;
    }
    return NOERROR;
}

//======================================================================
//  CService
//======================================================================
const String CService::TAG("SessionlessService");
const Int32 CService::MESSAGE_PING;
const Int32 CService::MESSAGE_PING_REPLY;
const Int32 CService::MESSAGE_POST_TOAST;

ECode CService::constructor()
{
    mHandler = new MyHandler(this);
    return Activity::constructor();
}

ECode CService::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    SetContentView(R::layout::main);

    CArrayAdapter::New((IContext*)this, R::layout::message, (IArrayAdapter**)&mListViewArrayAdapter);
    AutoPtr<IView> v;
    FindViewById(R::id::ListView, (IView**)&v);
    mListView = IListView::Probe(v);
    IAdapterView::Probe(mListView)->SetAdapter(IAdapter::Probe(mListViewArrayAdapter));

    /*
     * You always need to make all AllJoyn calls through a separate handler
     * thread to prevent blocking the Activity.
     */
    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BusHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBusHandler = new BusHandler();
    mBusHandler->constructor(looper, this);

    /* Start our service. */
    AutoPtr<ISimpleInterface> bi;
    CSimpleService::New(this, (ISimpleInterface**)&bi);
    mSimpleService = (SimpleService*)bi.Get();
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::ADDMATCH, &result);
    return NOERROR;
}

ECode CService::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::mainmenu, menu);
    mMenu = menu;
    *result = TRUE;
    return NOERROR;
}

ECode CService::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Handle item selection
    Int32 id;
    item->GetItemId(&id);
    switch (id) {
    case R::id::quit:
        Finish();
        *result = TRUE;
        return NOERROR;
    default:
        return Activity::OnOptionsItemSelected(item, result);
    }
}

ECode CService::OnDestroy()
{
    Activity::OnDestroy();

    /* Disconnect to prevent resource leaks. */
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::DISCONNECT, &result);
    return NOERROR;
}

void CService::LogStatus(
    /* [in] */ const String& msg,
    /* [in] */ ECode status)
{
    String log("");
    log.AppendFormat("%s: 0x%08x", msg.string(), status);
    if (status == (ECode)E_STATUS_OK) {
        Logger::I(TAG, log.string());
    }
    else {
        AutoPtr<IMessage> toastMsg;
        mHandler->ObtainMessage(MESSAGE_POST_TOAST, CoreUtils::Convert(log), (IMessage**)&toastMsg);
        Boolean result;
        mHandler->SendMessage(toastMsg, &result);
        Logger::E(TAG, log.string());
    }
}

} // namespace SLService
} // namespace DFSessionLessDemo
} // namespace DevSamples
} // namespace Elastos
