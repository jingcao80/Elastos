
#include "CClient.h"
#include "R.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "ecCore.h"
#include "ecRdt.h"
#include "ecSession.h"

using Elastos::Droid::App::IProgressDialogHelper;
using Elastos::Droid::App::CProgressDialogHelper;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

using Org::Alljoyn::Bus::IVariant;
using Org::Alljoyn::Bus::CVariant;
using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CMutableInteger32Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger32Value;
using Org::Alljoyn::Bus::ISessionListener;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {
namespace RemoteClient {

//======================================================================
//  CClient::MyHander
//======================================================================
CClient::MyHandler::MyHandler(
    /* [in] */ CClient* host)
    : Handler(FALSE)
    , mHost(host)
{}

ECode CClient::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
    case MESSAGE_PING: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String cat = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Cat args:  ") + cat));
        break;
    }
    case MESSAGE_PING_REPLY: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String ret = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Reply:  ") + ret));
        ITextView::Probe(mHost->mEditText)->SetText(CoreUtils::Convert(String("")));
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
    case MESSAGE_START_PROGRESS_DIALOG: {
        AutoPtr<IProgressDialogHelper> helper;
        CProgressDialogHelper::AcquireSingleton((IProgressDialogHelper**)&helper);
        helper->Show(mHost, CoreUtils::Convert(String("")),
                CoreUtils::Convert(String("Finding Basic Service.\nPlease wait...")),
                TRUE, TRUE, (IProgressDialog**)&mHost->mDialog);
        break;
    }
    case MESSAGE_STOP_PROGRESS_DIALOG:
        IDialogInterface::Probe(mHost->mDialog)->Dismiss();
        break;
    default:
        break;
    }
    return NOERROR;
}

ECode CClient::MyHandler::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CClient::MyHandler";
    return NOERROR;
}


//======================================================================
//  CClient::EditorListener
//======================================================================
CAR_INTERFACE_IMPL(CClient::EditorListener, Object, IOnEditorActionListener);

CClient::EditorListener::EditorListener(
    /* [in] */ CClient* host)
    : mHost(host)
{}

ECode CClient::EditorListener::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 action;
    if (actionId == IEditorInfo::IME_NULL
        && (event->GetAction(&action), action == IKeyEvent::ACTION_UP)) {
        Logger::I("CClient", "Call the remote object's cat method.");
        /* Call the remote object's cat method. */
        AutoPtr<ICharSequence> text;
        v->GetText((ICharSequence**)&text);
        AutoPtr<IMessage> msg;
        mHost->mBusHandler->ObtainMessage(BusHandler::CAT, text, (IMessage**)&msg);
        mHost->mBusHandler->SendMessage(msg, result);
    }
    *result = TRUE;
    return NOERROR;
}


//======================================================================
//  CClient::BusHandler::InnerBusListener
//======================================================================
// ECode CClient::BusHandler::InnerBusListener::FoundAdvertisedName(
//     /* [in] */ const String& name,
//     /* [in] */ Int16 transport,
//     /* [in] */ const String& namePrefix)
// {
//     Logger::I("CClient", "MyBusListener.foundAdvertisedName(%s, 0x%04x, %s)",
//             name.string(), transport, namePrefix.string());
//     /*
//      * This client will only join the first service that it sees advertising
//      * the indicated well-known name.  If the program is already a member of
//      * a session (i.e. connected to a service) we will not attempt to join
//      * another session.
//      * It is possible to join multiple session however joining multiple
//      * sessions is not shown in this sample.
//      */
//     if (!mHost->mIsConnected) {
//         AutoPtr<IMessage> msg;
//         mHost->ObtainMessage(JOIN_SESSION, (IMessage**)&msg);
//         msg->SetArg1(transport);
//         msg->SetObj(CoreUtils::Convert(name));
//         Boolean result;
//         mHost->SendMessage(msg, &result);
//     }
//     return NOERROR;
// }


// //======================================================================
// //  CClient::BusHandler::InnerSessionListener
// //======================================================================
// ECode CClient::BusHandler::InnerSessionListener::SessionLost(
//     /* [in] */ Int32 sessionId,
//     /* [in] */ Int32 reason)
// {
//     mHost->mIsConnected = FALSE;
//     Logger::I("CClient", "MyBusListener.sessionLost(sessionId = %d, reason = %d)", sessionId,reason);
//     Boolean result;
//     mHost->mHost->mHandler->SendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG, &result);
//     return NOERROR;
// }


//======================================================================
//  CClient::BusHandler
//======================================================================

static AutoPtr<CClient::BusHandler> sHandler;
static const char* sDeviceID = "cb6f60d7d139cce13a6c99daaadcacbd";
static const char* sPassWord = "123456";
static const char* sAppID = "3DF1AC53C8BE42EFB14426DECEA2BFDD";
static const char* sClientID = "529FD46DFA82484CAF3A8CDE45BE8B0A";
static const char* sClientKey = "7BDCBA1612E64B06B3B19C929CEC3F34";

static int setPreference(const char *key, const char *value)
{
    Logger::D("Client", "setPreference key = %s, value = %s", key, value);
    return 0;
}

static int getPreference(const char *key, char *value, int length)
{
    if (!strcmp(key, "app.Id") || !strcmp(key, "app.key"))
        strcpy(value, sAppID);
    else if (!strcmp(key, "client.Id"))
        strcpy(value, sClientID);
    else if (!strcmp(key, "client.key"))
        strcpy(value, sClientKey);
    else if (!strcmp(key, "carrier.server.host") || !strcmp(key, "api.carrier.server.host"))
        return -1;

    Logger::D("Client", "getPreference key = %s, value = %s", key, value);
    return 0;
}

static void onOnline(ecContext* ctxt)
{
    Logger::D("Client", "Turn server address:%s:%d\n", ctxt->turnServer, ctxt->turnPort);
    Logger::D("Client", "onOnline called\n");
    Boolean result;
    sHandler->SendEmptyMessage(CClient::BusHandler::PAIR, &result);

    return;
}

static void onOffline(int status)
{
    Logger::D("Client", "onOffline called\n");
    return;
}

static void onPairRequest(const char *device_id,
                     const char *device_name,
                     const char *service_id,
                     const char *client_id,
                     const char *client_name,
                     const char *hello_msg)
{
    Logger::D("Client", "\033[41;37m  Get a pair request for a device from other client."
           "device id is %s, device name is %s, "
           "client_id is %s, client name is %s, "
           "hello message is %s.\033[0m\n",
           device_id, device_name, client_id, client_name, hello_msg);
    return;
}

static void onPairConfirmed(const char *device_id, const char *service_id, int mode, int status)
{
    Logger::D("Client", "\033[41;37m The host client confirmed the pair request "
           "of this client.device id is %s.\n.mode is %d, ec is %d.\033[0m\n",
           device_id, mode, status);
    return;
}

static void onDeviceEvent(const char *deviceId, int event)
{
    char* sevent = "Unknown Event";
    switch(event) {
    case DEVICE_EVENT_ONLINE:         sevent = "Online" ;        break;
    case DEVICE_EVENT_OFFLINE:        sevent = "Offline";        break;
    case DEVICE_EVENT_PAIRING_DENIED: sevent = "Pairing Denied"; break;
    case DEVICE_EVENT_HOST_PAIRED:    sevent = "Host Paired";    break;
    case DEVICE_EVENT_GUEST_PAIRED:   sevent = "Guest Paired";   break;
    default: break;
    }
    Logger::D("Client", "Device (%s) event (%s) received.\n", deviceId, sevent);
    return ;
}

static void onMessage(const char* serviceId, const char* deviceId, void* message, int length)
{
    Logger::D("Client", "onMessage called\n");
    return ;
}

static ecClientHandler gClientHandler = {
    .setPreference   = setPreference,
    .getPreference   = getPreference,
    .onOnline        = onOnline,
    .onOffline       = onOffline,
    .onPairRequest   = onPairRequest,
    .onPairConfirmed = onPairConfirmed,
    .onDeviceEvent   = onDeviceEvent,
    .onMessage       = onMessage,
};

