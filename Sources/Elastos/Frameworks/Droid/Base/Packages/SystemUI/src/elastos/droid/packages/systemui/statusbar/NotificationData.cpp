#include "elastos/droid/systemui/statusbar/NotificationData.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Droid::App::INotification;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


//=============================================================================
//              NotificationDataEntry
//=============================================================================
NotificationDataEntry::NotificationDataEntry()
{
}

NotificationDataEntry::NotificationDataEntry(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ IStatusBarIconView* ic)
    : mKey(key)
    , mNotification(n)
    , mIcon(ic)
{
}

void NotificationDataEntry::SetLargeView(
    /* [in] */ IView* expandedLarge)
{
    mExpanded = expandedLarge;
    NotificationData::WriteBooleanTag(mRow, SystemUIR::id::expandable_tag, expandedLarge != NULL);
}

AutoPtr<IView> NotificationDataEntry::GetLargeView()
{
    return mExpanded;
}

/**
 * Return whether the entry can be expanded.
 */
Boolean NotificationDataEntry::Expandable()
{
    return NotificationData::GetIsExpandable(mRow);
}

/**
 * Return whether the entry has been manually expanded by the user.
 */
Boolean NotificationDataEntry::UserExpanded()
{
    return NotificationData::GetUserExpanded(mRow);
}

Boolean NotificationDataEntry::SetUserExpanded(
    /* [in] */ Boolean userExpanded)
{
    return NotificationData::SetUserExpanded(mRow, userExpanded);
}

/**
 * Return whether the entry is being touched by the user.
 */
Boolean NotificationDataEntry::UserLocked()
{
    return NotificationData::GetUserLocked(mRow);
}

/**
 * Set the flag indicating that this is being touched by the user.
 */
Boolean NotificationDataEntry::SetUserLocked(
    /* [in] */ Boolean userLocked)
{
    return NotificationData::SetUserLocked(mRow, userLocked);
}

Int32 NotificationDataEntry::Compare(
    /* [in] */ const NotificationDataEntry* lhs,
    /* [in] */ const NotificationDataEntry* rhs)
{
    assert(lhs != NULL && rhs != NULL);
    if (lhs == rhs) return 0;

    // sort first by score, then by when
    Int32 score, otherScore;
    lhs->mNotification->GetScore(&score);
    rhs->mNotification->GetScore(&otherScore);

    Int32 d = score - otherScore;
    if (d != 0) return d;

    Int64 when, otherWhen;
    AutoPtr<INotification> ln, rn;

    lhs->mNotification->GetNotification((INotification**)&ln);
    ln->GetWhen(&when);
    rhs->mNotification->GetNotification((INotification**)&rn);
    rn->GetWhen(&otherWhen);
    return (Int32)(when - otherWhen);
}

//=============================================================================
//              NotificationData
//=============================================================================
Int32 NotificationData::Size()
{
    return mEntries.GetSize();
}

AutoPtr<NotificationDataEntry> NotificationData::Get(
    /* [in] */ Int32 i)
{
    return mEntries[i];
}

List<AutoPtr<NotificationDataEntry> >::Iterator NotificationData::FindIteratorByKey(
    /* [in] */ IBinder* key)
{
    List<AutoPtr<NotificationDataEntry> >::Iterator it;
    for (it = mEntries.Begin(); it != mEntries.End(); ++it) {
        if ((*it)->mKey.Get() == key) {
            return it;
        }
    }

    return it;
}

AutoPtr<NotificationDataEntry> NotificationData::FindByKey(
    /* [in] */ IBinder* key)
{
    List<AutoPtr<NotificationDataEntry> >::Iterator it;
    for (it = mEntries.Begin(); it != mEntries.End(); ++it) {
        if ((*it)->mKey.Get() == key) {
            return *it;
        }
    }

    return NULL;
}

Int32 NotificationData::Add(
    /* [in] */ NotificationDataEntry* entry)
{
    Int32 i = 0;
    List<AutoPtr<NotificationDataEntry> >::Iterator it;
    for (it = mEntries.Begin(); it != mEntries.End(); ++it, ++i) {
        if (NotificationDataEntry::Compare(*it, entry) > 0) {
            break;
        }
    }

    AutoPtr<NotificationDataEntry> ne = entry;
    mEntries.Insert(it, ne);
    return i;
}

Int32 NotificationData::Add(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification,
    /* [in] */ IView* row,
    /* [in] */ IView* content,
    /* [in] */ IView* expanded,
    /* [in] */ IStatusBarIconView* icon)
{
    AutoPtr<NotificationDataEntry> entry = new NotificationDataEntry();
    entry->mKey = key;
    entry->mNotification = notification;
    entry->mRow = row;
    entry->mContent = content;
    entry->mExpanded = expanded;
    entry->mIcon = icon;
    entry->mLargeIcon = NULL; // TODO add support for large icons
    return Add(entry);
}

AutoPtr<NotificationDataEntry> NotificationData::Remove(
    /* [in] */ IBinder* key)
{
    AutoPtr<NotificationDataEntry> e;
    List<AutoPtr<NotificationDataEntry> >::Iterator it = FindIteratorByKey(key);
    if (it != mEntries.End()) {
        e = *it;
        mEntries.Erase(it);
    }
    return e;
}

/**
 * Return whether there are any visible items (i.e. items without an error).
 */
Boolean NotificationData::HasVisibleItems()
{
    List<AutoPtr<NotificationDataEntry> >::Iterator it;
    for (it = mEntries.Begin(); it != mEntries.End(); ++it) {
        if ((*it)->mExpanded != NULL) { // the view successfully inflated
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Return whether there are any clearable items (that aren't errors).
 */
Boolean NotificationData::HasClearableItems()
{
    Boolean isClearable;
    List<AutoPtr<NotificationDataEntry> >::Iterator it;
    for (it = mEntries.Begin(); it != mEntries.End(); ++it) {
        if ((*it)->mExpanded != NULL) { // the view successfully inflated
            (*it)->mNotification->IsClearable(&isClearable);
            if (isClearable)
                return TRUE;
        }
    }

    return FALSE;
}

Boolean NotificationData::ReadBooleanTag(
    /* [in] */ IView* view,
    /* [in] */ Int32 id)
{
    if (view != NULL) {
        AutoPtr<IInterface> value;
        view->GetTag(id, (IInterface**)&value);
        if (IBoolean::Probe(value) != NULL) {
            Boolean result;
            IBoolean::Probe(value)->GetValue(&result);
            return result;
        }
    }

    return FALSE;
}

Boolean NotificationData::WriteBooleanTag(
    /* [in] */ IView* view,
    /* [in] */ Int32 id,
    /* [in] */ Boolean value)
{
    if (view != NULL) {
        AutoPtr<IBoolean> bVal;
        CBoolean::New(value, (IBoolean**)&bVal);
        view->SetTag(id, bVal);
        return value;
    }

    return FALSE;
}

/**
 * Return whether the entry can be expanded.
 */
Boolean NotificationData::GetIsExpandable(
    /* [in] */ IView* row)
{
    return ReadBooleanTag(row, SystemUIR::id::expandable_tag);
}

/**
 * Return whether the entry has been manually expanded by the user.
 */
Boolean NotificationData::GetUserExpanded(
    /* [in] */ IView* row)
{
    return ReadBooleanTag(row, SystemUIR::id::user_expanded_tag);
}

/**
 * Set whether the entry has been manually expanded by the user.
 */
Boolean NotificationData::SetUserExpanded(
    /* [in] */ IView* row,
    /* [in] */ Boolean userExpanded)
{
    return WriteBooleanTag(row, SystemUIR::id::user_expanded_tag, userExpanded);
}

/**
 * Return whether the entry is being touched by the user.
 */
Boolean NotificationData::GetUserLocked(
    /* [in] */ IView* row)
{
    return ReadBooleanTag(row, SystemUIR::id::user_lock_tag);
}

/**
 * Set whether the entry is being touched by the user.
 */
Boolean NotificationData::SetUserLocked(
    /* [in] */ IView* row,
    /* [in] */ Boolean userLocked)
{
    return WriteBooleanTag(row, SystemUIR::id::user_lock_tag, userLocked);
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
