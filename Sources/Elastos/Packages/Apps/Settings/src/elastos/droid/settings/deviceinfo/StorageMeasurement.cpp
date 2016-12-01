
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/settings/deviceinfo/StorageMeasurement.h"
#include "elastos/droid/settings/deviceinfo/CStorageMeasurementStatsObserver.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::EIID_IIPackageStatsObserver;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IUserEnvironment;
using Elastos::Droid::Os::CUserEnvironment;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Utility::CSparseInt64Array;
using Elastos::Core::IThread;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

//===============================================================================
//                  StorageMeasurement::MeasurementDetails
//===============================================================================

CAR_INTERFACE_IMPL(StorageMeasurement::MeasurementDetails, Object, IMeasurementDetails)

StorageMeasurement::MeasurementDetails::MeasurementDetails()
    : mTotalSize(0)
    , mAvailSize(0)
    , mAppsSize(0)
    , mCacheSize(0)
    , mMiscSize(0)
{
    CHashMap::New((IHashMap**)&mMediaSize);
    CSparseInt64Array::New((ISparseInt64Array**)&mUsersSize);
}

//===============================================================================
//                  StorageMeasurement::StatsObserver
//===============================================================================

CAR_INTERFACE_IMPL_2(StorageMeasurement::StatsObserver, Object, IIPackageStatsObserver, IBinder)

StorageMeasurement::StatsObserver::StatsObserver()
    : mIsInternal(FALSE)
    , mCurrentUser(0)
    , mRemaining(0)
{}

ECode StorageMeasurement::StatsObserver::constructor(
    /* [in] */ Boolean isInternal,
    /* [in] */ IMeasurementDetails* details, // MeasurementDetails
    /* [in] */ Int32 currentUser,
    /* [in] */ IMessage* finished,
    /* [in] */ Int32 remaining)
{
    mIsInternal = isInternal;
    mDetails = (MeasurementDetails*)details;
    mCurrentUser = currentUser;
    mFinished = finished;
    mRemaining = remaining;
    return NOERROR;
}

ECode StorageMeasurement::StatsObserver::OnGetStatsCompleted(
    /* [in] */ IPackageStats* stats,
    /* [in] */ Boolean succeeded)
{
    {
        AutoLock syncLock(mDetails);
        if (succeeded) {
            AddStatsLocked(stats);
        }
        if (--mRemaining == 0) {
            mFinished->SendToTarget();
        }
    }
    return NOERROR;
}

void StorageMeasurement::StatsObserver::AddStatsLocked(
    /* [in] */ IPackageStats* stats)
{
    if (mIsInternal) {
        Int64 codeSize = 0;
        stats->GetCodeSize(&codeSize);
        Int64 dataSize = 0;
        stats->GetDataSize(&dataSize);
        Int64 cacheSize = 0;
        stats->GetCacheSize(&cacheSize);
        AutoPtr<IEnvironment> environment;
        CEnvironment::AcquireSingleton((IEnvironment**)&environment);
        Boolean res;
        if (environment->IsExternalStorageEmulated(&res), res) {
            // Include emulated storage when measuring internal. OBB is
            // shared on emulated storage, so treat as code.
            Int64 externalCodeSize, externalObbSize, externalDataSize, externalMediaSize, externalCacheSize;
            stats->GetExternalCodeSize(&externalCodeSize);
            stats->GetExternalObbSize(&externalObbSize);
            codeSize += externalCodeSize + externalObbSize;

            stats->GetExternalDataSize(&externalDataSize);
            stats->GetExternalMediaSize(&externalMediaSize);
            dataSize += externalDataSize + externalMediaSize;

            stats->GetExternalCacheSize(&externalCacheSize);
            cacheSize += externalCacheSize;
        }

        // Count code and data for current user
        Int32 userHandle;
        stats->GetUserHandle(&userHandle);
        if (userHandle == mCurrentUser) {
            mDetails->mAppsSize += codeSize;
            mDetails->mAppsSize += dataSize;
        }

        // User summary only includes data (code is only counted once
        // for the current user)
        AddValue(mDetails->mUsersSize, userHandle, dataSize);

        // Include cache for all users
        mDetails->mCacheSize += cacheSize;

    }
    else {
        // Physical storage; only count external sizes
        Int64 externalCodeSize, externalDataSize, externalMediaSize, externalObbSize, externalCacheSize;
        stats->GetExternalCodeSize(&externalCodeSize);
        stats->GetExternalDataSize(&externalDataSize);
        stats->GetExternalMediaSize(&externalMediaSize);
        stats->GetExternalObbSize(&externalObbSize);
        stats->GetExternalCacheSize(&externalCacheSize);

        mDetails->mAppsSize += externalCodeSize + externalDataSize + externalMediaSize + externalObbSize;
        mDetails->mCacheSize += externalCacheSize;
    }
}

//===============================================================================
//                  StorageMeasurement::FileInfo
//===============================================================================

CAR_INTERFACE_IMPL(StorageMeasurement::FileInfo, Object, IComparable)

StorageMeasurement::FileInfo::FileInfo(
    /* [in] */ const String& fileName,
    /* [in] */ Int64 size,
    /* [in] */ Int64 id)
{
    mFileName = fileName;
    mSize = size;
    mId = id;
}

ECode StorageMeasurement::FileInfo::CompareTo(
    /* [in] */ IInterface* obj,// FileInfo that
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    FileInfo* that = (FileInfo*)IObject::Probe(obj);
    if (this == that || mSize == that->mSize) {
        *result = 0;
        return NOERROR;
    }
    else {
        *result = (mSize < that->mSize) ? 1 : -1; // for descending sort
        return NOERROR;
    }
}

ECode StorageMeasurement::FileInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder builder("");
    builder += mFileName;
    builder += " : ";
    builder += mSize;
    builder += ", id:";
    builder += mId;
    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  StorageMeasurement::MeasurementHandler::InitServiceConnection
//===============================================================================

CAR_INTERFACE_IMPL(StorageMeasurement::MeasurementHandler::InitServiceConnection, Object, IServiceConnection)

StorageMeasurement::MeasurementHandler::InitServiceConnection::InitServiceConnection(
    /* [in] */ MeasurementHandler* host)
    : mHost(host)
{}

ECode StorageMeasurement::MeasurementHandler::InitServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    AutoPtr<IIMediaContainerService> imcs = IIMediaContainerService::Probe(service);
    mHost->mDefaultContainer = imcs;
    mHost->mBound = TRUE;
    AutoPtr<IMessage> message;
    mHost->ObtainMessage(MSG_CONNECTED, imcs, (IMessage**)&message);
    Boolean res;
    return mHost->SendMessage(message, &res);
}

ECode StorageMeasurement::MeasurementHandler::InitServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mHost->mBound = FALSE;
    return mHost->RemoveMessages(MSG_CONNECTED);
}

//===============================================================================
//                  StorageMeasurement::MeasurementHandler
//===============================================================================

const Int32 StorageMeasurement::MeasurementHandler::MSG_MEASURE = 1;
const Int32 StorageMeasurement::MeasurementHandler::MSG_CONNECTED = 2;
const Int32 StorageMeasurement::MeasurementHandler::MSG_DISCONNECT = 3;
const Int32 StorageMeasurement::MeasurementHandler::MSG_COMPLETED = 4;
const Int32 StorageMeasurement::MeasurementHandler::MSG_INVALIDATE = 5;

StorageMeasurement::MeasurementHandler::MeasurementHandler(
    /* [in] */ StorageMeasurement* host)
    : mBound(FALSE)
    , mHost(host)
{}

ECode StorageMeasurement::MeasurementHandler::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ILooper* looper)
{
    Handler::constructor(looper);

    mDefContainerConn = new InitServiceConnection(this);

    // mContext = new WeakReference<Context>(context);
    IWeakReferenceSource::Probe(context)->GetWeakReference((IWeakReference**)&mContext);
    return NOERROR;
}

ECode StorageMeasurement::MeasurementHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_MEASURE: {
            if (mCached != NULL) {
                mHost->SendExactUpdate(mCached);
                break;
            }

            // final Context context = (mContext != NULL) ? mContext->Get() : NULL;
            AutoPtr<IContext> context;
            if (mContext != NULL) {
                mContext->Resolve(EIID_IContext, (IInterface**)&context);
            }
            if (context == NULL) {
                return NOERROR;
            }

            {
                AutoLock syncLock(mLock);
                if (mBound) {
                    RemoveMessages(MSG_DISCONNECT);
                    AutoPtr<IMessage> message;
                    ObtainMessage(MSG_CONNECTED, mDefaultContainer, (IMessage**)&message);
                    Boolean res;
                    SendMessage(message, &res);
                }
                else {
                    AutoPtr<IIntent> service;
                    CIntent::New((IIntent**)&service);
                    service->SetComponent(DEFAULT_CONTAINER_COMPONENT);
                    Boolean res;
                    context->BindServiceAsUser(service, mDefContainerConn,
                            IContext::BIND_AUTO_CREATE, UserHandle::OWNER, &res);
                }
            }
            break;
        }
        case MSG_CONNECTED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IIMediaContainerService* imcs = IIMediaContainerService::Probe(obj);
            MeasureApproximateStorage(imcs);
            MeasureExactStorage(imcs);
            break;
        }
        case MSG_DISCONNECT: {
            {
                AutoLock syncLock(mLock);
                if (mBound) {
                    // final Context context = (mContext != NULL) ? mContext->Get() : NULL;
                    AutoPtr<IContext> context;
                    if (mContext != NULL) {
                        mContext->Resolve(EIID_IContext, (IInterface**)&context);
                    }
                    if (context == NULL) {
                        return NOERROR;
                    }

                    mBound = FALSE;
                    context->UnbindService(mDefContainerConn);
                }
            }
            break;
        }
        case MSG_COMPLETED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mCached = IMeasurementDetails::Probe(obj);
            mHost->SendExactUpdate(mCached);
            break;
        }
        case MSG_INVALIDATE: {
            mCached = NULL;
            break;
        }
    }
    return NOERROR;
}

void StorageMeasurement::MeasurementHandler::MeasureApproximateStorage(
    /* [in] */ IIMediaContainerService* imcs)
{
    String path;
    if (mHost->mVolume != NULL) {
        mHost->mVolume->GetPath(&path);
    }
    else {
        AutoPtr<IEnvironment> environment;
        CEnvironment::AcquireSingleton((IEnvironment**)&environment);
        AutoPtr<IFile> file;
        environment->GetDataDirectory((IFile**)&file);
        file->GetPath(&path);
    }
    // try {
    AutoPtr< ArrayOf<Int64> > stats;
    imcs->GetFileSystemStats(path, (ArrayOf<Int64>**)&stats);
    mHost->mTotalSize = (*stats)[0];
    mHost->mAvailSize = (*stats)[1];
    // } catch (Exception e) {
    //     Logger::W(TAG, "Problem in container service", e);
    // }

    mHost->SendInternalApproximateUpdate();
}

void StorageMeasurement::MeasurementHandler::MeasureExactStorage(
    /* [in] */ IIMediaContainerService* imcs)
{
    // final Context context = mContext != NULL ? mContext->Get() : NULL;
    AutoPtr<IContext> context;
    if (mContext != NULL) {
        mContext->Resolve(EIID_IContext, (IInterface**)&context);
    }
    if (context == NULL) {
        return;
    }

    AutoPtr<MeasurementDetails> details = new MeasurementDetails();
    AutoPtr<IMessage> finished;
    ObtainMessage(MSG_COMPLETED, (IMeasurementDetails*)details, (IMessage**)&finished);

    details->mTotalSize = mHost->mTotalSize;
    details->mAvailSize = mHost->mAvailSize;

    AutoPtr<IInterface> userManagerTmp;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&userManagerTmp);
    IUserManager* userManager = IUserManager::Probe(userManagerTmp);
    AutoPtr<IList> users;//List<UserInfo>
    userManager->GetUsers((IList**)&users);

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 currentUser;
    helper->GetCurrentUser(&currentUser);
    AutoPtr<IUserEnvironment> currentEnv;
    CUserEnvironment::New(currentUser, (IUserEnvironment**)&currentEnv);

    // Measure media types for emulated storage, or for primary physical
    // external volume
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    Boolean res;
    const Boolean measureMedia = (mHost->mIsInternal && (environment->IsExternalStorageEmulated(&res), res))
            || mHost->mIsPrimary;
    if (measureMedia) {
        AutoPtr<IIterator> iterator;
        sMeasureMediaTypes->GetIterator((IIterator**)&iterator);
        Boolean hasNext;
        while (iterator->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iterator->GetNext((IInterface**)&obj);
            String type = CoreUtils::Unbox(ICharSequence::Probe(obj));

            AutoPtr<IFile> path;
            currentEnv->GetExternalStoragePublicDirectory(type, (IFile**)&path);
            Int64 size = GetDirectorySize(imcs, path);
            details->mMediaSize->Put(CoreUtils::Convert(type), CoreUtils::Convert(size));
        }
    }

    // Measure misc files not counted under media
    if (measureMedia) {
        AutoPtr<IFile> path;
        if (mHost->mIsInternal) {
            currentEnv->GetExternalStorageDirectory((IFile**)&path);
        }
        else {
            mHost->mVolume->GetPathFile((IFile**)&path);
        }
        details->mMiscSize = mHost->MeasureMisc(imcs, path);
    }

    // Measure total emulated storage of all users; internal apps data
    // will be spliced in later
    Int32 len;
    users->GetSize(&len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        IUserInfo* user = IUserInfo::Probe(obj);

        Int32 id;
        user->GetId(&id);
        AutoPtr<IUserEnvironment> userEnv;
        CUserEnvironment::New(id, (IUserEnvironment**)&userEnv);
        AutoPtr<IFile> file;
        userEnv->GetExternalStorageDirectory((IFile**)&file);
        Int64 size = GetDirectorySize(imcs, file);
        AddValue(details->mUsersSize, id, size);
    }

    // Measure all apps for all users
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    if (mHost->mIsInternal || mHost->mIsPrimary) {
        AutoPtr<IList> apps;//List<ApplicationInfo>
        pm->GetInstalledApplications(IPackageManager::GET_UNINSTALLED_PACKAGES
                | IPackageManager::GET_DISABLED_COMPONENTS, (IList**)&apps);

        Int32 length;
        apps->GetSize(&length);
        const Int32 count = len * length;
        AutoPtr<IIPackageStatsObserver> observer;
        CStorageMeasurementStatsObserver::New(mHost->mIsInternal, (IMeasurementDetails*)details,
                currentUser, finished, count, (IIPackageStatsObserver**)&observer);

        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> obj;
            users->Get(i, (IInterface**)&obj);
            IUserInfo* user = IUserInfo::Probe(obj);

            for (Int32 j = 0; j < length; j++) {
                AutoPtr<IInterface> tmp;
                apps->Get(j, (IInterface**)&tmp);
                IApplicationInfo* app = IApplicationInfo::Probe(tmp);

                String packageName;
                IPackageItemInfo::Probe(app)->GetPackageName(&packageName);
                Int32 id;
                user->GetId(&id);
                pm->GetPackageSizeInfo(packageName, id, observer);
            }
        }

    }
    else {
        finished->SendToTarget();
    }
}

//===============================================================================
//                  StorageMeasurement
//===============================================================================

const Boolean StorageMeasurement::LOGV = LOCAL_LOGV && Logger::IsLoggable(TAG, Logger::VERBOSE);

static AutoPtr<IComponentName> InitDEFAULT_CONTAINER_COMPONENT()
{
    AutoPtr<IComponentName> comp;
    CComponentName::New(String("Elastos.Droid.DefContainer")/*DEFAULT_CONTAINER_PACKAGE*/,
            String("Elastos.Droid.DefContainer.CDefaultContainerService"), (IComponentName**)&comp);
    return comp;
}

const AutoPtr<IComponentName> StorageMeasurement::DEFAULT_CONTAINER_COMPONENT = InitDEFAULT_CONTAINER_COMPONENT();

const String StorageMeasurement::TAG("StorageMeasurement");

const Boolean StorageMeasurement::LOCAL_LOGV = TRUE;

const String StorageMeasurement::DEFAULT_CONTAINER_PACKAGE("Elastos.Droid.DefContainer");//"com.android.defcontainer"

static AutoPtr<ISet> InitsMeasureMediaTypes()
{
    AutoPtr< ArrayOf<IInterface*> > elements = ArrayOf<IInterface*>::Alloc(10);
    elements->Set(0, CoreUtils::Convert(IEnvironment::DIRECTORY_DCIM));
    elements->Set(1, CoreUtils::Convert(IEnvironment::DIRECTORY_MOVIES));
    elements->Set(2, CoreUtils::Convert(IEnvironment::DIRECTORY_PICTURES));
    elements->Set(3, CoreUtils::Convert(IEnvironment::DIRECTORY_MUSIC));
    elements->Set(4, CoreUtils::Convert(IEnvironment::DIRECTORY_ALARMS));
    elements->Set(5, CoreUtils::Convert(IEnvironment::DIRECTORY_NOTIFICATIONS));
    elements->Set(6, CoreUtils::Convert(IEnvironment::DIRECTORY_RINGTONES));
    elements->Set(7, CoreUtils::Convert(IEnvironment::DIRECTORY_PODCASTS));
    elements->Set(8, CoreUtils::Convert(IEnvironment::DIRECTORY_DOWNLOADS));
    elements->Set(9, CoreUtils::Convert(IEnvironment::DIRECTORY_ANDROID));

    Int32 capacity = elements->GetLength() * 4 / 3 + 1;

    AutoPtr<ISet> set;
    CHashSet::New(capacity, (ISet**)&set);
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->AddAll(ICollection::Probe(set), elements);
    return set;
}

const AutoPtr<ISet> StorageMeasurement::sMeasureMediaTypes = InitsMeasureMediaTypes();

static AutoPtr<IHashMap> InitsInstances()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

AutoPtr<IHashMap> StorageMeasurement::sInstances = InitsInstances();

CAR_INTERFACE_IMPL(StorageMeasurement, Object, IStorageMeasurement);

AutoPtr<StorageMeasurement> StorageMeasurement::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ IStorageVolume* volume)
{
    {
        AutoLock syncLock(sInstances);
        AutoPtr<IInterface> obj;
        sInstances->Get(volume, (IInterface**)&obj);
        AutoPtr<StorageMeasurement> value = (StorageMeasurement*) IStorageMeasurement::Probe(obj);
        if (value == NULL) {
            AutoPtr<IContext> ctx;
            context->GetApplicationContext((IContext**)&ctx);
            value = new StorageMeasurement(ctx, volume);
            sInstances->Put(volume, (IStorageMeasurement*)value);
        }
        return value;
    }
}

StorageMeasurement::StorageMeasurement(
    /* [in] */ IContext* context,
    /* [in] */ IStorageVolume* volume)
    : mIsInternal(FALSE)
    , mIsPrimary(FALSE)
    , mTotalSize(0)
    , mAvailSize(0)
{
    mVolume = volume;
    mIsInternal = volume == NULL;
    Boolean res;
    mIsPrimary = volume != NULL ? (volume->IsPrimary(&res), res) : FALSE;

    // Start the thread that will measure the disk usage.
    AutoPtr<IHandlerThread> handlerThread;
    CHandlerThread::New(String("MemoryMeasurement"), (IHandlerThread**)&handlerThread);
    IThread::Probe(handlerThread)->Start();
    AutoPtr<ILooper> looper;
    handlerThread->GetLooper((ILooper**)&looper);
    mHandler = new MeasurementHandler(this);
    mHandler->constructor(context, looper);
}

ECode StorageMeasurement::SetReceiver(
    /* [in] */ IStorageMeasurementReceiver* receiver)
{
    AutoPtr<IStorageMeasurementReceiver> obj;
    if (mReceiver == NULL || (mReceiver->Resolve(EIID_IStorageMeasurementReceiver, (IInterface**)&obj), obj == NULL)) {
        // mReceiver = new WeakReference<IStorageMeasurementReceiver>(receiver);
        IWeakReferenceSource::Probe(receiver)->GetWeakReference((IWeakReference**)&mReceiver);
    }
    return NOERROR;
}

ECode StorageMeasurement::Measure()
{
    Boolean res;
    if (mHandler->HasMessages(MeasurementHandler::MSG_MEASURE, &res), !res) {
        mHandler->SendEmptyMessage(MeasurementHandler::MSG_MEASURE, &res);
    }
    return NOERROR;
}

