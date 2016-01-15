
#include "elastos/droid/server/pm/CPackageInstallerSession.h"
#include "elastos/droid/server/pm/CLocalObserver.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/system/Os.h"
#include "elastos/droid/utility/MathUtils.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/system/OsConstants.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::CPackageInstallerSessionInfo;
using Elastos::Droid::Content::Pm::CParceledListSlice;
using Elastos::Droid::Content::Pm::CSignature;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Internal::Content::CPackageHelper;
using Elastos::Droid::Internal::Content::INativeLibraryHelper;
using Elastos::Droid::Internal::Content::CNativeLibraryHelper;
using Elastos::Droid::Internal::Content::INativeLibraryHelperHandle;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CFileBridge;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::System::Os;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructStat;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::CFileHelper;
using Elastos::IO::IFileHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

//==============================================================================
//                  CPackageInstallerSession::HandlerCallBack
//==============================================================================

CAR_INTERFACE_IMPL(CPackageInstallerSession::HandlerCallBack, Object, IHandlerCallback)

ECode CPackageInstallerSession::HandlerCallBack::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized (mHost->mLock) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        if (obj != NULL) {
            mHost->mRemoteObserver = IIPackageInstallObserver2::Probe(obj);
        }

        // try {
        if (FAILED(mHost->CommitLocked())) {
            // String completeMsg = ExceptionUtils.getCompleteMessage(e);
            Slogger::E(CPackageInstallerSession::TAG, "Commit of session %d failed", mHost->mSessionId);
            mHost->DestroyInternal();
            mHost->DispatchSessionFinished(mHost->mErrorCode, String(NULL), NULL);
        }
        // } catch (PackageManagerException e) {
        //     final String completeMsg = ExceptionUtils.getCompleteMessage(e);
        //     Slog.e(TAG, "Commit of session " + sessionId + " failed: " + completeMsg);
        //     destroyInternal();
        //     dispatchSessionFinished(e.error, completeMsg, null);
        // }

        *result = TRUE;
    }
    return NOERROR;
}


//==============================================================================
//                  CPackageInstallerSession
//==============================================================================

const String CPackageInstallerSession::TAG("PackageInstaller");
const Boolean CPackageInstallerSession::LOGD;
const Int32 CPackageInstallerSession::MSG_COMMIT;

CPackageInstallerSession::CPackageInstallerSession()
    : mSessionId(0)
    , mUserId(0)
    , mInstallerUid(0)
    , mCreatedMillis(0)
    , mClientProgress(0)
    , mInternalProgress(0)
    , mProgress(0)
    , mReportedProgress(-1)
    , mPrepared(FALSE)
    , mSealed(FALSE)
    , mPermissionsAccepted(FALSE)
    , mDestroyed(FALSE)
    , mFinalStatus(0)
    , mVersionCode(0)
    , mErrorCode(-1)
{
    CAtomicInteger32::New((IAtomicInteger32**)&mActiveCount);
    mHandlerCallback = (IHandlerCallback*)new HandlerCallback(this);
}

CAR_INTERFACE_IMPL_2(CPackageInstallerSession, Object, IIPackageInstallerSession, IBinder)

CAR_OBJECT_IMPL(CPackageInstallerSession)

ECode CPackageInstallerSession::constructor(
	/* [in] */ Handle64 callback,
    /* [in] */ IContext* ctx,
    /* [in] */ IIPackageManager* pm,
    /* [in] */ ILooper* looper,
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 userId,
    /* [in] */ const String& installerPackageName,
    /* [in] */ Int32 installerUid,
    /* [in] */ IPackageInstallerSessionParams* params,
    /* [in] */ Int64 createdMillis,
    /* [in] */ IFile* stagingDir,
    /* [in] */ const String& stageCid,
    /* [in] */ Boolean prepared,
    /* [in] */ Boolean sealed)
{
    mCallback = (CPackageInstallerService::InternalCallback*)callback;
    mContext = context;
    mPm = reinterpret_cast<CPackageManagerService*>(pm->Probe(EIID_CPackageManagerService));
    CHandler::New(looper, mHandlerCallback, (IHandler**)&mHandler);

    mSessionId = sessionId;
    mUserId = userId;
    mInstallerPackageName = installerPackageName;
    mInstallerUid = installerUid;
    mParams = params;
    mCreatedMillis = createdMillis;
    mStageDir = stageDir;
    mStageCid = stageCid;

    if ((stageDir == NULL) == (stageCid.IsNull())) {
        Slogger::E(TAG, "Exactly one of stageDir or stageCid stage must be set");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mPrepared = prepared;
    mSealed = sealed;

    Int32 result;
    FAIL_RETURN(mPm->checkUidPermission(Elastos::Droid::Manifest::permission::INSTALL_PACKAGES, installerUid, &result))
    if ((result == IPackageManager::PERMISSION_GRANTED) || (installerUid == IProcess::ROOT_UID)) {
        mPermissionsAccepted = TRUE;
    }
    else {
        mPermissionsAccepted = FALSE;
    }

    return NOERROR;
}

AutoPtr<IPackageInstallerSessionInfo> CPackageInstallerSession::GenerateInfo()
{
    AutoPtr<IPackageInstallerSessionInfo> info;
    CPackageInstallerSessionInfo::New((IPackageInstallerSessionInfo**)&info);
    synchronized (mLock) {
        info->SetSessionId(mSessionId);
        info->SetInstallerPackageName(mInstallerPackageName);
        String absolutePath(NULL);
        if (mResolvedBaseFile != NULL) {
            mResolvedBaseFile->GetAbsolutePath(&absolutePath);
        }
        info->SetResolvedBaseCodePath(absolutePath);
        info->SetProgress(mProgress);
        info->SetSealed(mSealed);
        Int32 value;
        mActiveCount->Get(&value);
        info->SetActive(value > 0);

        Int32 mode;
        mParams->GetMode(&mode);
        info->SetMode(mode);
        Int64 sizeBytes;
        mParams->GetSizeBytes(&sizeBytes);
        info->SetSizeBytes(sizeBytes);
        String appPackageName;
        mParams->GetAppPackageName(&appPackageName);
        info->SetAppPackageName(appPackageName);
        AutoPtr<IBitmap> appIcon;
        mParams->GetAppIcon((IBitmap**)&appIcon);
        info->SetAppIcon(appIcon);
        String appLabel;
        mParams->GetAppLabel(&appLabel);
        info->SetAppLabel(appLabel);
    }
    return info;
}

Boolean CPackageInstallerSession::IsPrepared()
{
    synchronized (mLock) {
        return mPrepared;
    }
}

Boolean CPackageInstallerSession::IsSealed()
{
    synchronized (mLock) {
        return mSealed;
    }
}

ECode CPackageInstallerSession::AssertPreparedAndNotSealed(
    /* [in] */ const String& cookie)
{
    synchronized (mLock) {
        if (!mPrepared) {
            Slogger::E(TAG, "%s before prepared", cookie.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (mSealed) {
            Slogger::E(TAG, "%s not allowed after commit", cookie.string());
            return E_SECURITY_EXCEPTION;
        }
    }
}

ECode CPackageInstallerSession::ResolveStageDir(
    /* [out] */ IFile* dir)
{
    VALIDATE_NOT_NULL(dir)
    *dir = NULL;

    synchronized (mLock) {
        if (mResolvedStageDir == NULL) {
            if (mStageDir != NULL) {
                mResolvedStageDir = mStageDir;
            }
            else {
                AutoPtr<IPackageHelper> helper;
                CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
                String path;
                helper->GetSdDir(mStageCid, &path);
                if (!path.IsNull()) {
                    mResolvedStageDir = NULL;
                    CFile::New(path, (IFile**)&mResolvedStageDir);
                }
                else {
                    Slogger::E(TAG, "Failed to resolve path to container %s", mStageCid.string());
                    return E_IO_EXCEPTION;
                }
            }
        }
        *dir = mResolvedStageDir;
        REFCOUNT_ADD(*file)
    }
    return NOERROR;
}

ECode CPackageInstallerSession::SetClientProgress(
    /* [in] */ Float progress)
{
    synchronized (mLock) {
        // Always publish first staging movement
        Boolean forcePublish = (mClientProgress == 0);
        mClientProgress = progress;
        ComputeProgressLocked(forcePublish);
    }
    return NOERROR;
}

ECode CPackageInstallerSession::AddClientProgress(
    /* [in] */ Float progress)
{
    synchronized (mLock) {
        SetClientProgress(mClientProgress + progress);
    }
    return NOERROR;
}

void CPackageInstallerSession::ComputeProgressLocked(
    /* [in] */ Boolean forcePublish)
{
    mProgress = MathUtils::Constrain(mClientProgress * 0.8f, 0f, 0.8f)
            + MathUtils::Constrain(mInternalProgress * 0.2f, 0f, 0.2f);

    // Only publish when meaningful change
    if (forcePublish || Elastos::Core::Math::Abs(mProgress - mReportedProgress) >= 0.01) {
        mReportedProgress = mProgress;
        mCallback->OnSessionProgressChanged(this, mProgress);
    }
}

ECode CPackageInstallerSession::GetNames(
    /* [out, callee] */ ArrayOf<String>* names)
{
    VALIDATE_NOT_NULL(names)
    *names = NULL;

    FAIL_RETURN(AssertPreparedAndNotSealed(String("getNames")))
    // try {
    AutoPtr<IFile> dir;
    FAIL_RETURN(ResolveStageDir((IFile**)&dir))
    return dir->List(names);
    // } catch (IOException e) {
    //     throw ExceptionUtils.wrap(e);
    // }
}

ECode CPackageInstallerSession::OpenWrite(
    /* [in] */ const String& name,
    /* [in] */ Int64 offsetBytes,
    /* [in] */ Int64 lengthBytes,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd)
    // try {
    return OpenWriteInternal(name, offsetBytes, lengthBytes, pfd);
    // } catch (IOException e) {
    //     throw ExceptionUtils.wrap(e);
    // }
}

ECode CPackageInstallerSession::OpenWriteInternal(
    /* [in] */ const String& name,
    /* [in] */ Int64 offsetBytes,
    /* [in] */ Int64 lengthBytes,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd)
    *pfd = NULL;
    // Quick sanity check of state, and allocate a pipe for ourselves. We
    // then do heavy disk allocation outside the lock, but this open pipe
    // will block any attempted install transitions.
    AutoPtr<IFileBridge> bridge;
    synchronized (mLock) {
        FAIL_RETURN(AssertPreparedAndNotSealed(String("openWrite")))

        CFileBridge::New((IFileBridge**)&bridge);
        mBridges.PushBack(bridge);
    }

    // try {
    // Use installer provided name for now; we always rename later
    if (!FileUtils::IsValidExtFilename(name)) {
        Slogger::E(TAG, "Invalid name: %s", name.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IFile> dir;
    FAIL_RETURN(ResolveStageDir((IFile**)&dir))
    AutoPtr<IFile> target;
    CFile::New(dir, name, (IFile**)&target);

    // TODO: this should delegate to DCS so the system process avoids
    // holding open FDs into containers.
    String absolutePath;
    target->GetAbsolutePath(&absolutePath);
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    AutoPtr<IFileDescriptor> targetFd;
    if (FAILED(os->Open(absolutePath, OsConstants::O_CREAT | OsConstants::O_WRONLY, 0644, (IFileDescriptor**)&targetFd))) {
        return E_IO_EXCEPTION;
    }
    Os::Chmod(absolutePath, 0644);

    // If caller specified a total length, allocate it for them. Free up
    // cache space to grow, if needed.
    if (lengthBytes > 0) {
        AutoPtr<IStructStat> stat;
        if (FAILED(os->Fstat(targetFd, (IStructStat**)&stat))) {
            return E_IO_EXCEPTION;
        }
        Int64 size;
        star->GetSize(&size);
        Int64 deltaBytes = lengthBytes - size;
        // Only need to free up space when writing to internal stage
        if (mStageDir != NULL && deltaBytes > 0) {
            mPm->FreeStorage(deltaBytes);
        }
        if (FAILED(os->Posix_fallocate(targetFd, 0, lengthBytes))) {
            return E_IO_EXCEPTION;
        }
    }

    if (offsetBytes > 0) {
        if (FAILED(os->Lseek(targetFd, offsetBytes, OsConstants::SEEK_SET))) {
            return E_IO_EXCEPTION;
        }
    }

    bridge->SetTargetFile(targetFd);
    IThread::Probe(bridge)->Start();
    AutoPtr<IFileDescriptor> fd;
    bridge->GetClientSocket((IFileDescriptor**)&fd);
    return CParcelFileDescriptor::New(fd, pfd);

    // } catch (ErrnoException e) {
    //     throw e.rethrowAsIOException();
    // }
}

ECode CPackageInstallerSession::OpenRead(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd)
    // try {
    return OpenReadInternal(name);
    // } catch (IOException e) {
    //     throw ExceptionUtils.wrap(e);
    // }
}

ECode CPackageInstallerSession::OpenReadInternal(
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd)
    *pfd = NULL;
    FAIL_RETURN(AssertPreparedAndNotSealed(String("openRead")))

    // try {
    if (!FileUtils::IsValidExtFilename(name)) {
        Slogger::E(TAG, "Invalid name: %s", name.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IFile> dir;
    FAIL_RETURN(ResolveStageDir((IFile**)&dir))
    AutoPtr<IFile> target;
    CFile::New(dir, name, (IFile**)&target);

    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    String absolutePath;
    target->GetAbsolutePath(&absolutePath);
    AutoPtr<IFileDescriptor> targetFd;
    if (FAILED(os->Open(absolutePath, OsConstants::O_RDONLY, 0, (IFileDescriptor**)&targetFd))) {
        return E_IO_EXCEPTION;
    }
    return CParcelFileDescriptor::New(targetFdm, pfd);

    // } catch (ErrnoException e) {
    //     throw e.rethrowAsIOException();
    // }
}

ECode CPackageInstallerSession::Commit(
    /* [in] */ IIntentSender* statusReceiver)
{
    if (statusReceiver == NULL) {
        Slogger::E(TAG, "argument must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Boolean wasSealed;
    synchronized (mLock) {
        wasSealed = mSealed;
        if (!mSealed) {
            // Verify that all writers are hands-off
            List<AutoPtr<IFileBridge> >::Iterator it = mBridges.Begin();
            for (; it != mBridges.End(); ++it) {
                Boolean isClosed;
                if ((*it)->IsClosed(&isClosed), !isClosed) {
                    Slogger::E(TAG, "Files still open");
                    return E_SECURITY_EXCEPTION;
                }
            }
            mSealed = TRUE;
        }

        // Client staging is fully done at this point
        mClientProgress = 1f;
        ComputeProgressLocked(TRUE);
    }

    if (!wasSealed) {
        // Persist the fact that we've sealed ourselves to prevent
        // mutations of any hard links we create. We do this without holding
        // the session lock, since otherwise it's a lock inversion.
        mCallback->OnSessionSealedBlocking(this);
    }

    // This ongoing commit should keep session active, even though client
    // will probably close their end.
    mActiveCount->IncrementAndGet();

    AutoPtr<CPackageInstallerService::PackageInstallObserverAdapter> adapter
            = new CPackageInstallerService::PackageInstallObserverAdapter(mContext,
                    statusReceiver, mSessionId);
    AutoPtr<IIPackageInstallObserver2> b;
    adapter->GetBinder((IIPackageInstallObserver2**)&b);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_COMMIT, b, (IMessage**)&msg);
    msg->SendToTarget();
    reutrn NOERROR;
}

ECode CPackageInstallerSession::CommitLocked()
{
    if (mDestroyed) {
        mErrorCode = IPackageManager::INSTALL_FAILED_INTERNAL_ERROR;
        Slogger::E(TAG, "Session destroyed");
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    if (!mSealed) {
        mErrorCode = IPackageManager::INSTALL_FAILED_INTERNAL_ERROR;
        Slogger::E(TAG, "Session not sealed");
        return E_PACKAGE_MANAGER_EXCEPTION;
    }

    // try {
    AutoPtr<IFile> dir;
    if (FAILED(ResolveStageDir((IFile**)&dir))) {
        mErrorCode = IPackageManager::INSTALL_FAILED_CONTAINER_ERROR;
        Slogger::E(TAG, "Failed to resolve stage location");
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    // } catch (IOException e) {
    //     throw new PackageManagerException(INSTALL_FAILED_CONTAINER_ERROR,
    //             "Failed to resolve stage location", e);
    // }

    // Verify that stage looks sane with respect to existing application.
    // This currently only ensures packageName, versionCode, and certificate
    // consistency.
    FAIL_RETURN(ValidateInstallLocked())

    if (mPackageName.IsNull() || mSignatures == NULL || mResolvedBaseFile == NULL) {
        Slogger::E(TAG, "argument must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (!mPermissionsAccepted) {
        // User needs to accept permissions; give installer an intent they
        // can use to involve user.
        AutoPtr<IIntent> intent;
        CIntent::New(IPackageInstaller::ACTION_CONFIRM_PERMISSIONS);
        intent->SetPackage(String("com.android.packageinstaller"));
        intent->PutExtra(IPackageInstaller::EXTRA_SESSION_ID, sessionId);
        // try {
        mRemoteObserver->OnUserActionRequired(intent);
        // } catch (RemoteException ignored) {
        // }

        // Commit was keeping session marked as active until now; release
        // that extra refcount so session appears idle.
        Close();
        return NOERROR;
    }

    if (!mStageCid.IsNull()) {
        // Figure out the final installed size and resize the container once
        // and for all. Internally the parser handles straddling between two
        // locations when inheriting.
        Int64 finalSize;
        FAIL_RETURN(CalculateInstalledSize())
        FAIL_RETURN(ResizeContainer(stageCid, finalSize))
    }

    // Inherit any packages and native libraries from existing install that
    // haven't been overridden.
    Int32 mode;
    if (mParams->GetMode(&mode), mode == IPackageInstallerSessionParams::MODE_INHERIT_EXISTING) {
        // try {
        if (!mStageCid.IsNull()) {
            // TODO: this should delegate to DCS so the system process
            // avoids holding open FDs into containers.
            if (FAILED(CopyFiles(mResolvedInheritedFiles, resolveStageDir()))) {
                mErrorCode = IPackageManager::INSTALL_FAILED_INSUFFICIENT_STORAGE;
                Slogger::E(TAG, "Failed to inherit existing install");
                return E_PACKAGE_MANAGER_EXCEPTION;
            }
        }
        else {
            if (FAILED(LinkFiles(mResolvedInheritedFiles, resolveStageDir()))) {
                mErrorCode = IPackageManager::INSTALL_FAILED_INSUFFICIENT_STORAGE;
                Slogger::E(TAG, "Failed to inherit existing install");
                return E_PACKAGE_MANAGER_EXCEPTION;
            }
        }
        // } catch (IOException e) {
        //     throw new PackageManagerException(INSTALL_FAILED_INSUFFICIENT_STORAGE,
        //             "Failed to inherit existing install", e);
        // }
    }

    // TODO: surface more granular state from dexopt
    mInternalProgress = 0.5f;
    ComputeProgressLocked(TRUE);

    // Unpack native libraries
    String abiOverride;
    mParams->GetAbiOverride(&abiOverride);
    FAIL_RETURN(ExtractNativeLibraries(mResolvedStageDir, abiOverride))

    // Container is ready to go, let's seal it up!
    if (!mStageCid.IsNull()) {
        FAIL_RETURN(FinalizeAndFixContainer(stageCid))
    }

    // We've reached point of no return; call into PMS to install the stage.
    // Regardless of success or failure we always destroy session.
    AutoPtr<IIPackageInstallObserver2> localObserver;
    CLocalObserver::New((IIPackageInstallObserver2**)&localObserver);

    AutoPtr<IUserHandle> user;
    Int32 installFlags;
    if (mParams->GetInstallFlags(&installFlags), (installFlags & IPackageManager::INSTALL_ALL_USERS) != 0) {
        user = UserHandle::ALL;
    }
    else {
        CUserHandle::New(userId, (IUserHandle**)&user);
    }

    mPm->InstallStage(mPackageName, mStageDir, mStageCid, localObserver, mParams,
            mInstallerPackageName, mInstallerUid, user);
    return NOERROR;
}

ECode CPackageInstallerSession::ValidateInstallLocked()
{
    mPackageName = String(NULL);
    mVersionCode = -1;
    mSignatures = NULL;

    mResolvedBaseFile = NULL;
    mResolvedStagedFiles.Clear();
    mResolvedInheritedFiles.Clear();

    AutoPtr<ArrayOf<IFile*> > files;
    mResolvedStageDir->ListFiles((ArrayOf<IFile*>**)&files);
    if (ArrayUtils::IsEmpty(files)) {
        mErrorCode = IPackageManager::INSTALL_FAILED_INVALID_APK;
        Slogger::E(TAG, "No packages staged");
        return E_PACKAGE_MANAGER_EXCEPTION;
    }

    // Verify that all staged packages are internally consistent
    HashSet<String> stagedSplits;
    for (Int32 i = 0; i < files->GetLength(); ++i) {
        AutoPtr<IFile> file = (*files)[i];
        // Installers can't stage directories, so it's fine to ignore
        // entries like "lost+found".
        Boolean isDirectory;
        if (file->IsDirectory(&isDirectory), isDirectory) continue;

        AutoPtr<PackageParser::ApkLite> apk;
        // try {
        if (FAILED(PackageParser::ParseApkLite(file, PackageParser::PARSE_COLLECT_CERTIFICATES,
                (PackageParser::ApkLite**)&apk))) {
            mErrorCode = PackageParser::GetParseError();
            return E_PACKAGE_MANAGER_EXCEPTION;
        }
        // } catch (PackageParserException e) {
        //     throw PackageManagerException.from(e);
        // }

        if (stagedSplits.Find(apk->mSplitName) != stagedSplits.End()) {
            mErrorCode = IPackageManager::INSTALL_FAILED_INVALID_APK;
            Slogger::E(TAG, "Split %s was defined multiple times", apk->mSplitName.string());
            return E_PACKAGE_MANAGER_EXCEPTION;
        }
        else {
            stagedSplits.Insert(apk->mSplitName);
        }
        // if (!stagedSplits.Insert(apk->mSplitName)) {
        //     throw new PackageManagerException(INSTALL_FAILED_INVALID_APK,
        //             "Split " + apk.splitName + " was defined multiple times");
        // }

        // Use first package to define unknown values
        if (mPackageName.IsNull()) {
            mPackageName = apk->mPackageName;
            mVersionCode = apk->mVersionCode;
        }
        if (mSignatures == NULL) {
            mSignatures = apk->mSignatures;
        }

        String str;
        IObject::Probe(file)->ToString(&str);
        FAIL_RETURN(AssertApkConsistent(str, apk))

        // Take this opportunity to enforce uniform naming
        String targetName;
        if (apk->mSplitName.IsNull()) {
            targetName = "base.apk";
        }
        else {
            targetName = String("split_") + apk->mSplitName + ".apk";
        }
        if (!FileUtils::IsValidExtFilename(targetName)) {
            mErrorCode = IPackageManager::INSTALL_FAILED_INVALID_APK;
            Slogger::E(TAG, "Invalid filename: %s", targetName.string());
            return E_PACKAGE_MANAGER_EXCEPTION;
        }

        AutoPtr<IFile> targetFile;
        CFile::New(mResolvedStageDir, targetName, (IFile**)&targetFile);
        Boolean equals;
        if (IObject::Probe(file)->Equals(targetFile, &equals). !equals) {
            file->RenameTo(targetFile);
        }

        // Base is coming from session
        if (apk->mSplitName.IsNull()) {
            mResolvedBaseFile = targetFile;
        }

        mResolvedStagedFiles.PushBack(targetFile);
    }

    Int32 mode;
    if (mParams->GetMode(&mode), mode == IPackageInstallerSessionParams::MODE_FULL_INSTALL) {
        // Full installs must include a base package
        if (stagedSplits.Find(String(NULL)) == stagedSplits.End()) {
            mErrorCode = IPackageManager::INSTALL_FAILED_INVALID_APK;
            Slogger::E(TAG, "Full install must include a base package");
            return E_PACKAGE_MANAGER_EXCEPTION;
        }

    }
    else {
        // Partial installs must be consistent with existing install
        AutoPtr<IApplicationInfo> app;
        mPm->GetApplicationInfo(mPackageName, 0, userId, (IApplicationInfo**)&app);
        if (app == NULL) {
            mErrorCode = IPackageManager::INSTALL_FAILED_INVALID_APK;
            Slogger::E(TAG, "Missing existing base package for %s", mPackageName.string());
            return E_PACKAGE_MANAGER_EXCEPTION;
        }

        AutoPtr<PackageParser::PackageLite> existing;
        AutoPtr<PackageParser::ApkLite> existingBase;
        // try {
        Striong path;
        app->GetCodePath(&path);
        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        if (FAILED(PackageParser::ParsePackageLite(file, 0, (PackageParser::PackageLite**)&existing))) {
            return E_PACKAGE_MANAGER_EXCEPTION;
        }
        app->GetBaseCodePath(&path);
        file = NULL;
        CFile::New(path, (IFile**)&file);
        if (FAILED(PackageParser::ParseApkLite(file,
                PackageParser::PARSE_COLLECT_CERTIFICATES, (PackageParser::ApkLite**)&existingBase))) {
            mErrorCode = PackageParser::GetParseError();
            return E_PACKAGE_MANAGER_EXCEPTION;
        }
        // } catch (PackageParserException e) {
        //     throw PackageManagerException.from(e);
        // }

        FAIL_RETURN(AssertApkConsistent(String("Existing base"), existingBase))

        // Inherit base if not overridden
        if (mResolvedBaseFile == NULL) {
            mResolvedBaseFile = file;
            mResolvedInheritedFiles.PushBack(mResolvedBaseFile);
        }

        // Inherit splits if not overridden
        if (!ArrayUtils::IsEmpty(existing->mSplitNames)) {
            for (Int32 i = 0; i < existing->mSplitNames->GetLength(); i++) {
                String splitName = (*existing->mSplitNames)[i];
                AutoPtr<IFile> splitFile;
                CFile::New((*existing->mSplitCodePaths)[i], (IFile**)&splitFile);

                if (stagedSplits.Find(splitName) == stagedSplits.End()) {
                    mResolvedInheritedFiles.PushBack(splitFile);
                }
            }
        }
    }
    return NOERROR;
}

ECode CPackageInstallerSession::AssertApkConsistent(
    /* [in] */ const String& tag,
    /* [in] */ PackageParser::ApkLite* apk)
{
    if (!mPackageName.Equals(apk->PmackageName)) {
        mErrorCode = INSTALL_FAILED_INVALID_APK;
        Slogger::E(TAG, " package %s inconsistent with %s", apk->mPackageName.string(), mPackageName.string());
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    if (mVersionCode != apk->mVersionCode) {
        mErrorCode = INSTALL_FAILED_INVALID_APK;
        Slogger::E(TAG, " version code %d inconsistent with %d", apk->mVersionCode, mVersionCode);
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    AutoPtr<ISignature> sig;
    CSignature::AcquireSingleton((ISignature**)&sig);
    Boolean result;
    if (sig->AreExactMatch(mSignatures, apk->mSignatures, &result), !result) {
        mErrorCode = INSTALL_FAILED_INVALID_APK;
        Slogger::E(TAG, "signatures are inconsistent");
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    return NOERROR;
}

ECode CPackageInstallerSession::CalculateInstalledSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 0;

    if (mResolvedBaseFile == NULL) {
        Slogger::E(TAG, "argument must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // Preconditions.checkNotNull(mResolvedBaseFile);

    AutoPtr<PackageParser::ApkLite> baseApk;
    // try {
    if (FAILED(PackageParser::ParseApkLite(mResolvedBaseFile, 0,
            (PackageParser::ApkLite**)&baseApk))) {
        mErrorCode = PackageParser::GetParseError();
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    // } catch (PackageParserException e) {
    //     throw PackageManagerException.from(e);
    // }

    List<String> splitPaths;
    List<AutoPtr<IFile> >::Iterator it = mResolvedStagedFiles.Begin();
    for (; it != mResolvedStagedFiles.End(); ++it) {
        AutoPtr<IFile> file = *it;
        Boolean equals;
        if (IObject::Probe(mResolvedBaseFile)->Equals(file, &equals), equals) continue;
        String absolutePath;
        file->GetAbsolutePath(&absolutePath);
        splitPaths.PushBack(absolutePath);
    }
    it = mResolvedInheritedFiles.Begin();
    for (; it != mResolvedInheritedFiles.End(); ++it) {
        AutoPtr<IFile> file = *it;
        Boolean equals;
        if (IObject::Probe(mResolvedBaseFile)->Equals(file, &equals), equals) continue;
        String absolutePath;
        file->GetAbsolutePath(&absolutePath);
        splitPaths.PushBack(absolutePath);
    }

    // This is kind of hacky; we're creating a half-parsed package that is
    // straddled between the inherited and staged APKs.
    AutoPtr<ArrayOf<String> > pathsArray = ArrayOf<String>::Alloc(splitPaths.GetSize());
    List<String>::Iterator strIt = splitPaths.Begin();
    for (Int32 i = 0; strIt != splitPaths.End(); ++strIt, ++i) {
        (*pathsArray)[i] = *strIt;
    }
    AutoPtr<PackageParser::PackageLite> pkg = new PackageLite(String(NULL), baseApk, NULL, pathsArray);
    Int32 installFlags;
    mParams->GetInstallFlags(&installFlags);
    Boolean isForwardLocked = installFlags & IPackageManager::INSTALL_FORWARD_LOCK) != 0;

    // try {
    AutoPtr<IPackageHelper> pkgHelper;
    CPackageHelper::AcquireSingleton((IPackageHelper**)&pkgHelper);
    String abiOverride;
    mParams->GetAbiOverride(&abiOverride);
    ECode ec = pkgHelper->CalculateInstalledSize(pkg, isForwardLocked, abiOverride);
    if (ec == (ECode)E_IO_EXCEPTION) {
        mErrorCode = INSTALL_FAILED_INVALID_APK;
        Slogger::E(TAG, "Failed to calculate install size");
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    return NOERROR;
    // } catch (IOException e) {
    //     throw new PackageManagerException(INSTALL_FAILED_INVALID_APK,
    //             "Failed to calculate install size", e);
    // }
}

ECode CPackageInstallerSession::LinkFiles(
    /* [in] */ List<AutoPtr<IFile> >& fromFiles,
    /* [in] */ IFile* toDir)
{
    List<AutoPtr<IFile> >::Iterator it = fromFiles.Begin();
    for (; it != fromFiles.End(); ++it) {
        AutoPtr<IFile> fromFile = *it;
        String name;
        fromFile->GetName(&name);
        AutoPtr<IFile> toFile;
        CFile::New(toDir, name, (IFile**)&toFile);
        // try {
        if (LOGD) Slogger::D(TAG, "Linking %p to %p", fromFile.Get(), toFile.Get());
        String absolutePath;
        fromFile->GetAbsolutePath(&absolutePath);
        String toAbsolutePath;
        toFile->GetAbsolutePath(&toAbsolutePath);
        if (FAILED(Os::Link(absolutePath, toAbsolutePath))) {
            Slogger::E(TAG, "Failed to link %p to %p", fromFile.Get(), toFile.Get());
            return E_IO_EXCEPTION;
        }
        // } catch (ErrnoException e) {
        //     throw new IOException("Failed to link " + fromFile + " to " + toFile, e);
        // }
    }
    Slogger::D(TAG, "Linked %d files into %p", fromFiles.GetSize(), toDir.Get());
    return NOERROR;
}

ECode CPackageInstallerSession::CopyFiles(
    /* [in] */ List<AutoPtr<IFile> >& fromFiles,
    /* [in] */ IFile* toDir)
{
    // Remove any partial files from previous attempt
    List<AutoPtr<IFile> >::Iterator it = fromFiles.Begin();
    for (; it != fromFiles.End(); ++it) {
        AutoPtr<IFile> fromFile = *it;
        String name;
        fromFile->GetName(&name);
        if (name.EndWith(".tmp")) {
            file->Delete();
        }
    }

    it = fromFiles.Begin();
    for (; it != fromFiles.End(); ++it) {
        AutoPtr<IFile> fromFile = *it;
        AutoPtr<IFileHelper> helper;
        CFileHelper::AcquireSingleton((IFileHelper**)&helper);
        AutoPtr<IFile> tmpFile;
        helper->CreateTempFile(String("inherit"), String(".tmp"), toDir, (IFile**)&tmpFile);
        if (LOGD) Slogger::D(TAG, "Copying %p to %p", fromFile.Get(), tmpFile.Get());
        if (!FileUtils::CopyFile(fromFile, tmpFile)) {
            Slogger::E(TAG, "Failed to copy %p to %p", fromFile.Get(), tmpFile.Get());
            return E_IO_EXCEPTION;
        }

        AutoPtr<IFile> toFile;
        String name;
        fromFile->GetName(&name);
        CFile::New(toDir, name, (IFile**)&toFile);
        if (LOGD) Slogger::D(TAG, "Renaming %p to %p", tmpFile.Get(), toFile.Get());
        Boolean result;
        if (tmpFile->RenameTo(toFile, &result), !result) {
            Slogger::E(TAG, "Failed to rename %p to %p", tmpFile.Get());
            return E_IO_EXCEPTION;
        }
    }
    Int32 size;
    fromFiles->GetSize(&size);
    Slogger::D(TAG, "Copied %d files into %p", size, toDir.Get());
    return NOERROR;
}

ECode CPackageInstallerSession::ExtractNativeLibraries(
    /* [in] */ IFile* packageDir,
    /* [in] */ const String& abiOverride)
{
    // Always start from a clean slate
    AutoPtr<IFile> libDir;
    CFile::New(packageDir, INativeLibraryHelper::LIB_DIR_NAME, (IFile**)&libDir);
    AutoPtr<INativeLibraryHelper> helper;
    CNativeLibraryHelper::AcquireSingleton((INativeLibraryHelper**)&helper);
    helper->RemoveNativeBinariesFromDirLI(libDir, TRUE);

    AutoPtr<INativeLibraryHelperHandle> handle;
    // try {
    AutoPtr<INativeLibraryHelperHandleHelper> handleHleper;
    CNativeLibraryHelperHandleHelper::AcquireSingleton((INativeLibraryHelperHandleHelper**)&handleHleper);
    ECode ec = handleHleper->Create(packageDir, (INativeLibraryHelperHandle**)&handle);
    if (ec == (ECode)E_IO_EXCEPTION) {
        goto fail;
    }
    Int32 res;
    ec = helper->CopyNativeBinariesWithOverride(handle, libDir, abiOverride, &res);
    if (ec == (ECode)E_IO_EXCEPTION) {
        goto fail;
    }
    if (res != IPackageManager::INSTALL_SUCCEEDED) {
        mErrorCode = res;
        Slogger::E(TAG, "Failed to extract native libraries, res=%d", res);
        AutoPtr<IIoUtils> ioutils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
        ioutils->CloseQuietly(ICloseable::Probe(os));
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    // } catch (IOException e) {
    //     throw new PackageManagerException(INSTALL_FAILED_INTERNAL_ERROR,
    //             "Failed to extract native libraries", e);
    // } finally {
    //     IoUtils.closeQuietly(handle);
    // }
fail:
    mErrorCode = INSTALL_FAILED_INTERNAL_ERROR;
    Slogger::E(TAG, "Failed to extract native libraries");
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(os));
    return E_PACKAGE_MANAGER_EXCEPTION;
}

ECode CPackageInstallerSession::ResizeContainer(
    /* [in] */ const String& cid,
    /* [in] */ Int64 targetSize)
{
    AutoPtr<IPackageHelper> helper;
    CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
    String path;
    helper->GetSdDir(cid, &path);
    if (path.IsNull) {
        mErrorCode = INSTALL_FAILED_CONTAINER_ERROR;
        Slogger::E(TAG, "Failed to find mounted %s", cid.string());
        return E_PACKAGE_MANAGER_EXCEPTION;
    }

    AutoPtr<IFile> f;
    CFile::New(path, (IFile**)&f);
    Int64 currentSize;
    f->GetTotalSpace(&currentSize);
    if (currentSize > targetSize) {
        Slogger::W(TAG, "Current size %d is larger than target size %d; skipping resize", currentSize, targetSize);
        return NOERROR;
    }

    Boolean result;
    if (helper->UnMountSdDir(cid, &result), !result) {
        mErrorCode = INSTALL_FAILED_CONTAINER_ERROR;
        Slogger::E(TAG, "Failed to unmount %s before resize", cid.string());
        return E_PACKAGE_MANAGER_EXCEPTION;
    }

    if (helper->ResizeSdDir(targetSize, cid,
            PackageManagerService::GetEncryptKey(), &result), !result) {
        mErrorCode = INSTALL_FAILED_CONTAINER_ERROR;
        Slogger::E(TAG, "Failed to resize %s to %d bytes", cid.string(), targetSize);
        return E_PACKAGE_MANAGER_EXCEPTION;
    }

    helper->MountSdDir(cid, PackageManagerService::GetEncryptKey(),
            IProcess::SYSTEM_UID, FALSE, &path);
    if (path.IsNull()) {
        mErrorCode = INSTALL_FAILED_CONTAINER_ERROR;
        Slogger::E(TAG, "Failed to unmount %s after resize", cid.string());
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
}

ECode CPackageInstallerSession::FinalizeAndFixContainer(
    /* [in] */ const String& cid)
{
    AutoPtr<IPackageHelper> helper;
    CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
    Boolean result;
    if (helper->FinalizeSdDir(cid, &result), !result) {
        mErrorCode = INSTALL_FAILED_CONTAINER_ERROR;
        Slogger::E(TAG, "Failed to finalize container %s", cid.string());
        return E_PACKAGE_MANAGER_EXCEPTION;
    }

    Int32 uid;
    mPm->GetPackageUid(IPackageManagerService::DEFAULT_CONTAINER_PACKAGE,
            IUserHandle::USER_OWNER, &uid);
    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
    Int32 gid;
    handleHelper->GetSharedAppGid(uid, &gid);
    if (helper->FixSdPermissions(cid, gid, String(NULL), &result), !result) {
        mErrorCode = INSTALL_FAILED_CONTAINER_ERROR;
        Slogger::E(TAG, "Failed to fix permissions on container %s", cid.string());
        return E_PACKAGE_MANAGER_EXCEPTION;
    }
    return NOERROR;
}

ECode CPackageInstallerSession::SetPermissionsResult(
    /* [in] */ Boolean accepted)
{
    if (!mSealed) {
        Slogger::E(TAG, "Must be sealed to accept permissions");
        return E_SECURITY_EXCEPTION;
    }

    if (accepted) {
        // Mark and kick off another install pass
        mPermissionsAccepted = TRUE;
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_COMMIT, (IMessage**)&msg);
        msg->SendToTarget();
    }
    else {
        DestroyInternal();
        DispatchSessionFinished(INSTALL_FAILED_ABORTED, String("User rejected permissions"), NULL);
    }
    return NOERROR;
}

ECode CPackageInstallerSession::Open()
{
    Int32 value;
    if (mActiveCount->GetAndIncrement(&value), value == 0) {
        mCallback->OnSessionActiveChanged(this, TRUE);
    }

    synchronized (mLock) {
        if (!mPrepared) {
            if (mStageDir != NULL) {
                FAIL_RETURN(CPackageInstallerService::PrepareInternalStageDir(stageDir))
            }
            else if (!mStageCid.IsNull()) {
                Int64 size;
                FAIL_RETURN(CPackageInstallerService::PrepareExternalStageCid(
                        stageCid, params.sizeBytes))

                // TODO: deliver more granular progress for ASEC allocation
                mInternalProgress = 0.25f;
                ComputeProgressLocked(TRUE);
            }
            else {
                Slogger::E(TAG, "Exactly one of stageDir or stageCid stage must be set");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            mPrepared = TRUE;
            mCallback->OnSessionPrepared(this);
        }
    }
    return NOERROR;
}

ECode CPackageInstallerSession::Close()
{
    if (mActiveCount.decrementAndGet() == 0) {
        mCallback.onSessionActiveChanged(this, false);
    }
}

ECode CPackageInstallerSession::Abandon()
{
    DestroyInternal();
    DispatchSessionFinished(IPackageManager::INSTALL_FAILED_ABORTED, "Session was abandoned", NULL);
}

void CPackageInstallerSession::DispatchSessionFinished(
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    mFinalStatus = returnCode;
    mFinalMessage = msg;

    if (mRemoteObserver != NULL) {
        // try {
        mRemoteObserver->OnPackageInstalled(mPackageName, returnCode, msg, extras);
        // } catch (RemoteException ignored) {
        // }
    }

    Boolean success = (returnCode == IPackageManager::INSTALL_SUCCEEDED);
    mCallback->OnSessionFinished(this, success);
}

void CPackageInstallerSession::DestroyInternal()
{
    synchronized (mLock) {
        mSealed = TRUE;
        mDestroyed = TRUE;

        // Force shut down all bridges
        List<AutoPtr<IFileBridge> >::Iterator it = mBridges.Begin();
        for (; it != mBridges.End(); ++it) {
            (*it)->ForceClose();
        }
    }
    if (stageDir != null) {
        FileUtils.deleteContents(stageDir);
        stageDir.delete();
    }
    if (stageCid != null) {
        PackageHelper.destroySdDir(stageCid);
    }
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
