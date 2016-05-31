
#include "elastos/droid/server/pm/CPackageInstallerService.h"
#include "elastos/droid/server/pm/CPackageInstallerSession.h"
#include "elastos/droid/server/IoThread.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SELinux.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/system/Os.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Graphics.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/etl/Set.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionParams;
using Elastos::Droid::Content::Pm::CPackageInstallerSessionParams;
using Elastos::Droid::Content::Pm::CParceledListSlice;
using Elastos::Droid::Content::Pm::IPackageManagerHelper;
using Elastos::Droid::Content::Pm::CPackageManagerHelper;
using Elastos::Droid::Content::Pm::IPackageInstaller;
using Elastos::Droid::Content::Pm::EIID_IIPackageInstaller;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Internal::Content::CPackageHelper;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SELinux;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Server::IoThread;
using Elastos::Droid::System::Os;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IThread;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::EIID_IFilenameFilter;
using Elastos::IO::IFileInputStream;
// using Elastos::Security::CSecureRandom;
using Elastos::Utility::Objects;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CRandom;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Etl::Set;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

//==============================================================================
//                  CPackageInstallerService::StageFilter
//==============================================================================

CAR_INTERFACE_IMPL(CPackageInstallerService::StageFilter, Object, IFilenameFilter)

ECode CPackageInstallerService::StageFilter::Accept(
    /* [in] */ IFile* dir,
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    *succeeded = CPackageInstallerService::IsStageName(name);
    return NOERROR;
}


//==============================================================================
//                  CPackageInstallerService::WriteSessionsRunnable
//==============================================================================

ECode CPackageInstallerService::WriteSessionsRunnable::Run()
{
    Object& lock = mHost->mSessionsLock;
    {    AutoLock syncLock(lock);
        mHost->WriteSessionsLocked();
    }
    return NOERROR;
}


//==============================================================================
//                  CPackageInstallerService::PackageDeleteObserverAdapter
//==============================================================================

ECode CPackageInstallerService::PackageDeleteObserverAdapter::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IIntent> fillIn;
    CIntent::New((IIntent**)&fillIn);
    fillIn->PutExtra(IPackageInstaller::EXTRA_PACKAGE_NAME, mPackageName);
    fillIn->PutExtra(IPackageInstaller::EXTRA_STATUS, IPackageInstaller::STATUS_PENDING_USER_ACTION);
    fillIn->PutExtra(IIntent::EXTRA_INTENT, IParcelable::Probe(intent));
    // try {
    mTarget->SendIntent(mContext, 0, fillIn, NULL, NULL);
    // } catch (SendIntentException ignored) {
    // }
    Release();
    return NOERROR;
}

ECode CPackageInstallerService::PackageDeleteObserverAdapter::OnPackageDeleted(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg)
{
    AutoPtr<IIntent> fillIn;
    CIntent::New((IIntent**)&fillIn);
    fillIn->PutExtra(IPackageInstaller::EXTRA_PACKAGE_NAME, mPackageName);
    AutoPtr<IPackageManagerHelper> helper;
    CPackageManagerHelper::AcquireSingleton((IPackageManagerHelper**)&helper);
    Int32 value;
    helper->DeleteStatusToPublicStatus(returnCode, &value);
    fillIn->PutExtra(IPackageInstaller::EXTRA_STATUS, value);
    String str;
    helper->DeleteStatusToString(returnCode, msg, &str);
    fillIn->PutExtra(IPackageInstaller::EXTRA_STATUS_MESSAGE, str);
    fillIn->PutExtra(IPackageInstaller::EXTRA_LEGACY_STATUS, returnCode);
    // try {
    mTarget->SendIntent(mContext, 0, fillIn, NULL, NULL);
    // } catch (SendIntentException ignored) {
    // }
    Release();
    return NOERROR;
}


//==============================================================================
//                  CPackageInstallerService::PackageInstallObserverAdapter
//==============================================================================

ECode CPackageInstallerService::PackageInstallObserverAdapter::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IIntent> fillIn;
    CIntent::New((IIntent**)&fillIn);
    fillIn->PutExtra(IPackageInstaller::EXTRA_SESSION_ID, mSessionId);
    fillIn->PutExtra(IPackageInstaller::EXTRA_STATUS, IPackageInstaller::STATUS_PENDING_USER_ACTION);
    fillIn->PutExtra(IIntent::EXTRA_INTENT, IParcelable::Probe(intent));
    // try {
    mTarget->SendIntent(mContext, 0, fillIn, NULL, NULL);
    // } catch (SendIntentException ignored) {
    // }
    Release();
    return NOERROR;
}

ECode CPackageInstallerService::PackageInstallObserverAdapter::OnPackageInstalled(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IIntent> fillIn;
    CIntent::New((IIntent**)&fillIn);
    fillIn->PutExtra(IPackageInstaller::EXTRA_SESSION_ID, mSessionId);
    AutoPtr<IPackageManagerHelper> helper;
    CPackageManagerHelper::AcquireSingleton((IPackageManagerHelper**)&helper);
    Int32 value;
    helper->DeleteStatusToPublicStatus(returnCode, &value);
    fillIn->PutExtra(IPackageInstaller::EXTRA_STATUS, value);
    String str;
    helper->DeleteStatusToString(returnCode, msg, &str);
    fillIn->PutExtra(IPackageInstaller::EXTRA_STATUS_MESSAGE, str);
    fillIn->PutExtra(IPackageInstaller::EXTRA_LEGACY_STATUS, returnCode);
    if (extras != NULL) {
        String existing;
        extras->GetString(IPackageManager::EXTRA_FAILURE_EXISTING_PACKAGE, &existing);
        if (!TextUtils::IsEmpty(existing)) {
            fillIn->PutExtra(IPackageInstaller::EXTRA_OTHER_PACKAGE_NAME, existing);
        }
    }
    // try {
    mTarget->SendIntent(mContext, 0, fillIn, NULL, NULL);
    // } catch (SendIntentException ignored) {
    // }
    Release();
    return NOERROR;
}


//==============================================================================
//                  CPackageInstallerService::Callbacks
//==============================================================================

const Int32 CPackageInstallerService::Callbacks::MSG_SESSION_CREATED;
const Int32 CPackageInstallerService::Callbacks::MSG_SESSION_BADGING_CHANGED;
const Int32 CPackageInstallerService::Callbacks::MSG_SESSION_ACTIVE_CHANGED;
const Int32 CPackageInstallerService::Callbacks::MSG_SESSION_PROGRESS_CHANGED;
const Int32 CPackageInstallerService::Callbacks::MSG_SESSION_FINISHED;

CPackageInstallerService::Callbacks::Callbacks(
    /* [in] */ ILooper* looper)
    : Handler(looper)
{
    CRemoteCallbackList::New((IRemoteCallbackList**)&mCallbacks);
}

void CPackageInstallerService::Callbacks::Register(
    /* [in] */ IPackageInstallerCallback* callback,
    /* [in] */ Int32 userId)
{
    AutoPtr<IUserHandle> userH;
    CUserHandle::New(userId, (IUserHandle**)&userH);
    Boolean result;
    mCallbacks->Register(callback, userH, &result);
}

void CPackageInstallerService::Callbacks::Unregister(
    /* [in] */ IPackageInstallerCallback* callback)
{
    Boolean result;
    mCallbacks->Unregister(callback, &result);
}

ECode CPackageInstallerService::Callbacks::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 userId;
    msg->GetArg2(&userId);
    Int32 n;
    mCallbacks->BeginBroadcast(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> item;
        mCallbacks->GetBroadcastItem(i, (IInterface**)&item);
        AutoPtr<IPackageInstallerCallback> callback = IPackageInstallerCallback::Probe(item);
        AutoPtr<IInterface> cookie;
        mCallbacks->GetBroadcastCookie(i, (IInterface**)&cookie);
        AutoPtr<IUserHandle> user = IUserHandle::Probe(cookie);
        // TODO: dispatch notifications for slave profiles
        Int32 id;
        if (user->GetIdentifier(&id), userId == id) {
            // try {
            InvokeCallback(callback, msg);
            // } catch (RemoteException ignored) {
            // }
        }
    }
    return mCallbacks->FinishBroadcast();
}

ECode CPackageInstallerService::Callbacks::InvokeCallback(
    /* [in] */ IPackageInstallerCallback* callback,
    /* [in] */ IMessage* msg)
{
    Int32 sessionId;
    msg->GetArg1(&sessionId);
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_SESSION_CREATED:
            return callback->OnSessionCreated(sessionId);
        case MSG_SESSION_BADGING_CHANGED:
            return callback->OnSessionBadgingChanged(sessionId);
        case MSG_SESSION_ACTIVE_CHANGED: {
            AutoPtr<IInterface> obj;
            Boolean value;
            IBoolean::Probe(obj)->GetValue(&value);
            return callback->OnSessionActiveChanged(sessionId, value);
        }
        case MSG_SESSION_PROGRESS_CHANGED: {
            AutoPtr<IInterface> obj;
            Float value;
            IFloat::Probe(obj)->GetValue(&value);
            return callback->OnSessionProgressChanged(sessionId, value);
        }
        case MSG_SESSION_FINISHED: {
            AutoPtr<IInterface> obj;
            Boolean value;
            IBoolean::Probe(obj)->GetValue(&value);
            return callback->OnSessionFinished(sessionId, value);
        }
    }
    return NOERROR;
}

void CPackageInstallerService::Callbacks::NotifySessionCreated(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 userId)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_SESSION_CREATED, sessionId, userId, (IMessage**)&msg);
    msg->SendToTarget();
}

void CPackageInstallerService::Callbacks::NotifySessionBadgingChanged(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 userId)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_SESSION_BADGING_CHANGED, sessionId, userId, (IMessage**)&msg);
    msg->SendToTarget();
}

void CPackageInstallerService::Callbacks::NotifySessionActiveChanged(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean active)
{
    AutoPtr<IMessage> msg;
    AutoPtr<IBoolean> b;
    CBoolean::New(active, (IBoolean**)&b);
    ObtainMessage(MSG_SESSION_ACTIVE_CHANGED, sessionId, userId, b, (IMessage**)&msg);
    msg->SendToTarget();
}

void CPackageInstallerService::Callbacks::NotifySessionProgressChanged(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 userId,
    /* [in] */ Float progress)
{
    AutoPtr<IMessage> msg;
    AutoPtr<IFloat> f;
    CFloat::New(progress, (IFloat**)&f);
    ObtainMessage(MSG_SESSION_PROGRESS_CHANGED, sessionId, userId, f, (IMessage**)&msg);
    msg->SendToTarget();
}

void CPackageInstallerService::Callbacks::NotifySessionFinished(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean success)
{
    AutoPtr<IMessage> msg;
    AutoPtr<IBoolean> b;
    CBoolean::New(success, (IBoolean**)&b);
    ObtainMessage(MSG_SESSION_FINISHED, sessionId, userId, b, (IMessage**)&msg);
    msg->SendToTarget();
}


//==============================================================================
//                  InternalCallback::SessionFinishedRunnable
//==============================================================================

ECode InternalCallback::SessionFinishedRunnable::Run()
{
    Object& lock = mHost->mSessionsLock;
    {    AutoLock syncLock(lock);
        mHost->mSessions.Erase(mSession->mSessionId);
        mHost->mHistoricalSessions[mSession->mSessionId] = mSession;

        AutoPtr<IFile> appIconFile = mHost->BuildAppIconFile(mSession->mSessionId);
        Boolean exists;
        if (appIconFile->Exists(&exists), exists) {
            appIconFile->Delete();
        }

        mHost->WriteSessionsLocked();
    }
    return NOERROR;
}


//==============================================================================
//                  InternalCallback
//==============================================================================

void InternalCallback::OnSessionBadgingChanged(
    /* [in] */ CPackageInstallerSession* session)
{
    mHost->mCallbacks->NotifySessionBadgingChanged(session->mSessionId, session->mUserId);
    mHost->WriteSessionsAsync();
}

void InternalCallback::OnSessionActiveChanged(
    /* [in] */ CPackageInstallerSession* session,
    /* [in] */ Boolean active)
{
    mHost->mCallbacks->NotifySessionActiveChanged(session->mSessionId, session->mUserId, active);
}

void InternalCallback::OnSessionProgressChanged(
    /* [in] */ CPackageInstallerSession* session,
    /* [in] */ Float progress)
{
    mHost->mCallbacks->NotifySessionProgressChanged(session->mSessionId, session->mUserId, progress);
}

void InternalCallback::OnSessionFinished(
    /* [in] */ CPackageInstallerSession* session,
    /* [in] */ Boolean success)
{
    mHost->mCallbacks->NotifySessionFinished(session->mSessionId, session->mUserId, success);

    AutoPtr<IRunnable> runnable = new SessionFinishedRunnable(mHost, session);
    Boolean result;
    mHost->mInstallHandler->Post(runnable, &result);
}

void InternalCallback::OnSessionPrepared(
    /* [in] */ CPackageInstallerSession* session)
{
    // We prepared the destination to write into; we want to persist
    // this, but it's not critical enough to block for.
    mHost->WriteSessionsAsync();
}

void InternalCallback::OnSessionSealedBlocking(
    /* [in] */ CPackageInstallerSession* session)
{
    // It's very important that we block until we've recorded the
    // session as being sealed, since we never want to allow mutation
    // after sealing.
    Object& lock = mHost->mSessionsLock;
    {    AutoLock syncLock(lock);
        mHost->WriteSessionsLocked();
    }
}


//==============================================================================
//                  CPackageInstallerService
//==============================================================================

