
#include "CClient.h"
#include "R.h"
#include <Org.Alljoyn.Bus.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;

using Org::Alljoyn::Bus::IDaemonInit;
using Org::Alljoyn::Bus::CDaemonInit;

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
    // switch (msg.what) {
    // case MESSAGE_PING:
    //     String cat = (String) msg.obj;
    //     mListViewArrayAdapter.add("Cat args:  " + cat);
    //     break;
    // case MESSAGE_PING_REPLY:
    //     String ret = (String) msg.obj;
    //     mListViewArrayAdapter.add("Reply:  " + ret);
    //     mEditText.setText("");
    //     break;
    // case MESSAGE_POST_TOAST:
    //     Toast.makeText(getApplicationContext(), (String) msg.obj, Toast.LENGTH_LONG).show();
    //     break;
    // case MESSAGE_START_PROGRESS_DIALOG:
    //     mDialog = ProgressDialog.show(Client.this,
    //                                   "",
    //                                   "Finding Basic Service.\nPlease wait...",
    //                                   true,
    //                                   true);
    //     break;
    // case MESSAGE_STOP_PROGRESS_DIALOG:
    //     mDialog.dismiss();
    //     break;
    // default:
    //     break;
    // }
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
    // if (actionId == EditorInfo.IME_NULL
    //     && event.getAction() == KeyEvent.ACTION_UP) {
    //     /* Call the remote object's cat method. */
    //     Message msg = mBusHandler.obtainMessage(BusHandler.CAT,
    //                                             view.getText().toString());
    //     mBusHandler.sendMessage(msg);
    // }
    *result = TRUE;
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
        di->PrepareDaemon(context);
//        /*
//         * All communication through AllJoyn begins with a BusAttachment.
//         *
//         * A BusAttachment needs a name. The actual name is unimportant except for internal
//         * security. As a default we use the class name as the name.
//         *
//         * By default AllJoyn does not allow communication between devices (i.e. bus to bus
//         * communication). The second argument must be set to Receive to allow communication
//         * between devices.
//         */
//        mBus = new BusAttachment(getPackageName(), BusAttachment.RemoteMessage.Receive);
//
//        /*
//         * Create a bus listener class
//         */
//        mBus.registerBusListener(new BusListener() {
//            @Override
//            public void foundAdvertisedName(String name, short transport, String namePrefix) {
//                logInfo(String.format("MyBusListener.foundAdvertisedName(%s, 0x%04x, %s)", name, transport, namePrefix));
//                /*
//                 * This client will only join the first service that it sees advertising
//                 * the indicated well-known name.  If the program is already a member of
//                 * a session (i.e. connected to a service) we will not attempt to join
//                 * another session.
//                 * It is possible to join multiple session however joining multiple
//                 * sessions is not shown in this sample.
//                 */
//                if(!mIsConnected) {
//                    Message msg = obtainMessage(JOIN_SESSION);
//                    msg.arg1 = transport;
//                    msg.obj = name;
//                    sendMessage(msg);
//                }
//            }
//        });
//
//        /* To communicate with AllJoyn objects, we must connect the BusAttachment to the bus. */
//        Status status = mBus.connect();
//        logStatus("BusAttachment.connect()", status);
//        if (Status.OK != status) {
//            finish();
//            return;
//        }
//
//        /*
//         * Now find an instance of the AllJoyn object we want to call.  We start by looking for
//         * a name, then connecting to the device that is advertising that name.
//         *
//         * In this case, we are looking for the well-known SERVICE_NAME.
//         */
//        status = mBus.findAdvertisedName(SERVICE_NAME);
//        logStatus(String.format("BusAttachement.findAdvertisedName(%s)", SERVICE_NAME), status);
//        if (Status.OK != status) {
//            finish();
//            return;
//        }

        break;
    }
    case (JOIN_SESSION): {
//        /*
//         * If discovery is currently being stopped don't join to any other sessions.
//         */
//        if (mIsStoppingDiscovery) {
//            break;
//        }
//
//        /*
//         * In order to join the session, we need to provide the well-known
//         * contact port.  This is pre-arranged between both sides as part
//         * of the definition of the chat service.  As a result of joining
//         * the session, we get a session identifier which we must use to
//         * identify the created session communication channel whenever we
//         * talk to the remote side.
//         */
//        short contactPort = CONTACT_PORT;
//        SessionOpts sessionOpts = new SessionOpts();
//        sessionOpts.transports = (short)msg.arg1;
//        Mutable.IntegerValue sessionId = new Mutable.IntegerValue();
//
//        Status status = mBus.joinSession((String) msg.obj, contactPort, sessionId, sessionOpts, new SessionListener() {
//            @Override
//            public void sessionLost(int sessionId, int reason) {
//                mIsConnected = false;
//                logInfo(String.format("MyBusListener.sessionLost(sessionId = %d, reason = %d)", sessionId,reason));
//                mHandler.sendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG);
//            }
//        });
//        logStatus("BusAttachment.joinSession() - sessionId: " + sessionId.value, status);
//
//        if (status == Status.OK) {
//            /*
//             * To communicate with an AllJoyn object, we create a ProxyBusObject.
//             * A ProxyBusObject is composed of a name, path, sessionID and interfaces.
//             *
//             * This ProxyBusObject is located at the well-known SERVICE_NAME, under path
//             * "/sample", uses sessionID of CONTACT_PORT, and implements the BasicInterface.
//             */
//            mProxyObj =  mBus.getProxyBusObject(SERVICE_NAME,
//                                                "/sample",
//                                                sessionId.value,
//                                                new Class<?>[] { BasicInterface.class });
//
//            /* We make calls to the methods of the AllJoyn object through one of its interfaces. */
//            mBasicInterface =  mProxyObj.getInterface(BasicInterface.class);
//
//            mSessionId = sessionId.value;
//            mIsConnected = true;
//            mHandler.sendEmptyMessage(MESSAGE_STOP_PROGRESS_DIALOG);
//        }
        break;
    }

    /* Release all resources acquired in the connect. */
    case DISCONNECT: {
//        mIsStoppingDiscovery = true;
//        if (mIsConnected) {
//            Status status = mBus.leaveSession(mSessionId);
//            logStatus("BusAttachment.leaveSession()", status);
//        }
//        mBus.disconnect();
//        getLooper().quit();
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
//            if (mBasicInterface != null) {
//                sendUiMessage(MESSAGE_PING, msg.obj + " and " + msg.obj);
//                String reply = mBasicInterface.cat((String) msg.obj, (String) msg.obj);
//                sendUiMessage(MESSAGE_PING_REPLY, reply);
//            }
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


//======================================================================
//  CClient
//======================================================================
const Int32 CClient::MESSAGE_PING = 1;
const Int32 CClient::MESSAGE_PING_REPLY = 2;
const Int32 CClient::MESSAGE_POST_TOAST = 3;
const Int32 CClient::MESSAGE_START_PROGRESS_DIALOG = 4;
const Int32 CClient::MESSAGE_STOP_PROGRESS_DIALOG = 5;
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
    // mBusHandler = new BusHandler(busThread.getLooper());

    /* Connect to an AllJoyn object. */
    Boolean result;
    // mBusHandler.sendEmptyMessage(BusHandler.CONNECT);
    mHandler->SendEmptyMessage(MESSAGE_START_PROGRESS_DIALOG, &result);
    return NOERROR;
}

} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos
