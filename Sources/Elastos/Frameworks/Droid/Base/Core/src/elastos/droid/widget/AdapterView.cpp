
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/widget/AdapterView.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IContextMenuInfo;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::Widget::EIID_IAdapterContextMenuInfo;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 AdapterView::SYNC_SELECTED_POSITION;
const Int32 AdapterView::SYNC_FIRST_POSITION;
const Int32 AdapterView::SYNC_MAX_DURATION_MILLIS;

//==============================================================================
//              AdapterView::AdapterContextMenuInfo
//==============================================================================
CAR_INTERFACE_IMPL_2(AdapterView::AdapterContextMenuInfo, Object, IContextMenuInfo, IAdapterContextMenuInfo)

AdapterView::AdapterContextMenuInfo::AdapterContextMenuInfo(
    /* [in] */ IView* targetView,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
    : mTargetView(targetView)
    , mPosition(position)
    , mId(id)
{
}

ECode AdapterView::AdapterContextMenuInfo::GetTargetView(
    /* [out] */ IView** targetView)
{
    VALIDATE_NOT_NULL(targetView);
    *targetView = mTargetView;
    REFCOUNT_ADD(*targetView);
    return NOERROR;
}

ECode AdapterView::AdapterContextMenuInfo::SetTargetView(
    /* [in] */ IView* targetView)
{
    mTargetView = targetView;
    return NOERROR;
}

ECode AdapterView::AdapterContextMenuInfo::GetPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = mPosition;
    return NOERROR;
}

ECode AdapterView::AdapterContextMenuInfo::SetPosition(
    /* [in] */ Int32 position)
{
    mPosition = position;
    return NOERROR;
}

ECode AdapterView::AdapterContextMenuInfo::GetId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode AdapterView::AdapterContextMenuInfo::SetId(
    /* [in] */ Int64 id)
{
    mId = id;
    return NOERROR;
}

//==============================================================================
//              AdapterView::AdapterDataSetObserver
//==============================================================================
CAR_INTERFACE_IMPL(AdapterView::AdapterDataSetObserver, Object, IDataSetObserver);

AdapterView::AdapterDataSetObserver::AdapterDataSetObserver(
    /* [in] */ AdapterView* host)
    : mHost(host)
{
    assert(mHost);
}

ECode AdapterView::AdapterDataSetObserver::OnChanged()
{
    mHost->mDataChanged = TRUE;
    mHost->mOldItemCount = mHost->mItemCount;

    AutoPtr<IAdapter> adapter;
    ((IAdapterView*)(mHost->Probe(EIID_IAdapterView)))->GetAdapter((IAdapter**)&adapter);
    assert(adapter.Get());
    adapter->GetCount(&mHost->mItemCount);

    // Detect the case where a cursor that was previously invalidated has
    // been repopulated with new data.
    Boolean hasStableIds;
    adapter->HasStableIds(&hasStableIds);
    if (hasStableIds && mInstanceState != NULL
        && mHost->mOldItemCount == 0 && mHost->mItemCount > 0)
    {
        mHost->OnRestoreInstanceState(mInstanceState);
        mInstanceState = NULL;
    }
    else {
        mHost->RememberSyncState();
    }

    mHost->CheckFocus();
    mHost->RequestLayout();
    return NOERROR;
}

//@Override
ECode AdapterView::AdapterDataSetObserver::OnInvalidated()
{
    mHost->mDataChanged = TRUE;
    AutoPtr<IAdapter> adapter;
    ((IAdapterView*)(mHost->Probe(EIID_IAdapterView)))->GetAdapter((IAdapter**)&adapter);
    assert(adapter.Get());
    Boolean hasStableIds;
    adapter->HasStableIds(&hasStableIds);
    if (hasStableIds) {
        // Remember the current state for the case where our hosting activity is being
        // stopped and later restarted
        mInstanceState = mHost->OnSaveInstanceState();
    }

    // Data is invalid so we should reset our state
    mHost->mOldItemCount = mHost->mItemCount;
    mHost->mItemCount = 0;
    mHost->mSelectedPosition = IAdapterView::INVALID_POSITION;
    mHost->mSelectedRowId = IAdapterView::INVALID_ROW_ID;
    mHost->mNextSelectedPosition = IAdapterView::INVALID_POSITION;
    mHost->mNextSelectedRowId = IAdapterView::INVALID_POSITION;
    mHost->mNeedSync = FALSE;

    mHost->CheckFocus();
    mHost->RequestLayout();
    return NOERROR;
}

ECode AdapterView::AdapterDataSetObserver::ClearSavedState()
{
    mInstanceState = NULL;
    return NOERROR;
}

//==============================================================================
//                      AdapterView::SelectionNotifier
//==============================================================================
CAR_INTERFACE_IMPL(AdapterView::SelectionNotifier, Object, IRunnable)

AdapterView::SelectionNotifier::SelectionNotifier(
    /* [in] */ AdapterView* host)
    : mHost(host)
{
    assert(host);
}

ECode AdapterView::SelectionNotifier::Run()
{
    if (mHost->mDataChanged) {
        // Data has changed between when this SelectionNotifier
        // was posted and now. We need to wait until the AdapterView
        // has been synched to the new data.
        AutoPtr<IAdapter> adapter;
        ((IAdapterView*)(mHost->Probe(EIID_IAdapterView)))->GetAdapter((IAdapter**)&adapter);
        if (adapter != NULL) {
            AutoPtr<IRunnable> r = this;
            Boolean resTmp;
            mHost->Post(r, &resTmp);
        }
    }
    else {
        mHost->FireOnSelected();
        mHost->PerformAccessibilityActionsOnSelected();
    }
    return NOERROR;
}

//==============================================================================
//                              AdapterView
//==============================================================================
CAR_INTERFACE_IMPL(AdapterView, ViewGroup, IAdapterView)

AdapterView::AdapterView()
    : mFirstPosition(0)
    , mSpecificTop(0)
    , mSyncPosition(0)
    , mSyncRowId(IAdapterView::INVALID_ROW_ID)
    , mSyncHeight(0)
    , mNeedSync(FALSE)
    , mSyncMode(0)
    , mInLayout(FALSE)
    , mDataChanged(FALSE)
    , mNextSelectedPosition(IAdapterView::INVALID_POSITION)
    , mNextSelectedRowId(IAdapterView::INVALID_ROW_ID)
    , mSelectedPosition(IAdapterView::INVALID_POSITION)
    , mSelectedRowId(IAdapterView::INVALID_ROW_ID)
    , mItemCount(0)
    , mOldItemCount(0)
    , mOldSelectedPosition(IAdapterView::INVALID_POSITION)
    , mOldSelectedRowId(IAdapterView::INVALID_ROW_ID)
    , mBlockLayoutRequests(FALSE)
    , mLayoutHeight(0)
    , mDesiredFocusableState(FALSE)
    , mDesiredFocusableInTouchModeState(FALSE)
{
}

ECode AdapterView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode AdapterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode AdapterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return constructor(context, attrs, defStyle, 0);
}

ECode AdapterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle,
    /* [in] */ Int32 defStyleRes)
{
    ViewGroup::constructor(context, attrs, defStyle, defStyleRes);
    // If not explicitly specified this view is important for accessibility.
    Int32 intTmp = 0;
    GetImportantForAccessibility(&intTmp);
    if (intTmp == IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

/**
 * Register a callback to be invoked when an item in this AdapterView has
 * been clicked.
 *
 * @param listener The callback that will be invoked.
 */
ECode AdapterView::SetOnItemClickListener(
    /* [in] */ IAdapterViewOnItemClickListener* listener)
{
    mOnItemClickListener = listener;
    return NOERROR;
}

/**
 * @return The callback to be invoked with an item in this AdapterView has
 *         been clicked, or NULL id no callback has been set.
 */
ECode AdapterView::GetOnItemClickListener(
    /* [out] */ IAdapterViewOnItemClickListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOnItemClickListener;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Call the OnItemClickListener, if it is defined. Performs all normal
 * actions associated with clicking: reporting accessibility event, playing
 * a sound, etc.
 *
 * @param view The view within the AdapterView that was clicked.
 * @param position The position of the view in the adapter.
 * @param id The row id of the item that was clicked.
 * @return True if there was an assigned OnItemClickListener that was
 *         called, false otherwise is returned.
 */
ECode AdapterView::PerformItemClick(
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(result);

    if (mOnItemClickListener != NULL) {
        PlaySoundEffect(SoundEffectConstants::CLICK);
        mOnItemClickListener->OnItemClick(this, view, position, id);
        if (view != NULL) {
           IAccessibilityEventSource* temp = IAccessibilityEventSource::Probe(view);
           temp->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_CLICKED);
        }
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

/**
 * Register a callback to be invoked when an item in this AdapterView has
 * been clicked and held
 *
 * @param listener The callback that will run
 */
ECode AdapterView::SetOnItemLongClickListener(
    /* [in] */ IAdapterViewOnItemLongClickListener* listener)
{
    Boolean click = FALSE;
    IsLongClickable(&click);
    if (!click) {
        SetLongClickable(TRUE);
    }
    mOnItemLongClickListener = listener;
    return NOERROR;
}

/**
 * @return The callback to be invoked with an item in this AdapterView has
 *         been clicked and held, or NULL id no callback as been set.
 */
ECode AdapterView::GetOnItemLongClickListener(
    /* [out] */ IAdapterViewOnItemLongClickListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOnItemLongClickListener;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Register a callback to be invoked when an item in this AdapterView has
 * been selected.
 *
 * @param listener The callback that will run
 */
ECode AdapterView::SetOnItemSelectedListener(
    /* [in] */ IAdapterViewOnItemSelectedListener* listener)
{
    mOnItemSelectedListener = listener;
    return NOERROR;
}

ECode AdapterView::GetOnItemSelectedListener(
    /* [out] */ IAdapterViewOnItemSelectedListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOnItemSelectedListener;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * This method is not supported and throws an UnsupportedOperationException when called.
 *
 * @param child Ignored.
 *
 * @throws UnsupportedOperationException Every time this method is invoked.
 */
//@Override
ECode AdapterView::AddView(
    /* [in] */ IView* child)
{
    Logger::E("AdapterView", "addView(View) is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

/**
 * This method is not supported and throws an UnsupportedOperationException when called.
 *
 * @param child Ignored.
 * @param index Ignored.
 *
 * @throws UnsupportedOperationException Every time this method is invoked.
 */
//@Override
ECode AdapterView::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index)
{
    Logger::E("AdapterView",
        "addView(View, Int32) is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode AdapterView::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Logger::E("AdapterView",
        "addView(View, Int32) is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

/**
 * This method is not supported and throws an UnsupportedOperationException when called.
 *
 * @param child Ignored.
 * @param params Ignored.
 *
 * @throws UnsupportedOperationException Every time this method is invoked.
 */
//@Override
ECode AdapterView::AddView(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* params)
{
    Logger::E("AdapterView",
        "addView(View, LayoutParams) is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

/**
 * This method is not supported and throws an UnsupportedOperationException when called.
 *
 * @param child Ignored.
 * @param index Ignored.
 * @param params Ignored.
 *
 * @throws UnsupportedOperationException Every time this method is invoked.
 */
//@Override
ECode AdapterView::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    Logger::E("AdapterView",
        "addView(View, Int32, LayoutParams) is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

/**
 * This method is not supported and throws an UnsupportedOperationException when called.
 *
 * @param child Ignored.
 *
 * @throws UnsupportedOperationException Every time this method is invoked.
 */
//@Override
ECode AdapterView::RemoveView(
    /* [in] */ IView* child)
{
    Logger::E("AdapterView",
        "removeView(View) is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

/**
 * This method is not supported and throws an UnsupportedOperationException when called.
 *
 * @param index Ignored.
 *
 * @throws UnsupportedOperationException Every time this method is invoked.
 */
//@Override
ECode AdapterView::RemoveViewAt(
    /* [in] */ Int32 index)
{
    Logger::E("AdapterView",
        "removeViewAt(Int32) is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

/**
 * This method is not supported and throws an UnsupportedOperationException when called.
 *
 * @throws UnsupportedOperationException Every time this method is invoked.
 */
//@Override
ECode AdapterView::RemoveAllViews()
{
    Logger::E("AdapterView",
        "removeAllViews() is not supported in AdapterView");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//@Override
ECode AdapterView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    GetHeight(&mLayoutHeight);
    return NOERROR;
}

/**
 * Return the position of the currently selected item within the adapter's data set
 *
 * @return Int32 Position (starting at 0), or {@link #IAdapterView::INVALID_POSITION} if there is nothing selected.
 */
//@ViewDebug.CapturedViewProperty
ECode AdapterView::GetSelectedItemPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNextSelectedPosition;
    return NOERROR;
}

/**
 * @return The id corresponding to the currently selected item, or {@link #IAdapterView::INVALID_ROW_ID}
 * if nothing is selected.
 */
//@ViewDebug.CapturedViewProperty
ECode AdapterView::GetSelectedItemId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNextSelectedRowId;
    return NOERROR;
}

/**
 * @return The data corresponding to the currently selected item, or
 * NULL if there is nothing selected.
 */
ECode AdapterView::GetSelectedItem(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    Int32 selection = 0;
    GetSelectedItemPosition(&selection);
    if (adapter != NULL) {
        Int32 count;
        adapter->GetCount(&count);
        if (count > 0 && selection >= 0) {
            adapter->GetItem(selection, result);
        }
    }

    *result = NULL;
    return NOERROR;
}

/**
 * @return The number of items owned by the Adapter associated with this
 *         AdapterView. (This is the number of data items, which may be
 *         larger than the number of visible view.)
 */
//@ViewDebug.CapturedViewProperty
ECode AdapterView::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mItemCount;
    return NOERROR;
}

/**
 * Get the position within the adapter's data set for the view, where view is a an adapter item
 * or a descendant of an adapter item.
 *
 * @param view an adapter item, or a descendant of an adapter item. This must be visible in this
 *        AdapterView at the time of the call.
 * @return the position within the adapter's data set of the view, or {@link #IAdapterView::INVALID_POSITION}
 *         if the view does not correspond to a list item (or it is not currently visible).
 */
ECode AdapterView::GetPositionForView(
    /* [in] */ IView* view,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(view);
    VALIDATE_NOT_NULL(result);

    AutoPtr<IView> listItem = view;
    AutoPtr<IViewParent> vp;
    listItem->GetParent((IViewParent**)&vp);
    AutoPtr<IView> v = IView::Probe(vp);
    while (TO_IINTERFACE(v) != TO_IINTERFACE(this)) {
        if (v == NULL) {
            *result = IAdapterView::INVALID_POSITION;
            return NOERROR;
        }
        listItem = v;
        vp = NULL;
        listItem->GetParent((IViewParent**)&vp);
        v = IView::Probe(vp);
    }

    // Search the children for the list item
    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; ++i) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child.Get() == listItem) {
            *result = mFirstPosition + i;
            return NOERROR;
        }
    }

    // Child not found!
    *result = IAdapterView::INVALID_POSITION;
    return NOERROR;
}

/**
 * Returns the position within the adapter's data set for the first item
 * displayed on screen.
 *
 * @return The position within the adapter's data set
 */
ECode AdapterView::GetFirstVisiblePosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFirstPosition;
    return NOERROR;
}

/**
 * Returns the position within the adapter's data set for the last item
 * displayed on screen.
 *
 * @return The position within the adapter's data set
 */
ECode AdapterView::GetLastVisiblePosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count = 0;
    GetChildCount(&count);
    *result = mFirstPosition + count - 1;
    return NOERROR;
}

/**
 * Sets the view to show if the adapter is empty
 */
ECode AdapterView::SetEmptyView(
    /* [in] */ IView* emptyView)
{
    mEmptyView = emptyView;

    Int32 accessibility;
    emptyView->GetImportantForAccessibility(&accessibility);
    // If not explicitly specified this view is important for accessibility.
    if (emptyView != NULL
        &&  accessibility== IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
            emptyView->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }

    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    Boolean empty = TRUE;
    if (adapter != NULL) {
        adapter->IsEmpty(&empty);
    }

    UpdateEmptyStatus(empty);
    return NOERROR;
}

/**
 * When the current adapter is empty, the AdapterView can display a special view
 * call the empty view. The empty view is used to provide feedback to the user
 * that no data is available in this AdapterView.
 *
 * @return The view to show if the adapter is empty.
 */
ECode AdapterView::GetEmptyView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEmptyView;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Indicates whether this view is in filter mode. Filter mode can for instance
 * be enabled by a user when typing on the keyboard.
 *
 * @return True if the view is in filter mode, FALSE otherwise.
 */
ECode AdapterView::IsInFilterMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode AdapterView::SetFocusable(
    /* [in] */ Boolean focusable)
{
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    Boolean empty = TRUE;
    if (adapter != NULL) {
        Int32 count;
        adapter->GetCount(&count);
        empty = (count == 0);
    }

    mDesiredFocusableState = focusable;
    if (!focusable) {
        mDesiredFocusableInTouchModeState = FALSE;
    }

    Boolean isInMode = FALSE;
    IsInFilterMode(&isInMode);
    return ViewGroup::SetFocusable(focusable && (!empty || isInMode));
}

//@Override
ECode AdapterView::SetFocusableInTouchMode(
    /* [in] */ Boolean focusable)
{
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    Boolean empty = TRUE;
    if (adapter != NULL) {
        Int32 count;
        adapter->GetCount(&count);
        empty = (count == 0);
    }

    mDesiredFocusableInTouchModeState = focusable;
    if (focusable) {
        mDesiredFocusableState = TRUE;
    }

    Boolean isInMode = FALSE;
    IsInFilterMode(&isInMode);
    return ViewGroup::SetFocusableInTouchMode(
        focusable && (!empty || isInMode));
}

ECode AdapterView::CheckFocus()
{
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    Boolean empty = TRUE;
    if (adapter != NULL) {
        Int32 count;
        adapter->GetCount(&count);
        empty = (count == 0);
    }

    Boolean isInMode = FALSE;
    IsInFilterMode(&isInMode);
    Boolean focusable = !empty || isInMode;
    // The order in which we set focusable in touch mode/focusable may matter
    // for the client, see View.setFocusableInTouchMode() comments for more
    // details
    ViewGroup::SetFocusableInTouchMode(focusable && mDesiredFocusableInTouchModeState);
    ViewGroup::SetFocusable(focusable && mDesiredFocusableState);
    if (mEmptyView != NULL) {
        Boolean isEmpty = TRUE;
        if (adapter != NULL) {
            adapter->IsEmpty(&isEmpty);
        }
        UpdateEmptyStatus(isEmpty);
    }
    return NOERROR;
}

/**
 * Update the status of the list based on the empty parameter.  If empty is TRUE and
 * we have an empty view, display it.  In all the other cases, make sure that the listview
 * is VISIBLE and that the empty view is GONE (if it's not NULL).
 */
void AdapterView::UpdateEmptyStatus(
    /* [in] */ Boolean empty)
{
    Boolean isInMode = FALSE;
    IsInFilterMode(&isInMode);
    if (isInMode) {
        empty = FALSE;
    }

    if (empty) {
        if (mEmptyView != NULL) {
            mEmptyView->SetVisibility(IView::VISIBLE);
            SetVisibility(IView::GONE);
        }
        else {
            // If the caller just removed our empty view, make sure the list view is visible
            SetVisibility(IView::VISIBLE);
        }

        // We are now GONE, so pending layouts will not be dispatched.
        // Force one here to make sure that the state of the list matches
        // the state of the adapter.
        if (mDataChanged) {
            OnLayout(FALSE, mLeft, mTop, mRight, mBottom);
        }
    }
    else {
        if (mEmptyView != NULL) {
            mEmptyView->SetVisibility(IView::GONE);
        }
        SetVisibility(IView::VISIBLE);
    }
}

/**
 * Gets the data associated with the specified position in the list.
 *
 * @param position Which data to get
 * @return The data associated with the specified position in the list
 */
ECode AdapterView::GetItemAtPosition(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    if (adapter == NULL || position < 0) {
        *result = AutoPtr<IInterface>(NULL);
    }
    else {
        adapter->GetItem(position, result);
    }
    return NOERROR;
}

ECode AdapterView::GetItemIdAtPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    if (adapter == NULL || position < 0) {
        *result = IAdapterView::INVALID_ROW_ID;
    }
    else {
        Int64 itemId = 0;
        adapter->GetItemId(position, &itemId);
        *result = itemId;
    }
    return NOERROR;
}

ECode AdapterView::GetAdapter(
    /* [out] */ IAdapter** adapter)
{
    // abstract function, nothing to do here
    return NOERROR;
}

ECode AdapterView::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    // abstract function, nothing to do here
    return NOERROR;
}

ECode AdapterView::GetSelectedItemId(
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = mNextSelectedRowId;
    return NOERROR;
}

ECode AdapterView::GetSelectedView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    // abstract function, nothing to do here
    return NOERROR;
}

ECode AdapterView::SetSelection(
    /* [in] */ Int32 position)
{
    // abstract function, nothing to do here
    return NOERROR;
}

//@Override
ECode AdapterView::SetOnClickListener(
    /* [in] */ IViewOnClickListener* l)
{
    Logger::E("AdapterView", "Don't call setOnClickListener for an AdapterView. "
            "You probably want setOnItemClickListener instead");
    return E_RUNTIME_EXCEPTION;
}

/**
 * Override to prevent freezing of any views created by the adapter.
 */
//@Override
ECode AdapterView::DispatchSaveInstanceState(
    /* [in] */ ISparseArray* container)
{
    return DispatchFreezeSelfOnly(container);
}

/**
 * Override to prevent thawing of any views created by the adapter.
 */
//@Override
ECode AdapterView::DispatchRestoreInstanceState(
    /* [in] */ ISparseArray* container)
{
    return DispatchThawSelfOnly(container);
}

//@Override
ECode AdapterView::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();
    if (mSelectionNotifier != NULL) {
        Boolean resTmp;
        RemoveCallbacks(mSelectionNotifier, &resTmp);
    }
    return NOERROR;
}

void AdapterView::SelectionChanged()
{
    AutoPtr<IAccessibilityManager> manager;
    Boolean enable = TRUE;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&manager);
    manager->IsEnabled(&enable);
    if (mOnItemSelectedListener != NULL || enable) {
        if (mInLayout || mBlockLayoutRequests) {
            // If we are in a layout traversal, defer notification
            // by posting. This ensures that the view tree is
            // in a consistent state and is able to accomodate
            // new layout or invalidate requests.
            if (mSelectionNotifier == NULL) {
                mSelectionNotifier = new SelectionNotifier(this);
            }
            Boolean resTmp;
            Post(mSelectionNotifier, &resTmp);
        }
        else {
            FireOnSelected();
            PerformAccessibilityActionsOnSelected();
        }
    }
}

void AdapterView::FireOnSelected()
{
    if (mOnItemSelectedListener == NULL) {
        return;
    }

    Int32 selection = 0;
    GetSelectedItemPosition(&selection);
    if (selection >= 0) {
        AutoPtr<IView> v;
        GetSelectedView((IView**)&v);
        AutoPtr<IAdapter> adapter;
        GetAdapter((IAdapter**)&adapter);
        Int64 itemId;
        adapter->GetItemId(selection, &itemId);

        mOnItemSelectedListener->OnItemSelected(
            this, v, selection, itemId);
    }
    else {
        mOnItemSelectedListener->OnNothingSelected(this);
    }
}

void AdapterView::PerformAccessibilityActionsOnSelected()
{
    AutoPtr<IAccessibilityManager> manager;
    Boolean enable = TRUE;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&manager);
    manager->IsEnabled(&enable);
    if (!enable) {
        return;
    }

    Int32 position = 0;
    GetSelectedItemPosition(&position);
    if (position >= 0) {
        // we fire selection events here not in View
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
    }
}

ECode AdapterView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> selectedView;
    GetSelectedView((IView**)&selectedView);
    Int32 visible;
    Boolean hasDispatch;
    selectedView->GetVisibility(&visible);
    selectedView->DispatchPopulateAccessibilityEvent(event, &hasDispatch);
    if (selectedView != NULL && visible == IView::VISIBLE
        && hasDispatch) {
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

//@Override
ECode AdapterView::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(child);
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    Boolean sendAccess = FALSE;
    ViewGroup::OnRequestSendAccessibilityEvent(child, event, &sendAccess);
    if (sendAccess) {
        // Add a record for ourselves as well.
        AutoPtr<IAccessibilityEvent> record;
        CAccessibilityEvent::Obtain((IAccessibilityEvent**)&record);
        OnInitializeAccessibilityEvent(record);
        // Populate with the text of the requesting child.
        Boolean temp;
        child->DispatchPopulateAccessibilityEvent(record, &temp);
        IAccessibilityRecord* recordTmp = IAccessibilityRecord::Probe(record);
        event->AppendRecord(recordTmp);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

//@Override
ECode AdapterView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    View::OnInitializeAccessibilityNodeInfo(info);
    String classNameStr("AdapterView");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(classNameStr, (ICharSequence**)&className));
    info->SetClassName(className);
    info->SetScrollable(IsScrollableForAccessibility());
    AutoPtr<IView> selectedView;
    GetSelectedView((IView**)&selectedView);
    if (selectedView != NULL) {
        Boolean enabled;
        selectedView->IsEnabled(&enabled);
        info->SetEnabled(enabled);
    }
    return NOERROR;
}

//@Override
ECode AdapterView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    IAccessibilityRecord* eventTmp = IAccessibilityRecord::Probe(event);
    View::OnInitializeAccessibilityEvent(event);
    String classNameStr("AdapterView");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(classNameStr, (ICharSequence**)&className));
    eventTmp->SetClassName(className);
    eventTmp->SetScrollable(IsScrollableForAccessibility());
    AutoPtr<IView> selectedView;
    GetSelectedView((IView**)&selectedView);
    if (selectedView != NULL) {
        Boolean enabled;
        selectedView->IsEnabled(&enabled);
        eventTmp->SetEnabled(enabled);
    }

    Int32 tmp = 0;
    GetSelectedItemPosition(&tmp);
    eventTmp->SetCurrentItemIndex(tmp);

    GetFirstVisiblePosition(&tmp);
    eventTmp->SetFromIndex(tmp);

    GetLastVisiblePosition(&tmp);
    eventTmp->SetToIndex(tmp);

    GetCount(&tmp);
    eventTmp->SetItemCount(tmp);
    return NOERROR;
}

void AdapterView::HandleDataChanged()
{
    Int32 count = mItemCount;
    Boolean found = FALSE;

    if (count > 0) {
        Int32 newPos;

        // Find the row we are supposed to sync to
        if (mNeedSync) {
            // Update this first, since setNextSelectedPositionInt inspects
            // it
            mNeedSync = FALSE;

            // See if we can find a position in the new data with the same
            // id as the old selection
            //
            newPos = FindSyncPosition();
            if (newPos >= 0) {
                // Verify that new selection is selectable
                Int32 selectablePos = LookForSelectablePosition(newPos, TRUE);
                if (selectablePos == newPos) {
                    // Same row id is selected
                    SetNextSelectedPositionInt(newPos);
                    found = TRUE;
                }
            }
        }

        if (!found) {
            // Try to use the same position if we can't find matching data
            GetSelectedItemPosition(&newPos);

            // Pin position to the available range
            if (newPos >= count) {
                newPos = count - 1;
            }

            if (newPos < 0) {
                newPos = 0;
            }

            // Make sure we select something selectable -- first look down
            Int32 selectablePos = LookForSelectablePosition(newPos, TRUE);
            if (selectablePos < 0) {
                // Looking down didn't work -- try looking up
                selectablePos = LookForSelectablePosition(newPos, FALSE);
            }

            if (selectablePos >= 0) {
                SetNextSelectedPositionInt(selectablePos);
                CheckSelectionChanged();
                found = TRUE;
            }
        }
    }

    if (!found) {
        // Nothing is selected
        mSelectedPosition = IAdapterView::INVALID_POSITION;
        mSelectedRowId = IAdapterView::INVALID_ROW_ID;
        mNextSelectedPosition = IAdapterView::INVALID_POSITION;
        mNextSelectedRowId = IAdapterView::INVALID_ROW_ID;
        mNeedSync = FALSE;
        CheckSelectionChanged();
    }

    NotifySubtreeAccessibilityStateChangedIfNeeded();
}

Boolean AdapterView::IsScrollableForAccessibility()
{
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    if (adapter != NULL) {
        Int32 itemCount = 0;
        adapter->GetCount(&itemCount);
        Int32 firstPosition = 0;
        GetFirstVisiblePosition(&firstPosition);
        Int32 lastPosition = 0;
        GetLastVisiblePosition(&lastPosition);
        return itemCount > 0 && (firstPosition > 0 || lastPosition < itemCount - 1);
    }
    return FALSE;
}

Boolean AdapterView::CanAnimate()
{
    return ViewGroup::CanAnimate() && mItemCount > 0;
}

void AdapterView::CheckSelectionChanged()
{
    if ((mSelectedPosition != mOldSelectedPosition)
        || (mSelectedRowId != mOldSelectedRowId)) {
        SelectionChanged();
        mOldSelectedPosition = mSelectedPosition;
        mOldSelectedRowId = mSelectedRowId;
    }
}

Int32 AdapterView::FindSyncPosition()
{
    Int32 count = mItemCount;

    if (count == 0) {
        return IAdapterView::INVALID_POSITION;
    }

    Int64 idToMatch = mSyncRowId;
    Int32 seed = mSyncPosition;

    // If there isn't a selection don't hunt for it
    if (idToMatch == IAdapterView::INVALID_ROW_ID) {
        return IAdapterView::INVALID_POSITION;
    }

    // Pin seed to reasonable values
    seed = Elastos::Core::Math::Max(0, seed);
    seed = Elastos::Core::Math::Min(count - 1, seed);

    Int64 endTime = SystemClock::GetUptimeMillis() + SYNC_MAX_DURATION_MILLIS;
    Int64 rowId;

    // first position scanned so far
    Int32 first = seed;

    // last position scanned so far
    Int32 last = seed;

    // True if we should move down on the next iteration
    Boolean next = FALSE;

    // True when we have looked at the first item in the data
    Boolean hitFirst;

    // True when we have looked at the last item in the data
    Boolean hitLast;

    // Get the item ID locally (instead of getItemIdAtPosition), so
    // we need the adapter
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    if (adapter == NULL) {
        return IAdapterView::INVALID_POSITION;
    }

    while (SystemClock::GetUptimeMillis() <= endTime) {
        adapter->GetItemId(seed, &rowId);
        if (rowId == idToMatch) {
            // Found it!
            return seed;
        }

        hitLast = last == count - 1;
        hitFirst = first == 0;

        if (hitLast && hitFirst) {
            // Looked at everything
            break;
        }

        if (hitFirst || (next && !hitLast)) {
            // Either we hit the top, or we are trying to move down
            ++last;
            seed = last;
            // Try going up next time
            next = FALSE;
        }
        else if (hitLast || (!next && !hitFirst)) {
            // Either we hit the bottom, or we are trying to move up
            --first;
            seed = first;
            // Try going down next time
            next = TRUE;
        }
    }

    return IAdapterView::INVALID_POSITION;
}

Int32 AdapterView::LookForSelectablePosition(
    /* [in] */ Int32 position,
    /* [in] */ Boolean lookDown)
{
    return position;
}

void AdapterView::SetSelectedPositionInt(
    /* [in] */ Int32 position)
{
    mSelectedPosition = position;
    GetItemIdAtPosition(position, &mSelectedRowId);
}

void AdapterView::SetNextSelectedPositionInt(
    /* [in] */ Int32 position)
{
    mNextSelectedPosition = position;
    GetItemIdAtPosition(position, &mNextSelectedRowId);
    // If we are trying to sync to the selection, update that too
    if (mNeedSync && mSyncMode == SYNC_SELECTED_POSITION && position >= 0) {
        mSyncPosition = position;
        mSyncRowId = mNextSelectedRowId;
    }
}

void AdapterView::RememberSyncState()
{
    Int32 clildCount = 0;
    GetChildCount(&clildCount);
    if (clildCount > 0) {
        mNeedSync = TRUE;
        mSyncHeight = mLayoutHeight;
        if (mSelectedPosition >= 0) {
            // Sync the selection state
            AutoPtr<IView> v;
            GetChildAt(mSelectedPosition - mFirstPosition, (IView**)&v);
            mSyncRowId = mNextSelectedRowId;
            mSyncPosition = mNextSelectedPosition;
            if (v != NULL) {
                v->GetTop(&mSpecificTop);
            }
            mSyncMode = SYNC_SELECTED_POSITION;
        }
        else {
            // Sync the based on the offset of the first view
            AutoPtr<IView> v;
            GetChildAt(0, (IView**)&v);
            AutoPtr<IAdapter> adapter;
            GetAdapter((IAdapter**)&adapter);
            Int32 count;
            adapter->GetCount(&count);

            if (mFirstPosition >= 0 && mFirstPosition < count) {
                adapter->GetItemId(mFirstPosition, &mSyncRowId);
            }
            else {
                mSyncRowId = IView::NO_ID;
            }

            mSyncPosition = mFirstPosition;
            if (v != NULL) {
                v->GetTop(&mSpecificTop);
            }
            mSyncMode = SYNC_FIRST_POSITION;
        }
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
