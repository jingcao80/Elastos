//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_STORAGEMEASUREMENT_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_STORAGEMEASUREMENT_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/os/Handler.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IIPackageStatsObserver;
using Elastos::Droid::Content::Pm::IPackageStats;
using Elastos::Droid::Internal::App::IIMediaContainerService;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Utility::ISparseInt64Array;
using Elastos::Core::IComparable;
using Elastos::IO::IFile;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * Utility for measuring the disk usage of internal storage or a physical
 * {@link StorageVolume}. Connects with a remote {@link IMediaContainerService}
 * and delivers results to {@link MeasurementReceiver}.
 */
class StorageMeasurement
    : public Object
    , public IStorageMeasurement
{
public:
    class MeasurementDetails
        : public Object
        , public IMeasurementDetails
    {
    public:
        TO_STRING_IMPL("StorageMeasurement::MeasurementDetails")

        CAR_INTERFACE_DECL()

        MeasurementDetails();

    public:
        Int64 mTotalSize;
        Int64 mAvailSize;

        /**
         * Total apps disk usage.
         * <p>
         * When measuring internal storage, this value includes the code size of
         * all apps (regardless of install status for current user), and
         * internal disk used by the current user's apps. When the device
         * emulates external storage, this value also includes emulated storage
         * used by the current user's apps.
         * <p>
         * When measuring a physical {@link StorageVolume}, this value includes
         * usage by all apps on that volume.
         */
        Int64 mAppsSize;

        /**
         * Total cache disk usage by apps.
         */
        Int64 mCacheSize;

        /**
         * Total media disk usage, categorized by types such as
         * {@link Environment#DIRECTORY_MUSIC}.
         * <p>
         * When measuring internal storage, this reflects media on emulated
         * storage for the current user.
         * <p>
         * When measuring a physical {@link StorageVolume}, this reflects media
         * on that volume.
         */
        AutoPtr<IHashMap> mMediaSize; // HashMap<String, Long> mediaSize

        /**
         * Misc external disk usage for the current user, unaccounted in
         * {@link #mediaSize}.
         */
        Int64 mMiscSize;

        /**
         * Total disk usage for users, which is only meaningful for emulated
         * internal storage. Key is {@link UserHandle}.
         */
        AutoPtr<ISparseInt64Array> mUsersSize;
    };

    class StatsObserver
        : public Object
        , public IIPackageStatsObserver
        , public IBinder
    {
    public:
        TO_STRING_IMPL("StorageMeasurement::StatsObserver")

        CAR_INTERFACE_DECL()

        StatsObserver();

        CARAPI constructor(
            /* [in] */ Boolean isInternal,
            /* [in] */ IMeasurementDetails* details, // MeasurementDetails
            /* [in] */ Int32 currentUser,
            /* [in] */ IMessage* finished,
            /* [in] */ Int32 remaining);

        //@Override
        CARAPI OnGetStatsCompleted(
            /* [in] */ IPackageStats* stats,
            /* [in] */ Boolean succeeded);
    private:
        CARAPI_(void) AddStatsLocked(
            /* [in] */ IPackageStats* stats);

    private:
        Boolean mIsInternal;
        AutoPtr<MeasurementDetails> mDetails;
        Int32 mCurrentUser;
        AutoPtr<IMessage> mFinished;

        Int32 mRemaining;
    };

    class FileInfo
        : public Object
        , public IComparable
    {
    public:
        CAR_INTERFACE_DECL()

        FileInfo(
            /* [in] */ const String& fileName,
            /* [in] */ Int64 size,
            /* [in] */ Int64 id);

        //@Override
        CARAPI CompareTo(
            /* [in] */ IInterface* obj,// FileInfo that
            /* [out] */ Int32* result);

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        String mFileName;
        Int64 mSize;
        Int64 mId;
    };

private:
    class MeasurementHandler
        : public Handler
    {
    private:
        class InitServiceConnection
            : public Object
            , public IServiceConnection
        {
        public:
            TO_STRING_IMPL("StorageMeasurement::MeasurementHandler::InitServiceConnection")

            CAR_INTERFACE_DECL()

            InitServiceConnection(
                /* [in] */ MeasurementHandler* host);

            //@Override
            CARAPI OnServiceConnected(
                /* [in] */ IComponentName* name,
                /* [in] */ IBinder* service);

            //@Override
            CARAPI OnServiceDisconnected(
                /* [in] */ IComponentName* name);

        private:
            MeasurementHandler* mHost;
        };

    public:
        TO_STRING_IMPL("StorageMeasurement::MeasurementHandler")

        MeasurementHandler(
            /* [in] */ StorageMeasurement* host);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ ILooper* looper);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) MeasureApproximateStorage(
            /* [in] */ IIMediaContainerService* imcs);

        CARAPI_(void) MeasureExactStorage(
            /* [in] */ IIMediaContainerService* imcs);

    public:
        static const Int32 MSG_MEASURE;
        static const Int32 MSG_CONNECTED;
        static const Int32 MSG_DISCONNECT;
        static const Int32 MSG_COMPLETED;
        static const Int32 MSG_INVALIDATE;

    private:
        Object mLock;

        AutoPtr<IIMediaContainerService> mDefaultContainer;

        volatile Boolean mBound;

        AutoPtr<IMeasurementDetails> mCached;

        AutoPtr<IWeakReference> mContext;//WeakReference<Context>

        AutoPtr<IServiceConnection> mDefContainerConn;
        StorageMeasurement* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Obtain shared instance of {@link StorageMeasurement} for given physical
     * {@link StorageVolume}, or internal storage if {@code NULL}.
     */
    static CARAPI_(AutoPtr<StorageMeasurement>) GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ IStorageVolume* volume);

    CARAPI SetReceiver(
        /* [in] */ IStorageMeasurementReceiver* receiver);

    CARAPI Measure();

    CARAPI CleanUp();

    CARAPI Invalidate();

private:
    StorageMeasurement(
        /* [in] */ IContext* context,
        /* [in] */ IStorageVolume* volume);

    CARAPI_(void) SendInternalApproximateUpdate();

    CARAPI_(void) SendExactUpdate(
        /* [in] */ IMeasurementDetails* details);

    static CARAPI_(Int64) GetDirectorySize(
        /* [in] */ IIMediaContainerService* imcs,
        /* [in] */ IFile* path);

    CARAPI_(Int64) MeasureMisc(
        /* [in] */ IIMediaContainerService* imcs,
        /* [in] */ IFile* dir);

    static CARAPI_(void) AddValue(
        /* [in] */ ISparseInt64Array* array,
        /* [in] */ Int32 key,
        /* [in] */ Int64 value);

public:
    static const Boolean LOGV;
    static const AutoPtr<IComponentName> DEFAULT_CONTAINER_COMPONENT;
    AutoPtr<IList> /*List<FileInfo>*/ mFileInfoForMisc;

private:
    static const String TAG;

    static const Boolean LOCAL_LOGV;

    static const String DEFAULT_CONTAINER_PACKAGE;

    /** Media types to measure on external storage. */
    static const AutoPtr<ISet> sMeasureMediaTypes; //Set<String>

    // @GuardedBy("sInstances")
    static AutoPtr<IHashMap> sInstances; /*HashMap<StorageVolume, StorageMeasurement>*/

    /*volatile*/ AutoPtr<IWeakReference> mReceiver; // WeakReference<IStorageMeasurementReceiver>

    /** Physical volume being measured, or {@code NULL} for internal. */
    AutoPtr<IStorageVolume> mVolume;

    Boolean mIsInternal;
    Boolean mIsPrimary;

    AutoPtr<MeasurementHandler> mHandler;

    Int64 mTotalSize;
    Int64 mAvailSize;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_STORAGEMEASUREMENT_H__