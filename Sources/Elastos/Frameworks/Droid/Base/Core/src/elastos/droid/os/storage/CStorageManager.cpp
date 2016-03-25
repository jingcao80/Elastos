
#include "elastos/droid/os/storage/CStorageManager.h"
#include "elastos/droid/os/storage/CStorageManagerHelper.h"
#include "elastos/droid/os/storage/CObbActionListener.h"
#include "elastos/droid/os/storage/CMountServiceBinderListener.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "Elastos.Droid.Provider.h"

#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/Math.h>

using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Provider::ISettingsGlobal;

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

const Int32 CStorageManager::CRYPT_TYPE_PASSWORD = 0;
const Int32 CStorageManager::CRYPT_TYPE_DEFAULT = 1;
const Int32 CStorageManager::CRYPT_TYPE_PATTERN = 2;
const Int32 CStorageManager::CRYPT_TYPE_PIN = 3;
const String CStorageManager::SYSTEM_LOCALE_KEY("SystemLocale");
const String CStorageManager::OWNER_INFO_KEY("OwnerInfo");
const String CStorageManager::PATTERN_VISIBLE_KEY("PatternVisible");
const String CStorageManager::TAG("StorageManager");
const Int32 CStorageManager::DEFAULT_THRESHOLD_PERCENTAGE = 10;
const Int64 CStorageManager::DEFAULT_THRESHOLD_MAX_BYTES = 500 * ITrafficStats::MB_IN_BYTES;
const Int64 CStorageManager::DEFAULT_FULL_THRESHOLD_BYTES = ITrafficStats::MB_IN_BYTES;

CStorageManager::StorageEvent::StorageEvent(
    /* [in] */ Int32 what)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain((IMessage**)&mMessage);
    mMessage->SetWhat(what);
    mMessage->SetObj(TO_IINTERFACE(this));
}

ECode CStorageManager::ObbHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IOnObbStateChangeListener> changeListener = mHost->GetListener();
    if (changeListener == NULL) {
        return NOERROR;
    }

    AutoPtr<IInterface> tmp;
    msg->GetObj((IInterface**)&tmp);
    AutoPtr<IObject> obj = IObject::Probe(tmp);
    StorageEvent* e = (StorageEvent*)obj.Get();

    Int32 what;
    msg->GetWhat(&what);

    if (what == StorageEvent::EVENT_OBB_STATE_CHANGED) {
        ObbStateChangedStorageEvent* ev = (ObbStateChangedStorageEvent *) e;
        changeListener->OnObbStateChange(ev->mPath, ev->mState);
    }
    else {
        Logger::E("CStorageManager::ObbHandler", "Unsupported event %d", what);
    }

    return NOERROR;
}

/**
 * Private class containing sender and receiver code for StorageEvents.
 */
CStorageManager::ObbListenerDelegate::ObbListenerDelegate(
    /* [in] */ IOnObbStateChangeListener* listener,
    /* [in] */ CStorageManager* host)
    : mHost(host)
{
    mNonce = mHost->GetNextNonce();
    IWeakReferenceSource * wr = IWeakReferenceSource::Probe(listener);
    if (wr) {
        wr->GetWeakReference((IWeakReference**)&mObbEventListenerRef);
    }
    mHandler = new ObbHandler(this, mHost->mTgtLooper);
}

AutoPtr<IOnObbStateChangeListener> CStorageManager::ObbListenerDelegate::GetListener()
{
    if (mObbEventListenerRef == NULL) {
        return NULL;
    }
    AutoPtr<IInterface> obj;
    mObbEventListenerRef->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<IOnObbStateChangeListener> listener;
    if (obj != NULL)
        listener = IOnObbStateChangeListener::Probe(obj);
    return listener;
}

void CStorageManager::ObbListenerDelegate::SendObbStateChanged(
    /* [in] */ const String& path,
    /* [in] */ Int32 state)
{
    AutoPtr<ObbStateChangedStorageEvent> e = new ObbStateChangedStorageEvent(path, state);
    Boolean result;
    mHandler->SendMessage(e->GetMessage(), &result);
}

ECode CStorageManager::ListenerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> tmp;
    msg->GetObj((IInterface**)&tmp);
    AutoPtr<IObject> obj = IObject::Probe(tmp);
    StorageEvent* e = (StorageEvent*)obj.Get();

    if (what == StorageEvent::EVENT_UMS_CONNECTION_CHANGED) {
        AutoPtr<UmsConnectionChangedStorageEvent> ev = (UmsConnectionChangedStorageEvent*)e;
        mHost->mStorageEventListener->OnUsbMassStorageConnectionChanged(ev->mAvailable);
    }
    else if (what == StorageEvent::EVENT_STORAGE_STATE_CHANGED) {
        AutoPtr<StorageStateChangedStorageEvent> ev = (StorageStateChangedStorageEvent*)e;
        mHost->mStorageEventListener->OnStorageStateChanged(ev->mPath, ev->mOldState, ev->mNewState);
    }
    else {
        Logger::E("CStorageManager::ListenerHandler", "Unsupported event %d", what);
    }
    return NOERROR;
}

/**
 * Private class containing sender and receiver code for StorageEvents.
 */
CStorageManager::ListenerDelegate::ListenerDelegate(
    /* [in] */ IStorageEventListener* listener,
    /* [in] */ CStorageManager* host)
    : mStorageEventListener(listener)
    , mHost(host)
{
    mHandler = new ListenerHandler(this, mHost->mTgtLooper);
}

AutoPtr<IStorageEventListener> CStorageManager::ListenerDelegate::GetListener()
{
    return mStorageEventListener;
}

void CStorageManager::ListenerDelegate::SendShareAvailabilityChanged(
    /* [in] */ Boolean available)
{
    AutoPtr<UmsConnectionChangedStorageEvent> e = new CStorageManager::UmsConnectionChangedStorageEvent(available);
    Boolean result;
    mHandler->SendMessage(e->GetMessage(), &result);
}

void CStorageManager::ListenerDelegate::SendStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    AutoPtr<StorageStateChangedStorageEvent> e = new CStorageManager::StorageStateChangedStorageEvent(path, oldState, newState);
    Boolean result;
    mHandler->SendMessage(e->GetMessage(), &result);
}

CAR_INTERFACE_IMPL(CStorageManager, Object, IStorageManager)
CAR_OBJECT_IMPL(CStorageManager)

CStorageManager::CStorageManager()
{
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mNextNonce);
    CObbActionListener::New(this, (IIObbActionListener**)&mObbActionListener);
}

ECode CStorageManager::constructor(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ ILooper* tgtLooper)
{
    mResolver = resolver;
    mTgtLooper = tgtLooper;
    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    assert(serviceManager != NULL);
    AutoPtr<IInterface> service;
    serviceManager->GetService(String("mount"), (IInterface**)&service);
    mMountService = IIMountService::Probe(service);
    if (mMountService == NULL) {
        Logger::E(TAG, "Unable to connect to mount service! - is it running yet?");
        return NOERROR;
    }

    return NOERROR;
}

Int32 CStorageManager::GetNextNonce()
{
    Int32 value;
    mNextNonce->GetAndIncrement(&value);
    return value;
}

/**
 * Registers a {@link android.os.storage.StorageEventListener StorageEventListener}.
 *
 * @param listener A {@link android.os.storage.StorageEventListener StorageEventListener} object.
 *
 * @hide
 */
ECode CStorageManager::RegisterListener(
    /* [in] */ IStorageEventListener * listener)
{
    if (listener == NULL) {
        return NOERROR;
    }

    AutoLock lock(mListenersLock);
    if (mBinderListener == NULL ) {
        // try {
        CMountServiceBinderListener::New(this, (IIMountServiceListener**)&mBinderListener);
        if (FAILED(mMountService->RegisterListener(mBinderListener))) {
        // } catch (RemoteException rex) {
            Logger::E(TAG, "Register mBinderListener failed");
            return NOERROR;
        }
    }
    AutoPtr<ListenerDelegate> delegate = new ListenerDelegate(listener, this);
    mListeners.PushBack(delegate);
    return NOERROR;
}

/**
 * Unregisters a {@link android.os.storage.StorageEventListener StorageEventListener}.
 *
 * @param listener A {@link android.os.storage.StorageEventListener StorageEventListener} object.
 *
 * @hide
 */
ECode CStorageManager::UnregisterListener(
    /* [in] */ IStorageEventListener * listener)
{
    if (listener == NULL) {
        return NOERROR;
    }

    AutoLock lock(mListenersLock);
    List<AutoPtr<ListenerDelegate> >::Iterator it = mListeners.Begin();
    while ( it != mListeners.End()) {
        if ((*it)->GetListener().Get() == listener) {
            it = mListeners.Erase(it);
            break;
        }
        ++it;
    }
    if (mListeners.Begin() == mListeners.End() && mBinderListener != NULL) {
        // try {
        if (FAILED(mMountService->UnregisterListener(mBinderListener))) {
        // } catch (RemoteException rex) {
            Logger::E(TAG, "Unregister mBinderListener failed");
            return NOERROR;
        }
    }
    return NOERROR;
}

/**
 * Enables USB Mass Storage (UMS) on the device.
 *
 * @hide
 */
ECode CStorageManager::EnableUsbMassStorage()
{
    if (FAILED(mMountService->SetUsbMassStorageEnabled(TRUE)))
        Logger::E(TAG, "Failed to enable UMS");
    return NOERROR;
}

/**
 * Disables USB Mass Storage (UMS) on the device.
 *
 * @hide
 */
ECode CStorageManager::DisableUsbMassStorage()
{
    if (FAILED(mMountService->SetUsbMassStorageEnabled(FALSE)))
        Logger::E(TAG, "Failed to disable UMS");
    return NOERROR;
}

/**
 * Query if a USB Mass Storage (UMS) host is connected.
 * @return true if UMS host is connected.
 *
 * @hide
 */
ECode CStorageManager::IsUsbMassStorageConnected(
    /* [out] */ Boolean * pConnect)
{
    VALIDATE_NOT_NULL(pConnect);
    *pConnect = FALSE;
    if (FAILED(mMountService->IsUsbMassStorageConnected(pConnect)))
        Logger::E(TAG, "Failed to get UMS connection state");
    return NOERROR;
}

/**
 * Query if a USB Mass Storage (UMS) is enabled on the device.
 * @return true if UMS host is enabled.
 *
 * @hide
 */
ECode CStorageManager::IsUsbMassStorageEnabled(
    /* [out] */ Boolean * pEnabled)
{
    VALIDATE_NOT_NULL(pEnabled);
    *pEnabled = FALSE;
    if (FAILED(mMountService->IsUsbMassStorageEnabled(pEnabled)))
        Logger::E(TAG, "Failed to get UMS enable state");
    return NOERROR;
}

/**
 * Mount an Opaque Binary Blob (OBB) file. If a <code>key</code> is
 * specified, it is supplied to the mounting process to be used in any
 * encryption used in the OBB.
 * <p>
 * The OBB will remain mounted for as long as the StorageManager reference
 * is held by the application. As soon as this reference is lost, the OBBs
 * in use will be unmounted. The {@link OnObbStateChangeListener} registered
 * with this call will receive the success or failure of this operation.
 * <p>
 * <em>Note:</em> you can only mount OBB files for which the OBB tag on the
 * file matches a package ID that is owned by the calling program's UID.
 * That is, shared UID applications can attempt to mount any other
 * application's OBB that shares its UID.
 *
 * @param rawPath the path to the OBB file
 * @param key secret used to encrypt the OBB; may be <code>null</code> if no
 *            encryption was used on the OBB.
 * @param listener will receive the success or failure of the operation
 * @return whether the mount call was successfully queued or not
 */
ECode CStorageManager::MountObb(
    /* [in] */ const String& rawPath,
    /* [in] */ const String& key,
    /* [in] */ IOnObbStateChangeListener * listener,
    /* [out] */ Boolean * pMounted)
{
    VALIDATE_NOT_NULL(pMounted);
    *pMounted = FALSE;
    // TODO:
    //Preconditions.checkNotNull(rawPath, "rawPath cannot be null");
    //Preconditions.checkNotNull(listener, "listener cannot be null");

    //try {
    AutoPtr<IFile> file;
    CFile::New(rawPath, (IFile**)&file);
    String canonicalPath;
    if (FAILED(file->GetCanonicalPath(&canonicalPath))) {
        Logger::E(TAG, "Failed to resolve path: %s", rawPath.string());
        return NOERROR;
    }
    Int32 nonce = ((CObbActionListener*)mObbActionListener.Get())->AddListener(listener);
    if (FAILED(mMountService->MountObb(rawPath, canonicalPath, key,
            mObbActionListener, nonce))) {
        Logger::E(TAG, "Failed to mount OBB");
        return NOERROR;
    }
    *pMounted = TRUE;
    return NOERROR;
    //} catch (IOException e) {
    //    throw new IllegalArgumentException("Failed to resolve path: " + rawPath, e);
    //} catch (RemoteException e) {
    //    Log.e(TAG, "Failed to mount OBB", e);
    //}
}

/**
 * Unmount an Opaque Binary Blob (OBB) file asynchronously. If the
 * <code>force</code> flag is true, it will kill any application needed to
 * unmount the given OBB (even the calling application).
 * <p>
 * The {@link OnObbStateChangeListener} registered with this call will
 * receive the success or failure of this operation.
 * <p>
 * <em>Note:</em> you can only mount OBB files for which the OBB tag on the
 * file matches a package ID that is owned by the calling program's UID.
 * That is, shared UID applications can obtain access to any other
 * application's OBB that shares its UID.
 * <p>
 *
 * @param rawPath path to the OBB file
 * @param force whether to kill any programs using this in order to unmount
 *            it
 * @param listener will receive the success or failure of the operation
 * @return whether the unmount call was successfully queued or not
 */
ECode CStorageManager::UnmountObb(
    /* [in] */ const String& rawPath,
    /* [in] */ Boolean force,
    /* [in] */ IOnObbStateChangeListener * listener,
    /* [out] */ Boolean * pMounted)
{
    VALIDATE_NOT_NULL(pMounted);
    *pMounted = FALSE;
    // TODO:
    //Preconditions.checkNotNull(rawPath, "rawPath cannot be null");
    //Preconditions.checkNotNull(listener, "listener cannot be null");

    //try {
    Int32 nonce = ((CObbActionListener*)mObbActionListener.Get())->AddListener(listener);
    if (FAILED(mMountService->UnmountObb(rawPath, force,
            mObbActionListener, nonce))) {
        Logger::E(TAG, "Failed to mount OBB");
        return NOERROR;
    }
    *pMounted = TRUE;
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.e(TAG, "Failed to mount OBB", e);
    //}
}

/**
 * Check whether an Opaque Binary Blob (OBB) is mounted or not.
 *
 * @param rawPath path to OBB image
 * @return true if OBB is mounted; false if not mounted or on error
 */
ECode CStorageManager::IsObbMounted(
    /* [in] */ const String& rawPath,
    /* [out] */ Boolean * pMounted)
{
    VALIDATE_NOT_NULL(pMounted);
    *pMounted = FALSE;
    // TODO:
    //Preconditions.checkNotNull(rawPath, "rawPath cannot be null");

    //try {
    if (FAILED(mMountService->IsObbMounted(rawPath, pMounted))) {
        Logger::E(TAG, "Failed to check if OBB is mounted");
        return NOERROR;
    }
    //} catch (RemoteException e) {
    //    Log.e(TAG, "Failed to check if OBB is mounted", e);
    //}

    return NOERROR;
}

/**
 * Check the mounted path of an Opaque Binary Blob (OBB) file. This will
 * give you the path to where you can obtain access to the internals of the
 * OBB.
 *
 * @param rawPath path to OBB image
 * @return absolute path to mounted OBB image data or <code>null</code> if
 *         not mounted or exception encountered trying to read status
 */
ECode CStorageManager::GetMountedObbPath(
    /* [in] */ const String& rawPath,
    /* [out] */ String * pObbpath)
{
    VALIDATE_NOT_NULL(pObbpath);
    *pObbpath = String(NULL);
    // TODO:
    //Preconditions.checkNotNull(rawPath, "rawPath cannot be null");

    //try {
    if (FAILED(mMountService->GetMountedObbPath(rawPath, pObbpath))) {
        Logger::E(TAG, "Failed to find mounted path for OBB");
        return NOERROR;
    }
    //} catch (RemoteException e) {
    //    Log.e(TAG, "Failed to find mounted path for OBB", e);
    //}
    return NOERROR;
}

/**
 * Gets the state of a volume via its mountpoint.
 * @hide
 */
ECode CStorageManager::GetVolumeState(
    /* [in] */ const String& mountPoint,
    /* [out] */ String* state)
{
    VALIDATE_NOT_NULL(state);
    *state = String(NULL);

    if (mMountService == NULL) {
        *state = IEnvironment::MEDIA_REMOVED;
    }
    else {
        if (FAILED(mMountService->GetVolumeState(mountPoint, state))) {
            Logger::E(TAG, "Failed to get volume state");
            return NOERROR;
        }
    }
    return NOERROR;
}

/**
 * Returns list of all mountable volumes.
 * @hide
 */
ECode CStorageManager::GetVolumeList(
    /* [out, callee] */ ArrayOf<IStorageVolume *> ** ppVolumelists)
{
    VALIDATE_NOT_NULL(ppVolumelists);
    *ppVolumelists = NULL;

    if (mMountService == NULL) {
        *ppVolumelists = ArrayOf<IStorageVolume*>::Alloc(0);
        REFCOUNT_ADD(*ppVolumelists);
        return NOERROR;
    }

    //try {
    AutoPtr<ArrayOf<IStorageVolume*> > list;
    if (FAILED(mMountService->GetVolumeList((ArrayOf<IStorageVolume*>**)&list))) {
        Logger::E(TAG, "Failed to get volume list");
        return NOERROR;
    }
    if (list == NULL) {
        *ppVolumelists = ArrayOf<IStorageVolume*>::Alloc(0);
        REFCOUNT_ADD(*ppVolumelists);
        return NOERROR;
    }

    *ppVolumelists = list;
    REFCOUNT_ADD(*ppVolumelists);
    return NOERROR;
    //} catch (RemoteException e) {
    //    Log.e(TAG, "Failed to get volume list", e);
    //    return null;
    //}
}

ECode CStorageManager::GetVolumePaths(
    /* [out, callee] */ ArrayOf<String> ** ppVolumepaths)
{
    VALIDATE_NOT_NULL(ppVolumepaths);
    *ppVolumepaths = NULL;

    AutoPtr<ArrayOf<IStorageVolume *> > volumes;
    GetVolumeList((ArrayOf<IStorageVolume*>**)&volumes);
    if (volumes == NULL) return NOERROR;
    Int32 count = volumes->GetLength();
    AutoPtr<ArrayOf<String> > paths = ArrayOf<String>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        (*volumes)[i]->GetPath(&(*paths)[i]);
    }
    *ppVolumepaths = paths;
    REFCOUNT_ADD(*ppVolumepaths);
    return NOERROR;
}

/** {@hide} */
ECode CStorageManager::GetPrimaryVolume(
    /* [out] */ IStorageVolume** result )
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<IStorageVolume*> > list;
    GetVolumeList((ArrayOf<IStorageVolume*>**)&list);
    AutoPtr<IStorageManagerHelper> helper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&helper);
    return helper->GetPrimaryVolume(list, result);
}

ECode CStorageManager::GetStorageBytesUntilLow(
    /* [in] */ IFile* path,
    /* [out] */ Int64* result)
{
    Int64 usableSpace, fullBytes;
    path->GetUsableSpace(&usableSpace);
    GetStorageFullBytes(path, &fullBytes);
    *result = usableSpace - fullBytes;
    return NOERROR;
}

ECode CStorageManager::GetStorageLowBytes(
    /* [in] */ IFile* path,
    /* [out] */ Int64* result)
{
    Int64 lowPercent;
    Settings::Global::GetInt64(mResolver,
            ISettingsGlobal::SYS_STORAGE_THRESHOLD_PERCENTAGE, DEFAULT_THRESHOLD_PERCENTAGE, &lowPercent);
    Int64 totalSpace;
    path->GetTotalSpace(&totalSpace);
    Int64 lowBytes = (totalSpace * lowPercent) / 100;

    Int64 maxLowBytes;
    Settings::Global::GetInt64(mResolver,
            ISettingsGlobal::SYS_STORAGE_THRESHOLD_MAX_BYTES, DEFAULT_THRESHOLD_MAX_BYTES, &maxLowBytes);

    *result = Elastos::Core::Math::Min(lowBytes, maxLowBytes);
    return NOERROR;
}

ECode CStorageManager::GetStorageFullBytes(
    /* [in] */ IFile* path,
    /* [out] */ Int64* result)
{
    return Settings::Global::GetInt64(mResolver,
            ISettingsGlobal::SYS_STORAGE_FULL_THRESHOLD_BYTES, DEFAULT_FULL_THRESHOLD_BYTES, result);
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
