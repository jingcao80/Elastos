
#ifndef __ELASTOS_DROID_SERVER_CBACKUPMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CBACKUPMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CBackupManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/os/Runnable.h"
#include "PackageManagerBackupAgent.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>
#include <elastos/Map.h>

using Elastos::Core::IRandom;
using Elastos::Core::IStringBuilder;
using Elastos::Crypto::ISecretKey;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IPrintWriter;
using Elastos::Security::ISecureRandom;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Zip::IDeflaterOutputStream;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::IBackupManager;
using Elastos::Droid::App::Backup::IIFullBackupRestoreObserver;
using Elastos::Droid::App::Backup::IIRestoreSession;
using Elastos::Droid::App::Backup::IIRestoreObserver;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageDataObserver;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInstallObserver;
using Elastos::Droid::Content::Pm::IPackageDeleteObserver;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::Backup::IIBackupTransport;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::PackageManagerBackupAgent;


namespace Elastos {
namespace Droid {
namespace Server {

extern "C" const InterfaceID EIID_FullBackupParams;
extern "C" const InterfaceID EIID_FullRestoreParams;
CarClass(CBackupManagerService)
{
private:

    // ----- Asynchronous backup/restore handler thread -----
    class BackupHandler : public HandlerBase
    {
    public:
        TO_STRING_IMPL("CBackupManagerService::BackupHandler")

        BackupHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CBackupManagerService* service);

        HandleMessage(
            /* [in]] */ IMessage* msg);

    private:
        CBackupManagerService* mHost;
    };

    // set of backup services that have pending changes
    class BackupRequest : public ElRefBase
    {
    public:
        BackupRequest(
            /* [in] */ const String& pkgName);

        String ToString();

        String mPackageName;
    };


    class ProvisionedObserver
        : public ContentObserver
    {
    public:
        ProvisionedObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CBackupManagerService* service);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        CBackupManagerService* mHost;
    };

    class RestoreGetSetsParams
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        RestoreGetSetsParams(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ IIRestoreSession* session,
            /* [in] */ IIRestoreObserver* observer);

    public:
        AutoPtr<IIBackupTransport> mTransport;
        AutoPtr<IIRestoreSession> mSession;
        AutoPtr<IIRestoreObserver> mObserver;
        Object mSessionLock;
    };

    class RestoreParams
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        RestoreParams(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ IIRestoreObserver* obs,
            /* [in] */ Int64 token,
            /* [in] */ IPackageInfo* cap,
            /* [in] */ Int32 pmToken,
            /* [in] */ Boolean needFullBackup);

        RestoreParams(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ IIRestoreObserver* obs,
            /* [in] */ Int64 token,
            /* [in] */ Boolean needFullBackup);

        RestoreParams(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ IIRestoreObserver* obs,
            /* [in] */ Int64 token,
            /* [in] */ ArrayOf<String>* filterSet,
            /* [in] */ Boolean needFullBackup);

    public:
        AutoPtr<IIBackupTransport> mTransport;
        AutoPtr<IIRestoreObserver> mObserver;
        Int64 mToken;
        AutoPtr<IPackageInfo> mPkgInfo;
        Int32 mPmToken; // in post-install restore, the PM's token for this transaction
        Boolean mNeedFullBackup;
        AutoPtr<ArrayOf<String> > mFilterSet;
    };

    class ClearParams
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        ClearParams(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ IPackageInfo* info);

    public:
        AutoPtr<IIBackupTransport> mTransport;
        AutoPtr<IPackageInfo> mPkgInfo;
    };

    class FullParams
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()
        FullParams();

    public:
        AutoPtr<IParcelFileDescriptor> mFd;
        AutoPtr<IAtomicBoolean> mLatch;
        Object mLatchLock;
        AutoPtr<IIFullBackupRestoreObserver> mObserver;
        String mCurPassword;     // filled in by the confirmation step
        String mEncryptPassword;
    };

    class FullBackupParams
        : public FullParams
    {
    public:
        FullBackupParams(
            /* [in] */ IParcelFileDescriptor* output,
            /* [in] */ Boolean saveApks,
            /* [in] */ Boolean saveShared,
            /* [in] */ Boolean doAllApps,
            /* [in] */ Boolean doSystem,
            /* [in] */ ArrayOf<String>* pkgList);

        PInterface Probe(
            /* [in] */ REIID riid);

    public:
        Boolean mIncludeApks;
        Boolean mIncludeShared;
        Boolean mAllApps;
        Boolean mIncludeSystem;
        AutoPtr<ArrayOf<String> > mPackages;
    };

    class FullRestoreParams
        : public FullParams
    {
    public:
        FullRestoreParams(
            /* [in] */ IParcelFileDescriptor* input);

        PInterface Probe(
            /* [in] */ REIID riid);
    };

    class BackupRestoreTask;
    class Operation : public ElRefBase
    {
    public:
        Operation(
            /* [in] */ Int32 initialState,
            /* [in] */ BackupRestoreTask* callbackObj);

        Int32 mState;
        AutoPtr<BackupRestoreTask> mCallback;

    };

    class RunBackupReceiver
        : public BroadcastReceiver
    {
    public:
        RunBackupReceiver(
            /* [in] */ CBackupManagerService* host);

    protected:
        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CBackupManagerService* mHost;
    };

    class RunInitializeReceiver
        : public BroadcastReceiver
    {
    public:
        RunInitializeReceiver(
            /* [in] */ CBackupManagerService* host);

    protected:
        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CBackupManagerService* mHost;
    };

    class ClearDataObserver
        : public ElRefBase
        , public IPackageDataObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        ClearDataObserver(
            /* [in] */ CBackupManagerService* host);

        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

        CARAPI ToString(
            /* [out] */ String* res);

    private:
        CBackupManagerService*  mHost;
    };

    // -----
    // Interface and methods used by the asynchronous-with-timeout backup/restore operations

    class BackupRestoreTask
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        // Execute one tick of whatever state machine the task implements
        virtual CARAPI_(void) Execute() = 0;

        // An operation that wanted a callback has completed
        virtual CARAPI_(void) OperationComplete() = 0;

        // An operation that wanted a callback has timed out
        virtual CARAPI_(void) HandleTimeout() = 0;
    };

    // ----- Back up a set of applications via a worker thread -----

    class PerformBackupTask : public BackupRestoreTask
    {
    public:
        enum BackupState {
            INITIAL,
            RUNNING_QUEUE,
            FINAL,
        };

    public:
        PerformBackupTask(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ List<AutoPtr<BackupRequest> >* queue,
            /* [in] */ IFile* journal,
            /* [in] */ CBackupManagerService* host);

        // Main entry point: perform one chunk of work, updating the state as appropriate
        // and reposting the next chunk to the primary backup handler thread.
        virtual CARAPI_(void) Execute();

        // We're starting a backup pass.  Initialize the transport and send
        // the PM metadata blob if we haven't already.
        CARAPI_(void) BeginBackup();

        // Transport has been initialized and the PM metadata submitted successfully
        // if that was warranted.  Now we process the single next thing in the queue.
        CARAPI_(void) InvokeNextAgent();

        CARAPI_(void) FinalizeBackup();

        // Remove the PM metadata state. This will generate an init on the next pass.
        CARAPI_(void) ClearMetadata();

        // Invoke an agent's doBackup() and start a timeout message spinning on the main
        // handler in case it doesn't get back to us.
        CARAPI_(Int32) InvokeAgentForBackup(
            /* [in] */ const String& packageName,
            /* [in] */ IIBackupAgent* agent,
            /* [in] */ IIBackupTransport* transport);

        virtual CARAPI_(void) OperationComplete();

        virtual CARAPI_(void) HandleTimeout();

        CARAPI_(void) RevertAndEndBackup();

        CARAPI_(void) AgentErrorCleanup();

        // Cleanup common to both success and failure cases
        CARAPI_(void) ClearAgentState();

        CARAPI_(void) RestartBackupAlarm();

        CARAPI_(void) ExecuteNextState(
            /* [in] */ BackupState nextState);

    private:
        static const String TAG;
        typedef List<AutoPtr<BackupRequest> > BackupRequestList;
        AutoPtr<IIBackupTransport> mTransport;
        AutoPtr<BackupRequestList> mQueue;
        AutoPtr<BackupRequestList> mOriginalQueue;
        AutoPtr<IFile> mStateDir;
        AutoPtr<IFile> mJournal;
        BackupState mCurrentState;
        CBackupManagerService*  mHost;
        AutoPtr<IPackageInfo> mCurrentPackage;
        AutoPtr<IFile> mSavedStateName;
        AutoPtr<IFile> mBackupDataName;
        AutoPtr<IFile> mNewStateName;
        AutoPtr<IParcelFileDescriptor> mSavedState;
        AutoPtr<IParcelFileDescriptor> mBackupData;
        AutoPtr<IParcelFileDescriptor> mNewState;
        Int32 mStatus;
        Boolean mFinished;
    };


    // ----- Full backup to a file/socket -----

    class PerformFullBackupTask
        : public Runnable
    {
    public:
        class FullBackupRunner
            : public Runnable
        {
        public:
            FullBackupRunner(
                /* [in] */ IPackageInfo* pack,
                /* [in] */ IIBackupAgent* agent,
                /* [in] */ IParcelFileDescriptor* pipe,
                /* [in] */ Int32 token,
                /* [in] */ Boolean sendApk,
                /* [in] */ Boolean writeManifest,
                /* [in] */ PerformFullBackupTask* host);

            CARAPI Run();

        private:
            AutoPtr<IPackageInfo> mPackage;
            AutoPtr<IIBackupAgent> mAgent;
            AutoPtr<IParcelFileDescriptor> mPipe;
            Int32 mToken;
            Boolean mSendApk;
            Boolean mWriteManifest;
            PerformFullBackupTask* mHost;
        };

        PerformFullBackupTask(
            /* [in] */ IParcelFileDescriptor* fd,
            /* [in] */ IIFullBackupRestoreObserver* observer,
            /* [in] */ Boolean includeApks,
            /* [in] */ Boolean includeShared,
            /* [in] */ const String& curPassword,
            /* [in] */ const String& encryptPassword,
            /* [in] */ Boolean doAllApps,
            /* [in] */ Boolean doSystem,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ IAtomicBoolean* latch,
            /* [in] */ CBackupManagerService* service);

        CARAPI Run();

    private:
        CARAPI_(AutoPtr<IOutputStream>) EmitAesBackupHeader(
            /* [in] */ IStringBuilder* headerbuf,
            /* [in] */ IOutputStream* ofstream);

        CARAPI_(void) BackupOnePackage(
            /* [in] */ IPackageInfo* pkg,
            /* [in] */ IOutputStream* out);

        CARAPI_(void) WriteApkToBackup(
            /* [in] */ IPackageInfo* pkg,
            /* [in] */ IBackupDataOutput* output);

        CARAPI_(void) FinalizeBackup(
            /* [in] */ IOutputStream* out);

        CARAPI_(void) WriteAppManifest(
            /* [in] */ IPackageInfo* pkg,
            /* [in] */ IFile* manifestFile,
            /* [in] */ Boolean withApk);

        CARAPI_(void) TearDown(
            /* [in] */ IPackageInfo* pkg);

        // wrappers for observer use
        CARAPI_(void) SendStartBackup();

        CARAPI_(void) SendOnBackupPackage(
            /* [in] */ const String& name);

        CARAPI_(void) SendEndBackup();

    private:
        AutoPtr<IParcelFileDescriptor> mOutputFile;
        AutoPtr<IDeflaterOutputStream> mDeflater;
        AutoPtr<IIFullBackupRestoreObserver> mObserver;
        Boolean mIncludeApks;
        Boolean mIncludeShared;
        Boolean mAllApps;
        const Boolean mIncludeSystem;
        AutoPtr<ArrayOf<String> > mPackages;
        String mCurrentPassword;
        String mEncryptPassword;
        AutoPtr<IAtomicBoolean> mLatchObject;
        Object mLatchObjectLock;
        AutoPtr<IFile> mFilesDir;
        AutoPtr<IFile> mManifestFile;
        CBackupManagerService* mHost;
    };

    // ----- Full restore from a file/socket -----

    // Description of a file in the restore datastream
    class FileMetadata
        : public ElRefBase
    {
    public:
        String ToString();

        String mPackageName;             // name of the owning app
        String mInstallerPackageName;    // name of the market-type app that installed the owner
        Int32 mType;                       // e.g. BackupAgent.TYPE_DIRECTORY
        String mDomain;                  // e.g. FullBackup.DATABASE_TREE_TOKEN
        String mPath;                    // subpath within the semantic domain
        Int64 mMode;                      // e.g. 0666 (actually int)
        Int64 mMtime;                     // last mod time, UTC time_t (actually int)
        Int64 mSize;                      // bytes of content
    };

    enum RestorePolicy {
        IGNORE,
        ACCEPT,
        ACCEPT_IF_APK,
    };

    class PerformFullRestoreTask
        : public Runnable
    {
    public:

        class RestoreFileRunnable
            : public Runnable
        {
        public:
            RestoreFileRunnable(
                /* [in] */ IIBackupAgent* agent,
                /* [in] */ FileMetadata* info,
                /* [in] */ IParcelFileDescriptor* socket,
                /* [in] */ Int32 token,
                /* [in] */ PerformFullRestoreTask* host);

            CARAPI Run();

        private:
            AutoPtr<IIBackupAgent> mAgent;
            AutoPtr<FileMetadata> mInfo;
            AutoPtr<IParcelFileDescriptor> mSocket;
            Int32 mToken;
            PerformFullRestoreTask* mHost;
        };

        class RestoreInstallObserver
            : public ElRefBase
            , public IPackageInstallObserver
            , public IBinder
        {
        public:
            CAR_INTERFACE_DECL()

            RestoreInstallObserver();

            CARAPI Reset();

            CARAPI WaitForCompletion();

            CARAPI_(Int32) GetResult();

            CARAPI PackageInstalled(
                /* [in] */ const String& packageName,
                /* [in] */ Int32 returnCode);

            AutoPtr<IAtomicBoolean> mDone ;
            Object mDoneLock;
            String mPackageName;
            Int32 mResult;
        };

        class RestoreDeleteObserver
            : public ElRefBase
            , public IPackageDeleteObserver
            , public IBinder
        {
        public:
            CAR_INTERFACE_DECL()

            RestoreDeleteObserver();

            CARAPI Reset();

            CARAPI WaitForCompletion();

            CARAPI PackageDeleted(
                /* [in] */ const String& packageName,
                /* [in] */ Int32 returnCode);

        private:
            AutoPtr<IAtomicBoolean> mDone;
            Object mDoneLock;
            Int32 mResult;
        };

        PerformFullRestoreTask(
            /* [in] */ IParcelFileDescriptor* fd,
            /* [in] */ const String& curPassword,
            /* [in] */ const String& decryptPassword,
            /* [in] */ IIFullBackupRestoreObserver* observer,
            /* [in] */ IAtomicBoolean* latch,
            /* [in] */ CBackupManagerService* service);

        CARAPI Run();

    private:
        CARAPI ReadHeaderLine(
            /* [in] */ IInputStream* in,
            /* [out] */ String* out);

        CARAPI_(AutoPtr<IInputStream>) DecodeAesHeaderAndInitialize(
            /* [in] */ const String& encryptionName,
            /* [in] */ IInputStream* rawInStream);

        CARAPI_(Boolean) RestoreOneFile(
            /* [in] */ IInputStream* instream,
            /* [in] */ ArrayOf<Byte>* buffer);

        CARAPI_(void) SetUpPipes();

        CARAPI_(void) TearDownPipes();

        CARAPI_(void) TearDownAgent(
            /* [in] */ IApplicationInfo* app);

        CARAPI_(Boolean) InstallApk(
            /* [in] */ FileMetadata* info,
            /* [in] */ const String& installerPackage,
            /* [in] */ IInputStream* instream);

        // Given an actual file content size, consume the post-content padding mandated
        // by the tar format.
        CARAPI SkipTarPadding(
            /* [in] */ Int64 size,
            /* [in] */ IInputStream* instream);

        // Returns a policy constant; takes a buffer arg to reduce memory churn
        CARAPI ReadAppManifest(
            /* [in] */ FileMetadata* info,
            /* [in] */ IInputStream* instream,
            /* [out] */ RestorePolicy* out);

        // Builds a line from a byte buffer starting at 'offset', and returns
        // the index of the next unconsumed data in the buffer.
        CARAPI ExtractLine(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ ArrayOf<String>* outStr,
            /* [out] */ Int32* result);

        CARAPI_(void) DumpFileMetadata(
            /* [in] */ FileMetadata* info);

        // Consume a tar file header block [sequence] and accumulate the relevant metadata
        CARAPI ReadTarHeaders(
            /* [in] */ IInputStream* instream,
            /* [out] */ FileMetadata** outInfo);

        CARAPI_(void) HEXLOG(
            /* [in] */ ArrayOf<Byte>* block);

        // Read exactly the given number of bytes into a buffer at the stated offset.
        // Returns false if EOF is encountered before the requested number of bytes
        // could be read.
        CARAPI ReadExactly(
            /* [in] */ IInputStream* in,
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 size,
            /* [out] */ Int32* soFar);

        CARAPI ReadTarHeader(
            /* [in] */ IInputStream* instream,
            /* [in] */ ArrayOf<Byte>* block,
            /* [out] */ Boolean* result);

        // overwrites 'info' fields based on the pax extended header
        CARAPI ReadPaxExtendedHeader(
            /* [in] */ IInputStream* instream,
            /* [in] */ FileMetadata* info,
            /* [out] */ Boolean* result);

        CARAPI ExtractRadix(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 maxChars,
            /* [in] */ Int32 radix,
            /* [out] */ Int64* result);

        CARAPI_(String) ExtractString(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 maxChars);


        CARAPI_(void) SendStartRestore();

        CARAPI_(void) SendOnRestorePackage(
            /* [in] */ const String& name);

        CARAPI_(void) SendEndRestore();

    private:
        AutoPtr<IParcelFileDescriptor> mInputFile;
        String mCurrentPassword;
        String mDecryptPassword;
        AutoPtr<IIFullBackupRestoreObserver> mObserver;
        AutoPtr<IAtomicBoolean> mLatchObject;
        Object mLatchObjectLock;
        AutoPtr<IIBackupAgent> mAgent;
        String mAgentPackage;
        AutoPtr<IApplicationInfo> mTargetApp;
        AutoPtr<ArrayOf<IParcelFileDescriptor*> > mPipes ;
        Int64 mBytes;

        // possible handling states for a given package in the restore dataset
        HashMap<String, RestorePolicy> mPackagePolicies;

        // installer package names for each encountered app, derived from the manifests
        HashMap<String, String> mPackageInstallers;

        // Signatures for a given package found in its manifest file
        HashMap<String, AutoPtr<ArrayOf<ISignature*> > > mManifestSignatures;

        // Packages we've already wiped data on when restoring their first file
        HashSet<String> mClearedPackages;

        AutoPtr<RestoreInstallObserver> mInstallObserver ;
        AutoPtr<RestoreDeleteObserver> mDeleteObserver ;
        CBackupManagerService* mHost;
    };

    enum RestoreState {
        INITIAL,
        DOWNLOAD_DATA,
        PM_METADATA,
        RUNNING_QUEUE,
        FINAL,
    };

    class PerformRestoreTask : public BackupRestoreTask
    {
    private:

        class RestoreRequest {
        public:
            RestoreRequest(
                /* [in] */ IPackageInfo* _app,
                /* [in] */ Int32 _version);

        public:
            AutoPtr<IPackageInfo> mApp;
            Int32 mStoredAppVersion;
        };

    public:

        PerformRestoreTask(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ IIRestoreObserver* observer,
            /* [in] */ Int64 restoreSetToken,
            /* [in] */ IPackageInfo* targetPackage,
            /* [in] */ Int32 pmToken,
            /* [in] */ Boolean needFullBackup,
            /* [in] */ ArrayOf<String>* filterSet,
            /* [in] */ CBackupManagerService* host);

        virtual CARAPI_(void) Execute();

        // Initialize and set up for the PM metadata restore, which comes first
        CARAPI_(void) BeginRestore();

        CARAPI_(void) DownloadRestoreData();

        CARAPI_(void) RestorePmMetadata();

        CARAPI_(void) RestoreNextAgent();

        CARAPI_(void) FinalizeRestore();

        // Call asynchronously into the app, passing it the restore data.  The next step
        // after this is always a callback, either operationComplete() or handleTimeout().
        CARAPI_(void) InitiateOneRestore(
            /* [in] */ IPackageInfo* app,
            /* [in] */ Int32 appVersionCode,
            /* [in] */ IIBackupAgent* agent,
            /* [in] */ Boolean needFullBackup);

        CARAPI_(void) AgentErrorCleanup();

        CARAPI_(void) AgentCleanup();

        // A call to agent.doRestore() has been positively acknowledged as complete
        virtual CARAPI_(void) OperationComplete();

        // A call to agent.doRestore() has timed out
        virtual CARAPI_(void) HandleTimeout();

        CARAPI_(void) ExecuteNextState(
            /* [in] */ RestoreState nextState);

    private:
        typedef List<AutoPtr<IPackageInfo> > PackageInfoList;
        typedef List<AutoPtr<IPackageInfo> >::Iterator PackageInfoListIter;
        AutoPtr<IIBackupTransport> mTransport;
        AutoPtr<IIRestoreObserver> mObserver;
        Int64 mToken;
        AutoPtr<IPackageInfo> mTargetPackage;
        AutoPtr<IFile> mStateDir;
        Int32 mPmToken;
        Boolean mNeedFullBackup;
        AutoPtr<HashSet<String> > mFilterSet;
        Int64 mStartRealtime;
        AutoPtr<PackageManagerBackupAgent> mPmAgent;
        AutoPtr<PackageInfoList> mAgentPackages;
        AutoPtr<PackageInfoList> mRestorePackages;
        RestoreState mCurrentState;
        Int32 mCount;
        Boolean mFinished;
        Int32 mStatus;
        AutoPtr<IFile> mBackupDataName;
        AutoPtr<IFile> mNewStateName;
        AutoPtr<IFile> mSavedStateName;
        AutoPtr<IParcelFileDescriptor> mBackupData;
        AutoPtr<IParcelFileDescriptor> mNewState;
        AutoPtr<IPackageInfo> mCurrentPackage;
        CBackupManagerService* mHost;
    };

    class PerformClearTask
        : public Runnable
    {
    public:

        PerformClearTask(
            /* [in] */ IIBackupTransport* transport,
            /* [in] */ IPackageInfo* packageInfo,
            /* [in] */ CBackupManagerService* host);

        CARAPI Run();

    private:
        AutoPtr<IIBackupTransport> mTransport;
        AutoPtr<IPackageInfo> mPackageInfo;
        CBackupManagerService*  mHost;
    };

    class PerformInitializeTask
        : public Runnable
    {
    public:

        PerformInitializeTask(
            /* [in] */ HashSet<String>* transportNames,
            /* [in] */ CBackupManagerService* host);

        CARAPI Run();

    private:
        AutoPtr<HashSet<String> > mQueue;
        CBackupManagerService*  mHost;
    };

    // ----- Track connection to GoogleBackupTransport service -----
    class GroupServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        GroupServiceConnection(
            /* [in] */ CBackupManagerService* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        CBackupManagerService*  mHost;
    };

    class DataChangedRunable
        : public Runnable
    {
    public:
        DataChangedRunable(
            /* [in] */ const String& name,
            /* [in] */ HashSet<String>* targets,
            /* [in] */ CBackupManagerService* host);

        CARAPI Run();

    private:
        String      mPackageName;
        AutoPtr< HashSet<String> > mTargets;
        CBackupManagerService*  mHost;
    };

    class TrackPackageInstallAndRemoval
        : public BroadcastReceiver
    {
    public:
        TrackPackageInstallAndRemoval(
            /* [in] */ CBackupManagerService* host);

    protected:
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CBackupManagerService*  mHost;
    };

public:
    CBackupManagerService();

    ~CBackupManagerService();

    // ----- Back up a set of applications via a worker thread -----

    CARAPI constructor(
        /* [in] */ IContext* context);


    // ----- IBackupManager binder interface -----
    CARAPI DataChanged(
        /* [in] */ const String& packageName);

    // Clear the given package's backup data from the current transport
    CARAPI ClearBackupData(
        /* [in] */ const String& packageName);

    // Callback: a requested backup agent has been instantiated.  This should only
    // be called from the Activity Manager.
    CARAPI AgentConnected(
        /* [in] */ const String& packageName,
        /* [in] */ IBinder* agentBinder);

    // Callback: a backup agent has failed to come up, or has unexpectedly quit.
    // If the agent failed to come up in the first place, the agentBinder argument
    // will be null.  This should only be called from the Activity Manager.
    CARAPI AgentDisconnected(
        /* [in] */ const String& packageName);

    // An application being installed will need a restore pass, then the Package Manager
    // will need to be told when the restore is finished.
    CARAPI RestoreAtInstall(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 token);

    // Enable/disable the backup service
    CARAPI SetBackupEnabled(
        /* [in] */ Boolean enable);

    // Enable/disable automatic restore of app data at install time
    CARAPI SetAutoRestore(
        /* [in] */ Boolean doAutoRestore);


    // Mark the backup service as having been provisioned
    CARAPI SetBackupProvisioned(
        /* [in] */ Boolean available);

    // Report whether the backup mechanism is currently enabled
    CARAPI IsBackupEnabled(
        /* [out] */ Boolean* enabled);


    CARAPI SetBackupPassword(
        /* [in] */ const String& currentPw,
        /* [in] */ const String& newPw,
        /* [out] */ Boolean* result);

    CARAPI HasBackupPassword(
        /* [out] */ Boolean* result);


    // Run a backup pass immediately for any applications that have declared
    // that they have pending updates.
    CARAPI BackupNow();

    // Run a *full* backup pass for the given package, writing the resulting data stream
    // to the supplied file descriptor.  This method is synchronous and does not return
    // to the caller until the backup has been completed.
    CARAPI FullBackup(
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Boolean includeApks,
        /* [in] */ Boolean includeShared,
        /* [in] */ Boolean allApps,
        /* [in] */ Boolean allIncludeSystem,
        /* [in] */ ArrayOf<String>* pkgList);

    CARAPI FullRestore(
        /* [in] */ IParcelFileDescriptor* fd);

    // Confirm that the previously-requested full backup/restore operation can proceed.  This
    // is used to require a user-facing disclosure about the operation.
    CARAPI AcknowledgeFullBackupOrRestore(
        /* [in] */ Int32 token,
        /* [in] */ Boolean allow,
        /* [in] */ const String& curPassword,
        /* [in] */ const String& encPpassword,
        /* [in] */ IIFullBackupRestoreObserver* observer);

    // Report the name of the currently active transport
    CARAPI GetCurrentTransport(
        /* [out] */ String* transport);

    // Report all known, available backup transports
    CARAPI ListAllTransports(
        /*[out, callee]*/ ArrayOf<String>** transports);

    // Select which transport to use for the next backup operation.  If the given
    // name is not one of the available transports, no action is taken and the method
    // returns null.
    CARAPI SelectBackupTransport(
        /* [in] */ const String& transport,
        /* [out] */ String* port);

    // Supply the configuration Intent for the given transport.  If the name is not one
    // of the available transports, or if the transport does not supply any configuration
    // UI, the method returns null.
    CARAPI GetConfigurationIntent(
        /* [in] */ const String& transportName,
        /* [out] */ IIntent** result);

    // Supply the configuration summary string for the given transport.  If the name is
    // not one of the available transports, or if the transport does not supply any
    // summary / destination string, the method can return null.
    //
    // This string is used VERBATIM as the summary text of the relevant Settings item!
    CARAPI GetDestinationString(
        /* [in] */ const String& transportName,
        /* [out] */ String* result);


    // Hand off a restore session
    CARAPI BeginRestoreSession(
        /* [in] */ const String& packageName,
        /* [in] */ const String& transport,
        /* [out] */ IIRestoreSession** session);

    // Note that a currently-active backup agent has notified us that it has
    // completed the given outstanding asynchronous backup/restore operation.
    CARAPI OpComplete(
        /* [in] */ Int32 token);

    CARAPI ToString(
        /* [out] */ String* str);


    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String> args);

private:
    // Utility: build a new random integer token
    CARAPI_(Int32) GenerateToken();

    // ----- Debug-only backup operation trace -----
    CARAPI_(void) AddBackupTrace(
        /* [in] */ const String& s);

    CARAPI_(void) ClearBackupTrace();

    CARAPI_(void) InitPackageTracking();

    CARAPI_(void) ParseLeftoverJournals();

    CARAPI_(AutoPtr<ISecretKey>) BuildPasswordKey(
        /* [in] */ const String& pw,
        /* [in] */ ArrayOf<Byte>* salt,
        /* [in] */ Int32 rounds);

    CARAPI_(AutoPtr<ISecretKey>) BuildCharArrayKey(
        /* [in] */ ArrayOf<Char32>* pwArray,
        /* [in] */ ArrayOf<Byte>* salt,
        /* [in] */ Int32 rounds);

    CARAPI_(String) BuildPasswordHash(
        /* [in] */ const String& pw,
        /* [in] */ ArrayOf<Byte>* salt,
        /* [in] */ Int32 rounds);

    CARAPI_(String) ByteArrayToHex(
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI HexToByteArray(
        /* [in] */ const String& digits,
        /* [out] */ ArrayOf<Byte>** out);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) MakeKeyChecksum(
        /* [in] */ ArrayOf<Byte>* pwBytes,
        /* [in] */ ArrayOf<Byte>* salt,
        /* [in] */ Int32 rounds);

    // Backup password management
    CARAPI_(Boolean) PasswordMatchesSaved(
        /* [in] */ const String& candidatePw,
        /* [in] */ Int32 rounds);

    // Used for generating random salts or passwords
    CARAPI_(AutoPtr<ArrayOf<Byte> >) RandomBytes(
        /* [in] */ Int32 bits);

    // Maintain persistent state around whether need to do an initialize operation.
    // Must be called with the queue lock held.
    CARAPI_(void) RecordInitPendingLocked(
        /* [in] */ Boolean isPending,
        /* [in] */ const String& transportName);

    // Reset all of our bookkeeping, in response to having been told that
    // the backend data has been wiped [due to idle expiry, for example],
    // so we must re-upload all saved settings.
    CARAPI_(void) ResetBackupState(
        /* [in] */ IFile* stateFileDir);

    // Add a transport to our set of available backends.  If 'transport' is null, this
    // is an unregistration, and the transport's entry is removed from our bookkeeping.
    CARAPI_(void) RegisterTransport(
        /* [in] */ const String& name,
        /* [in] */ IIBackupTransport* transport);

    // Add the backup agents in the given packages to our set of known backup participants.
    // If 'packageNames' is null, adds all backup agents in the whole system.
    CARAPI_(void) AddPackageParticipantsLocked(
        /* [in] */ const ArrayOf<String>* packageNames);

    CARAPI_(void) AddPackageParticipantsLockedInner(
        /* [in] */ const String& packageName,
        /* [in] */ List<AutoPtr<IPackageInfo> >* targetPkgs);

    // Remove the given packages' entries from our known active set.
    CARAPI_(void) RemovePackageParticipantsLocked(
        /* [in] */ ArrayOf<String>* packageNames,
        /* [in] */ Int32 oldUid);

    CARAPI_(void) RemovePackageFromSetLocked(
        /* [in] */ HashSet<String>& set,
        /* [in] */ const String packageName);

    // Returns the set of all applications that define an android:backupAgent attribute
    CARAPI_(AutoPtr< List<AutoPtr<IPackageInfo> > >) AllAgentPackages();

    // Called from the backup task: record that the given app has been successfully
    // backed up at least once
    CARAPI_(void) LogBackupComplete(
        /* [in] */ const String& packageName);

    // Remove our awareness of having ever backed up the given package
    CARAPI RemoveEverBackedUp(
        /* [in] */ const String& packageName);

    // Persistently record the current and ancestral backup tokens as well
    // as the set of packages with data [supposedly] available in the
    // ancestral dataset.
    CARAPI_(void) WriteRestoreTokens();

    // Return the given transport
    CARAPI_(AutoPtr<IIBackupTransport>) GetTransport(
        /* [in] */ const String& transportName);

    // fire off a backup agent, blocking until it attaches or times out
    CARAPI_(AutoPtr<IIBackupAgent>) BindToAgentSynchronous(
        /* [in] */ IApplicationInfo* app,
        /* [in] */ Int32 mode);

    // clear an application's data, blocking until the operation completes or times out
    CARAPI_(void) ClearApplicationDataSynchronous(
        /* [in] */ const String& packageName);

    // Get the restore-set token for the best-available restore set for this package:
    // the active set if possible, else the ancestral one.  Returns zero if none available.
    CARAPI_(Int64) GetAvailableRestoreToken(
        /* [in] */ const String& packageName);

    CARAPI_(void) PrepareOperationTimeout(
        /* [in] */ Int32 token,
        /* [in] */ Int64 interval,
        /* [in] */ BackupRestoreTask* callback);

    // -----
    // Utility methods used by the asynchronous-with-timeout backup/restore operations
    CARAPI_(Boolean) WaitUntilOperationComplete(
        /* [in] */ Int32 token);

    CARAPI_(void) HandleTimeout(
        /* [in] */ Int32 token,
        /* [in] */ IObject* obj);

    // ----- Restore handling -----

    CARAPI_(Boolean) SignaturesMatch(
        /* [in] */ ArrayOf<ISignature*>* storedSigs,
        /* [in] */ IPackageInfo* target);

    CARAPI_(void) DataChangedImpl(
        /* [in] */ const String& packageName);

    CARAPI_(void) DataChangedImpl(
        /* [in] */ const String& packageName,
        /* [in] */ HashSet<String>* targets);

    // Note: packageName is currently unused, but may be in the future
    CARAPI_(AutoPtr< HashSet<String> >) DataChangedTargets(
        /* [in] */ const String& packageName);

    CARAPI_(void) WriteToJournalLocked(
        /* [in] */ const String& str);

    CARAPI_(Boolean) DeviceIsProvisioned();

    CARAPI_(Boolean) StartConfirmationUi(
        /* [in] */ Int32 token,
        /* [in] */ const String& action);

    CARAPI_(void) StartConfirmationTimeout(
        /* [in] */ Int32 token,
        /* [in] */ FullParams* params);

    CARAPI_(void) WaitForCompletion(
        /* [in] */ FullParams* params);

    CARAPI_(void) SignalFullBackupRestoreCompletion(
        /* [in] */ FullParams* params);

    CARAPI_(void) StartBackupAlarmsLocked(
        /* [in] */ Int64 delayBeforeFirstBackup);

    CARAPI_(void) ClearRestoreSession(
        /* [in] */ IIRestoreSession* currentSession);

    CARAPI_(void) DumpInternal(
        /* [in] */ IPrintWriter* pw);

    //Callback function.
    CARAPI_(void) HandleRunBackup();

    CARAPI_(void) HandleRunInitilize();

    CARAPI_(void) HandleGetRestoreSets(
        /* [in] */ RestoreGetSetsParams* params);

    CARAPI_(void) HandleRestore(
        /* [in] */ RestoreParams* params);

    CARAPI_(void) HandleFullConfirmationTimeout(
        /* [in] */ Int32 token);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) StringToByteArray(
        /* [in] */ const String& str);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Boolean MORE_DEBUG;

    // Name and current contents version of the full-backup manifest file
    static const String BACKUP_MANIFEST_FILENAME;
    static const Int32 BACKUP_MANIFEST_VERSION;
    static const String BACKUP_FILE_HEADER_MAGIC;
    static const Int32 BACKUP_FILE_VERSION;
    static const Boolean COMPRESS_FULL_BACKUPS; // should be true in production

    static const String SHARED_BACKUP_AGENT_PACKAGE;

    // How often we perform a backup pass.  Privileged external callers can
    // trigger an immediate pass.
    static const Int64 BACKUP_INTERVAL; //AlarmManager.INTERVAL_HOUR;

    // Random variation in backup scheduling time to avoid server load spikes
    static const Int32 FUZZ_MILLIS;

    // The amount of time between the initial provisioning of the device and
    // the first backup pass.
    static const Int64 FIRST_BACKUP_INTERVAL;//AlarmManager.INTERVAL_HOUR;

    static const String RUN_BACKUP_ACTION;
    static const String RUN_INITIALIZE_ACTION;
    static const String RUN_CLEAR_ACTION;
    static const Int32 MSG_RUN_BACKUP;
    static const Int32 MSG_RUN_FULL_BACKUP;
    static const Int32 MSG_RUN_RESTORE;
    static const Int32 MSG_RUN_CLEAR;
    static const Int32 MSG_RUN_INITIALIZE;
    static const Int32 MSG_RUN_GET_RESTORE_SETS;
    static const Int32 MSG_TIMEOUT;
    static const Int32 MSG_RESTORE_TIMEOUT;
    static const Int32 MSG_FULL_CONFIRMATION_TIMEOUT;
    static const Int32 MSG_RUN_FULL_RESTORE;

    // backup task state machine tick
    static const Int32 MSG_BACKUP_RESTORE_STEP;
    static const Int32 MSG_OP_COMPLETE;


    // Timeout interval for deciding that a bind or clear-data has taken too Int64
    static const Int64 TIMEOUT_INTERVAL;

    // Timeout intervals for agent backup & restore operations
    static const Int64 TIMEOUT_BACKUP_INTERVAL;
    static const Int64 TIMEOUT_FULL_BACKUP_INTERVAL;
    static const Int64 TIMEOUT_SHARED_BACKUP_INTERVAL;
    static const Int64 TIMEOUT_RESTORE_INTERVAL;

    // User confirmation timeout for a full backup/restore operation.  It's this long in
    // order to give them time to enter the backup password.
    static const Int64 TIMEOUT_FULL_CONFIRMATION;

    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPackageManager;
    AutoPtr<IIPackageManager> mPackageManagerBinder;
    AutoPtr<IIActivityManager> mActivityManager;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IIMountService> mMountService;
    AutoPtr<IIBackupManager> mBackupManagerBinder;

    Boolean mEnabled;   // access to this is synchronized on 'this'
    Boolean mProvisioned;
    Boolean mAutoRestore;
    AutoPtr<IPowerManagerWakeLock> mWakelock;

    AutoPtr<IHandlerThread> mHandlerThread;
    AutoPtr<BackupHandler> mBackupHandler;


    AutoPtr<IPendingIntent> mRunBackupIntent;
    AutoPtr<IPendingIntent> mRunInitIntent;
    AutoPtr<IBroadcastReceiver> mRunBackupReceiver;
    AutoPtr<IBroadcastReceiver> mRunInitReceiver;
    // map UIDs to the set of participating packages under that UID
    // final SparseArray<HashSet<String>> mBackupParticipants
    //         = new SparseArray<HashSet<String>>();
    HashMap<Int32, AutoPtr<HashSet<String> > >  mBackupParticipants;
    Object mBackupParticipantsLock;

    // Backups that we haven't started yet.
    typedef HashMap<String, AutoPtr<BackupRequest> > BackupRequestHashMap;
    typedef HashMap<String, AutoPtr<BackupRequest> >::Iterator BackupRequestHashMapIter;
    BackupRequestHashMap mPendingBackups;

    // Pseudoname that we use for the Package Manager metadata "package"
    static const String PACKAGE_MANAGER_SENTINEL;

    // locking around the pending-backup management
    Object mQueueLock;

    // The thread performing the sequence of queued backups binds to each app's agent
    // in succession.  Bind notifications are asynchronously delivered through the
    // Activity Manager; use this lock object to signal when a requested binding has
    // completed.
    Object mAgentConnectLock;
    AutoPtr<IIBackupAgent> mConnectedAgent;
    Boolean mBackupRunning;
    Boolean mConnecting;
    Int64 mLastBackupPass;
    Int64 mNextBackupPass;

    // For debugging, we maintain a progress trace of operations during backup
    static const Boolean DEBUG_BACKUP_TRACE;
    List<String> mBackupTrace;
    Object mBackupTraceLock;

    // A similar synchronization mechanism around clearing apps' data for restore
    Object mClearDataLock;
    Boolean mClearingData;

    // Transport bookkeeping
    HashMap<String, AutoPtr<IIBackupTransport> > mTransports;
    Object mTransportsLock;

    String mCurrentTransport;
    AutoPtr<IIBackupTransport> mLocalTransport;
    AutoPtr<IIBackupTransport> mGoogleTransport;
    AutoPtr<IIRestoreSession> mActiveRestoreSession;
    // Watch the device provisioning operation during setup
    AutoPtr<ContentObserver> mProvisionedObserver;

    // Bookkeeping of in-flight operations for timeout etc. purposes.  The operation
    // token is the index of the entry in the pending-operations list.
    static const Int32 OP_PENDING;
    static const Int32 OP_ACKNOWLEDGED;
    static const Int32 OP_TIMEOUT;

    // SparseInt32Array* mCurrentOperations;
    //SparseArray<Operation> mCurrentOperations; // = new SparseArray<Operation>();
    HashMap<Int32, AutoPtr<Operation> > mCurrentOperations; // = new SparseArray<Operation>();
    Object mCurrentOpLock;

    AutoPtr<IRandom> mTokenGenerator;
    Object mTokenGeneratorLock;
    //const SparseArray<FullParams> mFullConfirmations; // = new SparseArray<FullParams>();
    HashMap<Int32, AutoPtr<FullParams> > mFullConfirmations;
    Object mFullConfirmationsLock;

    // Where we keep our journal files and other bookkeeping
    AutoPtr<IFile> mBaseStateDir;
    AutoPtr<IFile> mDataDir;
    AutoPtr<IFile> mJournalDir;
    AutoPtr<IFile> mJournal;

    // Backup password, if any, and the file where it's saved.  What is stored is not the
    // password text itself; it's the result of a PBKDF2 hash with a randomly chosen (but
    // persisted) salt.  Validation is performed by running the challenge text through the
    // same PBKDF2 cycle with the persisted salt; if the resulting derived key string matches
    // the saved hash string, then the challenge text matches the originally supplied
    // password text.
    AutoPtr<ISecureRandom> mRng;
    String mPasswordHash;
    AutoPtr<IFile> mPasswordHashFile;
    AutoPtr<ArrayOf<Byte> > mPasswordSalt;

    // Configuration of PBKDF2 that we use for generating pw hashes and intermediate keys
    static const Int32 PBKDF2_HASH_ROUNDS;
    static const Int32 PBKDF2_KEY_SIZE;     // bits
    static const Int32 PBKDF2_SALT_SIZE;    // bits
    static const String ENCRYPTION_ALGORITHM_NAME;

    // Keep a log of all the apps we've ever backed up, and what the
    // dataset tokens are for both the current backup dataset and
    // the ancestral dataset.
    AutoPtr<IFile> mEverStored;
    HashSet<String> mEverStoredApps;
    Object mEverStoredAppsLock;

    static const Int32 CURRENT_ANCESTRAL_RECORD_VERSION;  // increment when the schema changes
    AutoPtr<IFile> mTokenFile;
    AutoPtr<HashSet<String> > mAncestralPackages;
    Int64 mAncestralToken;
    Int64 mCurrentToken;

    // Persistently track the need to do a full init
    static String INIT_SENTINEL_FILE_NAME;
    HashSet<String> mPendingInits;  // transport names

    // ----- Track installation/removal of packages -----
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    // ----- Track connection to GoogleBackupTransport service -----
    AutoPtr<IServiceConnection> mGoogleConnection;

    Object mBackupServiceLock;

private:
    friend class RunBackupReceiver;
    friend class CActiveRestoreSession;
};

}// namespace Server {
}// namespace Droid {
}// namespace Elastos {

#endif  //__ELASTOS_DROID_SERVER_CBACKUPMANAGERSERVICE_H__
