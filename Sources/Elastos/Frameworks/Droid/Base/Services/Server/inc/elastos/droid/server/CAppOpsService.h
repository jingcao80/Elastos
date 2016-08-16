#ifndef __ELASTOS_DROID_SERVER_CAPPOPS_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CAPPOPS_SERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CAppOpsService.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/droid/server/PermissionDialogReqQueue.h>
#include <elastos/droid/utility/SparseArray.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/HashMap.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.External.h>
#include <binder/IAppOpsCallback.h>
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"

using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::SparseArray;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Internal::App::IIAppOpsCallback;

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;


namespace Elastos {
namespace Droid {
namespace Server {

/**
 * This service is responsible for packing preloaded bitmaps into a single
 * atlas texture. The resulting texture can be shared across processes to
 * reduce overall memory usage.
 *
 * @hide
 */
CarClass(CAppOpsService)
    , public Object
    , public IIAppOpsService
    , public IBinder
{
public:
    class ClientState
        : public Object
        , public IProxyDeathRecipient
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IBinder* appToken,
            /* [in] */ IIAppOpsService* host);

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

        //@Override
        CARAPI ProxyDied();

    private:
        friend class CAppOpsService;
        AutoPtr<IBinder> mAppToken;
        Int32 mPid;
        AutoPtr<IArrayList> mStartedOps; //ArrayList<Op>
        CAppOpsService* mHost;
    };

private:
    class NativeAppOpsService : public android::BBinder
    {
    private:
        class Token : public android::BBinder
        {
        public:
            Token(
                /* [in] */ Elastos::Droid::Os::IBinder* token)
                : mToken(token)
            {}

        public:
            AutoPtr<Elastos::Droid::Os::IBinder> mToken;
        };

        class AppOpsCallback
            : public Object
            , public IIAppOpsCallback
        {
        public:
            AppOpsCallback(
                /* [in] */ android::IAppOpsCallback* callback)
                : mCallback(callback)
            {}

            CAR_INTERFACE_DECL()

            CARAPI OpChanged(
                /* [in] */ Int32 op,
                /* [in] */ const String& packageName);

        private:
            android::sp<android::IAppOpsCallback> mCallback;
        };

        class ClientToken
            : public Object
            , public Elastos::Droid::Os::IBinder
        {
        public:
            ClientToken(
                /* [in] */ android::IBinder* clientToken)
                : mClientToken(clientToken)
            {}

            CAR_INTERFACE_DECL()

            CARAPI LinkToDeath(
                /* [in] */ IProxyDeathRecipient* recipient,
                /* [in] */ NativeAppOpsService* host);

            TO_STRING_IMPL("NativeAppOpsService::ClientToken")

        private:
            android::sp<android::IBinder> mClientToken;
        };

        class ClientTokenDeathRecipient : public android::IBinder::DeathRecipient
        {
        public:
            ClientTokenDeathRecipient(
                /* [in] */ IProxyDeathRecipient* recipient,
                /* [in] */ NativeAppOpsService* host)
                : mRecipient(recipient)
                , mHost(host)
            {}

            void binderDied(
                /* [in] */ const android::wp<android::IBinder>& who);

        private:
            AutoPtr<IProxyDeathRecipient> mRecipient;
            NativeAppOpsService* mHost;
        };

    public:
        NativeAppOpsService(
            /* [in] */ CAppOpsService* host)
            : mHost(host)
        {}

        android::status_t onTransact(
            /* [in] */ uint32_t code,
            /* [in] */ const android::Parcel& data,
            /* [in] */ android::Parcel* reply,
            /* [in] */ uint32_t flags = 0);

    private:
        CAppOpsService* mHost;
        Object mTokenMapLock;
        HashMap<AutoPtr<Elastos::Droid::Os::IBinder>, android::sp<android::IBinder> > mTokenMap;
        HashMap<Int64, AutoPtr<ClientToken> > mClientTokenMap;
        HashMap<Int64, AutoPtr<AppOpsCallback> > mCallbackMap;
    };

    class WriteStateTask
        : public AsyncTask
    {
    public:
        WriteStateTask(
            /* [in] */ CAppOpsService* host);

        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);
    private:
        CAppOpsService* mHost;
    };

    class WriteRunner
        : public Runnable
    {
    public:
        WriteRunner(
            /* [in] */ CAppOpsService* host);

        CARAPI Run();

    private:
        CAppOpsService* mHost;
    };

    class ChangedRunner
        : public Runnable
    {
    public:
        ChangedRunner(
            /* [in] */ CAppOpsService* host);

        CARAPI Run();

    private:
        CAppOpsService* mHost;
    };

    class Op
        : public Object
    {
    public:
        Op(
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [in] */ Int32 op,
            /* [in] */ Int32 mode);

        CARAPI ToString(
            /* [out] */ String* str);
    public:
        Int32 mUid;
        String mPackageName;
        Int32 mOp;
        Int32 mMode;
        Int32 mDuration;
        Int64 mTime;
        Int64 mRejectTime;
        Int32 mNesting;
        Int32 mNoteOpCount;
        Int32 mStartOpCount;
        AutoPtr<PermissionDialogReqQueue> mDialogReqQueue;
        AutoPtr<IArrayList> mClientTokens;

        Int32 mAllowedCount;
        Int32 mIgnoredCount;
    };

    class Ops
        : public SparseArray
    {
    public:
        Ops();

        CARAPI constructor(
            /* [in] */ const String& _packageName,
            /* [in] */ Int32 _uid,
            /* [in] */ Boolean _isPrivileged);

    public:
        String mPackageName;
        Int32 mUid;
        Boolean mIsPrivileged;
    };

    class Callback
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CAppOpsService::Callback")

        Callback(
            /* [in] */ IIAppOpsCallback* callback,
            /* [in] */ CAppOpsService* host);

        CARAPI UnlinkToDeath();

        //@Override
        CARAPI ProxyDied();

    private:
        friend class CAppOpsService;

        AutoPtr<IIAppOpsCallback> mCallback;
        CAppOpsService* mHost;
    };

    class Restriction
        : public Object
    {
    public:
        Restriction();

        Int32 mMode;
        AutoPtr<HashSet<String> > mExceptionPackages;// = NO_EXCEPTIONS;//ArraySet<String>

    private:
        static const AutoPtr<HashSet<String> > NO_EXCEPTIONS;// = new ArraySet<String>();
    };

public:
    class AskRunnable
        : public Runnable
    {
    public:
        AskRunnable(
            /* [in] */ Int32 code,
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [in] */ Op* op,
            /* [in] */ PermissionDialogReqQueue::PermissionDialogReq* request,
            /* [in] */ CAppOpsService* host);

        CARAPI Run();

    public:
        Int32 mCode;
        Int32 mUid;
        String mPackageName;
        AutoPtr<Op> mOp;
        AutoPtr<PermissionDialogReqQueue::PermissionDialogReq> mRequest;
        CAppOpsService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAppOpsService();

    CARAPI constructor(
        /* [in] */ IFile* storagePath,
        /* [in] */ IHandler* handler);

    CARAPI Publish(
        /* [in] */ IContext* context);

    CARAPI SystemReady();

    CARAPI PackageRemoved(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI UidRemoved(
        /* [in] */ Int32 uid);

    CARAPI Shutdown();

    //@Override
    CARAPI GetPackagesForOps(
        /* [in] */ ArrayOf<Int32>* ops,
        /* [out] */ IList** pkgOps); // List<AppOpsManager.PackageOps>

    //@Override
    CARAPI GetOpsForPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ ArrayOf<Int32>* ops,
        /* [out] */ IList** pkgOps); // List<AppOpsManager.PackageOps>

    //@Override
    CARAPI SetMode(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 mode);

    //@Override
    CARAPI ResetAllModes();

    //@Override
    CARAPI StartWatchingMode(
        /* [in] */ Int32 op,
        /* [in] */ const String& packageName,
        /* [in] */ IIAppOpsCallback* callback);

    //@Override
    CARAPI StopWatchingMode(
        /* [in] */ IIAppOpsCallback* callback);

    //@Override
    CARAPI GetToken(
        /* [in] */ IBinder* clientToken,
        /* [out] */ IBinder** token);

    //@Override
    CARAPI CheckOperation(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* op);

    //@Override
    CARAPI CheckAudioOperation(
        /* [in] */ Int32 code,
        /* [in] */ Int32 usage,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* op);

    //@Override
    CARAPI SetAudioRestriction(
        /* [in] */ Int32 code,
        /* [in] */ Int32 usage,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 mode,
        /* [in] */ ArrayOf<String>* exceptionPackages);

    //@Override
    CARAPI CheckPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* mode);

    //@Override
    CARAPI NoteOperation(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* mode);

    //@Override
    CARAPI StartOperation(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* mode);

    //@Override
    CARAPI FinishOperation(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI_(void) FinishOperationLocked(
        /* [in] */ Op* op);

    CARAPI_(void) ReadState();

    CARAPI ReadPackage(
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadUid(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& pkgName);

    CARAPI WriteState();

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    //@Override
    CARAPI SetUserRestrictions(
        /* [in] */ IBundle* restrictions,
        /* [in] */ Int32 userHandle);

    //@Override
    CARAPI RemoveUser(
        /* [in] */ Int32 userHandle);

    CARAPI IsControlAllowed(
        /* [in] */ Int32 code,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI GetPrivacyGuardSettingForPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI SetPrivacyGuardSettingForPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean state);

    CARAPI ResetCounters();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI NotifyOperation(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 mode,
        /* [in] */ Boolean remember);

private:
    CARAPI_(AutoPtr<IArrayList>) CollectOps( //ArrayList><AppOpsManager.OpEntry>
        /* [in] */ Ops* pkgOps,
        /* [in] */ ArrayOf<Int32>* ops);

    CARAPI_(void) PruneOp(
        /* [in] */ Op* op,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    static CARAPI_(AutoPtr<IHashMap>) AddCallbacks(
        /* [in] */ IHashMap* callbacks, //HashMap<Callback, ArrayList<Pair<String, Integer>>>
        /* [in] */ const String& packageName,
        /* [in] */ Int32 op,
        /* [in] */ IArrayList* cbs);  //ArrayList<Callback>

    CARAPI_(Int32) CheckRestrictionLocked(
        /* [in] */ Int32 code,
        /* [in] */ Int32 usage,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI VerifyIncomingUid(
        /* [in] */ Int32 uid);

    CARAPI VerifyIncomingOp(
        /* [in] */ Int32 op);

    CARAPI_(AutoPtr<Ops>) GetOpsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean edit);

    CARAPI_(AutoPtr<Ops>) GetOpsRawLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean edit);

    CARAPI_(void) ScheduleWriteLocked();

    CARAPI_(void) ScheduleWriteNowLocked();

    CARAPI_(AutoPtr<Op>) GetOpLocked(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean edit);

    CARAPI_(AutoPtr<Op>) GetOpLocked(
        /* [in] */ Ops* ops,
        /* [in] */ Int32 code,
        /* [in] */ Boolean edit);

    CARAPI_(Boolean) IsOpRestricted(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 code,
        /* [in] */ const String& packageName);

    CARAPI CheckSystemUid(
        /* [in] */ const String& function);

    CARAPI_(Boolean) IsStrict(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 code,
        /* [in] */ const String& packageName);

    CARAPI_(Int32) GetDefaultMode(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    AutoPtr<PermissionDialogReqQueue::PermissionDialogReq> AskOperationLocked(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Op* op);

    CARAPI_(void) PrintOperationLocked(
        /* [in] */ Op* op,
        /* [in] */ Int32 mode,
        /* [in] */ const String& operation);

    CARAPI_(void) RecordOperationLocked(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 mode);

    CARAPI_(void) BroadcastOpIfNeeded(
        /* [in] */ Int32 op);

    CARAPI_(void) ReadPolicy();

private:
    static const String TAG;
    static const Boolean DEBUG;

    // Write at most every 30 minutes.
    static const Int64 WRITE_DELAY;

    // Location of policy file.
    static const String DEFAULT_POLICY_FILE;

    static AutoPtr<ArrayOf<Int32> > PRIVACY_GUARD_OP_STATES;

    AutoPtr<IContext> mContext;
    AutoPtr<IAtomicFile> mFile;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ILooper> mLooper;
    Boolean mStrictEnable;
    AutoPtr<IAppOpsPolicy> mPolicy;

    Boolean mWriteScheduled;

    AutoPtr<IRunnable> mWriteRunner;

    AutoPtr<IRunnable> mSuSessionChangedRunner;

    AutoPtr<ISparseArray> mUidOps; //SparseArray<HashMap<String, Ops>>

    AutoPtr<ISparseArray> mOpRestrictions;// = new SparseArray<Boolean[]>();

    AutoPtr<ISparseArray> mOpModeWatchers;// = new SparseArray<ArrayList<Callback>>();
    AutoPtr<IArrayMap> mPackageModeWatchers;//= new ArrayMap<String, ArrayList<Callback>>();
    AutoPtr<IArrayMap> mModeWatchers;// = new ArrayMap<IBinder, Callback>();
    AutoPtr<ISparseArray> mAudioRestrictions;// = new SparseArray<SparseArray<Restriction>>();
    AutoPtr<ISparseArray> mLoadPrivLaterPkgs;

    AutoPtr<IArrayMap> mClients;// = new ArrayMap<IBinder, ClientState>();

    android::sp<NativeAppOpsService> mNative;
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__

