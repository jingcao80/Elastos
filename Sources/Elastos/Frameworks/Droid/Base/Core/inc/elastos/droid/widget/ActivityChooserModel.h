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

#ifndef __ELASTOS_DROID_WIDGET_ACTIVITYCHOOSERMODEL_H__
#define __ELASTOS_DROID_WIDGET_ACTIVITYCHOOSERMODEL_H__

#include "elastos/droid/database/DataSetObservable.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::DataSetObservable;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Widget::IActivitySorter;
using Elastos::Droid::Widget::IHistoricalRecord;
using Elastos::Core::IComparable;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Widget {

class ActivityChooserModel
    : public DataSetObservable
    , public IActivityChooserModel
{
public:
    class HistoricalRecord
        : public Object
        , public IHistoricalRecord
    {
    public:
        CAR_INTERFACE_DECL()

        HistoricalRecord(
            /* [in] */ const String& activityName,
            /* [in] */ Int64 time,
            /* [in] */ Float weight);

        HistoricalRecord(
            /* [in] */ IComponentName* activityName,
            /* [in] */ Int64 time,
            /* [in] */ Float weight);

        CARAPI GetHashCode(
            /* [out] */ Int32* code);

        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* res);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<IComponentName> mActivity;
        Int64 mTime;
        Float mWeight;
    };

    class ActivityResolveInfo
        : public Object
        , public IActivityResolveInfo
    {
        friend class ActivityChooserModel;

    public:
        CAR_INTERFACE_DECL()

        ActivityResolveInfo(
            /* [in] */ IResolveInfo* resolveInfo,
            /* [in] */ ActivityChooserModel* host);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* rst);

        CARAPI CompareTo(
            /* [in] */ IInterface* other,
            /* [out] */ Int32* rst);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        ActivityChooserModel* mHost;
        AutoPtr<IResolveInfo> mResolveInfo;
        Float mWeight;
    };

    class DefaultSorter
        : public Object
        , public IActivitySorter
    {
    public:
        CAR_INTERFACE_DECL()

        DefaultSorter(
            /* [in] */ ActivityChooserModel* host);

        CARAPI Sort(
            /* [in] */ IIntent* intent,
            /* [in] */ IList* activities,
            /* [in] */ IList* historicalRecords);

    private:
        ActivityChooserModel* mHost;
        HashMap< AutoPtr<IComponentName>, AutoPtr<ActivityResolveInfo> > mPackageNameToActivityMap;
        static const Float WEIGHT_DECAY_COEFFICIENT;
    };

    class PersistHistoryAsyncTask
        : public AsyncTask
    {
    public:
        PersistHistoryAsyncTask(
            /* [in] */ ActivityChooserModel* host);

        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* args,
            /* [out] */ IInterface** result);

    private:
        ActivityChooserModel* mHost;
    };

    class DataModelPackageMonitor
        : public PackageMonitor
    {
    public:
        DataModelPackageMonitor(
            /* [in] */ ActivityChooserModel* host);

        CARAPI OnSomePackagesChanged();

    private:
        ActivityChooserModel* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IActivityChooserModel>) Get(
        /* [in] */ IContext* context,
        /* [in] */ const String& historyFileName);

    virtual ~ActivityChooserModel();

    CARAPI SetIntent(
        /* [in] */ IIntent* intent);

    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    CARAPI GetActivityCount(
        /* [out] */ Int32* rst);

    CARAPI GetActivity(
        /* [in] */ Int32 index,
        /* [out] */ IResolveInfo** activity);

    CARAPI GetActivityIndex(
        /* [in] */ IResolveInfo* activity,
        /* [out] */ Int32* rst);

    CARAPI ChooseActivity(
        /* [in] */ Int32 index,
        /* [out] */ IIntent** intent);

    CARAPI SetOnChooseActivityListener(
        /* [in] */ IOnChooseActivityListener* listener);

    CARAPI GetDefaultActivity(
        /* [out] */ IResolveInfo** activity);

    CARAPI SetDefaultActivity(
        /* [in] */ Int32 index);

    CARAPI SetActivitySorter(
        /* [in] */ IActivitySorter* activitySorter);

    CARAPI SetHistoryMaxSize(
        /* [in] */ Int32 historyMaxSize);

    CARAPI GetHistoryMaxSize(
        /*[out] */ Int32* size);

    CARAPI GetHistorySize(
        /*[out] */ Int32* size);

    CARAPI RegisterObserver(
        /* [in] */ IInterface* observer);

    CARAPI UnregisterObserver(
        /* [in] */ IInterface* observer);

    CARAPI UnregisterAll();

    CARAPI NotifyChanged();

    CARAPI NotifyInvalidated();

private:
    ActivityChooserModel(
        /* [in] */ IContext* context,
        /* [in] */ const String& historyFileName);

    CARAPI PersistHistoricalDataIfNeeded();

    CARAPI EnsureConsistentState();

    CARAPI_(Boolean) SortActivitiesIfNeeded();

    CARAPI_(Boolean) LoadActivitiesIfNeeded();

    CARAPI_(Boolean) ReadHistoricalDataIfNeeded();

    CARAPI_(Boolean) AddHisoricalRecord(
        /* [in] */ IHistoricalRecord* historicalRecord);

    CARAPI PruneExcessiveHistoricalRecordsIfNeeded();

    CARAPI ReadHistoricalDataImpl();

    template<typename T>
    AutoPtr<IList> TransfromList(
        /* [in] */ List< AutoPtr<T> >& list);

    template<typename T, typename IT>
    List< AutoPtr<T> > TransfromContainer(
        /* [in] */ IList* list);

    static CARAPI_(Int32) FloatToIntBits(
        /* [in] */ Float value);

private:
    static const Boolean DEBUG;
    static const String TAG;
    static const String TAG_HISTORICAL_RECORDS;
    static const String TAG_HISTORICAL_RECORD;
    static const String ATTRIBUTE_ACTIVITY;
    static const String ATTRIBUTE_TIME;
    static const String ATTRIBUTE_WEIGHT;
    static const String DEFAULT_HISTORY_FILE_NAME;
    static const Int32 DEFAULT_HISTORY_MAX_LENGTH;
    static const Int32 DEFAULT_ACTIVITY_INFLATION;
    static const Float DEFAULT_HISTORICAL_RECORD_WEIGHT;
    static const String HISTORY_FILE_EXTENSION;
    static const Int32 INVALID_INDEX;
    static Object sRegistryLock;
    static HashMap<String, AutoPtr<IActivityChooserModel> > sDataModelRegistry;
    Object mInstanceLock;
    List<AutoPtr<ActivityResolveInfo> > mActivities;
    List<AutoPtr<HistoricalRecord> > mHistoricalRecords;
    AutoPtr<PackageMonitor> mPackageMonitor;
    AutoPtr<IContext> mContext;
    String mHistoryFileName;
    AutoPtr<IIntent> mIntent;
    AutoPtr<DefaultSorter> mActivitySorter;
    Int32 mHistoryMaxSize;
    Boolean mCanReadHistoricalData;
    Boolean mReadShareHistoryCalled;
    Boolean mHistoricalRecordsChanged;
    Boolean mReloadActivities;
    AutoPtr<IOnChooseActivityListener> mActivityChoserModelPolicy;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ACTIVITYCHOOSERMODEL_H__

