
#ifndef __ELASTOS_DROID_SERVER_CMOUNTSERVICE_H__
#define __ELASTOS_DROID_SERVER_CMOUNTSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_CMountService.h"
#include "NativeDaemonConnector.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/HandlerBase.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/List.h>


using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Core::IInteger32;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Res::IObbInfo;
using Elastos::Droid::Internal::App::IMediaContainerService;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::Storage::IIObbActionListener;
using Elastos::Droid::Os::Storage::IIMountShutdownObserver;
using Elastos::Droid::Os::Storage::IIMountServiceListener;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Server::Pm::CPackageManagerService;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CMountService)
{
public:
    class MountServiceHandler : public HandlerBase
    {
    public:
        MountServiceHandler(
            /* [in] */ ILooper* l,
            /* [in] */ CMountService* service);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CMountService* mHost;
    };

    class ObbActionHandler : public HandlerBase
    {
    public:
        ObbActionHandler(
            /* [in] */ ILooper* l,
            /* [in] */ CMountService* service);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CMountService* mHost;
    };

    /*
     * Internal vold volume state constants
     */
    class VolumeState
    {
    public:
        static const Int32 Init       = -1;
        static const Int32 NoMedia    = 0;
        static const Int32 Idle       = 1;
        static const Int32 Pending    = 2;
        static const Int32 Checking   = 3;
        static const Int32 Mounted    = 4;
        static const Int32 Unmounting = 5;
        static const Int32 Formatting = 6;
        static const Int32 Shared     = 7;
        static const Int32 SharedMnt  = 8;
    };

    /*
     * Internal vold response code constants
     */
    class VoldResponseCode
    {
    public:
        /*
         * 100 series - Requestion action was initiated; expect another reply
         *              before proceeding with a new command.
         */
        static const Int32 VolumeListResult               = 110;
        static const Int32 AsecListResult                 = 111;
        static const Int32 StorageUsersListResult         = 112;

        /*
         * 200 series - Requestion action has been successfully completed.
         */
        static const Int32 ShareStatusResult              = 210;
        static const Int32 AsecPathResult                 = 211;
        static const Int32 ShareEnabledResult             = 212;

        /*
         * 400 series - Command was accepted, but the requested action
         *              did not take place.
         */
        static const Int32 OpFailedNoMedia                = 401;
        static const Int32 OpFailedMediaBlank             = 402;
        static const Int32 OpFailedMediaCorrupt           = 403;
        static const Int32 OpFailedVolNotMounted          = 404;
        static const Int32 OpFailedStorageBusy            = 405;
        static const Int32 OpFailedStorageNotFound        = 406;

        /*
         * 600 series - Unsolicited broadcasts.
         */
        static const Int32 VolumeStateChange              = 605;
        static const Int32 VolumeDiskInserted             = 630;
        static const Int32 VolumeDiskRemoved              = 631;
        static const Int32 VolumeBadRemoval               = 632;
    };

    class ObbState
        : public ElRefBase
        , public IProxyDeathRecipient
    {
    public:
        ObbState(
            /* [in] */ const String& rawPath,
            /* [in] */ const String& canonicalPath,
            /* [in] */ Int32 callerUid,
            /* [in] */ IIObbActionListener* token,
            /* [in] */ Int32 nonce,
            /* [in] */ CMountService* host);

        CAR_INTERFACE_DECL()

        CARAPI_(AutoPtr<IBinder>) GetBinder();

        CARAPI ProxyDied();

        CARAPI Link();

        CARAPI Unlink();

        CARAPI_(String) ToString();

    public:
        String mRawPath;
        String mCanonicalPath;
        String mOwnerPath;
        String mVoldPath;

        Int32 mOwnerGid;

        // Token of remote Binder caller
        AutoPtr<IIObbActionListener> mToken;

        // Identifier to pass back to the token
        Int32 mNonce;

        CMountService* mHost;
    };

    class DefaultContainerConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        DefaultContainerConnection(
            /* [in] */ CMountService* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        CMountService* mHost;
    };

    class UnmountCallBack
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        UnmountCallBack(
            /* [in] */ const String& path,
            /* [in] */ Boolean force,
            /* [in] */ Boolean removeEncryption,
            /* [in] */ CMountService* host);

        virtual ~UnmountCallBack() {}

        virtual CARAPI_(void) HandleFinished();

    public:
        String mPath;
        Boolean mForce;
        Boolean mRemoveEncryption;
        Int32 mRetries;
        CMountService* mHost;
    };

    class UmsEnableCallBack : public UnmountCallBack
    {
    public:
        UmsEnableCallBack(
            /* [in] */ const String& path,
            /* [in] */ const String& method,
            /* [in] */ Boolean force,
            /* [in] */ CMountService* host);

        CARAPI_(void) HandleFinished();

    private:
        String mMethod;
    };

    class ShutdownCallBack : public UnmountCallBack
    {
    public:
        ShutdownCallBack(
            /* [in] */ const String& path,
            /* [in] */ IIMountShutdownObserver* observer,
            /* [in] */ CMountService* host);


        CARAPI_(void) HandleFinished();

    private:
        AutoPtr<IIMountShutdownObserver> mObserver;
    };

    class ObbAction
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        ObbAction(
            /* [in] */ ObbState* obbState,
            /* [in] */ CMountService* host);

        virtual ~ObbAction() {}

        CARAPI_(void) Execute(
            /* [in] */ IHandler* handler);

        virtual CARAPI HandleExecute() = 0;

        virtual CARAPI_(void) HandleError() = 0;

        virtual CARAPI_(String) ToString() = 0;

    protected:
        virtual CARAPI GetObbInfo(
            /* [out] */ IObbInfo** info);

        virtual CARAPI SendNewStatusOrIgnore(
            /* [in] */ Int32 status);

    protected:
        AutoPtr<ObbState> mObbState;
        CMountService* mHost;

    private:
        static const Int32 MAX_RETRIES = 3;
        Int32 mRetries;
    };

    class MountObbAction: public ObbAction
    {
    public:
        MountObbAction(
            /* [in] */ ObbState* obbState,
            /* [in] */ const String& key,
            /* [in] */ Int32 callingUid,
            /* [in] */ CMountService* host);

        CARAPI HandleExecute();

        CARAPI_(void) HandleError();

        CARAPI_(String) ToString();

    private:
        String mKey;
        Int32 mCallingUid;
    };

    class UnmountObbAction: public ObbAction
    {
    public:
        UnmountObbAction(
            /* [in] */ ObbState* obbState,
            /* [in] */ Boolean force,
            /* [in] */ CMountService* host);

        CARAPI HandleExecute();

        CARAPI_(void) HandleError();

        CARAPI_(String) ToString();

    private:
        Boolean mForceUnmount;
    };

private:
    class MountServiceBinderListener
        : public ElRefBase
        , public IProxyDeathRecipient
    {
    public:
        MountServiceBinderListener(
            /* [in] */ IIMountServiceListener* listener,
            /* [in] */ CMountService* host);

        CAR_INTERFACE_DECL();

        CARAPI ProxyDied();

    public:
        AutoPtr<IIMountServiceListener> mListener;
        CMountService* mHost;
    };

    class UserBroadcastReceiver : public BroadcastReceiver
    {
    public:
        UserBroadcastReceiver(
            /* [in] */ CMountService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CMountService::UserBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CMountService* mHost;
    };

    class UsbBroadcastReceiver : public BroadcastReceiver
    {
    public:
        UsbBroadcastReceiver(
            /* [in] */ CMountService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CMountService::UsbBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CMountService* mHost;
    };

    /*
    * USB mass storage disconnected while enabled
    */
    class UsbMassStorageThread
        : public Thread
    {
    public:
        UsbMassStorageThread(
            /* [in] */ const String& path,
            /* [in] */ CMountService* host);

        virtual CARAPI Run();

    private:
        String mPath;
        CMountService* mHost;
    };

    /*
     * Callback from NativeDaemonConnector
     */
    class OnDaemonConnectedThread
        : public Thread
    {
    public:

        OnDaemonConnectedThread(
            /* [in] */ const String& threadName,
            /* [in] */ CMountService* host);

        virtual CARAPI Run();

    private:
        CMountService* mHost;;
    };

    class OnEventThread
        : public Thread
    {
    public:
        OnEventThread(
            /* [in] */ const String& path,
            /* [in] */ CMountService* host);

        virtual CARAPI Run();

    private:
        String mPath;
        CMountService* mHost;;
    };

    class DecryptStorageRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        DecryptStorageRunnable(
            /* [in] */ CMountService* host);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        CMountService* mHost;
    };

public:
    CMountService();

    ~CMountService();

    CARAPI_(void) WaitForAsecScan();

    /**
     * Callback from NativeDaemonConnector
     */
    CARAPI OnDaemonConnected();

    /**
     * Callback from NativeDaemonConnector
     */
    CARAPI OnEvent(
        /* [in] */ Int32 code,
        /* [in] */ const String& raw,
        /* [in] */ const ArrayOf<String>& cooked,
        /* [out] */ Boolean* result);

    CARAPI_(void) SystemReady();

    CARAPI RegisterListener(
        /* [in] */ IIMountServiceListener* listener);

    CARAPI UnregisterListener(
        /* [in] */ IIMountServiceListener* listener);

    CARAPI Shutdown(
        /* [in] */ IIMountShutdownObserver* observer);

    CARAPI IsUsbMassStorageConnected(
        /* [out] */ Boolean* connected);

    CARAPI SetUsbMassStorageEnabled(
        /* [in] */ Boolean enable);

    CARAPI IsUsbMassStorageEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI GetVolumeState(
        /* [in] */ const String& mountPoint,
        /* [out] */ String* state);

    CARAPI IsExternalStorageEmulated(
        /* [out] */ Boolean* emulated);

    CARAPI MountVolume(
        /* [in] */ const String& mountPoint,
        /* [out] */ Int32* result);

    CARAPI UnmountVolume(
        /* [in] */ const String& path,
        /* [in] */ Boolean force,
        /* [in] */ Boolean removeEncryption);

    CARAPI FormatVolume(
        /* [in] */ const String& mountPoint,
        /* [out] */ Int32* result);

    CARAPI GetStorageUsers(
        /* [in] */ const String& path,
        /* [out, callee] */ ArrayOf<Int32>** pids);

    CARAPI GetSecureContainerList(
        /* [out, callee] */ ArrayOf<String>** pids);

    CARAPI CreateSecureContainer(
        /* [in] */ const String& id,
        /* [in] */ Int32 sizeMb,
        /* [in] */ const String& fstype,
        /* [in] */ const String& key,
        /* [in] */ Int32 ownerUid,
        /* [in] */ Boolean external,
        /* [out] */ Int32* result);

    CARAPI FinalizeSecureContainer(
        /* [in] */ const String& id,
        /* [out] */ Int32* result);

    CARAPI FixPermissionsSecureContainer(
        /* [in] */ const String& id,
        /* [in] */ Int32 gid,
        /* [in] */ const String& filename,
        /* [out] */ Int32* result);

    CARAPI DestroySecureContainer(
        /* [in] */ const String& id,
        /* [in] */ Boolean force,
        /* [out] */ Int32* result);

    CARAPI MountSecureContainer(
        /* [in] */ const String& id,
        /* [in] */ const String& key,
        /* [in] */ Int32 ownerUid,
        /* [out] */ Int32* result);

    CARAPI UnmountSecureContainer(
        /* [in] */ const String& id,
        /* [in] */ Boolean force,
        /* [out] */ Int32* result);

    CARAPI IsSecureContainerMounted(
        /* [in] */ const String& id,
        /* [out] */ Boolean* mounted);

    CARAPI RenameSecureContainer(
        /* [in] */ const String& oldId,
        /* [in] */ const String& newId,
        /* [out] */ Int32* result);

    CARAPI GetSecureContainerPath(
        /* [in] */ const String& id,
        /* [out] */ String* path);

    CARAPI GetSecureContainerFilesystemPath(
        /* [in] */ const String& id,
        /* [out] */ String* path);

    CARAPI FinishMediaUpdate();

    CARAPI GetMountedObbPath(
        /* [in] */ const String& rawPath,
        /* [out] */ String* obbPath);

    CARAPI IsObbMounted(
        /* [in] */ const String& rawPath,
        /* [out] */ Boolean* mounted);

    CARAPI MountObb(
        /* [in] */ const String& rawPath,
        /* [in] */ const String& canonicalPath,
        /* [in] */ const String& key,
        /* [in] */ IIObbActionListener* token,
        /* [in] */ Int32 nonce);

    CARAPI UnmountObb(
        /* [in] */ const String& rawPath,
        /* [in] */ Boolean force,
        /* [in] */ IIObbActionListener* token,
        /* [in] */ Int32 nonce);

    CARAPI GetEncryptionState(
        /* [out] */ Int32* state);

    CARAPI DecryptStorage(
        /* [in] */ const String& password,
        /* [out] */ Int32* result);

    CARAPI EncryptStorage(
        /* [in] */ const String& password,
        /* [out] */ Int32* result);

    CARAPI ChangeEncryptionPassword(
        /* [in] */ const String& password,
        /* [out] */ Int32* result);

    CARAPI VerifyEncryptionPassword(
        /* [in] */ const String& password,
        /* [out] */ Int32* result);

    CARAPI GetVolumeList(
        /* [out, callee] */ ArrayOf<IStorageVolume*>** volList);

    static CARAPI_(String) BuildObbPath(
        /* [in] */ const String& canonicalPath,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean forVold);

    CARAPI Monitor();

    CARAPI constructor(
        /* [in] */ IContext* context);

protected:
    // protected void dump(FileDescriptor fd, PrintWriter pw, String[] args)

private:
    // for MountServiceHandler
    CARAPI_(void) HandleUnmountPmUpdate(
        /* [in] */ UnmountCallBack* ucb);

    CARAPI_(void) HandleUnmountPmDone();

    CARAPI_(void) HandleUnmountMs(
        /* [in] */ UnmountCallBack* ucb);

    CARAPI_(void) WaitForReady();

    CARAPI_(void) WaitForLatch(
        /* [in] */ ICountDownLatch* latch);

    CARAPI_(void) HandleSystemReady();

    CARAPI DoShareUnshareVolume(
        /* [in] */ const String& path,
        /* [in] */ const String& method,
        /* [in] */ Boolean enable);

    CARAPI_(void) UpdatePublicVolumeState(
        /* [in] */ IStorageVolume* volume,
        /* [in] */ const String& state);

    CARAPI_(void) NotifyVolumeStateChange(
        /* [in] */ const String& label,
        /* [in] */ const String& path,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    CARAPI_(Int32) DoMountVolume(
        /* [in] */ const String& path);

    CARAPI_(Int32) DoUnmountVolume(
        /* [in] */ const String& path,
        /* [in] */ Boolean force,
        /* [in] */ Boolean removeEncryption);

    CARAPI_(Int32) DoFormatVolume(
        /* [in] */ const String& path);

    CARAPI_(Boolean) DoGetVolumeShared(
        /* [in] */ const String& path,
        /* [in] */ const String& method);

    CARAPI_(void) NotifyShareAvailabilityChange(
        /* [in] */ const Boolean avail);

    CARAPI_(void) SendStorageIntent(
        /* [in] */ const String& action,
        /* [in] */ IStorageVolume* volume,
        /* [in] */ IUserHandle* user);

    CARAPI_(void) SendUmsIntent(
        /* [in] */ Boolean c);

    CARAPI ValidatePermission(
        /* [in] */ const String& perm);

    CARAPI ReadStorageListLocked();

    /**
     * Create and add new {@link StorageVolume} for given {@link UserHandle}
     * using {@link #mEmulatedTemplate} as template.
     */
    CARAPI_(void) CreateEmulatedVolumeForUserLocked(
        /* [in] */ IUserHandle* user);

    CARAPI AddVolumeLocked(
        /* [in] */ IStorageVolume* volume);

    CARAPI_(void) RemoveVolumeLocked(
        /* [in] */ IStorageVolume* volume);

    CARAPI_(AutoPtr<IStorageVolume>) GetPrimaryPhysicalVolume();

    CARAPI_(Boolean) GetUmsEnabling();

    CARAPI_(void) SetUmsEnabling(
        /* [in] */ Boolean enable);

    CARAPI_(void) WarnOnNotMounted();

    CARAPI_(Boolean) IsUidOwnerOfPackageOrSystem(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 callerUid);

    CARAPI AddObbStateLocked(
        /* [in] */ ObbState* obbState);

    CARAPI_(void) RemoveObbStateLocked(
        /* [in] */ ObbState* obbState);

    // for ObbActionHandler
    CARAPI_(void) HandleObbRunAction(
        /* [in] */ ObbAction* action);

    CARAPI_(void) HandleObbMcsBound(
        /* [in] */ IMediaContainerService* service);

    CARAPI_(void) HandleObbMcsReconnect();

    CARAPI_(void) HandleObbMcsUnbind();

    CARAPI_(void) HandleObbFlushMountState(
        /* [in] */ const String& path);

    CARAPI_(Boolean) ConnectToService();

    CARAPI_(void) DisconnectService();

private:
    static const Boolean LOCAL_LOGD;
    static const Boolean DEBUG_UNMOUNT;
    static const Boolean DEBUG_EVENTS;
    static const Boolean DEBUG_OBB;

    // Disable this since it messes up long-running cryptfs operations.
    static const Boolean WATCHDOG_ENABLE;

    static const String TAG;

    static const String VOLD_TAG;

    /** Maximum number of ASEC containers allowed to be mounted. */
    static const Int32 MAX_CONTAINERS = 250;

    /**
     * The size of the crypto algorithm key in bits for OBB files. Currently
     * Twofish is used which takes 128-bit keys.
     */
    static const Int32 CRYPTO_ALGORITHM_KEY_SIZE = 128;

    /**
     * The number of times to run SHA1 in the PBKDF2 function for OBB files.
     * 1024 is reasonably secure and not too slow.
     */
    static const Int32 PBKDF2_HASH_ROUNDS = 1024;

    /*
     * Default Container Service information
     */
    static const AutoPtr<IComponentName> DEFAULT_CONTAINER_COMPONENT;

    // Handler messages
    static const Int32 H_UNMOUNT_PM_UPDATE = 1;
    static const Int32 H_UNMOUNT_PM_DONE = 2;
    static const Int32 H_UNMOUNT_MS = 3;
    static const Int32 H_SYSTEM_READY = 4;

    static const Int32 RETRY_UNMOUNT_DELAY = 30; // in ms
    static const Int32 MAX_UNMOUNT_RETRIES = 4;

    // Storage list XML tags
    static const String TAG_STORAGE_LIST;
    static const String TAG_STORAGE;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<NativeDaemonConnector> mConnector;

    Object mVolumesLock;

    /** When defined, base template for user-specific {@link StorageVolume}. */
    AutoPtr<IStorageVolume> mEmulatedTemplate;

    // @GuardedBy("mVolumesLock")
    List< AutoPtr<IStorageVolume> > mVolumes;

    /** Map from path to {@link StorageVolume} */
    // @GuardedBy("mVolumesLock")
    HashMap<String, AutoPtr<IStorageVolume> > mVolumesByPath;

    /** Map from path to state */
    // @GuardedBy("mVolumesLock")
    HashMap<String, String> mVolumeStates;
    Object mVolumeStatesLock;

    volatile Boolean mSystemReady;

    AutoPtr<CPackageManagerService> mPms;
    Boolean mUmsEnabling;
    Boolean mUmsAvailable;
    // Used as a lock for methods that register/unregister listeners.
    List<AutoPtr<MountServiceBinderListener> > mListeners;
    Object mListenersLock;
    AutoPtr<ICountDownLatch> mConnectedSignal;
    AutoPtr<ICountDownLatch> mAsecsScanned;
    Boolean mSendUmsConnectedOnBoot;

    /**
     * Private hash of currently mounted secure containers.
     * Used as a lock in methods to manipulate secure containers.
     */
    HashSet<String> mAsecMountSet;
    Object mAsecMountSetLock;

    /**
     * Mounted OBB tracking information. Used to track the current state of all
     * OBBs.
     */
    typedef List< AutoPtr<ObbState> > ObbStateList;
    HashMap<AutoPtr<IBinder>, AutoPtr<ObbStateList> > mObbMounts;
    Object mObbMountsLock;

    /** Map from raw paths to {@link ObbState}. */
    HashMap<String, AutoPtr<ObbState> > mObbPathToStateMap;
    Object mObbPathToStateMapLock;

    // OBB Action Handler
    AutoPtr<IHandler> mObbActionHandler;

    static const Int32 OBB_RUN_ACTION = 1;
    static const Int32 OBB_MCS_BOUND = 2;
    static const Int32 OBB_MCS_UNBIND = 3;
    static const Int32 OBB_MCS_RECONNECT = 4;
    static const Int32 OBB_FLUSH_MOUNT_STATE = 5;

    AutoPtr<DefaultContainerConnection> mDefContainerConn;

    // Used in the ObbActionHandler
    AutoPtr<IMediaContainerService> mContainerService;

    AutoPtr<IHandlerThread> mHandlerThread;
    AutoPtr<IHandler> mHandler;

    AutoPtr<IBroadcastReceiver> mUserReceiver;
    AutoPtr<IBroadcastReceiver> mUsbReceiver;

    // for mHandler
    List<AutoPtr<UnmountCallBack> > mForceUnmounts;
    Boolean mUpdatingStatus;

    // for mObbActionHandler
    Boolean mBound;
    List<AutoPtr<ObbAction> > mActions;

    friend class DefaultContainerConnection;
    friend class UnmountCallBack;
    friend class UserBroadcastReceiver;
    friend class UsbBroadcastReceiver;
    friend class OnDaemonConnectedThread;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CMOUNTSERVICE_H__
