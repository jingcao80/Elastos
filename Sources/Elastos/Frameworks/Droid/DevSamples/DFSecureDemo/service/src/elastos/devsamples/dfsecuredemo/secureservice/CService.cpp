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

#include "CService.h"
#include "CSecureService.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/R.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CRandom;
using Elastos::Utility::Logging::Logger;

using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CBusListener;
using Org::Alljoyn::Bus::CMutableInteger16Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::EIID_IAuthListener;
using Org::Alljoyn::Bus::EIID_IBusObject;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger16Value;
using Org::Alljoyn::Bus::ICertificateRequest;
using Org::Alljoyn::Bus::IExpirationRequest;
using Org::Alljoyn::Bus::IPasswordRequest;
using Org::Alljoyn::Bus::IPrivateKeyRequest;
using Org::Alljoyn::Bus::IVerifyRequest;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::ISessionPortListener;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFSecureDemo {
namespace SecureService {

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
        String ping = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Ping:  ") + ping));
        break;
    }
    case MESSAGE_PING_REPLY:{
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        String reply = CoreUtils::Unbox(ICharSequence::Probe(obj));
        mHost->mListViewArrayAdapter->Add(CoreUtils::Convert(String("Reply:  ") + reply));
        break;
    }
    case MESSAGE_SHOW_ONE_TIME_PASSWORD_DIALOG:
        mHost->mCredentialsDialog = DIALOG_ONE_TIME_PASSWORD;
        mHost->ShowDialog(mHost->mCredentialsDialog);
        break;
    case MESSAGE_SHOW_PASSPHRASE_DIALOG: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        IPasswordRequest* request = IPasswordRequest::Probe(obj);
        Boolean res;
        request->IsNewPassword(&res);
        mHost->mCredentialsDialog = res ? DIALOG_CREATE_PASSPHRASE : DIALOG_ENTER_PASSPHRASE;
        mHost->ShowDialog(mHost->mCredentialsDialog);
        break;
    }
    case MESSAGE_AUTH_COMPLETE: {
        if (mHost->mCredentialsDialog != 0) {
            mHost->DismissDialog(mHost->mCredentialsDialog);
            mHost->mCredentialsDialog = 0;
        }
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
    default:
        break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CService::OnEditorActionListener, Object, IOnEditorActionListener)

CService::OnEditorActionListener::OnEditorActionListener(
    /* [in] */ CService* host)
    : mHost(host)
{}

ECode CService::OnEditorActionListener::OnEditorAction(
    /* [in] */ ITextView* view,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 action;
    if (actionId == IEditorInfo::IME_NULL
        && (event->GetAction(&action), action == IKeyEvent::ACTION_UP)) {
        AutoPtr<ICharSequence> text;
        view->GetText((ICharSequence**)&text);
        text->ToString(&mHost->mPassword);
        mHost->mLatch->CountDown();
        mHost->DismissDialog(mHost->mCredentialsDialog);
    }
    *res = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CService::AuthListeners, Object, IAuthListener)

CService::AuthListeners::AuthListeners(
    /* [in] */ CService* host)
    : mHost(host)
{
    mAuthListeners[String("ALLJOYN_SRP_KEYX")] = new SrpKeyXListener(mHost);
    mAuthListeners[String("ALLJOYN_SRP_LOGON")] = new SrpLogonListener();
    AutoPtr<ECDHEKeyXListener> ecdheListener = new ECDHEKeyXListener(mHost);
    mAuthListeners[String("ALLJOYN_ECDHE_NULL")] = ecdheListener;
    mAuthListeners[String("ALLJOYN_ECDHE_PSK")] = ecdheListener;
    mAuthListeners[String("ALLJOYN_ECDHE_ECDSA")] = ecdheListener;
}

String CService::AuthListeners::GetMechanisms()
{
    StringBuilder mechanisms;
    HashMap<String, AutoPtr<IAuthListener> >::Iterator it;
    for (it = mAuthListeners.Begin(); it != mAuthListeners.End(); ++it) {
        mechanisms.Append(it->mFirst);
        mechanisms.Append(" ");
    }
    return mechanisms.ToString();
}

