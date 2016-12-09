
#include "CClient.h"
#include "R.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Wifi.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

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
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiManagerMulticastLock;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

using Org::Alljoyn::Bus::EIID_IAuthListener;
using Org::Alljoyn::Bus::CBusAttachment;
using Org::Alljoyn::Bus::CMutableInteger32Value;
using Org::Alljoyn::Bus::CSessionOpts;
using Org::Alljoyn::Bus::IBusListener;
using Org::Alljoyn::Bus::IMutableInteger32Value;
using Org::Alljoyn::Bus::ICertificateRequest;
using Org::Alljoyn::Bus::IExpirationRequest;
using Org::Alljoyn::Bus::IPasswordRequest;
using Org::Alljoyn::Bus::IPrivateKeyRequest;
using Org::Alljoyn::Bus::IVerifyRequest;
using Org::Alljoyn::Bus::ISessionListener;
using Org::Alljoyn::Bus::ISessionOpts;
using Org::Alljoyn::Bus::RemoteMessage_Receive;
using Org::Alljoyn::Bus::Alljoyn::IDaemonInit;
using Org::Alljoyn::Bus::Alljoyn::CDaemonInit;

namespace Elastos {
namespace DevSamples {
namespace DFSecureDemo {
namespace ECDHEClient {

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

const String CClient::ECDHEKeyXListener::CLIENT_PK_PEM(
    "-----BEGIN EC PRIVATE KEY-----\n"
    "MHcCAQEEIAqN6AtyOAPxY5k7eFNXAwzkbsGMl4uqvPrYkIj0LNZBoAoGCCqGSM49\n"
    "AwEHoUQDQgAEvnRd4fX9opwgXX4Em2UiCMsBbfaqhB1U5PJCDZacz9HumDEzYdrS\n"
    "MymSxR34lL0GJVgEECvBTvpaHP2bpTIl6g==\n"
    "-----END EC PRIVATE KEY-----");

const String CClient::ECDHEKeyXListener::CLIENT_ECC_X509_PEM(
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBtDCCAVmgAwIBAgIJAMlyFqk69v+OMAoGCCqGSM49BAMCMFYxKTAnBgNVBAsM\n"
    "IDdhNDhhYTI2YmM0MzQyZjZhNjYyMDBmNzdhODlkZDAyMSkwJwYDVQQDDCA3YTQ4\n"
    "YWEyNmJjNDM0MmY2YTY2MjAwZjc3YTg5ZGQwMjAeFw0xNTAyMjYyMTUxMjVaFw0x\n"
    "NjAyMjYyMTUxMjVaMFYxKTAnBgNVBAsMIDZkODVjMjkyMjYxM2IzNmUyZWVlZjUy\n"
    "NzgwNDJjYzU2MSkwJwYDVQQDDCA2ZDg1YzI5MjI2MTNiMzZlMmVlZWY1Mjc4MDQy\n"
    "Y2M1NjBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABL50XeH1/aKcIF1+BJtlIgjL\n"
    "AW32qoQdVOTyQg2WnM/R7pgxM2Ha0jMpksUd+JS9BiVYBBArwU76Whz9m6UyJeqj\n"
    "EDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0EAwIDSQAwRgIhAKfmglMgl67L5ALF\n"
    "Z63haubkItTMACY1k4ROC2q7cnVmAiEArvAmcVInOq/U5C1y2XrvJQnAdwSl/Ogr\n"
    "IizUeK0oI5c=\n"
    "-----END CERTIFICATE-----\n"
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBszCCAVmgAwIBAgIJAILNujb37gH2MAoGCCqGSM49BAMCMFYxKTAnBgNVBAsM\n"
    "IDdhNDhhYTI2YmM0MzQyZjZhNjYyMDBmNzdhODlkZDAyMSkwJwYDVQQDDCA3YTQ4\n"
    "YWEyNmJjNDM0MmY2YTY2MjAwZjc3YTg5ZGQwMjAeFw0xNTAyMjYyMTUxMjNaFw0x\n"
    "NjAyMjYyMTUxMjNaMFYxKTAnBgNVBAsMIDdhNDhhYTI2YmM0MzQyZjZhNjYyMDBm\n"
    "NzdhODlkZDAyMSkwJwYDVQQDDCA3YTQ4YWEyNmJjNDM0MmY2YTY2MjAwZjc3YTg5\n"
    "ZGQwMjBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABGEkAUATvOE4uYmt/10vkTcU\n"
    "SA0C+YqHQ+fjzRASOHWIXBvpPiKgHcINtNFQsyX92L2tMT2Kn53zu+3S6UAwy6yj\n"
    "EDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0EAwIDSAAwRQIgKit5yeq1uxTvdFmW\n"
    "LDeoxerqC1VqBrmyEvbp4oJfamsCIQDvMTmulW/Br/gY7GOP9H/4/BIEoR7UeAYS\n"
    "4xLyu+7OEA==\n"
    "-----END CERTIFICATE-----\n");

CAR_INTERFACE_IMPL(CClient::ECDHEKeyXListener, Object, IAuthListener)

CClient::ECDHEKeyXListener::ECDHEKeyXListener(
    /* [in] */ CClient* host)
    : mSendBackKeys(TRUE)
    , mHost(host)
{
}

String CClient::ECDHEKeyXListener::GetMechanisms()
{
    return  String("ALLJOYN_ECDHE_ECDSA");
}

String CClient::ECDHEKeyXListener::GetKeyStoreFileName()
{
    AutoPtr<IFile> file;
    mHost->GetFileStreamPath(String("alljoyn_keystore"), (IFile**)&file);
    String path;
    file->GetAbsolutePath(&path);
    return path;
}

ECode CClient::ECDHEKeyXListener::Requested(
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
                    String privateKeyPEM = CLIENT_PK_PEM;
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
                    String certChainPEM = CLIENT_ECC_X509_PEM;
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

ECode CClient::ECDHEKeyXListener::Completed(
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
        // Acquire multicast lock
        Boolean doMulticastLock = TRUE;
        AutoPtr<IWifiManagerMulticastLock> multicastLock;
        if (doMulticastLock) {
            AutoPtr<IInterface> wifi;
            mHost->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&wifi);
            IWifiManager::Probe(wifi)->CreateMulticastLock(
                String("multicastLock"), (IWifiManagerMulticastLock**)&multicastLock);
            multicastLock->SetReferenceCounted(/* true */ FALSE);
            multicastLock->AcquireLock();
        }

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

         /* The following block enables the ECDHE key exchange debug messages to show in the Android log */
        Boolean daemonDebugEnabled = FALSE;
        if (daemonDebugEnabled) {
            mBus->UseOSLogging(TRUE);
            mBus->SetDaemonDebug(String("ALLJOYN_AUTH"), 3);
            mBus->SetDebugLevel(String("ALLJOYN_AUTH"), 3);
            mBus->SetDaemonDebug(String("AUTH_KEY_EXCHANGER"), 3);
            mBus->SetDebugLevel(String("AUTH_KEY_EXCHANGER"), 3);
        }

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
            loader->LoadInterface(String("Elastos.DevSamples.DFSecureDemo.ECDHEClient.ISecureInterface"),
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
            mProxyObj->GetInterface(itfcInfo, (IInterface**)&mSecureInterface);

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
const Int32 CClient::MESSAGE_PING;
const Int32 CClient::MESSAGE_PING_REPLY;
const Int32 CClient::MESSAGE_AUTH_COMPLETE;
const Int32 CClient::MESSAGE_POST_TOAST;
const Int32 CClient::MESSAGE_START_PROGRESS_DIALOG;
const Int32 CClient::MESSAGE_STOP_PROGRESS_DIALOG;
const String CClient::TAG("ECDHEClient");

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

    mAuthListener = new ECDHEKeyXListener(this);
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

} // namespace ECDHEClient
} // namespace DFSecureDemo
} // namespace DevSamples
} // namespace Elastos