const String CPackageInstallerService::TAG("CPackageInstallerService");
const Boolean CPackageInstallerService::LOGD = TRUE;
const String CPackageInstallerService::TAG_SESSIONS("sessions");
const String CPackageInstallerService::TAG_SESSION("session");
const String CPackageInstallerService::ATTR_SESSION_ID("sessionId");
const String CPackageInstallerService::ATTR_USER_ID("userId");
const String CPackageInstallerService::ATTR_INSTALLER_PACKAGE_NAME("installerPackageName");
const String CPackageInstallerService::ATTR_INSTALLER_UID("installerUid");
const String CPackageInstallerService::ATTR_CREATED_MILLIS("createdMillis");
const String CPackageInstallerService::ATTR_SESSION_STAGE_DIR("sessionStageDir");
const String CPackageInstallerService::ATTR_SESSION_STAGE_CID("sessionStageCid");
const String CPackageInstallerService::ATTR_PREPARED("prepared");
const String CPackageInstallerService::ATTR_SEALED("sealed");
const String CPackageInstallerService::ATTR_MODE("mode");
const String CPackageInstallerService::ATTR_INSTALL_FLAGS("installFlags");
const String CPackageInstallerService::ATTR_INSTALL_LOCATION("installLocation");
const String CPackageInstallerService::ATTR_SIZE_BYTES("sizeBytes");
const String CPackageInstallerService::ATTR_APP_PACKAGE_NAME("appPackageName");
const String CPackageInstallerService::ATTR_APP_ICON("appIcon");
const String CPackageInstallerService::ATTR_APP_LABEL("appLabel");
const String CPackageInstallerService::ATTR_ORIGINATING_URI("originatingUri");
const String CPackageInstallerService::ATTR_REFERRER_URI("referrerUri");
const String CPackageInstallerService::ATTR_ABI_OVERRIDE("abiOverride");
const Int64 CPackageInstallerService::MAX_AGE_MILLIS = 3 * IDateUtils::DAY_IN_MILLIS;
const Int64 CPackageInstallerService::MAX_ACTIVE_SESSIONS;
const Int64 CPackageInstallerService::MAX_HISTORICAL_SESSIONS;

AutoPtr<IFilenameFilter> CPackageInstallerService::InitStageFilter()
{
    return (IFilenameFilter*)new StageFilter();
}
AutoPtr<IFilenameFilter> CPackageInstallerService::sStageFilter = InitStageFilter();

CAR_INTERFACE_IMPL_2(CPackageInstallerService, Object, IIPackageInstaller, IBinder)

CAR_OBJECT_IMPL(CPackageInstallerService)

CPackageInstallerService::CPackageInstallerService()
{
}

ECode CPackageInstallerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIPackageManager* pm,
    /* [in] */ IFile* stagingDir)
{
    mInternalCallback = new InternalCallback(this);
    Slogger::D(TAG, "TODO: CSecureRandom has not been realized!");
    // CSecureRandom::New((IRandom**)&mRandom);
    CRandom::New((IRandom**)&mRandom);

    mContext = context;
    mPm = (CPackageManagerService*)pm;
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&service);
    mAppOps = IAppOpsManager::Probe(service);

    mStagingDir = stagingDir;

    CHandlerThread::New(TAG, (IHandlerThread**)&mInstallThread);
    IThread::Probe(mInstallThread)->Start();

    AutoPtr<ILooper> looper;
    mInstallThread->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mInstallHandler);

    mCallbacks = new Callbacks(looper);

    AutoPtr<IFile> dir = Environment::GetSystemSecureDirectory();
    AutoPtr<IFile> file;
    CFile::New(dir, String("install_sessions.xml"), (IFile**)&file);
    CAtomicFile::New(file, (IAtomicFile**)&mSessionsFile);
    CFile::New(dir, String("install_sessions"), (IFile**)&mSessionsDir);
    Boolean result;
    mSessionsDir->Mkdirs(&result);

    {    AutoLock syncLock(mSessionsLock);
        ReadSessionsLocked();
        AutoPtr<ArrayOf<IFile*> > files;
        mStagingDir->ListFiles(sStageFilter, (ArrayOf<IFile*>**)&files);
        Set<AutoPtr<IFile> > unclaimedStages;
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            unclaimedStages.Insert((*files)[i]);
        }
        files = NULL;
        mSessionsDir->ListFiles((ArrayOf<IFile*>**)&files);
        Set<AutoPtr<IFile> > unclaimedIcons;
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            unclaimedIcons.Insert((*files)[i]);
        }

        // Ignore stages and icons claimed by active sessions
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Begin();
        for (; it != mSessions.End(); ++it) {
            AutoPtr<CPackageInstallerSession> session = it->mSecond;
            unclaimedStages.Erase(session->mStageDir);
            AutoPtr<IFile> iconF = BuildAppIconFile(session->mSessionId);
            unclaimedIcons.Erase(iconF);
        }

        // Clean up orphaned staging directories
        Set<AutoPtr<IFile> >::Iterator stageIt = unclaimedStages.Begin();
        for (; stageIt != unclaimedStages.End(); ++stageIt) {
            AutoPtr<IFile> stage = *stageIt;
            Slogger::W(TAG, "Deleting orphan stage %p", stage.Get());
            Boolean isDirectory;
            if (stage->IsDirectory(&isDirectory), isDirectory) {
                FileUtils::DeleteContents(stage);
            }
            stage->Delete();
        }

        // Clean up orphaned icons
        Set<AutoPtr<IFile> >::Iterator iconIt = unclaimedIcons.Begin();
        for (; iconIt != unclaimedIcons.End(); ++iconIt) {
            AutoPtr<IFile> icon = *iconIt;
            Slogger::W(TAG, "Deleting orphan icon %p", icon.Get());
            icon->Delete();
        }
    }
    return NOERROR;
}

void CPackageInstallerService::OnSecureContainersAvailable()
{
    {    AutoLock syncLock(mSessionsLock);
        HashSet<String> unclaimed;
        AutoPtr<IPackageHelper> helper;
        CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
        AutoPtr<ArrayOf<String> > list;
        helper->GetSecureContainerList((ArrayOf<String>**)&list);
        for (Int32 i = 0; i < list->GetLength(); ++i) {
            String cid = (*list)[i];
            if (IsStageName(cid)) {
                unclaimed.Insert(cid);
            }
        }

        // Ignore stages claimed by active sessions
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Begin();
        for (; it != mSessions.End(); ++it) {
            AutoPtr<CPackageInstallerSession> session = it->mSecond;
            String cid = session->mStageCid;

            if (unclaimed.Find(cid) != unclaimed.End()) {
                unclaimed.Erase(cid);
                // Claimed by active session, mount it
                String dir;
                helper->MountSdDir(cid, CPackageManagerService::GetEncryptKey(), IProcess::SYSTEM_UID, &dir);
            }
        }

        // Clean up orphaned staging containers
        HashSet<String>::Iterator cidIt = unclaimed.Begin();
        for (; cidIt != unclaimed.End(); ++cidIt) {
            String cid = *cidIt;
            Slogger::W(TAG, "Deleting orphan container %s", cid.string());
            Boolean result;
            helper->DestroySdDir(cid, &result);
        }
    }
}

Boolean CPackageInstallerService::IsStageName(
    /* [in] */ const String& name)
{
    Boolean isFile = name.StartWith("vmdl") && name.EndWith(".tmp");
    Boolean isContainer = name.StartWith("smdl") && name.EndWith(".tmp");
    Boolean isLegacyContainer = name.StartWith("smdl2tmp");
    return isFile || isContainer || isLegacyContainer;
}

ECode CPackageInstallerService::AllocateInternalStageDirLegacy(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    *dir = NULL;
    {    AutoLock syncLock(mSessionsLock);
        // try {
        Int32 sessionId;
        if (FAILED(AllocateSessionIdLocked(&sessionId))) {
            return E_IO_EXCEPTION;
        }
        mLegacySessions[sessionId] = TRUE;
        AutoPtr<IFile> stageDir = BuildInternalStageDir(sessionId);
        PrepareInternalStageDir(stageDir);
        *dir = stageDir;
        REFCOUNT_ADD(*dir)
        // } catch (IllegalStateException e) {
        //     throw new IOException(e);
        // }
    }
    return NOERROR;
}

String CPackageInstallerService::AllocateExternalStageCidLegacy()
{
    String value;
    {    AutoLock syncLock(mSessionsLock);
        Int32 sessionId;
        AllocateSessionIdLocked(&sessionId);
        mLegacySessions[sessionId] = TRUE;
        value = String("smdl") + StringUtils::ToString(sessionId) + ".tmp";
    }
    return value;
}

void CPackageInstallerService::ReadSessionsLocked()
{
    if (LOGD) Slogger::V(TAG, "readSessionsLocked()");

    mSessions.Clear();

    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);

    AutoPtr<IFileInputStream> fis;
    AutoPtr<IXmlPullParser> in;
    AutoPtr<ICloseable> closeFis;
    Int32 type;

    // try {
    ECode ec = mSessionsFile->OpenRead((IFileInputStream**)&fis);
    FAIL_GOTO(ec, _EXIT_)

    ec = Xml::NewPullParser((IXmlPullParser**)&in);
    FAIL_GOTO(ec, _EXIT_)

    in->SetInput(IInputStream::Probe(fis), String(NULL));

    closeFis = ICloseable::Probe(fis);

    while (in->Next(&type), type != IXmlPullParser::END_DOCUMENT) {
        if (type == IXmlPullParser::START_TAG) {
            String tag;
            in->GetName(&tag);
            if (TAG_SESSION.Equals(tag)) {
                AutoPtr<CPackageInstallerSession> session;
                if (FAILED(ReadSessionLocked(in, (CPackageInstallerSession**)&session))) {
                    Slogger::E(TAG, "Failed reading install sessions");
                    ioutils->CloseQuietly(closeFis);
                    return;
                }
                AutoPtr<ISystem> sys;
                CSystem::AcquireSingleton((ISystem**)&sys);
                Int64 millis;
                sys->GetCurrentTimeMillis(&millis);
                Int64 age = millis - session->mCreatedMillis;

                Boolean valid, exists;
                if (age >= MAX_AGE_MILLIS) {
                    Slogger::W(TAG, "Abandoning old session first created at %d", session->mCreatedMillis);
                    valid = FALSE;
                }
                else if (session->mStageDir != NULL && (session->mStageDir->Exists(&exists), !exists)) {
                    Slogger::W(TAG, "Abandoning internal session with missing stage %p",
                            session->mStageDir.Get());
                    valid = FALSE;
                }
                else {
                    valid = TRUE;
                }

                if (valid) {
                    mSessions[session->mSessionId] = session;
                }
                else {
                    // Since this is early during boot we don't send
                    // any observer events about the session, but we
                    // keep details around for dumpsys.
                    mHistoricalSessions[session->mSessionId] = session;
                }
            }
        }
    }

_EXIT_:
    ioutils->CloseQuietly(closeFis);

    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        // Missing sessions are okay, probably first boot
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Failed reading install sessions. E_IO_EXCEPTION");
    }
    else if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Slogger::E(TAG, "Failed reading install sessions. E_XML_PULL_PARSER_EXCEPTION");
    }
}

ECode CPackageInstallerService::ReadSessionLocked(
    /* [in] */ IXmlPullParser* in,
    /* [out] */ CPackageInstallerSession** session)
{
    VALIDATE_NOT_NULL(session)
    *session = NULL;

    Int32 sessionId;
    FAIL_RETURN(XmlUtils::ReadInt32Attribute(in, ATTR_SESSION_ID, &sessionId))
    Int32 userId;
    FAIL_RETURN(XmlUtils::ReadInt32Attribute(in, ATTR_USER_ID, &userId))
    String installerPackageName;
    FAIL_RETURN(XmlUtils::ReadStringAttribute(in, ATTR_INSTALLER_PACKAGE_NAME, &installerPackageName))
    Int32 uid;
    FAIL_RETURN(mPm->GetPackageUid(installerPackageName, userId, &uid))
    Int32 installerUid;
    FAIL_RETURN(XmlUtils::ReadInt32Attribute(in, ATTR_INSTALLER_UID, uid, &installerUid))
    Int64 createdMillis;
    FAIL_RETURN(XmlUtils::ReadInt64Attribute(in, ATTR_CREATED_MILLIS, &createdMillis))
    String stageDirRaw;
    FAIL_RETURN(XmlUtils::ReadStringAttribute(in, ATTR_SESSION_STAGE_DIR, &stageDirRaw))
    AutoPtr<IFile> stageDir;
    if (!stageDirRaw.IsNull()) {
        CFile::New(stageDirRaw, (IFile**)&stageDir);
    }
    String stageCid;
    FAIL_RETURN(XmlUtils::ReadStringAttribute(in, ATTR_SESSION_STAGE_CID, &stageCid))
    Boolean prepared;
    FAIL_RETURN(XmlUtils::ReadBooleanAttribute(in, ATTR_PREPARED, TRUE, &prepared))
    Boolean sealed;
    FAIL_RETURN(XmlUtils::ReadBooleanAttribute(in, ATTR_SEALED, &sealed))

    AutoPtr<IPackageInstallerSessionParams> params;
    CPackageInstallerSessionParams::New(IPackageInstallerSessionParams::MODE_INVALID,
            (IPackageInstallerSessionParams**)&params);
    Int32 mode;
    FAIL_RETURN(XmlUtils::ReadInt32Attribute(in, ATTR_MODE, &mode))
    params->SetMode(mode);
    Int32 installFlags;
    FAIL_RETURN(XmlUtils::ReadInt32Attribute(in, ATTR_INSTALL_FLAGS, &installFlags))
    params->SetInstallFlags(installFlags);
    Int32 installLocation;
    FAIL_RETURN(XmlUtils::ReadInt32Attribute(in, ATTR_INSTALL_LOCATION, &installLocation))
    params->SetInstallLocation(installLocation);
    Int64 sizeBytes;
    FAIL_RETURN(XmlUtils::ReadInt64Attribute(in, ATTR_SIZE_BYTES, &sizeBytes))
    params->SetSizeBytes(sizeBytes);
    String appPackageName;
    FAIL_RETURN(XmlUtils::ReadStringAttribute(in, ATTR_APP_PACKAGE_NAME, &appPackageName))
    params->SetAppPackageName(appPackageName);
    AutoPtr<IBitmap> appIcon;
    FAIL_RETURN(XmlUtils::ReadBitmapAttribute(in, ATTR_APP_ICON, (IBitmap**)&appIcon))
    params->SetAppIcon(appIcon);
    String appLabel;
    FAIL_RETURN(XmlUtils::ReadStringAttribute(in, ATTR_APP_LABEL, &appLabel))
    AutoPtr<ICharSequence> cs;
    CString::New(appLabel, (ICharSequence**)&cs);
    params->SetAppLabel(cs);
    AutoPtr<IUri> originatingUri;
    FAIL_RETURN(XmlUtils::ReadUriAttribute(in, ATTR_APP_LABEL, (IUri**)&originatingUri))
    params->SetOriginatingUri(originatingUri);
    AutoPtr<IUri> referrerUri;
    FAIL_RETURN(XmlUtils::ReadUriAttribute(in, ATTR_REFERRER_URI, (IUri**)&referrerUri))
    params->SetReferrerUri(referrerUri);
    String abiOverride;
    FAIL_RETURN(XmlUtils::ReadStringAttribute(in, ATTR_ABI_OVERRIDE, &abiOverride))
    params->SetAbiOverride(abiOverride);

    AutoPtr<IFile> appIconFile = BuildAppIconFile(sessionId);
    Boolean exists;
    if (appIconFile->Exists(&exists), exists) {
        String absolutePath;
        appIconFile->GetAbsolutePath(&absolutePath);
        AutoPtr<IBitmapFactory> bmf;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bmf);
        appIcon = NULL;
        bmf->DecodeFile(absolutePath, (IBitmap**)&appIcon);
        params->SetAppIcon(appIcon);
        Int64 lastModified;
        appIconFile->GetLastModified(&lastModified);
        params->SetAppIconLastModified(lastModified);
    }

    AutoPtr<ILooper> looper;
    mInstallHandler->GetLooper((ILooper**)&looper);
    return CPackageInstallerSession::NewByFriend((Handle64)mInternalCallback.Get(), mContext, mPm,
            looper, sessionId, userId, installerPackageName, installerUid,
            params, createdMillis, stageDir, stageCid, prepared, sealed, session);
}

void CPackageInstallerService::WriteSessionsLocked()
{
    if (LOGD) Slogger::V(TAG, "writeSessionsLocked()");

    AutoPtr<IFileOutputStream> fos;
    // try {
    do {
        if (FAILED(mSessionsFile->StartWrite((IFileOutputStream**)&fos))) {
            break;
        }

        AutoPtr<IXmlSerializer> out;
        CFastXmlSerializer::New((IXmlSerializer**)&out);
        if (FAILED(out->SetOutput(IOutputStream::Probe(fos), String("utf-8")))) {
            break;
        }
        if (FAILED(out->StartDocument(String(NULL), TRUE))) {
            break;
        }
        if (FAILED(out->WriteStartTag(String(NULL), TAG_SESSIONS))) {
            break;
        }
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Begin();
        for (; it != mSessions.End(); ++it) {
            if (FAILED(WriteSessionLocked(out, it->mSecond))) {
                break;
            }
        }
        if (FAILED(out->WriteEndTag(String(NULL), TAG_SESSIONS))) {
            break;
        }
        if (FAILED(out->EndDocument())) {
            break;
        }

        mSessionsFile->FinishWrite(fos);
        return;
    } while (0);

    // } catch (IOException e) {
    //     if (fos != null) {
    //         mSessionsFile.failWrite(fos);
    //     }
    // }
    if (fos != NULL) {
        mSessionsFile->FailWrite(fos);
    }
}

ECode CPackageInstallerService::WriteSessionLocked(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ CPackageInstallerSession* session)
{
    AutoPtr<IPackageInstallerSessionParams> params = session->mParams;

    FAIL_RETURN(out->WriteStartTag(String(NULL), TAG_SESSION))

    FAIL_RETURN(XmlUtils::WriteInt32Attribute(out, ATTR_SESSION_ID, session->mSessionId))
    FAIL_RETURN(XmlUtils::WriteInt32Attribute(out, ATTR_USER_ID, session->mUserId))
    FAIL_RETURN(XmlUtils::WriteStringAttribute(out, ATTR_INSTALLER_PACKAGE_NAME,
            session->mInstallerPackageName))
    FAIL_RETURN(XmlUtils::WriteInt32Attribute(out, ATTR_INSTALLER_UID, session->mInstallerUid))
    FAIL_RETURN(XmlUtils::WriteInt64Attribute(out, ATTR_CREATED_MILLIS, session->mCreatedMillis))
    if (session->mStageDir != NULL) {
        String absolutePath;
        session->mStageDir->GetAbsolutePath(&absolutePath);
        FAIL_RETURN(XmlUtils::WriteStringAttribute(out, ATTR_SESSION_STAGE_DIR, absolutePath))
    }
    if (!session->mStageCid.IsNull()) {
        FAIL_RETURN(XmlUtils::WriteStringAttribute(out, ATTR_SESSION_STAGE_CID, session->mStageCid))
    }
    FAIL_RETURN(XmlUtils::WriteBooleanAttribute(out, ATTR_PREPARED, session->IsPrepared()))
    FAIL_RETURN(XmlUtils::WriteBooleanAttribute(out, ATTR_SEALED, session->IsSealed()))

    Int32 mode;
    params->GetMode(&mode);
    FAIL_RETURN(XmlUtils::WriteInt32Attribute(out, ATTR_MODE, mode))
    Int32 installFlags;
    params->GetInstallFlags(&installFlags);
    FAIL_RETURN(XmlUtils::WriteInt32Attribute(out, ATTR_INSTALL_FLAGS, installFlags))
    Int32 installLocation;
    params->GetInstallLocation(&installLocation);
    FAIL_RETURN(XmlUtils::WriteInt32Attribute(out, ATTR_INSTALL_LOCATION, installLocation))
    Int64 sizeBytes;
    params->GetSizeBytes(&sizeBytes);
    FAIL_RETURN(XmlUtils::WriteInt64Attribute(out, ATTR_SIZE_BYTES, sizeBytes))
    String appPackageName;
    params->GetAppPackageName(&appPackageName);
    FAIL_RETURN(XmlUtils::WriteStringAttribute(out, ATTR_APP_PACKAGE_NAME, appPackageName))
    String appLabel;
    params->GetAppLabel(&appLabel);
    FAIL_RETURN(XmlUtils::WriteStringAttribute(out, ATTR_APP_LABEL, appLabel))
    AutoPtr<IUri> originatingUri;
    params->GetOriginatingUri((IUri**)&originatingUri);
    FAIL_RETURN(XmlUtils::WriteUriAttribute(out, ATTR_ORIGINATING_URI, originatingUri))
    AutoPtr<IUri> referrerUri;
    params->GetReferrerUri((IUri**)&referrerUri);
    FAIL_RETURN(XmlUtils::WriteUriAttribute(out, ATTR_REFERRER_URI, referrerUri))
    String abiOverride;
    params->GetAbiOverride(&abiOverride);
    FAIL_RETURN(XmlUtils::WriteStringAttribute(out, ATTR_ABI_OVERRIDE, abiOverride))

    // Persist app icon if changed since last written
    AutoPtr<IFile> appIconFile = BuildAppIconFile(session->mSessionId);
    AutoPtr<IBitmap> appIcon;
    params->GetAppIcon((IBitmap**)&appIcon);
    Boolean exists;
    if (appIcon == NULL && (appIconFile->Exists(&exists), exists)) {
        appIconFile->Delete();
    }
    else if (appIcon != NULL) {
        Int64 lastModified, appIconLastModified;
        appIconFile->GetLastModified(&lastModified);
        params->GetAppIconLastModified(&appIconLastModified);
        if (lastModified != appIconLastModified) {
            if (LOGD) Slogger::W(TAG, "Writing changed icon %p", appIconFile.Get());
            AutoPtr<IFileOutputStream> os;
            // try {
            if (FAILED(CFileOutputStream::New(appIconFile, (IFileOutputStream**)&os))) {
                Slogger::W(TAG, "Failed to write icon %p", appIconFile.Get());
            }
            else {
                Boolean result;
                appIcon->Compress(BitmapCompressFormat_PNG, 90, IOutputStream::Probe(os), &result);
            }
            // } catch (IOException e) {
            //     Slog.w(TAG, "Failed to write icon " + appIconFile + ": " + e.getMessage());
            // } finally {
            //     IoUtils.closeQuietly(os);
            // }
            AutoPtr<IIoUtils> ioutils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
            ioutils->CloseQuietly(ICloseable::Probe(os));

            appIconFile->GetLastModified(&lastModified);
            params->SetAppIconLastModified(lastModified);
        }
    }

    return out->WriteEndTag(String(NULL), TAG_SESSION);
}

