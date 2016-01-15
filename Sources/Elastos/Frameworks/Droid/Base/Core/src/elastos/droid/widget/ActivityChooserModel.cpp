#define HASH_FOR_CONTENT
#include "elastos/droid/ext/frameworkhash.h"
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/widget/ActivityChooserModel.h"
#include "elastos/core/AutoLock.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Database::EIID_IDataSetObservable;
using Elastos::Droid::Database::EIID_IObservable;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Widget::EIID_IHistoricalRecord;
using Elastos::Droid::Widget::IActivitySorter;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::Math::CBigDecimal;
using Elastos::Math::IBigDecimal;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;
using Org::Kxml2::IO::CKXmlParser;
using Org::Kxml2::IO::CKXmlSerializer;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Widget {

const Boolean ActivityChooserModel::DEBUG = FALSE;
const String ActivityChooserModel::TAG("ActivityChooserModel");
const String ActivityChooserModel::TAG_HISTORICAL_RECORDS("historical-records");
const String ActivityChooserModel::TAG_HISTORICAL_RECORD("historical-record");
const String ActivityChooserModel::ATTRIBUTE_ACTIVITY("activity");
const String ActivityChooserModel::ATTRIBUTE_TIME("time");
const String ActivityChooserModel::ATTRIBUTE_WEIGHT("weight");
const String ActivityChooserModel::DEFAULT_HISTORY_FILE_NAME("activity_choser_model_history.xml");
const Int32 ActivityChooserModel::DEFAULT_HISTORY_MAX_LENGTH = 50;
const Int32 ActivityChooserModel::DEFAULT_ACTIVITY_INFLATION = 5;
const Float ActivityChooserModel::DEFAULT_HISTORICAL_RECORD_WEIGHT = 1.0f;
const String ActivityChooserModel::HISTORY_FILE_EXTENSION(".xml");
const Int32 ActivityChooserModel::INVALID_INDEX = -1;
Object ActivityChooserModel::sRegistryLock;
HashMap<String, AutoPtr<IActivityChooserModel> > ActivityChooserModel::sDataModelRegistry;
const Float ActivityChooserModel::DefaultSorter::WEIGHT_DECAY_COEFFICIENT = 0.95f;

CAR_INTERFACE_IMPL(ActivityChooserModel::HistoricalRecord, Object, IHistoricalRecord)
CAR_INTERFACE_IMPL(ActivityChooserModel::ActivityResolveInfo, Object, IActivityResolveInfo)
CAR_INTERFACE_IMPL(ActivityChooserModel::DefaultSorter, Object, IActivitySorter)
CAR_INTERFACE_IMPL(ActivityChooserModel, Object, IActivityChooserModel)

ActivityChooserModel::HistoricalRecord::HistoricalRecord(
    /* [in] */ const String& activityName,
    /* [in] */ Int64 time,
    /* [in] */ Float weight)
{
    CComponentName::UnflattenFromString(activityName, (IComponentName**)&mActivity);
    mTime = time;
    mWeight = weight;
}

ActivityChooserModel::HistoricalRecord::HistoricalRecord(
    /* [in] */ IComponentName* activityName,
    /* [in] */ Int64 time,
    /* [in] */ Float weight)
{
    mActivity = activityName;
    mTime = time;
    mWeight = weight;
}

ECode ActivityChooserModel::HistoricalRecord::GetHashCode(
    /* [out] */ Int32* code)
{
    Int32 prime = 31;
    Int32 result = 1;
    Int32 activityHash = 0;
    if(mActivity != NULL) {
        IObject* objTmp = IObject::Probe(mActivity);
        objTmp->GetHashCode(&activityHash);
    }
    result = prime * result + activityHash;
    UInt64 tmp = *((UInt64*)(&mTime));
    result = prime * result + (Int32) (mTime ^ (tmp >> 32));
    result = prime * result + FloatToIntBits(mWeight);
    *code = result;
    return NOERROR;
}

ECode ActivityChooserModel::HistoricalRecord::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* res)
{
    if(TO_IINTERFACE(obj) == TO_IINTERFACE(this)) {
        return TRUE;
    }
    if (obj == NULL) {
        return FALSE;
    }
    if (IHistoricalRecord::Probe(obj) == NULL) {
        return FALSE;
    }

    IObject* objTmp = IObject::Probe(obj);
    HistoricalRecord* other = (HistoricalRecord*)objTmp;
    Boolean activityEquals = FALSE;
    if (mActivity == NULL) {
        if (other->mActivity != NULL) {
            return FALSE;
        }
    }
    else if (objTmp->Equals(TO_IINTERFACE(other->mActivity), &activityEquals), !activityEquals) {
        return FALSE;
    }
    if (mTime != other->mTime) {
        return FALSE;
    }

    if (FloatToIntBits(mWeight) != FloatToIntBits(other->mWeight)) {
        return FALSE;
    }
    return TRUE;
}

ECode ActivityChooserModel::HistoricalRecord::ToString(
    /* [out] */ String* str)
{
    StringBuilder builder;
    builder += "[";
    builder += "; activity:";
    String strBuf;
    mActivity->ToString(&strBuf);
    builder += strBuf;
    builder += "; time:";
    builder += mTime;
    builder += "; weight:";
    AutoPtr<IBigDecimal> bigDecimal;
    CBigDecimal::New(mWeight, (IBigDecimal**)&bigDecimal);
    IObject* objTmp = IObject::Probe(bigDecimal);
    objTmp->ToString(&strBuf);
    builder += strBuf;
    builder += "]";
    *str = builder.ToString();
    return NOERROR;
}

ActivityChooserModel::ActivityResolveInfo::ActivityResolveInfo(
    /* [in] */ IResolveInfo* resolveInfo,
    /* [in] */ ActivityChooserModel* host)
{
    mResolveInfo = resolveInfo;
    mHost = host;
}

ECode ActivityChooserModel::ActivityResolveInfo::GetHashCode(
    /* [out] */ Int32* hash)
{
    *hash = 31 + FloatToIntBits(mWeight);
    return NOERROR;
}

ECode ActivityChooserModel::ActivityResolveInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* rst)
{
    if (TO_IINTERFACE(obj) == TO_IINTERFACE(this)) {
        return TRUE;
    }
    if (obj == NULL) {
        return FALSE;
    }
    if (IActivityResolveInfo::Probe(obj) == NULL) {
        return FALSE;
    }
    ActivityResolveInfo* other = (ActivityResolveInfo*)(IActivityResolveInfo::Probe(obj));
    if (FloatToIntBits(mWeight) != FloatToIntBits(other->mWeight)) {
        return FALSE;
    }
    return TRUE;
}

ECode ActivityChooserModel::ActivityResolveInfo::CompareTo(
    /* [in] */ IInterface* obj,
    /* [out] */ Int32* rst)
{
    AutoPtr<IActivityResolveInfo> other = IActivityResolveInfo::Probe(obj);
    assert(other != NULL);
    ActivityResolveInfo* pTmp = (ActivityResolveInfo*)other.Get();
    *rst = Elastos::Core::Math::Compare(pTmp->mWeight, mWeight);
    return NOERROR;
}

ECode ActivityChooserModel::ActivityResolveInfo::ToString(
    /* [out] */ String* str)
{
    *str = "ActivityChooserModel::ActivityResolveInfo";
    return NOERROR;
}

ActivityChooserModel::DefaultSorter::DefaultSorter(
    /* [in] */ ActivityChooserModel* host)
    : mHost(host)
{
}

ECode ActivityChooserModel::DefaultSorter::Sort(
    /* [in] */ IIntent* intent,
    /* [in] */ IList* activities,
    /* [in] */ IList* historicalRecords)
{
    HashMap< AutoPtr<IComponentName>, AutoPtr<ActivityResolveInfo> > componentNameToActivityMap = mPackageNameToActivityMap;
    componentNameToActivityMap.Clear();

    Int32 activityCount = 0;
    activities->GetSize(&activityCount);

    String packageName;
    String name;
    for (int i = 0; i < activityCount; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        activities->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        ActivityResolveInfo* activity = (ActivityResolveInfo*)objTmp;
        activity->mWeight = 0.0f;

        AutoPtr<IActivityInfo> activityInfo;
        activity->mResolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        IPackageItemInfo* packageItemInfo = IPackageItemInfo::Probe(activityInfo);
        packageItemInfo->GetPackageName(&packageName);
        packageItemInfo->GetName(&name);

        AutoPtr<IComponentName> componentName;
        CComponentName::New(packageName, name, (IComponentName**)&componentName);
        componentNameToActivityMap[componentName] = activity;
    }

    Int32 historicalRecordSize = 0;
    historicalRecords->GetSize(&historicalRecordSize);
    Int32 lastShareIndex = historicalRecordSize - 1;
    float nextRecordWeight = 1;
    for (int i = lastShareIndex; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        historicalRecords->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        HistoricalRecord* historicalRecord = (HistoricalRecord*)objTmp;

        AutoPtr<IComponentName> componentName = historicalRecord->mActivity;
        AutoPtr<ActivityResolveInfo> activity = componentNameToActivityMap[componentName];
        if (activity != NULL) {
            activity->mWeight += historicalRecord->mWeight * nextRecordWeight;
            nextRecordWeight = nextRecordWeight * WEIGHT_DECAY_COEFFICIENT;
        }
    }

    AutoPtr<ICollections> connection;
    CCollections::AcquireSingleton((ICollections**)&connection);
    connection->Sort(activities);
    return NOERROR;
}

ActivityChooserModel::PersistHistoryAsyncTask::PersistHistoryAsyncTask(
    /* [in] */ ActivityChooserModel* host)
    : mHost(host)
{
}

ECode ActivityChooserModel::PersistHistoryAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(args);
    VALIDATE_NOT_NULL(result);

    AutoPtr<IList> historicalRecords = IList::Probe((*args)[0]);
    assert(historicalRecords != NULL);
    String historyFileName;
    AutoPtr<ICharSequence> csq = ICharSequence::Probe((*args)[1]);
    csq->ToString(&historyFileName);
    AutoPtr<IFileOutputStream> fos;

    // try {
        ECode ec = mHost->mContext->OpenFileOutput(historyFileName, IContext::MODE_PRIVATE, (IFileOutputStream**)&fos);
        if(FAILED(ec)) {
            *result = NULL;
            return NOERROR;
        }
    // } catch (FileNotFoundException fnfe) {
        // Log.e(LOG_TAG, "Error writing historical recrod file: " + hostoryFileName, fnfe);
        // return null;
    // }

    AutoPtr<IXmlSerializer> serializer;
    CKXmlSerializer::New((IXmlSerializer**)&serializer);
    // try {
        serializer->SetOutput(IOutputStream::Probe(fos), String(""));
        serializer->StartDocument(String("UTF-8"), TRUE);
        serializer->WriteStartTag(String(""), TAG_HISTORICAL_RECORDS);

        Int32 recordCount = 0;
        historicalRecords->GetSize(&recordCount);
        for (Int32 idx=0; idx<recordCount; ++idx) {
            AutoPtr<IInterface> interfaceTmp;
            historicalRecords->Get(idx, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            HistoricalRecord* record = (HistoricalRecord*)objTmp;
            historicalRecords->Remove(interfaceTmp);
            serializer->WriteStartTag(String(""), TAG_HISTORICAL_RECORD);

            String str;
            record->mActivity->FlattenToString(&str);
            serializer->WriteAttribute(String(""), ATTRIBUTE_ACTIVITY, str);

            String time, weight;
            time = StringUtils::ToString(record->mTime);
            weight = StringUtils::ToString(record->mWeight);
            serializer->WriteAttribute(String(""), ATTRIBUTE_TIME, time);
            serializer->WriteAttribute(String(""), ATTRIBUTE_WEIGHT, weight);
            serializer->WriteEndTag(String(""), TAG_HISTORICAL_RECORD);

            if (DEBUG) {
                String recordStr;
                record->ToString(&recordStr);
                Logger::I(mHost->TAG, String("Wrote ") + recordStr);
            }
        }

        serializer->WriteEndTag(String(""), TAG_HISTORICAL_RECORDS);
        serializer->EndDocument();

        if (DEBUG) {
            Logger::I(mHost->TAG, String("Wrote ") + StringUtils::ToString(recordCount) + String(" historical records."));
        }
    // } catch (IllegalArgumentException iae) {
    //     Log.e(LOG_TAG, "Error writing historical recrod file: " + mHistoryFileName, iae);
    // } catch (IllegalStateException ise) {
    //     Log.e(LOG_TAG, "Error writing historical recrod file: " + mHistoryFileName, ise);
    // } catch (IOException ioe) {
    //     Log.e(LOG_TAG, "Error writing historical recrod file: " + mHistoryFileName, ioe);
    // } finally {
            mHost->mCanReadHistoricalData = TRUE;
            if (fos != NULL) {
                // try {
                    IOutputStream::Probe(fos)->Close();
                // } catch (IOException e) {
                    /* ignore */
                // }
            }
    // }
    *result = NULL;
    return NOERROR;
}

