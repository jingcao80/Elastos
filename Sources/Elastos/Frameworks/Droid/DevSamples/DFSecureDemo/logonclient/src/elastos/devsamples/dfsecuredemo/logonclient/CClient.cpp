//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CClient.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Wifi.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/R.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IProgressDialogHelper;
using Elastos::Droid::App::CProgressDialogHelper;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiManagerMulticastLock;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::CCountDownLatch;

using Org::Alljoyn::Bus::EIID_IAuthListener;
using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CMutableInteger32Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger32Value;
using Org::Alljoyn::Bus::IPasswordRequest;
using Org::Alljoyn::Bus::IUserNameRequest;
using Org::Alljoyn::Bus::ISessionListener;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFSecureDemo {
namespace LogonClient {

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
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Pings:  ") + cat));
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
    case MESSAGE_SHOW_LOGON_DIALOG:
        mHost->ShowDialog(DIALOG_LOGON);
        break;
    case MESSAGE_AUTH_COMPLETE: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        Boolean authenticated;
        IBoolean::Probe(obj)->GetValue(&authenticated);
        if (!authenticated) {
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(mHost, CoreUtils::Convert(String("Authentication failed")), IToast::LENGTH_LONG, (IToast**)&toast);
            toast->Show();
        }
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
                CoreUtils::Convert(String("Finding Secure Service.\nPlease wait...")),
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
        mHost->mBusHandler->ObtainMessage(BusHandler::PING, text, (IMessage**)&msg);
        mHost->mBusHandler->SendMessage(msg, result);
    }
    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CClient::OnClickListener, Object, IDialogInterfaceOnClickListener);

CClient::OnClickListener::OnClickListener(
    /* [in] */ CClient* host,
    /* [in] */ IView* view)
    : mHost(host)
    , mView(view)
{}

ECode CClient::OnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 id)
{
    AutoPtr<IView> view;
    mView->FindViewById(R::id::UserNameEditText, (IView**)&view);
    AutoPtr<ICharSequence> text;
    ITextView::Probe(view)->GetText((ICharSequence**)&text);
    text->ToString(&mHost->mUserName);
    view = NULL;
    mView->FindViewById(R::id::PasswordEditText, (IView**)&view);
    text = NULL;
    ITextView::Probe(view)->GetText((ICharSequence**)&text);
    text->ToString(&mHost->mPassword);
    mHost->mLatch->CountDown();

    return NOERROR;
}

CAR_INTERFACE_IMPL(CClient::SrpLogonListener, Object, IAuthListener)

CClient::SrpLogonListener::SrpLogonListener(
    /* [in] */ CClient* host)
    : mHost(host)
{
}

String CClient::SrpLogonListener::GetMechanisms()
{
    return  String("ALLJOYN_SRP_LOGON");
}

String CClient::SrpLogonListener::GetKeyStoreFileName()
{
    AutoPtr<IFile> file;
    mHost->GetFileStreamPath(String("alljoyn_keystore"), (IFile**)&file);
    String path;
    file->GetAbsolutePath(&path);
    return path;
}

ECode CClient::SrpLogonListener::Requested(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& peer,
    /* [in] */ Int32 count,
    /* [in] */ const String& userName,
    /* [in] */ ArrayOf<IAuthRequest*>* requests,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    /* Collect the requests we're interested in to simplify processing below. */
    AutoPtr<IPasswordRequest> passwordRequest;
    AutoPtr<IUserNameRequest> userNameRequest;
    Int32 len = requests ? requests->GetLength() : 0;
    for (Int32 i = 0; i < len; i++) {
        IAuthRequest* request = (*requests)[i];
        if (IPasswordRequest::Probe(request)) {
            passwordRequest = IPasswordRequest::Probe(request);
        }
        else if (IUserNameRequest::Probe(request)) {
            userNameRequest = IUserNameRequest::Probe(request);
        }
    }
    *res = FALSE;
    if (count <= 3) {
        /*
         * We need to wait here for the user to enter the credentials before we can
         * return.  The latch takes care of the synchronization for us.
         */
        mHost->mLatch = NULL;
        CCountDownLatch::New(1, (ICountDownLatch**)&mHost->mLatch);
        mHost->SendUiMessage(MESSAGE_SHOW_LOGON_DIALOG, NULL);
        if (FAILED(mHost->mLatch->Await())) {
            Logger::E(TAG, "Error waiting for logon");
            return NOERROR;
        }
        userNameRequest->SetUserName(mHost->mUserName);
        passwordRequest->SetPassword(mHost->mPassword.GetChars());
        *res =  TRUE;
    }

    return NOERROR;
}