AutoPtr<IFile> CPackageInstallerService::BuildAppIconFile(
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IFile> f;
    CFile::New(mSessionsDir, String("app_icon.") + StringUtils::ToString(sessionId) + ".png", (IFile**)&f);
    return f;
}

void CPackageInstallerService::WriteSessionsAsync()
{
    AutoPtr<IRunnable> runnable = new WriteSessionsRunnable(this);
    Boolean result;
    IoThread::GetHandler()->Post(runnable, &result);
}

ECode CPackageInstallerService::CreateSession(
    /* [in] */ IPackageInstallerSessionParams* params,
    /* [in] */ const String& installerPackageName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return CreateSessionInternal(params, installerPackageName, userId, result);
    // } catch (IOException e) {
    //     throw ExceptionUtils.wrap(e);
    // }
}

ECode CPackageInstallerService::CreateSessionInternal(
    /* [in] */ IPackageInstallerSessionParams* params,
    /* [in] */ const String& installerPackageName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    Int32 callingUid = Binder::GetCallingUid();
    FAIL_RETURN(mPm->EnforceCrossUserPermission(callingUid, userId, TRUE,
            TRUE, String("createSession")))

    if (mPm->IsUserRestricted(userId, IUserManager::DISALLOW_INSTALL_APPS)) {
        Slogger::E(TAG, "User restriction prevents installing");
        return E_SECURITY_EXCEPTION;
    }

    Int32 installFlags;
    params->GetInstallFlags(&installFlags);
    if ((callingUid == IProcess::SHELL_UID) || (callingUid == Process::ROOT_UID)) {
        installFlags |= IPackageManager::INSTALL_FROM_ADB;
        params->SetInstallFlags(installFlags);

    }
    else {
        FAIL_RETURN(mAppOps->CheckPackage(callingUid, installerPackageName))

        installFlags &= ~IPackageManager::INSTALL_FROM_ADB;
        installFlags &= ~IPackageManager::INSTALL_ALL_USERS;
        installFlags |= IPackageManager::INSTALL_REPLACE_EXISTING;
        params->SetInstallFlags(installFlags);
    }

    // Defensively resize giant app icons
    AutoPtr<IBitmap> appIcon;
    if (params->GetAppIcon((IBitmap**)&appIcon), appIcon != NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
        AutoPtr<IActivityManager> am = IActivityManager::Probe(service);
        Int32 iconSize;
        am->GetLauncherLargeIconSize(&iconSize);
        Int32 width, height;
        if ((appIcon->GetWidth(&width), width > iconSize * 2)
                || (appIcon->GetHeight(&height), height > iconSize * 2)) {
            AutoPtr<IBitmapHelper> helper;
            CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
            AutoPtr<IBitmap> tmp;
            helper->CreateScaledBitmap(appIcon, iconSize, iconSize, TRUE, (IBitmap**)&tmp);
            params->SetAppIcon(tmp);
        }
    }

    Int32 mode;
    if ((params->GetMode(&mode), mode == IPackageInstallerSessionParams::MODE_FULL_INSTALL)
            || mode == IPackageInstallerSessionParams::MODE_INHERIT_EXISTING) {
        // Resolve best location for install, based on combination of
        // requested install flags, delta size, and manifest settings.
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        String appPackageName;
        params->GetAppPackageName(&appPackageName);
        Int32 installLocation;
        params->GetInstallLocation(&installLocation);
        Int64 sizeBytes;
        params->GetSizeBytes(&sizeBytes);
        params->GetInstallFlags(&installFlags);
        AutoPtr<IPackageHelper> pkgHelper;
        CPackageHelper::AcquireSingleton((IPackageHelper**)&pkgHelper);
        Int32 resolved;
        pkgHelper->ResolveInstallLocation(mContext, appPackageName, installLocation, sizeBytes,
                installFlags, &resolved);

        if (resolved == IPackageHelper::RECOMMEND_INSTALL_INTERNAL) {
            params->SetInstallFlagsInternal();
        }
        else if (resolved == IPackageHelper::RECOMMEND_INSTALL_EXTERNAL) {
            params->SetInstallFlagsExternal();
        }
        else {
            Slogger::E(TAG, "No storage with enough free space; res=%d", resolved);
            Binder::RestoreCallingIdentity(ident);
            return E_IO_EXCEPTION;
        }
        // } finally {
        //     Binder.restoreCallingIdentity(ident);
        // }
    }
    else {
        Slogger::E(TAG, "Invalid install mode: %d", mode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 sessionId;
    AutoPtr<CPackageInstallerSession> session;
    {    AutoLock syncLock(mSessionsLock);
        // Sanity check that installer isn't going crazy
        Int32 activeCount = GetSessionCount(mSessions, callingUid);
        if (activeCount >= MAX_ACTIVE_SESSIONS) {
            Slogger::E(TAG, "Too many active sessions for UID %d", callingUid);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Int32 historicalCount = GetSessionCount(mHistoricalSessions, callingUid);
        if (historicalCount >= MAX_HISTORICAL_SESSIONS) {
            Slogger::E(TAG, "Too many historical sessions for UID %d", callingUid);
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 createdMillis;
        sys->GetCurrentTimeMillis(&createdMillis);
        FAIL_RETURN(AllocateSessionIdLocked(&sessionId))

        // We're staging to exactly one location
        AutoPtr<IFile> stageDir;
        String stageCid(NULL);


        if (params->GetInstallFlags(&installFlags), (installFlags & IPackageManager::INSTALL_INTERNAL) != 0) {
            stageDir = BuildInternalStageDir(sessionId);
        }
        else {
            stageCid = BuildExternalStageCid(sessionId);
        }

        AutoPtr<ILooper> looper;
        mInstallThread->GetLooper((ILooper**)&looper);
        CPackageInstallerSession::NewByFriend((Handle64)mInternalCallback.Get(), mContext, mPm,
                looper, sessionId, userId, installerPackageName, callingUid,
                params, createdMillis, stageDir, stageCid, FALSE, FALSE, (CPackageInstallerSession**)&session);
        mSessions[sessionId] = session;
    }

    mCallbacks->NotifySessionCreated(session->mSessionId, session->mUserId);
    WriteSessionsAsync();
    *result = sessionId;
    return NOERROR;
}

ECode CPackageInstallerService::UpdateSessionAppIcon(
    /* [in] */ Int32 sessionId,
    /* [in] */ IBitmap* _appIcon)
{
    AutoPtr<IBitmap> appIcon = _appIcon;
    {    AutoLock syncLock(mSessionsLock);
        AutoPtr<CPackageInstallerSession> session;
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Find(sessionId);
        if (it != mSessions.End()) {
            session = it->mSecond;
        }

        if (session == NULL || !IsCallingUidOwner(session)) {
            Slogger::E(TAG, "Caller has no access to session %d", sessionId);
            return E_SECURITY_EXCEPTION;
        }

        // Defensively resize giant app icons
        if (appIcon != NULL) {
            AutoPtr<IInterface> service;
            mContext->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
            AutoPtr<IActivityManager> am = IActivityManager::Probe(service);
            Int32 iconSize;
            am->GetLauncherLargeIconSize(&iconSize);
            Int32 width, height;
            if ((appIcon->GetWidth(&width), width > iconSize * 2)
                    || (appIcon->GetHeight(&height), height > iconSize * 2)) {
                AutoPtr<IBitmapHelper> helper;
                CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
                helper->CreateScaledBitmap(appIcon, iconSize, iconSize, TRUE, (IBitmap**)&appIcon);
            }
        }

        session->mParams->SetAppIcon(appIcon);
        session->mParams->SetAppIconLastModified(-1);

        mInternalCallback->OnSessionBadgingChanged(session);
    }
    return NOERROR;
}

ECode CPackageInstallerService::UpdateSessionAppLabel(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& appLabel)
{
    {    AutoLock syncLock(mSessionsLock);
        AutoPtr<CPackageInstallerSession> session;
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Find(sessionId);
        if (it != mSessions.End()) {
            session = it->mSecond;
        }
        if (session == NULL || !IsCallingUidOwner(session)) {
            Slogger::E(TAG, "Caller has no access to session %d", sessionId);
            return E_SECURITY_EXCEPTION;
        }
        AutoPtr<ICharSequence> cs;
        CString::New(appLabel, (ICharSequence**)&cs);
        session->mParams->SetAppLabel(cs);
        mInternalCallback->OnSessionBadgingChanged(session);
    }
    return NOERROR;
}

ECode CPackageInstallerService::AbandonSession(
    /* [in] */ Int32 sessionId)
{
    {    AutoLock syncLock(mSessionsLock);
        AutoPtr<CPackageInstallerSession> session;
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Find(sessionId);
        if (it != mSessions.End()) {
            session = it->mSecond;
        }
        if (session == NULL || !IsCallingUidOwner(session)) {
            Slogger::E(TAG, "Caller has no access to session %d", sessionId);
            return E_SECURITY_EXCEPTION;
        }
        session->Abandon();
    }
    return NOERROR;
}

ECode CPackageInstallerService::OpenSession(
    /* [in] */ Int32 sessionId,
    /* [out] */ IIPackageInstallerSession** session)
{
    VALIDATE_NOT_NULL(session)
    // try {
    return OpenSessionInternal(sessionId, session);
    // } catch (IOException e) {
    //     throw ExceptionUtils.wrap(e);
    // }
}

ECode CPackageInstallerService::OpenSessionInternal(
    /* [in] */ Int32 sessionId,
    /* [out] */ IIPackageInstallerSession** s)
{
    VALIDATE_NOT_NULL(s)
    {    AutoLock syncLock(mSessionsLock);
        AutoPtr<CPackageInstallerSession> session;
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Find(sessionId);
        if (it != mSessions.End()) {
            session = it->mSecond;
        }
        if (session == NULL || !IsCallingUidOwner(session)) {
            Slogger::E(TAG, "Caller has no access to session %d", sessionId);
            return E_SECURITY_EXCEPTION;
        }
        session->Open();
        *s = session;
        REFCOUNT_ADD(*s)
    }
    return NOERROR;
}

ECode CPackageInstallerService::AllocateSessionIdLocked(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    Int32 n = 0;
    Int32 sessionId;
    do {
        mRandom->NextInt32(Elastos::Core::Math::INT32_MAX_VALUE - 1, &sessionId);
        sessionId  += 1;
        if (mSessions.Find(sessionId) == mSessions.End()
                && mHistoricalSessions.Find(sessionId) == mHistoricalSessions.End()) {
            Boolean value = FALSE;
            HashMap<Int32, Boolean>::Iterator it = mLegacySessions.Find(sessionId);
            if (it != mLegacySessions.End()) {
                value = it->mSecond;
            }
            if (!value) {
                *id = sessionId;
                return NOERROR;
            }
        }
    } while (n++ < 32);

    Slogger::E(TAG, "Failed to allocate session ID");
    return E_ILLEGAL_STATE_EXCEPTION;
}

AutoPtr<IFile> CPackageInstallerService::BuildInternalStageDir(
    /* [in] */ Int32 sessionId)
{
    AutoPtr<IFile> f;
    CFile::New(mStagingDir, String("vmdl") + StringUtils::ToString(sessionId) + ".tmp", (IFile**)&f);
    return f;
}

ECode CPackageInstallerService::PrepareInternalStageDir(
    /* [in] */ IFile* stageDir)
{
    Boolean exists;
    if (stageDir->Exists(&exists), exists) {
        Slogger::E(TAG, "Session dir already exists: %p", stageDir);
        return E_IO_EXCEPTION;
    }

    // try {
    String absolutePath;
    stageDir->GetAbsolutePath(&absolutePath);
    if (FAILED(Elastos::Droid::System::Os::Mkdir(absolutePath, 0755))) {
        Slogger::E(TAG, "Failed to prepare session dir: %p", stageDir);
        return E_IO_EXCEPTION;
    }
    if (FAILED(Elastos::Droid::System::Os::Chmod(absolutePath, 0755))) {
        Slogger::E(TAG, "Failed to prepare session dir: %p", stageDir);
        return E_IO_EXCEPTION;
    }
    // } catch (ErrnoException e) {
    //     // This purposefully throws if directory already exists
    //     throw new IOException(, e);
    // }

    if (!SELinux::Restorecon(stageDir)) {
        Slogger::E(TAG, "Failed to restorecon session dir: %p", stageDir);
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

String CPackageInstallerService::BuildExternalStageCid(
    /* [in] */ Int32 sessionId)
{
    return String("smdl") + StringUtils::ToString(sessionId) + ".tmp";
}

ECode CPackageInstallerService::PrepareExternalStageCid(
    /* [in] */ const String& stageCid,
    /* [in] */ Int64 sizeBytes)
{
    AutoPtr<IPackageHelper> helper;
    CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
    String dir;
    if (helper->CreateSdDir(sizeBytes, stageCid, CPackageManagerService::GetEncryptKey(),
            IProcess::SYSTEM_UID, TRUE, &dir), dir.IsNull()) {
        Slogger::E(TAG, "Failed to create session cid: %s", stageCid.string());
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CPackageInstallerService::GetSessionInfo(
    /* [in] */ Int32 sessionId,
    /* [out] */ IPackageInstallerSessionInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;
    {    AutoLock syncLock(mSessionsLock);
        AutoPtr<CPackageInstallerSession> session;
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Find(sessionId);
        if (it != mSessions.End()) {
            session = it->mSecond;
        }
        if (session != NULL) {
            AutoPtr<IPackageInstallerSessionInfo> temp = session->GenerateInfo();
            *info = temp;
            REFCOUNT_ADD(*info)
        }
    }
    return NOERROR;
}

ECode CPackageInstallerService::GetAllSessions(
    /* [in] */ Int32 userId,
    /* [out] */ IParceledListSlice** slice)
{
    VALIDATE_NOT_NULL(slice)
    *slice = NULL;

    FAIL_RETURN(mPm->EnforceCrossUserPermission(Binder::GetCallingUid(),
            userId, TRUE, FALSE, String("getAllSessions")))

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    {    AutoLock syncLock(mSessionsLock);
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Begin();
        for (; it != mSessions.End(); ++it) {
            AutoPtr<CPackageInstallerSession> session = it->mSecond;
            if (session->mUserId == userId) {
                AutoPtr<IPackageInstallerSessionInfo> info = session->GenerateInfo();
                result->Add(info);
            }
        }
    }
    return CParceledListSlice::New(result, slice);
}

ECode CPackageInstallerService::GetMySessions(
    /* [in] */ const String& installerPackageName,
    /* [in] */ Int32 userId,
    /* [out] */ IParceledListSlice** slice)
{
    VALIDATE_NOT_NULL(slice)
    *slice = NULL;

    FAIL_RETURN(mPm->EnforceCrossUserPermission(Binder::GetCallingUid(), userId,
            TRUE, FALSE, String("getMySessions")))
    FAIL_RETURN(mAppOps->CheckPackage(Binder::GetCallingUid(), installerPackageName))

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    {    AutoLock syncLock(mSessionsLock);
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Begin();
        for (; it != mSessions.End(); ++it) {
            AutoPtr<CPackageInstallerSession> session = it->mSecond;
            AutoPtr<ICharSequence> cs1, cs2;
            CString::New(session->mInstallerPackageName, (ICharSequence**)&cs1);
            CString::New(installerPackageName, (ICharSequence**)&cs2);
            if (Objects::Equals(cs1, cs2) && session->mUserId == userId) {
                AutoPtr<IPackageInstallerSessionInfo> info = session->GenerateInfo();
                result->Add(info);
            }
        }
    }
    return CParceledListSlice::New(result, slice);
}

ECode CPackageInstallerService::Uninstall(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [in] */ IIntentSender* statusReceiver,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(mPm->EnforceCrossUserPermission(Binder::GetCallingUid(), userId,
            TRUE, TRUE, String("uninstall")))

    AutoPtr<PackageDeleteObserverAdapter> adapter = new PackageDeleteObserverAdapter(mContext,
            statusReceiver, packageName);
    adapter->AddRef();
    Int32 perm;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::DELETE_PACKAGES, &perm))
    AutoPtr<IIPackageDeleteObserver2> binder;
    adapter->GetBinder((IIPackageDeleteObserver2**)&binder);
    if (perm == IPackageManager::PERMISSION_GRANTED) {
        // Sweet, call straight through!
        mPm->DeletePackage(packageName, binder, userId, flags);

    }
    else {
        // Take a short detour to confirm with user
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_UNINSTALL_PACKAGE, (IIntent**)&intent);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
        intent->SetData(uri);
        assert(0);
        // intent->PutExtra(IPackageInstaller::EXTRA_CALLBACK, IBinder::Probe(binder));
        adapter->OnUserActionRequired(intent);
    }
    return NOERROR;
}

ECode CPackageInstallerService::SetPermissionsResult(
    /* [in] */ Int32 sessionId,
    /* [in] */ Boolean accepted)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::INSTALL_PACKAGES, TAG))

    {    AutoLock syncLock(mSessionsLock);
        AutoPtr<CPackageInstallerSession> session;
        HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = mSessions.Find(sessionId);
        if (it != mSessions.End()) {
            session = it->mSecond;
        }
        if (session) {
            session->SetPermissionsResult(accepted);
        }
    }
    return NOERROR;
}

ECode CPackageInstallerService::RegisterCallback(
    /* [in] */ IPackageInstallerCallback* cb,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(mPm->EnforceCrossUserPermission(Binder::GetCallingUid(), userId,
            TRUE, FALSE, String("registerCallback")))
    mCallbacks->Register(cb, userId);
    return NOERROR;
}

ECode CPackageInstallerService::UnregisterCallback(
    /* [in] */ IPackageInstallerCallback* cb)
{
    mCallbacks->Unregister(cb);
    return NOERROR;
}

Int32 CPackageInstallerService::GetSessionCount(
    /* [in] */ HashMap<Int32, AutoPtr<CPackageInstallerSession> >& sessions,
    /* [in] */ Int32 installerUid)
{
    Int32 count = 0;
    HashMap<Int32, AutoPtr<CPackageInstallerSession> >::Iterator it = sessions.Begin();
    for (; it != sessions.End(); ++it) {
        AutoPtr<CPackageInstallerSession> session = it->mSecond;
        if (session->mInstallerUid == installerUid) {
            count++;
        }
    }
    return count;
}

Boolean CPackageInstallerService::IsCallingUidOwner(
    /* [in] */ CPackageInstallerSession* session)
{
    Int32 callingUid = Binder::GetCallingUid();
    if (callingUid == Process::ROOT_UID) {
        return TRUE;
    }
    else {
        return (session != NULL) && (callingUid == session->mInstallerUid);
    }
}

ECode CPackageInstallerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
