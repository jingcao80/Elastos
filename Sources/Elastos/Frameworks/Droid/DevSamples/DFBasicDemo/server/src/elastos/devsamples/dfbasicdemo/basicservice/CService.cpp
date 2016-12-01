
#include "CService.h"
#include "CBasicService.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
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
using Org::Alljoyn::Bus::CMutableInteger16Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::EIID_IBusObject;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger16Value;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::ISessionPortListener;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {
namespace BasicService {

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
    case MESSAGE_CAT: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String cat = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Cat args:  ") + cat));
        break;
    }
    case MESSAGE_CAT_REPLY: {
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
//  CService::BasicService
//======================================================================
CAR_INTERFACE_IMPL_2(CService::BasicService, Object, IBasicInterface, IBusObject)
CAR_OBJECT_IMPL(CBasicService)

ECode CService::BasicService::constructor(
    /* [in] */ IObject* host)
{
    mHost = (CService*)host;
    return NOERROR;
}

ECode CService::BasicService::Cat(
    /* [in] */ const String& inStr1,
    /* [in] */ const String& inStr2,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    SendUiMessage(MESSAGE_CAT, inStr1 + String(" and ") + inStr2);

    /* Simply echo the cat message. */
    SendUiMessage(MESSAGE_CAT_REPLY, inStr1 + inStr2);
    *result = inStr1 + inStr2;
    return NOERROR;
}

void CService::BasicService::SendUiMessage(
    /* [in] */ Int32 what,
    /* [in] */ const String& str)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(what, CoreUtils::Convert(str), (IMessage**)&msg);
    Boolean result;
    mHost->mHandler->SendMessage(msg, &result);
}


//======================================================================
//  CService::BusHandler::InnerSessionPortListener
//======================================================================
ECode CService::BusHandler::InnerSessionPortListener::AcceptSessionJoiner(
    /* [in] */ Int16 sessionPort,
    /* [in] */ const String& joiner,
    /* [in] */ ISessionOpts* opts,
    /* [out] */ Boolean* accepted)
{
    VALIDATE_NOT_NULL(accepted);
    if (sessionPort == CONTACT_PORT) {
        *accepted = TRUE;
    }
    else {
        *accepted = FALSE;
    }
    return NOERROR;
}


//======================================================================
//  CService::BusHandler
//======================================================================
const Int32 CService::BusHandler::CONNECT = 1;
const Int32 CService::BusHandler::DISCONNECT = 2;
const String CService::BusHandler::SERVICE_NAME("org.alljoyn.Bus.sample");
const Int16 CService::BusHandler::CONTACT_PORT = 25;

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
    case CONNECT: {
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
         * To make a service available to other AllJoyn peers, first register a BusObject with
         * the BusAttachment at a specific path.
         *
         * Our service is the BasicService BusObject at the "/sample" path.
         */
        ECode ec = mBus->RegisterBusObject(mHost->mBasicService, String("/sample"));
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
         * Create a new session listening on the contact port of the chat service.
         */
        AutoPtr<IMutableInteger16Value> contactPort;
        CMutableInteger16Value::New(CONTACT_PORT, (IMutableInteger16Value**)&contactPort);

        AutoPtr<ISessionOpts> sessionOpts;
        CSessionOpts::New((ISessionOpts**)&sessionOpts);
        sessionOpts->SetTraffic(ISessionOpts::TRAFFIC_MESSAGES);
        sessionOpts->SetMultipoint(FALSE);
        sessionOpts->SetProximity(ISessionOpts::PROXIMITY_ANY);
        sessionOpts->SetTransports(ISessionOpts::TRANSPORT_ANY);

        AutoPtr<ISessionPortListener> spl = new InnerSessionPortListener(this);
        ec = mBus->BindSessionPort(contactPort, sessionOpts, spl);
        Int16 value;
        contactPort->GetValue(&value);
        mHost->LogStatus(String::Format("BusAttachment.bindSessionPort(%d, %s)", value, TO_CSTR(sessionOpts)), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * request a well-known name from the bus
         */
        Int32 flag = IBusAttachment::ALLJOYN_REQUESTNAME_FLAG_REPLACE_EXISTING |
                IBusAttachment::ALLJOYN_REQUESTNAME_FLAG_DO_NOT_QUEUE;

        ec = mBus->RequestName(SERVICE_NAME, flag);
        mHost->LogStatus(String::Format("BusAttachment.requestName(%s, 0x%08x)", SERVICE_NAME.string(), flag), ec);
        if (ec == (ECode)E_STATUS_OK) {
            /*
             * If we successfully obtain a well-known name from the bus
             * advertise the same well-known name
             */
            Int16 transports;
            sessionOpts->GetTransports(&transports);
            ec = mBus->AdvertiseName(SERVICE_NAME, transports);
            mHost->LogStatus(String::Format("BusAttachement.advertiseName(%s)", SERVICE_NAME.string()), ec);
            if (ec != (ECode)E_STATUS_OK) {
                /*
                 * If we are unable to advertise the name, release
                 * the well-known name from the local bus.
                 */
                ec = mBus->ReleaseName(SERVICE_NAME);
                mHost->LogStatus(String::Format("BusAttachment.releaseName(%s)", SERVICE_NAME.string()), ec);
                mHost->Finish();
                return NOERROR;
            }
        }

        break;
    }
    /* Release all resources acquired in connect. */
    case DISCONNECT: {
        /*
         * It is important to unregister the BusObject before disconnecting from the bus.
         * Failing to do so could result in a resource leak.
         */
        mBus->UnregisterBusObject(mHost->mBasicService);
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
const String CService::TAG("BasicService");
const Int32 CService::MESSAGE_CAT;
const Int32 CService::MESSAGE_CAT_REPLY;
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

    mListViewArrayAdapter->Add(CoreUtils::Convert(String("Service is running...")));

    /* Make all AllJoyn calls through a separate handler thread to prevent blocking the UI. */
    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BusHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBusHandler = new BusHandler();
    mBusHandler->constructor(looper, this);

    /* Start our service. */
    AutoPtr<IBasicInterface> bi;
    CBasicService::New(this, (IBasicInterface**)&bi);
    mBasicService = (BasicService*)bi.Get();
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::CONNECT, &result);
    return NOERROR;
}

ECode CService::OnResume()
{
    return Activity::OnResume();
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

ECode CService::OnStop()
{
    return Activity::OnStop();
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

} // namespace BasicService
} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos
