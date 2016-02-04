
#ifndef __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLERSESSION_H__
#define __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLERSESSION_H__

#include "_Elastos_Droid_Server_Pm_CPackageInstallerSession.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::Pm::IIPackageInstallerSession;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionParams;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionInfo;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver2;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IFileBridge;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class CLocalObserver;
class InternalCallback;

CarClass(CPackageInstallerSession)
    , public Object
    , public IIPackageInstallerSession
    , public IBinder
{
private:
    class MyHandlerCallBack
        : public Object
        , public IHandlerCallback
    {
    public:
        MyHandlerCallBack(
            /* [in] */ CPackageInstallerSession* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        CPackageInstallerSession* mHost;
    };

public:
    CPackageInstallerSession();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
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
        /* [in] */ Boolean sealed);

    CARAPI_(AutoPtr<IPackageInstallerSessionInfo>) GenerateInfo();

    CARAPI_(Boolean) IsPrepared();

    CARAPI_(Boolean) IsSealed();

    CARAPI SetClientProgress(
        /* [in] */ Float progress);

    CARAPI AddClientProgress(
        /* [in] */ Float progress);

    CARAPI GetNames(
        /* [out, callee] */ ArrayOf<String>** names);

    CARAPI OpenWrite(
        /* [in] */ const String& name,
        /* [in] */ Int64 offsetBytes,
        /* [in] */ Int64 lengthBytes,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI OpenRead(
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI SetPermissionsResult(
        /* [in] */ Boolean accepted);

    CARAPI Open();

    CARAPI Close();

    CARAPI Commit(
        /* [in] */ IIntentSender* statusReceiver);

    CARAPI Abandon();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI AssertPreparedAndNotSealed(
        /* [in] */ const String& cookie);

    /**
     * Resolve the actual location where staged data should be written. This
     * might point at an ASEC mount point, which is why we delay path resolution
     * until someone actively works with the session.
     */
    CARAPI ResolveStageDir(
        /* [out] */ IFile** dir);

    CARAPI_(void) ComputeProgressLocked(
        /* [in] */ Boolean forcePublish);

    CARAPI OpenWriteInternal(
        /* [in] */ const String& name,
        /* [in] */ Int64 offsetBytes,
        /* [in] */ Int64 lengthBytes,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI OpenReadInternal(
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI CommitLocked();

    /**
     * Validate install by confirming that all application packages are have
     * consistent package name, version code, and signing certificates.
     * <p>
     * Clears and populates {@link #mResolvedBaseFile},
     * {@link #mResolvedStagedFiles}, and {@link #mResolvedInheritedFiles}.
     * <p>
     * Renames package files in stage to match split names defined inside.
     * <p>
     * Note that upgrade compatibility is still performed by
     * {@link PackageManagerService}.
     */
    CARAPI ValidateInstallLocked();

    CARAPI AssertApkConsistent(
        /* [in] */ const String& tag,
        /* [in] */ PackageParser::ApkLite* apk);

    /**
     * Calculate the final install footprint size, combining both staged and
     * existing APKs together and including unpacked native code from both.
     */
    CARAPI CalculateInstalledSize(
        /* [out] */ Int64* size);

    static CARAPI LinkFiles(
        /* [in] */ List<AutoPtr<IFile> >& fromFiles,
        /* [in] */ IFile* toDir);

    static CARAPI CopyFiles(
        /* [in] */ List<AutoPtr<IFile> >& fromFiles,
        /* [in] */ IFile* toDir);

    static CARAPI ExtractNativeLibraries(
        /* [in] */ IFile* packageDir,
        /* [in] */ const String& abiOverride);

    static CARAPI ResizeContainer(
        /* [in] */ const String& cid,
        /* [in] */ Int64 targetSize);

    CARAPI FinalizeAndFixContainer(
        /* [in] */ const String& cid);

    CARAPI_(void) DispatchSessionFinished(
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg,
        /* [in] */ IBundle* extras);

    CARAPI_(void) DestroyInternal();

public:
    Int32 mSessionId;
    Int32 mUserId;
    String mInstallerPackageName;
    Int32 mInstallerUid;
    AutoPtr<IPackageInstallerSessionParams> mParams;
    Int64 mCreatedMillis;

    /** Staging location where client data is written. */
    AutoPtr<IFile> mStageDir;
    String mStageCid;

private:
    static const String TAG;
    static const Boolean LOGD = TRUE;

    static const Int32 MSG_COMMIT = 0;

    // TODO: enforce INSTALL_ALLOW_TEST
    // TODO: enforce INSTALL_ALLOW_DOWNGRADE

    AutoPtr<InternalCallback> mCallback;
    AutoPtr<IContext> mContext;
    AutoPtr<CPackageManagerService> mPm;
    AutoPtr<IHandler> mHandler;

    AutoPtr<IAtomicInteger32> mActiveCount;

    Object mLock;

    // @GuardedBy("mLock")
    Float mClientProgress;
    // @GuardedBy("mLock")
    Float mInternalProgress;

    // @GuardedBy("mLock")
    Float mProgress;
    // @GuardedBy("mLock")
    Float mReportedProgress;

    // @GuardedBy("mLock")
    Boolean mPrepared;
    // @GuardedBy("mLock")
    Boolean mSealed;
    // @GuardedBy("mLock")
    Boolean mPermissionsAccepted;
    // @GuardedBy("mLock")
    Boolean mDestroyed;

    Int32 mFinalStatus;
    String mFinalMessage;

    // @GuardedBy("mLock")
    List<AutoPtr<IFileBridge> > mBridges;

    // @GuardedBy("mLock")
    AutoPtr<IIPackageInstallObserver2> mRemoteObserver;

    /** Fields derived from commit parsing */
    String mPackageName;
    Int32 mVersionCode;
    AutoPtr<ArrayOf<ISignature*> > mSignatures;

    /**
     * Path to the validated base APK for this session, which may point at an
     * APK inside the session (when the session defines the base), or it may
     * point at the existing base APK (when adding splits to an existing app).
     * <p>
     * This is used when confirming permissions, since we can't fully stage the
     * session inside an ASEC before confirming with user.
     */
    // @GuardedBy("mLock")
    AutoPtr<IFile> mResolvedBaseFile;

    // @GuardedBy("mLock")
    AutoPtr<IFile> mResolvedStageDir;

    // @GuardedBy("mLock")
    List<AutoPtr<IFile> > mResolvedStagedFiles;
    // @GuardedBy("mLock")
    List<AutoPtr<IFile> > mResolvedInheritedFiles;

    AutoPtr<IHandlerCallback> mHandlerCallback;

    static Int32 sErrorCode;

    friend class HandlerCallBack;
    friend class CLocalObserver;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLERSESSION_H__
