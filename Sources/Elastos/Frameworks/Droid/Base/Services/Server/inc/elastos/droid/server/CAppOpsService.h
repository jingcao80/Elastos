#ifndef __ELASTOS_DROID_SERVER_CAPPOPS_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CAPPOPS_SERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CAppOpsService.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/droid/utility/SparseArray.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.External.h>

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
private:

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

    class Op
        : public Object
    {
    public:

        Op(
            /* [in] */ Int32 uid,
            /* [in] */ const String& packageName,
            /* [in] */ Int32 op,
            /* [in] */ Int32 mode);

    public:
        Int32 mUid;
        String mPackageName;
        Int32 mOp;
        Int32 mMode;
        Int32 mDuration;
        Int64 mTime;
        Int64 mRejectTime;
        Int32 mNesting;

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

        Callback(
            /* [in] */ IIAppOpsCallback* callback,
            /* [in] */ CAppOpsService* host);

        CARAPI UnlinkToDeath();

        //@Override
        CARAPI ProxyDied();

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        friend class CAppOpsService;

        AutoPtr<IIAppOpsCallback> mCallback;
        CAppOpsService* mHost;
    };

    class ClientState
        : public Object
        , public IProxyDeathRecipient
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        ClientState(
            /* [in] */ IBinder* appToken,
            /* [in] */ CAppOpsService* host);

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

    class Restriction
        : public Object
    {
    public:
        Restriction();

        Int32 mMode;
        AutoPtr<IArraySet> mExceptionPackages;// = NO_EXCEPTIONS;//ArraySet<String>

    private:
        static const AutoPtr<IArraySet> NO_EXCEPTIONS;// = new ArraySet<String>();
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

    void FinishOperationLocked(
        /* [in] */ Op* op);

    void ReadState();

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

private:
    AutoPtr<IArrayList> CollectOps( //ArrayList><AppOpsManager.OpEntry>
        /* [in] */ Ops* pkgOps,
        /* [in] */ ArrayOf<Int32>* ops);

    void PruneOp(
        /* [in] */ Op* op,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    static AutoPtr<IHashMap> AddCallbacks(
        /* [in] */ IHashMap* callbacks, //HashMap<Callback, ArrayList<Pair<String, Integer>>>
        /* [in] */ const String& packageName,
        /* [in] */ Int32 op,
        /* [in] */ IArrayList* cbs);  //ArrayList<Callback>

    Int32 CheckRestrictionLocked(
        /* [in] */ Int32 code,
        /* [in] */ Int32 usage,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI VerifyIncomingUid(
        /* [in] */ Int32 uid);

    CARAPI VerifyIncomingOp(
        /* [in] */ Int32 op);

    AutoPtr<Ops> GetOpsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean edit);

    AutoPtr<Ops> GetOpsRawLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean edit);

    void ScheduleWriteLocked();

    void ScheduleWriteNowLocked();

    AutoPtr<Op> GetOpLocked(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean edit);

    AutoPtr<Op> GetOpLocked(
        /* [in] */ Ops* ops,
        /* [in] */ Int32 code,
        /* [in] */ Boolean edit);

    Boolean IsOpRestricted(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 code,
        /* [in] */ const String& packageName);

    CARAPI CheckSystemUid(
        /* [in] */ const String& function);

    Boolean IsStrict(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 code,
        /* [in] */ const String& packageName);

    CARAPI_(Int32) GetDefaultMode(
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

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
    Boolean mStrictEnable;
    // AppOpsPolicy mPolicy;

    Boolean mWriteScheduled;

    AutoPtr<IRunnable> mWriteRunner;

    AutoPtr<ISparseArray> mUidOps; //SparseArray<HashMap<String, Ops>>

    AutoPtr<ISparseArray> mOpRestrictions;// = new SparseArray<Boolean[]>();

    AutoPtr<ISparseArray> mOpModeWatchers;// = new SparseArray<ArrayList<Callback>>();
    AutoPtr<IArrayMap> mPackageModeWatchers;//= new ArrayMap<String, ArrayList<Callback>>();
    AutoPtr<IArrayMap> mModeWatchers;// = new ArrayMap<IBinder, Callback>();
    AutoPtr<ISparseArray> mAudioRestrictions;// = new SparseArray<SparseArray<Restriction>>();

    AutoPtr<IArrayMap> mClients;// = new ArrayMap<IBinder, ClientState>();
};



} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__

