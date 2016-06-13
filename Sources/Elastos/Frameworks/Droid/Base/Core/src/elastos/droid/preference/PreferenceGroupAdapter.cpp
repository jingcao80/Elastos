
#include "elastos/droid/preference/PreferenceGroupAdapter.h"
#include "elastos/droid/preference/Preference.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/widget/CFrameLayout.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::Widget::CFrameLayout;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::EIID_IBaseAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::Math;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

//====================================================
// PreferenceGroupAdapter::PreferenceLayout
//====================================================

CAR_INTERFACE_IMPL_2(PreferenceGroupAdapter::PreferenceLayout, Object, IPreferenceLayout, IComparable)

PreferenceGroupAdapter::PreferenceLayout::PreferenceLayout()
    : mResId(0)
    , mWidgetResId(0)
{
}

ECode PreferenceGroupAdapter::PreferenceLayout::CompareTo(
    /* [in] */ IInterface* otherObj,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPreferenceLayout> _otherObj = IPreferenceLayout::Probe(otherObj);
    if (_otherObj != NULL) {
        String name;
        _otherObj->GetName(&name);
        Int32 resId, widgetResId;
        _otherObj->GetResId(&resId);
        _otherObj->GetWidgetResId(&widgetResId);
        Int32 compareNames = mName.Compare(name);
        if (compareNames == 0) {
            if (mResId == resId) {
                if (mWidgetResId == widgetResId) {
                    *result = 0;
                    return NOERROR;
                }
                else {
                    *result = mWidgetResId - widgetResId;
                    return NOERROR;
                }
            }
            else {
                *result = mResId - resId;
                return NOERROR;
            }
        }
        *result = compareNames;
    }
    return NOERROR;
}

ECode PreferenceGroupAdapter::PreferenceLayout::GetResId(
    /* [out]  */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId)
    *resId = mResId;
    return NOERROR;
}

ECode PreferenceGroupAdapter::PreferenceLayout::SetResId(
    /* [in]  */ Int32 resId)
{
    mResId = resId;
    return NOERROR;
}

ECode PreferenceGroupAdapter::PreferenceLayout::GetWidgetResId(
    /* [out]  */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId)
    *resId = mWidgetResId;
    return NOERROR;
}

ECode PreferenceGroupAdapter::PreferenceLayout::SetWidgetResId(
    /* [in]  */ Int32 resId)
{
    mWidgetResId = resId;
    return NOERROR;
}

ECode PreferenceGroupAdapter::PreferenceLayout::GetName(
    /* [out]  */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode PreferenceGroupAdapter::PreferenceLayout::SetName(
    /* [in]  */ const String & name)
{
    mName = name;
    return NOERROR;
}

//====================================================
// PreferenceGroupAdapter::SyncRunnable
//====================================================

PreferenceGroupAdapter::SyncRunnable::SyncRunnable(
    /* [in] */ PreferenceGroupAdapter* host)
    : mHost(host)
{}

ECode PreferenceGroupAdapter::SyncRunnable::Run()
{
    mHost->SyncMyPreferences();
    return NOERROR;
}


//====================================================
// PreferenceGroupAdapter
//====================================================

static AutoPtr<IViewGroupLayoutParams> init()
{
    AutoPtr<IViewGroupLayoutParams> vglp;
    CViewGroupLayoutParams::New(
        IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::WRAP_CONTENT,
        (IViewGroupLayoutParams**)&vglp);
    return vglp;
}

AutoPtr<IViewGroupLayoutParams> PreferenceGroupAdapter::sWrapperLayoutParams = init();

const String PreferenceGroupAdapter::TAG("PreferenceGroupAdapter");

CAR_INTERFACE_IMPL_2(PreferenceGroupAdapter, BaseAdapter, IPreferenceGroupAdapter, IPreferenceOnPreferenceChangeInternalListener)

PreferenceGroupAdapter::PreferenceGroupAdapter()
    : mHasReturnedViewTypeCount(FALSE)
    , mIsSyncing(FALSE)
    , mHighlightedPosition(-1)
{
    mTempPreferenceLayout = new PreferenceLayout();
    CHandler::New((IHandler**)&mHandler);
    mSyncRunnable = new SyncRunnable(this);
}

ECode PreferenceGroupAdapter::constructor(
    /* [in] */ IPreferenceGroup* preferenceGroup)
{
    AutoPtr<IWeakReference> wr;
    IWeakReferenceSource::Probe(preferenceGroup)->GetWeakReference((IWeakReference**)&wr);
    mWeakPreferenceGroup = wr.Get();

    AutoPtr<IPreferenceGroup> _mPreferenceGroup;
    wr->Resolve(EIID_IPreferenceGroup, (IInterface**)&_mPreferenceGroup);
    IPreference::Probe(_mPreferenceGroup)->SetOnPreferenceChangeInternalListener(this);

    CArrayList::New((IList**)&mPreferenceList);
    CArrayList::New((IList**)&mPreferenceLayouts);

    SyncMyPreferences();
    return NOERROR;
}

void PreferenceGroupAdapter::SyncMyPreferences()
{
    {
        AutoLock syncLock(this);
        if (mIsSyncing) {
            return;
        }

        mIsSyncing = TRUE;
    }

    Int32 size;
    mPreferenceList->GetSize(&size);
    AutoPtr<IList> newPreferenceList;
    CArrayList::New(size, (IList**)&newPreferenceList);
    AutoPtr<IPreferenceGroup> group;
    mWeakPreferenceGroup->Resolve(EIID_IPreferenceGroup, (IInterface**)&group);
    FlattenPreferenceGroup(newPreferenceList, group);
    mPreferenceList = newPreferenceList;

    BaseAdapter::NotifyDataSetChanged();

    {
        AutoLock syncLock(this);
        mIsSyncing = FALSE;
        mLock.NotifyAll();
    }
}

void PreferenceGroupAdapter::FlattenPreferenceGroup(
    /* [in] */ IList* preferences,
    /* [in] */ IPreferenceGroup* group)
{
    // TODO: shouldn't always?
    group->SortPreferences();

    Int32 groupSize;
    group->GetPreferenceCount(&groupSize);
    for (Int32 i = 0; i < groupSize; i++) {
        AutoPtr<IPreference> preference;
        group->GetPreference(i, (IPreference**)&preference);

        preferences->Add(preference);

        Boolean canRecycleLayout;
        if (!mHasReturnedViewTypeCount && (preference->CanRecycleLayout(&canRecycleLayout), canRecycleLayout)) {
            AddPreferenceClassName(preference);
        }

        if (IPreferenceGroup::Probe(preference) != NULL) {
            AutoPtr<IPreferenceGroup> preferenceGroup = IPreferenceGroup::Probe(preference);
            Boolean isOnSameScreenAsChildren;
            if (preferenceGroup->IsOnSameScreenAsChildren(&isOnSameScreenAsChildren), isOnSameScreenAsChildren) {
                FlattenPreferenceGroup(preferences, preferenceGroup);
            }
        }

        preference->SetOnPreferenceChangeInternalListener(this);
    }
}

AutoPtr<IPreferenceLayout> PreferenceGroupAdapter::CreatePreferenceLayout(
    /* [in] */ IPreference* preference,
    /* [in] */ IPreferenceLayout* in)
{
    AutoPtr<IPreferenceLayout> pl = in != NULL ? in : new PreferenceLayout();

    pl->SetName(Object::GetFullClassName(preference));

    Int32 resId, widgetResId;
    preference->GetLayoutResource(&resId);
    pl->SetResId(resId);
    preference->GetWidgetLayoutResource(&widgetResId);
    pl->SetWidgetResId(widgetResId);
    return pl;
}

void PreferenceGroupAdapter::AddPreferenceClassName(
    /* [in] */ IPreference* preference)
{
    AutoPtr<IPreferenceLayout> pl = CreatePreferenceLayout(preference, NULL);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Int32 insertPos;
    collections->BinarySearch(mPreferenceLayouts, pl, &insertPos);

    // Only insert if it doesn't exist (when it is negative).
    if (insertPos < 0) {
        // Convert to insert index
        insertPos = insertPos * -1 - 1;
        mPreferenceLayouts->Add(insertPos, pl);
    }
}

ECode PreferenceGroupAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mPreferenceList->GetSize(count);
}

ECode PreferenceGroupAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    Int32 count;
    if (position < 0 || (GetCount(&count), position >= count)) {
        *item = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mPreferenceList->Get(position, (IInterface**)&obj);
    *item = obj;
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode PreferenceGroupAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    Int32 count;
    if (position < 0 || (GetCount(&count), position >= count)) {
        *id = IAdapterView::INVALID_ROW_ID;
        return NOERROR;
    }
    AutoPtr<IInterface> p;
    GetItem(position, (IInterface**)&p);
    return IPreference::Probe(p)->GetId(id);
}

ECode PreferenceGroupAdapter::SetHighlighted(
    /* [in] */ Int32 position)
{
    mHighlightedPosition = position;
    return NOERROR;
}

ECode PreferenceGroupAdapter::SetHighlightedDrawable(
    /* [in] */ IDrawable* drawable)
{
    mHighlightedDrawable = drawable;
    return NOERROR;
}

ECode PreferenceGroupAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<IPreference> preference = IPreference::Probe(obj);
    // Build a PreferenceLayout to compare with known ones that are cacheable.
    mTempPreferenceLayout = CreatePreferenceLayout(preference, mTempPreferenceLayout);

    // If it's not one of the cached ones, set the convertView to null so that
    // the layout gets re-created by the Preference.
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Int32 index, type, viewType;
    if ((collections->BinarySearch(mPreferenceLayouts, mTempPreferenceLayout, &index), index < 0) ||
        (GetItemViewType(position, &type),  GetHighlightItemViewType(&viewType), type == viewType)) {
        convertView = NULL;
    }
    AutoPtr<IView> result;
    preference->GetView(convertView, parent, (IView**)&result);
    if (position == mHighlightedPosition && mHighlightedDrawable != NULL) {
        AutoPtr<IContext> ctx;
        IView::Probe(parent)->GetContext((IContext**)&ctx);
        AutoPtr<IViewGroup> wrapper;
        CFrameLayout::New(ctx, (IViewGroup**)&wrapper);
        AutoPtr<IView> vwrapper = IView::Probe(wrapper);
        vwrapper->SetLayoutParams(sWrapperLayoutParams);
        vwrapper->SetBackgroundDrawable(mHighlightedDrawable);
        wrapper->AddView(result);
        result = IView::Probe(wrapper);
    }
    *view = result;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode PreferenceGroupAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    Int32 count;
    if (position < 0 || (GetCount(&count), position >= count)) {
        *enabled = TRUE;
        return NOERROR;
    }
    AutoPtr<IInterface> p;
    GetItem(position, (IInterface**)&p);
    return IPreference::Probe(p)->IsSelectable(enabled);
}

ECode PreferenceGroupAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    // There should always be a preference group, and these groups are always disabled
    *enabled = FALSE;
    return NOERROR;
}

ECode PreferenceGroupAdapter::OnPreferenceChange(
    /* [in] */ IPreference* preference)
{
    return BaseAdapter::NotifyDataSetChanged();
}

ECode PreferenceGroupAdapter::OnPreferenceHierarchyChange(
    /* [in] */ IPreference* preference)
{
    mHandler->RemoveCallbacks(mSyncRunnable);
    Boolean result;
    return mHandler->Post(mSyncRunnable, &result);
}

ECode PreferenceGroupAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds)
    *hasStableIds = TRUE;
    return NOERROR;
}

ECode PreferenceGroupAdapter::GetHighlightItemViewType(
    /* [out] */ Int32* type)
{
    FAIL_RETURN(GetViewTypeCount(type));
    *type = *type - 1;
    return NOERROR;
}

ECode PreferenceGroupAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    if (position == mHighlightedPosition) {
        return GetHighlightItemViewType(type);
    }

    if (!mHasReturnedViewTypeCount) {
        mHasReturnedViewTypeCount = TRUE;
    }

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<IPreference> preference = IPreference::Probe(obj);
    Boolean canRecycleLayout;
    if (preference->CanRecycleLayout(&canRecycleLayout), !canRecycleLayout) {
        *type = IAdapter::IGNORE_ITEM_VIEW_TYPE;
        return NOERROR;
    }

    mTempPreferenceLayout = CreatePreferenceLayout(preference, mTempPreferenceLayout);

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    Int32 viewType;
    collections->BinarySearch(mPreferenceLayouts, mTempPreferenceLayout, &viewType);
    if (viewType < 0) {
        // This is a class that was seen after we returned the count, so
        // don't recycle it.
        *type = IAdapter::IGNORE_ITEM_VIEW_TYPE;
        return NOERROR;
    }
    else {
        *type = viewType;
        return NOERROR;
    }
}

ECode PreferenceGroupAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (!mHasReturnedViewTypeCount) {
        mHasReturnedViewTypeCount = TRUE;
    }

    Int32 size;
    mPreferenceLayouts->GetSize(&size);
    *count = Elastos::Core::Math::Max(1, size) + 1;
    return NOERROR;
}

}
}
}
