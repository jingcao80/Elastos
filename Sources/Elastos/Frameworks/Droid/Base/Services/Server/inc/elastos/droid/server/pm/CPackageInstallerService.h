
#ifndef __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLERSERVICE_H__
#define __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLERSERVICE_H__

#include "_Elastos_Droid_Server_Pm_CPackageInstallerService.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/server/pm/CPackageInstallerSession.h"
#include "elastos/droid/app/PackageDeleteObserver.h"
#include "elastos/droid/app/PackageInstallObserver.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Text.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::PackageDeleteObserver;
using Elastos::Droid::App::PackageInstallObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::Pm::IIPackageInstaller;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionInfo;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionParams;
using Elastos::Droid::Content::Pm::IIPackageInstallerSession;
using Elastos::Droid::Content::Pm::IPackageInstallerCallback;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::IO::IFilenameFilter;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class CPackageInstallerService;

class InternalCallback : public Object
{
private:
    class SessionFinishedRunnable : public Runnable
    {
    public:
        SessionFinishedRunnable(
            /* [in] */ CPackageInstallerService* host,
            /* [in] */ CPackageInstallerSession* session)
            : mHost(host)
            , mSession(session)
        {}

        CARAPI Run();

    private:
        CPackageInstallerService* mHost;
        CPackageInstallerSession* mSession;
    };

public:
    InternalCallback(
        /* [in] */ CPackageInstallerService* host)
        : mHost(host)
    {}

    CARAPI_(void) OnSessionBadgingChanged(
        /* [in] */ CPackageInstallerSession* session);

    CARAPI_(void) OnSessionActiveChanged(
        /* [in] */ CPackageInstallerSession* session,
        /* [in] */ Boolean active);

    CARAPI_(void) OnSessionProgressChanged(
        /* [in] */ CPackageInstallerSession* session,
        /* [in] */ Float progress);

    CARAPI_(void) OnSessionFinished(
        /* [in] */ CPackageInstallerSession* session,
        /* [in] */ Boolean success);

    CARAPI_(void) OnSessionPrepared(
        /* [in] */ CPackageInstallerSession* session);

    CARAPI_(void) OnSessionSealedBlocking(
        /* [in] */ CPackageInstallerSession* session);

private:
    CPackageInstallerService* mHost;
};