ECode CClient::SrpLogonListener::Completed(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& authPeer,
    /* [in] */ Boolean authenticated)
{
    Logger::D(TAG, "Listener: authentication %s completed %d", authMechanism.string(), authenticated);
    mHost->SendUiMessage(MESSAGE_AUTH_COMPLETE, CoreUtils::Convert(authenticated));
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
const Int32 CClient::BusHandler::PING = 4;
const String CClient::BusHandler::SERVICE_NAME("org.alljoyn.bus.samples.secure");
const Int16 CClient::BusHandler::CONTACT_PORT = 42;

ECode CClient::BusHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ CClient* host)
{
    Handler::constructor(looper);

    mSessionId = 0;
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
    /* Connect to a remote instance of an object implementing the SecureInterface. */
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
         * Register the AuthListener before calling connect() to ensure that everything is
         * in place before any remote peers access the service.
         */
        ECode ec = mBus->RegisterAuthListener(mHost->mAuthListener->GetMechanisms(),
            mHost->mAuthListener, mHost->mAuthListener->GetKeyStoreFileName());
        mHost->LogStatus(String("BusAttachment.registerAuthListener()"), ec);
        if (ec != E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * Create a bus listener class
         */
        AutoPtr<IBusListener> bl = new InnerBusListener(this);
        mBus->RegisterBusListener(bl);

        /* To communicate with AllJoyn objects, we must connect the BusAttachment to the bus. */
        ec = mBus->Connect();
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

        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<ISessionListener> sl = new InnerSessionListener(this);
        ECode ec = mBus->JoinSession(CoreUtils::Unbox(ICharSequence::Probe(obj)),
                contactPort, sessionId, sessionOpts, sl);
        mHost->LogStatus(String("BusAttachment.joinSession()"), ec);

        if (ec == (ECode)E_STATUS_OK) {
            /*
             * To communicate with an AllJoyn object, we create a ProxyBusObject.
             * A ProxyBusObject is composed of a name, path, sessionID and interfaces.
             *
             * This ProxyBusObject is located at the well-known SERVICE_NAME, under path
             * "/sample", uses sessionID of CONTACT_PORT, and implements the SecureInterface.
             */
            AutoPtr<IClassLoader> loader;
            mHost->GetClassLoader((IClassLoader**)&loader);
            AutoPtr<IInterfaceInfo> itfcInfo;
            loader->LoadInterface(String("Elastos.DevSamples.DFSecureDemo.LogonClient.ISecureInterface"),
                    (IInterfaceInfo**)&itfcInfo);
            AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
            busInterfaces->Set(0, itfcInfo);
            Int32 value;
            sessionId->GetValue(&value);
            mProxyObj = NULL;
            mBus->GetProxyBusObject(SERVICE_NAME, String("/SecureService"),
                    value, busInterfaces, (IProxyBusObject**)&mProxyObj);

            /* We make calls to the methods of the AllJoyn object through one of its interfaces. */
            mSecureInterface = NULL;
            mProxyObj->GetInterface(EIID_ISecureInterface, (IInterface**)&mSecureInterface);

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
            mHost->LogStatus(String("BusAttachment.leaveSession()"), ec);
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
    case PING: {
    // try {
        if (mSecureInterface != NULL) {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String str = CoreUtils::Unbox(ICharSequence::Probe(obj));
            mHost->SendUiMessage(MESSAGE_PING, obj);
            String reply;
            mSecureInterface->Ping(str, &reply);
            mHost->SendUiMessage(MESSAGE_PING_REPLY, CoreUtils::Convert(reply));
        }
    // catch (ErrorReplyBusException erbe) {
    //     logException("SecureInterface.Ping() " + erbe.getErrorName() + ": " +
    //        erbe.getErrorMessage() + " status: " + erbe.getErrorStatus().getErrorCode(), erbe);
    // }
    // catch (BusException ex) {
    //     logException("SecureInterface.Ping()", ex);
    // }
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
const Int32 CClient::DIALOG_LOGON;
const Int32 CClient::MESSAGE_PING;
const Int32 CClient::MESSAGE_PING_REPLY;
const Int32 CClient::MESSAGE_SHOW_LOGON_DIALOG;
const Int32 CClient::MESSAGE_AUTH_COMPLETE;
const Int32 CClient::MESSAGE_POST_TOAST;
const Int32 CClient::MESSAGE_START_PROGRESS_DIALOG;
const Int32 CClient::MESSAGE_STOP_PROGRESS_DIALOG;
const String CClient::TAG("SecureLogonClient");

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

    mAuthListener = new SrpLogonListener(this);
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

AutoPtr<IDialog> CClient::OnCreateDialog(
    /* [in] */ Int32 id)
{
    switch (id) {
    case DIALOG_LOGON: {
        AutoPtr<ILayoutInflater> factory;
        LayoutInflater::From(this, (ILayoutInflater**)&factory);
        AutoPtr<IView> view;
        factory->Inflate(R::layout::logon_dialog, NULL, (IView**)&view);
        AutoPtr<IAlertDialog> dialog;
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetIcon(Elastos::Droid::R::drawable::ic_dialog_alert);
        builder->SetTitle(R::string::logon_dialog);
        builder->SetCancelable(FALSE);
        builder->SetView(view);
        AutoPtr<OnClickListener> listener = new OnClickListener(this, view);
        builder->SetPositiveButton(R::string::dialog_ok, listener);
        builder->Create((IAlertDialog**)&dialog);
        return IDialog::Probe(dialog);
    }
    default:
        return NULL;
    }
}

void CClient::SendUiMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(what, obj, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
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

} // namespace LogonClient
} // namespace DFSecureDemo
} // namespace DevSamples
} // namespace Elastos