ActivityChooserModel::DataModelPackageMonitor::DataModelPackageMonitor(
    /* [in] */ ActivityChooserModel* host)
    : mHost(host)
{
    assert(mHost);
}

ECode ActivityChooserModel::DataModelPackageMonitor::OnSomePackagesChanged()
{
    mHost->mReloadActivities = TRUE;
    return NOERROR;
}

AutoPtr<IActivityChooserModel> ActivityChooserModel::Get(
    /* [in] */ IContext* context,
    /* [in] */ const String& historyFileName)
{
    AutoLock lock(sRegistryLock);
    HashMap<String, AutoPtr<IActivityChooserModel> >::Iterator it =
        sDataModelRegistry.Find(historyFileName);
    AutoPtr<IActivityChooserModel> dataModel;
    if(it != sDataModelRegistry.End() && it->mSecond != NULL)
    {
        dataModel = it->mSecond;
    }
    else {
        dataModel = new ActivityChooserModel(context, historyFileName);
        sDataModelRegistry[historyFileName] = dataModel;
    }
    return dataModel;
}

ActivityChooserModel::~ActivityChooserModel()
{
    mPackageMonitor->Unregister();
}

ECode ActivityChooserModel::SetIntent(
    /* [in] */ IIntent* intent)
{
    AutoLock lock(mInstanceLock);
    if (TO_IINTERFACE(intent) == TO_IINTERFACE(mIntent)) {
        return NOERROR;
    }
    mIntent = intent;
    mReloadActivities = TRUE;
    EnsureConsistentState();
    return NOERROR;
}

ECode ActivityChooserModel::GetIntent(
    /* [out] */ IIntent** intent)
{
    AutoLock lock(mInstanceLock);
    *intent = mIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode ActivityChooserModel::GetActivityCount(
    /* [out] */ Int32* rst)
{
    AutoLock lock(mInstanceLock);
    EnsureConsistentState();
    *rst = mActivities.GetSize();
    return NOERROR;
}

ECode ActivityChooserModel::GetActivity(
    /* [in] */ Int32 index,
    /* [out] */ IResolveInfo** activity)
{
    AutoLock lock(mInstanceLock);
    EnsureConsistentState();
    *activity = mActivities[index]->mResolveInfo;
    REFCOUNT_ADD(*activity);
    return NOERROR;
}

ECode ActivityChooserModel::GetActivityIndex(
    /* [in] */ IResolveInfo* activity,
    /* [out] */ Int32* rst)
{
    AutoLock lock(mInstanceLock);
    EnsureConsistentState();
    List<AutoPtr<ActivityResolveInfo> >::Iterator it = mActivities.Begin();
    Int32 i = 0 ;
    for (; it != mActivities.End(); it++, i++) {
        AutoPtr<ActivityResolveInfo> currentActivity = *it;
        if (activity == currentActivity->mResolveInfo) {
            *rst = i;
            return NOERROR;
        }
    }
    *rst = INVALID_INDEX;
    return NOERROR;
}

ECode ActivityChooserModel::ChooseActivity(
    /* [in] */ Int32 index,
    /* [out] */ IIntent** intent)
{
    AutoLock lock(mInstanceLock);
    if (mIntent == NULL) {
        *intent = NULL;
        return NOERROR;
    }

    EnsureConsistentState();
    AutoPtr<ActivityResolveInfo> chosenActivity = mActivities[index];

    AutoPtr<IComponentName> chosenName;
    AutoPtr<IActivityInfo> aInfo;
    chosenActivity->mResolveInfo->GetActivityInfo((IActivityInfo**)&aInfo);
    IPackageItemInfo* packageItemInfo = IPackageItemInfo::Probe(aInfo);
    String packageName, name;
    packageItemInfo->GetPackageName(&packageName);
    packageItemInfo->GetName(&name);
    CComponentName::New(packageName, name, (IComponentName**)&chosenName);
    AutoPtr<IIntent> choiceIntent;
    CIntent::New(mIntent, (IIntent**)&choiceIntent);
    choiceIntent->SetComponent(chosenName);

    if (mActivityChoserModelPolicy != NULL) {
        AutoPtr<IIntent> choiceIntentCopy;
        CIntent::New(choiceIntent, (IIntent**)&choiceIntent);
        Boolean handled;
        mActivityChoserModelPolicy->OnChooseActivity(this, choiceIntentCopy, &handled);
        if (handled) {
            *intent = NULL;
            return NOERROR;
        }
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    AutoPtr<HistoricalRecord> historicalRecord =
        new HistoricalRecord(chosenName, millis, DEFAULT_HISTORICAL_RECORD_WEIGHT);
    AddHisoricalRecord(historicalRecord);
    *intent = choiceIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode ActivityChooserModel::SetOnChooseActivityListener(
    /* [in] */ IOnChooseActivityListener* listener)
{
    AutoLock lock(mInstanceLock);
    mActivityChoserModelPolicy = listener;
    return NOERROR;
}

ECode ActivityChooserModel::GetDefaultActivity(
    /* [out] */ IResolveInfo** rInfo)
{
    AutoLock lock(mInstanceLock);
    EnsureConsistentState();
    if (!mActivities.IsEmpty()) {
        *rInfo = mActivities[0]->mResolveInfo;
        REFCOUNT_ADD(*rInfo);
        return NOERROR;
    }
    *rInfo = NULL;
    return NOERROR;
}

ECode ActivityChooserModel::SetDefaultActivity(
    /* [in] */ Int32 index)
{
    AutoLock lock(mInstanceLock);
    EnsureConsistentState();
    AutoPtr<ActivityResolveInfo> newDefaultActivity = mActivities[index];
    AutoPtr<ActivityResolveInfo> oldDefaultActivity = mActivities[0];

    Float weight;
    if (oldDefaultActivity != NULL) {
        // Add a record with weight enough to boost the chosen at the top.
        weight = oldDefaultActivity->mWeight - newDefaultActivity->mWeight
            + DEFAULT_ACTIVITY_INFLATION;
    }
    else {
        weight = DEFAULT_HISTORICAL_RECORD_WEIGHT;
    }

    AutoPtr<IComponentName> defaultName;
    AutoPtr<IActivityInfo> aInfo;
    newDefaultActivity->mResolveInfo->GetActivityInfo((IActivityInfo**)&aInfo);
    IPackageItemInfo* packageItemInfo = IPackageItemInfo::Probe(aInfo);
    String packageName, name;
    packageItemInfo->GetPackageName(&packageName);
    packageItemInfo->GetName(&name);
    CComponentName::New(packageName, name, (IComponentName**)&defaultName);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    AutoPtr<HistoricalRecord> historicalRecord = new HistoricalRecord(defaultName, millis, weight);
    AddHisoricalRecord(historicalRecord);
    return NOERROR;
}

ECode ActivityChooserModel::SetActivitySorter(
    /* [in] */ IActivitySorter* activitySorter)
{
    AutoLock lock(mInstanceLock);
    if (activitySorter == mActivitySorter) {
        return NOERROR;
    }
    mActivitySorter = (DefaultSorter*)activitySorter;
    if (SortActivitiesIfNeeded()) {
        NotifyChanged();
    }
    return NOERROR;
}

ECode ActivityChooserModel::SetHistoryMaxSize(
    /* [in] */ Int32 historyMaxSize)
{
    AutoLock lock(mInstanceLock);
    if (mHistoryMaxSize == historyMaxSize) {
        return NOERROR;
    }
    mHistoryMaxSize = historyMaxSize;
    PruneExcessiveHistoricalRecordsIfNeeded();
    if (SortActivitiesIfNeeded()) {
        NotifyChanged();
    }
    return NOERROR;
}

ECode ActivityChooserModel::GetHistoryMaxSize(
    /* [out] */ Int32* size)
{
    AutoLock lock(mInstanceLock);
    *size = mHistoryMaxSize;
    return NOERROR;
}

ECode ActivityChooserModel::GetHistorySize(
        /*[out] */ Int32* size)
{
    AutoLock lock(mInstanceLock);
    return mHistoryMaxSize;
}

ECode ActivityChooserModel::RegisterObserver(
    /* [in] */ IInterface* observer)
{
    return DataSetObservable::RegisterObserver(observer);
}

ECode ActivityChooserModel::UnregisterObserver(
    /* [in] */ IInterface* observer)
{
    return DataSetObservable::UnregisterObserver(observer);
}

ECode ActivityChooserModel::UnregisterAll()
{
    return DataSetObservable::UnregisterAll();
}

ECode ActivityChooserModel::NotifyChanged()
{
    return DataSetObservable::NotifyChanged();
}

ECode ActivityChooserModel::NotifyInvalidated()
{
    return DataSetObservable::NotifyInvalidated();
}

ActivityChooserModel::ActivityChooserModel(
    /* [in] */ IContext* context,
    /* [in] */ const String& historyFileName)
{
    mPackageMonitor = new DataModelPackageMonitor(this);
    mActivitySorter = new DefaultSorter(this);
    mHistoryMaxSize = DEFAULT_HISTORY_MAX_LENGTH;
    mCanReadHistoricalData = TRUE;
    mReadShareHistoryCalled = FALSE;
    mHistoricalRecordsChanged = TRUE;
    mReloadActivities = FALSE;

    context->GetApplicationContext((IContext**)&mContext);
    if (!TextUtils::IsEmpty(historyFileName) && !historyFileName.EndWith(HISTORY_FILE_EXTENSION)) {
        mHistoryFileName = historyFileName + HISTORY_FILE_EXTENSION;
    }
    else {
        mHistoryFileName = historyFileName;
    }
    mPackageMonitor->Register(mContext, NULL, TRUE);
}

ECode ActivityChooserModel::PersistHistoricalDataIfNeeded()
{
    if (!mReadShareHistoryCalled) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!mHistoricalRecordsChanged) {
        return NOERROR;
    }
    mHistoricalRecordsChanged = FALSE;
    if (!TextUtils::IsEmpty(mHistoryFileName)) {
        AutoPtr<IList> param1;
        CArrayList::New((IList**)&param1);
        List<AutoPtr<HistoricalRecord> >::Iterator it = mHistoricalRecords.Begin();
        for (; it != mHistoricalRecords.End(); it++) {
            param1->Add(TO_IINTERFACE(*it));
        }
        AutoPtr<ICharSequence> param2;
        CString::New(mHistoryFileName, (ICharSequence**)&param2);
        AutoPtr< ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(2);
        params->Set(0, TO_IINTERFACE(param1));
        params->Set(1, TO_IINTERFACE(param2));
        AutoPtr<PersistHistoryAsyncTask> task = new PersistHistoryAsyncTask(this);
        task->ExecuteOnExecutor(AsyncTask::SERIAL_EXECUTOR, params);
    }
    return NOERROR;
}

ECode ActivityChooserModel::EnsureConsistentState()
{
    Boolean stateChanged = LoadActivitiesIfNeeded();
    stateChanged |= ReadHistoricalDataIfNeeded();
    PruneExcessiveHistoricalRecordsIfNeeded();
    if (stateChanged) {
        SortActivitiesIfNeeded();
        NotifyChanged();
    }
    return NOERROR;
}

Boolean ActivityChooserModel::SortActivitiesIfNeeded()
{
    if (mActivitySorter != NULL && mIntent != NULL && !mActivities.IsEmpty() && !mHistoricalRecords.IsEmpty()) {
        AutoPtr<IList> activities = TransfromList(mActivities);
        AutoPtr<IList> historicalRecords = TransfromList(mHistoricalRecords);

        AutoPtr<ICollections> connection;
        CCollections::AcquireSingleton((ICollections**)&connection);
        AutoPtr<IList> unmodifiableList;
        connection->UnmodifiableList(historicalRecords, (IList**)&unmodifiableList);
        mActivitySorter->Sort(mIntent, activities, unmodifiableList);
        return TRUE;
    }
    return FALSE;
}

Boolean ActivityChooserModel::LoadActivitiesIfNeeded()
{
    if (mReloadActivities && mIntent != NULL) {
        mReloadActivities = FALSE;
        mActivities.Clear();
        AutoPtr<IPackageManager> pmgr;
        mContext->GetPackageManager((IPackageManager**)&pmgr);
        AutoPtr<IList> container;
        pmgr->QueryIntentActivities(mIntent, 0, (IList**)&container);

        AutoPtr<IActivityManagerHelper> helper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
        String permission;
        AutoPtr<IApplicationInfo> applicationInfo;
        CApplicationInfo::New((IApplicationInfo**)&applicationInfo);
        Int32 applicationUId;
        applicationInfo->GetUid(&applicationUId);
        Int32 checkPermissionRes = 0;

        Int32 size = 0;
        container->GetSize(&size);
        for (Int32 idx=0; idx<size; ++idx) {
            AutoPtr<IInterface> interfaceTmp;
            container->Get(idx, (IInterface**)&interfaceTmp);
            IResolveInfo* resolveInfo = IResolveInfo::Probe(interfaceTmp);

            AutoPtr<IActivityInfo> activityInfo;
            resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
            activityInfo->GetPermission(&permission);

            Boolean exported;
            IComponentInfo::Probe(activityInfo)->GetExported(&exported);
            helper->CheckComponentPermission(permission, Process::MyUid(), applicationUId, exported, &checkPermissionRes);
            if (checkPermissionRes == IPackageManager::PERMISSION_GRANTED) {
                AutoPtr<ActivityResolveInfo> activityResolveInfo = new ActivityResolveInfo(resolveInfo, this);
                mActivities.PushBack(activityResolveInfo);
            }
        }
        return TRUE;
    }
    return FALSE;
}

Boolean ActivityChooserModel::ReadHistoricalDataIfNeeded()
{
    if (mCanReadHistoricalData && mHistoricalRecordsChanged && !TextUtils::IsEmpty(mHistoryFileName)) {
        mCanReadHistoricalData = FALSE;
        mReadShareHistoryCalled = TRUE;
        ReadHistoricalDataImpl();
        return TRUE;
    }
    return FALSE;
}

Boolean ActivityChooserModel::AddHisoricalRecord(
    /* [in] */ IHistoricalRecord* historicalRecord)
{
    mHistoricalRecords.PushBack((HistoricalRecord*)historicalRecord);
    mHistoricalRecordsChanged = TRUE;
    PruneExcessiveHistoricalRecordsIfNeeded();
    PersistHistoricalDataIfNeeded();
    SortActivitiesIfNeeded();
    NotifyChanged();
    return TRUE;
}

ECode ActivityChooserModel::PruneExcessiveHistoricalRecordsIfNeeded()
{
    Int32 pruneCount = mHistoricalRecords.GetSize() - mHistoryMaxSize;
    if (pruneCount <= 0) {
        return NOERROR;
    }
    mHistoricalRecordsChanged = TRUE;
    List<AutoPtr<HistoricalRecord> >::Iterator it = mHistoricalRecords.Begin();
    for (Int32 i = 0; i < pruneCount; ++i) {
        List<AutoPtr<HistoricalRecord> >::Iterator oldIt = it++;
        mHistoricalRecords.Erase(oldIt);
    }
    return NOERROR;
}

ECode ActivityChooserModel::ReadHistoricalDataImpl()
{
    AutoPtr<IFileInputStream> fis;
    FAIL_RETURN(mContext->OpenFileInput(mHistoryFileName, (IFileInputStream**)&fis));
    AutoPtr<IXmlPullParser> parser;
    CKXmlParser::New((IXmlPullParser**)&parser);
    String nodeName;
    String activity;
    String parseTmp;
    String name;
    Int64 time;
    Int32 type;

    parser->SetInput(IInputStream::Probe(fis), String(""));
    type = IXmlPullParser::START_DOCUMENT;
    while (type != IXmlPullParser::END_DOCUMENT && type != IXmlPullParser::START_TAG) {
        if (FAILED(parser->Next(&type))) {
            goto finally;
        }
    }
    parser->GetName(&name);
    if (!TAG_HISTORICAL_RECORDS.Equals(name)) {
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    mHistoricalRecords.Clear();
    while(TRUE) {
        if (FAILED(parser->Next(&type))) {
            goto finally;
        }
        if (type == IXmlPullParser::END_DOCUMENT) {
            break;
        }
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        parser->GetName(&nodeName);
        if (!TAG_HISTORICAL_RECORD.Equals(nodeName)) {
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        parser->GetAttributeValue(String(NULL), ATTRIBUTE_ACTIVITY, &activity);
        parser->GetAttributeValue(String(NULL), ATTRIBUTE_TIME, &parseTmp);
        time = StringUtils::ParseInt64(parseTmp);
        parser->GetAttributeValue(String(NULL), ATTRIBUTE_WEIGHT, &parseTmp);
        Float weight = StringUtils::ParseFloat(parseTmp);
        AutoPtr<HistoricalRecord> readRecord = new HistoricalRecord(activity, time, weight);
        mHistoricalRecords.PushBack(readRecord);
    }
    finally:
        if (fis != NULL) {
            IInputStream::Probe(fis)->Close();
        }
        return NOERROR;
}

template<typename T>
AutoPtr<IList> ActivityChooserModel::TransfromList(
        /* [in] */ List<AutoPtr<T> >& list)
{
    AutoPtr<IList> container;
    CArrayList::New((IList**)&container);
    typename List<AutoPtr<T> >::Iterator it = list.Begin();
    for(; it != list.End(); it++) {
        container->Add(TO_IINTERFACE(*it));
    }
    return container;
}

template<typename T, typename IT>
List<AutoPtr<T> > ActivityChooserModel::TransfromContainer(
        /* [in] */ IList* container)
{
    List< AutoPtr<T> > list;
    AutoPtr<IList> em;
    Int32 size = 0;
    container->GetSize(&size);
    for (Int32 idx=0; idx<size; ++idx) {
        AutoPtr<IInterface> interfaceTmp;
        container->Get(idx, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        AutoPtr<T> obj = (T*)objTmp;
        list.PushBack(obj);
    }
    return list;
}

Int32 ActivityChooserModel::FloatToIntBits(
    /* [in] */ Float value)
{
    Int32 intBit;
    Boolean isNan = Elastos::Core::Math::IsNaN(value);
    if(isNan) {
        intBit = 0x7fc00000;
    }
    else {
        intBit = *((Int32*)(&value));
    }
    return intBit;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