static void onSessionData(int sessionId, int channelId, void* data, int length)
{
    if (length <= 0) {
        return;
    }
    sHandler->mCSessionId = sessionId;
    sHandler->mChannelId = channelId;
    Logger::D("Client", "Received data on channel(%d:%d): %s\n", sessionId, channelId, (char*)data);
    String strData((char*)data, length);
    AutoPtr<IMessage> msg;
    sHandler->ObtainMessage(CClient::BusHandler::RECEIVE_DATA, CoreUtils::Convert(strData), (IMessage**)&msg);
    Boolean result;
    sHandler->SendMessage(msg, &result);
    return ;
}

static void onSessionClosed(int sessionId, int status)
{
    Logger::D("Client", "Session %d closed, status: %d.\n", sessionId, status);
    return ;
}

static
ecPeerSessionHandler gPeerSessionHandler = {
    .onData   = onSessionData,
    .onClosed = onSessionClosed
};

static int onSessionInvite(const char* serviceId, const char* peerId)
{
    Logger::D("Client", "onInvite called\n");
    return 1;
}

static ecPeerSessionHandler* onSessionOpened(int sessionId, int nChannels)
{
    Logger::D("Client", "onSessionOpened called\n");
    return &gPeerSessionHandler;
}

static ecPeerSessionInitializer gPeerInitializer = {
    .onInvite        = onSessionInvite,
    .onSessionOpened = onSessionOpened
};

static void onRdtData(int rdtId, void* data, int length)
{
    if (length <= 0) {
        return ;
    }

    Logger::D("Client", "Receved data on Rdt tunnel(%d):%s\n", rdtId, (char*)data);
    return ;
}

static void onRdtClosed(int rdtId, int status)
{
    Logger::D("Client", "Rdt tunnel %d closed, status:%d\n", rdtId, status);
    return ;
}

static ecRdtHandler gRdtHandler = {
    .onData   = onRdtData,
    .onClosed = onRdtClosed
};

static ecRdtHandler* onRdtOpened(int sessionId, int channelId, int rdtId)
{
    Logger::D("Client", "onRdtOpened called (rdtId:%d)\n", rdtId);
    return &gRdtHandler;
}

static ecRdtInitializer gRdtInitializer = {
    .onRdtOpened = onRdtOpened
};

const Int32 CClient::BusHandler::CONNECT = 1;
const Int32 CClient::BusHandler::JOIN_SESSION = 2;
const Int32 CClient::BusHandler::DISCONNECT = 3;
const Int32 CClient::BusHandler::CAT = 4;
const Int32 CClient::BusHandler::CONNECT_CARRIER = 5;
const Int32 CClient::BusHandler::PAIR = 6;
const Int32 CClient::BusHandler::RECEIVE_DATA = 7;
const Int32 CClient::BusHandler::DISCONNECT_CARRIER = 8;

const String CClient::BusHandler::SERVICE_NAME("org.alljoyn.Bus.sample");
const Int16 CClient::BusHandler::CONTACT_PORT = 25;

ECode CClient::BusHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ CClient* host)
{
    Handler::constructor(looper);

    mSessionId = 0;
    mIsInASession = FALSE;
    mIsConnected = FALSE;
    mIsStoppingDiscovery = FALSE;

    mCSessionId = 0;
    mChannelId = 1;
    mHost = host;
    return NOERROR;
}

ECode CClient::BusHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
    case CONNECT_CARRIER: {
        sHandler = this;
        int ret = 0;
        ret = ecClientStart(&gClientHandler);
        if (ret < 0) {
            Logger::E(TAG, "[Client] ecClientStart failed (err:%s)\n", ecGetErrString(ret));
            return NOERROR;
        }
        ret = ecPeerInitialize(MAX_SESSION_NUM, &gPeerInitializer);
        if (ret < 0) {
            Logger::E(TAG, "[Client] ecPeerInitialize failed!");
            ecClientStop();
            return NOERROR;
        }
        ret = ecRdtModuleInitialize(&gRdtInitializer);
        if (ret < 0) {
            Logger::E(TAG, "[Client] ecRdtModuleInitialize failed (err:0x%x)\n", ret);
            ecPeerDestroy();
            ecClientStop();
            return NOERROR;
        }
        break;
    }
    case PAIR: {
        int alreadyPaired = 0;
        int ec = 0;

        ec = ecPair(sDeviceID, NULL, SUBSCRIBER_MODE_HOST, sPassWord, &alreadyPaired);
        if (ec < 0) {
            Logger::E("Client", "ecPair failed (err:%s)\n", ecGetErrString(ec));
            return NOERROR;
        }
        if (alreadyPaired) {
            Logger::D("Client", "\033[41;37m Already paired!!\033[0m\n");
        }
        else {
            Logger::D("Client", "\033[41;37m Pair device OK!!\033[0m\n");
        }

        Int32 sessionId = ecSessionOpen(sDeviceID, NULL,  1, &gPeerSessionHandler);
        if (sessionId < 0) {
            Logger::E("Client", "ecSessionOpen failed (err:%s)\n", ecSessionGetErrString(sessionId));
            return NOERROR;
        }
        mCSessionId = sessionId;
        Boolean result;
        mHost->mHandler->SendEmptyMessage(MESSAGE_STOP_PROGRESS_DIALOG, &result);
        Logger::D("Client", "\033[41;37m Connect device OK! (sid: %d)\033[0m\n", sessionId);
        break;
    }
    case RECEIVE_DATA: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String str = CoreUtils::Unbox(ICharSequence::Probe(obj));
        Logger::D("Client", "RECEIVE_DATA %s", str.string());
        break;
    }
    case CAT: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String str = CoreUtils::Unbox(ICharSequence::Probe(obj));
        SendUiMessage(MESSAGE_PING, str + String(" and ") + str);

        Int32 ec = ecSessionWrite(mCSessionId, mChannelId, str.string(), str.GetLength() + 1);
        if (ec < 0) {
            Logger::E("Client", "ecSessionWrite failed (err:%s)\n", ecSessionGetErrString(ec));
            return NOERROR;
        }
        Logger::E("Client", "\033[41;37m Session Write OK!\033[0m\n");

        // String reply;
        // mBasicInterface->Cat(str, str, &reply);
        // SendUiMessage(MESSAGE_PING_REPLY, reply);
        break;
    }
    case DISCONNECT_CARRIER: {
        ecRdtModuleDestroy();
        ecPeerDestroy();
        ecClientStop();
        Logger::D("Client", "DISCONNECT_CARRIER");
        break;
    }
    /* Connect to a remote instance of an object implementing the BasicInterface. */
//     case CONNECT: {
//         AutoPtr<IContext> context;
//         mHost->GetApplicationContext((IContext**)&context);
//         AutoPtr<IDaemonInit> di;
//         CDaemonInit::AcquireSingleton((IDaemonInit**)&di);
//         Boolean result;
//         di->PrepareDaemon(context, &result);
//         /*
//          * All communication through AllJoyn begins with a BusAttachment.
//          *
//          * A BusAttachment needs a name. The actual name is unimportant except for internal
//          * security. As a default we use the class name as the name.
//          *
//          * By default AllJoyn does not allow communication between devices (i.e. bus to bus
//          * communication). The second argument must be set to Receive to allow communication
//          * between devices.
//          */
//         String pName;
//         mHost->GetPackageName(&pName);
//         mBus = NULL;
//         CBusAttachment::New(pName, RemoteMessage_Receive, (IBusAttachment**)&mBus);

//         /*
//          * Create a bus listener class
//          */
//         AutoPtr<IBusListener> bl = new InnerBusListener(this);
//         mBus->RegisterBusListener(bl);

//         /* To communicate with AllJoyn objects, we must connect the BusAttachment to the bus. */
//         ECode ec = mBus->Connect();
//         mHost->LogStatus(String("BusAttachment.connect()"), ec);
//         if ((ECode)E_STATUS_OK != ec) {
//             mHost->Finish();
//             return NOERROR;
//         }

//         /*
//          * Now find an instance of the AllJoyn object we want to call.  We start by looking for
//          * a name, then connecting to the device that is advertising that name.
//          *
//          * In this case, we are looking for the well-known SERVICE_NAME.
//          */
//         ec = mBus->FindAdvertisedName(SERVICE_NAME);
//         String strMsg("");
//         strMsg.AppendFormat("BusAttachement.findAdvertisedName(%s)", SERVICE_NAME.string());
//         mHost->LogStatus(strMsg, ec);
//         if ((ECode)E_STATUS_OK != ec) {
//             mHost->Finish();
//             return NOERROR;
//         }

//         break;
//     }
//     case JOIN_SESSION: {
//         /*
//          * If discovery is currently being stopped don't join to any other sessions.
//          */
//         if (mIsStoppingDiscovery) {
//             break;
//         }

//         /*
//          * In order to join the session, we need to provide the well-known
//          * contact port.  This is pre-arranged between both sides as part
//          * of the definition of the chat service.  As a result of joining
//          * the session, we get a session identifier which we must use to
//          * identify the created session communication channel whenever we
//          * talk to the remote side.
//          */
//         Int16 contactPort = CONTACT_PORT;
//         AutoPtr<ISessionOpts> sessionOpts;
//         CSessionOpts::New((ISessionOpts**)&sessionOpts);
//         Int32 arg1;
//         msg->GetArg1(&arg1);
//         sessionOpts->SetTransports(arg1);
//         AutoPtr<IMutableInteger32Value> sessionId;
//         CMutableInteger32Value::New((IMutableInteger32Value**)&sessionId);

//         AutoPtr<IInterface> obj;
//         msg->GetObj((IInterface**)&obj);
//         AutoPtr<ISessionListener> sl = new InnerSessionListener(this);
//         ECode ec = mBus->JoinSession(CoreUtils::Unbox(ICharSequence::Probe(obj)),
//                 contactPort, sessionId, sessionOpts, sl);
//         Int32 value;
//         sessionId->GetValue(&value);
//         String strMsg("");
//         strMsg.AppendFormat("BusAttachment.joinSession() - sessionId: %d", value);
//         mHost->LogStatus(strMsg, ec);

//         if (ec == (ECode)E_STATUS_OK) {
//             /*
//              * To communicate with an AllJoyn object, we create a ProxyBusObject.
//              * A ProxyBusObject is composed of a name, path, sessionID and interfaces.
//              *
//              * This ProxyBusObject is located at the well-known SERVICE_NAME, under path
//              * "/sample", uses sessionID of CONTACT_PORT, and implements the BasicInterface.
//              */
//             AutoPtr<IClassLoader> loader;
//             mHost->GetClassLoader((IClassLoader**)&loader);
//             AutoPtr<IInterfaceInfo> itfcInfo;
//             loader->LoadInterface(String("Elastos.DevSamples.DFBasicDemo.RemoteClient.IBasicInterface"),
//                     (IInterfaceInfo**)&itfcInfo);
//             AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
//             busInterfaces->Set(0, itfcInfo);
//             mProxyObj = NULL;
//             mBus->GetProxyBusObject(SERVICE_NAME, String("/sample"),
//                     value, busInterfaces, (IProxyBusObject**)&mProxyObj);

//             /* We make calls to the methods of the AllJoyn object through one of its interfaces. */
//             mBasicInterface = NULL;
//             mProxyObj->GetInterface(EIID_IBasicInterface, (IInterface**)&mBasicInterface);

//             mSessionId = value;
//             mIsConnected = TRUE;
//             Boolean result;
//             mHost->mHandler->SendEmptyMessage(MESSAGE_STOP_PROGRESS_DIALOG, &result);
//         }
//         break;
//     }

//     /* Release all resources acquired in the connect. */
//     case DISCONNECT: {
//         mIsStoppingDiscovery = TRUE;
//         if (mIsConnected) {
//             ECode ec = mBus->LeaveSession(mSessionId);
//             mHost->LogStatus(String("BusAttachment.leaveSession()"), ec);
//         }
//         mBus->Disconnect();
//         AutoPtr<ILooper> looper;
//         GetLooper((ILooper**)&looper);
//         looper->Quit();
//         break;
//     }

//     /*
//      * Call the service's Cat method through the ProxyBusObject.
//      *
//      * This will also print the String that was sent to the service and the String that was
//      * received from the service to the user interface.
//      */
//     case CAT: {
// //        try {
//         if (mBasicInterface != NULL) {
//             ECode ec = NOERROR;
//             AutoPtr<IInterface> obj;
//             msg->GetObj((IInterface**)&obj);
//             String str = CoreUtils::Unbox(ICharSequence::Probe(obj));
//             SendUiMessage(MESSAGE_PING, str + String(" and ") + str);
//             String reply;
//             mBasicInterface->Cat(str, str, &reply);
//             SendUiMessage(MESSAGE_PING_REPLY, reply);

// //        } catch (BusException ex) {
// //            logException("BasicInterface.cat()", ex);
// //        }
//         break;
//     }
    default:
        break;
    }
    return NOERROR;
}

void CClient::BusHandler::SendUiMessage(
    /* [in] */ Int32 what,
    /* [in] */ const String& str)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(what, CoreUtils::Convert(str), (IMessage**)&msg);
    Boolean result;
    mHost->mHandler->SendMessage(msg, &result);
}

ECode CClient::BusHandler::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CClient::BusHandler";
    return NOERROR;
}

//======================================================================
//  CClient
//======================================================================
const Int32 CClient::MESSAGE_PING;
const Int32 CClient::MESSAGE_PING_REPLY;
const Int32 CClient::MESSAGE_POST_TOAST;
const Int32 CClient::MESSAGE_START_PROGRESS_DIALOG;
const Int32 CClient::MESSAGE_STOP_PROGRESS_DIALOG;
const String CClient::TAG("RemoteClient");

ECode CClient::constructor()
{
    mHandler = new MyHandler(this);
    return Activity::constructor();
}

ECode CClient::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    SetContentView(R::layout::main);

    CArrayAdapter::New((IContext*)this, R::layout::message, (IArrayAdapter**)&mListViewArrayAdapter);
    AutoPtr<IView> v;
    FindViewById(R::id::ListView, (IView**)&v);
    mListView = IListView::Probe(v);
    IAdapterView::Probe(mListView)->SetAdapter(IAdapter::Probe(mListViewArrayAdapter));

    v = NULL;
    FindViewById(R::id::EditText, (IView**)&v);
    mEditText = IEditText::Probe(v);
    AutoPtr<IOnEditorActionListener> editorListener = new EditorListener(this);
    ITextView::Probe(mEditText)->SetOnEditorActionListener(editorListener);

    /* Make all AllJoyn calls through a separate handler thread to prevent blocking the UI. */
    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BusHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBusHandler = new BusHandler();
    mBusHandler->constructor(looper, this);

    /* Connect to an AllJoyn object. */
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::CONNECT_CARRIER, &result);
    mHandler->SendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG, &result);
    return NOERROR;
}

ECode CClient::OnCreateOptionsMenu(
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

ECode CClient::OnOptionsItemSelected(
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

ECode CClient::OnDestroy()
{
    Activity::OnDestroy();

    /* Disconnect to prevent resource leaks. */
    Boolean result;
    // mBusHandler->SendEmptyMessage(BusHandler::DISCONNECT, &result);
    mBusHandler->SendEmptyMessage(BusHandler::DISCONNECT_CARRIER, &result);

    return NOERROR;
}

void CClient::LogStatus(
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

void CClient::LogException(
    /* [in] */ const String& msg,
    /* [in] */ ECode ex)
{
    String log("");
    log.AppendFormat("%s: 0x%08x", msg.string(), ex);
    AutoPtr<IMessage> toastMsg;
    mHandler->ObtainMessage(MESSAGE_POST_TOAST, CoreUtils::Convert(log), (IMessage**)&toastMsg);
    Boolean result;
    mHandler->SendMessage(toastMsg, &result);
    Logger::E(TAG, log.string());
}

ECode CClient::GetClassID(
    /* [out] */ ClassID* clsid)
{
    VALIDATE_NOT_NULL(clsid);
    *clsid = ECLSID_CClient;
    return NOERROR;
}

} // namespace RemoteClient
} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos
