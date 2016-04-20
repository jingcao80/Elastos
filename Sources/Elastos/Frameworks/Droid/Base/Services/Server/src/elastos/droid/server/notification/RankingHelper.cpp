
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/notification/RankingHelper.h"
#include "elastos/droid/server/notification/RankingReconsideration.h"
#include "elastos/droid/server/notification/NotificationManagerService.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Service::Notification::INotificationListenerServiceRanking;
using Elastos::Droid::Server::Notification::NotificationManagerService;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Core::CoreUtils;
using Elastos::Core::IClassLoader;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ISet;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String RankingHelper::TAG("RankingHelper");
const Boolean RankingHelper::DEBUG = FALSE;

const Int32 RankingHelper::XML_VERSION = 1;

const String RankingHelper::TAG_RANKING("ranking");
const String RankingHelper::TAG_PACKAGE("package");
const String RankingHelper::ATT_VERSION("version");

const String RankingHelper::ATT_NAME("name");
const String RankingHelper::ATT_UID("uid");
const String RankingHelper::ATT_PRIORITY("priority");
const String RankingHelper::ATT_VISIBILITY("visibility");
const String RankingHelper::ATT_KEYGUARD("keyguard");

CAR_INTERFACE_IMPL(RankingHelper, Object, IRankingConfig);

RankingHelper::RankingHelper(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* rankingHandler,
    /* [in] */ ArrayOf<String>* extractorNames)
{
    mPreliminaryComparator = new NotificationComparator();
    mFinalComparator = new GlobalSortKeyComparator();

    mContext = context;
    mRankingHandler = rankingHandler;
    CArrayMap::New((IArrayMap**)&mPackagePriorities);
    CArrayMap::New((IArrayMap**)&mPackageVisibilities);
    CArrayMap::New((IArrayMap**)&mPackageOnKeyguard);

    const Int32 N = extractorNames->GetLength();
    mSignalExtractors = ArrayOf<INotificationSignalExtractor*>::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        // try {
        AutoPtr<IClassLoader> loader;
        mContext->GetClassLoader((IClassLoader**)&loader);
        AutoPtr<IClassInfo> extractorClass;
        ECode ec = loader->LoadClass((*extractorNames)[i], (IClassInfo**)&extractorClass);
        if (FAILED(ec)) {
            if (ec == (ECode)E_CLASS_NOT_FOUND_EXCEPTION){
                Slogger::W(TAG, "Couldn't find extractor %s. 0x%08X", (*extractorNames)[i].string(), ec);
            }
            if (ec == (ECode)E_INSTANTIATION_EXCEPTION){
                Slogger::W(TAG, "Couldn't instantiate extractor %s. 0x%08X", (*extractorNames)[i].string(), ec);
            }
            if (ec == (ECode)E_ILLEGAL_ACCESS_EXCEPTION){
                Slogger::W(TAG, "Couldn't accessing extractor %s. 0x%08X", (*extractorNames)[i].string(), ec);
            }
            continue;
        }
        AutoPtr<IInterface> obj;
        extractorClass->CreateObject((IInterface**)&obj);
        AutoPtr<INotificationSignalExtractor> extractor = INotificationSignalExtractor::Probe(obj);
        extractor->Initialize(mContext);
        extractor->SetConfig(this);
        (*mSignalExtractors)[i] = extractor;
        // } catch (ClassNotFoundException e) {
        //     Slog.w(TAG, "Couldn't find extractor " + extractorNames[i] + ".", e);
        // } catch (InstantiationException e) {
        //     Slog.w(TAG, "Couldn't instantiate extractor " + extractorNames[i] + ".", e);
        // } catch (IllegalAccessException e) {
        //     Slog.w(TAG, "Problem accessing extractor " + extractorNames[i] + ".", e);
        // }
    }
    CArrayMap::New((IArrayMap**)&mProxyByGroupTmp);
}

RankingHelper::~RankingHelper()
{}

AutoPtr<INotificationSignalExtractor> RankingHelper::FindExtractor(
    /* [in] */ const InterfaceID& extractorClass)
{
    const Int32 N = mSignalExtractors->GetLength();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<INotificationSignalExtractor> extractor = (*mSignalExtractors)[i];

        if (extractor->Probe(extractorClass) != NULL) {
            return extractor;
        }

    }
    return NULL;
}

ECode RankingHelper::ExtractSignals(
    /* [in] */ NotificationRecord* r)
{
    const Int32 N = mSignalExtractors->GetLength();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<INotificationSignalExtractor> extractor = (*mSignalExtractors)[i];
        // try {
        AutoPtr<IRankingReconsideration> recon;
        extractor->Process((INotificationRecord*)r, (IRankingReconsideration**)&recon);
        if (recon != NULL) {
            AutoPtr<IMessageHelper> helper;
            CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
            AutoPtr<IMessage> m;
            helper->Obtain(mRankingHandler,
                    NotificationManagerService::MESSAGE_RECONSIDER_RANKING,
                    recon, (IMessage**)&m);
            AutoPtr<ITimeUnitHelper> timeHelper;
            CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&timeHelper);
            AutoPtr<ITimeUnit> timeUnit;
            timeHelper->GetMILLISECONDS((ITimeUnit**)&timeUnit);
            Int64 delay = ((RankingReconsideration*)recon.Get())->GetDelay(timeUnit);
            Boolean res;
            mRankingHandler->SendMessageDelayed(m, delay, &res);
        }
        // } catch (Throwable t) {
        //     Slog.w(TAG, "NotificationSignalExtractor failed.", t);
        // }
    }
    return NOERROR;
}

ECode RankingHelper::ReadXml(
    /* [in] */ IXmlPullParser* parser)
{
   Int32 type;
   parser->GetEventType(&type);
    if (type != IXmlPullParser::START_TAG) return NOERROR;
    String tag;
    parser->GetName(&tag);
    if (!TAG_RANKING.Equals(tag)) return NOERROR;
    mPackagePriorities->Clear();
    Int32 version;
    SafeInt32(parser, ATT_VERSION, XML_VERSION, &version);
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT) {
        parser->GetName(&tag);
        if (type == IXmlPullParser::END_TAG && TAG_RANKING.Equals(tag)) {
            return NOERROR;
        }
        if (type == IXmlPullParser::START_TAG) {
            if (TAG_PACKAGE.Equals(tag)) {
                Int32 uid;
                SafeInt32(parser, ATT_UID, IUserHandle::USER_ALL, &uid);
                Int32 priority;
                SafeInt32(parser, ATT_PRIORITY, INotification::PRIORITY_DEFAULT, &priority);
                Int32 vis;
                SafeInt32(parser, ATT_VISIBILITY,
                        INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE, &vis);
                Int32 keyguard = 0;
                SafeInt32(parser, ATT_KEYGUARD,
                        INotification::SHOW_ALL_NOTI_ON_KEYGUARD, &keyguard);
                String name;
                parser->GetAttributeValue(String(NULL), ATT_NAME, &name);

                if (!TextUtils::IsEmpty(name)) {
                    if (priority != INotification::PRIORITY_DEFAULT) {
                        AutoPtr<IInterface> obj;
                        mPackagePriorities->Get(CoreUtils::Convert(name), (IInterface**)&obj);
                        AutoPtr<ISparseInt32Array> priorityByUid = ISparseInt32Array::Probe(obj);
                        if (priorityByUid == NULL) {
                            CSparseInt32Array::New((ISparseInt32Array**)&priorityByUid);
                            mPackagePriorities->Put(CoreUtils::Convert(name), priorityByUid);
                        }
                        priorityByUid->Put(uid, priority);
                    }
                    if (vis != INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE) {
                        AutoPtr<IInterface> obj;
                        mPackageVisibilities->Get(CoreUtils::Convert(name), (IInterface**)&obj);
                        AutoPtr<ISparseInt32Array> visibilityByUid = ISparseInt32Array::Probe(obj);
                        if (visibilityByUid == NULL) {
                            CSparseInt32Array::New((ISparseInt32Array**)&visibilityByUid);
                            mPackageVisibilities->Put(CoreUtils::Convert(name), visibilityByUid);
                        }
                        visibilityByUid->Put(uid, vis);
                    }
                    if (keyguard != INotification::SHOW_ALL_NOTI_ON_KEYGUARD) {
                        AutoPtr<IInterface> obj;
                        mPackageOnKeyguard->Get(CoreUtils::Convert(name), (IInterface**)&obj);
                        AutoPtr<ISparseInt32Array> keyguardByUid = ISparseInt32Array::Probe(obj);
                        if (keyguardByUid == NULL) {
                            CSparseInt32Array::New((ISparseInt32Array**)&keyguardByUid);
                            mPackageOnKeyguard->Put(CoreUtils::Convert(name), keyguardByUid);
                        }
                        keyguardByUid->Put(uid, keyguard);
                    }
                }
            }
        }
    }
    Slogger::E(TAG, "Failed to reach END_DOCUMENT");
    return E_ILLEGAL_STATE_EXCEPTION;
    // throw new IllegalStateException("Failed to reach END_DOCUMENT");
}

ECode RankingHelper::WriteXml(
    /* [in] */ IXmlSerializer* out)
{
    out->WriteStartTag(String(NULL), TAG_RANKING);
    out->WriteAttribute(String(NULL), ATT_VERSION, StringUtils::ToString(XML_VERSION));

    Int32 size1 = 0, size2 = 0, size3 = 0;
    mPackagePriorities->GetSize(&size1);
    mPackageVisibilities->GetSize(&size2);
    mPackageOnKeyguard->GetSize(&size3);
    AutoPtr<ISet> packageNames;
    CArraySet::New(size1 + size2 + size3, (ISet**)&packageNames);
    AutoPtr<ISet> set1, set2, set3;
    mPackagePriorities->GetKeySet((ISet**)&set1);
    mPackageVisibilities->GetKeySet((ISet**)&set2);
    mPackageOnKeyguard->GetKeySet((ISet**)&set3);
    packageNames->AddAll(ICollection::Probe(set1));
    packageNames->AddAll(ICollection::Probe(set2));
    packageNames->AddAll(ICollection::Probe(set3));
    AutoPtr<ISet> packageUids;
    CArraySet::New((ISet**)&packageUids);
    AutoPtr<IIterator> iterator;
    packageNames->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        String packageName;
        ICharSequence::Probe(obj)->ToString(&packageName);

        packageUids->Clear();
        obj = NULL;
        mPackagePriorities->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<ISparseInt32Array> priorityByUid = ISparseInt32Array::Probe(obj);
        obj = NULL;
        mPackageVisibilities->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<ISparseInt32Array> visibilityByUid = ISparseInt32Array::Probe(obj);
        obj = NULL;
        mPackageOnKeyguard->Get(CoreUtils::Convert(packageName), (IInterface**)&obj);
        AutoPtr<ISparseInt32Array> keyguardByUid = ISparseInt32Array::Probe(obj);
        if (priorityByUid != NULL) {
            Int32 M = 0;
            priorityByUid->GetSize(&M);
            for (Int32 j = 0; j < M; j++) {
                Int32 key = 0;
                priorityByUid->KeyAt(j, &key);
                packageUids->Add(CoreUtils::Convert(key));
            }
        }
        if (visibilityByUid != NULL) {
            Int32 M = 0;
            visibilityByUid->GetSize(&M);
            for (Int32 j = 0; j < M; j++) {
                Int32 key = 0;
                visibilityByUid->KeyAt(j, &key);
                packageUids->Add(CoreUtils::Convert(key));
            }
        }
        if (keyguardByUid != NULL) {
            Int32 M = 0;
            keyguardByUid->GetSize(&M);
            for (Int32 j = 0; j < M; j++) {
                Int32 key = 0;
                keyguardByUid->KeyAt(j, &key);
                packageUids->Add(CoreUtils::Convert(key));
            }
        }

        AutoPtr<IIterator> it;
        packageUids->GetIterator((IIterator**)&it);
        Boolean has = FALSE;
        while (it->HasNext(&has), has) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            Int32 uid;
            IInteger32::Probe(obj)->GetValue(&uid);

            out->WriteStartTag(String(NULL), TAG_PACKAGE);
            out->WriteAttribute(String(NULL), ATT_NAME, packageName);
            if (priorityByUid != NULL) {
                Int32 priority;
                priorityByUid->Get(uid, &priority);
                if (priority != INotification::PRIORITY_DEFAULT) {
                    out->WriteAttribute(String(NULL), ATT_PRIORITY, StringUtils::ToString(priority));
                }
            }
            if (visibilityByUid != NULL) {
                Int32 visibility;
                visibilityByUid->Get(uid, &visibility);
                if (visibility != INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE) {
                    out->WriteAttribute(String(NULL), ATT_VISIBILITY, StringUtils::ToString(visibility));
                }
            }
            if (keyguardByUid != NULL) {
                Int32 keyguard = 0;
                keyguardByUid->Get(uid, &keyguard);
                if (keyguard != INotification::SHOW_ALL_NOTI_ON_KEYGUARD) {
                    out->WriteAttribute(String(NULL), ATT_KEYGUARD, StringUtils::ToString(keyguard));
                }
            }
            out->WriteAttribute(String(NULL), ATT_UID, StringUtils::ToString(uid));
            out->WriteEndTag(String(NULL), TAG_PACKAGE);
        }
    }
    out->WriteEndTag(String(NULL), TAG_RANKING);
    return NOERROR;
}

void RankingHelper::UpdateConfig()
{
    const Int32 N = mSignalExtractors->GetLength();
    for (Int32 i = 0; i < N; i++) {
        (*mSignalExtractors)[i]->SetConfig(this);
    }

    Boolean result;
    mRankingHandler->SendEmptyMessage(NotificationManagerService::MESSAGE_RANKING_CONFIG_CHANGE, &result);
}

ECode RankingHelper::Sort(
    /* [in] */ IArrayList* notificationList)
{
    Int32 N;
    notificationList->GetSize(&N);
    // clear global sort keys
    for (Int32 i = N - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        notificationList->Get(i, (IInterface**)&obj);
        ((NotificationRecord*)INotificationRecord::Probe(obj))->SetGlobalSortKey(String(NULL));
    }

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);

    // try {
    // rank each record individually
    ECode ec = coll->Sort(IList::Probe(notificationList), (IComparator*)mPreliminaryComparator.Get());
    // } catch (RuntimeException ex) {
        // Don't crash the system server if something bad happened.
    if (FAILED(ec)) {
        Logger::E(TAG, "Extreme badness during notification sort, 0X%08X", ec);
        Logger::E(TAG, "Current notification list: ");
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            notificationList->Get(i, (IInterface**)&obj);
            AutoPtr<NotificationRecord> nr = (NotificationRecord*)INotificationRecord::Probe(obj);
            String key;
            nr->GetNotification()->GetSortKey(&key);
            String nrStr;
            nr->ToString(&nrStr);
            String str("");
            str.AppendFormat(
                    "  [%d] %s (group %s, rank %d, sortkey %s)",
                    i, nrStr.string(), nr->GetGroupKey().string(), nr->GetAuthoritativeRank(), key.string());
            Logger::E(TAG, str);
        }
        // STOPSHIP: remove once b/16626175 is found
        return ec;
    }
    // }

    synchronized(mProxyByGroupTmp) {
        // record individual ranking result and nominate proxies for each group
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            notificationList->Get(i, (IInterface**)&obj);
            AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
            record->SetAuthoritativeRank(i);
            const String groupKey = record->GetGroupKey();
            Boolean isGroupSummary;
            record->GetNotification()->IsGroupSummary(&isGroupSummary);
            Boolean res;
            if (isGroupSummary || (mProxyByGroupTmp->ContainsKey(CoreUtils::Convert(groupKey), &res), !res)) {
                mProxyByGroupTmp->Put(CoreUtils::Convert(groupKey), (INotificationRecord*)record);
            }
        }
        // assign global sort key:
        //   is_recently_intrusive:group_rank:is_group_summary:group_sort_key:rank
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            notificationList->Get(i, (IInterface**)&obj);
            AutoPtr<NotificationRecord> record = (NotificationRecord*)INotificationRecord::Probe(obj);
            obj = NULL;
            mProxyByGroupTmp->Get(CoreUtils::Convert(record->GetGroupKey()), (IInterface**)&obj);
            AutoPtr<NotificationRecord> groupProxy = (NotificationRecord*)INotificationRecord::Probe(obj);

            String groupSortKey;
            record->GetNotification()->GetSortKey(&groupSortKey);

            // We need to make sure the developer provided group sort key (gsk) is handled
            // correctly:
            //   gsk="" < gsk=non-null-string < gsk=null
            //
            // We enforce this by using different prefixes for these three cases.
            String groupSortKeyPortion;
            if (groupSortKey.IsNull()) {
                groupSortKeyPortion = "nsk";
            }
            else if (groupSortKey.Equals("")) {
                groupSortKeyPortion = "esk";
            }
            else {
                groupSortKeyPortion = String("gsk=") + groupSortKey;
            }

            Boolean isGroupSummary;
            record->GetNotification()->IsGroupSummary(&isGroupSummary);

            String str("");
            str.AppendFormat("intrsv=%c:grnk=0x%04x:gsmry=%c:%s:rnk=0x%04x",
                    record->IsRecentlyIntrusive() ? '0' : '1',
                    groupProxy->GetAuthoritativeRank(),
                    isGroupSummary ? '0' : '1',
                    groupSortKeyPortion.string(),
                    record->GetAuthoritativeRank());

            record->SetGlobalSortKey(str);
        }
        mProxyByGroupTmp->Clear();
    }

    // Do a second ranking pass, using group proxies
    return coll->Sort(IList::Probe(notificationList), (IComparator*)mFinalComparator.Get());
}

Int32 RankingHelper::IndexOf(
    /* [in] */ IArrayList* notificationList,
    /* [in] */ NotificationRecord* target)
{
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    Int32 value;
    coll->BinarySearch(IList::Probe(notificationList),
            (INotificationRecord*)target,
            (IComparator*)mFinalComparator.Get(),
            &value);
    return value;
}

ECode RankingHelper::SafeInt32(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& att,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String val;
    parser->GetAttributeValue(String(NULL), att, &val);
    return TryParseInt32(val, defValue, result);
}

ECode RankingHelper::TryParseInt32(
    /* [in] */ const String& value,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (TextUtils::IsEmpty(value)) {
        *result = defValue;
        return NOERROR;
    }

    // try {
    Int32 data;
    ECode ec = StringUtils::Parse(value, &data);
    if (SUCCEEDED(ec)) {
        *result = data;
        return NOERROR;
    }
    else {
        *result = defValue;
    }
    // } catch (NumberFormatException e) {
    //     return defValue;
    // }
    return NOERROR;
}

ECode RankingHelper::GetPackagePriority(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* _priority)
{
    VALIDATE_NOT_NULL(_priority);

    Int32 priority = INotification::PRIORITY_DEFAULT;

    AutoPtr<IInterface> object;
    mPackagePriorities->Get(CoreUtils::Convert(packageName), (IInterface**)&object);
    AutoPtr<ISparseInt32Array> priorityByUid = ISparseInt32Array::Probe(object);

    if (priorityByUid != NULL) {
        priorityByUid->Get(uid, INotification::PRIORITY_DEFAULT, &priority);
    }
    *_priority = priority;
    return NOERROR;
}

ECode RankingHelper::SetPackagePriority(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 priority)
{
    Int32 result;
    GetPackagePriority(packageName, uid, &result);
    if (priority == result) {
        return NOERROR;
    }

    AutoPtr<IInterface> object;
    mPackagePriorities->Get(CoreUtils::Convert(packageName), (IInterface**)&object);
    AutoPtr<ISparseInt32Array> priorityByUid = ISparseInt32Array::Probe(object);

    if (priorityByUid == NULL) {
        CSparseInt32Array::New((ISparseInt32Array**)&priorityByUid);
        mPackagePriorities->Put(CoreUtils::Convert(packageName), priorityByUid);
    }
    priorityByUid->Put(uid, priority);
    UpdateConfig();
    return NOERROR;
}

ECode RankingHelper::GetPackageVisibilityOverride(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* _visibility)
{
    VALIDATE_NOT_NULL(_visibility);

    Int32 visibility = INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE;

    AutoPtr<IInterface> object;
    mPackageVisibilities->Get(CoreUtils::Convert(packageName), (IInterface**)&object);
    AutoPtr<ISparseInt32Array> visibilityByUid = ISparseInt32Array::Probe(object);

    if (visibilityByUid != NULL) {
        visibilityByUid->Get(uid,
                INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE, &visibility);
    }
    *_visibility = visibility;
    return NOERROR;
}

ECode RankingHelper::SetPackageVisibilityOverride(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 visibility)
{
    Int32 result;
    GetPackageVisibilityOverride(packageName, uid, &result);
    if (visibility == result) {
        return NOERROR;
    }

    AutoPtr<IInterface> object;
    mPackageVisibilities->Get(CoreUtils::Convert(packageName), (IInterface**)&object);
    AutoPtr<ISparseInt32Array> visibilityByUid = ISparseInt32Array::Probe(object);

    if (visibilityByUid == NULL) {
        CSparseInt32Array::New((ISparseInt32Array**)&visibilityByUid);
        mPackageVisibilities->Put(CoreUtils::Convert(packageName), visibilityByUid);
    }
    visibilityByUid->Put(uid, visibility);
    UpdateConfig();
    return NOERROR;
}

ECode RankingHelper::GetShowNotificationForPackageOnKeyguard(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 keyguard = INotification::SHOW_ALL_NOTI_ON_KEYGUARD;
    AutoPtr<IInterface> object;
    mPackageOnKeyguard->Get(CoreUtils::Convert(packageName), (IInterface**)&object);
    AutoPtr<ISparseInt32Array> keyguardByUid = ISparseInt32Array::Probe(object);
    if (keyguardByUid != NULL) {
        keyguardByUid->Get(uid, INotification::SHOW_ALL_NOTI_ON_KEYGUARD, &keyguard);
    }
    *result = keyguard;
    return NOERROR;
}

ECode RankingHelper::SetShowNotificationForPackageOnKeyguard(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 keyguard)
{
    Int32 kg = 0;
    GetShowNotificationForPackageOnKeyguard(packageName, uid, &kg);
    if (keyguard == kg) {
        return NOERROR;
    }
    AutoPtr<IInterface> object;
    mPackageOnKeyguard->Get(CoreUtils::Convert(packageName), (IInterface**)&object);
    AutoPtr<ISparseInt32Array> keyguardByUid = ISparseInt32Array::Probe(object);
    if (keyguardByUid == NULL) {
        CSparseInt32Array::New((ISparseInt32Array**)&keyguardByUid);
        mPackageOnKeyguard->Put(CoreUtils::Convert(packageName), keyguardByUid);
    }
    keyguardByUid->Put(uid, keyguard);
    UpdateConfig();
    return NOERROR;
}

void RankingHelper::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ DumpFilter* filter)
{
    if (filter == NULL) {
        const Int32 N = mSignalExtractors->GetLength();
        pw->Print(prefix);
        pw->Print(String("mSignalExtractors.length = "));
        pw->Println(N);
        for (Int32 i = 0; i < N; i++) {
            pw->Print(prefix);
            pw->Print(String("  "));
            pw->Println((*mSignalExtractors)[i]);
        }
    }
    Int32 N;
    mPackagePriorities->GetSize(&N);
    if (filter == NULL) {
        pw->Print(prefix);
        pw->Println(String("package priorities:"));
    }
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mPackagePriorities->GetKeyAt(i, (IInterface**)&obj);
        String name;
        ICharSequence::Probe(obj)->ToString(&name);
        if (filter == NULL || filter->Matches(name)) {
            AutoPtr<IInterface> object;
            mPackagePriorities->Get(CoreUtils::Convert(name), (IInterface**)&object);
            AutoPtr<ISparseInt32Array> priorityByUid = ISparseInt32Array::Probe(object);
            Int32 M;
            priorityByUid->GetSize(&M);
            for (Int32 j = 0; j < M; j++) {
                Int32 uid;
                priorityByUid->KeyAt(j, &uid);
                Int32 priority;
                priorityByUid->Get(uid, &priority);
                pw->Print(prefix);
                pw->Print(String("  "));
                pw->Print(name);
                pw->Print(String(" ("));
                pw->Print(uid);
                pw->Print(String(") has priority: "));
                pw->Println(priority);
            }
        }
    }
}

} // Notification
} // Server
} // Droid
} // Elastos
