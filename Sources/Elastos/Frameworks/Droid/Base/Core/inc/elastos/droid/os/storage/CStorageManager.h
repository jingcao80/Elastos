
#ifndef __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGER_H__
#define __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGER_H__

#include "_Elastos_Droid_Os_Storage_CStorageManager.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/provider/Settings.h"

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"

#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Provider::Settings;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CarClass(CStorageManager)
     , public Object
     , public IStorageManager
{
public:
    friend class CObbActionListener;
    friend class CStorageManagerHelper;
    friend class CMountServiceBinderListener;
    class StorageEvent;
    class ObbListenerDelegate;

    class ObbHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CStorageManager::ObbHandler")

        ObbHandler(
            /* [in] */ ObbListenerDelegate * host,
            /* [in] */ ILooper * looper)
            : Handler(looper)
            , mHost(host)
        {}

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        ObbListenerDelegate * mHost;
    };

    /**
     * Private class containing sender and receiver code for StorageEvents.
     */
    class ObbListenerDelegate
        : public Object
    {
    public:
        friend class CObbActionListener;

        ObbListenerDelegate(
            /* [in] */ IOnObbStateChangeListener* listener,
            /* [in] */ CStorageManager* host);

        CARAPI_(AutoPtr<IOnObbStateChangeListener>) GetListener();

        CARAPI_(void) SendObbStateChanged(
            /* [in] */ const String& path,
            /* [in] */ Int32 state);

    private:
        AutoPtr<ObbHandler> mHandler;

        CStorageManager* mHost;
        //WeakReference<IOnObbStateChangeListener> mObbEventListenerRef;
        AutoPtr<IWeakReference> mObbEventListenerRef;
        Int32 mNonce;
    };

    /**
     * Private base class for messages sent between the callback thread
     * and the target looper handler.
     */
    class StorageEvent
        : public Object
    {
    public:
        static const Int32 EVENT_UMS_CONNECTION_CHANGED = 1;
        static const Int32 EVENT_STORAGE_STATE_CHANGED = 2;
        static const Int32 EVENT_OBB_STATE_CHANGED = 3;

        StorageEvent(
            /* [in] */ Int32 what);

        AutoPtr<IMessage> GetMessage()
        {
            return mMessage;
        }
    private:
        AutoPtr<IMessage> mMessage;
    };

    /**
     * Message sent during an OBB status change event.
     */
    class ObbStateChangedStorageEvent
        : public StorageEvent
    {
    public:
        ObbStateChangedStorageEvent(
            /* [in] */ const String& path,
            /* [in] */ Int32 state)
            : StorageEvent(EVENT_OBB_STATE_CHANGED)
            , mPath(path)
            , mState(state)
        {}

    public:
        String mPath;
        Int32 mState;
    };

    /**
     * Message sent on a USB mass storage connection change.
     */
    class UmsConnectionChangedStorageEvent
        : public StorageEvent
    {
    public:
        UmsConnectionChangedStorageEvent(
            /* [in] */ Boolean a)
            : StorageEvent(EVENT_UMS_CONNECTION_CHANGED)
            , mAvailable(a)
        {}

    public:
        Boolean mAvailable;
    };

    /**
     * Message sent on volume state change.
     */
    class StorageStateChangedStorageEvent
        : public StorageEvent
    {
    public:
        StorageStateChangedStorageEvent(
            /* [in] */ const String& p,
            /* [in] */ const String& oldS,
            /* [in] */ const String& newS)
            : StorageEvent(EVENT_STORAGE_STATE_CHANGED)
            , mPath(p)
            , mOldState(oldS)
            , mNewState(newS)
        {}

    public:
        String mPath;
        String mOldState;
        String mNewState;
    };

    class ListenerDelegate;
    class ListenerHandler
        : public Handler
    {
    public:
        ListenerHandler(
            /* [in] */ ListenerDelegate * host,
            /* [in] */ ILooper * looper)
            : Handler(looper)
            , mHost(host)
        {}

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        ListenerDelegate * mHost;
    };

    /**
     * Private class containing sender and receiver code for StorageEvents.
     */
    class ListenerDelegate
        : public Object
    {
    public:
        ListenerDelegate(
            /* [in] */ IStorageEventListener* listener,
            /* [in] */ CStorageManager* host);

        CARAPI_(AutoPtr<IStorageEventListener>) GetListener();

        CARAPI_(void) SendShareAvailabilityChanged(
            /* [in] */ Boolean available);

        CARAPI_(void) SendStorageStateChanged(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState);

    public:
        AutoPtr<IStorageEventListener> mStorageEventListener;

    private:
        CStorageManager* mHost;

        AutoPtr<ListenerHandler> mHandler;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CStorageManager();

    CARAPI constructor(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ ILooper* tgtLooper);

    CARAPI RegisterListener(
        /* [in] */ IStorageEventListener * listener);

    CARAPI UnregisterListener(
        /* [in] */ IStorageEventListener * listener);

    CARAPI EnableUsbMassStorage();

    CARAPI DisableUsbMassStorage();

    CARAPI IsUsbMassStorageConnected(
        /* [out] */ Boolean * pConnect);

    CARAPI IsUsbMassStorageEnabled(
        /* [out] */ Boolean * pEnabled);

    CARAPI MountObb(
        /* [in] */ const String& rawPath,
        /* [in] */ const String& key,
        /* [in] */ IOnObbStateChangeListener * listener,
        /* [out] */ Boolean * pMounted);

    CARAPI UnmountObb(
        /* [in] */ const String& rawPath,
        /* [in] */ Boolean force,
        /* [in] */ IOnObbStateChangeListener * listener,
        /* [out] */ Boolean * pMounted);

    CARAPI IsObbMounted(
        /* [in] */ const String& rawPath,
        /* [out] */ Boolean * pMounted);

    CARAPI GetMountedObbPath(
        /* [in] */ const String& rawPath,
        /* [out] */ String * pObbpath);

    CARAPI GetVolumeState(
        /* [in] */ const String& mountPoint,
        /* [out] */ String * pState);

    CARAPI GetVolumeList(
        /* [out, callee] */ ArrayOf<IStorageVolume *> ** ppVolumelists);

    CARAPI GetVolumePaths(
        /* [out, callee] */ ArrayOf<String> ** ppVolumepaths);

    CARAPI GetPrimaryVolume(
        /* [out] */ IStorageVolume** result );

    /**
     * Return the number of available bytes until the given path is considered
     * running low on storage.
     *
     * @hide
     */
    CARAPI GetStorageBytesUntilLow(
        /* [in] */ IFile* path,
        /* [out] */ Int64* result);

    /**
     * Return the number of available bytes at which the given path is
     * considered running low on storage.
     *
     * @hide
     */
    CARAPI GetStorageLowBytes(
        /* [in] */ IFile* path,
        /* [out] */ Int64* result);

    /**
     * Return the number of available bytes at which the given path is
     * considered full.
     *
     * @hide
     */
    CARAPI GetStorageFullBytes(
        /* [in] */ IFile* path,
        /* [out] */ Int64* result);

public:
    /// Consts to match the password types in cryptfs.h
    /** @hide */
    const static Int32 CRYPT_TYPE_PASSWORD;// = 0;
    /** @hide */
    const static Int32 CRYPT_TYPE_DEFAULT;// = 1;
    /** @hide */
    const static Int32 CRYPT_TYPE_PATTERN;// = 2;
    /** @hide */
    const static Int32 CRYPT_TYPE_PIN;// = 3;

    // Constants for the data available via MountService.getField.
    /** @hide */
    const static  String SYSTEM_LOCALE_KEY;// = "SystemLocale";
    /** @hide */
    const static  String OWNER_INFO_KEY;// = "OwnerInfo";
    /** @hide */
    const static  String PATTERN_VISIBLE_KEY;// = "PatternVisible";

private:
    CARAPI_(Int32) GetNextNonce();

private:
    static const String TAG;

    const static Int32 DEFAULT_THRESHOLD_PERCENTAGE;// = 10;
    const static Int64 DEFAULT_THRESHOLD_MAX_BYTES;// = 500 * ITrafficStats::MB_IN_BYTES;
    const static Int64 DEFAULT_FULL_THRESHOLD_BYTES;// = ITrafficStats::MB_IN_BYTES;

    AutoPtr<IContentResolver> mResolver;
    /*
     * Our internal MountService binder reference
     */
    AutoPtr<IIMountService> mMountService;

    /*
     * The looper target for callbacks
     */
    AutoPtr<ILooper> mTgtLooper;

    /*
     * Target listener for binder callbacks
     */
    AutoPtr<IIMountServiceListener> mBinderListener;

    /*
     * List of our listeners
     */
    List<AutoPtr<ListenerDelegate> > mListeners;

    /*
     * Next available nonce
     */
    AutoPtr<IAtomicInteger32> mNextNonce;

    /**
     * Binder listener for OBB action results.
     */
    AutoPtr<IIObbActionListener> mObbActionListener;

    Object mListenersLock;
};

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_STORAGE_CSTORAGEMANAGER_H__
