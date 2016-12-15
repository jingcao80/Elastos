
#include "CClient.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/R.h>

using Elastos::Droid::App::CProgressDialogHelper;
using Elastos::Droid::App::IProgressDialogHelper;
using Elastos::Droid::App::IProgressDialog;
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
using Elastos::IO::CFileDescriptor;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFlushable;
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
namespace DFRawDemo {
namespace RawClient {

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
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Ping:  ") + cat));
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
                CoreUtils::Convert(String("Finding Raw Service.\nPlease wait...")),
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
        AutoPtr<ICharSequence> text;
        v->GetText((ICharSequence**)&text);
        AutoPtr<IMessage> msg;
        mHost->mBusHandler->ObtainMessage(BusHandler::SEND_RAW, text, (IMessage**)&msg);
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
    if (name.Equals(SERVICE_NAME)) {
        mHost->mHaveServiceName = TRUE;
    }
    if (name.Equals(RAW_SERVICE_NAME)) {
        mHost->mHaveRawServiceName = TRUE;
    }

    if (mHost->mHaveServiceName && mHost->mHaveRawServiceName) {
        /*
         * This client will only join the first service that it sees advertising
         * the indicated well-known names.  If the program is already a member of
         * a session (i.e. connected to a service) we will not attempt to join
         * another session.
         * It is possible to join multiple session however joining multiple
         * sessions is not shown in this sample.
         */
        if(!mHost->mIsConnected){
            Boolean result;
            mHost->SendEmptyMessage(BusHandler::JOIN_SESSION, &result);
        }
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
const Int32 CClient::BusHandler::SEND_RAW = 4;
const String CClient::BusHandler::SERVICE_NAME("org.alljoyn.bus.samples.raw");
const Int16 CClient::BusHandler::CONTACT_PORT = 88;
const String CClient::BusHandler::RAW_SERVICE_NAME("org.alljoyn.bus.samples.yadda888");

ECode CClient::BusHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ CClient* host)
{
    Handler::constructor(looper);

    mHaveServiceName = FALSE;
    mHaveRawServiceName = FALSE;
    mMsgSessionId = -1;
    mRawSessionId = -1;
    mIsConnected = FALSE;
    mIsStoppingDiscovery = FALSE;
    mStreamUp = FALSE;
    mHost = host;
    return NOERROR;
}

ECode CClient::BusHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
    /* Connect to a remote instance of an object implementing the RawInterface. */
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
         * If using the debug version of the AllJoyn libraries, tell
         * them to write debug output to the OS log so we can see it
         * using adb logcat.  Turn on all of the debugging output from
         * the Java language bindings (module ALLJOYN_JAVA).
         */
        mBus->UseOSLogging(TRUE);
        mBus->SetDebugLevel(String("ALLJOYN_JAVA"), 7);

        /*
         * Create a bus listener class
         */
        AutoPtr<IBusListener> bl = new InnerBusListener(this);
        mBus->RegisterBusListener(bl);

        /* To communicate with AllJoyn objects, we must connect the BusAttachment to the bus. */
        ECode ec = mBus->Connect();
        mHost->LogStatus(String("BusAttachment.connect()"), ec);
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
        String strMsg("");
        strMsg.AppendFormat("BusAttachement.findAdvertisedName(%s)", SERVICE_NAME.string());
        mHost->LogStatus(strMsg, ec);
        if ((ECode)E_STATUS_OK != ec) {
            mHost->Finish();
            return NOERROR;
        }

        ec = mBus->FindAdvertisedName(RAW_SERVICE_NAME);
        strMsg = "";
        strMsg.AppendFormat("BusAttachement.findAdvertisedName(%s)", RAW_SERVICE_NAME.string());
        mHost->LogStatus(strMsg, ec);
        if ((ECode)E_STATUS_OK != ec) {
            mHost->Finish();
            return NOERROR;
        }

        break;
    }
    case JOIN_SESSION: {
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
        AutoPtr<IMutableInteger32Value> sessionId;
        CMutableInteger32Value::New((IMutableInteger32Value**)&sessionId);

        AutoPtr<ISessionListener> sl = new InnerSessionListener(this);
        ECode ec = mBus->JoinSession(SERVICE_NAME,
                contactPort, sessionId, sessionOpts, sl);
        mHost->LogStatus(String("BusAttachment.joinSession()"), ec);

        if (ec != (ECode)E_STATUS_OK) {
            break;
        }

        /*
         * To communicate with an AllJoyn object, we create a ProxyBusObject.
         * A ProxyBusObject is composed of a name, path, sessionID and interfaces.
         *
         * This ProxyBusObject is located at the well-known SERVICE_NAME, under path
         * "/sample", uses sessionID of CONTACT_PORT, and implements the RawInterface.
         */
        AutoPtr<IClassLoader> loader;
        mHost->GetClassLoader((IClassLoader**)&loader);
        AutoPtr<IInterfaceInfo> itfcInfo;
        loader->LoadInterface(String("Elastos.DevSamples.DFRawDemo.RawClient.IRawInterface"),
                (IInterfaceInfo**)&itfcInfo);
        AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
        busInterfaces->Set(0, itfcInfo);
        Int32 value;
        sessionId->GetValue(&value);
        mProxyObj = NULL;
        mBus->GetProxyBusObject(SERVICE_NAME, String("/RawService"),
                value, busInterfaces, (IProxyBusObject**)&mProxyObj);

        /* We make calls to the methods of the AllJoyn object through one of its interfaces. */
        mRawInterface = NULL;
        mProxyObj->GetInterface(itfcInfo, (IInterface**)&mRawInterface);

        mMsgSessionId = value;
        mIsConnected = TRUE;
        Boolean result;
        mHost->mHandler->SendEmptyMessage(MESSAGE_STOP_PROGRESS_DIALOG, &result);
        break;
    }

    /* Release all resources acquired in the connect. */
    case DISCONNECT: {
        mIsStoppingDiscovery = TRUE;
        if (mIsConnected) {
            ECode ec = mBus->LeaveSession(mMsgSessionId);
            mHost->LogStatus(String("BusAttachment.leaveSession(): message-based session"), ec);

            ec = mBus->LeaveSession(mRawSessionId);
            mHost->LogStatus(String("BusAttachment.leaveSession(): raw session"), ec);
        }
        mBus->Disconnect();
        if (mStreamUp == true) {
            ICloseable::Probe(mOutputStream)->Close();
        }
        AutoPtr<ILooper> looper;
        GetLooper((ILooper**)&looper);
        looper->Quit();
        break;
    }

    /*
     * We have a string to send to the server via a raw session
     * socket.  If this is the first string we've ever sent on this
     * session, we need to get a so-called raw session started in the
     * service.  We are eventually going to talk to the raw session
     * over a socket FileDescriptor, but don't confuse this "socket
     * used to communicate over a raw session" with the idea of a
     * BSD raw socket (which allow you to provide your own IP or
     * Ethernet headers).  Raw sessions only means that the data
     * sent over the session will be sent using the underlying socket
     * fiel descriptor and will not be encapsulated in AllJoyn
     * messages).
     *
     * Once we have joined the raw session, we can retrieve the
     * underlying session's OS socket file descriptor and build a Java
     * FileDescriptor using a private constructor found via reflection.
     * We then create a Java output stream using that FileDescriptor.
     *
     * Once/If we have the raw session all set up, and have a Java IO
     * stream ready, we simply send the bytes of the string to the
     * service directly using the Java stream.  This completely
     * bypasses AllJoyn which was used for discovery and connection
     * establishment.
     */
    case SEND_RAW: {
        if (!mIsConnected || !mStreamUp) {
            // try {
                /*
                 * In order get a raw session to join, we need to get an
                 * ephemeral contact port.  As a part of the RawInterface,
                 * we have a method used to get that port.  Note that
                 * errors are returned through Java exceptions, and so we
                 * wrap this code in a try catch block.
                 */
                Logger::I(TAG, "RequestRawSession()");
                Int16 contactPort;
                mRawInterface->RequestRawSession(&contactPort);
                Logger::I(TAG, "RequestRawSession() returns %d", contactPort);

                /*
                 * Now join the raw session.  Note that we are asking
                 * for TRAFFIC_RAW_RELIABLE.  Once this happens, the
                 * session is ready for raw traffic that will not be
                 * encapsulated in AllJoyn messages.
                 */
                AutoPtr<ISessionOpts> sessionOpts;
                CSessionOpts::New((ISessionOpts**)&sessionOpts);
                sessionOpts->SetTraffic(ISessionOpts::TRAFFIC_RAW_RELIABLE);
                AutoPtr<IMutableInteger32Value> sessionId;
                CMutableInteger32Value::New((IMutableInteger32Value**)&sessionId);
                Logger::I(TAG, "joinSession()");
                AutoPtr<ISessionListener> sl = new SessionListener();
                ECode ec = mBus->JoinSession(RAW_SERVICE_NAME, contactPort, sessionId, sessionOpts, sl);
                mHost->LogStatus(String("BusAttachment.joinSession()"), ec);
                if (ec != (ECode)E_STATUS_OK) {
                    break;
                }
                sessionId->GetValue(&mRawSessionId);

                /*
                 * The session is in raw mode, but we need to get a
                 * socket file descriptor back from AllJoyn that
                 * represents the established connection.  We are then
                 * free to do whatever we want with the sock.
                 */
                Logger::I(TAG, "getSessionFd()");
                AutoPtr<IMutableInteger32Value> sockFd;
                CMutableInteger32Value::New((IMutableInteger32Value**)&sockFd);
                ec = mBus->GetSessionFd(mRawSessionId, sockFd);
                mHost->LogStatus(String("BusAttachment.getSessionFd()"), ec);
                if (ec != (ECode)E_STATUS_OK) {
                    break;
                }

                /*
                 * We have a socked FD, but now we need a Java file
                 * descriptor.  There is no appropriate constructor,
                 * public or not in the Dalvik FileDescriptor, so
                 * we new up a file descriptor and set its internal
                 * descriptor field using reflection.
                 */
                AutoPtr<IFileDescriptor> fd;
                CFileDescriptor::New((IFileDescriptor**)&fd);
                Int32 value;
                sockFd->GetValue(&value);
                fd->SetDescriptor(value);

                /*
                 * Now that we have a FileDescriptor with an AllJoyn
                 * raw session socket FD in it, we can use it like
                 * any other "normal" FileDescriptor.
                 */
                if (SUCCEEDED(CFileOutputStream::New(fd, (IOutputStream**)&mOutputStream)))
                    mStreamUp = TRUE;
            // } catch (Throwable ex) {
            //     Logger::I(TAG, String.format("Exception bringing up raw stream: %s", ex.toString()));
            // }
        }

        /*
         * If we've sucessfully created an output stream from the raw
         * session connection established by AllJoyn, we write the
         * byte string (from the user) to the TCP stream that now
         * underlies it all.
         */
        if (mStreamUp == TRUE) {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String string = CoreUtils::Unbox(ICharSequence::Probe(obj)) + "\n";
            // try {
                Logger::I(TAG, "Writing %s to output stream", string.string());
                mOutputStream->Write(string.GetBytes());
                Logger::I(TAG, "Flushing stream");
                IFlushable::Probe(mOutputStream)->Flush();
            // } catch (IOException ex) {
            //     Logger::I(TAG, "Exception writing and flushing the string");
            // }
        }
        break;
    }
    default:
        break;
    }
    return NOERROR;
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
const Int32 CClient::DIALOG_GET_CREDENTIALS;
const Int32 CClient::MESSAGE_PING;
const Int32 CClient::MESSAGE_PING_REPLY;
const Int32 CClient::MESSAGE_POST_TOAST;
const Int32 CClient::MESSAGE_START_PROGRESS_DIALOG;
const Int32 CClient::MESSAGE_STOP_PROGRESS_DIALOG;
const String CClient::TAG("RawRawClient");

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

    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::CONNECT, &result);
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
    mBusHandler->SendEmptyMessage(BusHandler::DISCONNECT, &result);
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

ECode CClient::GetClassID(
    /* [out] */ ClassID* clsid)
{
    VALIDATE_NOT_NULL(clsid);
    *clsid = ECLSID_CClient;
    return NOERROR;
}

} // namespace RawClient
} // namespace DFRawDemo
} // namespace DevSamples
} // namespace Elastos
