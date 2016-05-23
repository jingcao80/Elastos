
#include "elastos/droid/server/MmsServiceBroker.h"
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;

namespace Elastos {
namespace Droid {
namespace Server {

const String MmsServiceBroker::TAG("MmsServiceBroker");

static AutoPtr<IComponentName> InitMMS_SERVICE_COMPONENT()
{
    AutoPtr<IComponentName> cn;
    CComponentName::New(
        String("com.android.mms.service"),
        String("com.android.mms.service.MmsService"),
        (IComponentName**)&cn);
    return cn;
}

const AutoPtr<IComponentName> MmsServiceBroker::MMS_SERVICE_COMPONENT = InitMMS_SERVICE_COMPONENT();

const Int32 MmsServiceBroker::MSG_TRY_CONNECTING = 1;

static AutoPtr<IUri> InitUri(
    /* [in] */ const String& str)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(str, (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> MmsServiceBroker::FAKE_SMS_SENT_URI = InitUri("content://sms/sent/0");
const AutoPtr<IUri> MmsServiceBroker::FAKE_MMS_SENT_URI = InitUri("content://mms/sent/0");
const AutoPtr<IUri> MmsServiceBroker::FAKE_SMS_DRAFT_URI = InitUri("content://sms/draft/0");
const AutoPtr<IUri> MmsServiceBroker::FAKE_MMS_DRAFT_URI = InitUri("content://mms/draft/0");

const Int64 MmsServiceBroker::SERVICE_CONNECTION_WAIT_TIME_MS = 4 * 1000L; // 4 seconds
const Int64 MmsServiceBroker::RETRY_DELAY_ON_DISCONNECTION_MS = 3 * 1000L; // 3 seconds

//==========================================================================================
// MmsServiceBroker::BinderService
//==========================================================================================

CAR_INTERFACE_IMPL_2(MmsServiceBroker::BinderService, Object, IIMms, IBinder)

ECode MmsServiceBroker::BinderService::constructor(
    /* [in] */ ISystemService* mmsServiceBroker)
{
    mHost = (MmsServiceBroker*)mmsServiceBroker;
    return NOERROR;
}

//@Override
ECode MmsServiceBroker::BinderService::SendMessage(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* contentUri,
    /* [in] */ const String& locationUrl,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* sentIntent)
{
    ECode ec = mHost->mContext->EnforceCallingPermission(
        Manifest::permission::SEND_SMS, String("Send MMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    mms->SendMessage(subId, callingPkg, contentUri, locationUrl, configOverrides, sentIntent);
    return NOERROR;
}

//@Override
ECode MmsServiceBroker::BinderService::DownloadMessage(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPkg,
    /* [in] */ const String& locationUrl,
    /* [in] */ IUri* contentUri,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* downloadedIntent)
{
    ECode ec = mHost->mContext->EnforceCallingPermission(
        Manifest::permission::RECEIVE_MMS, String("Download MMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_RECEIVE_MMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->DownloadMessage(subId, callingPkg, locationUrl, contentUri,
            configOverrides, downloadedIntent);
}

//@Override
ECode MmsServiceBroker::BinderService::UpdateMmsSendStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 status)
{
    FAIL_RETURN(mHost->EnforceCarrierPrivilege())
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->UpdateMmsSendStatus(messageRef, pdu, status);
}

//@Override
ECode MmsServiceBroker::BinderService::UpdateMmsDownloadStatus(
    /* [in] */ Int32 messageRef,
    /* [in] */ Int32 status)
{
    FAIL_RETURN(mHost->EnforceCarrierPrivilege())
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->UpdateMmsDownloadStatus(messageRef, status);
}

//@Override
ECode MmsServiceBroker::BinderService::GetCarrierConfigValues(
    /* [in] */ Int64 subId,
    /* [out] */ IBundle* bundle)
{
    VALIDATE_NOT_NULL(bundle)
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->GetCarrierConfigValues(subId, bundle);
}

//@Override
ECode MmsServiceBroker::BinderService::ImportTextMessage(
    /* [in] */ const String& callingPkg,
    /* [in] */ const String& address,
    /* [in] */ Int32 type,
    /* [in] */ const String& text,
    /* [in] */ Int64 timestampMillis,
    /* [in] */ Boolean seen,
    /* [in] */ Boolean read,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = NULL;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Import SMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_WRITE_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        // Silently fail AppOps failure due to not being the default SMS app
        // while writing the TelephonyProvider
        *uri = MmsServiceBroker::FAKE_SMS_SENT_URI;
        REFCOUNT_ADD(*uri)
        return NOERROR;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->ImportTextMessage(
        callingPkg, address, type, text, timestampMillis, seen, read, uri);
}

//@Override
ECode MmsServiceBroker::BinderService::ImportMultimediaMessage(
    /* [in] */ const String& callingPkg,
    /* [in] */ Uri contentUri,
    /* [in] */ const String& messageId,
    /* [in] */ Int64 timestampSecs,
    /* [in] */ Boolean seen,
    /* [in] */ Boolean read,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = NULL;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Import SMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_WRITE_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        // Silently fail AppOps failure due to not being the default SMS app
        // while writing the TelephonyProvider
        *uri = MmsServiceBroker::FAKE_MMS_SENT_URI;
        REFCOUNT_ADD(*uri)
        return NOERROR;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->ImportMultimediaMessage(
            callingPkg, contentUri, messageId, timestampSecs, seen, read, uri);
}

//@Override
ECode MmsServiceBroker::BinderService::DeleteStoredMessage(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* messageUri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Delete SMS/MMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_WRITE_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        return FALSE;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->DeleteStoredMessage(callingPkg, messageUri, result);
}

//@Override
ECode MmsServiceBroker::BinderService::DeleteStoredConversation(
    /* [in] */ const String& callingPkg,
    /* [in] */ Int64 conversationId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Delete conversation"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_WRITE_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        return FALSE;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->DeleteStoredConversation(callingPkg, conversationId, result);
}

//@Override
ECode MmsServiceBroker::BinderService::UpdateStoredMessageStatus(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* messageUri,
    /* [in] */ IContentValues* statusValues,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Update SMS/MMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->UpdateStoredMessageStatus(callingPkg, messageUri, statusValues, result);
}

//@Override
ECode MmsServiceBroker::BinderService::ArchiveStoredConversation(
    /* [in] */ const String& callingPkg,
    /* [in] */ Int64 conversationId,
    /* [in] */ Boolean archived,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Update SMS/MMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->ArchiveStoredConversation(callingPkg, conversationId, archived);
}

//@Override
ECode MmsServiceBroker::BinderService::AddTextMessageDraft(
    /* [in] */ const String& callingPkg,
    /* [in] */ const String& address,
    /* [in] */ const String& text,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = NULL;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Add SMS draft"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_WRITE_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        // Silently fail AppOps failure due to not being the default SMS app
        // while writing the TelephonyProvider
        *uri = FAKE_SMS_DRAFT_URI;
        REFCOUNT_ADD(*uri)
        return NOERROR;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->AddTextMessageDraft(callingPkg, address, text, uri);
}

//@Override
ECode MmsServiceBroker::BinderService::AddMultimediaMessageDraft(
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* contentUri,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = NULL;

    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Add MMS draft"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_WRITE_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        // Silently fail AppOps failure due to not being the default SMS app
        // while writing the TelephonyProvider
        *uri = FAKE_MMS_DRAFT_URI;
        REFCOUNT_ADD(*uri)
        return NOERROR;
    }
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->AddMultimediaMessageDraft(callingPkg, contentUri, uri);
}

//@Override
ECode MmsServiceBroker::BinderService::SendStoredMessage(
    /* [in] */ Int64 subId,
    /* [in] */ const String& callingPkg,
    /* [in] */ IUri* messageUri,
    /* [in] */ IBundle* configOverrides,
    /* [in] */ IPendingIntent* sentIntent)
{
    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::SEND_SMS, String("Send stored MMS message"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_SEND_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->SendStoredMessage(subId, callingPkg, messageUri, configOverrides, sentIntent);
}

//@Override
ECode MmsServiceBroker::BinderService::SetAutoPersisting(
    /* [in] */ const String& callingPkg,
    /* [in] */ Boolean enabled)
{
    ECode ec = mContext->EnforceCallingPermission(
        Manifest::permission::WRITE_SMS, String("Set auto persist"));
    FAIL_RETURN(ec)

    AutoPtr<IAppOpsManager> aom = mHost->GetAppOpsManager();
    Int32 op;
    aom->NoteOp(IAppOpsManager::OP_WRITE_SMS, Binder::GetCallingUid(), callingPkg, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->SetAutoPersisting(callingPkg, enabled);
}

//@Override
ECode MmsServiceBroker::BinderService::GetAutoPersisting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIMms> mms = mHost->GetServiceGuarded();
    return mms->GetAutoPersisting(result);
}

//==========================================================================================
// MmsServiceBroker::ConnectionHandler
//==========================================================================================

MmsServiceBroker::ConnectionHandler::ConnectionHandler(
    /* [in] */ MmsServiceBroker* host)
    : mHost(host)
{}

//@Override
ECode MmsServiceBroker::ConnectionHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    switch (msg.what) {
        case MSG_TRY_CONNECTING:
            mHost->TryConnecting();
            break;
        default:
            Slogger::E(TAG, "Unknown message");
    }
    return NOERROR;
}

//==========================================================================================
// MmsServiceBroker::Connection
//==========================================================================================

CAR_INTERFACE_IMPL(MmsServiceBroker::Connection, Override, IServiceConnection)

MmsServiceBroker::Connection::Connection(
    /* [in] */ MmsServiceBroker* host)
    : mHost(host)
{}

//@Override
ECode MmsServiceBroker::Connection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    Slogger::I(TAG, "MmsService connected");
    ISynchronize* sync = ISynchronize::Probe(mHost);
    {    AutoLock syncLock(sync);
        mHost->mService = IIMms::Probe(service);
        sync->NotifyAll();
    }
    return NOERROR;
}

//@Override
ECode MmsServiceBroker::Connection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    Slogger::I(TAG, "MmsService unexpectedly disconnected");
    ISynchronize* sync = ISynchronize::Probe(mHost);
    {    AutoLock syncLock(sync);
        mHost->mService = NULL;
        sync->NotifyAll();
    }
    // Retry connecting, but not too eager (with a delay)
    // since it may come back by itself.
    AutoPtr<IMessage> msg;
    mHost->mConnectionHandler->ObtainMessage(MSG_TRY_CONNECTING, (IMessage**)&msg);
    Boolean bval;
    return mHost->mConnectionHandler->SendMessageDelayed(
        msg, RETRY_DELAY_ON_DISCONNECTION_MS, &bval);
}

//==========================================================================================
// MmsServiceBroker
//==========================================================================================
MmsServiceBroker::MmsServiceBroker()
{}

ECode MmsServiceBroker::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    mContext = context;
    mService = NULL;
    return NOERROR;
}

//@Override
ECode MmsServiceBroker::OnStart()
{
    AutoPtr<IBinder> service;
    CMmsServiceBrokerBinderService::New((IBinder**)&service);
    PublishBinderService(String("imms"), service);
    return NOERROR;
}

ECode MmsServiceBroker::SystemRunning()
{
    TryConnecting();
    return NOERROR;
}

ECode MmsServiceBroker::TryConnecting()
{
    ECode ec = NOERROR;
    Slogger::I(TAG, "Connecting to MmsService");
    {    AutoLock syncLock(this);
        if (mService != NULL) {
            Slogger::D(TAG, "Already connected");
            return;
        }
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetComponent(MMS_SERVICE_COMPONENT);
        Boolean bval;
        ECode ec = mContext->BindService(intent, mConnection, Context.BIND_AUTO_CREATE, &bval);
        if (ec == (ECode)E_SECURITY_EXCEPTION) {
            Slogger::E(TAG, "Forbidden to bind to MmsService");
            return ec;
        }
        else if (!bval) {
            Slogger::E(TAG, "Failed to bind to MmsService");
        }
    }
    return ec;
}

ECode MmsServiceBroker::EnsureService()
{
    {    AutoLock syncLock(this);
        if (mService == NULL) {
            // Service is not connected. Try blocking connecting.
            Slogger::W(TAG, "MmsService not connected. Try connecting...");
            AutoPtr<IMessage> msg;
            mConnectionHandler->ObtainMessage(MSG_TRY_CONNECTING, (IMessage**)&msg);
            mConnectionHandler->SendMessage(msg, &bval);
            Int64 shouldEnd = SystemClock::GetElapsedRealtime() + SERVICE_CONNECTION_WAIT_TIME_MS;
            Int64 waitTime = SERVICE_CONNECTION_WAIT_TIME_MS;
            while (waitTime > 0) {
                // try {
                    // TODO: consider using Java concurrent construct instead of raw object wait
                ECode ec = Wait(waitTime);
                if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    Slogger::W(TAG, "Connection wait interrupted");
                }
                if (mService != NULL) {
                    // Success
                    return;
                }
                // Calculate remaining waiting time to make sure we wait the full timeout period
                waitTime = shouldEnd - SystemClock::GetElapsedRealtime();
            }
            // Timed out. Something's really wrong.
            Slogger::E(TAG, "Can not connect to MmsService (timed out)");
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

/**
 * Making sure when we obtain the mService instance it is always valid.
 * Throws {@link RuntimeException} when it is empty.
 */
AutoPtr<IIMms> MmsServiceBroker::GetServiceGuarded()
{
    EnsureService();
    return mService;
}

AutoPtr<IAppOpsManager> MmsServiceBroker::GetAppOpsManager()
{
    if (mAppOpsManager == NULL) {
        mAppOpsManager = (AppOpsManager) mContext->GetSystemService(Context.APP_OPS_SERVICE);
    }
    return mAppOpsManager;
}

AutoPtr<IPackageManager> MmsServiceBroker::GetPackageManager()
{
    if (mPackageManager == NULL) {
        mPackageManager = mContext->GetPackageManager();
    }
    return mPackageManager;
}

AutoPtr<ITelephonyManager> MmsServiceBroker::GetTelephonyManager()
{
    if (mTelephonyManager == NULL) {
        mTelephonyManager = (TelephonyManager) mContext->GetSystemService(
                Context.TELEPHONY_SERVICE);
    }
    return mTelephonyManager;
}

/*
 * Throws a security exception unless the caller has carrier privilege.
 */
ECode MmsServiceBroker::EnforceCarrierPrivilege()
{
    String[] packages = GetPackageManager()->GetPackagesForUid(Binder::GetCallingUid());
    for (String pkg : packages) {
        if (GetTelephonyManager()->CheckCarrierPrivilegesForPackage(pkg) ==
                TelephonyManager.CARRIER_PRIVILEGE_STATUS_HAACCESS) {
            return;
        }
    }
    throw new SecurityException("No carrier privilege");
}


} // namespace Server
} // namespace Droid
} // namespace Elastos