CarClass(CPackageInstallerService)
    , public Object
    , public IIPackageInstaller
    , public IBinder
{
public:
    class PackageDeleteObserverAdapter : public PackageDeleteObserver
    {
    public:
        PackageDeleteObserverAdapter(
            /* [in] */ IContext* context,
            /* [in] */ IIntentSender* target,
            /* [in] */ const String& packageName)
            : mContext(context)
            , mTarget(target)
            , mPackageName(packageName)
        {}

        CARAPI OnUserActionRequired(
            /* [in] */ IIntent* intent);

        CARAPI OnPackageDeleted(
            /* [in] */ const String& basePackageName,
            /* [in] */ Int32 returnCode,
            /* [in] */ const String& msg);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IIntentSender> mTarget;
        String mPackageName;
    };

    class PackageInstallObserverAdapter : public PackageInstallObserver
    {
    public:
        PackageInstallObserverAdapter(
            /* [in] */ IContext* context,
            /* [in] */ IIntentSender* target,
            /* [in] */ Int32 sessionId)
            : mContext(context)
            , mTarget(target)
            , mSessionId(sessionId)
        {}

        CARAPI OnUserActionRequired(
            /* [in] */ IIntent* intent);

        CARAPI OnPackageInstalled(
            /* [in] */ const String& basePackageName,
            /* [in] */ Int32 returnCode,
            /* [in] */ const String& msg,
            /* [in] */ IBundle* extras);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IIntentSender> mTarget;
        Int32 mSessionId;
    };

private:
    class StageFilter
        : public Object
        , public IFilenameFilter
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Accept(
            /* [in] */ IFile* dir,
            /* [in] */ const String& name,
            /* [out] */ Boolean* succeeded);
    };

    class WriteSessionsRunnable : public Runnable
    {
    public:
        WriteSessionsRunnable(
            /* [in] */ CPackageInstallerService* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CPackageInstallerService* mHost;
    };

    class Callbacks : public Handler
    {
    public:
        Callbacks(
            /* [in] */ ILooper* looper);

        CARAPI_(void) Register(
            /* [in] */ IPackageInstallerCallback* callback,
            /* [in] */ Int32 userId);

        CARAPI_(void) Unregister(
            /* [in] */ IPackageInstallerCallback* callback);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI_(void) NotifySessionFinished(
            /* [in] */ Int32 sessionId,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean success);

    private:
        CARAPI InvokeCallback(
            /* [in] */ IPackageInstallerCallback* callback,
            /* [in] */ IMessage* msg);

        CARAPI_(void) NotifySessionCreated(
            /* [in] */ Int32 sessionId,
            /* [in] */ Int32 userId);

        CARAPI_(void) NotifySessionBadgingChanged(
            /* [in] */ Int32 sessionId,
            /* [in] */ Int32 userId);

        CARAPI_(void) NotifySessionActiveChanged(
            /* [in] */ Int32 sessionId,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean active);

        CARAPI_(void) NotifySessionProgressChanged(
            /* [in] */ Int32 sessionId,
            /* [in] */ Int32 userId,
            /* [in] */ Float progress);

    private:
        static const Int32 MSG_SESSION_CREATED = 1;
        static const Int32 MSG_SESSION_BADGING_CHANGED = 2;
        static const Int32 MSG_SESSION_ACTIVE_CHANGED = 3;
        static const Int32 MSG_SESSION_PROGRESS_CHANGED = 4;
        static const Int32 MSG_SESSION_FINISHED = 5;

        AutoPtr<IRemoteCallbackList> mCallbacks;

        friend class InternalCallback;
        friend class CPackageInstallerService;
    };

public:
    CPackageInstallerService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IIPackageManager* pm,
        /* [in] */ IFile* stagingDir);

    CARAPI_(void) OnSecureContainersAvailable();

    static CARAPI_(Boolean) IsStageName(
        /* [in] */ const String& name);

    CARAPI AllocateInternalStageDirLegacy(
        /* [out] */ IFile** dir);

    CARAPI_(String) AllocateExternalStageCidLegacy();

    CARAPI CreateSession(
        /* [in] */ IPackageInstallerSessionParams* params,
        /* [in] */ const String& installerPackageName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI UpdateSessionAppIcon(
        /* [in] */ Int32 sessionId,
        /* [in] */ IBitmap* appIcon);

    CARAPI UpdateSessionAppLabel(
        /* [in] */ Int32 sessionId,
        /* [in] */ const String& appLabel);

    CARAPI AbandonSession(
        /* [in] */ Int32 sessionId);

    CARAPI OpenSession(
        /* [in] */ Int32 sessionId,
        /* [out] */ IIPackageInstallerSession** session);

    static CARAPI PrepareInternalStageDir(
        /* [in] */ IFile* stageDir);

    static CARAPI PrepareExternalStageCid(
        /* [in] */ const String& stageCid,
        /* [in] */ Int64 sizeBytes);

    CARAPI GetSessionInfo(
        /* [in] */ Int32 sessionId,
        /* [out] */ IPackageInstallerSessionInfo** info);

    CARAPI GetAllSessions(
        /* [in] */ Int32 userId,
        /* [out] */ IParceledListSlice** slice);

    CARAPI GetMySessions(
        /* [in] */ const String& installerPackageName,
        /* [in] */ Int32 userId,
        /* [out] */ IParceledListSlice** slice);

    CARAPI RegisterCallback(
        /* [in] */ IPackageInstallerCallback* cb,
        /* [in] */ Int32 userId);

    CARAPI UnregisterCallback(
        /* [in] */ IPackageInstallerCallback* cb);

    CARAPI Uninstall(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 flags,
        /* [in] */ IIntentSender* statusReceiver,
        /* [in] */ Int32 userId);

    CARAPI SetPermissionsResult(
        /* [in] */ Int32 sessionId,
        /* [in] */ Boolean accepted);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static CARAPI_(AutoPtr<IFilenameFilter>) InitStageFilter();

    CARAPI_(void) ReadSessionsLocked();

    CARAPI ReadSessionLocked(
        /* [in] */ IXmlPullParser* in,
        /* [out] */ CPackageInstallerSession** session);

    CARAPI_(void) WriteSessionsLocked();

    CARAPI WriteSessionLocked(
        /* [in] */ IXmlSerializer* out,
        /* [in] */ CPackageInstallerSession* session);

    CARAPI_(AutoPtr<IFile>) BuildAppIconFile(
        /* [in] */ Int32 sessionId);

    CARAPI_(void) WriteSessionsAsync();

    CARAPI CreateSessionInternal(
        /* [in] */ IPackageInstallerSessionParams* params,
        /* [in] */ const String& installerPackageName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI OpenSessionInternal(
        /* [in] */ Int32 sessionId,
        /* [out] */ IIPackageInstallerSession** session);

    CARAPI AllocateSessionIdLocked(
        /* [out] */ Int32* id);

    CARAPI_(AutoPtr<IFile>) BuildInternalStageDir(
        /* [in] */ Int32 sessionId);

    CARAPI_(String) BuildExternalStageCid(
        /* [in] */ Int32 sessionId);

    static CARAPI_(Int32) GetSessionCount(
        /* [in] */ HashMap<Int32, AutoPtr<CPackageInstallerSession> >& sessions,
        /* [in] */ Int32 installerUid);

    CARAPI_(Boolean) IsCallingUidOwner(
        /* [in] */ CPackageInstallerSession* session);

private:
    static const String TAG;
    static const Boolean LOGD;

    // TODO: remove outstanding sessions when installer package goes away
    // TODO: notify listeners in other users when package has been installed there
    // TODO: purge expired sessions periodically in addition to at reboot

    /** XML constants used in {@link #mSessionsFile} */
    static const String TAG_SESSIONS;
    static const String TAG_SESSION;
    static const String ATTR_SESSION_ID;
    static const String ATTR_USER_ID;
    static const String ATTR_INSTALLER_PACKAGE_NAME;
    static const String ATTR_INSTALLER_UID;
    static const String ATTR_CREATED_MILLIS;
    static const String ATTR_SESSION_STAGE_DIR;
    static const String ATTR_SESSION_STAGE_CID;
    static const String ATTR_PREPARED;
    static const String ATTR_SEALED;
    static const String ATTR_MODE;
    static const String ATTR_INSTALL_FLAGS;
    static const String ATTR_INSTALL_LOCATION;
    static const String ATTR_SIZE_BYTES;
    static const String ATTR_APP_PACKAGE_NAME;
    // @Deprecated
    static const String ATTR_APP_ICON;
    static const String ATTR_APP_LABEL;
    static const String ATTR_ORIGINATING_URI;
    static const String ATTR_REFERRER_URI;
    static const String ATTR_ABI_OVERRIDE;

    /** Automatically destroy sessions older than this */
    static const Int64 MAX_AGE_MILLIS;
    /** Upper bound on number of active sessions for a UID */
    static const Int64 MAX_ACTIVE_SESSIONS = 1024;
    /** Upper bound on number of historical sessions for a UID */
    static const Int64 MAX_HISTORICAL_SESSIONS = 1048576;

    AutoPtr<IContext> mContext;
    CPackageManagerService* mPm;
    AutoPtr<IAppOpsManager> mAppOps;

    AutoPtr<IFile> mStagingDir;
    AutoPtr<IHandlerThread> mInstallThread;
    AutoPtr<IHandler> mInstallHandler;

    AutoPtr<Callbacks> mCallbacks;

    /**
     * File storing persisted {@link #mSessions} metadata.
     */
    AutoPtr<IAtomicFile> mSessionsFile;

    /**
     * Directory storing persisted {@link #mSessions} metadata which is too
     * heavy to store directly in {@link #mSessionsFile}.
     */
    AutoPtr<IFile> mSessionsDir;

    AutoPtr<InternalCallback> mInternalCallback;

    /**
     * Used for generating session IDs. Since this is created at boot time,
     * normal random might be predictable.
     */
    AutoPtr<IRandom> mRandom;

    // @GuardedBy("mSessions")
    HashMap<Int32, AutoPtr<CPackageInstallerSession> > mSessions;
    Object mSessionsLock;

    /** Historical sessions kept around for debugging purposes */
    // @GuardedBy("mSessions")
    HashMap<Int32, AutoPtr<CPackageInstallerSession> > mHistoricalSessions;

    /** Sessions allocated to legacy users */
    // @GuardedBy("mSessions")
    HashMap<Int32, Boolean> mLegacySessions;

    static AutoPtr<IFilenameFilter> sStageFilter;

    friend class WriteSessionsRunnable;
    friend class InternalCallback;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CPackageInstallerService_H__