String CService::AuthListeners::GetKeyStoreFileName()
{
    AutoPtr<IFile> file;
    mHost->GetFileStreamPath(String("alljoyn_keystore"), (IFile**)&file);
    String path;
    file->GetAbsolutePath(&path);
    return path;
}

ECode CService::AuthListeners::Requested(
    /* [in] */ const String& mechanism,
    /* [in] */ const String& peer,
    /* [in] */ Int32 count,
    /* [in] */ const String& userName,
    /* [in] */ ArrayOf<IAuthRequest*>* requests,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    HashMap<String, AutoPtr<IAuthListener> >::Iterator it = mAuthListeners.Find(mechanism);
    if (it != mAuthListeners.End()) {
        return it->mSecond->Requested(mechanism, peer, count, userName, requests, res);
    }
    else {
        *res = FALSE;
    }
    return NOERROR;
}

ECode CService::AuthListeners::Completed(
    /* [in] */ const String& mechanism,
    /* [in] */ const String& authPeer,
    /* [in] */ Boolean authenticated)
{
    HashMap<String, AutoPtr<IAuthListener> >::Iterator it = mAuthListeners.Find(mechanism);
    if (it != mAuthListeners.End()) {
        it->mSecond->Completed(mechanism, authPeer, authenticated);
    }
    mHost->SendUiMessage(MESSAGE_AUTH_COMPLETE, CoreUtils::Convert(authenticated));
    return NOERROR;
}

CAR_INTERFACE_IMPL(CService::SrpKeyXListener, Object, IAuthListener)

CService::SrpKeyXListener::SrpKeyXListener(
    /* [in] */ CService* host)
    : mGeneratePassword(TRUE)
    , mHost(host)
{
    CRandom::New((IRandom**)&mPasswordGenerator);
}

ECode CService::SrpKeyXListener::Requested(
    /* [in] */ const String& mechanism,
    /* [in] */ const String& peer,
    /* [in] */ Int32 count,
    /* [in] */ const String& userName,
    /* [in] */ ArrayOf<IAuthRequest*>* requests,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mGeneratePassword) {
        mGeneratePassword = FALSE;
        Int32 i;
        mPasswordGenerator->NextInt32(1000000, &i);
        mHost->mPassword = NULL;
        mHost->mPassword.AppendFormat("%06d", i);
        mHost->SendUiMessage(MESSAGE_SHOW_ONE_TIME_PASSWORD_DIALOG, NULL);
    }
    Int32 len = requests ? requests->GetLength() : 0;
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IPasswordRequest> request = IPasswordRequest::Probe((*requests)[i]);
        if (request) {
            request->SetPassword(mHost->mPassword.GetChars());
        }
    }
    *res = TRUE;
    return NOERROR;
}

ECode CService::SrpKeyXListener::Completed(
    /* [in] */ const String& mechanism,
    /* [in] */ const String& authPeer,
    /* [in] */ Boolean authenticated)
{
    mGeneratePassword = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CService::SrpLogonListener, Object, IAuthListener)

CService::SrpLogonListener::SrpLogonListener()
{
    mUserNamePassword[String("user1")] = String("password1").GetChars();
    mUserNamePassword[String("user2")] = String("password2").GetChars();
}

ECode CService::SrpLogonListener::Requested(
    /* [in] */ const String& mechanism,
    /* [in] */ const String& peer,
    /* [in] */ Int32 count,
    /* [in] */ const String& userName,
    /* [in] */ ArrayOf<IAuthRequest*>* requests,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    HashMap<String, AutoPtr<ArrayOf<Char32> > >::Iterator it = mUserNamePassword.Find(userName);
    if (it != mUserNamePassword.End()) {
        Int32 len = requests ? requests->GetLength() : 0;
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IPasswordRequest> request = IPasswordRequest::Probe((*requests)[i]);
            if (request) {
                request->SetPassword(it->mSecond);
            }
        }
    }
    return TRUE;
}

ECode CService::SrpLogonListener::Completed(
    /* [in] */ const String& mechanism,
    /* [in] */ const String& authPeer,
    /* [in] */ Boolean authenticated)
{
    return NOERROR;
}

const String CService::ECDHEKeyXListener::SERVER_PK_PEM(
    "-----BEGIN EC PRIVATE KEY-----\n"
    "MDECAQEEIICSqj3zTadctmGnwyC/SXLioO39pB1MlCbNEX04hjeioAoGCCqGSM49\n"
    "AwEH\n"
    "-----END EC PRIVATE KEY-----");

const String CService::ECDHEKeyXListener::SERVER_ECC_X509_PEM(
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBWjCCAQGgAwIBAgIHMTAxMDEwMTAKBggqhkjOPQQDAjArMSkwJwYDVQQDDCAw\n"
    "ZTE5YWZhNzlhMjliMjMwNDcyMGJkNGY2ZDVlMWIxOTAeFw0xNTAyMjYyMTU1MjVa\n"
    "Fw0xNjAyMjYyMTU1MjVaMCsxKTAnBgNVBAMMIDZhYWM5MjQwNDNjYjc5NmQ2ZGIy\n"
    "NmRlYmRkMGM5OWJkMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEP/HbYga30Afm\n"
    "0fB6g7KaB5Vr5CDyEkgmlif/PTsgwM2KKCMiAfcfto0+L1N0kvyAUgff6sLtTHU3\n"
    "IdHzyBmKP6MQMA4wDAYDVR0TBAUwAwEB/zAKBggqhkjOPQQDAgNHADBEAiAZmNVA\n"
    "m/H5EtJl/O9x0P4zt/UdrqiPg+gA+wm0yRY6KgIgetWANAE2otcrsj3ARZTY/aTI\n"
    "0GOQizWlQm8mpKaQ3uE=\n"
    "-----END CERTIFICATE-----");

CAR_INTERFACE_IMPL(CService::ECDHEKeyXListener, Object, IAuthListener)

CService::ECDHEKeyXListener::ECDHEKeyXListener(
    /* [in] */ CService* host)
    : mSendBackKeys(TRUE)
    , mHost(host)
{
}

ECode CService::ECDHEKeyXListener::Requested(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& peer,
    /* [in] */ Int32 count,
    /* [in] */ const String& userName,
    /* [in] */ ArrayOf<IAuthRequest*>* requests,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    ECode ec = NOERROR;
    do {
        Logger::D(TAG, "Listener requested with mechanism %s", authMechanism.string());
        if (!authMechanism.Equals("ALLJOYN_ECDHE_NULL") &&
            !authMechanism.Equals("ALLJOYN_ECDHE_PSK") &&
            !authMechanism.Equals("ALLJOYN_ECDHE_ECDSA")) {
            *res = FALSE;
            return NOERROR;
        }

        Int32 len = requests ? requests->GetLength() : 0;
        for (Int32 i = 0; i < len; i++) {
            IAuthRequest* rqst = (*requests)[i];
            if (IPrivateKeyRequest::Probe(rqst)) {
                /*
                 * Only the ALLJOYN_ECDHE_ECDSA requests for DSA private key.
                 */
                if (mSendBackKeys) {
                    IPrivateKeyRequest* pkRqst = IPrivateKeyRequest::Probe(rqst);
                    String privateKeyPEM = SERVER_PK_PEM;
                    ec = pkRqst->SetPrivateKey(privateKeyPEM);
                    if (FAILED(ec))
                        break;
                    Logger::D(TAG, "Listener sends back private key %s", privateKeyPEM.string());
                }
            }
            else if (ICertificateRequest::Probe(rqst)) {
                /*
                 * Only the ALLJOYN_ECDHE_ECDSA requests for certificate
                 */
                if (mSendBackKeys) {
                    String certChainPEM = SERVER_ECC_X509_PEM;
                    ICertificateRequest* certChainRqst = ICertificateRequest::Probe(rqst);
                    ec = certChainRqst->SetCertificateChain(certChainPEM);
                    if (FAILED(ec))
                        break;
                    Logger::D(TAG, "Listener sends back cert chain %s", certChainPEM.string());
                }
            }
            else if (IPasswordRequest::Probe(rqst)) {
                /*
                 * Only the ECDHE_PSK key exchange requests for the pre shared secret.
                 * Based on the pre shared secret id given the username input variable,
                 * the application can retrieve the pre shared secret
                 * from storage or from the end user.
                 * In this example, the pre shared secret is a hard coded string
                 */
                IPasswordRequest* pskRqst = IPasswordRequest::Probe(rqst);
                String psk("123456");
                ec = pskRqst->SetPassword(psk.GetChars());
                if (FAILED(ec))
                    break;
                Logger::D(TAG, "Listener sends back PSK %s", psk.string());
            }
            else if (IVerifyRequest::Probe(rqst)) {
                /*
                 * Only the ECDHE_ECDSA key exchange sends a VerifyRequest
                 * with the certificate chain from the peer for the
                 * application to verify.
                 * The application has to option to verify the certificate chain.
                 * If the cert chain is validated and trusted then return TRUE; otherwise, return false.
                 */
                IVerifyRequest* verifyRqst = IVerifyRequest::Probe(rqst);
                String certPEM;
                ec = verifyRqst->GetCertificateChain(&certPEM);
                if (FAILED(ec))
                    break;
                Logger::D(TAG, "Listener: verifying cert %s", certPEM.string());
            }
            else if (IExpirationRequest::Probe(rqst)) {
                IExpirationRequest* er = IExpirationRequest::Probe(rqst);
                ec = er->SetExpiration(100); /* expired 100 seconds from now */
                if (FAILED(ec))
                    break;
            }
        }
        if (FAILED(ec))
            break;
        *res = TRUE;
        return NOERROR;
    } while (0);

    Logger::E(TAG, "Error processing auth request ec = 0x%08x", ec);
    *res = FALSE;
    return NOERROR;
}

ECode CService::ECDHEKeyXListener::Completed(
    /* [in] */ const String& mechanism,
    /* [in] */ const String& authPeer,
    /* [in] */ Boolean authenticated)
{
    mHost->SendUiMessage(MESSAGE_AUTH_COMPLETE, CoreUtils::Convert(authenticated));
    return NOERROR;
}

//======================================================================
//  CService::SecureService
//======================================================================
CAR_INTERFACE_IMPL_2(CService::SecureService, Object, ISecureInterface, IBusObject)
CAR_OBJECT_IMPL(CSecureService)

ECode CService::SecureService::constructor(
    /* [in] */ IObject* host)
{
    mHost = (CService*)host;
    return NOERROR;
}

ECode CService::SecureService::Ping(
    /* [in] */ const String& inStr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->SendUiMessage(MESSAGE_PING, CoreUtils::Convert(inStr));
    mHost->SendUiMessage(MESSAGE_PING_REPLY, CoreUtils::Convert(inStr));
    *result = inStr;
    return NOERROR;
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
    Logger::I(TAG, "InnerSessionPortListener.AcceptSessionJoiner(%d, %s, %s)",
        sessionPort, joiner.string(), TO_CSTR(opts));
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
const String CService::BusHandler::SERVICE_NAME("org.alljoyn.bus.samples.secure");
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
    case CONNECT: {
        AutoPtr<IContext> context;
        mHost->GetApplicationContext((IContext**)&context);
        AutoPtr<IDaemonInit> di;
        CDaemonInit::AcquireSingleton((IDaemonInit**)&di);
        Boolean result;
        di->PrepareDaemon(context, &result);

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
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

        /*
         * Create a bus listener class
         */
        AutoPtr<IBusListener> bl;
        CBusListener::New((IBusListener**)&bl);
        mBus->RegisterBusListener(bl);

        ec = mBus->RegisterBusObject(mHost->mSecureService, String("/SecureService"));
        mHost->LogStatus(String("BusAttachment.registerBusObject()"), ec);
        if (ec != (ECode)E_STATUS_OK) {
            mHost->Finish();
            return NOERROR;
        }

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
        mHost->LogStatus(String("BusAttachment.bindSessionPort()"), ec);
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
        mBus->UnregisterBusObject(mHost->mSecureService);
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
const String CService::TAG("SecureService");
const Int32 CService::DIALOG_ONE_TIME_PASSWORD;
const Int32 CService::DIALOG_CREATE_PASSPHRASE;
const Int32 CService::DIALOG_ENTER_PASSPHRASE;
const Int32 CService::MESSAGE_PING;
const Int32 CService::MESSAGE_PING_REPLY;
const Int32 CService::MESSAGE_SHOW_ONE_TIME_PASSWORD_DIALOG;
const Int32 CService::MESSAGE_SHOW_PASSPHRASE_DIALOG;
const Int32 CService::MESSAGE_AUTH_COMPLETE;
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

    AutoPtr<IHandlerThread> busThread;
    CHandlerThread::New(String("BusHandler"), (IHandlerThread**)&busThread);
    IThread::Probe(busThread)->Start();
    AutoPtr<ILooper> looper;
    busThread->GetLooper((ILooper**)&looper);
    mBusHandler = new BusHandler();
    mBusHandler->constructor(looper, this);

    mAuthListener = new AuthListeners(this);
    AutoPtr<ISecureInterface> bi;
    CSecureService::New(this, (ISecureInterface**)&bi);
    mSecureService = (SecureService*)bi.Get();
    Boolean result;
    mBusHandler->SendEmptyMessage(BusHandler::CONNECT, &result);
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

AutoPtr<IDialog> CService::OnCreateDialog(
    /* [in] */ Int32 id)
{
    switch (id) {
    case DIALOG_ONE_TIME_PASSWORD: {
        AutoPtr<ILayoutInflater> factory;
        LayoutInflater::From(this, (ILayoutInflater**)&factory);
        AutoPtr<IView> view;
        factory->Inflate(R::layout::one_time_password_dialog, NULL, (IView**)&view);
        AutoPtr<IAlertDialog> dialog;
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetIcon(Elastos::Droid::R::drawable::ic_dialog_alert);
        builder->SetTitle(R::string::one_time_password_dialog);
        builder->SetCancelable(FALSE);
        builder->SetView(view);
        builder->Create((IAlertDialog**)&dialog);
        return IDialog::Probe(dialog);
    }
    case DIALOG_CREATE_PASSPHRASE:
    case DIALOG_ENTER_PASSPHRASE: {
        Int32 title = (id == DIALOG_CREATE_PASSPHRASE)
            ? R::string::create_passphrase_dialog : R::string::enter_passphrase_dialog;
        AutoPtr<ILayoutInflater> factory;
        LayoutInflater::From(this, (ILayoutInflater**)&factory);
        AutoPtr<IView> view;
        factory->Inflate(R::layout::passphrase_dialog, NULL, (IView**)&view);
        AutoPtr<IView> editText;
        view->FindViewById(R::id::PassphraseEditText, (IView**)&editText);;
        AutoPtr<OnEditorActionListener> listener = new OnEditorActionListener(this);
        ITextView::Probe(editText)->SetOnEditorActionListener(listener);

        AutoPtr<IAlertDialog> dialog;
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        builder->SetIcon(Elastos::Droid::R::drawable::ic_dialog_alert);
        builder->SetTitle(title);
        builder->SetCancelable(FALSE);
        builder->SetView(view);
        builder->Create((IAlertDialog**)&dialog);
        return IDialog::Probe(dialog);
    }
    default:
        return NULL;
    }
}

void CService::OnPrepareDialog(
    /* [in] */ Int32 id,
    /* [in] */ IDialog* dialog)
{
    switch (id) {
    case DIALOG_ONE_TIME_PASSWORD: {
        AutoPtr<IView> textView;
        dialog->FindViewById(R::id::PasswordText, (IView**)&textView);
        ITextView::Probe(textView)->SetText(CoreUtils::Convert(mPassword));
        break;
    }
    default:
        break;
    }
}

void CService::SendUiMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(what, obj, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
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

} // namespace SecureService
} // namespace DFSecureDemo
} // namespace DevSamples
} // namespace Elastos