ECode StorageMeasurement::CleanUp()
{
    mReceiver = NULL;
    mHandler->RemoveMessages(MeasurementHandler::MSG_MEASURE);
    Boolean res;
    return mHandler->SendEmptyMessage(MeasurementHandler::MSG_DISCONNECT, &res);
}

ECode StorageMeasurement::Invalidate()
{
    Boolean res;
    return mHandler->SendEmptyMessage(MeasurementHandler::MSG_INVALIDATE, &res);
}

void StorageMeasurement::SendInternalApproximateUpdate()
{
    // AutoPtr<IStorageMeasurementReceiver> receiver = (mReceiver != NULL) ? mReceiver->Get() : NULL;
    AutoPtr<IStorageMeasurementReceiver> receiver;
    if (mReceiver != NULL) {
        mReceiver->Resolve(EIID_IStorageMeasurementReceiver, (IInterface**)&receiver);
    }
    if (receiver == NULL) {
        return;
    }
    receiver->UpdateApproximate(this, mTotalSize, mAvailSize);
}

void StorageMeasurement::SendExactUpdate(
    /* [in] */ IMeasurementDetails* details)
{
    // AutoPtr<IIStorageMeasurementReceiver> receiver = (mReceiver != NULL) ? mReceiver->Get() : NULL;
    AutoPtr<IStorageMeasurementReceiver> receiver;
    if (mReceiver != NULL) {
        mReceiver->Resolve(EIID_IStorageMeasurementReceiver, (IInterface**)&receiver);
    }
    if (receiver == NULL) {
        if (LOGV) {
            Logger::I(TAG, "measurements dropped because receiver is NULL! wasted effort");
        }
        return;
    }
    receiver->UpdateDetails(this, details);
}

Int64 StorageMeasurement::GetDirectorySize(
    /* [in] */ IIMediaContainerService* imcs,
    /* [in] */ IFile* path)
{
    String str;
    path->ToString(&str);
    Int64 size;
    ECode ec = imcs->CalculateDirectorySize(str, &size);
    if (FAILED(ec)) {
        Logger::W(TAG, "Could not read memory from default container service for %s 0x%08x", TO_CSTR(path), ec);
        return 0;
    }
    Logger::D(TAG, "GetDirectorySize( %s) returned %ld", TO_CSTR(path), size);
    return size;
}

Int64 StorageMeasurement::MeasureMisc(
    /* [in] */ IIMediaContainerService* imcs,
    /* [in] */ IFile* dir)
{
    CArrayList::New((IList**)&mFileInfoForMisc);

    AutoPtr< ArrayOf<IFile*> > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);
    if (files == NULL) return 0;

    // Get sizes of all top level nodes except the ones already computed
    Int64 counter = 0;
    Int64 miscSize = 0;

    for (Int32 i = 0; i < files->GetLength(); i++) {
        AutoPtr<IFile> file = (*files)[i];

        String path;
        file->GetAbsolutePath(&path);
        String name;
        file->GetName(&name);
        Boolean res;
        if (sMeasureMediaTypes->Contains(CoreUtils::Convert(name), &res), res) {
            continue;
        }

        if (file->IsFile(&res), res) {
            Int64 fileSize;
            file->GetLength(&fileSize);
            AutoPtr<FileInfo> info = new FileInfo(path, fileSize, counter++);
            mFileInfoForMisc->Add((IObject*)info);
            miscSize += fileSize;
        }
        else if (file->IsDirectory(&res), res) {
            Int64 dirSize = GetDirectorySize(imcs, file);
            AutoPtr<FileInfo> info = new FileInfo(path, dirSize, counter++);
            mFileInfoForMisc->Add((IObject*)info);
            miscSize += dirSize;
        }
        else {
            // Non directory, non file: not listed
        }
    }

    // sort the list of FileInfo objects collected above in descending order of their sizes
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(mFileInfoForMisc);

    return miscSize;
}

void StorageMeasurement::AddValue(
    /* [in] */ ISparseInt64Array* array,
    /* [in] */ Int32 key,
    /* [in] */ Int64 value)
{
    Int64 data;
    array->Get(key, &data);
    array->Put(key, data + value);
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos
