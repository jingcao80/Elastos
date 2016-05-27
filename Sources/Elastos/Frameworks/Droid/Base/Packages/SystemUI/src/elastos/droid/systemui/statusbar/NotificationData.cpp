
#include "elastos/droid/systemui/statusbar/NotificationData.h"
#include "../R.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Service::Notification::CNotificationListenerServiceRanking;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Core::CString;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

//=============================================================================
//              Entry
//=============================================================================
CAR_INTERFACE_IMPL(NotificationData::Entry, Object, INotificationDataEntry)
NotificationData::Entry::Entry()
    : mAutoRedacted(FALSE)
    , mLegacy(FALSE)
    , mTargetSdk(0)
    , mInterruption(FALSE)
{
}

NotificationData::Entry::Entry(
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ IStatusBarIconView* ic)
    : mNotification(n)
    , mIcon(ic)
{
    n->GetKey(&mKey);
}

ECode NotificationData::Entry::SetBigContentView(
    /* [in] */ IView* bigContentView)
{
    mExpandedBig = bigContentView;
    mRow->SetExpandable(bigContentView != NULL);
    return NOERROR;
}

ECode NotificationData::Entry::GetBigContentView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mExpandedBig;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode NotificationData::Entry::GetPublicContentView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mExpandedPublic;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode NotificationData::Entry::SetInterruption()
{
    mInterruption = TRUE;
    return NOERROR;
}

ECode NotificationData::Entry::HasInterrupted(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = mInterruption;
    return NOERROR;
}

ECode NotificationData::Entry::Reset()
{
    // NOTE: Icon needs to be preserved for now.
    // We should fix this at some point.
    mExpanded = NULL;
    mExpandedPublic = NULL;
    mExpandedBig = NULL;
    mAutoRedacted = FALSE;
    mLegacy = FALSE;
    if (mRow != NULL) {
        IActivatableNotificationView::Probe(mRow)->Reset();
    }
    return NOERROR;
}

ECode NotificationData::Entry::SetKey(
    /* [in] */ const String& key)
{
    mKey = key;
    return NOERROR;
}

ECode NotificationData::Entry::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    *key = mKey;
    return NOERROR;
}

ECode NotificationData::Entry::SetNotification(
    /* [in] */ IStatusBarNotification* sbn)
{
    mNotification = sbn;
    return NOERROR;
}

ECode NotificationData::Entry::GetNotification(
    /* [out] */ IStatusBarNotification** sbn)
{
    VALIDATE_NOT_NULL(sbn);
    *sbn = mNotification;
    REFCOUNT_ADD(*sbn);
    return NOERROR;
}

ECode NotificationData::Entry::SetIcon(
    /* [in] */ IStatusBarIconView* view)
{
    mIcon = view;
    return NOERROR;
}

ECode NotificationData::Entry::GetIcon(
    /* [out] */ IStatusBarIconView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mIcon;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode NotificationData::Entry::SetRow(
    /* [in] */ IExpandableNotificationRow* row)
{
    mRow = row;
    return NOERROR;
}

ECode NotificationData::Entry::GetRow(
    /* [out] */ IExpandableNotificationRow** row)
{
    VALIDATE_NOT_NULL(row);
    *row = mRow;
    REFCOUNT_ADD(*row);
    return NOERROR;
}

ECode NotificationData::Entry::SetExpanded(
    /* [in] */ IView* view)
{
    mExpanded = view;
    return NOERROR;
}

ECode NotificationData::Entry::GetExpanded(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mExpanded;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode NotificationData::Entry::SetExpandedPublic(
    /* [in] */ IView* view)
{
    mExpandedPublic = view;
    return NOERROR;
}

ECode NotificationData::Entry::GetExpandedPublic(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mExpandedPublic;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode NotificationData::Entry::SetExpandedBig(
    /* [in] */ IView* big)
{
    mExpandedBig = big;
    return NOERROR;
}

ECode NotificationData::Entry::GetExpandedBig(
    /* [out] */ IView** big)
{
    VALIDATE_NOT_NULL(big);
    *big = mExpandedBig;
    REFCOUNT_ADD(*big);
    return NOERROR;
}

ECode NotificationData::Entry::SetAutoRedacted(
    /* [in] */ Boolean value)
{
    mAutoRedacted = value;
    return NOERROR;
}

ECode NotificationData::Entry::GetAutoRedacted(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mAutoRedacted;
    return NOERROR;
}

ECode NotificationData::Entry::SetLegacy(
    /* [in] */ Boolean value)
{
    mLegacy = value;
    return NOERROR;
}

ECode NotificationData::Entry::GetLegacy(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLegacy;
    return NOERROR;
}

ECode NotificationData::Entry::SetTargetSdk(
    /* [in] */ Int32 sdk)
{
    mTargetSdk = sdk;
    return NOERROR;
}

ECode NotificationData::Entry::GetTargetSdk(
    /* [out] */ Int32* sdk)
{
    VALIDATE_NOT_NULL(sdk);
    *sdk = mTargetSdk;
    return NOERROR;
}

CAR_INTERFACE_IMPL(NotificationData::Comparator, Object, IComparator)
NotificationData::Comparator::Comparator(
    /* [in] */ NotificationData* host)
    : mHost(host)
{
    CNotificationListenerServiceRanking::New((INotificationListenerServiceRanking**)&mRankingA);
    CNotificationListenerServiceRanking::New((INotificationListenerServiceRanking**)&mRankingB);
}

ECode NotificationData::Comparator::Compare(
    /* [in] */ IInterface* _a,
    /* [in] */ IInterface* _b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // Upsort current media notification.
    String mediaNotification;
    mHost->mEnvironment->GetCurrentMediaNotificationKey(&mediaNotification);
    Entry* a = (Entry*)INotificationDataEntry::Probe(_a);
    Entry* b = (Entry*)INotificationDataEntry::Probe(_b);
    Boolean aMedia = a->mKey.Equals(mediaNotification);
    Boolean bMedia = b->mKey.Equals(mediaNotification);
    if (aMedia != bMedia) {
        *result = aMedia ? -1 : 1;
        return NOERROR;
    }

    AutoPtr<IStatusBarNotification> na = a->mNotification;
    AutoPtr<IStatusBarNotification> nb = b->mNotification;

    // Upsort PRIORITY_MAX system notifications
    AutoPtr<INotification> tmpA;
    na->GetNotification((INotification**)&tmpA);
    AutoPtr<INotification> tmpB;
    nb->GetNotification((INotification**)&tmpB);
    Int32 priorityA = 0, priorityB = 0;
    tmpA->GetPriority(&priorityA);
    tmpB->GetPriority(&priorityB);
    Boolean aSystemMax = priorityA >= INotification::PRIORITY_MAX &&
            IsSystemNotification(na);
    Boolean bSystemMax = priorityB >= INotification::PRIORITY_MAX &&
            IsSystemNotification(nb);
    if (aSystemMax != bSystemMax) {
        *result = aSystemMax ? -1 : 1;
        return NOERROR;
    }

    // RankingMap as received from NoMan.
    if (mHost->mRankingMap != NULL) {
        Boolean ret = FALSE;
        mHost->mRankingMap->GetRanking(a->mKey, mRankingA, &ret);
        mHost->mRankingMap->GetRanking(b->mKey, mRankingB, &ret);
        Int32 rankA = 0, rankB = 0;
        mRankingA->GetRank(&rankA);
        mRankingB->GetRank(&rankB);
        *result = rankA - rankB;
        return NOERROR;
    }

    Int32 sA = 0, sB = 0;
    nb->GetScore(&sB);
    na->GetScore(&sA);
    Int32 d = sB - sA;
    if (a->mInterruption != b->mInterruption) {
        *result = a->mInterruption ? -1 : 1;
        return NOERROR;
    }
    else if (d != 0) {
        *result = d;
        return NOERROR;
    }
    else {
        Int64 whenA = 0, whenB = 0;
        tmpA->GetWhen(&whenA);
        tmpB->GetWhen(&whenB);
        *result = (Int32) (whenB - whenA);
        return NOERROR;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(NotificationData, Object, INotificationData)
NotificationData::NotificationData(
    /* [in] */ INotificationEnvironment* environment)
{
    mEnvironment = environment;
    mRankingComparator = new Comparator(this);

    CArrayMap::New((IArrayMap**)&mEntries);
    CArrayList::New((IArrayList**)&mSortedAndFiltered);
    CArraySet::New((IArraySet**)&mGroupsWithSummaries);
    CNotificationListenerServiceRanking::New((INotificationListenerServiceRanking**)&mTmpRanking);
}

ECode NotificationData::GetActiveNotifications(
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mSortedAndFiltered;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode NotificationData::Get(
    /* [in] */ const String& key,
    /* [out] */ INotificationDataEntry** entry)
{
    VALIDATE_NOT_NULL(entry);
    AutoPtr<IInterface> obj;
    AutoPtr<ICharSequence> k;
    CString::New(key, (ICharSequence**)&k);
    mEntries->Get(k, (IInterface**)&obj);
    *entry = INotificationDataEntry::Probe(obj);
    REFCOUNT_ADD(*entry);
    return NOERROR;
}

ECode NotificationData::Add(
    /* [in] */ INotificationDataEntry* entry,
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    String key;
    ((Entry*)entry)->mNotification->GetKey(&key);
    AutoPtr<ICharSequence> k;
    CString::New(key, (ICharSequence**)&k);
    mEntries->Put(k, entry);
    UpdateRankingAndSort(ranking);
    return NOERROR;
}

ECode NotificationData::Remove(
    /* [in] */ const String& key,
    /* [in] */ INotificationListenerServiceRankingMap* ranking,
    /* [out] */ INotificationDataEntry** entry)
{
    VALIDATE_NOT_NULL(entry);
    *entry = NULL;

    AutoPtr<ICharSequence> k;
    CString::New(key, (ICharSequence**)&k);
    AutoPtr<IInterface> obj;
    mEntries->Remove(k, (IInterface**)&obj);
    AutoPtr<INotificationDataEntry> removed = INotificationDataEntry::Probe(obj);
    if (removed == NULL) return NOERROR;
    UpdateRankingAndSort(ranking);
    *entry = removed;
    REFCOUNT_ADD(*entry);
    return NOERROR;
}

ECode NotificationData::UpdateRanking(
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    UpdateRankingAndSort(ranking);
    return NOERROR;
}

ECode NotificationData::IsAmbient(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mRankingMap != NULL) {
        mRankingMap->GetRanking(key, mTmpRanking, result);
        return mTmpRanking->IsAmbient(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode NotificationData::GetVisibilityOverride(
    /* [in] */ const String& key,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mRankingMap != NULL) {
        Boolean tmp = FALSE;
        mRankingMap->GetRanking(key, mTmpRanking, &tmp);
        return mTmpRanking->GetVisibilityOverride(result);
    }
    *result = INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE;
    return NOERROR;
}

void NotificationData::UpdateRankingAndSort(
    /* [in] */ INotificationListenerServiceRankingMap* ranking)
{
    if (ranking != NULL) {
        mRankingMap = ranking;
    }
    FilterAndSort();
}

ECode NotificationData::FilterAndSort()
{
    mSortedAndFiltered->Clear();
    ISet::Probe(mGroupsWithSummaries)->Clear();

    Int32 N = 0;
    mEntries->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mEntries->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(obj);
        AutoPtr<IStatusBarNotification> sbn;
        entry->GetNotification((IStatusBarNotification**)&sbn);

        if (ShouldFilterOut(sbn)) {
            Logger::D("NotificationData", "[TODO] UpdateRankingAndSort has an error????");
            // continue;
        }

        AutoPtr<INotification> n;
        sbn->GetNotification((INotification**)&n);
        Boolean state = FALSE;
        if (n->IsGroupSummary(&state), state) {
            String key;
            sbn->GetGroupKey(&key);
            AutoPtr<ICharSequence> k;
            CString::New(key, (ICharSequence**)&k);
            ISet::Probe(mGroupsWithSummaries)->Add(k);
        }
        mSortedAndFiltered->Add(entry);
    }

    // Second pass: Filter out group children with summary.
    Boolean empty = FALSE;
    if (ISet::Probe(mGroupsWithSummaries)->IsEmpty(&empty), !empty) {
        Int32 M = 0;
        mSortedAndFiltered->GetSize(&M);
        for (Int32 i = M - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mSortedAndFiltered->Get(i, (IInterface**)&obj);
            AutoPtr<INotificationDataEntry> ent = INotificationDataEntry::Probe(obj);
            AutoPtr<IStatusBarNotification> sbn;
            ent->GetNotification((IStatusBarNotification**)&sbn);

            AutoPtr<INotification> n;
            sbn->GetNotification((INotification**)&n);
            Boolean tmp = FALSE;
            if (n->IsGroupChild(&tmp), tmp) {
                String key;
                sbn->GetGroupKey(&key);
                AutoPtr<ICharSequence> k;
                CString::New(key, (ICharSequence**)&k);

                if (ISet::Probe(mGroupsWithSummaries)->Contains(k, &tmp), tmp) {
                    mSortedAndFiltered->Remove(i);
                }
            }
        }
    }

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(IList::Probe(mSortedAndFiltered), mRankingComparator);
    return NOERROR;
}

ECode NotificationData::IsGroupWithSummary(
    /* [in] */ const String& groupKey,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICharSequence> k;
    CString::New(groupKey, (ICharSequence**)&k);
    return ISet::Probe(mGroupsWithSummaries)->Contains(k, result);
}

Boolean NotificationData::ShouldFilterOut(
    /* [in] */ IStatusBarNotification* sbn)
{
    Boolean tmp = FALSE;
    if (!((mEnvironment->IsDeviceProvisioned(&tmp), tmp) ||
            ShowNotificationEvenIfUnprovisioned(sbn))) {
        return TRUE;
    }

    if (mEnvironment->IsNotificationForCurrentProfiles(sbn, &tmp), !tmp) {
        return TRUE;
    }

    AutoPtr<INotification> n;
    sbn->GetNotification((INotification**)&n);
    Int32 visibility = 0, id = 0;
    n->GetVisibility(&visibility);
    sbn->GetUserId(&id);
    if (visibility == INotification::VISIBILITY_SECRET &&
            (mEnvironment->ShouldHideSensitiveContents(id, &tmp), tmp)) {
        return TRUE;
    }
    return FALSE;
}

ECode NotificationData::HasActiveClearableNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> it;
    mSortedAndFiltered->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Entry* e = (Entry*)INotificationDataEntry::Probe(obj);
        if (e->mExpanded.Get() != NULL) { // the view successfully inflated
            Boolean clearable = FALSE;
            if (e->mNotification->IsClearable(&clearable), clearable) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }

    *result = FALSE;
    return NOERROR;
}

Boolean NotificationData::ShowNotificationEvenIfUnprovisioned(
    /* [in] */ IStatusBarNotification* sbn)
{
    AutoPtr<INotification> n;
    sbn->GetNotification((INotification**)&n);
    AutoPtr<IBundle> data;
    n->GetExtras((IBundle**)&data);
    Boolean tmp = FALSE;
    data->GetBoolean(INotification::EXTRA_ALLOW_DURING_SETUP, &tmp);
    String name;
    sbn->GetPackageName(&name);
    return String("android").Equals(name) && tmp;
}

void NotificationData::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& indent)
{
    Int32 N = 0;
    mSortedAndFiltered->GetSize(&N);
    pw->Print(indent);
    pw->Println(String("active notifications: ") + StringUtils::ToString(N));
    Int32 active = 0;
    for (active = 0; active < N; active++) {
        AutoPtr<IInterface> obj;
        mSortedAndFiltered->Get(active, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> e = INotificationDataEntry::Probe(obj);
        DumpEntry(pw, indent, active, (Entry*)e.Get());
    }

    Int32 M = 0;
    mEntries->GetSize(&M);
    pw->Print(indent);
    pw->Println(String("inactive notifications: ") + StringUtils::ToString(M - active));
    Int32 inactiveCount = 0;
    for (Int32 i = 0; i < M; i++) {
        AutoPtr<IInterface> obj;
        mEntries->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<INotificationDataEntry> entry = INotificationDataEntry::Probe(obj);
        Boolean tmp = FALSE;
        if (mSortedAndFiltered->Contains(entry, &tmp), !tmp) {
            DumpEntry(pw, indent, inactiveCount, (Entry*)entry.Get());
            inactiveCount++;
        }
    }
}

void NotificationData::DumpEntry(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& indent,
    /* [in] */ Int32 i,
    /* [in] */ Entry* e)
{
    pw->Print(indent);
    pw->Println(String("  [") + StringUtils::ToString(i) + "] key=" + e->mKey + " icon="
        + StringUtils::ToHexString((Int32)e->mIcon.Get()));
    AutoPtr<IStatusBarNotification> n = e->mNotification;
    pw->Print(indent);
    String value;
    n->GetPackageName(&value);
    Int32 id = 0, s = 0;
    n->GetId(&id);
    n->GetScore(&s);
    pw->Println(String("      pkg=") + value
            + " id=" + StringUtils::ToString(id) + " score=" + StringUtils::ToString(s));
    pw->Print(indent);
    AutoPtr<INotification> tmpN;
    n->GetNotification((INotification**)&tmpN);
    pw->Println(String("      notification=") + StringUtils::ToHexString((Int32)tmpN.Get()));
    pw->Print(indent);
    AutoPtr<ICharSequence> tickerText;
    tmpN->GetTickerText((ICharSequence**)&tickerText);
    tickerText->ToString(&value);
    pw->Println(String("      tickerText=\"") + value + "\"");
}

Boolean NotificationData::IsSystemNotification(
    /* [in] */ IStatusBarNotification* sbn)
{
    String sbnPackage;
    sbn->GetPackageName(&sbnPackage);
    return String("android").Equals(sbnPackage) || String("Elastos.Droid.SystemUI").Equals(sbnPackage);
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
