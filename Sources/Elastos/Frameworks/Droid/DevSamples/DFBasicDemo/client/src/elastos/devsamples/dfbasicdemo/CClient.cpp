
#include "CClient.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IProgressDialogHelper;
using Elastos::Droid::App::CProgressDialogHelper;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

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

//======================================================================
//  CClient::MyHander
//======================================================================
CClient::MyHandler::MyHandler(
    /* [in] */ CClient* host)
    : mHost(host)
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
ECode CClient::BusHandler::InnerBusListener::FoundAdvertisedName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transport,
    /* [in] */ const String& namePrefix)
{
    Logger::I("CClient", "MyBusListener.foundAdvertisedName(%s, 0x%04x, %s)",
            name.string(), transport, namePrefix.string());
    /*
     * This client will only join the first service that it sees advertising
     * the indicated well-known name.  If the program is already a member of
     * a session (i.e. connected to a service) we will not attempt to join
     * another session.
     * It is possible to join multiple session however joining multiple
     * sessions is not shown in this sample.
     */
    if (!mHost->mIsConnected) {
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(JOIN_SESSION, (IMessage**)&msg);
        msg->SetArg1(transport);
        msg->SetObj(CoreUtils::Convert(name));
        Boolean result;
        mHost->SendMessage(msg, &result);
    }
    return NOERROR;
}


//======================================================================
//  CClient::BusHandler::InnerSessionListener
//======================================================================
ECode CClient::BusHandler::InnerSessionListener::SessionLost(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 reason)
{
    mHost->mIsConnected = FALSE;
    Logger::I("CClient", "MyBusListener.sessionLost(sessionId = %d, reason = %d)", sessionId,reason);
    Boolean result;
    mHost->mHost->mHandler->SendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG, &result);
    return NOERROR;
}


//======================================================================
//  CClient::BusHandler
//======================================================================
const Int32 CClient::BusHandler::CONNECT = 1;
const Int32 CClient::BusHandler::JOIN_SESSION = 2;
const Int32 CClient::BusHandler::DISCONNECT = 3;
const Int32 CClient::BusHandler::CAT = 4;
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
    mHost = host;
    return NOERROR;
}

ECode CClient::BusHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
    /* Connect to a remote instance of an object implementing the BasicInterface. */
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
        AutoPtr<IBusListener> bl = new InnerBusListener(this);
        mBus->RegisterBusListener(bl);

        /* To communicate with AllJoyn objects, we must connect the BusAttachment to the bus. */
        ECode ec = mBus->Connect();
        Logger::I("CClient", "BusAttachment.connect() 0x%08x", ec);
        if ((ECode)E_STATUS_OK != ec) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * Now find an instance of the AllJoyn object we want to call.  We start by looking for
         * a name, then connecting to the device that is advertising that name.
         *
         * In this case, we are looking for the well-known SERVICE_NAME.
         */
        ec = mBus->FindAdvertisedName(SERVICE_NAME);
        Logger::I("CClient", "BusAttachement.findAdvertisedName(%s) 0x%08x", SERVICE_NAME.string(), ec);
        if ((ECode)E_STATUS_OK != ec) {
            mHost->Finish();
            return NOERROR;
        }

        break;
    }
    case (JOIN_SESSION): {
        /*
         * If discovery is currently being stopped don't join to any other sessions.
         */
        if (mIsStoppingDiscovery) {
            break;
        }

        /*
         * In order to join the session, we need to provide the well-known
         * contact port.  This is pre-arranged between both sides as part
         * of the definition of the chat service.  As a result of joining
         * the session, we get a session identifier which we must use to
         * identify the created session communication channel whenever we
         * talk to the remote side.
         */
        Int16 contactPort = CONTACT_PORT;
        AutoPtr<ISessionOpts> sessionOpts;
        CSessionOpts::New((ISessionOpts**)&sessionOpts);
        Int32 arg1;
        msg->GetArg1(&arg1);
        sessionOpts->SetTransports(arg1);
        AutoPtr<IMutableInteger32Value> sessionId;
        CMutableInteger32Value::New((IMutableInteger32Value**)&sessionId);

        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<ISessionListener> sl = new InnerSessionListener(this);
        ECode ec = mBus->JoinSession(CoreUtils::Unbox(ICharSequence::Probe(obj)),
                contactPort, sessionId, sessionOpts, sl);
        Int32 value;
        sessionId->GetValue(&value);
        Logger::I("CClient", "BusAttachment.joinSession() - sessionId: %d, 0x%08x", value, ec);

        if (ec == (ECode)E_STATUS_OK) {
            /*
             * To communicate with an AllJoyn object, we create a ProxyBusObject.
             * A ProxyBusObject is composed of a name, path, sessionID and interfaces.
             *
             * This ProxyBusObject is located at the well-known SERVICE_NAME, under path
             * "/sample", uses sessionID of CONTACT_PORT, and implements the BasicInterface.
             */
            AutoPtr< ArrayOf<InterfaceID> > busInterfaces = ArrayOf<InterfaceID>::Alloc(1);
            (*busInterfaces)[0] = EIID_IBasicInterface;
            mProxyObj = NULL;
            mBus->GetProxyBusObject(SERVICE_NAME, String("/sample"),
                    value, busInterfaces, (IProxyBusObject**)&mProxyObj);

            /* We make calls to the methods of the AllJoyn object through one of its interfaces. */
            mBasicInterface = NULL;
            mProxyObj->GetInterface(EIID_IBasicInterface, (IInterface**)&mBasicInterface);

            mSessionId = value;
            mIsConnected = TRUE;
            Boolean result;
            mHost->mHandler->SendEmptyMessage(MESSAGE_STOP_PROGRESS_DIALOG, &result);
        }
        break;
    }

    /* Release all resources acquired in the connect. */
    case DISCONNECT: {
        mIsStoppingDiscovery = TRUE;
        if (mIsConnected) {
            ECode ec = mBus->LeaveSession(mSessionId);
            Logger::I("CClient", "BusAttachment.leaveSession() 0x%08x", ec);
        }
        mBus->Disconnect();
        AutoPtr<ILooper> looper;
        GetLooper((ILooper**)&looper);
        looper->Quit();
        break;
    }

    /*
     * Call the service's Cat method through the ProxyBusObject.
     *
     * This will also print the String that was sent to the service and the String that was
     * received from the service to the user interface.
     */
    case CAT: {
//        try {
        if (mBasicInterface != NULL) {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String str = CoreUtils::Unbox(ICharSequence::Probe(obj));
            SendUiMessage(MESSAGE_PING, str + String(" and ") + str);
            String reply;
            mBasicInterface->Cat(str, str, &reply);
            SendUiMessage(MESSAGE_PING_REPLY, reply);
        }
//        } catch (BusException ex) {
//            logException("BasicInterface.cat()", ex);
//        }
        break;
    }
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
const String CClient::TAG("BasicClient");

ECode CClient::constructor()
{
    mHandler = new MyHandler(this);
    return NOERROR;
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
    mBusHandler->SendEmptyMessage(BusHandler::CONNECT, &result);
    mHandler->SendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG, &result);
    return NOERROR;
}

} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos
