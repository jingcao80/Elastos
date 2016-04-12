
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/AbsListView.h"
#include "elastos/droid/widget/CAbsListViewLayoutParams.h"
#include "elastos/droid/widget/CAbsListViewSavedState.h"
#include "elastos/droid/widget/CEdgeEffect.h"
#include "elastos/droid/widget/CPopupWindow.h"
#include "elastos/droid/widget/CRemoteViewsAdapter.h"
#include "elastos/droid/widget/FastScroller.h"
#include "elastos/droid/widget/COverScroller.h"
#include "elastos/droid/content/CIntentFilterComparison.h"
// #include "elastos/droid/os/CStrictMode.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/CSparseBooleanArray.h"
#include "elastos/droid/utility/CInt64SparseArray.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/utility/StateSet.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include "elastos/droid/view/inputmethod/CBaseInputConnection.h"
#include "elastos/droid/view/KeyEvent.h"
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::ITransitionDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_ITransitionDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::IStrictMode;
// using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::EIID_IEditable;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::StateSet;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CInt64SparseArray;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoCollectionInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::InputMethod::EIID_IInputConnection;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CBaseInputConnection;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::KeyEvent;
using Elastos::Droid::View::MotionEvent;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IOnTouchModeChangeListener;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IViewGroupLayoutParams;
using Elastos::Droid::View::IAccessibilityDelegate;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::Widget::EIID_IAbsListView;
using Elastos::Droid::Widget::EIID_IFilterListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IFilterListener;
using Elastos::Droid::Widget::IRecyclerListener;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Thread;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 AbsListView::TOUCH_MODE_REST;
const Int32 AbsListView::TOUCH_MODE_DOWN;
const Int32 AbsListView::TOUCH_MODE_TAP;
const Int32 AbsListView::TOUCH_MODE_DONE_WAITING;
const Int32 AbsListView::TOUCH_MODE_SCROLL;
const Int32 AbsListView::TOUCH_MODE_FLING;
const Int32 AbsListView::TOUCH_MODE_OVERSCROLL;
const Int32 AbsListView::TOUCH_MODE_OVERFLING;
const Int32 AbsListView::LAYOUT_NORMAL;
const Int32 AbsListView::LAYOUT_FORCE_TOP;
const Int32 AbsListView::LAYOUT_SET_SELECTION;
const Int32 AbsListView::LAYOUT_FORCE_BOTTOM;
const Int32 AbsListView::LAYOUT_SPECIFIC;
const Int32 AbsListView::LAYOUT_SYNC;
const Int32 AbsListView::LAYOUT_MOVE_SELECTION;
const Int32 AbsListView::OVERSCROLL_LIMIT_DIVISOR;
const Int32 AbsListView::CHECK_POSITION_SEARCH_DISTANCE;
const Int32 AbsListView::TOUCH_MODE_UNKNOWN = -1;
const Int32 AbsListView::TOUCH_MODE_ON = 0;
const Int32 AbsListView::TOUCH_MODE_OFF = 1;
const Boolean AbsListView::PROFILE_SCROLLING = FALSE;
const Boolean AbsListView::PROFILE_FLINGING = FALSE;
const Int32 AbsListView::INVALID_POINTER = 1;

const Int32 AbsListView::PositionScroller::SCROLL_DURATION = 200;
const Int32 AbsListView::PositionScroller::MOVE_DOWN_POS = 1;
const Int32 AbsListView::PositionScroller::MOVE_UP_POS = 2;
const Int32 AbsListView::PositionScroller::MOVE_DOWN_BOUND = 3;
const Int32 AbsListView::PositionScroller::MOVE_UP_BOUND = 4;
const Int32 AbsListView::PositionScroller::MOVE_OFFSET = 5;

static AutoPtr<IInterpolator> InitInterpolator()
{
    AutoPtr<CLinearInterpolator> ator;
    CLinearInterpolator::NewByFriend((CLinearInterpolator**)&ator);
    return (IInterpolator*)ator.Get();
}

AutoPtr<IInterpolator> AbsListView::sLinearInterpolator = InitInterpolator();

//==============================================================================
//          AbsListView::LayoutParams
//==============================================================================

CAR_INTERFACE_IMPL(AbsListView::LayoutParams, ViewGroup::LayoutParams, IAbsListViewLayoutParams);

AbsListView::LayoutParams::LayoutParams()
    : mViewType(0)
    , mRecycledHeaderFooter(FALSE)
    , mForceAdd(FALSE)
    , mScrappedFromPosition(0)
    , mItemId(-1)
{}

AbsListView::LayoutParams::~LayoutParams()
{}

ECode AbsListView::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewGroup::LayoutParams::constructor(c, attrs);
}

ECode AbsListView::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return ViewGroup::LayoutParams::constructor(width, height);
}

ECode AbsListView::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 viewType)
{
    ViewGroup::LayoutParams::constructor(width, height);
    mViewType = viewType;

    return NOERROR;
}

ECode AbsListView::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::LayoutParams::constructor(source);
}

ECode AbsListView::LayoutParams::SetItemId(
    /* [in] */ Int64 itemId)
{
    mItemId = itemId;
    return NOERROR;
}

ECode AbsListView::LayoutParams::SetScrappedFromPosition(
    /* [in] */ Int32 scrappedFromPosition)
{
    mScrappedFromPosition = scrappedFromPosition;
    return NOERROR;
}

ECode AbsListView::LayoutParams::SetForceAdd(
    /* [in] */ Boolean force)
{
    mForceAdd = force;
    return NOERROR;
}

ECode AbsListView::LayoutParams::SetRecycledHeaderFooter(
    /* [in] */ Boolean footer)
{
    mRecycledHeaderFooter = footer;
    return NOERROR;
}

ECode AbsListView::LayoutParams::SetViewType(
    /* [in] */ Int32 type)
{
    mViewType = type;
    return NOERROR;
}

ECode AbsListView::LayoutParams::GetItemId(
    /* [out] */ Int64* itemId)
{
    *itemId = mItemId;
    return NOERROR;
}

ECode AbsListView::LayoutParams::GetScrappedFromPosition(
    /* [out] */ Int32* scrappedFromPosition)
{
    *scrappedFromPosition = mScrappedFromPosition;
    return NOERROR;
}

ECode AbsListView::LayoutParams::GetForceAdd(
    /* [out] */ Boolean* force)
{
    *force = mForceAdd;
    return NOERROR;
}

ECode AbsListView::LayoutParams::GetRecycledHeaderFooter(
    /* [out] */ Boolean* footer)
{
    *footer = mRecycledHeaderFooter;
    return NOERROR;
}

ECode AbsListView::LayoutParams::GetViewType(
    /* [in] */ Int32* type)
{
    *type = mViewType;
    return NOERROR;
}

//==============================================================================
//          AbsListView::SavedState
//==============================================================================

CAR_INTERFACE_IMPL(AbsListView::SavedState, View::BaseSavedState, IAbsListViewSavedState);

AbsListView::SavedState::SavedState()
    : mSelectedId(-1)
    , mFirstId(-1)
    , mViewTop(0)
    , mPosition(0)
    , mHeight(0)
    , mInActionMode(FALSE)
    , mCheckedItemCount(0)
{
}

AbsListView::SavedState::~SavedState()
{
    mCheckState = NULL;
    mCheckIdState = NULL;
}

ECode AbsListView::SavedState::constructor()
{
    return View::BaseSavedState::constructor();
}

ECode AbsListView::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return View::BaseSavedState::constructor(superState);
}

ECode AbsListView::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(View::BaseSavedState::WriteToParcel(dest));

    dest->WriteInt64(mSelectedId);
    dest->WriteInt64(mFirstId);
    dest->WriteInt32(mViewTop);
    dest->WriteInt32(mPosition);
    dest->WriteInt32(mHeight);
    dest->WriteString(mFilter);

    dest->WriteBoolean(mInActionMode);
    dest->WriteInt32(mCheckedItemCount);

    Int32 size;
    Int32 M = mCheckState != NULL ? (mCheckState->GetSize(&size), size) : 0;
    dest->WriteInt32(M);
    for (Int32 i = 0; i < M; i++) {
        Int32 key;
        mCheckState->KeyAt(i, &key);
        dest->WriteInt32(key);

        Boolean value;
        mCheckState->ValueAt(i, &value);
        dest->WriteBoolean(value);
    }

    Int32 N = mCheckIdState != NULL ? (mCheckIdState->GetSize(&size), size) : 0;
    dest->WriteInt32(N);
    for (Int32 i = 0; i < N; i++) {
        Int64 key;
        mCheckIdState->KeyAt(i, &key);
        dest->WriteInt64(key);

        AutoPtr<IInterface> value;
        mCheckIdState->ValueAt(i, (IInterface**)&value);
        dest->WriteInterfacePtr(IInteger32::Probe(value));
    }

    return NOERROR;
}

ECode AbsListView::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(View::BaseSavedState::ReadFromParcel(source));
    source->ReadInt64(&mSelectedId);
    source->ReadInt64(&mFirstId);
    source->ReadInt32(&mViewTop);
    source->ReadInt32(&mPosition);
    source->ReadInt32(&mHeight);
    source->ReadString(&mFilter);
    source->ReadBoolean(&mInActionMode);
    source->ReadInt32(&mCheckedItemCount);

    Int32 M;
    source->ReadInt32(&M);
    if (M > 0) {
        if (!mCheckState) {
            CSparseBooleanArray::New((ISparseBooleanArray**)&mCheckState);
        }
        for (Int32 i = 0; i < M; i++) {
            Int32 key;
            source->ReadInt32(&key);
            Boolean value;
            source->ReadBoolean(&value);
            mCheckState->Put(key, value);
        }
    }

    Int32 N;
    source->ReadInt32(&N);
    if (N > 0) {
        if (!mCheckIdState) {
            CInt64SparseArray::New((IInt64SparseArray**)&mCheckIdState);
        }
        for (Int32 i = 0; i < N; i++) {
            Int64 key;
            source->ReadInt64(&key);
            AutoPtr<IInterface> value;
            source->ReadInterfacePtr((Handle32*)(IInterface**)&value);
            mCheckIdState->Put(key, value);
        }
    }
    return NOERROR;
}

ECode AbsListView::SavedState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder buider;
    buider += "AbsListView.SavedState{";

    buider += StringUtils::ToHexString((Int32)this);
    buider += " selectedId=";
    buider += mSelectedId;
    buider += " firstId=";
    buider += mFirstId;
    buider += " viewTop=";
    buider += mViewTop;
    buider += " position=";
    buider += mPosition;
    buider += " height=";
    buider += mHeight;
    buider += " filter=";
    buider += mFilter;
    buider += " checkState=";
    buider += mCheckState;
    buider += "}";

    *str = buider.ToString();
    return NOERROR;
}

//==============================================================================
//          AbsListView::ListItemAccessibilityDelegate
//==============================================================================

AbsListView::ListItemAccessibilityDelegate::ListItemAccessibilityDelegate(
    /* [in] */ AbsListView* host)
    : mHost(host)
{
}

ECode AbsListView::ListItemAccessibilityDelegate::CreateAccessibilityNodeInfo(
    /* [in] */ IView* host,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    // If the data changed the children are invalid since the data model changed.
    // Hence, we pretend they do not exist. After a layout the children will sync
    // with the model at which point we notify that the accessibility state changed,
    // so a service will be able to re-fetch the views.
    if (mHost->mDataChanged) {
        return NOERROR;
    }
    return View::AccessibilityDelegate::CreateAccessibilityNodeInfo(host, info);
}

ECode AbsListView::ListItemAccessibilityDelegate::OnInitializeAccessibilityNodeInfo(
     /* [in] */ IView* host,
     /* [in] */ IAccessibilityNodeInfo* info)
{
    AccessibilityDelegate::OnInitializeAccessibilityNodeInfo(host, info);

    Int32 position;
    mHost->GetPositionForView(host, &position);

    mHost->OnInitializeAccessibilityNodeInfoForItem(host, position, info);

    return NOERROR;
}

ECode AbsListView::ListItemAccessibilityDelegate::PerformAccessibilityAction(
    /* [in] */ IView* host,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean bval;
    View::AccessibilityDelegate::PerformAccessibilityAction(host, action, arguments, &bval);
    if (bval) {
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;

    Int32 position;
    mHost->GetPositionForView(host, &position);
    AutoPtr<IAdapter> adapterTemp;
    ((IAdapterView*)(mHost->Probe(EIID_IAdapterView)))->GetAdapter((IAdapter**)&adapterTemp);
    AutoPtr<IListAdapter> adapter = (IListAdapter*)adapterTemp->Probe(EIID_IListAdapter);

    if ((position == IAdapterView::INVALID_POSITION) || (adapter == NULL)) {
        // Cannot perform actions on invalid items.
        return NOERROR;
    }

    Boolean adapterEnabled;
    adapter->IsEnabled(position, &adapterEnabled);
    Boolean res;
    if ((mHost->IsEnabled(&res), !res) || !adapterEnabled) {
        // Cannot perform actions on disabled items.
        return NOERROR;
    }

    Int64 id;
    mHost->GetItemIdAtPosition(position, &id);

    Int32 pos;

    switch (action) {
        case IAccessibilityNodeInfo::ACTION_CLEAR_SELECTION: {
            if ((mHost->GetSelectedItemPosition(&pos), pos) == position) {
                mHost->SetSelection(IAdapterView::INVALID_POSITION);
                *result = TRUE;
            }
            return NOERROR;
        }
        case IAccessibilityNodeInfo::ACTION_SELECT: {
            if ((mHost->GetSelectedItemPosition(&pos), pos) != position) {
                mHost->SetSelection(position);
                *result = TRUE;
            }
            return NOERROR;
        }
        case IAccessibilityNodeInfo::ACTION_CLICK: {
            if (mHost->IsClickable(&res), res) {
                return mHost->PerformItemClick(host, position, id, result);
            }
        }
        case IAccessibilityNodeInfo::ACTION_LONG_CLICK: {
            if (mHost->IsLongClickable(&res), res) {
                *result = mHost->PerformLongPress(host, position, id);
            }
            return NOERROR;
        }
    }

    return NOERROR;
}

//==============================================================================
//          AbsListView::AdapterDataSetObserver
//==============================================================================

AbsListView::AdapterDataSetObserver::AdapterDataSetObserver(
    /* [in] */ AbsListView* host)
    : AdapterView::AdapterDataSetObserver(host)
    , mHost(host)
{
}

ECode AbsListView::AdapterDataSetObserver::OnChanged()
{
    FAIL_RETURN(AdapterView::AdapterDataSetObserver::OnChanged());
    if (mHost->mFastScroll != NULL) {
        mHost->mFastScroll->OnSectionsChanged();
    }
    return NOERROR;
}

ECode AbsListView::AdapterDataSetObserver::OnInvalidated()
{
    FAIL_RETURN(AdapterView::AdapterDataSetObserver::OnInvalidated());
    if (mHost->mFastScroll != NULL) {
        mHost->mFastScroll->OnSectionsChanged();
    }
    return NOERROR;
}

//==============================================================================
//          AbsListView::MultiChoiceModeWrapper
//==============================================================================

CAR_INTERFACE_IMPL(AbsListView::MultiChoiceModeWrapper, Object, IMultiChoiceModeListener);

AbsListView::MultiChoiceModeWrapper::MultiChoiceModeWrapper(
    /* [in] */ AbsListView* host)
    : mHost(host)
{}

ECode AbsListView::MultiChoiceModeWrapper::SetWrapped(
    /* [in] */ IMultiChoiceModeListener* wrapped)
{
    mWrapped = wrapped;
    return NOERROR;
}

ECode AbsListView::MultiChoiceModeWrapper::HasWrappedCallback(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mWrapped != NULL;
    return NOERROR;
}

ECode AbsListView::MultiChoiceModeWrapper::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Boolean superResult;
    IActionModeCallback::Probe(mWrapped)->OnCreateActionMode(mode, menu, &superResult);
    if (superResult) {
        // Initialize checked graphic state?
        mHost->SetLongClickable(FALSE);
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode AbsListView::MultiChoiceModeWrapper::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IActionModeCallback::Probe(mWrapped)->OnPrepareActionMode(mode, menu, result);
}

ECode AbsListView::MultiChoiceModeWrapper::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IActionModeCallback::Probe(mWrapped)->OnActionItemClicked(mode, item, result);
}

ECode AbsListView::MultiChoiceModeWrapper::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    IActionModeCallback::Probe(mWrapped)->OnDestroyActionMode(mode);
    mHost->mChoiceActionMode = NULL;

    // Ending selection mode means deselecting everything.
    mHost->ClearChoices();

    mHost->mDataChanged = TRUE;
    mHost->RememberSyncState();
    mHost->RequestLayout();

    mHost->SetLongClickable(TRUE);
    return NOERROR;
}

ECode AbsListView::MultiChoiceModeWrapper::OnItemCheckedStateChanged(
    /* [in] */ IActionMode* mode,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [in] */ Boolean checked)
{
    mWrapped->OnItemCheckedStateChanged(mode, position, id, checked);

    // If there are no items selected we no longer need the selection mode.
    Int32 count;
    mHost->GetCheckedItemCount(&count);
    if (count == 0) {
        mode->Finish();
    }
    return NOERROR;
}

//==============================================================================
//          AbsListView::RecycleBin
//==============================================================================

AbsListView::RecycleBin::RecycleBin(
    /* [in] */ AbsListView* host)
    : mFirstActivePosition(0)
    , mViewTypeCount(0)
    , mHost(host)
{
    mActiveViews = ArrayOf<IView*>::Alloc(0);
}

AbsListView::RecycleBin::~RecycleBin()
{
    mScrapViews = NULL;
    mCurrentScrap = NULL;
    mSkippedScrap = NULL;

    mTransientStateViews = NULL;
}

ECode AbsListView::RecycleBin::SetViewTypeCount(
    /* [in] */ Int32 viewTypeCount)
{
    if (viewTypeCount < 1) {
        Logger::E("AbsListView::RecycleBin", "Can't have a viewTypeCount < 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //noinspection unchecked
    AutoPtr< ArrayOf<IArrayList*> > scrapViews = ArrayOf<IArrayList*>::Alloc(viewTypeCount);
    for (Int32 i = 0; i < viewTypeCount; i++) {
        AutoPtr<IArrayList> listView;
        CArrayList::New((IArrayList**)&listView);
        scrapViews->Set(i, listView);
    }

    mViewTypeCount = viewTypeCount;
    mCurrentScrap = (*scrapViews)[0];
    mScrapViews = scrapViews;

    return NOERROR;
}

void AbsListView::RecycleBin::MarkChildrenDirty()
{
    if (mViewTypeCount == 1) {
        AutoPtr<IArrayList> scrap = mCurrentScrap;
        Int32 scrapCount;
        scrap->GetSize(&scrapCount);
        for (Int32 i = 0; i < scrapCount; i++) {
            AutoPtr<IInterface> obj;
            scrap->Get(i, (IInterface**)&obj);
            IView::Probe(obj)->ForceLayout();
        }
    }
    else {
        Int32 typeCount = mViewTypeCount;
        for (Int32 i = 0; i < typeCount; i++) {
            AutoPtr<IArrayList> scrap = (*mScrapViews)[i];
            Int32 scrapCount;
            scrap->GetSize(&scrapCount);
            for (Int32 j = 0; j < scrapCount; j++) {
                AutoPtr<IInterface> obj;
                scrap->Get(i, (IInterface**)&obj);
                IView::Probe(obj)->ForceLayout();
            }
        }
    }
    if (mTransientStateViews != NULL) {
        Int32 count;
        mTransientStateViews->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            mTransientStateViews->ValueAt(i, (IInterface**)&obj);
            IView::Probe(obj)->ForceLayout();
        }
    }

    if (mTransientStateViewsById != NULL) {
        Int32 count;
        mTransientStateViewsById->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            mTransientStateViewsById->ValueAt(i, (IInterface**)&obj);
            IView::Probe(obj)->ForceLayout();
        }
    }
}

Boolean AbsListView::RecycleBin::ShouldRecycleViewType(
    /* [in] */ Int32 viewType)
{
    return viewType >= 0;
}

void AbsListView::RecycleBin::Clear()
{
    if (mViewTypeCount == 1) {
        AutoPtr<IArrayList> scrap = mCurrentScrap;
        ClearScrap(scrap);
    }
    else {
        Int32 typeCount = mViewTypeCount;;
        for (Int32 i = 0; i < typeCount; i++) {
            AutoPtr<IArrayList> scrap = (*mScrapViews)[i];
            ClearScrap(scrap);
        }
    }
    ClearTransientStateViews();
}

void AbsListView::RecycleBin::FillActiveViews(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 firstActivePosition)
{
    if (mActiveViews->GetLength() < childCount) {
        mActiveViews = ArrayOf<IView*>::Alloc(childCount);
    }

    mFirstActivePosition = firstActivePosition;

    //noinspection MismatchedReadAndWriteOfArray
    AutoPtr< ArrayOf<IView*> > activeViews = mActiveViews;
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        mHost->GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> vglp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        IAbsListViewLayoutParams* lp = IAbsListViewLayoutParams::Probe(vglp);
        // Don't put header or footer views into the scrap heap

        if(lp != NULL) {
            Int32 viewType;
            lp->GetViewType(&viewType);
            if(viewType != IAdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER) {
                // Note:  We do place AdapterView.ITEM_VIEW_TYPE_IGNORE in active views.
                //        However, we will NOT place them into scrap views.
                activeViews->Set(i, child);
            }
        }
    }
}

AutoPtr<IView> AbsListView::RecycleBin::GetActiveView(
    /* [in] */ Int32 position)
{
    Int32 index = position - mFirstActivePosition;
    AutoPtr< ArrayOf<IView*> > activeViews = mActiveViews;
    if (index >= 0 && index < activeViews->GetLength()) {
        AutoPtr<IView> match = (*activeViews)[index];
        activeViews->Set(index, NULL);
        return match;
    }

    return NULL;
}

AutoPtr<IView> AbsListView::RecycleBin::GetTransientStateView(
    /* [in] */ Int32 position)
{
    if (mHost->mAdapter != NULL
            && mHost->mAdapterHasStableIds
            && mTransientStateViewsById) {
        Int64 id;
        IAdapter::Probe(mHost->mAdapter)->GetItemId(position, &id);
        AutoPtr<IInterface> obj;
        mTransientStateViewsById->Get(id, (IInterface**)&obj);
        AutoPtr<IView> result = IView::Probe(obj);
        mTransientStateViewsById->Remove(id);
        return result;
    }
    if (mTransientStateViews != NULL) {
        Int32 index;
        mTransientStateViews->IndexOfKey(position, &index);
        if (index >= 0) {
            AutoPtr<IInterface> obj;
            mTransientStateViews->ValueAt(index, (IInterface**)&obj);
            AutoPtr<IView> result = IView::Probe(obj);
            mTransientStateViews->RemoveAt(index);
            return result;
        }
    }

    return NULL;
}

void AbsListView::RecycleBin::ClearTransientStateViews()
{
    AutoPtr<ISparseArray> viewsByPos = mTransientStateViews;
    if (viewsByPos != NULL) {
        Int32 N;
        viewsByPos->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            viewsByPos->ValueAt(i, (IInterface**)&obj);
            RemoveDetachedView(IView::Probe(obj), FALSE);
        }
        viewsByPos->Clear();
    }

    AutoPtr<IInt64SparseArray> viewsById = mTransientStateViewsById;
    if (viewsById != NULL) {
        Int32 N;
        viewsById->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            viewsById->ValueAt(i, (IInterface**)&obj);
            RemoveDetachedView(IView::Probe(obj), FALSE);
        }
        viewsById->Clear();
    }
}

AutoPtr<IView> AbsListView::RecycleBin::GetScrapView(
    /* [in] */ Int32 position)
{
    if (mViewTypeCount == 1) {
        return RetrieveFromScrap(mCurrentScrap, position);
    }
    else {
        Int32 whichScrap;
        IAdapter::Probe(mHost->mAdapter)->GetItemViewType(position, &whichScrap);
        if (whichScrap >= 0 && whichScrap < mScrapViews->GetLength()) {
            return RetrieveFromScrap((*mScrapViews)[whichScrap], position);
        }
    }
    return NULL;
}

void AbsListView::RecycleBin::AddScrapView(
    /* [in ]*/ IView* scrap,
    /* [in] */ Int32 position)
{
    AutoPtr<IViewGroupLayoutParams> vglp;
    scrap->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
    IAbsListViewLayoutParams* lp = IAbsListViewLayoutParams::Probe(vglp);
    if (lp == NULL) {
        return;
    }

    lp->SetScrappedFromPosition(position);
    // Remove but don't scrap header or footer views, or views that
    // should otherwise not be recycled.

    Int32 viewType = 0;
    lp->GetViewType(&viewType);
    if (!ShouldRecycleViewType(viewType)) {
        return;
    }

    scrap->DispatchStartTemporaryDetach();

    // The the accessibility state of the view may change while temporary
    // detached and we do not allow detached views to fire accessibility
    // events. So we are announcing that the subtree changed giving a chance
    // to clients holding on to a view in this subtree to refresh it.
    mHost->NotifyViewAccessibilityStateChangedIfNeeded(IAccessibilityEvent::CONTENT_CHANGE_TYPE_SUBTREE);

    // Don't scrap views that have transient state.
    Boolean scrapHasTransientState;
    scrap->HasTransientState(&scrapHasTransientState);
    if (scrapHasTransientState) {
        if (mHost->mAdapter != NULL && mHost->mAdapterHasStableIds) {
            // If the adapter has stable IDs, we can reuse the view for
            // the same data.
            if (mTransientStateViewsById == NULL) {
                CInt64SparseArray::New((IInt64SparseArray**)&mTransientStateViewsById);
            }
            Int64 id;
            lp->GetItemId(&id);
            mTransientStateViewsById->Put(id, scrap);
        }
        else if (!mHost->mDataChanged) {
            // If the data hasn't changed, we can reuse the views at
            // their old positions.
            if (mTransientStateViews == NULL) {
                CSparseArray::New((ISparseArray**)&mTransientStateViews);
            }
            mTransientStateViews->Put(position, scrap);
        }
        else {
            // Otherwise, we'll have to remove the view and start over.
            if (mSkippedScrap == NULL) {
                CArrayList::New((IArrayList**)&mSkippedScrap);
            }
            mSkippedScrap->Add(scrap);
        }
    }
    else {
        if (mViewTypeCount == 1) {
            mCurrentScrap->Add(scrap);
        }
        else {
            (*mScrapViews)[viewType]->Add(scrap);
        }

        if (mRecyclerListener != NULL) {
            mRecyclerListener->OnMovedToScrapHeap(scrap);
        }
    }
}

void AbsListView::RecycleBin::RemoveSkippedScrap()
{
    if (mSkippedScrap == NULL) {
        return;
    }

    Int32 count;
    mSkippedScrap->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mSkippedScrap->Get(i, (IInterface**)&obj);
        RemoveDetachedView(IView::Probe(obj), FALSE);
    }
    mSkippedScrap->Clear();
}

void AbsListView::RecycleBin::ScrapActiveViews()
{
    AutoPtr< ArrayOf<IView*> > activeViews = mActiveViews;
    Boolean hasListener = mRecyclerListener != NULL;
    Boolean multipleScraps = mViewTypeCount > 1;

    AutoPtr<IArrayList> scrapViews = mCurrentScrap;
    Int32 count = activeViews->GetLength();
    for (Int32 i = count - 1; i >= 0; i--) {
        AutoPtr<IView> victim = (*activeViews)[i];
        if (victim != NULL) {
            AutoPtr<IViewGroupLayoutParams> params;
            victim->GetLayoutParams((IViewGroupLayoutParams**)&params);
            AutoPtr<IAbsListViewLayoutParams> lp = IAbsListViewLayoutParams::Probe(params);
            Int32 whichScrap = 0;
            lp->GetViewType(&whichScrap);

            activeViews->Set(i, NULL);

            Boolean res;
            if (victim->HasTransientState(&res), res) {
                // Store views with transient state for later use.
                victim->DispatchStartTemporaryDetach();

                if (mHost->mAdapter != NULL && mHost->mAdapterHasStableIds) {
                    if (mTransientStateViewsById == NULL) {
                        CInt64SparseArray::New((IInt64SparseArray**)&mTransientStateViewsById);
                    }
                    Int64 id;
                    IAdapter::Probe(mHost->mAdapter)->GetItemId(mFirstActivePosition + i, &id);
                    mTransientStateViewsById->Put(id, victim);
                }
                else if (!mHost->mDataChanged) {
                    if (mTransientStateViews == NULL) {
                        CSparseArray::New((ISparseArray**)&mTransientStateViews);
                    }
                    mTransientStateViews->Put(mFirstActivePosition + i, victim);
                }
                else if (whichScrap != IAdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER) {
                    // The data has changed, we can't keep this view.
                    RemoveDetachedView(victim, FALSE);
                }
            }
            else if (!ShouldRecycleViewType(whichScrap)) {
                // Discard non-recyclable views except headers/footers.
                if (whichScrap != IAdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER) {
                    RemoveDetachedView(victim, FALSE);
                }
            }
            else {
                // Store everything else on the appropriate scrap heap.
                if (multipleScraps) {
                    scrapViews = (*mScrapViews)[whichScrap];
                }
                victim->DispatchStartTemporaryDetach();
                lp->SetScrappedFromPosition(mFirstActivePosition + i);
                scrapViews->Add(victim);

                if (hasListener) {
                    mRecyclerListener->OnMovedToScrapHeap(victim);
                }
            }
        }
    }
    PruneScrapViews();
}

void AbsListView::RecycleBin::PruneScrapViews()
{
    Int32 maxViews = mActiveViews->GetLength();
    Int32 viewTypeCount = mViewTypeCount;
    AutoPtr< ArrayOf<IArrayList*> > scrapViews = mScrapViews;
    for (Int32 i = 0; i < viewTypeCount; ++i) {
        AutoPtr<IArrayList> scrapPile = (*scrapViews)[i];
        Int32 size;
        scrapPile->GetSize(&size);
        Int32 extras = size - maxViews;
        size--;
        for (Int32 j = 0; j < extras; j++) {
            AutoPtr<IInterface> obj;
            scrapPile->Remove(size--, (IInterface**)&obj);
            RemoveDetachedView(IView::Probe(obj), FALSE);
        }
    }

    AutoPtr<ISparseArray> transViewsByPos = mTransientStateViews;
    if (transViewsByPos != NULL) {
        Int32 size;
        transViewsByPos->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            transViewsByPos->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IView> v = IView::Probe(obj);
            Boolean res;
            if (v->HasTransientState(&res), !res) {
                RemoveDetachedView(v, FALSE);
                transViewsByPos->RemoveAt(i);
                i--;
            }
        }
    }

    AutoPtr<IInt64SparseArray> transViewsById = mTransientStateViewsById;
    if (transViewsById != NULL) {
        Int32 size;
        transViewsById->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            transViewsById->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IView> v = IView::Probe(obj);
            Boolean res;
            if (v->HasTransientState(&res), !res) {
                RemoveDetachedView(v, FALSE);
                transViewsById->RemoveAt(i);
                i--;
            }
        }
    }
}

void AbsListView::RecycleBin::ReclaimScrapViews(
    /* [in] */ IList* views)
{
    if (mViewTypeCount == 1) {
        views->AddAll(ICollection::Probe(mCurrentScrap));
    }
    else {
        Int32 viewTypeCount = mViewTypeCount;
        AutoPtr<ArrayOf<IArrayList*> > scrapViews = mScrapViews;
        for (Int32 i = 0; i < viewTypeCount; ++i) {
            AutoPtr<IArrayList> scrapPile = (*scrapViews)[i];
            views->AddAll(ICollection::Probe(scrapPile));
        }
    }
}

void AbsListView::RecycleBin::SetCacheColorHint(
    /* [in] */ Int32 color)
{
    if (mViewTypeCount == 1) {
        assert(mCurrentScrap != NULL);
        AutoPtr<IArrayList> scrap = mCurrentScrap;
        Int32 scrapCount = scrap->GetSize(&scrapCount);
        for (Int32 i = 0; i < scrapCount; i++) {
            AutoPtr<IInterface> obj;
            scrap->Get(i, (IInterface**)&obj);
            IView::Probe(obj)->SetDrawingCacheBackgroundColor(color);
        }
    }
    else {
        Int32 typeCount = mViewTypeCount;
        for (Int32 i = 0; i < typeCount; i++) {
            AutoPtr<IArrayList> scrap = (*mScrapViews)[i];
            Int32 scrapCount = scrap->GetSize(&scrapCount);
            for (Int32 j = 0; j < scrapCount; j++) {
                AutoPtr<IInterface> obj;
                scrap->Get(j, (IInterface**)&obj);
                IView::Probe(obj)->SetDrawingCacheBackgroundColor(color);
            }
        }
    }

    // Just in case this is called during a layout pass
    assert(mActiveViews != NULL);
    AutoPtr< ArrayOf<IView*> > activeViews = mActiveViews;
    for (Int32 i = 0; i < activeViews->GetLength(); ++i) {
        AutoPtr<IView> victim = (*activeViews)[i];
        if (victim != NULL) {
            victim->SetDrawingCacheBackgroundColor(color);
        }
    }
}

AutoPtr<IView> AbsListView::RecycleBin::RetrieveFromScrap(
    /* [in] */ IArrayList* scrapViews,
    /* [in] */ Int32 position)
{
    Int32 size;
    scrapViews->GetSize(&size);
    if (size > 0) {
        // See if we still have a view for this position or ID.
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            scrapViews->Get(i, (IInterface**)&obj);
            AutoPtr<IView> view = IView::Probe(obj);
            AutoPtr<IViewGroupLayoutParams> lp;
            view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            AutoPtr<IAbsListViewLayoutParams> params = IAbsListViewLayoutParams::Probe(lp);

            Int64 itemId;
            params->GetItemId(&itemId);
            Int32 pos;
            params->GetScrappedFromPosition(&pos);
            if (mHost->mAdapterHasStableIds) {
                Int64 id;
                IAdapter::Probe(mHost->mAdapter)->GetItemId(position, &id);
                if (id == itemId) {
                    AutoPtr<IInterface> obj;
                    scrapViews->Remove(i, (IInterface**)&obj);
                    return IView::Probe(obj);
                }
            }
            else if (pos == position) {
                AutoPtr<IInterface> obj;
                scrapViews->Remove(i, (IInterface**)&obj);
                AutoPtr<IView> scrap = IView::Probe(obj);
                ClearAccessibilityFromScrap(scrap);
                return scrap;
            }
        }

        AutoPtr<IInterface> object;
        scrapViews->Remove(size - 1, (IInterface**)&object);
        AutoPtr<IView> scrap = IView::Probe(object);
        ClearAccessibilityFromScrap(scrap);
        return scrap;
    }
    else {
        return NULL;
    }
}

void AbsListView::RecycleBin::ClearScrap(
    /* [in] */ IArrayList* scrap)
{
    Int32 scrapCount;
    scrap->GetSize(&scrapCount);
    for (Int32 j = 0; j < scrapCount; j++) {
        AutoPtr<IInterface> obj;
        scrap->Remove(scrapCount - 1 - j, (IInterface**)&obj);
        RemoveDetachedView(IView::Probe(obj), FALSE);
    }
}

void AbsListView::RecycleBin::ClearAccessibilityFromScrap(
    /* [in] */ IView* view)
{
    Boolean res;
    if (view->IsAccessibilityFocused(&res), res) {
        view->ClearAccessibilityFocus();
    }
    view->SetAccessibilityDelegate(NULL);
}

void AbsListView::RecycleBin::RemoveDetachedView(
    /* [in] */ IView* child,
    /* [in] */ Boolean animate)
{
    child->SetAccessibilityDelegate(NULL);
    mHost->RemoveDetachedView(child, animate);
}

//==============================================================================
//          AbsListView::AbsPositionScroller
//==============================================================================

AbsListView::AbsPositionScroller::~AbsPositionScroller()
{
    Logger::I("AbsListView", "AbsPositionScroller::~AbsPositionScroller()");
}

//==============================================================================
//          AbsListView::PositionScroller
//==============================================================================

CAR_INTERFACE_IMPL(AbsListView::PositionScroller, AbsPositionScroller, IRunnable);

AbsListView::PositionScroller::PositionScroller(
    /* [in] */ AbsListView* host)
    : mHost(host)
    , mMode(0)
    , mTargetPos(0)
    , mBoundPos(0)
    , mLastSeenPos(0)
    , mScrollDuration(0)
    , mOffsetFromTop(0)
{
    AutoPtr<IViewConfiguration> configuration = ViewConfiguration::Get(mHost->mContext);

    configuration->GetScaledFadingEdgeLength(&mExtraScroll);
}

AbsListView::PositionScroller::~PositionScroller()
{
    Logger::I("AbsListView", "PositionScroller::~PositionScroller()");
}

void AbsListView::PositionScroller::Start(
    /* [in] */ const Int32 position)
{
    Stop();

    if (mHost->mDataChanged) {
        // Wait until we're back in a stable state to try this.
        mHost->mPositionScrollAfterLayout =
                new PositionScrollerStartRunnable(1, position, 0, 0, this);
        return;
    }

    Int32 childCount;
    mHost->GetChildCount(&childCount);
    if (childCount == 0) {
        // Can't scroll without children.
        return;
    }

    Int32 firstPos = mHost->mFirstPosition;
    Int32 lastPos = firstPos + childCount - 1;

    Int32 viewTravelCount;

    Int32 count;
    mHost->GetCount(&count);
    Int32 clampedPosition = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(count - 1, position));

    if (clampedPosition < firstPos) {
        viewTravelCount = firstPos - clampedPosition + 1;
        mMode = MOVE_UP_POS;
    }
    else if (clampedPosition >= lastPos) {
        viewTravelCount = clampedPosition - lastPos + 1;
        mMode = MOVE_DOWN_POS;
    }
    else {
        ScrollToVisible(clampedPosition, IAdapterView::INVALID_POSITION, SCROLL_DURATION);
        return;
    }

    if (viewTravelCount > 0) {
        mScrollDuration = SCROLL_DURATION / viewTravelCount;
    }
    else {
        mScrollDuration = SCROLL_DURATION;
    }
    mTargetPos = clampedPosition;
    mBoundPos = IAdapterView::INVALID_POSITION;
    mLastSeenPos = IAdapterView::INVALID_POSITION;

    mHost->PostOnAnimation(this);
}

void AbsListView::PositionScroller::Start(
    /* [in] */ const Int32 position,
    /* [in] */ const Int32 boundPosition)
{
    Stop();

    if (boundPosition == IAdapterView::INVALID_POSITION) {
        Start(position);
        return;
    }

    if (mHost->mDataChanged) {
        // Wait until we're back in a stable state to try this.
        mHost->mPositionScrollAfterLayout =
                new PositionScrollerStartRunnable(2, position, boundPosition, 0, this);
        return;
    }

    Int32 childCount;
    mHost->GetChildCount(&childCount);
    if (childCount == 0) {
        // Can't scroll without children.
        return;
    }

    Int32 firstPos = mHost->mFirstPosition;
    Int32 lastPos = firstPos + childCount - 1;

    Int32 viewTravelCount = 0;
    Int32 count;
    mHost->GetCount(&count);
    Int32 clampedPosition = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(count - 1, position));

    if (clampedPosition < firstPos) {
        Int32 boundPosFromLast = lastPos - boundPosition;
        if (boundPosFromLast < 1) {
            // Moving would shift our bound position off the screen. Abort.
            return;
        }

        Int32 posTravel = firstPos - clampedPosition + 1;
        Int32 boundTravel = boundPosFromLast - 1;
        if (boundTravel < posTravel) {
            viewTravelCount = boundTravel;
            mMode = MOVE_UP_BOUND;
        }
        else {
            viewTravelCount = posTravel;
            mMode = MOVE_UP_POS;
        }
    }
    else if (clampedPosition > lastPos) {
        Int32 boundPosFromFirst = boundPosition - firstPos;
        if (boundPosFromFirst < 1) {
            // Moving would shift our bound position off the screen. Abort.
            return;
        }

        Int32 posTravel = clampedPosition - lastPos + 1;
        Int32 boundTravel = boundPosFromFirst - 1;
        if (boundTravel < posTravel) {
            viewTravelCount = boundTravel;
            mMode = MOVE_DOWN_BOUND;
        }
        else {
            viewTravelCount = posTravel;
            mMode = MOVE_DOWN_POS;
        }
    }
    else {
        ScrollToVisible(clampedPosition, boundPosition, SCROLL_DURATION);
        return;
    }

    if (viewTravelCount > 0) {
        mScrollDuration = SCROLL_DURATION / viewTravelCount;
    }
    else {
        mScrollDuration = SCROLL_DURATION;
    }
    mTargetPos = clampedPosition;
    mBoundPos = boundPosition;
    mLastSeenPos = IAdapterView::INVALID_POSITION;

    mHost->PostOnAnimation(this);
}

void AbsListView::PositionScroller::StartWithOffset(
    /* [in] */ Int32 position,
    /* [in] */ Int32 offset)
{
    StartWithOffset(position, offset, SCROLL_DURATION);
}

void AbsListView::PositionScroller::StartWithOffset(
    /* [in] */ Int32 position,
    /* [in] */ Int32 offset,
    /* [in] */ const Int32 duration)
{
    Stop();

    if (mHost->mDataChanged) {
        // Wait until we're back in a stable state to try this.
        mHost->mPositionScrollAfterLayout =
                new PositionScrollerStartRunnable(3, position, offset, duration, this);
        return;
    }

    Int32 childCount;
    mHost->GetChildCount(&childCount);
    if (childCount == 0) {
        // Can't scroll without children.
        return;
    }

    Int32 top;
    mHost->GetPaddingTop(&top);
    offset += top;

    Int32 count;
    mHost->GetCount(&count);
    mTargetPos = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(count - 1, position));
    mOffsetFromTop = offset;
    mBoundPos = IAdapterView::INVALID_POSITION;
    mLastSeenPos = IAdapterView::INVALID_POSITION;
    mMode = MOVE_OFFSET;

    Int32 firstPos = mHost->mFirstPosition;
    Int32 lastPos = firstPos + childCount - 1;

    Int32 viewTravelCount;
    if (mTargetPos < firstPos) {
        viewTravelCount = firstPos - mTargetPos;
    }
    else if (mTargetPos > lastPos) {
        viewTravelCount = mTargetPos - lastPos;
    }
    else {
        // On-screen, just scroll.
        AutoPtr<IView> view;
        mHost->GetChildAt(mTargetPos - firstPos, (IView**)&view);
        Int32 targetTop ;
        view->GetTop(&targetTop);
        mHost->SmoothScrollBy(targetTop - offset, duration, TRUE);
        return;
    }

    // Estimate how many screens we should travel
    Float screenTravelCount = (Float) viewTravelCount / childCount;
    mScrollDuration = screenTravelCount < 1 ?
            duration : (Int32) (duration / screenTravelCount);
    mLastSeenPos = IAdapterView::INVALID_POSITION;

    mHost->PostOnAnimation(this);
}

/**
 * Scroll such that targetPos is in the visible padded region without scrolling
 * boundPos out of view. Assumes targetPos is onscreen.
 */
void AbsListView::PositionScroller::ScrollToVisible(
    /* [in] */ Int32 targetPos,
    /* [in] */ Int32 boundPos,
    /* [in] */ Int32 duration)
{
    Int32 firstPos = mHost->mFirstPosition;
    Int32 childCount;
    mHost->GetChildCount(&childCount);
    Int32 lastPos = firstPos + childCount - 1;

    Int32 topTmp = 0, bottomTmp = 0;
    mHost->mListPadding->GetTop(&topTmp);
    mHost->mListPadding->GetBottom(&bottomTmp);
    Int32 paddedTop = topTmp;
    Int32 height;
    mHost->GetHeight(&height);
    Int32 paddedBottom = height - bottomTmp;

    if (targetPos < firstPos || targetPos > lastPos) {
        Logger::W("AbsListView", "scrollToVisible called with targetPos %d  not visible [%d, %d].",
            targetPos, firstPos, lastPos);
    }
    if (boundPos < firstPos || boundPos > lastPos) {
        // boundPos doesn't matter, it's already offscreen.
        boundPos = IAdapterView::INVALID_POSITION;
    }

    AutoPtr<IView> targetChild;
    mHost->GetChildAt(targetPos - firstPos, (IView**)&targetChild);
    Int32 targetTop;
    targetChild->GetTop(&targetTop);
    Int32 targetBottom;
    targetChild->GetBottom(&targetBottom);
    Int32 scrollBy = 0;

    if (targetBottom > paddedBottom) {
        scrollBy = targetBottom - paddedBottom;
    }
    if (targetTop < paddedTop) {
        scrollBy = targetTop - paddedTop;
    }

    if (scrollBy == 0) {
        return;
    }

    if (boundPos >= 0) {
        AutoPtr<IView> boundChild;
        mHost->GetChildAt(boundPos - firstPos, (IView**)&boundChild);
        Int32 boundTop;
        boundChild->GetTop(&boundTop);
        Int32 boundBottom ;
        boundChild->GetBottom(&boundBottom);
        Int32 absScroll = Elastos::Core::Math::Abs(scrollBy);

        if (scrollBy < 0 && boundBottom + absScroll > paddedBottom) {
            // Don't scroll the bound view off the bottom of the screen.
            scrollBy = Elastos::Core::Math::Max(0, boundBottom - paddedBottom);
        }
        else if (scrollBy > 0 && boundTop - absScroll < paddedTop) {
            // Don't scroll the bound view off the top of the screen.
            scrollBy = Elastos::Core::Math::Min(0, boundTop - paddedTop);
        }
    }

    mHost->SmoothScrollBy(scrollBy, duration);
}

void AbsListView::PositionScroller::Stop()
{
    Boolean res;
    mHost->RemoveCallbacks(this, &res);
}

ECode AbsListView::PositionScroller::Run()
{
    Int32 listHeight;
    mHost->GetHeight(&listHeight);
    Int32 firstPos = mHost->mFirstPosition;

    switch (mMode) {
        case MOVE_DOWN_POS: {
                Int32 count;
                mHost->GetChildCount(&count);
                Int32 lastViewIndex = count - 1;
                Int32 lastPos = firstPos + lastViewIndex;

                if (lastViewIndex < 0) {
                    return NOERROR;
                }

                if (lastPos == mLastSeenPos) {
                    // No new views, let things keep going.
                    mHost->PostOnAnimation(this);
                    return NOERROR;
                }

                AutoPtr<IView> lastView;
                mHost->GetChildAt(lastViewIndex, (IView**)&lastView);
                Int32 lastViewHeight;
                lastView->GetHeight(&lastViewHeight);
                Int32 lastViewTop;
                lastView->GetTop(&lastViewTop);
                Int32 lastViewPixelsShowing = listHeight - lastViewTop;

                Int32 bottomTmp = 0;
                mHost->mListPadding->GetBottom(&bottomTmp);
                Int32 extraScroll = lastPos < mHost->mItemCount - 1
                    ? Elastos::Core::Math::Max(bottomTmp, mExtraScroll) : bottomTmp;

                mHost->SmoothScrollBy(
                        lastViewHeight - lastViewPixelsShowing + extraScroll, mScrollDuration);
                Int32 scrollBy = lastViewHeight - lastViewPixelsShowing + extraScroll;
                mHost->SmoothScrollBy(scrollBy, mScrollDuration, true);

                mLastSeenPos = lastPos;
                if (lastPos < mTargetPos) {
                    mHost->PostOnAnimation(this);
                }
            }
            break;
        case MOVE_DOWN_BOUND: {
                Int32 nextViewIndex = 1;
                Int32 childCount;
                mHost->GetChildCount(&childCount);

                if (firstPos == mBoundPos || childCount <= nextViewIndex
                        || firstPos + childCount >= mHost->mItemCount) {
                    return NOERROR;
                }

                Int32 nextPos = firstPos + nextViewIndex;
                if (nextPos == mLastSeenPos) {
                    // No new views, let things keep going.
                    mHost->PostOnAnimation(this);
                    return NOERROR;
                }

                AutoPtr<IView> nextView;
                mHost->GetChildAt(nextViewIndex, (IView**)&nextView);
                Int32 nextViewHeight;
                nextView->GetHeight(&nextViewHeight);
                Int32 nextViewTop;
                nextView->GetTop(&nextViewTop);

                Int32 bottomTmp = 0;
                mHost->mListPadding->GetBottom(&bottomTmp);
                Int32 extraScroll = Elastos::Core::Math::Max(bottomTmp, mExtraScroll);
                if (nextPos < mBoundPos) {
                    mHost->SmoothScrollBy(
                            Elastos::Core::Math::Max(0, nextViewHeight + nextViewTop - extraScroll),
                            mScrollDuration, TRUE);
                    mLastSeenPos = nextPos;
                    mHost->PostOnAnimation(this);
                }
                else {
                    if (nextViewTop > extraScroll) {
                        mHost->SmoothScrollBy(nextViewTop - extraScroll, mScrollDuration, TRUE);
                    }
                }
            }
            break;
        case MOVE_UP_POS: {
                if (firstPos == mLastSeenPos) {
                    // No new views, let things keep going.
                    mHost->PostOnAnimation(this);
                    return NOERROR;
                }

                AutoPtr<IView> firstView;
                mHost->GetChildAt(0, (IView**)&firstView);
                if (firstView == NULL) {
                    return NOERROR;
                }
                Int32 firstViewTop;
                firstView->GetTop(&firstViewTop);

                Int32 topTmp = 0;
                mHost->mListPadding->GetTop(&topTmp);
                Int32 extraScroll = firstPos > 0
                        ? Elastos::Core::Math::Max(mExtraScroll, topTmp) : topTmp;

                mHost->SmoothScrollBy(
                        firstViewTop - extraScroll, mScrollDuration, TRUE);

                mLastSeenPos = firstPos;

                if (firstPos > mTargetPos) {
                    mHost->PostOnAnimation(this);
                }
            }
            break;
        case MOVE_UP_BOUND: {
                Int32 count;
                mHost->GetChildCount(&count);
                Int32 lastViewIndex = count - 2;
                if (lastViewIndex < 0) {
                    return NOERROR;
                }

                Int32 lastPos = firstPos + lastViewIndex;
                if (lastPos == mLastSeenPos) {
                    // No new views, let things keep going.
                    mHost->PostOnAnimation(this);
                    return NOERROR;
                }

                AutoPtr<IView> lastView;
                mHost->GetChildAt(lastViewIndex, (IView**)&lastView);
                Int32 lastViewHeight;
                lastView->GetHeight(&lastViewHeight);
                Int32 lastViewTop;
                lastView->GetTop(&lastViewTop);
                Int32 lastViewPixelsShowing = listHeight - lastViewTop;

                Int32 topTmp = 0;
                mHost->mListPadding->GetTop(&topTmp);
                Int32 extraScroll = Elastos::Core::Math::Max(topTmp, mExtraScroll);
                mLastSeenPos = lastPos;
                if (lastPos > mBoundPos) {
                    mHost->SmoothScrollBy(
                            -(lastViewPixelsShowing - extraScroll), mScrollDuration, TRUE);
                    mHost->PostOnAnimation(this);
                }
                else {
                    Int32 bottom = listHeight - mExtraScroll;
                    Int32 lastViewBottom = lastViewTop + lastViewHeight;
                    if (bottom > lastViewBottom) {
                        mHost->SmoothScrollBy(
                                -(bottom - lastViewBottom), mScrollDuration, TRUE);
                    }
                }
            }
            break;

        case MOVE_OFFSET: {
            if (mLastSeenPos == firstPos) {
                // No new views, let things keep going.
                mHost->PostOnAnimation(this);
                return NOERROR;
            }

            mLastSeenPos = firstPos;

            Int32 childCount;
            mHost->GetChildCount(&childCount);
            Int32 position = mTargetPos;
            Int32 lastPos = firstPos + childCount - 1;

            Int32 viewTravelCount = 0;
            if (position < firstPos) {
                viewTravelCount = firstPos - position + 1;
            }
            else if (position > lastPos) {
                viewTravelCount = position - lastPos;
            }

            // Estimate how many screens we should travel
            Float screenTravelCount = (Float) viewTravelCount / childCount;

            Float modifier = Elastos::Core::Math::Min(Elastos::Core::Math::Abs(screenTravelCount), 1.0f);
            Int32 height;
            mHost->GetHeight(&height);
            if (position < firstPos) {
                Int32 distance = (Int32) (-height * modifier);
                Int32 duration = (Int32) (mScrollDuration * modifier);
                mHost->SmoothScrollBy(distance, duration, TRUE);
                mHost->PostOnAnimation(this);
            }
            else if (position > lastPos) {
                Int32 distance = (Int32) (height * modifier);
                Int32 duration = (Int32) (mScrollDuration * modifier);
                mHost->SmoothScrollBy(distance, duration, TRUE);
                mHost->PostOnAnimation(this);
            }
            else {
                // On-screen, just scroll.
                AutoPtr<IView> view;
                mHost->GetChildAt(position - firstPos, (IView**)&view);
                Int32 targetTop;
                view->GetTop(&targetTop);
                Int32 distance = targetTop - mOffsetFromTop;
                Int32 duration = (Int32) (mScrollDuration *
                        ((Float) Elastos::Core::Math::Abs(distance) / height));
                mHost->SmoothScrollBy(distance, duration, TRUE);
            }
            break;
        }

        default:
            break;
    }

    return NOERROR;
}

//==============================================================================
//          AbsListView::WindowRunnnable
//==============================================================================

AbsListView::WindowRunnnable::WindowRunnnable(
    /* [in] */ AbsListView* host)
    : mHost(host)
    , mOriginalAttachCount(0)
{}

void AbsListView::WindowRunnnable::RememberWindowAttachCount()
{
    mOriginalAttachCount = mHost->GetWindowAttachCount();
}

Boolean AbsListView::WindowRunnnable::SameWindow()
{
    return mHost->GetWindowAttachCount() == mOriginalAttachCount;
}

//==============================================================================
//          AbsListView::PerformClick
//==============================================================================

AbsListView::PerformClick::PerformClick (
    /* [in] */ AbsListView* host)
    : WindowRunnnable(host)
    , mClickMotionPosition(0)
{}

ECode AbsListView::PerformClick::Run()
{
    // The data has changed since we posted this action in the event queue,
    // bail out before bad things happen
    if (mHost->mDataChanged) {
        return NOERROR;
    }

    AutoPtr<IListAdapter> adapter = mHost->mAdapter;
    Int32 motionPosition = mClickMotionPosition;
    if (adapter != NULL && mHost->mItemCount > 0
        && motionPosition != IAdapterView::INVALID_POSITION) {
        Int32 count;
        IAdapter::Probe(adapter)->GetCount(&count);
        if (motionPosition < count && SameWindow()) {
            AutoPtr<IView> view;
            mHost->GetChildAt(motionPosition - mHost->mFirstPosition, (IView**)&view);
            // If there is no view, something bad happened (the view scrolled off the
            // screen, etc.) and we should cancel the click
            if (view != NULL) {
                Int64 id;
                IAdapter::Probe(adapter)->GetItemId(motionPosition, &id);
                Boolean res;
                mHost->PerformItemClick(view, motionPosition, id, &res);
            }
        }
    }
    return NOERROR;
}

//==============================================================================
//          AbsListView::CheckForLongPress
//==============================================================================

AbsListView::CheckForLongPress::CheckForLongPress (
    /* [in] */ AbsListView* host)
    : WindowRunnnable(host)
{}

ECode AbsListView::CheckForLongPress::Run()
{
    Int32 motionPosition = mHost->mMotionPosition;
    AutoPtr<IView> child;
    mHost->GetChildAt(motionPosition - mHost->mFirstPosition, (IView**)&child);
    if (child != NULL) {
        Int32 longPressPosition = mHost->mMotionPosition;
        Int64 longPressId;
        IAdapter::Probe(mHost->mAdapter)->GetItemId(mHost->mMotionPosition, &longPressId);

        Boolean handled = FALSE;
        if (SameWindow() && !mHost->mDataChanged) {
            handled = mHost->PerformLongPress(child, longPressPosition, longPressId);
        }
        if (handled) {
            mHost->mTouchMode = AbsListView::TOUCH_MODE_REST;
            mHost->SetPressed(FALSE);
            child->SetPressed(FALSE);
        }
        else {
            mHost->mTouchMode = AbsListView::TOUCH_MODE_DONE_WAITING;
        }
    }
    return NOERROR;
}

//==============================================================================
//          AbsListView::CheckForKeyLongPress
//==============================================================================

AbsListView::CheckForKeyLongPress::CheckForKeyLongPress (
    /* [in] */ AbsListView* host)
    : WindowRunnnable(host)
{}

ECode AbsListView::CheckForKeyLongPress::Run()
{
    Boolean res;
    if ((mHost->IsPressed(&res), res) && mHost->mSelectedPosition >= 0) {
        Int32 index = mHost->mSelectedPosition - mHost->mFirstPosition;
        AutoPtr<IView> child;
        mHost->GetChildAt(index, (IView**)&child);
        if (!mHost->mDataChanged) {
            Boolean handled = FALSE;
            if (SameWindow()) {
                handled = mHost->PerformLongPress(
                        child, mHost->mSelectedPosition, mHost->mSelectedRowId);
            }
            if (handled) {
                mHost->SetPressed(FALSE);
                child->SetPressed(FALSE);
            }
        }
        else {
            mHost->SetPressed(FALSE);
            if (child != NULL) {
                child->SetPressed(FALSE);
            }
        }
    }

    return NOERROR;
}

//==============================================================================
//          AbsListView::CheckForTap
//==============================================================================

AbsListView::CheckForTap::CheckForTap(
    /* [in] */ AbsListView* host)
    : mX(0.f)
    , mY(0.f)
    , mHost(host)
{}

ECode AbsListView::CheckForTap::Run()
{
    if (mHost->mTouchMode == mHost->TOUCH_MODE_DOWN) {
        mHost->mTouchMode = mHost->TOUCH_MODE_TAP;
        AutoPtr<IView> child;
        mHost->GetChildAt(mHost->mMotionPosition - mHost->mFirstPosition, (IView**)&child);

        Boolean hasFocusable;
        if (child != NULL && (child->HasFocusable(&hasFocusable), !hasFocusable)) {
            mHost->mLayoutMode = mHost->LAYOUT_NORMAL;
            if (!mHost->mDataChanged) {
                child->SetPressed(TRUE);
                mHost->SetPressed(TRUE);
                mHost->LayoutChildren();
                mHost->PositionSelector(mHost->mMotionPosition, child);
                mHost->RefreshDrawableState();

                Int32 longPressTimeout = ViewConfiguration::GetLongPressTimeout();
                Boolean longClickable;
                mHost->IsLongClickable(&longClickable);
                if (mHost->mSelector != NULL) {
                    AutoPtr<IDrawable> d;
                    mHost->mSelector->GetCurrent((IDrawable**)&d);
                    if (d != NULL && ITransitionDrawable::Probe(d) != NULL) {
                        AutoPtr<ITransitionDrawable> td = ITransitionDrawable::Probe(d);
                        if (longClickable) {
                            td->StartTransition(longPressTimeout);
                        }
                        else {
                            td->ResetTransition();
                        }
                    }
                    mHost->mSelector->SetHotspot(mX, mY);
                }

                if (longClickable) {
                    if (mHost->mPendingCheckForLongPress == NULL) {
                        mHost->mPendingCheckForLongPress =
                            new AbsListView::CheckForLongPress(mHost);
                    }

                    mHost->mPendingCheckForLongPress->RememberWindowAttachCount();
                    Boolean res;
                    mHost->PostDelayed(mHost->mPendingCheckForLongPress, longPressTimeout, &res);
                }
                else {
                    mHost->mTouchMode = mHost->TOUCH_MODE_DONE_WAITING;
                }
            }
            else {
                mHost->mTouchMode = mHost->TOUCH_MODE_DONE_WAITING;
            }
        }
    }

    return NOERROR;
}

//==============================================================================
//          AbsListView::FlingRunnableInner
//==============================================================================

AbsListView::FlingRunnableInner::FlingRunnableInner(
    /* [in] */ FlingRunnable* host,
    /* [in] */ AbsListView* mainHost)
    : mHost(host)
    , mMainHost(mainHost)
{
}

ECode AbsListView::FlingRunnableInner::Run()
{
    Int32 activeId = mMainHost->mActivePointerId;

    AutoPtr<VelocityTracker> vt = mMainHost->mVelocityTracker;
    AutoPtr<IOverScroller> scroller = mHost->mScroller;

    if (vt == NULL || activeId == AbsListView::INVALID_POINTER) {
        return NOERROR;
    }

    vt->ComputeCurrentVelocity(1000, mMainHost->mMaximumVelocity);
    Float y;
    vt->GetYVelocity(activeId, &y);
    Float yvel = -y;

    Boolean res;
    if (Elastos::Core::Math::Abs(yvel) >= mMainHost->mMinimumVelocity
            && (scroller->IsScrollingInDirection(0, yvel, &res), res)) {
        // Keep the fling alive a little longer
        mMainHost->PostDelayed(this, FlingRunnable::FLYWHEEL_TIMEOUT, &res);
    }
    else {
        mHost->EndFling(FALSE);
        mMainHost->mTouchMode = AbsListView::TOUCH_MODE_SCROLL;
        mMainHost->ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL);
    }
    return NOERROR;
}

//==============================================================================
//          AbsListView::FlingRunnableInner
//==============================================================================

AbsListView::FlingRunnable::FlingRunnable(
    /* [in] */ AbsListView* host)
    : mHost(host)
    , mLastFlingY(0)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    COverScroller::New(context, (IOverScroller**)&mScroller);
    mCheckFlywheel = new FlingRunnableInner(this, mHost);
}

void AbsListView::FlingRunnable::Start(
    /* [in] */ Int32 initialVelocity)
{
    if (Elastos::Core::Math::Abs(initialVelocity) > mHost->mDecacheThreshold) {
        // For long flings, scrolling cache causes stutter, so don't use it
        mHost->ClearScrollingCache();
    }

    Int32 initialY = initialVelocity < 0 ? Elastos::Core::Math::INT32_MAX_VALUE : 0;
    mLastFlingY = initialY;
    AutoPtr<OverScroller> over = (OverScroller*)mScroller.Get();
    over->SetInterpolator(NULL);
    mScroller->Fling(0, initialY, 0, initialVelocity,
            0, Elastos::Core::Math::INT32_MAX_VALUE, 0, Elastos::Core::Math::INT32_MAX_VALUE);
    mHost->mTouchMode = AbsListView::TOUCH_MODE_FLING;

    mHost->PostOnAnimation(this);

    if (AbsListView::PROFILE_FLINGING) {
        if (!mHost->mFlingProfilingStarted) {
            //Debug.startMethodTracing("AbsListViewFling");
            mHost->mFlingProfilingStarted = TRUE;
        }
    }

    if (mHost->mFlingStrictSpan == NULL) {
        AutoPtr<IStrictMode> helper;
        // CStrictMode::AcquireSingleton((IStrictMode**)&helper);
        helper->EnterCriticalSpan(String("AbsListView-fling"),
                (IStrictModeSpan**)&mHost->mFlingStrictSpan);
    }
}

void AbsListView::FlingRunnable::StartSpringback()
{
    Boolean res;
    if (mScroller->SpringBack(0, mHost->mScrollY, 0, 0, 0, 0, &res), res) {
        mHost->mTouchMode = AbsListView::TOUCH_MODE_OVERFLING;
        mHost->Invalidate();
        mHost->PostOnAnimation(this);
    }
    else {
        mHost->mTouchMode = AbsListView::TOUCH_MODE_REST;
        mHost->ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
    }
}

void AbsListView::FlingRunnable::StartOverfling(
    /* [in] */ Int32 initialVelocity)
{
    AutoPtr<OverScroller> over = (OverScroller*)mScroller.Get();
    over->SetInterpolator(NULL);
    Int32 height;
    mHost->GetHeight(&height);
    mScroller->Fling(0, mHost->mScrollY, 0, initialVelocity, 0, 0,
            Elastos::Core::Math::INT32_MIN_VALUE, Elastos::Core::Math::INT32_MAX_VALUE, 0, height);
    mHost->mTouchMode = AbsListView::TOUCH_MODE_OVERFLING;
    mHost->Invalidate();
    mHost->PostOnAnimation(this);
}

void AbsListView::FlingRunnable::EdgeReached(
    /* [in] */ Int32 delta)
{
    mScroller->NotifyVerticalEdgeReached(
            mHost->mScrollY, 0, mHost->mOverflingDistance);
    Int32 overscrollMode;
    mHost->GetOverScrollMode(&overscrollMode);
    if (overscrollMode == IView::OVER_SCROLL_ALWAYS ||
            (overscrollMode == IView::OVER_SCROLL_IF_CONTENT_SCROLLS
            && !mHost->ContentFits())) {
        mHost->mTouchMode = AbsListView::TOUCH_MODE_OVERFLING;
        Float velocity;
        mScroller->GetCurrVelocity(&velocity);
        Int32 vel = (Int32)velocity;
        if (delta > 0) {
            mHost->mEdgeGlowTop->OnAbsorb(vel);
        }
        else {
            mHost->mEdgeGlowBottom->OnAbsorb(vel);
        }
    }
    else {
        mHost->mTouchMode = AbsListView::TOUCH_MODE_REST;
        if (mHost->mPositionScroller != NULL) {
            mHost->mPositionScroller->Stop();
        }
    }

    mHost->Invalidate();
    mHost->PostOnAnimation(this);
}

void AbsListView::FlingRunnable::StartScroll(
    /* [in] */ Int32 distance,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean linear)
{
    Int32 initialY = distance < 0 ? Elastos::Core::Math::INT32_MAX_VALUE : 0;
    mLastFlingY = initialY;
    AutoPtr<OverScroller> over = (OverScroller*)mScroller.Get();
    over->SetInterpolator(linear ? AbsListView::sLinearInterpolator : NULL);
    mScroller->StartScroll(0, initialY, 0, distance, duration);
    mHost->mTouchMode = AbsListView::TOUCH_MODE_FLING;
    mHost->PostOnAnimation(this);
}

void AbsListView::FlingRunnable::EndFling(
    /* [in] */ Boolean clearCache)
{
    mHost->mTouchMode = AbsListView::TOUCH_MODE_REST;

    Boolean res;
    mHost->RemoveCallbacks(this, &res);
    mHost->RemoveCallbacks(mCheckFlywheel, &res);

    mHost->ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
    if (clearCache)
        mHost->ClearScrollingCache();

    mScroller->AbortAnimation();

    if (mHost->mFlingStrictSpan != NULL) {
        mHost->mFlingStrictSpan->Finish();
        mHost->mFlingStrictSpan = NULL;
    }
}

void AbsListView::FlingRunnable::EndFling()
{
    EndFling(TRUE);
}

void AbsListView::FlingRunnable::FlywheelTouch()
{
    Boolean res;
    mHost->PostDelayed(mCheckFlywheel, FLYWHEEL_TIMEOUT, &res);
}

ECode AbsListView::FlingRunnable::Run()
{
    switch(mHost->mTouchMode) {
        default:
            EndFling();
            return NOERROR;
        case AbsListView::TOUCH_MODE_SCROLL: {
            Boolean res;
            if (mScroller->IsFinished(&res), res) {
                return NOERROR;
            }
        }
        case AbsListView::TOUCH_MODE_FLING: {
            if (mHost->mDataChanged) {
                mHost->LayoutChildren();
            }

            Int32 count;
            mHost->GetChildCount(&count);
            if (mHost->mItemCount == 0 || count == 0) {
                EndFling();
                return NOERROR;
            }

            Boolean more;
            mScroller->ComputeScrollOffset(&more);
            Int32 y;
            mScroller->GetCurrY(&y);
            // Flip sign to convert finger direction to list items direction
            // (e.g. finger moving down means list is moving towards the top)
            Int32 delta = mLastFlingY - y;

            // Pretend that each frame of a fling scroll is a touch scroll
            if (delta > 0) {
                // List is moving towards the top. Use first view as mMotionPosition
                mHost->mMotionPosition = mHost->mFirstPosition;
                AutoPtr<IView> firstView;
                mHost->GetChildAt(0, (IView**)&firstView);
                firstView->GetTop(&(mHost->mMotionViewOriginalTop));

                // Don't fling more than 1 screen
                Int32 h;
                mHost->GetHeight(&h);
                delta = Elastos::Core::Math::Min(h - mHost->mPaddingBottom - mHost->mPaddingTop - 1, delta);
            }
            else {
                // List is moving towards the bottom. Use last view as mMotionPosition
                Int32 offsetToLast = count - 1;
                mHost->mMotionPosition = mHost->mFirstPosition + offsetToLast;

                AutoPtr<IView> lastView;
                mHost->GetChildAt(offsetToLast, (IView**)&lastView);
                lastView->GetTop(&(mHost->mMotionViewOriginalTop));

                // Don't fling more than 1 screen
                Int32 h;
                mHost->GetHeight(&h);
                delta = Elastos::Core::Math::Max(-(h - mHost->mPaddingBottom - mHost->mPaddingTop - 1), delta);
            }

            // Check to see if we have bumped into the scroll limit
            AutoPtr<IView> motionView;
            mHost->GetChildAt(mHost->mMotionPosition - mHost->mFirstPosition, (IView**)&motionView);
            Int32 oldTop = 0;
            if (motionView != NULL) {
                motionView->GetTop(&oldTop);
            }

            // Don't stop just because delta is zero (it could have been rounded)
            Boolean atEdge = mHost->TrackMotionScroll(delta, delta);
            Boolean atEnd = atEdge && (delta != 0);
            if (atEnd) {
                if (motionView != NULL) {
                    // Tweak the scroll for how far we overshot
                    Int32 top;
                    motionView->GetTop(&top);
                    Int32 overshoot = -(delta - (top - oldTop));
                    mHost->OverScrollBy(0, overshoot, 0, mHost->mScrollY, 0, 0,
                            0, mHost->mOverflingDistance, FALSE);
                }
                if (more) {
                    EdgeReached(delta);
                }
                break;
            }

            if (more && !atEnd) {
                if (atEdge) mHost->Invalidate();
                mLastFlingY = y;
                mHost->PostOnAnimation(this);
            }
            else {
                EndFling();

                if (AbsListView::PROFILE_FLINGING) {
                    if (mHost->mFlingProfilingStarted) {
                        // Debug.stopMethodTracing();
                        mHost->mFlingProfilingStarted = FALSE;
                    }

                    if (mHost->mFlingStrictSpan != NULL) {
                        mHost->mFlingStrictSpan->Finish();
                        mHost->mFlingStrictSpan = NULL;
                    }
                }
            }
            break;
        }

        case TOUCH_MODE_OVERFLING: {
            Boolean res;
            if (mScroller->ComputeScrollOffset(&res), res) {
                Int32 scrollY = mHost->mScrollY;
                Int32 currY;
                mScroller->GetCurrY(&currY);
                Int32 deltaY = currY - scrollY;
                if (mHost->OverScrollBy(0, deltaY, 0, scrollY, 0, 0,
                        0, mHost->mOverflingDistance, FALSE)) {
                    Boolean crossDown = scrollY <= 0 && currY > 0;
                    Boolean crossUp = scrollY >= 0 && currY < 0;
                    if (crossDown || crossUp) {
                        Float _velocity;
                        mScroller->GetCurrVelocity(&_velocity);
                        Int32 velocity = (Int32)_velocity;
                        if (crossUp) {
                            velocity = -velocity;
                        }

                        // Don't flywheel from this; we're just continuing things.
                        mScroller->AbortAnimation();
                        Start(velocity);
                    }
                    else {
                        StartSpringback();
                    }
                }
                else {
                    mHost->Invalidate();
                    mHost->PostOnAnimation(this);
                }
            }
            else {
                EndFling();
            }
            break;
        }
    }

    return NOERROR;
}

//==============================================================================
//          AbsListView::InputConnectionWrapper
//==============================================================================

CAR_INTERFACE_IMPL(AbsListView::InputConnectionWrapper, Object, IInputConnection);

AbsListView::InputConnectionWrapper::InputConnectionWrapper(
    /* [in] */ AbsListView* host,
    /* [in] */ IEditorInfo* outAttrs)
    : mHost(host)
    , mOutAttrs(outAttrs)
{}

AbsListView::InputConnectionWrapper::~InputConnectionWrapper()
{}

AutoPtr<IInputConnection> AbsListView::InputConnectionWrapper::GetTarget()
{
    if (mTarget == NULL) {
        AutoPtr<IEditText> text = mHost->GetTextFilterInput();
        IView::Probe(text)->OnCreateInputConnection(mOutAttrs, (IInputConnection**)&mTarget);
    }
    return mTarget;
}

ECode AbsListView::InputConnectionWrapper::ReportFullscreenMode(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Use our own input connection, since it is
    // the "real" one the IME is talking with.
    return mHost->mDefInputConnection->ReportFullscreenMode(enabled, result);
}

ECode AbsListView::InputConnectionWrapper::PerformEditorAction(
    /* [in] */ Int32 editorAction,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // The editor is off in its own window; we need to be
    // the one that does this.
    if (editorAction == IEditorInfo::IME_ACTION_DONE) {
        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
        AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
        if (imm != NULL) {
            AutoPtr<IBinder> token;
            mHost->GetWindowToken((IBinder**)&token);
            Boolean res;
            imm->HideSoftInputFromWindow(token, 0, &res);
        }
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode AbsListView::InputConnectionWrapper::SendKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Use our own input connection, since the filter
    // text view may not be shown in a window so has
    // no ViewAncestor to dispatch events with.
    return mHost->mDefInputConnection->SendKeyEvent(event, result);
}

ECode AbsListView::InputConnectionWrapper::GetTextBeforeCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq);

    if (mTarget == NULL) {
        AutoPtr<ICharSequence> cs = CoreUtils::Convert("");
        *seq = cs;
        REFCOUNT_ADD(*seq);
        return NOERROR;
    }

    return mTarget->GetTextBeforeCursor(length, flags, seq);
}

ECode AbsListView::InputConnectionWrapper::GetTextAfterCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq);

    if (mTarget == NULL) {
        AutoPtr<ICharSequence> cs = CoreUtils::Convert("");
        *seq = cs;
        REFCOUNT_ADD(*seq);
        return NOERROR;
    }

    return mTarget->GetTextAfterCursor(length, flags, seq);
}

ECode AbsListView::InputConnectionWrapper::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq);

    if (mTarget == NULL) {
        AutoPtr<ICharSequence> cs = CoreUtils::Convert("");
        *seq = cs;
        REFCOUNT_ADD(*seq);
        return NOERROR;
    }

    return mTarget->GetSelectedText(flags, seq);
}

ECode AbsListView::InputConnectionWrapper::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [out] */ Int32* afterLength)
{
    VALIDATE_NOT_NULL(afterLength);

    if (mTarget == NULL) {
        *afterLength = IInputType::TYPE_TEXT_FLAG_CAP_SENTENCES;
    }
    return mTarget->GetCursorCapsMode(reqModes, afterLength);
}

ECode AbsListView::InputConnectionWrapper::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [out] */ IExtractedText** text)
{
    VALIDATE_NOT_NULL(text);
    return GetTarget()->GetExtractedText(request, flags, text);
}

ECode AbsListView::InputConnectionWrapper::DeleteSurroundingText(
    /* [in] */ Int32 beforeLength,
    /* [in] */ Int32 afterLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->DeleteSurroundingText(beforeLength, afterLength, result);
}

ECode AbsListView::InputConnectionWrapper::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->SetComposingText(text, newCursorPosition, result);
}

ECode AbsListView::InputConnectionWrapper::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->SetComposingRegion(start, end, result);
}

ECode AbsListView::InputConnectionWrapper::FinishComposingText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean res;
    *result = mTarget == NULL || (mTarget->FinishComposingText(&res), res);
    return NOERROR;
}

ECode AbsListView::InputConnectionWrapper::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->CommitText(text, newCursorPosition, result);
}

ECode AbsListView::InputConnectionWrapper::CommitCompletion(
    /* [in] */ ICompletionInfo* text,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->CommitCompletion(text, result);
}

ECode AbsListView::InputConnectionWrapper::CommitCorrection(
    /* [in] */ ICorrectionInfo* correctionInfo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->CommitCorrection(correctionInfo, result);
}

ECode AbsListView::InputConnectionWrapper::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->SetSelection(start, end, result);
}

ECode AbsListView::InputConnectionWrapper::PerformContextMenuAction(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->PerformContextMenuAction(id, result);
}

ECode AbsListView::InputConnectionWrapper::BeginBatchEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->BeginBatchEdit(result);
}

ECode AbsListView::InputConnectionWrapper::EndBatchEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->EndBatchEdit(result);
}

ECode AbsListView::InputConnectionWrapper::ClearMetaKeyStates(
    /* [in] */ Int32 states,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->ClearMetaKeyStates(states, result);
}

ECode AbsListView::InputConnectionWrapper::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->PerformPrivateCommand(action, data, result);
}

ECode AbsListView::InputConnectionWrapper::RequestCursorUpdates(
    /* [in] */ Int32 cursorUpdateMode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTarget()->RequestCursorUpdates(cursorUpdateMode, result);
}

//==============================================================================
//          AbsListView::PositionScrollerStartRunnable
//==============================================================================
AbsListView::PositionScrollerStartRunnable::PositionScrollerStartRunnable(
    /* [in] */ Int32 paramsCount,
    /* [in] */ Int32 param1,
    /* [in] */ Int32 param2,
    /* [in] */ Int32 param3,
    /* [in] */ PositionScroller* host)
    : mParamsCount(paramsCount)
    , mParam1(param1)
    , mParam2(param1)
    , mParam3(param1)
    , mHost(host)
{
}

ECode AbsListView::PositionScrollerStartRunnable::Run()
{
    switch(mParamsCount) {
        case 1:
            mHost->Start(mParam1);
            break;
        case 2:
            mHost->Start(mParam1, mParam2);
            break;
        case 3:
            mHost->StartWithOffset(mParam1, mParam2, mParam3);
            break;
        default:
            break;
    }
    return NOERROR;
}

//==============================================================================
//          AbsListView::SetFastScrollRunnable
//==============================================================================
AbsListView::SetFastScrollRunnable::SetFastScrollRunnable(
    /* [in] */ AbsListView* host,
    /* [in] */ Boolean res,
    /* [in] */ Int32 id)
    : mHost(host)
    , mRes(res)
    , mId(id)
{}

ECode AbsListView::SetFastScrollRunnable::Run()
{
    switch (mId) {
        case 0:
            mHost->SetFastScrollerEnabledUiThread(mRes);
            break;
        case 1:
            mHost->SetFastScrollerAlwaysVisibleUiThread(mRes);
        default:
            break;
    }

    return NOERROR;
}

//==============================================================================
//          AbsListView::ClearScrollingCacheRunnable
//==============================================================================

AbsListView::ClearScrollingCacheRunnable::ClearScrollingCacheRunnable(
    /* [in] */ AbsListView* host)
    : mHost(host)
{}

ECode AbsListView::ClearScrollingCacheRunnable::Run()
{
    if (mHost->mCachingStarted) {
        mHost->mCachingStarted = mHost->mCachingActive = FALSE;
        mHost->SetChildrenDrawnWithCacheEnabled(FALSE);
        if ((mHost->mPersistentDrawingCache
            & IViewGroup::PERSISTENT_SCROLLING_CACHE) == 0) {
                mHost->SetChildrenDrawingCacheEnabled(FALSE);
        }

        Boolean res;
        if (mHost->IsAlwaysDrawnWithCacheEnabled(&res), !res) {
            mHost->Invalidate();
        }
    }

    return NOERROR;
}

//==============================================================================
//          AbsListView::TouchModeResetRunnable
//==============================================================================

AbsListView::TouchModeResetRunnable::TouchModeResetRunnable(
    /* [in] */ IRunnable* r,
    /* [in] */ AbsListView* host,
    /* [in] */ IView* v)
    : mRunnable(r)
    , mHost(host)
    , mView(v)
{
}

ECode AbsListView::TouchModeResetRunnable::Run()
{
    mHost->mTouchModeReset = NULL;
    mHost->mTouchMode = AbsListView::TOUCH_MODE_REST;
    mView->SetPressed(FALSE);
    mHost->SetPressed(FALSE);
    Boolean res;
    if(!mHost->mDataChanged && !mHost->mIsDetaching && (mHost->IsAttachedToWindow(&res), res)) {
        mRunnable->Run();
    }
    return NOERROR;
}

//==============================================================================
//                  AbsListView
//==============================================================================

CAR_INTERFACE_IMPL_6(AbsListView, AdapterView, IAbsListView, ITextWatcher, IOnGlobalLayoutListener,
        IFilterListener, IOnTouchModeChangeListener, IRemoteAdapterConnectionCallback);

AbsListView::AbsListView()
    : mChoiceMode(IAbsListView::CHOICE_MODE_NONE)
    , mCheckedItemCount(0)
    , mLayoutMode(LAYOUT_NORMAL)
    , mAdapterHasStableIds(FALSE)
    , mDrawSelectorOnTop(FALSE)
    , mSelectorPosition(IAdapterView::INVALID_POSITION)
    , mSelectionLeftPadding(0)
    , mSelectionTopPadding(0)
    , mSelectionRightPadding(0)
    , mSelectionBottomPadding(0)
    , mWidthMeasureSpec(0)
    , mCachingStarted(FALSE)
    , mCachingActive(FALSE)
    , mMotionPosition(0)
    , mMotionViewOriginalTop(0)
    , mMotionViewNewTop(0)
    , mMotionX(0)
    , mMotionY(0)
    , mTouchMode(TOUCH_MODE_REST)
    , mLastY(0)
    , mMotionCorrection(0)
    , mSelectedTop(0)
    , mStackFromBottom(FALSE)
    , mScrollingCacheEnabled(FALSE)
    , mFastScrollEnabled(FALSE)
    , mFastScrollAlwaysVisible(FALSE)
    , mResurrectToPosition(IAdapterView::INVALID_POSITION)
    , mOverscrollMax(FALSE)
    , mOverscrollDistance(0)
    , mOverflingDistance(0)
    , mDeferNotifyDataSetChanged(FALSE)
    , mSmoothScrollbarEnabled(TRUE)
    , mTextFilterEnabled(FALSE)
    , mFiltered(FALSE)
    , mLastTouchMode(TOUCH_MODE_UNKNOWN)
    , mScrollProfilingStarted(FALSE)
    , mFlingProfilingStarted(FALSE)
    , mTranscriptMode(0)
    , mCacheColorHint(0)
    , mIsChildViewEnabled(FALSE)
    , mLastScrollState(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE)
    , mFastScrollStyle(0)
    , mGlobalLayoutListenerAddedFilter(FALSE)
    , mTouchSlop(0)
    , mDensityScale(0)
    , mMinimumVelocity(0)
    , mMaximumVelocity(0)
    , mVelocityScale(1.0f)
    , mNestedYOffset(0)
    , mPopupHidden(FALSE)
    , mActivePointerId(INVALID_POINTER)
    , mFirstPositionDistanceGuess(0)
    , mLastPositionDistanceGuess(0)
    , mDirection(0)
    , mForceTranscriptScroll(0)
    , mGlowPaddingLeft(0)
    , mGlowPaddingRight(0)
    , mLastAccessibilityScrollEventFromIndex(0)
    , mLastAccessibilityScrollEventToIndex(0)
    , mLastHandledItemCount(0)
    , mIsDetaching(FALSE)
{
    CRect::New((IRect**)&mSelectorRect);
    CRect::New((IRect**)&mListPadding);
    mRecycler = new RecycleBin(this);
    mIsScrap = ArrayOf<Boolean>::Alloc(1);
    mScrollOffset = ArrayOf<Int32>::Alloc(2);
    mScrollConsumed = ArrayOf<Int32>::Alloc(2);
}

AbsListView::~AbsListView()
{
    mCheckStates = NULL;
    mCheckedIdStates = NULL;
}

ECode AbsListView::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(AdapterView::constructor(context));
    InitAbsListView();

    mOwnerThread = Thread::GetCurrentThread();

    SetVerticalScrollBarEnabled(TRUE);
    Int32 size = ArraySize(R::styleable::View);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::View, size);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(layout, (ITypedArray**)&a);
    InitializeScrollbarsInternal(a);

    a->Recycle();

    return NOERROR;
}

ECode AbsListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::absListViewStyle);
}

ECode AbsListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode AbsListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(AdapterView::constructor(context, attrs, defStyleAttr, defStyleRes));
    InitAbsListView();

    mOwnerThread = Thread::GetCurrentThread();

    Int32 size = ArraySize(R::styleable::AbsListView);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::AbsListView, size);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
            attrs, layout, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<IDrawable> d;
    a->GetDrawable(R::styleable::AbsListView_listSelector, (IDrawable**)&d);
    if (d != NULL) {
        SetSelector(d);
    }

    a->GetBoolean(R::styleable::AbsListView_drawSelectorOnTop, FALSE, &mDrawSelectorOnTop);

    Boolean stackFromBottom;
    a->GetBoolean(R::styleable::AbsListView_stackFromBottom, FALSE, &stackFromBottom);
    SetStackFromBottom(stackFromBottom);

    Boolean scrollingCacheEnabled;
    a->GetBoolean(R::styleable::AbsListView_scrollingCache, TRUE, &scrollingCacheEnabled);
    SetScrollingCacheEnabled(scrollingCacheEnabled);

    Boolean useTextFilter;
    a->GetBoolean(R::styleable::AbsListView_textFilterEnabled, FALSE, &useTextFilter);
    SetTextFilterEnabled(useTextFilter);

    Int32 transcriptMode;
    a->GetInt32(R::styleable::AbsListView_transcriptMode,
        IAbsListView::AbsListView_TRANSCRIPT_MODE_DISABLED, &transcriptMode);
    SetTranscriptMode(transcriptMode);

    Int32 color;
    a->GetColor(R::styleable::AbsListView_cacheColorHint, 0, &color);
    SetCacheColorHint(color);

    Boolean enableFastScroll;
    a->GetBoolean(R::styleable::AbsListView_fastScrollEnabled, FALSE, &enableFastScroll);
    SetFastScrollEnabled(enableFastScroll);

    Int32 fastScrollStyle;
    a->GetResourceId(R::styleable::AbsListView_fastScrollStyle, 0, &fastScrollStyle);
    SetFastScrollStyle(fastScrollStyle);

    Boolean smoothScrollbar;
    a->GetBoolean(R::styleable::AbsListView_smoothScrollbar, TRUE, &smoothScrollbar);
    SetSmoothScrollbarEnabled(smoothScrollbar);

    Int32 choiceMode;
    a->GetInt32(R::styleable::AbsListView_choiceMode, IAbsListView::CHOICE_MODE_NONE, &choiceMode);
    SetChoiceMode(choiceMode);

    Boolean fastScrollAlwaysVisible;
    a->GetBoolean(R::styleable::AbsListView_fastScrollAlwaysVisible, FALSE, &fastScrollAlwaysVisible);
    SetFastScrollAlwaysVisible(fastScrollAlwaysVisible);

    a->Recycle();

    return NOERROR;
}

ECode AbsListView::InitAbsListView()
{
    // Setting focusable in touch mode will set the focusable property to TRUE
    SetClickable(TRUE);
    SetFocusableInTouchMode(TRUE);
    SetWillNotDraw(FALSE);
    SetAlwaysDrawnWithCacheEnabled(FALSE);
    SetScrollingCacheEnabled(TRUE);

    AutoPtr<IViewConfiguration> configuration = ViewConfiguration::Get(mContext);

    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledMinimumFlingVelocity(&mMinimumVelocity);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    mDecacheThreshold = mMaximumVelocity / 2;
    configuration->GetScaledOverscrollDistance(&mOverscrollDistance);
    configuration->GetScaledOverflingDistance(&mOverflingDistance);

    AutoPtr<IResources> resources;
    FAIL_RETURN(mContext->GetResources((IResources**)&resources));

    AutoPtr<IDisplayMetrics> dm;
    FAIL_RETURN(resources->GetDisplayMetrics((IDisplayMetrics**)&dm));
    dm->GetDensity(&mDensityScale);

    return NOERROR;
}

ECode AbsListView::SetOverScrollMode(
    /* [in] */ Int32 mode)
{
    if (mode != IView::OVER_SCROLL_NEVER) {
        if (mEdgeGlowTop == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowTop);
            CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowBottom);
        }
    }
    else {
        mEdgeGlowTop = NULL;
        mEdgeGlowBottom = NULL;
    }

    return AdapterView::SetOverScrollMode(mode);
}

ECode AbsListView::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    if (adapter != NULL) {
        IAdapter::Probe(mAdapter)->HasStableIds(&mAdapterHasStableIds);
        if ((mChoiceMode != IAbsListView::CHOICE_MODE_NONE) && mAdapterHasStableIds &&
                mCheckedIdStates == NULL) {
            CInt64SparseArray::New((IInt64SparseArray**)&mCheckedIdStates);
        }
    }

    if (mCheckStates != NULL) {
        mCheckStates->Clear();
    }

    if (mCheckedIdStates != NULL) {
        mCheckedIdStates->Clear();
    }
    return NOERROR;
}

ECode AbsListView::GetCheckedItemCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mCheckedItemCount;
    return NOERROR;
}

ECode AbsListView::IsItemChecked(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mChoiceMode != IAbsListView::CHOICE_MODE_NONE && mCheckStates != NULL) {
        return mCheckStates->Get(position, result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode AbsListView::GetCheckedItemPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 size;
    if (mChoiceMode == IAbsListView::CHOICE_MODE_SINGLE
        && mCheckStates != NULL && (mCheckStates->GetSize(&size), size) == 1) {
        return mCheckStates->KeyAt(0, result);
    }

    *result = IAdapterView::INVALID_POSITION;
    return NOERROR;
}

ECode AbsListView::GetCheckedItemPositions(
    /* [out] */ ISparseBooleanArray** arr)
{
    VALIDATE_NOT_NULL(arr);
    *arr = NULL;

    if (mChoiceMode != IAbsListView::CHOICE_MODE_NONE) {
        *arr = mCheckStates;
        REFCOUNT_ADD(*arr);
        return NOERROR;
    }

    return NOERROR;
}

ECode AbsListView::GetCheckedItemIds(
    /* [out] */ ArrayOf<Int64>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mChoiceMode == IAbsListView::CHOICE_MODE_NONE || mCheckedIdStates == NULL || mAdapter == NULL) {
        *result = ArrayOf<Int64>::Alloc(0);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IInt64SparseArray> idStates = mCheckedIdStates;
    Int32 count;
    idStates->GetSize(&count);
    AutoPtr<ArrayOf<Int64> > ids = ArrayOf<Int64>::Alloc(count);

    for (Int32 i = 0; i < count; i++) {
        idStates->KeyAt(i, &(*ids)[i]);
    }

    *result = ids;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode AbsListView::ClearChoices()
{
    if (mCheckStates != NULL) {
        mCheckStates->Clear();
    }
    if (mCheckedIdStates != NULL) {
        mCheckedIdStates->Clear();
    }
    mCheckedItemCount = 0;

    return NOERROR;
}

ECode AbsListView::SetItemChecked(
    /* [in] */ Int32 position,
    /* [in] */ Boolean value)
{
    Boolean res;
    Int32 size;
    if (mChoiceMode == IAbsListView::CHOICE_MODE_NONE) {
        return NOERROR;
    }

    // Start selection mode if needed. We don't need to if we're unchecking something.
    if (value && mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE_MODAL && mChoiceActionMode == NULL) {
        Boolean hasWrappedCb;
        if (mMultiChoiceModeCallback == NULL
                || (mMultiChoiceModeCallback->HasWrappedCallback(&hasWrappedCb), !hasWrappedCb)) {
            Logger::E("AbsListView",
                    "AbsListView: attempted to start selection mode for CHOICE_MODE_MULTIPLE_MODAL " \
                    "but no choice mode callback wassupplied. Call setMultiChoiceModeListener to set a callback");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        StartActionMode(IActionModeCallback::Probe(mMultiChoiceModeCallback), (IActionMode**)&mChoiceActionMode);
    }

    if (mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE || mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE_MODAL) {
        Boolean oldValue;
        mCheckStates->Get(position, &oldValue);
        mCheckStates->Put(position, value);
        Boolean hasId;
        IAdapter::Probe(mAdapter)->HasStableIds(&hasId);
        if (mCheckedIdStates != NULL && hasId) {
            Int64 key;
            IAdapter::Probe(mAdapter)->GetItemId(position, &key);

            if (value) {
                mCheckedIdStates->Put(key, CoreUtils::Convert(position));
            }
            else {
                mCheckedIdStates->Delete(key);
            }

        }
        if (oldValue != value) {
            if (value) {
                mCheckedItemCount++;
            }
            else {
                mCheckedItemCount--;
            }
        }
        if (mChoiceActionMode != NULL) {
            Int64 id;
            IAdapter::Probe(mAdapter)->GetItemId(position, &id);
            mMultiChoiceModeCallback->OnItemCheckedStateChanged(mChoiceActionMode,
                    position, id, value);
        }
    }
    else {
        Boolean hasId;
        IAdapter::Probe(mAdapter)->HasStableIds(&hasId);
        Boolean updateIds = mCheckedIdStates != NULL && hasId;
        // Clear all values if we're checking something, or unchecking the currently
        // selected item
        Boolean checked;
        IsItemChecked(position, &checked);
        if (value || checked) {
            mCheckStates->Clear();
            if (updateIds) {
                mCheckedIdStates->Clear();
            }
        }
        // this may end up selecting the value we just cleared but this way
        // we ensure length of mCheckStates is 1, a fact getCheckedItemPosition relies on
        if (value) {
            mCheckStates->Put(position, TRUE);
            if (updateIds) {
                Int64 id;
                IAdapter::Probe(mAdapter)->GetItemId(position, &id);
                mCheckedIdStates->Put(id, CoreUtils::Convert(position));
            }
            mCheckedItemCount = 1;
        }
        else if ((mCheckStates->GetSize(&size), size) == 0 || (mCheckStates->ValueAt(0, &res), !res)) {
            mCheckedItemCount = 0;
        }
    }

    // Do not generate a data change while we are in the layout phase
    if (!mInLayout && !mBlockLayoutRequests) {
        mDataChanged = TRUE;
        RememberSyncState();
        RequestLayout();
    }

    return NOERROR;
}

ECode AbsListView::PerformItemClick(
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean handled = FALSE;
    Boolean dispatchItemClick = TRUE;

    AutoPtr<IAdapter> adapter = IAdapter::Probe(mAdapter);

    Int32 size;
    Boolean res;
    if (mChoiceMode != IAbsListView::CHOICE_MODE_NONE) {
        handled = TRUE;
        Boolean checkedStateChanged = FALSE;

        if (mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE ||
                (mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE_MODAL && mChoiceActionMode != NULL)) {
            mCheckStates->Get(position, FALSE, &res);
            Boolean checked = !res;
            mCheckStates->Put(position, checked);

            Boolean hasId;
            adapter->HasStableIds(&hasId);
            if (mCheckedIdStates != NULL && hasId) {
                if (checked) {
                    Int64 id;
                    adapter->GetItemId(position, &id);
                    mCheckedIdStates->Put(id, CoreUtils::Convert(position));
                }
                else {
                    Int64 id;
                    adapter->GetItemId(position, &id);
                    mCheckedIdStates->Delete(id);
                }
            }
            if (checked) {
                mCheckedItemCount++;
            }
            else {
                mCheckedItemCount--;
            }
            if (mChoiceActionMode != NULL) {
                mMultiChoiceModeCallback->OnItemCheckedStateChanged(mChoiceActionMode,
                        position, id, checked);
                dispatchItemClick = FALSE;
            }
            checkedStateChanged = TRUE;
        }
        else if (mChoiceMode == IAbsListView::CHOICE_MODE_SINGLE) {
            mCheckStates->Get(position, FALSE, &res);
            Boolean checked = !res;

            if (checked) {
                mCheckStates->Clear();
                mCheckStates->Put(position, TRUE);
                Boolean hasId;
                adapter->HasStableIds(&hasId);
                if (mCheckedIdStates != NULL && hasId) {
                    mCheckedIdStates->Clear();
                    Int64 itemId;
                    adapter->GetItemId(position, &itemId);
                    mCheckedIdStates->Put(itemId, CoreUtils::Convert(position));
                }
                mCheckedItemCount = 1;
            }
            else if ((mCheckStates->GetSize(&size), size) = 0
                    || (mCheckStates->ValueAt(0, &res), !res)) {
                mCheckedItemCount = 0;
            }
            checkedStateChanged = TRUE;
        }

        if (checkedStateChanged) {
            UpdateOnScreenCheckedViews();
        }
    }

    if (dispatchItemClick) {
        Boolean re;
        AdapterView::PerformItemClick(view, position, id, &re);
        handled |= re;
    }

    *result = handled;
    return NOERROR;
}

void AbsListView::UpdateOnScreenCheckedViews()
{
    Int32 firstPos = mFirstPosition;
    Int32 count;
    GetChildCount(&count);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);

    Int32 sdkVersion;
    info->GetTargetSdkVersion(&sdkVersion);
    Boolean useActivated = sdkVersion >= Build::VERSION_CODES::HONEYCOMB;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 position = firstPos + i;

        AutoPtr<ICheckable> checkable = ICheckable::Probe(child);
        Boolean res;
        mCheckStates->Get(position, &res);
        if (checkable != NULL) {
            checkable ->SetChecked(res);
        }
        else if (useActivated) {
            child->SetActivated(res);
        }
    }

}

ECode AbsListView::GetChoiceMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChoiceMode;
    return NOERROR;
}

ECode AbsListView::SetChoiceMode(
    /* [in] */ Int32 choiceMode)
{
    mChoiceMode = choiceMode;
    if (mChoiceActionMode != NULL) {
        mChoiceActionMode->Finish();
        mChoiceActionMode = NULL;
    }
    if (mChoiceMode != IAbsListView::CHOICE_MODE_NONE) {
        if (mCheckStates == NULL) {
            CSparseBooleanArray::New(0, (ISparseBooleanArray**)&mCheckStates);
        }
        Boolean stableIds;
        if (mCheckedIdStates == NULL && mAdapter != NULL &&
            (IAdapter::Probe(mAdapter)->HasStableIds(&stableIds), stableIds)) {
            CInt64SparseArray::New(0, (IInt64SparseArray**)&mCheckedIdStates);
        }
        // Modal multi-choice mode only has choices when the mode is active. Clear them.
        if (mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE_MODAL) {
            ClearChoices();
            SetLongClickable(TRUE);
        }
    }
    return NOERROR;
}

ECode AbsListView::SetMultiChoiceModeListener(
    /* [in] */ IMultiChoiceModeListener* listener)
{
    if (mMultiChoiceModeCallback == NULL) {
        mMultiChoiceModeCallback = new MultiChoiceModeWrapper(this);
    }
    mMultiChoiceModeCallback->SetWrapped(listener);
    return NOERROR;
}

Boolean AbsListView::ContentFits()
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount == 0) {
        return TRUE;
    }
    if (childCount != mItemCount) {
        return FALSE;
    }

    AutoPtr<IView> child;
    GetChildAt(0, (IView**)&child);
    Int32 childTop;
    child->GetTop(&childTop);
    child = NULL;
    GetChildAt(childCount - 1, (IView**)&child);
    Int32 childBottom;
    child->GetBottom(&childBottom);

    Int32 height;
    GetHeight(&height);

    Int32 topTmp = 0, bottomTmp = 0;
    mListPadding->GetTop(&topTmp);
    mListPadding->GetBottom(&bottomTmp);
    return (childTop >= topTmp && childBottom <= height - bottomTmp);
}

ECode AbsListView::SetFastScrollEnabled(
    /* [in] */ Boolean enabled)
{
    if (mFastScrollEnabled != enabled) {
        mFastScrollEnabled = enabled;

        if (IsOwnerThread()) {
            SetFastScrollerEnabledUiThread(enabled);
        }
        else {
            AutoPtr<SetFastScrollRunnable> runnable = new SetFastScrollRunnable(this, enabled, 0);
            Boolean res;
            Post(runnable, &res);
        }
    }

    return NOERROR;
}

void AbsListView::SetFastScrollerEnabledUiThread(
    /* [in] */ Boolean enabled)
{
    if (mFastScroll != NULL) {
        mFastScroll->SetEnabled(enabled);
    }
    else if (enabled) {
        mFastScroll = new FastScroller(this, mFastScrollStyle);
        mFastScroll->SetEnabled(TRUE);
    }

    ResolvePadding();

    if (mFastScroll != NULL) {
        mFastScroll->UpdateLayout();
    }
}

ECode AbsListView::SetFastScrollStyle(
    /* [in] */ Int32 styleResId)
{
    if (mFastScroll == NULL) {
        mFastScrollStyle = styleResId;
    }
    else {
        mFastScroll->SetStyle(styleResId);
    }
    return NOERROR;
}

ECode AbsListView::SetFastScrollAlwaysVisible(
    /* [in] */ Boolean alwaysShow)
{
     if (mFastScrollAlwaysVisible != alwaysShow) {
        if (alwaysShow && !mFastScrollEnabled) {
            SetFastScrollEnabled(TRUE);
        }

        mFastScrollAlwaysVisible = alwaysShow;

        if (IsOwnerThread()) {
            SetFastScrollerAlwaysVisibleUiThread(alwaysShow);
        }
        else {
            AutoPtr<SetFastScrollRunnable> runnable = new SetFastScrollRunnable(this, alwaysShow, 1);
            Boolean res;
            Post(runnable, &res);
        }
    }

    return NOERROR;
}

void AbsListView::SetFastScrollerAlwaysVisibleUiThread(
    /* [in] */ Boolean alwaysShow)
{
    if (mFastScroll != NULL) {
        mFastScroll->SetAlwaysShow(alwaysShow);
    }
}

Boolean AbsListView::IsOwnerThread()
{
    return mOwnerThread == Thread::GetCurrentThread();
}

ECode AbsListView::IsFastScrollAlwaysVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mFastScroll == NULL) {
        *result = mFastScrollEnabled && mFastScrollAlwaysVisible;
        return NOERROR;
    }
    else {
        *result = mFastScroll->IsEnabled() && mFastScroll->IsAlwaysShowEnabled();
        return NOERROR;
    }
}

ECode AbsListView::GetVerticalScrollbarWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    if (mFastScroll != NULL && mFastScroll->IsEnabled()) {
        Int32 w;
        View::GetVerticalScrollbarWidth(&w);
        *width = Elastos::Core::Math::Max(w, mFastScroll->GetWidth());
        return NOERROR;
    }
    return View::GetVerticalScrollbarWidth(width);
}

ECode AbsListView::IsFastScrollEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);

    if (mFastScroll == NULL) {
        *isEnabled = mFastScrollEnabled;
        return NOERROR;
    }
    else {
        *isEnabled = mFastScroll->IsEnabled();
        return NOERROR;
    }
}

ECode AbsListView::SetVerticalScrollbarPosition(
    /* [in] */ Int32 position)
{
    View::SetVerticalScrollbarPosition(position);
    if (mFastScroll != NULL) {
        mFastScroll->SetScrollbarPosition(position);
    }
    return NOERROR;
}

ECode AbsListView::SetScrollBarStyle(
    /* [in] */ Int32 style)
{
    View::SetScrollBarStyle(style);
    if (mFastScroll != NULL) {
        mFastScroll->SetScrollBarStyle(style);
    }
    return NOERROR;
}

Boolean AbsListView::IsVerticalScrollBarHidden()
{
    Boolean res;
    IsFastScrollEnabled(&res);
    return res;
}

ECode AbsListView::SetSmoothScrollbarEnabled(
    /* [in] */ Boolean enabled)
{
    mSmoothScrollbarEnabled = enabled;

    return NOERROR;
}

ECode AbsListView::IsSmoothScrollbarEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = mSmoothScrollbarEnabled;
    return NOERROR;
}

ECode AbsListView::SetOnScrollListener(
    /* [in] */ IAbsListViewOnScrollListener* l)
{
    mOnScrollListener = l;
    InvokeOnItemScrollListener();

    return NOERROR;
}

ECode AbsListView::InvokeOnItemScrollListener()
{
    Int32 count;
    GetChildCount(&count);
    if (mFastScroll != NULL) {
        mFastScroll->OnScroll(mFirstPosition, count, mItemCount);
    }

    if (mOnScrollListener != NULL) {
        mOnScrollListener->OnScroll(this,
                mFirstPosition, count, mItemCount);
    }

    OnScrollChanged(0, 0, 0, 0); // dummy values, View's implementation does not use these.

    return NOERROR;
}

ECode AbsListView::SendAccessibilityEvent(
    /* [in] */ Int32 eventType)
{
    // Since this class calls onScrollChanged even if the mFirstPosition and the
    // child count have not changed we will avoid sending duplicate accessibility
    // events.
    if (eventType == IAccessibilityEvent::TYPE_VIEW_SCROLLED) {
        Int32 firstVisiblePosition;
        GetFirstVisiblePosition(&firstVisiblePosition);
        Int32 lastVisiblePosition;
        GetLastVisiblePosition(&lastVisiblePosition);
        if (mLastAccessibilityScrollEventFromIndex == firstVisiblePosition
                && mLastAccessibilityScrollEventToIndex == lastVisiblePosition) {
            return NOERROR;
        }
        else {
            mLastAccessibilityScrollEventFromIndex = firstVisiblePosition;
            mLastAccessibilityScrollEventToIndex = lastVisiblePosition;
        }
    }
    return View::SendAccessibilityEvent(eventType);
}

ECode AbsListView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FAIL_RETURN(View::OnInitializeAccessibilityEvent(event));
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert("AbsListView"));

    return NOERROR;
}

ECode AbsListView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    View::OnInitializeAccessibilityNodeInfo(info);
    info->SetClassName(CoreUtils::Convert("AbsListView"));
    Boolean res;
    if (IsEnabled(&res), res) {
        if (CanScrollUp()) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
            info->SetScrollable(TRUE);
        }
        if (CanScrollDown()) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
            info->SetScrollable(TRUE);
        }
    }
    return NOERROR;
}

Int32 AbsListView::GetSelectionModeForAccessibility() {
    Int32 choiceMode;
    GetChoiceMode(&choiceMode);
    switch (choiceMode) {
        case IAbsListView::CHOICE_MODE_NONE:
            return IAccessibilityNodeInfoCollectionInfo::SELECTION_MODE_NONE;
        case IAbsListView::CHOICE_MODE_SINGLE:
            return IAccessibilityNodeInfoCollectionInfo::SELECTION_MODE_SINGLE;
        case IAbsListView::CHOICE_MODE_MULTIPLE:
        case IAbsListView::CHOICE_MODE_MULTIPLE_MODAL:
            return IAccessibilityNodeInfoCollectionInfo::SELECTION_MODE_MULTIPLE;
        default:
            return IAccessibilityNodeInfoCollectionInfo::SELECTION_MODE_NONE;
    }
}

ECode AbsListView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    Boolean result;
    if (View::PerformAccessibilityAction(action, arguments, &result), result) {
        *res = TRUE;
        return NOERROR;
    }

    switch(action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
            Int32 count;
            GetCount(&count);
            Int32 pos;
            GetLastVisiblePosition(&pos);
            if ((IsEnabled(&result), result) && (pos < count - 1)) {
                Int32 h;
                GetHeight(&h);

                Int32 topTmp = 0, bottomTmp = 0;
                mListPadding->GetTop(&topTmp);
                mListPadding->GetBottom(&bottomTmp);
                Int32 viewportHeight = h - topTmp - bottomTmp;
                SmoothScrollBy(viewportHeight, PositionScroller::SCROLL_DURATION);
                *res = TRUE;
                return NOERROR;
            }
        }
        return NOERROR;

        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
            if ((IsEnabled(&result), result) && mFirstPosition > 0) {
                Int32 h;
                GetHeight(&h);

                Int32 topTmp = 0, bottomTmp = 0;
                mListPadding->GetTop(&topTmp);
                mListPadding->GetBottom(&bottomTmp);
                Int32 viewportHeight = h - topTmp - bottomTmp;
                SmoothScrollBy(-viewportHeight, PositionScroller::SCROLL_DURATION);
                *res = TRUE;
                return NOERROR;
            }
        }
        return NOERROR;
    }

    return NOERROR;
}

ECode AbsListView::FindViewByAccessibilityIdTraversal(
    /* [in] */ Int32 accessibilityId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    Int32 id;
    if (accessibilityId == (GetAccessibilityViewId(&id), id)) {
        *view = this;
        REFCOUNT_ADD(*view);
        return NOERROR;
    }
    // If the data changed the children are invalid since the data model changed.
    // Hence, we pretend they do not exist. After a layout the children will sync
    // with the model at which point we notify that the accessibility state changed,
    // so a service will be able to re-fetch the views.
    if (mDataChanged) {
        *view = NULL;
        return NOERROR;
    }

    return ViewGroup::FindViewByAccessibilityIdTraversal(accessibilityId, view);
}

ECode AbsListView::IsScrollingCacheEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = mScrollingCacheEnabled;

    return NOERROR;
}

ECode AbsListView::SetScrollingCacheEnabled(
    /* [in] */ Boolean enabled)
{
    if (mScrollingCacheEnabled && !enabled) {
        ClearScrollingCache();
    }
    mScrollingCacheEnabled = enabled;

    return NOERROR;
}

ECode AbsListView::SetTextFilterEnabled(
    /* [in] */ Boolean textFilterEnabled)
{
    mTextFilterEnabled = textFilterEnabled;

    return NOERROR;
}

ECode AbsListView::IsTextFilterEnabled(
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = mTextFilterEnabled;

    return NOERROR;
}

ECode AbsListView::GetFocusedRect(
    /* [in] */ IRect* r)
{
    AutoPtr<IView> view;
    GetSelectedView((IView**)&view);
    if (view != NULL) {
        AutoPtr<IViewParent> vp;
        view->GetParent((IViewParent**)&vp);
        if (IView::Probe(vp) == (IView*)this) {
            // the focused rectangle of the selected view offset into the
            // coordinate space of this view.
            view->GetFocusedRect(r);
            OffsetDescendantRectToMyCoords(view, r);
        }
    }
    else {
        // otherwise, just the norm
        return AdapterView::GetFocusedRect(r);
    }
    return NOERROR;

}

ECode AbsListView::UseDefaultSelector()
{
    AutoPtr<IDrawable> drawable;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetDrawable(
            R::drawable::list_selector_background, (IDrawable**)&drawable);
    SetSelector(drawable);

    return NOERROR;
}

ECode AbsListView::IsStackFromBottom(
    /* [out] */ Boolean* isStackFromBottom)
{
    VALIDATE_NOT_NULL(isStackFromBottom);
    *isStackFromBottom = mStackFromBottom;

    return NOERROR;
}

ECode AbsListView::SetStackFromBottom(
    /* [in] */ Boolean stackFromBottom)
{
    if (mStackFromBottom != stackFromBottom) {
        mStackFromBottom = stackFromBottom;
        RequestLayoutIfNecessary();
    }

    return NOERROR;
}

ECode AbsListView::RequestLayoutIfNecessary()
{
    Int32 count;
    GetChildCount(&count);
    if (count > 0) {
        ResetList();
        RequestLayout();
        Invalidate();
    }

    return NOERROR;
}

AutoPtr<IParcelable> AbsListView::OnSaveInstanceState()
{
    /*
     * This doesn't really make sense as the place to dismiss the
     * popups, but there don't seem to be any other useful hooks
     * that happen early enough to keep from getting complaints
     * about having leaked the window.
     */
    DismissPopup();

    AutoPtr<IParcelable> superState = AdapterView::OnSaveInstanceState();

    AutoPtr<CAbsListViewSavedState> ss;
    ASSERT_SUCCEEDED(CAbsListViewSavedState::NewByFriend(
            superState, (CAbsListViewSavedState**)&ss));

    if (mPendingSync != NULL) {
        // Just keep what we last restored.
        ss->mSelectedId = mPendingSync->mSelectedId;
        ss->mFirstId = mPendingSync->mFirstId;
        ss->mViewTop = mPendingSync->mViewTop;
        ss->mPosition = mPendingSync->mPosition;
        ss->mHeight = mPendingSync->mHeight;
        ss->mFilter = mPendingSync->mFilter;
        ss->mInActionMode = mPendingSync->mInActionMode;
        ss->mCheckedItemCount = mPendingSync->mCheckedItemCount;
        ss->mCheckState = mPendingSync->mCheckState;
        ss->mCheckIdState = mPendingSync->mCheckIdState;
        return (IParcelable*)ss->Probe(EIID_IParcelable);
    }

    Int32 count;
    GetChildCount(&count);
    Boolean haveChildren = count > 0 && mItemCount > 0;
    Int64 selectedId;
    GetSelectedItemId(&selectedId);
    ss->mSelectedId = selectedId;
    GetHeight(&ss->mHeight);

    if (selectedId >= 0) {
        // Remember the selection
        ss->mViewTop = this->mSelectedTop;
        GetSelectedItemPosition(&ss->mPosition);
        ss->mFirstId = IAdapterView::INVALID_POSITION;
    }
    else {
        if (haveChildren && mFirstPosition > 0) {
            // Remember the position of the first child.
            // We only do this if we are not currently at the top of
            // the list, for two reasons:
            // (1) The list may be in the process of becoming empty, in
            // which case mItemCount may not be 0, but if we try to
            // ask for any information about position 0 we will crash.
            // (2) Being "at the top" seems like a special case, anyway,
            // and the user wouldn't expect to end up somewhere else when
            // they revisit the list even if its content has changed.
            AutoPtr<IView> v;
            GetChildAt(0, (IView**)&v);
            v->GetTop(&ss->mViewTop);
            Int32 firstPos = mFirstPosition;
            if (firstPos >= mItemCount) {
                firstPos = mItemCount - 1;
            }
            ss->mPosition = firstPos;
            IAdapter::Probe(mAdapter)->GetItemId(firstPos, &ss->mFirstId);
        }
        else {
            ss->mViewTop = 0;
            ss->mFirstId = IAdapterView::INVALID_POSITION;
            ss->mPosition = 0;
        }
    }
    ss->mFilter = NULL;
    if (mFiltered) {
        AutoPtr<IEditText> textFilter = mTextFilter;
        if (textFilter != NULL) {
            AutoPtr<ICharSequence> filterText;
            ITextView::Probe(textFilter)->GetText((ICharSequence**)&filterText);
            if (filterText->Probe(EIID_IEditable) != NULL) {
                filterText->ToString(&(ss->mFilter));
            }
        }
    }

    ss->mInActionMode = (mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE_MODAL) && (mChoiceActionMode != NULL);

    if (mCheckStates != NULL) {
        AutoPtr<IInterface> obj;
        ICloneable::Probe(mCheckStates)->Clone((IInterface**)&obj);
        ss->mCheckState = ISparseBooleanArray::Probe(obj);
    }

    if (mCheckedIdStates != NULL) {
        AutoPtr<IInt64SparseArray> idState;
        CInt64SparseArray::New((IInt64SparseArray**)&idState);
        Int32 count;
        mCheckedIdStates->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            Int64 key;
            mCheckedIdStates->KeyAt(i, &key);
            AutoPtr<IInterface> obj;
            mCheckedIdStates->ValueAt(i, (IInterface**)&obj);
            idState->Put(key, obj);
        }
        ss->mCheckIdState = idState;
    }
    ss->mCheckedItemCount = mCheckedItemCount;

    if (mRemoteAdapter != NULL) {
        mRemoteAdapter->SaveRemoteViewsCache();
    }

    return (IParcelable*)ss->Probe(EIID_IParcelable);
}

void AbsListView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<CAbsListViewSavedState> ss =
            (CAbsListViewSavedState*)IAbsListViewSavedState::Probe(state);
    if (!ss) return;

    AutoPtr<IParcelable> superState;
    ss->GetSuperState((IParcelable**)&superState);

    AdapterView::OnRestoreInstanceState(superState);

    mDataChanged = TRUE;

    mSyncHeight = ss->mHeight;

    if (ss->mSelectedId >= 0) {
        mNeedSync = TRUE;
        mPendingSync = ss;
        mSyncRowId = ss->mSelectedId;
        mSyncPosition = ss->mPosition;
        mSpecificTop = ss->mViewTop;
        mSyncMode = AdapterView::SYNC_SELECTED_POSITION;
    }
    else if (ss->mFirstId >= 0) {
        SetSelectedPositionInt(IAdapterView::INVALID_POSITION);
        // Do this before setting mNeedSync since setNextSelectedPosition looks at mNeedSync
        SetNextSelectedPositionInt(IAdapterView::INVALID_POSITION);
        mSelectorPosition = IAdapterView::INVALID_POSITION;
        mNeedSync = TRUE;
        mPendingSync = ss;
        mSyncRowId = ss->mFirstId;
        mSyncPosition = ss->mPosition;
        mSpecificTop = ss->mViewTop;
        mSyncMode = AdapterView::SYNC_FIRST_POSITION;
    }

    SetFilterText(ss->mFilter);

    if (ss->mCheckState != NULL) {
        mCheckStates = ss->mCheckState;
    }

    if (ss->mCheckIdState != NULL) {
        mCheckedIdStates = ss->mCheckIdState;
    }

    mCheckedItemCount = ss->mCheckedItemCount;

    if (ss->mInActionMode && mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE_MODAL &&
        mMultiChoiceModeCallback != NULL) {
                StartActionMode(IActionModeCallback::Probe(mMultiChoiceModeCallback),
                        (IActionMode**)&mChoiceActionMode);
    }

    RequestLayout();
}

Boolean AbsListView::AcceptFilter()
{
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    AutoPtr<IFilterable> temp = IFilterable::Probe(adapter);
    if (temp == NULL || !mTextFilterEnabled) {
        return FALSE;
    }
    else {
        AutoPtr<IFilter> filter;
        temp->GetFilter((IFilter**)&filter);
        if (filter != NULL){
            return TRUE;
        }
    }
    return FALSE;
}

ECode AbsListView::SetFilterText(
    /* [in] */ const String& filterText)
{
    // TODO: Should we check for acceptFilter()?
    if (mTextFilterEnabled && !filterText.IsNullOrEmpty()) {
        CreateTextFilter(FALSE);
        // This is going to call our listener onTextChanged, but we might not
        // be ready to bring up a window yet
        ITextView::Probe(mTextFilter)->SetText(CoreUtils::Convert(filterText));
        mTextFilter->SetSelection(filterText.GetLength());

        AutoPtr<IFilterable> filterable = IFilterable::Probe(mAdapter);
        if (filterable) {
            // if mPopup is non-NULL, then onTextChanged will do the filtering
            if (mPopup == NULL) {
                AutoPtr<IFilter> f;
                filterable->GetFilter((IFilter**)&f);
                f->DoFilter(CoreUtils::Convert(filterText));
            }
            // Set filtered to TRUE so we will display the filter window when our main
            // window is ready
            mFiltered = TRUE;
            mDataSetObserver->ClearSavedState();
        }
    }
    return NOERROR;
}

ECode AbsListView::GetTextFilter(
    /* [out] */ ICharSequence** textFilter)
{
    VALIDATE_NOT_NULL(textFilter);
    *textFilter = NULL;

    if (mTextFilterEnabled && mTextFilter != NULL) {
        AutoPtr<ICharSequence> result;
        ITextView::Probe(mTextFilter)->GetText((ICharSequence**)&result);
        *textFilter = result;
        REFCOUNT_ADD(*textFilter);

        return NOERROR;
    }
    return NOERROR;
}

void AbsListView::OnFocusChanged(
    /* [in] */ Boolean gainFocus,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AdapterView::OnFocusChanged(gainFocus, direction, previouslyFocusedRect);
    Boolean res;
    if (gainFocus && mSelectedPosition < 0 && (IsInTouchMode(&res), !res)) {
        Boolean res;
        if ((IsAttachedToWindow(&res), !res) && mAdapter != NULL) {
            // Data may have changed while we were detached and it's valid
            // to change focus while detached. Refresh so we don't die.
            mDataChanged = TRUE;
            mOldItemCount = mItemCount;
            IAdapter::Probe(mAdapter)->GetCount(&mItemCount);
        }
        ResurrectSelection();
    }
}

ECode AbsListView::RequestLayout()
{
    if (!mBlockLayoutRequests && !mInLayout) {
        AdapterView::RequestLayout();
    }

    return NOERROR;
}

void AbsListView::ResetList()
{
    RemoveAllViewsInLayout();
    mFirstPosition = 0;
    mDataChanged = FALSE;
    mPositionScrollAfterLayout = NULL;
    mNeedSync = FALSE;
    mPendingSync = NULL;
    mOldSelectedPosition = IAdapterView::INVALID_POSITION;
    mOldSelectedRowId = IAdapterView::INVALID_ROW_ID;
    SetSelectedPositionInt(IAdapterView::INVALID_POSITION);
    SetNextSelectedPositionInt(IAdapterView::INVALID_POSITION);
    mSelectedTop = 0;
    mSelectorPosition = IAdapterView::INVALID_POSITION;
    mSelectorRect->SetEmpty();
    Invalidate();
}

Int32 AbsListView::ComputeVerticalScrollExtent()
{
    Int32 count;
    GetChildCount(&count);
    if (count > 0) {
        if (mSmoothScrollbarEnabled) {
            Int32 extent = count * 100;

            AutoPtr<IView> view;
            GetChildAt(0, (IView**)&view);
            Int32 top, height;
            view->GetTop(&top);
            view->GetHeight(&height);

            if (height > 0) {
                extent += (top * 100) / height;
            }

            view = NULL;
            GetChildAt(count - 1, (IView**)&view);

            Int32 bottom;
            view->GetBottom(&bottom);
            view->GetHeight(&height);

            if (height > 0) {
                Int32 h;
                GetHeight(&h);
                extent -= ((bottom - h) * 100) / height;
            }

            return extent;
        }
        else {
            return 1;
        }
    }
    return 0;
}

Int32 AbsListView::ComputeVerticalScrollOffset()
{
    Int32 firstPosition = mFirstPosition;
    Int32 childCount;
    GetChildCount(&childCount);
    if (firstPosition >= 0 && childCount > 0) {
        if (mSmoothScrollbarEnabled) {
            AutoPtr<IView> view;
            GetChildAt(0, (IView**)&view);
            Int32 top, height;
            view->GetTop(&top);
            view->GetHeight(&height);
            if (height > 0) {
                Int32 h;
                GetHeight(&h);
                return Elastos::Core::Math::Max(firstPosition * 100 - (top * 100) / height +
                        (Int32)((Float)mScrollY / h * mItemCount * 100), 0);
            }
        }
        else {
            Int32 index;
            Int32 count = mItemCount;
            if (firstPosition == 0) {
                index = 0;
            }
            else if (firstPosition + childCount == count) {
                index = count;
            }
            else {
                index = firstPosition + childCount / 2;
            }
            return (Int32)(firstPosition + childCount * (index / (Float)count));
        }
    }
    return 0;
}

Int32 AbsListView::ComputeVerticalScrollRange()
{
    Int32 result;
    if (mSmoothScrollbarEnabled) {
        result = Elastos::Core::Math::Max(mItemCount * 100, 0);
        if (mScrollY != 0) {
            // Compensate for overscroll
            Int32 h;
            GetHeight(&h);
            result += Elastos::Core::Math::Abs((Int32)((Float) mScrollY / h * mItemCount * 100));
        }
    }
    else {
        result = mItemCount;
    }

    return result;
}

Float AbsListView::GetTopFadingEdgeStrength()
{
    Int32 count;
    GetChildCount(&count);
    Float fadeEdge = AdapterView::GetTopFadingEdgeStrength();
    if (count == 0) {
        return fadeEdge;
    }
    else {
        if (mFirstPosition > 0) {
            return 1.0f;
        }

        AutoPtr<IView> view;
        GetChildAt(0, (IView**)&view);
        Int32 top;
        view->GetTop(&top);
        Int32 length;
        GetVerticalFadingEdgeLength(&length);
        Float fadeLength = (Float)length;
        return top < mPaddingTop ? -(top - mPaddingTop) / fadeLength : fadeEdge;
    }
}

Float AbsListView::GetBottomFadingEdgeStrength()
{
    Int32 count;
    GetChildCount(&count);
    Float fadeEdge = AdapterView::GetBottomFadingEdgeStrength();
    if (count == 0) {
        return fadeEdge;
    }
    else {
        if (mFirstPosition + count - 1 < mItemCount - 1) {
            return 1.0f;
        }

        AutoPtr<IView> view;
        GetChildAt(count - 1, (IView**)&view);
        Int32 bottom;
        view->GetBottom(&bottom);

        Int32 height;
        GetHeight(&height);
        Int32 length;
        GetVerticalFadingEdgeLength(&length);
        Float fadeLength = length;
        return bottom > height - mPaddingBottom ?
                (bottom - height + mPaddingBottom) / fadeLength : fadeEdge;
    }
}

void AbsListView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (mSelector == NULL) {
        UseDefaultSelector();
    }

    mListPadding->Set(mSelectionLeftPadding + mPaddingLeft
            , mSelectionTopPadding + mPaddingTop
            , mSelectionRightPadding + mPaddingRight
            , mSelectionBottomPadding + mPaddingBottom);

    // Check if our previous measured size was at a point where we should scroll later.
    if (mTranscriptMode == IAbsListView::AbsListView_TRANSCRIPT_MODE_NORMAL) {
        Int32 childCount;
        GetChildCount(&childCount);
        Int32 height, bottom;
        GetHeight(&height);
        GetPaddingBottom(&bottom);
        Int32 listBottom = height - bottom;
        AutoPtr<IView> lastChild;
        GetChildAt(childCount - 1, (IView**)&lastChild);
        Int32 lastChildBottom;
        if (lastChild != NULL) {
            lastChild->GetBottom(&lastChildBottom);
        }
        else {
            lastChildBottom = listBottom;
        }

        mForceTranscriptScroll = mFirstPosition + childCount >= mLastHandledItemCount &&
                lastChildBottom <= listBottom;
    }
}

ECode AbsListView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    AdapterView::OnLayout(changed, l, t, r, b);

    mInLayout = TRUE;

    Int32 childCount;
    GetChildCount(&childCount);
    if (changed) {
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> view;
            GetChildAt(i, (IView**)&view);
            view->ForceLayout();
        }
        mRecycler->MarkChildrenDirty();
    }
    LayoutChildren();
    mInLayout = FALSE;

    mOverscrollMax = (b - t) / AbsListView::OVERSCROLL_LIMIT_DIVISOR;

    // TODO: Move somewhere sane. This doesn't belong in onLayout().
    GetChildCount(&childCount);
    if (mFastScroll != NULL) {
        mFastScroll->OnItemCountChanged(childCount, mItemCount);
    }
    return NOERROR;
}

Boolean AbsListView::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Boolean changed = AdapterView::SetFrame(left, top, right, bottom);

    if (changed) {
        // Reposition the popup when the frame has changed. This includes
        // translating the widget, not just changing its dimension. The
        // filter popup needs to follow the widget.
        Int32 visibility;
        GetWindowVisibility(&visibility);
        Boolean visible = visibility == IView::VISIBLE;
        Boolean showing = FALSE;
        if (mPopup) {
            mPopup->IsShowing(&showing);
        }
        if (mFiltered && visible && showing) {
            PositionPopup();
        }
    }

    return changed;
}

ECode AbsListView::LayoutChildren()
{
    return NOERROR;
}

AutoPtr<IView> AbsListView::GetAccessibilityFocusedChild(
    /* [in] */ IView* focusedView)
{
    AutoPtr<IViewParent> viewParent;
    focusedView->GetParent((IViewParent**)&viewParent);
    while (IView::Probe(viewParent) && IView::Probe(viewParent) != (IView*)this) {
        focusedView = IView::Probe(viewParent);
        AutoPtr<IViewParent> tmp;
        viewParent->GetParent((IViewParent**)&tmp);
        viewParent = tmp;
    }

    if (IView::Probe(viewParent) == NULL) {
        return NULL;
    }
    return focusedView;
}

void AbsListView::UpdateScrollIndicators()
{
    if (mScrollUp != NULL) {
        mScrollUp->SetVisibility(CanScrollUp() ? IView::VISIBLE : IView::INVISIBLE);
    }

    if (mScrollDown != NULL) {
        mScrollDown->SetVisibility(CanScrollDown() ? IView::VISIBLE : IView::INVISIBLE);
    }
}

Boolean AbsListView::CanScrollUp()
{
    Boolean canScrollUp;
    // 0th element is not visible
    canScrollUp = mFirstPosition > 0;

    // ... Or top of 0th element is not visible
    if (!canScrollUp) {
        Int32 count;
        GetChildCount(&count);
        if (count > 0) {
            AutoPtr<IView> view;
            GetChildAt(0, (IView**)&view);
            Int32 top;
            view->GetTop(&top);

            Int32 topTmp = 0;
            mListPadding->GetTop(&topTmp);
            canScrollUp = topTmp < top;
        }
    }

    return canScrollUp;
}

Boolean AbsListView::CanScrollDown()
{
    Boolean canScrollDown;
    Int32 count;
    GetChildCount(&count);

    // Last item is not visible
    canScrollDown = (mFirstPosition + count) < mItemCount;

    // ... Or bottom of the last element is not visible
    if (!canScrollDown && count > 0) {
        AutoPtr<IView> view;
        GetChildAt(count - 1, (IView**)&view);
        Int32 bottom;
        view->GetBottom(&bottom);

        Int32 bottomTmp = 0;
        mListPadding->GetBottom(&bottomTmp);
        canScrollDown = bottomTmp > mBottom - bottom;
    }

    return canScrollDown;

}

ECode AbsListView::GetSelectedView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;

    if (mItemCount > 0 && mSelectedPosition >= 0) {
        return GetChildAt(mSelectedPosition - mFirstPosition, view);
    }
    else {
        return NOERROR;
    }
}

ECode AbsListView::GetListPaddingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    mListPadding->GetTop(top);
    return NOERROR;
}

ECode AbsListView::GetListPaddingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    mListPadding->GetBottom(bottom);
    return NOERROR;
}

ECode AbsListView::GetListPaddingLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);
    mListPadding->GetLeft(left);
    return NOERROR;
}

ECode AbsListView::GetListPaddingRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right);
    mListPadding->GetRight(right);
    return NOERROR;
}

AutoPtr<IView> AbsListView::ObtainView(
    /* [in] */ Int32 position,
    /* [in] */ ArrayOf<Boolean>* isScrap)
{
    // Trace.traceBegin(Trace.TRACE_TAG_VIEW, "obtainView");

    (*isScrap)[0] = FALSE;
    AutoPtr<IAdapter> adapter = IAdapter::Probe(mAdapter);

    // Check whether we have a transient state view. Attempt to re-bind the
    // data and discard the view if we fail.
    AutoPtr<IView> transientView = mRecycler->GetTransientStateView(position);
    if (transientView != NULL) {
        AutoPtr<IViewGroupLayoutParams> layoutParams;
        transientView->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
        AutoPtr<IAbsListViewLayoutParams> params = IAbsListViewLayoutParams::Probe(layoutParams);

        // If the view type hasn't changed, attempt to re-bind the data.
        Int32 viewType1, viewType2;
        adapter->GetItemViewType(position, &viewType1);
        params->GetViewType(&viewType2);
        if (viewType2 == viewType1) {
            AutoPtr<IView> updatedView;
            adapter->GetView(position, transientView, this, (IView**)&updatedView);

            // If we failed to re-bind the data, scrap the obtained view.
            if (updatedView != transientView) {
                SetItemViewLayoutParams(updatedView, position);
                mRecycler->AddScrapView(updatedView, position);
            }
        }

        // Scrap view implies temporary detachment.
        (*isScrap)[0] = TRUE;
        return transientView;
    }

    const AutoPtr<IView> scrapView = mRecycler->GetScrapView(position);
    AutoPtr<IView> child;
    adapter->GetView(position, scrapView, this, (IView**)&child);
    if (scrapView != NULL) {
        if (child != scrapView) {
            // Failed to re-bind the data, return scrap to the heap.
            mRecycler->AddScrapView(scrapView, position);
        }
        else {
            (*isScrap)[0] = TRUE;

            child->DispatchFinishTemporaryDetach();
        }
    }

    if (mCacheColorHint != 0) {
        child->SetDrawingCacheBackgroundColor(mCacheColorHint);
    }

    Int32 importantForAccessibility;
    child->GetImportantForAccessibility(&importantForAccessibility);
    if (importantForAccessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        child->SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }

    SetItemViewLayoutParams(child, position);

    AutoPtr<IAccessibilityManager> manager;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&manager);
    Boolean enable = TRUE;
    manager->IsEnabled(&enable);
    if (enable) {
        if (mAccessibilityDelegate == NULL) {
            mAccessibilityDelegate = new ListItemAccessibilityDelegate(this);
        }
        AutoPtr<IAccessibilityDelegate> delegate;
        child->GetAccessibilityDelegate((IAccessibilityDelegate**)&delegate);
        if (delegate == NULL) {
            child->SetAccessibilityDelegate(mAccessibilityDelegate);
        }
    }

    // Trace.traceEnd(Trace.TRACE_TAG_VIEW);
    return child;
}

void AbsListView::SetItemViewLayoutParams(
    /* [in] */ IView* child,
    /* [in] */ Int32 position)
{
    AutoPtr<IViewGroupLayoutParams> vlp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
    AutoPtr<IAbsListViewLayoutParams> lp;
    AutoPtr<IViewGroupLayoutParams> lpTemp;
    if (vlp == NULL) {
        GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&lpTemp);
        lp = IAbsListViewLayoutParams::Probe(lpTemp);
    }
    else if (!CheckLayoutParams(vlp)) {
        AutoPtr<IViewGroupLayoutParams> vglp = GenerateLayoutParams(vlp);
        lp = IAbsListViewLayoutParams::Probe(vglp);
    }
    else {
        lp = IAbsListViewLayoutParams::Probe(vlp);
    }

    if (mAdapterHasStableIds) {
        Int64 itemId;
        IAdapter::Probe(mAdapter)->GetItemId(position, &itemId);
        lp->SetItemId(itemId);
    }
    Int32 viewType;
    IAdapter::Probe(mAdapter)->GetItemViewType(position, &viewType);
    lp->SetViewType(viewType);
    child->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
}

ECode AbsListView::OnInitializeAccessibilityNodeInfoForItem(
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AutoPtr<IAdapter> _adapter;
    GetAdapter((IAdapter**)&_adapter);
    AutoPtr<IListAdapter> adapter = IListAdapter::Probe(_adapter);

    if (position == IAdapterView::INVALID_POSITION || adapter == NULL) {
        // The item doesn't exist, so there's not much we can do here.
        return NOERROR;
    }

    Boolean res, res1;
    if ((IsEnabled(&res), !res) || (adapter->IsEnabled(position, &res1), !res1)) {
        info->SetEnabled(FALSE);
        return NOERROR;
    }

    Int32 pos;
    if (position == (GetSelectedItemPosition(&pos), pos)) {
        info->SetSelected(TRUE);
        info->AddAction(IAccessibilityNodeInfo::ACTION_CLEAR_SELECTION);
    }
    else {
        info->AddAction(IAccessibilityNodeInfo::ACTION_SELECT);
    }

    if (IsClickable(&res), res) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_CLICK);
        info->SetClickable(TRUE);
    }

    if (IsLongClickable(&res), res) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_LONG_CLICK);
        info->SetLongClickable(TRUE);
    }
    return NOERROR;
}

void AbsListView::PositionSelectorLikeTouch(
    /* [in] */ Int32 position,
    /* [in] */ IView* sel,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    PositionSelectorLikeFocus(position, sel);

    if (mSelector != NULL && position != IAdapterView::INVALID_POSITION) {
        mSelector->SetHotspot(x, y);
    }
}

void AbsListView::PositionSelectorLikeFocus(
    /* [in] */ Int32 position,
    /* [in] */ IView* sel)
{
    // If we're changing position, update the visibility since the selector
    // is technically being detached from the previous selection.
    AutoPtr<IDrawable> selector = mSelector;
    Boolean manageState = selector != NULL && mSelectorPosition != position
            && position != IAdapterView::INVALID_POSITION;
    Boolean res;
    if (manageState) {
        selector->SetVisible(FALSE, FALSE, &res);
    }

    PositionSelector(position, sel);

    if (manageState) {
        AutoPtr<IRect> bounds = mSelectorRect;
        Float x;
        bounds->GetExactCenterX(&x);
        Float y;
        bounds->GetExactCenterY(&y);
        Int32 visibility;
        GetVisibility(&visibility);
        selector->SetVisible(visibility == IView::VISIBLE, FALSE, &res);
        selector->SetHotspot(x, y);
    }
}

void AbsListView::PositionSelector(
    /* [in] */ Int32 position,
    /* [in] */ IView* sel)
{
    if (position != IAdapterView::INVALID_POSITION) {
        mSelectorPosition = position;
    }

    assert(sel);
    AutoPtr<IRect> selectorRect = mSelectorRect;
    Int32 left, top, right, bottom;
    sel->GetLeft(&left);
    sel->GetTop(&top);
    sel->GetRight(&right);
    sel->GetBottom(&bottom);
    selectorRect->Set(left, top, right, bottom);

    AutoPtr<ISelectionBoundsAdjuster> adjuster = ISelectionBoundsAdjuster::Probe(sel);
    if (adjuster != NULL) {
        adjuster->AdjustListItemSelectionBounds(selectorRect);
    }

    // Adjust for selection padding.
    selectorRect->Set(left - mSelectionLeftPadding,
            top - mSelectionTopPadding,
            right - mSelectionRightPadding,
            bottom - mSelectionBottomPadding);

    // Update the selector drawable.
    AutoPtr<IDrawable> selector = mSelector;
    if (selector != NULL) {
        selector->SetBounds(selectorRect);
    }

    Boolean viewEnabled;
    sel->IsEnabled(&viewEnabled);
    if (viewEnabled != mIsChildViewEnabled) {
        mIsChildViewEnabled = !mIsChildViewEnabled;
        Int32 pos;
        GetSelectedItemPosition(&pos);
        if (pos != IAdapterView::INVALID_POSITION) {
            RefreshDrawableState();
        }
    }
}

ECode AbsListView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 saveCount = 0;
    Boolean clipToPadding = (mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK;
    if (clipToPadding) {
        canvas->Save(&saveCount);

        Boolean result;
        canvas->ClipRect(
                mScrollX + mPaddingLeft, mScrollY + mPaddingTop,
                mScrollX + mRight - mLeft - mPaddingRight,
                mScrollY + mBottom - mTop - mPaddingBottom,
                &result);
        mGroupFlags &= ~ViewGroup::CLIP_TO_PADDING_MASK;
    }

    if (!mDrawSelectorOnTop) {
        DrawSelector(canvas);
    }

    AdapterView::DispatchDraw(canvas);

    if (mDrawSelectorOnTop) {
        DrawSelector(canvas);
    }

    if (clipToPadding) {
        canvas->RestoreToCount(saveCount);
        mGroupFlags |= ViewGroup::CLIP_TO_PADDING_MASK;
    }
    return NOERROR;
}

Boolean AbsListView::IsPaddingOffsetRequired()
{
    return (mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) != ViewGroup::CLIP_TO_PADDING_MASK;
}

Int32 AbsListView::GetLeftPaddingOffset()
{
    return (mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK ? 0 : -mPaddingLeft;
}

Int32 AbsListView::GetTopPaddingOffset()
{
    return (mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK ? 0 : -mPaddingTop;
}

Int32 AbsListView::GetRightPaddingOffset()
{
    return (mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK ? 0 : mPaddingRight;
}

Int32 AbsListView::GetBottomPaddingOffset()
{
    return (mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK ? 0 : mPaddingBottom;
}

void AbsListView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    Int32 count;
    GetChildCount(&count);
    if (count > 0) {
        mDataChanged = TRUE;
        RememberSyncState();
    }

    if (mFastScroll != NULL) {
        mFastScroll->OnSizeChanged(w, h, oldw, oldh);
    }
}

Boolean AbsListView::TouchModeDrawsInPressedState()
{
    // FIXME use isPressed for this
    switch (mTouchMode) {
        case TOUCH_MODE_TAP:
        case TOUCH_MODE_DONE_WAITING:
            return TRUE;
        default:
            return FALSE;
    }
}

Boolean AbsListView::ShouldShowSelector()
{
    Boolean res, res1;
    return (IsInTouchMode(&res), !res) || (TouchModeDrawsInPressedState() && (IsPressed(&res1), res1));
}

void AbsListView::DrawSelector(
    /* [in] */ ICanvas* canvas)
{
    Boolean empty;
    mSelectorRect->IsEmpty(&empty);
    if (!empty) {
        mSelector->SetBounds(mSelectorRect);
        mSelector->Draw(canvas);
    }
}

ECode AbsListView::SetDrawSelectorOnTop(
    /* [in] */ Boolean onTop)
{
    mDrawSelectorOnTop = onTop;

    return NOERROR;
}

ECode AbsListView::SetSelector(
    /* [in] */ Int32 resID)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(resID, (IDrawable**)&drawable);

    return SetSelector(drawable);
}

ECode AbsListView::SetSelector(
    /* [in] */ IDrawable* sel)
{
    if (mSelector != NULL) {
        mSelector->SetCallback(NULL);
        UnscheduleDrawable(mSelector);
    }

    mSelector = sel;
    AutoPtr<IRect> padding;
    CRect::New((IRect**)&padding);
    if (sel == NULL) {
        return NOERROR;
    }

    Boolean result;
    sel->GetPadding(padding, &result);

    padding->GetLeft(&mSelectionLeftPadding);
    padding->GetTop(&mSelectionTopPadding);
    padding->GetRight(&mSelectionRightPadding);
    padding->GetBottom(&mSelectionBottomPadding);
    sel->SetCallback(this);
    UpdateSelectorState();

    return NOERROR;
}

ECode AbsListView::GetSelector(
    /* [out] */ IDrawable** selector)
{
    VALIDATE_NOT_NULL(selector);
    *selector = mSelector;
    REFCOUNT_ADD(*selector);

    return NOERROR;
}

void AbsListView::KeyPressed()
{
    Boolean res, res1;
    if ((IsEnabled(&res), !res) || (IsClickable(&res1), !res1)) {
        return;
    }

    AutoPtr<IDrawable> selector = mSelector;
    AutoPtr<IRect> selectorRect = mSelectorRect;
    Boolean rectEmpty;
    selectorRect->IsEmpty(&rectEmpty);
    if (selector != NULL && ((IsFocused(&res), res) || TouchModeDrawsInPressedState())
            && !rectEmpty) {
        AutoPtr<IView> v;
        GetChildAt(mSelectedPosition - mFirstPosition, (IView**)&v);
        if (v != NULL) {
            Boolean hasfoucsable;
            v->HasFocusable(&hasfoucsable);
            if (hasfoucsable) {
                return;
            }

            v->SetPressed(TRUE);
        }

        SetPressed(TRUE);

        Boolean longClickable;
        IsLongClickable(&longClickable);
        AutoPtr<IDrawable> d;
        selector->GetCurrent((IDrawable**)&d);
        AutoPtr<ITransitionDrawable> td = ITransitionDrawable::Probe(d);
        Int32 timeout = ViewConfiguration::GetLongPressTimeout();

        if (td != NULL) {
            if (longClickable) {
                td->StartTransition(timeout);
            }
            else {
                td->ResetTransition();
            }
        }

        if (longClickable && !mDataChanged) {
            if (mPendingCheckForKeyLongPress == NULL) {
                mPendingCheckForKeyLongPress = new CheckForKeyLongPress(this);
            }

            mPendingCheckForKeyLongPress->RememberWindowAttachCount();
            PostDelayed(mPendingCheckForKeyLongPress,timeout, &res);
        }
    }
}

ECode AbsListView::SetScrollIndicators(
    /* [in] */ IView* up,
    /* [in] */ IView* down)
{
    mScrollUp = up;
    mScrollDown = down;

    return NOERROR;
}

void AbsListView::UpdateSelectorState()
{
    if (mSelector != NULL) {
        Boolean result;
        if (ShouldShowSelector()) {
            AutoPtr<ArrayOf<Int32> > drawableState;
            GetDrawableState((ArrayOf<Int32>**)&drawableState);
            mSelector->SetState(drawableState, &result);
        }
        else {
            mSelector->SetState(StateSet::NOTHING, &result);
        }
    }
}

ECode AbsListView::DrawableStateChanged()
{
    FAIL_RETURN(AdapterView::DrawableStateChanged());
    UpdateSelectorState();

    return NOERROR;
}

ECode AbsListView::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState);
    *drawableState = NULL;

    AutoPtr<ArrayOf<Int32> > state;

    // If the child view is enabled then do the default behavior.
    if (mIsChildViewEnabled) {
        // Common case
        FAIL_RETURN(AdapterView::OnCreateDrawableState(extraSpace, (ArrayOf<Int32>**)&state));
        *drawableState = state;
        REFCOUNT_ADD(*drawableState);
        return NOERROR;
    }

    // The selector uses this View's drawable state. The selected child view
    // is disabled, so we need to remove the enabled state from the drawable
    // states.
    Int32 enabledState = (*ENABLED_STATE_SET)[0];

    // If we don't have any extra space, it will return one of the static state arrays,
    // and clearing the enabled state on those arrays is a bad thing!  If we specify
    // we need extra space, it will create+copy into a new array that safely mutable.
    //
    state = NULL;
    AdapterView::OnCreateDrawableState(extraSpace + 1, (ArrayOf<Int32>**)&state);
    Int32 enabledPos = -1;
    if (state != NULL) {
        for (Int32 i = state->GetLength() - 1; i >= 0; i--) {
            if ((*state)[i] == enabledState) {
                enabledPos = i;
                break;
            }
        }
    }

    // Remove the enabled state
    if (enabledPos >= 0) {
        //System.arraycopy(state, enabledPos + 1, state, enabledPos,
        //        state.length - enabledPos - 1);
        state->Copy(enabledPos, state, enabledPos + 1, state->GetLength() - enabledPos - 1);
    }

    *drawableState = state;
    REFCOUNT_ADD(*drawableState);
    return NOERROR;
}

Boolean AbsListView::VerifyDrawable(
    /* [in] */ IDrawable* dr)
{
    return mSelector.Get() == dr || AdapterView::VerifyDrawable(dr);
}

ECode AbsListView::JumpDrawablesToCurrentState()
{
    FAIL_RETURN(AdapterView::JumpDrawablesToCurrentState());
    if (mSelector != NULL) {
        mSelector->JumpToCurrentState();
    }

    return NOERROR;
}

ECode AbsListView::OnAttachedToWindow()
{
    AdapterView::OnAttachedToWindow();

    AutoPtr<IViewTreeObserver> treeObserver;
    GetViewTreeObserver((IViewTreeObserver**)&treeObserver);
    treeObserver->AddOnTouchModeChangeListener(this);
    if (mTextFilterEnabled && mPopup != NULL && !mGlobalLayoutListenerAddedFilter) {
        treeObserver->AddOnGlobalLayoutListener(this);
    }

    if (mAdapter != NULL && mDataSetObserver == NULL) {
        mDataSetObserver = new AdapterDataSetObserver(this);
        IAdapter::Probe(mAdapter)->RegisterDataSetObserver(mDataSetObserver);

        // Data may have changed while we were detached. Refresh.
        mDataChanged = TRUE;
        mOldItemCount = mItemCount;
        IAdapter::Probe(mAdapter)->GetCount(&mItemCount);
    }
    return NOERROR;
}

ECode AbsListView::OnDetachedFromWindow()
{
    AdapterView::OnDetachedFromWindow();

    mIsDetaching = TRUE;

    // Dismiss the popup in case onSaveInstanceState() was not invoked
    DismissPopup();

    // Detach any view left in the scrap heap
    mRecycler->Clear();

    AutoPtr<IViewTreeObserver> treeObserver;
    GetViewTreeObserver((IViewTreeObserver**)&treeObserver);
    treeObserver->RemoveOnTouchModeChangeListener(this);
    if (mTextFilterEnabled && mPopup != NULL) {
        treeObserver->RemoveGlobalOnLayoutListener(this);
        mGlobalLayoutListenerAddedFilter = FALSE;
    }

    if (mAdapter != NULL && mDataSetObserver != NULL) {
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(mDataSetObserver);
        mDataSetObserver = NULL;
    }

    if (mScrollStrictSpan != NULL) {
        mScrollStrictSpan->Finish();
        mScrollStrictSpan = NULL;
    }

    if (mFlingStrictSpan != NULL) {
        mFlingStrictSpan->Finish();
        mFlingStrictSpan = NULL;
    }

    Boolean res;
    if (mFlingRunnable != NULL) {
        RemoveCallbacks(mFlingRunnable, &res);
    }

    if (mPositionScroller != NULL) {
        mPositionScroller->Stop();
    }

    if (mClearScrollingCache != NULL) {
        RemoveCallbacks(mClearScrollingCache, &res);
    }

    if (mPerformClick != NULL) {
        RemoveCallbacks(mPerformClick, &res);
    }

    if (mTouchModeReset != NULL) {
        RemoveCallbacks(mTouchModeReset, &res);
        mTouchModeReset->Run();
    }

    mIsDetaching = FALSE;
    return NOERROR;
}

ECode AbsListView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    FAIL_RETURN(AdapterView::OnWindowFocusChanged(hasWindowFocus));

    Boolean res;
    Int32 touchMode = (IsInTouchMode(&res), res) ? TOUCH_MODE_ON : TOUCH_MODE_OFF;

    if (!hasWindowFocus) {
        SetChildrenDrawingCacheEnabled(FALSE);
        if (mFlingRunnable != NULL) {
            RemoveCallbacks(mFlingRunnable, &res);
            // let the fling runnable report it's new state which
            // should be idle
            mFlingRunnable->EndFling();

            if (mPositionScroller != NULL) {
                mPositionScroller->Stop();
            }

            if (mScrollY != 0) {
                mScrollY = 0;

                InvalidateParentCaches();

                FinishGlows();
                Invalidate();
            }
        }
        // Always hide the type filter
        DismissPopup();

        if (touchMode == TOUCH_MODE_OFF) {
            // Remember the last selected element
            mResurrectToPosition = mSelectedPosition;
        }
    }
    else {
        if (mFiltered && !mPopupHidden) {
            // Show the type filter only if a filter is in effect
            ShowPopup();
        }

        // If we changed touch mode since the last time we had focus
        if (touchMode != mLastTouchMode && mLastTouchMode != TOUCH_MODE_UNKNOWN) {
            // If we come back in trackball mode, we bring the selection back
            if (touchMode == TOUCH_MODE_OFF) {
                // This will trigger a layout
                ResurrectSelection();

            // If we come back in touch mode, then we want to hide the selector
            }
            else {
                HideSelector();
                mLayoutMode = LAYOUT_NORMAL;
                LayoutChildren();
            }
        }
    }

    mLastTouchMode = touchMode;

    return NOERROR;
}

ECode AbsListView::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    FAIL_RETURN(AdapterView::OnRtlPropertiesChanged(layoutDirection));
    if (mFastScroll != NULL) {
        Int32 position;
        GetVerticalScrollbarPosition(&position);
       mFastScroll->SetScrollbarPosition(position);
    }
    return NOERROR;
}

AutoPtr<IContextMenuInfo> AbsListView::CreateContextMenuInfo(
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return new AdapterView::AdapterContextMenuInfo(view, position, id);
}

ECode AbsListView::OnCancelPendingInputEvents()
{
    FAIL_RETURN(AdapterView::OnCancelPendingInputEvents());
    Boolean res;
    if (mPerformClick != NULL) {
        RemoveCallbacks(mPerformClick, &res);
    }
    if (mPendingCheckForTap != NULL) {
        RemoveCallbacks(mPendingCheckForTap, &res);
    }
    if (mPendingCheckForLongPress != NULL) {
        RemoveCallbacks(mPendingCheckForLongPress, &res);
    }
    if (mPendingCheckForKeyLongPress != NULL) {
        RemoveCallbacks(mPendingCheckForKeyLongPress, &res);
    }
    return NOERROR;
}

Boolean AbsListView::PerformLongPress(
    /* [in] */ IView* child,
    /* [in] */ Int32 longPressPosition,
    /* [in] */ Int64 longPressId)
{
    // CHOICE_MODE_MULTIPLE_MODAL takes over long press.
    Boolean res;
    if (mChoiceMode == IAbsListView::CHOICE_MODE_MULTIPLE_MODAL) {
        if (mChoiceActionMode == NULL &&
                (StartActionMode(
                    IActionModeCallback::Probe(mMultiChoiceModeCallback),
                    (IActionMode**)&mChoiceActionMode), mChoiceActionMode) != NULL) {
            SetItemChecked(longPressPosition, TRUE);
            PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &res);
        }
        return TRUE;
    }

    Boolean handled = FALSE;
    if (mOnItemLongClickListener != NULL) {
        mOnItemLongClickListener->OnItemLongClick(this,
                child, longPressPosition, longPressId, &handled);
    }
    if (!handled) {
        mContextMenuInfo = CreateContextMenuInfo(child, longPressPosition, longPressId);
        AdapterView::ShowContextMenuForChild(this, &handled);
    }

    if (handled) {
        PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &res);
    }

    return handled;
}

AutoPtr<IContextMenuInfo> AbsListView::GetContextMenuInfo()
{
    return mContextMenuInfo;
}

ECode AbsListView::ShowContextMenu(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    return View::ShowContextMenu(res);
}

ECode AbsListView::ShowContextMenu(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 metaState,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 position;
    PointToPosition((Int32)x, (Int32)y, &position);
    if (position != IAdapterView::INVALID_POSITION) {
        Int64 id;
        IAdapter::Probe(mAdapter)->GetItemId(position, &id);
        AutoPtr<IView> child;
        GetChildAt(position - mFirstPosition, (IView**)&child);
        if (child != NULL) {
            mContextMenuInfo = CreateContextMenuInfo(child, position, id);
            return AdapterView::ShowContextMenuForChild(this, res);
        }
    }

    return AdapterView::ShowContextMenu(x, y, metaState, res);
}

ECode AbsListView::ShowContextMenuForChild(
    /* [in] */ IView* originalView,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 longPressPosition;
    GetPositionForView(originalView, &longPressPosition);
    if (longPressPosition >= 0) {
        Int64 longPressId;
        IAdapter::Probe(mAdapter)->GetItemId(longPressPosition, &longPressId);

        Boolean handled = FALSE;
        if (mOnItemLongClickListener != NULL) {
            mOnItemLongClickListener->OnItemLongClick(this,
                    originalView, longPressPosition, longPressId, &handled);
        }

        if (!handled) {
            AutoPtr<IView> child;
            GetChildAt(longPressPosition - mFirstPosition, (IView**)&child);

            mContextMenuInfo = CreateContextMenuInfo(
                    child, longPressPosition, longPressId);

            AdapterView::ShowContextMenuForChild(originalView, &handled);
        }
        *res = handled;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

ECode AbsListView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode AbsListView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean suc, suc1;
    if (KeyEvent::IsConfirmKey(keyCode)) {
        if (IsEnabled(&suc), !suc) {
            *res = TRUE;
            return NOERROR;
        }

        Int32 count = -1;
        if (mAdapter != NULL) {
            IAdapter::Probe(mAdapter)->GetCount(&count);
        }

        if ((IsClickable(&suc), suc)
                && (IsPressed(&suc1), suc1)
                && mSelectedPosition >= 0
                && mSelectedPosition < count) {
            AutoPtr<IView> view;
            GetChildAt(mSelectedPosition - mFirstPosition, (IView**)&view);
            if (view != NULL) {
                PerformItemClick(
                        view, mSelectedPosition, mSelectedRowId, &suc);
                view->SetPressed(FALSE);
            }
            SetPressed(FALSE);
            *res = TRUE;
            return NOERROR;
        }
    }
    return AdapterView::OnKeyUp(keyCode, event, res);
}

void AbsListView::DispatchSetPressed(
    /* [in] */ Boolean pressed)
{
    // Don't dispatch setPressed to our children. We call setPressed on ourselves to
    // get the selector in the right state, but we don't want to press each child.
}

ECode AbsListView::PointToPosition(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);

    AutoPtr<IRect> frame = mTouchFrame;
    if (frame == NULL) {
        mTouchFrame = NULL;
        FAIL_RETURN(CRect::New((IRect**)&mTouchFrame));
        frame = mTouchFrame;
    }

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = count - 1; i >= 0; i--) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility == IView::VISIBLE) {
            child->GetHitRect(frame);
            Boolean isContains;
            frame->Contains(x, y, &isContains);
            if (isContains) {
                *position = mFirstPosition + i;
                return NOERROR;
            }
        }
    }

    *position = IAdapterView::INVALID_POSITION;
    return NOERROR;
}

ECode AbsListView::PointToRowId(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Int64* rowId)
{
    VALIDATE_NOT_NULL(rowId);

    Int32 position;
    PointToPosition(x, y, &position);
    Int64 itemId = IAdapterView::INVALID_ROW_ID;

    if (position >= 0) {
        IAdapter::Probe(mAdapter)->GetItemId(position, &itemId);
    }

    *rowId = itemId;
    return NOERROR;
}

Boolean AbsListView::StartScrollIfNeeded(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IMotionEvent* vtev)
{
    // Check if we have moved far enough that it looks more like a
    // scroll than a tap
    Int32 deltaY = y - mMotionY;
    Int32 distance = Elastos::Core::Math::Abs(deltaY);
    Boolean overscroll = mScrollY != 0;

    Int32 axes;
    GetNestedScrollAxes(&axes);
    if ((overscroll || distance > mTouchSlop) &&
            (axes & SCROLL_AXIS_VERTICAL) == 0) {
        CreateScrollingCache();
        if (overscroll) {
            mTouchMode = TOUCH_MODE_OVERSCROLL;
            mMotionCorrection = 0;
        }
        else {
            mTouchMode = TOUCH_MODE_SCROLL;
            mMotionCorrection = deltaY > 0 ? mTouchSlop : -mTouchSlop;
        }

        Boolean res;
        RemoveCallbacks(mPendingCheckForLongPress, &res);

        SetPressed(FALSE);
        AutoPtr<IView> motionView;
        GetChildAt(mMotionPosition - mFirstPosition, (IView**)&motionView);
        if (motionView != NULL) {
            motionView->SetPressed(FALSE);
        }

        ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL);
        // Time to start stealing events! Once we've stolen them, don't let anyone
        // steal from us

        AutoPtr<IViewParent> parent;
        GetParent((IViewParent**)&parent);
        if (parent != NULL) {
            parent->RequestDisallowInterceptTouchEvent(TRUE);
        }
        ScrollIfNeeded(x, y, vtev);

        return TRUE;
    }

    return FALSE;
}

void AbsListView::ScrollIfNeeded(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IMotionEvent* vtev)
{
    using Elastos::Core::Math;
    Int32 rawDeltaY = y - mMotionY;
    Int32 scrollOffsetCorrection = 0;
    Int32 scrollConsumedCorrection = 0;
    if (mLastY == Math::INT32_MIN_VALUE) {
        rawDeltaY -= mMotionCorrection;
    }

    Boolean res;
    DispatchNestedPreScroll(0, mLastY != Math::INT32_MIN_VALUE ? mLastY - y : -rawDeltaY,
            mScrollConsumed, mScrollOffset, &res);
    if (res) {
        rawDeltaY += (*mScrollConsumed)[1];
        scrollOffsetCorrection = -(*mScrollOffset)[1];
        scrollConsumedCorrection = (*mScrollConsumed)[1];
        if (vtev != NULL) {
            vtev->OffsetLocation(0, (*mScrollOffset)[1]);
            mNestedYOffset += (*mScrollOffset)[1];
        }
    }
    const Int32 deltaY = rawDeltaY;
    Int32 incrementalDeltaY =
            mLastY != Math::INT32_MIN_VALUE ? y - mLastY + scrollConsumedCorrection : deltaY;
    Int32 lastYCorrection = 0;

    Boolean temp;
    if (mTouchMode == TOUCH_MODE_SCROLL) {
        if (PROFILE_SCROLLING) {
            if (!mScrollProfilingStarted) {
//                Debug.startMethodTracing("AbsListViewScroll");
                mScrollProfilingStarted = TRUE;
            }
        }

        if (mScrollStrictSpan == NULL) {
            // If it's non-null, we're already in a scroll.
            AutoPtr<IStrictMode> helper;
            // CStrictMode::AcquireSingleton((IStrictMode**)&helper);
            helper->EnterCriticalSpan(String("AbsListView-scroll"),
                    (IStrictModeSpan**)&mScrollStrictSpan);
        }

        if (y != mLastY) {
            // We may be here after stopping a fling and continuing to scroll.
            // If so, we haven't disallowed intercepting touch events yet.
            // Make sure that we do so in case we're in a parent that can intercept.
            if ((mGroupFlags & FLAG_DISALLOW_INTERCEPT) == 0
                && Math::Abs(rawDeltaY) > mTouchSlop) {
                AutoPtr<IViewParent> parent;
                GetParent((IViewParent**)&parent);
                if (parent != NULL) {
                    parent->RequestDisallowInterceptTouchEvent(TRUE);
                }
            }

            Int32 motionIndex;
            if (mMotionPosition >= 0) {
                motionIndex = mMotionPosition - mFirstPosition;
            } else {
                // If we don't have a motion position that we can reliably track,
                // pick something in the middle to make a best guess at things below.
                Int32 count;
                GetChildCount(&count);
                motionIndex = count / 2;
            }

            AutoPtr<IView> motionView;
            GetChildAt(motionIndex, (IView**)&motionView);
            Int32 motionViewPrevTop = 0;
            if (motionView != NULL) {
                motionView->GetTop(&motionViewPrevTop);
            }

            // No need to do all this work if we're not going to move anyway
            Boolean atEdge = FALSE;
            if (incrementalDeltaY != 0) {
                atEdge = TrackMotionScroll(deltaY, incrementalDeltaY);
            }

            // Check to see if we have bumped into the scroll limit
            motionView = NULL;
            GetChildAt(motionIndex, (IView**)&motionView);
            if (motionView != NULL) {
                // Check if the top of the motion view is where it is
                // supposed to be
                Int32 motionViewRealTop;
                motionView->GetTop(&motionViewRealTop);
                if (atEdge) {
                    // Apply overscroll

                    Int32 overscroll = -incrementalDeltaY -
                            (motionViewRealTop - motionViewPrevTop);
                    Boolean res;
                    if (DispatchNestedScroll(0, overscroll - incrementalDeltaY, 0, overscroll,
                            mScrollOffset, &res), res) {
                        lastYCorrection -= (*mScrollOffset)[1];
                        if (vtev != NULL) {
                            vtev->OffsetLocation(0, (*mScrollOffset)[1]);
                            mNestedYOffset += (*mScrollOffset)[1];
                        }
                    }
                    else {
                        Boolean atOverscrollEdge = OverScrollBy(0, overscroll,
                                0, mScrollY, 0, 0, 0, mOverscrollDistance, TRUE);

                        if (atOverscrollEdge && mVelocityTracker != NULL) {
                            // Don't allow overfling if we're at the edge
                            mVelocityTracker->Clear();
                        }

                        Int32 overscrollMode;
                        GetOverScrollMode(&overscrollMode);
                        Int32 height, width;
                        GetHeight(&height);
                        GetWidth(&width);
                        if (overscrollMode == IView::OVER_SCROLL_ALWAYS ||
                            (overscrollMode == IView::OVER_SCROLL_IF_CONTENT_SCROLLS
                                && !ContentFits())) {
                            if (!atOverscrollEdge) {
                                mDirection = 0; // Reset when entering overscroll.
                                mTouchMode = TOUCH_MODE_OVERSCROLL;
                            }
                            if (incrementalDeltaY > 0) {
                                mEdgeGlowTop->OnPull((Float) -overscroll / height,
                                    (Float) x / width);
                                mEdgeGlowBottom->IsFinished(&temp);
                                if (!temp) {
                                    mEdgeGlowBottom->OnRelease();
                                }
                                Int32 top;
                                GetPaddingTop(&top);
                                Int32 maxH;
                                mEdgeGlowTop->GetMaxHeight(&maxH);
                                Invalidate(0, 0, width, maxH + top);
                            }
                            else if (incrementalDeltaY < 0) {
                                mEdgeGlowBottom->OnPull((Float) overscroll / height,
                                        1.f - (Float) x / width);
                                mEdgeGlowTop->IsFinished(&temp);
                                if (!temp) {
                                    mEdgeGlowTop->OnRelease();
                                }
                                Int32 bottom;
                                GetPaddingBottom(&bottom);
                                Int32 maxH;
                                mEdgeGlowBottom->GetMaxHeight(&maxH);
                                Invalidate(0, height - bottom - maxH, width, height);
                            }
                        }
                    }
                }
                mMotionY = y + lastYCorrection + scrollOffsetCorrection;
            }
            mLastY = y + lastYCorrection + scrollOffsetCorrection;
        }
    }
    else if (mTouchMode == TOUCH_MODE_OVERSCROLL) {
        if (y != mLastY) {
            Int32 oldScroll = mScrollY;
            Int32 newScroll = oldScroll - incrementalDeltaY;
            Int32 newDirection = y > mLastY ? 1 : -1;

            if (mDirection == 0) {
                mDirection = newDirection;
            }

            Int32 overScrollDistance = -incrementalDeltaY;
            if ((newScroll < 0 && oldScroll >= 0) || (newScroll > 0 && oldScroll <= 0)) {
                overScrollDistance = -oldScroll;
                incrementalDeltaY += overScrollDistance;
            }
            else {
                incrementalDeltaY = 0;
            }

            if (overScrollDistance != 0) {
                OverScrollBy(0, overScrollDistance, 0, mScrollY, 0, 0,
                    0, mOverscrollDistance, TRUE);
                Int32 overscrollMode;
                GetOverScrollMode(&overscrollMode);
                Int32 height, width;
                GetHeight(&height);
                GetWidth(&width);
                if (overscrollMode == IView::OVER_SCROLL_ALWAYS ||
                    (overscrollMode == IView::OVER_SCROLL_IF_CONTENT_SCROLLS &&
                    !ContentFits())) {
                        if (rawDeltaY > 0) {
                            mEdgeGlowTop->OnPull((Float) overScrollDistance / height,
                                    (Float) x / width);
                            mEdgeGlowBottom->IsFinished(&temp);
                            if (!temp) {
                                mEdgeGlowBottom->OnRelease();
                            }
                            Int32 top;
                            GetPaddingTop(&top);
                            Int32 maxH;
                            mEdgeGlowTop->GetMaxHeight(&maxH);
                            Invalidate(0, 0, width, maxH + top);
                        }
                        else if (rawDeltaY < 0) {
                            mEdgeGlowBottom->OnPull((Float) overScrollDistance / height,
                                    1.f - (Float) x / width);
                            mEdgeGlowTop->IsFinished(&temp);
                            if (!temp) {
                                mEdgeGlowTop->OnRelease();
                            }
                            Int32 bottom;
                            GetPaddingBottom(&bottom);
                            Int32 maxH;
                            mEdgeGlowBottom->GetMaxHeight(&maxH);
                            Invalidate(0, height - bottom - maxH, width, height);
                        }
                }
            }

            if (incrementalDeltaY != 0) {
                // Coming back to 'real' list scrolling
                if (mScrollY != 0) {
                    mScrollY = 0;
                    InvalidateParentIfNeeded();
                }

                TrackMotionScroll(incrementalDeltaY, incrementalDeltaY);

                mTouchMode = TOUCH_MODE_SCROLL;

                // We did not scroll the full amount. Treat this essentially like the
                // start of a new touch scroll
                Int32 motionPosition = FindClosestMotionRow(y);

                mMotionCorrection = 0;
                AutoPtr<IView> motionView;
                GetChildAt(motionPosition - mFirstPosition, (IView**)&motionView);
                Int32 viewTop;
                motionView->GetTop(&viewTop);
                mMotionViewOriginalTop = motionView != NULL ? viewTop : 0;
                mMotionY = y + scrollOffsetCorrection;
                mMotionPosition = motionPosition;
            }
            mLastY = y + lastYCorrection + scrollOffsetCorrection;
            mDirection = newDirection;
        }
    }
}

ECode AbsListView::OnTouchModeChanged(
    /* [in] */ Boolean isInTouchMode)
{
    if (isInTouchMode) {
        // Get rid of the selection when we enter touch mode
        HideSelector();
        // Layout, but only if we already have done so previously.
        // (Otherwise may clobber a LAYOUT_SYNC layout that was requested to restore
        // state.)
        Int32 height;
        GetHeight(&height);
        Int32 count;
        GetChildCount(&count);
        if (height > 0 && count > 0) {
            // We do not lose focus initiating a touch (since AbsListView is focusable in
            // touch mode). Force an initial layout to get rid of the selection.
            LayoutChildren();
        }
        UpdateSelectorState();
    }
    else {
        Int32 touchMode = mTouchMode;
        if (touchMode == TOUCH_MODE_OVERSCROLL || touchMode == TOUCH_MODE_OVERFLING) {
            if (mFlingRunnable != NULL) {
                mFlingRunnable->EndFling();
            }
            if (mPositionScroller != NULL) {
                mPositionScroller->Stop();
            }

            if (mScrollY != 0) {
                mScrollY = 0;
                InvalidateParentCaches();
                FinishGlows();
                Invalidate();
            }
        }
    }

    return NOERROR;
}

ECode AbsListView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean suc;
    if (IsEnabled(&suc), !suc) {
        // A disabled view that is clickable still consumes the touch
        // events, it just doesn't respond to them.
        Boolean res1, res2;
        *res = (IsClickable(&res1), res1) || (IsLongClickable(&res2), res2);
        return NOERROR;
    }

    if (mPositionScroller != NULL) {
        mPositionScroller->Stop();
    }

    if (mIsDetaching || (IsAttachedToWindow(&suc), !suc)) {
        // Something isn't right.
        // Since we rely on being attached to get data set change notifications,
        // don't risk doing anything where we might try to resync and find things
        // in a bogus state.
        *res = FALSE;
        return NOERROR;
    }

    StartNestedScroll(IView::SCROLL_AXIS_VERTICAL, &suc);

    if (mFastScroll != NULL) {
        Boolean intercepted = mFastScroll->OnTouchEvent(ev);
        if (intercepted) {
            *res = TRUE;
            return NOERROR;
        }
    }

    InitVelocityTrackerIfNotExists();
    AutoPtr<IMotionEvent> vtev;
    MotionEvent::Obtain(ev, (IMotionEvent**)&vtev);

    Int32 actionMasked;
    ev->GetActionMasked(&actionMasked);
    if (actionMasked == IMotionEvent::ACTION_DOWN) {
        mNestedYOffset = 0;
    }
    vtev->OffsetLocation(0, mNestedYOffset);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            OnTouchDown(ev);
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            OnTouchMove(ev, vtev);
            break;
        }

        case IMotionEvent::ACTION_UP: {
            OnTouchUp(ev);
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            OnTouchCancel();
            break;
        }

        case IMotionEvent::ACTION_POINTER_UP: {
            OnSecondaryPointerUp(ev);
            Int32 x = mMotionX;
            Int32 y = mMotionY;
            Int32 motionPosition;
            PointToPosition(x, y, &motionPosition);
            if (motionPosition >= 0) {
                // Remember where the motion event started
                AutoPtr<IView> child;
                GetChildAt(motionPosition - mFirstPosition, (IView**)&child);
                child->GetTop(&mMotionViewOriginalTop);
                mMotionPosition = motionPosition;
            }
            mLastY = y;
            break;
        }

        case IMotionEvent::ACTION_POINTER_DOWN: {
            // New pointers take over dragging duties
            Int32 index;
            ev->GetActionIndex(&index);
            Int32 id;
            ev->GetPointerId(index, &id);

            Float fx, fy;
            ev->GetX(index, &fx);
            ev->GetY(index, &fy);
            Int32 x = (Int32)fx;
            Int32 y = (Int32)fy;
            mMotionCorrection = 0;
            mActivePointerId = id;
            mMotionX = x;
            mMotionY = y;
            Int32 motionPosition;
            PointToPosition(x, y, &motionPosition);
            if (motionPosition >= 0) {
                // Remember where the motion event started
                AutoPtr<IView> child;
                GetChildAt(motionPosition - mFirstPosition, (IView**)&child);
                child->GetTop(&mMotionViewOriginalTop);
                mMotionPosition = motionPosition;
            }
            mLastY = y;
            break;
        }
        default:
            break;
    }

    if (mVelocityTracker != NULL) {
        mVelocityTracker->AddMovement(vtev);
    }
    IInputEvent::Probe(vtev)->Recycle();
    return TRUE;
}

void AbsListView::OnTouchDown(
    /* [in] */ IMotionEvent* ev)
{
    ev->GetPointerId(0, &mActivePointerId);
    Float fx, fy;
    ev->GetX(&fx);
    ev->GetY(&fy);

    Boolean res;

    if (mTouchMode == TOUCH_MODE_OVERFLING) {
        // Stopped the fling. It is a scroll.
        mFlingRunnable->EndFling();
        if (mPositionScroller != NULL) {
            mPositionScroller->Stop();
        }
        mTouchMode = TOUCH_MODE_OVERSCROLL;
        mMotionX = (Int32)fx;
        mMotionY = (Int32)fy;
        mLastY = mMotionY;
        mMotionCorrection = 0;
        mDirection = 0;
    }
    else {
        Int32 x = (Int32)fx;
        Int32 y = (Int32)fy;
        Int32 motionPosition;
        PointToPosition(x, y, &motionPosition);
        if (!mDataChanged) {
            AutoPtr<IAdapter> adapter;
            GetAdapter((IAdapter**)&adapter);
            Boolean isEnabled;
            IListAdapter::Probe(adapter)->IsEnabled(motionPosition, &isEnabled);
            if (mTouchMode == TOUCH_MODE_FLING) {
                // Stopped a fling. It is a scroll.
                CreateScrollingCache();
                mTouchMode = TOUCH_MODE_SCROLL;
                mMotionCorrection = 0;
                motionPosition = FindMotionRow(y);
                mFlingRunnable->FlywheelTouch();
            }
            else if ((motionPosition >= 0) && isEnabled) {
                // User clicked on an actual view (and was not stopping a
                // fling). It might be a click or a scroll. Assume it is a
                // click until proven otherwise.
                mTouchMode = TOUCH_MODE_DOWN;

                // FIXME Debounce
                if (mPendingCheckForTap == NULL) {
                    mPendingCheckForTap = new CheckForTap(this);
                }

                mPendingCheckForTap->mX = fx;
                mPendingCheckForTap->mY = fy;
                PostDelayed(mPendingCheckForTap, ViewConfiguration::GetTapTimeout(), &res);
            }
        }
        if (motionPosition >= 0) {
            // Remember where the motion event started
            AutoPtr<IView> v;
            GetChildAt(motionPosition - mFirstPosition, (IView**)&v);
            v->GetTop(&mMotionViewOriginalTop);
        }
        mMotionX = x;
        mMotionY = y;
        mMotionPosition = motionPosition;
        mLastY = Elastos::Core::Math::INT32_MIN_VALUE;
    }

    if (mTouchMode == TOUCH_MODE_DOWN && mMotionPosition != IAdapterView::INVALID_POSITION
            && PerformButtonActionOnTouchDown(ev)) {
        RemoveCallbacks(mPendingCheckForTap, &res);
    }
}

void AbsListView::OnTouchMove(
    /* [in] */ IMotionEvent* ev,
    /* [in] */ IMotionEvent* vtev)
{
    Int32 pointerIndex;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    if (pointerIndex == -1) {
        pointerIndex = 0;
        ev->GetPointerId(pointerIndex, &mActivePointerId);
    }

    if (mDataChanged) {
        // Re-sync everything if data has been changed
        // since the scroll operation can query the adapter.
        LayoutChildren();
    }

    Float pify;
    ev->GetY(pointerIndex, &pify);
    Int32 y = (Int32)pify;
    Float pifx;
    ev->GetX(pointerIndex, &pifx);
    Int32 x = (Int32)pifx;

    switch (mTouchMode) {
        case TOUCH_MODE_DOWN:
        case TOUCH_MODE_TAP:
        case TOUCH_MODE_DONE_WAITING: {
            // Check if we have moved far enough that it looks more like a
            // scroll than a tap. If so, we'll enter scrolling mode.
            if (StartScrollIfNeeded(x, y, vtev)) {
                break;
            }
            // Otherwise, check containment within list bounds. If we're
            // outside bounds, cancel any active presses.
            Boolean res;
            if (PointInView(pifx, y, mTouchSlop, &res), !res) {
                SetPressed(FALSE);
                AutoPtr<IView> motionView;
                GetChildAt(mMotionPosition - mFirstPosition, (IView**)&motionView);
                if (motionView != NULL) {
                    motionView->SetPressed(FALSE);
                }
                Boolean res;
                RemoveCallbacks(mTouchMode == TOUCH_MODE_DOWN ?
                        (IRunnable*)mPendingCheckForTap.Get() : (IRunnable*)mPendingCheckForLongPress.Get(), &res);
                mTouchMode = TOUCH_MODE_DONE_WAITING;
                UpdateSelectorState();
            }
            break;
        }

        case TOUCH_MODE_SCROLL:
        case TOUCH_MODE_OVERSCROLL:
            ScrollIfNeeded(x, y, vtev);
            break;
    }
}

void AbsListView::OnTouchUp(
    /* [in] */ IMotionEvent* ev)
{
    switch (mTouchMode) {
        case TOUCH_MODE_DOWN:
        case TOUCH_MODE_TAP:
        case TOUCH_MODE_DONE_WAITING: {
            Int32 motionPosition = mMotionPosition;
            AutoPtr<IView> child;
            GetChildAt(motionPosition - mFirstPosition, (IView**)&child);
            if (child != NULL) {
                if (mTouchMode != TOUCH_MODE_DOWN) {
                    child->SetPressed(FALSE);
                }
                Float x, y;
                ev->GetX(&x);
                ev->GetY(&y);
                Int32 width;
                GetWidth(&width);

                Int32 leftTmp = 0, rightTmp = 0;
                mListPadding->GetLeft(&leftTmp);
                mListPadding->GetRight(&rightTmp);
                Boolean inList = x > leftTmp && x < width - rightTmp;
                Boolean hasFocusable;
                if (inList && (child->HasFocusable(&hasFocusable), !hasFocusable)) {
                    if (mPerformClick == NULL) {
                        mPerformClick = new PerformClick(this);
                    }

                    AutoPtr<PerformClick> performClick = mPerformClick;

                    performClick->mClickMotionPosition = motionPosition;
                    performClick->RememberWindowAttachCount();

                    mResurrectToPosition = motionPosition;

                    Boolean isEnabled;
                    mAdapter->IsEnabled(motionPosition, &isEnabled);
                    if (mTouchMode == TOUCH_MODE_DOWN || mTouchMode == TOUCH_MODE_TAP) {
                        AutoPtr<IRunnable> target = mTouchMode == TOUCH_MODE_DOWN ?
                                (IRunnable*)mPendingCheckForTap.Get() : (IRunnable*)mPendingCheckForLongPress.Get();
                        Boolean res;
                        RemoveCallbacks(target, &res);

                        mLayoutMode = LAYOUT_NORMAL;
                        if (!mDataChanged && isEnabled) {
                            mTouchMode = TOUCH_MODE_TAP;
                            SetSelectedPositionInt(mMotionPosition);
                            LayoutChildren();
                            child->SetPressed(TRUE);
                            PositionSelector(mMotionPosition, child);
                            SetPressed(TRUE);
                            if (mSelector != NULL) {
                                AutoPtr<IDrawable> d;
                                mSelector->GetCurrent((IDrawable**)&d);
                                AutoPtr<ITransitionDrawable> td = ITransitionDrawable::Probe(d);
                                if (td != NULL) {
                                    td->ResetTransition();
                                }
                                mSelector->SetHotspot(x, y);
                            }
                            if (mTouchModeReset != NULL) {
                                RemoveCallbacks(mTouchModeReset, &res);
                            }
                            mTouchModeReset = new TouchModeResetRunnable(performClick, this, child);
                            Int32 timeout = ViewConfiguration::GetPressedStateDuration();
                            Boolean res;
                            PostDelayed(mTouchModeReset, timeout, &res);
                        }
                        else {
                            mTouchMode = TOUCH_MODE_REST;
                            UpdateSelectorState();
                        }
                        return;
                    }
                    else if (!mDataChanged && isEnabled) {
                        performClick->Run();
                    }
                }
            }
            mTouchMode = TOUCH_MODE_REST;
            UpdateSelectorState();
            break;
        }

        case TOUCH_MODE_SCROLL: {
            Int32 childCount;
            GetChildCount(&childCount);
            if (childCount > 0) {
                AutoPtr<IView> view;
                GetChildAt(0, (IView**)&view);
                Int32 firstChildTop;
                view->GetTop(&firstChildTop);
                view = NULL;
                GetChildAt(childCount - 1, (IView**)&view);
                Int32 lastChildBottom;
                view->GetBottom(&lastChildBottom);

                Int32 topTmp = 0, bottomTmp = 0;
                mListPadding->GetTop(&topTmp);
                mListPadding->GetBottom(&bottomTmp);
                Int32 contentTop = topTmp;
                Int32 height;
                GetHeight(&height);
                Int32 contentBottom = height - bottomTmp;
                if (mFirstPosition == 0 && firstChildTop >= contentTop &&
                        mFirstPosition + childCount < mItemCount &&
                        lastChildBottom <= height - contentBottom) {
                    mTouchMode = TOUCH_MODE_REST;
                    ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
                }
                else {
                    mVelocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
                    Float y;
                    mVelocityTracker->GetYVelocity(mActivePointerId, &y);
                    Int32 initialVelocity = (Int32)(y * mVelocityScale);

                    // Fling if we have enough velocity and we aren't at a boundary.
                    // Since we can potentially overfling more than we can overscroll, don't
                    // allow the weird behavior where you can scroll to a boundary then
                    // fling further.
                    Boolean flingVelocity = Elastos::Core::Math::Abs(initialVelocity) > mMinimumVelocity;
                    if (flingVelocity &&
                            !((mFirstPosition == 0 &&
                            firstChildTop == contentTop - mOverscrollDistance) ||
                            (mFirstPosition + childCount == mItemCount &&
                            lastChildBottom == contentBottom + mOverscrollDistance))) {
                        Boolean res;
                        if ((DispatchNestedPreFling(0, -initialVelocity, &res), !res)) {
                            if (mFlingRunnable == NULL) {
                                mFlingRunnable = new FlingRunnable(this);
                            }
                            ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_FLING);

                            mFlingRunnable->Start(-initialVelocity);
                            Boolean res;
                            DispatchNestedFling(0, -initialVelocity, TRUE, &res);
                        }
                        else {
                            mTouchMode = TOUCH_MODE_REST;
                            ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
                        }
                    }
                    else {
                        mTouchMode = TOUCH_MODE_REST;
                        ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);

                        if (mFlingRunnable != NULL) {
                            mFlingRunnable->EndFling();
                        }
                        if (mPositionScroller != NULL) {
                            mPositionScroller->Stop();
                        }
                        Boolean res;
                        if (flingVelocity && (DispatchNestedPreFling(0, -initialVelocity, &res), !res)) {
                            Boolean res;
                            DispatchNestedFling(0, -initialVelocity, FALSE, &res);
                        }
                    }
                }
            }
            else {
                mTouchMode = TOUCH_MODE_REST;
                ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
            }
            break;
        }

        case TOUCH_MODE_OVERSCROLL: {
            if (mFlingRunnable == NULL) {
                mFlingRunnable = new FlingRunnable(this);
            }

            mVelocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
            Float y;
            mVelocityTracker->GetYVelocity(mActivePointerId, &y);
            Int32 initialVelocity = (Int32)y;

            ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_FLING);
            if (Elastos::Core::Math::Abs(initialVelocity) > mMinimumVelocity) {
                mFlingRunnable->StartOverfling(-initialVelocity);
            }
            else {
                mFlingRunnable->StartSpringback();
            }

            break;
        }
        default:
            break;
    }

    SetPressed(FALSE);

    if (mEdgeGlowTop != NULL) {
        mEdgeGlowTop->OnRelease();
        mEdgeGlowBottom->OnRelease();
    }

    // Need to redraw since we probably aren't drawing the selector anymore
    Invalidate();

    Boolean res;
    RemoveCallbacks(mPendingCheckForLongPress, &res);

    RecycleVelocityTracker();

    mActivePointerId = INVALID_POINTER;

    if (PROFILE_SCROLLING) {
        if (mScrollProfilingStarted) {
            //Debug.stopMethodTracing();
            mScrollProfilingStarted = FALSE;
        }
    }

    if (mScrollStrictSpan != NULL) {
        mScrollStrictSpan->Finish();
        mScrollStrictSpan = NULL;
    }
}

void AbsListView::OnTouchCancel()
{
    switch (mTouchMode) {
        case TOUCH_MODE_OVERSCROLL:
            if (mFlingRunnable == NULL) {
                mFlingRunnable = new FlingRunnable(this);
            }
            mFlingRunnable->StartSpringback();
            break;

        case TOUCH_MODE_OVERFLING:
            // Do nothing - let it play out.
            break;

        default:
            mTouchMode = TOUCH_MODE_REST;
            SetPressed(FALSE);
            AutoPtr<IView> motionView;
            GetChildAt(mMotionPosition - mFirstPosition, (IView**)&motionView);
            if (motionView != NULL) {
                motionView->SetPressed(FALSE);
            }
            ClearScrollingCache();

            Boolean res;
            RemoveCallbacks(mPendingCheckForLongPress, &res);

            RecycleVelocityTracker();
            break;
    }

    if (mEdgeGlowTop != NULL) {
        mEdgeGlowTop->OnRelease();
        mEdgeGlowBottom->OnRelease();
    }
    mActivePointerId = INVALID_POINTER;
}

void AbsListView::OnOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    if (mScrollY != scrollY) {
        OnScrollChanged(mScrollX, scrollY, mScrollX, mScrollY);
        mScrollY = scrollY;
        InvalidateParentIfNeeded();

        AwakenScrollBars();
    }
}

ECode AbsListView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 source;
    IInputEvent::Probe(event)->GetSource(&source);
    if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        Int32 action;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_SCROLL: {
                if (mTouchMode == TOUCH_MODE_REST) {
                    Float vscroll;
                    event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &vscroll);
                    if (vscroll != 0) {
                        Float scrollFactor;
                        GetVerticalScrollFactor(&scrollFactor);
                        Int32 delta = (Int32) (vscroll * scrollFactor);
                        if (!TrackMotionScroll(delta, delta)) {
                            *res = TRUE;
                            return NOERROR;
                        }
                    }
                }
            }
        }
    }
    return AdapterView::OnGenericMotionEvent(event, res);
}

ECode AbsListView::Fling(
    /* [in] */ Int32 velocityY)
{
    if (mFlingRunnable == NULL) {
        mFlingRunnable = new FlingRunnable(this);
    }
    ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_FLING);
    mFlingRunnable->Start(velocityY);
    return NOERROR;
}

ECode AbsListView::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 nestedScrollAxes,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (nestedScrollAxes & IView::SCROLL_AXIS_VERTICAL) != 0;
    return NOERROR;
}

ECode AbsListView::OnNestedScrollAccepted(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 axes)
{
    AdapterView::OnNestedScrollAccepted(child, target, axes);
    Boolean res;
    StartNestedScroll(IView::SCROLL_AXIS_VERTICAL, &res);
    return NOERROR;
}

ECode AbsListView::OnNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dxConsumed,
    /* [in] */ Int32 dyConsumed,
    /* [in] */ Int32 dxUnconsumed,
    /* [in] */ Int32 dyUnconsumed)
{
    Int32 count;
    GetChildCount(&count);
    Int32 motionIndex = count / 2;
    AutoPtr<IView> motionView;
    GetChildAt(motionIndex, (IView**)&motionView);
    Int32 top;
    motionView->GetTop(&top);
    const Int32 oldTop = motionView != NULL ? top : 0;
    if (motionView == NULL || TrackMotionScroll(-dyUnconsumed, -dyUnconsumed)) {
        Int32 myUnconsumed = dyUnconsumed;
        Int32 myConsumed = 0;
        if (motionView != NULL) {
            myConsumed = top - oldTop;
            myUnconsumed -= myConsumed;
        }
        Boolean res;
        DispatchNestedScroll(0, myConsumed, 0, myUnconsumed, NULL, &res);
    }
    return NOERROR;
}

ECode AbsListView::OnNestedFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [in] */ Boolean consumed,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 childCount;
    GetChildCount(&childCount);
    Boolean result;
    if (!consumed && childCount > 0 && (CanScrollList((Int32) velocityY, &result), result) &&
            Elastos::Core::Math::Abs(velocityY) > mMinimumVelocity) {
        ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_FLING);
        if (mFlingRunnable == NULL) {
            mFlingRunnable = new FlingRunnable(this);
        }
        if ((DispatchNestedPreFling(0, velocityY, &result), !result)) {
            mFlingRunnable->Start((Int32) velocityY);
        }
        *res = TRUE;
        return NOERROR;
    }
    return DispatchNestedFling(velocityX, velocityY, consumed, res);
}

ECode AbsListView::Draw(
    /* [in] */ ICanvas* canvas)
{
    AdapterView::Draw(canvas);
    if (mEdgeGlowTop != NULL) {
        AutoPtr<IRect> rect;
        Boolean temp;
        Int32 scrollY = mScrollY;
        mEdgeGlowTop->IsFinished(&temp);
        if (!temp) {
            Int32 restoreCount;
            canvas->Save(&restoreCount);

            Int32 width;
            GetWidth(&width);

            Int32 edgeY = Elastos::Core::Math::Min(0, scrollY + mFirstPositionDistanceGuess);
            canvas->Translate(0, edgeY);
            Int32 height;
            GetHeight(&height);
            mEdgeGlowTop->SetSize(width, height);
            mEdgeGlowTop->Draw(canvas, &temp);
            if (temp) {
                Int32 w;
                GetWidth(&w);
                Int32 top;
                GetPaddingTop(&top);
                Int32 maxH;
                mEdgeGlowTop->GetMaxHeight(&maxH);
                Invalidate(0, 0, w, maxH + top);
            }
            canvas->RestoreToCount(restoreCount);
        }

        mEdgeGlowBottom->IsFinished(&temp);
        if (!temp) {
            Int32 restoreCount;
            canvas->Save(&restoreCount);

            Int32 width;
            GetWidth(&width);
            Int32 height;
            GetHeight(&height);

            Int32 edgeX = -width;
            Int32 edgeY = Elastos::Core::Math::Max(height, scrollY + mLastPositionDistanceGuess);
            canvas->Translate(edgeX, edgeY);
            canvas->Rotate(180, width, 0);
            mEdgeGlowBottom->SetSize(width, height);
            mEdgeGlowBottom->Draw(canvas, &temp);
            if (temp) {
                Int32 w;
                GetWidth(&w);
                Int32 bottom;
                GetPaddingBottom(&bottom);
                Int32 maxH;
                mEdgeGlowBottom->GetMaxHeight(&maxH);
                Invalidate(0, height - bottom - maxH, w, height);
            }
            canvas->RestoreToCount(restoreCount);
        }
    }

    return NOERROR;
}

ECode AbsListView::SetOverScrollEffectPadding(
    /* [in] */ Int32 leftPadding,
    /* [in] */ Int32 rightPadding)
{
    mGlowPaddingLeft = leftPadding;
    mGlowPaddingRight = rightPadding;
    return NOERROR;
}

void AbsListView::InitOrResetVelocityTracker()
{
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
    else {
        mVelocityTracker->Clear();
    }
}

void AbsListView::InitVelocityTrackerIfNotExists()
{
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
}

void AbsListView::RecycleVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

ECode AbsListView::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    if (disallowIntercept) {
        RecycleVelocityTracker();
    }
    return AdapterView::RequestDisallowInterceptTouchEvent(disallowIntercept);
}

ECode AbsListView::OnInterceptHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (mFastScroll != NULL && mFastScroll->OnInterceptHoverEvent(event)) {
        *res = TRUE;
        return NOERROR;
    }

    return AbsListView::OnInterceptHoverEvent(event, res);
}

ECode AbsListView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 actionMasked;
    ev->GetActionMasked(&actionMasked);

    if (mPositionScroller != NULL) {
        mPositionScroller->Stop();
    }

    Boolean suc;
    if (mIsDetaching || (IsAttachedToWindow(&suc), !suc)) {
        // Something isn't right.
        // Since we rely on being attached to get data set change notifications,
        // don't risk doing anything where we might try to resync and find things
        // in a bogus state.
        *res = FALSE;
        return NOERROR;
    }

    if (mFastScroll != NULL && mFastScroll->OnInterceptTouchEvent(ev)) {
        *res = TRUE;
        return NOERROR;
    }

    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            Int32 touchMode = mTouchMode;
            if (touchMode == TOUCH_MODE_OVERFLING || touchMode == TOUCH_MODE_OVERSCROLL) {
                mMotionCorrection = 0;
                *res = TRUE;
                return NOERROR;
            }

            Float fx, fy;
            ev->GetX(&fx);
            ev->GetY(&fy);

            Int32 x = (Int32)fx;
            Int32 y = (Int32)fy;
            ev->GetPointerId(0, &mActivePointerId);

            Int32 motionPosition = FindMotionRow(y);
            if (touchMode != TOUCH_MODE_FLING && motionPosition >= 0) {
                // User clicked on an actual view (and was not stopping a fling).
                // Remember where the motion event started
                //
                AutoPtr<IView> v;
                GetChildAt(motionPosition - mFirstPosition, (IView**)&v);
                v->GetTop(&mMotionViewOriginalTop);
                mMotionX = x;
                mMotionY = y;
                mMotionPosition = motionPosition;
                mTouchMode = TOUCH_MODE_DOWN;
                ClearScrollingCache();
            }

            mLastY = Elastos::Core::Math::INT32_MIN_VALUE;
            InitOrResetVelocityTracker();
            mVelocityTracker->AddMovement(ev);
            mNestedYOffset = 0;
            StartNestedScroll(IView::SCROLL_AXIS_VERTICAL, &suc);
            if (touchMode == TOUCH_MODE_FLING) {
                *res = TRUE;
                return NOERROR;
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            switch (mTouchMode) {
                case TOUCH_MODE_DOWN: {
                    Int32 pointerIndex;
                    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
                    if (pointerIndex == -1) {
                        pointerIndex = 0;
                        ev->GetPointerId(pointerIndex, &mActivePointerId);
                    }
                    Float fx, fy;
                    ev->GetX(pointerIndex, &fx);
                    ev->GetY(pointerIndex, &fy);
                    Int32 x = (Int32)fx;
                    Int32 y = (Int32)fy;
                    InitVelocityTrackerIfNotExists();
                    mVelocityTracker->AddMovement(ev);
                    if (StartScrollIfNeeded(x, y, NULL)) {
                        *res = TRUE;
                        return NOERROR;
                    }
                    break;
                }
            }
            break;
        }

        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP: {
            mTouchMode = TOUCH_MODE_REST;
            mActivePointerId = INVALID_POINTER;
            RecycleVelocityTracker();
            ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
            StopNestedScroll();
            break;
        }

        case IMotionEvent::ACTION_POINTER_UP: {
            OnSecondaryPointerUp(ev);
            break;
        }

        default:
            break;
    }

    *res = FALSE;
    return NOERROR;
}

void AbsListView::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);
    Int32 pointerIndex = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK) >>
            IMotionEvent::ACTION_POINTER_INDEX_SHIFT;
    Int32 pointerId;
    ev->GetPointerId(pointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {
        // This was our active pointer going up. Choose a new
        // active pointer and adjust accordingly.
        // TODO: Make this decision more intelligent.
        Int32 newPointerIndex = pointerIndex == 0 ? 1 : 0;
        Float fx, fy;
        ev->GetX(newPointerIndex, &fx);
        ev->GetY(newPointerIndex, &fy);
        mMotionX = (Int32)fx;
        mMotionY = (Int32)fy;
        mMotionCorrection = 0;
        ev->GetPointerId(newPointerIndex, &mActivePointerId);
    }
}

ECode AbsListView::AddTouchables(
    /* [in] */ IArrayList* views)
{
    Int32 count;
    GetChildCount(&count);
    Int32 firstPosition = mFirstPosition;
    AutoPtr<IListAdapter> adapter = mAdapter;

    if (adapter == NULL) {
        return NOERROR;
    }

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Boolean isEnabled;
        adapter->IsEnabled(firstPosition + i, &isEnabled);
        if (isEnabled) {
            views->Add(child);
        }
        child->AddTouchables(views);
    }

    return NOERROR;
}

void AbsListView::ReportScrollStateChange(
    /* [in] */ Int32 newState)
{
    if (newState != mLastScrollState) {
        if (mOnScrollListener != NULL) {
            mLastScrollState = newState;
            mOnScrollListener->OnScrollStateChanged(this, newState);
        }
    }
}

ECode AbsListView::SetFriction(
    /* [in] */ Float friction)
{
    if (mFlingRunnable == NULL) {
        mFlingRunnable = new FlingRunnable(this);
    }
    mFlingRunnable->mScroller->SetFriction(friction);

    return NOERROR;
}

ECode AbsListView::SetVelocityScale(
    /* [in] */ Float scale)
{
    mVelocityScale = scale;
    return NOERROR;
}

AutoPtr<AbsListView::AbsPositionScroller> AbsListView::CreatePositionScroller()
{
    AutoPtr<AbsListView::AbsPositionScroller> scroller = new AbsListView::PositionScroller(this);
    return scroller;
}

ECode AbsListView::SmoothScrollToPosition(
    /* [in] */ Int32 position)
{
    if (mPositionScroller == NULL) {
        mPositionScroller = CreatePositionScroller();
    }
    mPositionScroller->Start(position);

    return NOERROR;
}

ECode AbsListView::SmoothScrollToPositionFromTop(
    /* [in] */ Int32 position,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 duration)
{
    if (mPositionScroller == NULL) {
        mPositionScroller = CreatePositionScroller();
    }
    mPositionScroller->StartWithOffset(position, offset, duration);
    return NOERROR;
}

ECode AbsListView::SmoothScrollToPositionFromTop(
    /* [in] */ Int32 position,
    /* [in] */ Int32 offset)
{
    if (mPositionScroller == NULL) {
        mPositionScroller = CreatePositionScroller();
    }
    mPositionScroller->StartWithOffset(position, offset, offset);
    return NOERROR;
}

ECode AbsListView::SmoothScrollToPosition(
    /* [in] */ Int32 position,
    /* [in] */ Int32 boundPosition)
{
    if (mPositionScroller == NULL) {
        mPositionScroller = CreatePositionScroller();
    }
    mPositionScroller->Start(position, boundPosition);

    return NOERROR;
}

ECode AbsListView::SmoothScrollBy(
    /* [in] */ Int32 distance,
    /* [in] */ Int32 duration)
{
    SmoothScrollBy(distance, duration, FALSE);

    return NOERROR;
}

void AbsListView::SmoothScrollBy(
    /* [in] */ Int32 distance,
    /* [in] */ Int32 duration,
    /* [in] */ Boolean linear)
{
    if (mFlingRunnable == NULL) {
        mFlingRunnable = new FlingRunnable(this);
    }

    // No sense starting to scroll if we're not going anywhere
    Int32 firstPos = mFirstPosition;
    Int32 childCount;
    GetChildCount(&childCount);
    Int32 lastPos = firstPos + childCount;
    Int32 topLimit;
    GetPaddingTop(&topLimit);
    Int32 height, bottom;
    GetHeight(&height);
    GetPaddingBottom(&bottom);
    Int32 bottomLimit = height - bottom;

    AutoPtr<IView> view;
    GetChildAt(0, (IView**)&view);
    Int32 topTemp;
    view->GetTop(&topTemp);
    view = NULL;
    GetChildAt(childCount - 1, (IView**)&view);
    Int32 bottomTemp;
    view->GetBottom(&bottomTemp);
    if (distance == 0 || mItemCount == 0 || childCount == 0 ||
            (firstPos == 0 && topTemp == topLimit && distance < 0) ||
            (lastPos == mItemCount && bottomTemp== bottomLimit && distance > 0)) {
        mFlingRunnable->EndFling();
        if (mPositionScroller != NULL) {
            mPositionScroller->Stop();
        }
    }
    else {
        ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_FLING);
        mFlingRunnable->StartScroll(distance, duration, linear);
    }
}

ECode AbsListView::SmoothScrollByOffset(
    /* [in] */ Int32 position)
{
    Int32 index = -1;
    if (position < 0) {
        GetFirstVisiblePosition(&index);
    }
    else if (position > 0) {
        GetLastVisiblePosition(&index);
    }

    if (index > -1) {
        Int32 pos;
        GetFirstVisiblePosition(&pos);
        AutoPtr<IView> child;
        GetChildAt(index - pos, (IView**)&child);
        if (child != NULL) {
            AutoPtr<IRect> visibleRect;
            CRect::New((IRect**)&visibleRect);
            Boolean childVisible;
            if (child->GetGlobalVisibleRect(visibleRect, &childVisible), childVisible) {
                // the child is partially visible
                Int32 childRectArea ;
                child->GetWidth(&childRectArea);
                Int32 childHeight;
                child->GetHeight(&childHeight);
                childRectArea *= childHeight;
                Int32 visibleRectArea;
                visibleRect->GetWidth(&visibleRectArea);
                Int32 visibleHeight;
                visibleRect->GetHeight(&visibleHeight);
                visibleRectArea *= visibleHeight;
                Float visibleArea = (visibleRectArea / (Float) childRectArea);
                Float visibleThreshold = 0.75f;
                if ((position < 0) && (visibleArea < visibleThreshold)) {
                    // the top index is not perceivably visible so offset
                    // to account for showing that top index as well
                    ++index;
                }
                else if ((position > 0) && (visibleArea < visibleThreshold)) {
                    // the bottom index is not perceivably visible so offset
                    // to account for showing that bottom index as well
                    --index;
                }
            }
            Int32 count;
            GetCount(&count);
            SmoothScrollToPosition(Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(count, index + position)));
        }
    }

    return NOERROR;
}

void AbsListView::CreateScrollingCache()
{
    Boolean res;
    if (mScrollingCacheEnabled && !mCachingStarted && (IsHardwareAccelerated(&res), res)) {
        SetChildrenDrawnWithCacheEnabled(TRUE);
        SetChildrenDrawingCacheEnabled(TRUE);
        mCachingStarted = mCachingActive = TRUE;
    }
}

void AbsListView::ClearScrollingCache()
{
    Boolean res;
    if (IsHardwareAccelerated(&res), res) {
        if (mClearScrollingCache == NULL) {
            AutoPtr<ClearScrollingCacheRunnable> temp
                = new ClearScrollingCacheRunnable(this);
            mClearScrollingCache = temp;
        }
    }

    Post(mClearScrollingCache, &res);
}

ECode AbsListView::ScrollListBy(
    /* [in] */ Int32 y)
{
    TrackMotionScroll(-y, -y);
    return NOERROR;
}

ECode AbsListView::CanScrollList(
    /* [in] */ Int32 direction,
    /* [out] */ Boolean* canScrollList)
{
    VALIDATE_NOT_NULL(canScrollList);
    *canScrollList = FALSE;

    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount == 0) {
        return NOERROR;
    }

    const Int32 firstPosition = mFirstPosition;
    const AutoPtr<CRect> listPadding = (CRect*)mListPadding.Get();
    if (direction > 0) {
        AutoPtr<IView> view;
        GetChildAt(childCount - 1, (IView**)&view);
        Int32 lastBottom;
        view->GetBottom(&lastBottom);
        const Int32 lastPosition = firstPosition + childCount;
        Int32 height;
        GetHeight(&height);
        *canScrollList = lastPosition < mItemCount || lastBottom > height - listPadding->mBottom;
        return NOERROR;
    }
    else {
        AutoPtr<IView> view;
        GetChildAt(0, (IView**)&view);
        Int32 firstTop;
        view->GetTop(&firstTop);
        *canScrollList = firstPosition > 0 || firstTop < listPadding->mTop;
        return NOERROR;
    }
}

Boolean AbsListView::TrackMotionScroll(
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 incrementalDeltaY)
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount == 0) {
        return TRUE;
    }

    AutoPtr<IView> view;
    GetChildAt(0, (IView**)&view);
    Int32 firstTop;
    view->GetTop(&firstTop);
    view = NULL;
    GetChildAt(childCount - 1, (IView**)&view);
    Int32 lastBottom;
    view->GetBottom(&lastBottom);

    // "effective padding" In this case is the amount of padding that affects
    // how much space should not be filled by items. If we don't clip to padding
    // there is no effective padding.
    Int32 effectivePaddingTop = 0;
    Int32 effectivePaddingBottom = 0;
    Int32 topTmp = 0, bottomTmp = 0;
    mListPadding->GetTop(&topTmp);
    mListPadding->GetBottom(&bottomTmp);
    if ((mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK) {
        effectivePaddingTop = topTmp;
        effectivePaddingBottom = bottomTmp;
    }

     // FIXME account for grid vertical spacing too?
    Int32 h;
    GetHeight(&h);
    Int32 spaceAbove = effectivePaddingTop - firstTop;
    Int32 end = h - effectivePaddingBottom;
    Int32 spaceBelow = lastBottom - end;

    Int32 height = h - mPaddingBottom - mPaddingTop;
    if (deltaY < 0) {
        deltaY = Elastos::Core::Math::Max(-(height - 1), deltaY);
    }
    else {
        deltaY = Elastos::Core::Math::Min(height - 1, deltaY);
    }

    if (incrementalDeltaY < 0) {
        incrementalDeltaY = Elastos::Core::Math::Max(-(height - 1), incrementalDeltaY);
    }
    else {
        incrementalDeltaY = Elastos::Core::Math::Min(height - 1, incrementalDeltaY);
    }

    Int32 firstPosition = mFirstPosition;

    // Update our guesses for where the first and last views are
    if (firstPosition == 0) {
        mFirstPositionDistanceGuess = firstTop - topTmp;
    }
    else {
        mFirstPositionDistanceGuess += incrementalDeltaY;
    }

    if (firstPosition + childCount == mItemCount) {
        mLastPositionDistanceGuess = lastBottom + bottomTmp;
    }
    else {
        mLastPositionDistanceGuess += incrementalDeltaY;
    }

    Boolean cannotScrollDown = (firstPosition == 0 &&
            firstTop >= topTmp && incrementalDeltaY >= 0);
    Boolean cannotScrollUp = (firstPosition + childCount == mItemCount &&
            lastBottom <= h - bottomTmp && incrementalDeltaY <= 0);

    if (cannotScrollDown || cannotScrollUp) {
        return incrementalDeltaY != 0;
    }

    Boolean down = incrementalDeltaY < 0;
    Boolean inTouchMode;
    IsInTouchMode(&inTouchMode);
    if (inTouchMode) {
        HideSelector();
    }

    Int32 headerViewsCount = GetHeaderViewsCount();
    Int32 footerViewsStart = mItemCount - GetFooterViewsCount();

    Int32 start = 0;
    Int32 count = 0;

    if (down) {
        Int32 top = -incrementalDeltaY;
        if ((mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK) {
            top += topTmp;
        }
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 childBottom;
            child->GetBottom(&childBottom);
            if (childBottom >= top) {
                break;
            }
            else {
                count++;
                Int32 position = firstPosition + i;
                if (position >= headerViewsCount && position < footerViewsStart) {
                    // The view will be rebound to new data, clear any
                    // system-managed transient state.
                    Boolean res;
                    if (child->IsAccessibilityFocused(&res), res) {
                        child->ClearAccessibilityFocus();
                    }
                    mRecycler->AddScrapView(child, position);
                }
            }
        }
    }
    else {
        Int32 height;
        GetHeight(&height);
        Int32 bottom = height - incrementalDeltaY;
        if ((mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK) {
            bottom -= bottomTmp;
        }
        for (Int32 i = childCount - 1; i >= 0; i--) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 childTop;
            child->GetTop(&childTop);
            if (childTop <= bottom) {
                break;
            }
            else {
                start = i;
                count++;
                Int32 position = firstPosition + i;
                if (position >= headerViewsCount && position < footerViewsStart) {
                    // The view will be rebound to new data, clear any
                    // system-managed transient state.
                    Boolean res;
                    if (child->IsAccessibilityFocused(&res), res) {
                        child->ClearAccessibilityFocus();
                    }
                    mRecycler->AddScrapView(child, position);
                }
            }
        }
    }

    mMotionViewNewTop = mMotionViewOriginalTop + deltaY;

    mBlockLayoutRequests = TRUE;

    if (count > 0) {
        DetachViewsFromParent(start, count);

        mRecycler->RemoveSkippedScrap();
    }

    // invalidate before moving the children to avoid unnecessary invalidate
    // calls to bubble up from the children all the way to the top
    if (!AwakenScrollBars()) {
        Invalidate();
    }

    OffsetChildrenTopAndBottom(incrementalDeltaY);

    if (down) {
        mFirstPosition += count;
    }

    Int32 absIncrementalDeltaY = Elastos::Core::Math::Abs(incrementalDeltaY);
    if (spaceAbove < absIncrementalDeltaY
        || spaceBelow < absIncrementalDeltaY) {
        FillGap(down);
    }

    if (!inTouchMode && mSelectedPosition != IAdapterView::INVALID_POSITION) {
        Int32 childIndex = mSelectedPosition - mFirstPosition;
        Int32 count;
        GetChildCount(&count);
        if (childIndex >= 0 && childIndex < count) {
            AutoPtr<IView> view;
            GetChildAt(childIndex, (IView**)&view);
            PositionSelector(mSelectedPosition, view);
        }
    }
    else if (mSelectorPosition != IAdapterView::INVALID_POSITION) {
        Int32 childIndex = mSelectorPosition - mFirstPosition;
        Int32 count;
        GetChildCount(&count);
        if (childIndex >= 0 && childIndex < count) {
            AutoPtr<IView> view;
            GetChildAt(childIndex, (IView**)&view);
            PositionSelector(IAdapterView::INVALID_POSITION, view);
        }
    }
    else {
        mSelectorRect->SetEmpty();
    }

    mBlockLayoutRequests = FALSE;

    InvokeOnItemScrollListener();
    return FALSE;
}

Int32 AbsListView::GetHeaderViewsCount()
{
    return 0;
}

Int32 AbsListView::GetFooterViewsCount()
{
    return 0;
}

ECode AbsListView::HideSelector()
{
    if (mSelectedPosition != IAdapterView::INVALID_POSITION) {
        if (mLayoutMode != LAYOUT_SPECIFIC) {
            mResurrectToPosition = mSelectedPosition;
        }

        if (mNextSelectedPosition >= 0 && mNextSelectedPosition != mSelectedPosition) {
            mResurrectToPosition = mNextSelectedPosition;
        }

        SetSelectedPositionInt(IAdapterView::INVALID_POSITION);
        SetNextSelectedPositionInt(IAdapterView::INVALID_POSITION);
        mSelectedTop = 0;
    }
    return NOERROR;
}

Int32 AbsListView::ReconcileSelectedPosition()
{
    Int32 position = mSelectedPosition;
    if (position < 0) {
        position = mResurrectToPosition;
    }
    position = Elastos::Core::Math::Max(0, position);
    position = Elastos::Core::Math::Min(position, mItemCount - 1);
    return position;
}

Int32 AbsListView::FindClosestMotionRow(
    /* [in] */ Int32 y)
{
    Int32 childCount;
    GetChildCount(&childCount);
    if (childCount == 0) {
        return IAdapterView::INVALID_POSITION;
    }

    Int32 motionRow = FindMotionRow(y);
    return motionRow != IAdapterView::INVALID_POSITION
            ? motionRow : mFirstPosition + childCount - 1;
}

ECode AbsListView::InvalidateViews()
{
    mDataChanged = TRUE;
    RememberSyncState();
    RequestLayout();
    Invalidate();

    return NOERROR;
}

Boolean AbsListView::ResurrectSelectionIfNeeded()
{
    if (mSelectedPosition < 0 && ResurrectSelection()) {
        UpdateSelectorState();
        return TRUE;
    }

    return FALSE;
}

Boolean AbsListView::ResurrectSelection()
{
    Int32 childCount;
    GetChildCount(&childCount);

    if (childCount <= 0) {
        return FALSE;
    }

    Int32 selectedTop = 0;
    Int32 selectedPos;

    Int32 topTmp = 0, bottomTmp = 0;
    mListPadding->GetTop(&topTmp);
    mListPadding->GetBottom(&bottomTmp);
    Int32 childrenTop = topTmp;
    Int32 childrenBottom = mBottom - mTop - bottomTmp;
    Int32 firstPosition = mFirstPosition;
    Int32 toPosition = mResurrectToPosition;
    Boolean down = TRUE;

    if (toPosition >= firstPosition
        && toPosition < firstPosition + childCount) {

        selectedPos = toPosition;
        AutoPtr<IView> selected;
        GetChildAt(selectedPos - mFirstPosition, (IView**)&selected);
        selected->GetTop(&selectedTop);
        Int32 selectedBottom;
        selected->GetBottom(&selectedBottom);

        // We are scrolled, don't get in the fade
        Int32 len;
        GetVerticalFadingEdgeLength(&len);
        if (selectedTop < childrenTop) {
            selectedTop = childrenTop + len;
        }
        else if (selectedBottom > childrenBottom) {
            Int32 measureHeight;
            selected->GetMeasuredHeight(&measureHeight);
            selectedTop =
                    childrenBottom - measureHeight - len;
        }
    }
    else {
        if (toPosition < firstPosition) {
            // Default to selecting whatever is first
            selectedPos = firstPosition;
            for (Int32 i = 0; i < childCount; i++) {
                AutoPtr<IView> view;
                GetChildAt(i, (IView**)&view);
                Int32 top;
                view->GetTop(&top);

                if (i == 0) {
                    // Remember the position of the first item
                    selectedTop = top;
                    // See if we are scrolled at all
                    if (firstPosition > 0 || top < childrenTop) {
                        // If we are scrolled, don't select anything that is
                        // in the fade region
                        Int32 len;
                        GetVerticalFadingEdgeLength(&len);
                        childrenTop += len;
                    }
                }

                if (top >= childrenTop) {
                    // Found a view whose top is fully visisble
                    selectedPos = firstPosition + i;
                    selectedTop = top;
                    break;
                }
            }
        }
        else {
            Int32 itemCount = mItemCount;
            down = FALSE;
            selectedPos = firstPosition + childCount - 1;

            for (Int32 i = childCount - 1; i >= 0; i--) {
                AutoPtr<IView> v;
                GetChildAt(i, (IView**)&v);
                Int32 top, bottom;
                v->GetTop(&top);
                v->GetBottom(&bottom);

                if (i == childCount - 1) {
                    selectedTop = top;
                    if (firstPosition + childCount < itemCount
                        || bottom > childrenBottom) {
                        Int32 len;
                        GetVerticalFadingEdgeLength(&len);
                        childrenBottom -= len;
                    }
                }

                if (bottom <= childrenBottom) {
                    selectedPos = firstPosition + i;
                    selectedTop = top;
                    break;
                }
            }
        }
    }

    mResurrectToPosition = IAdapterView::INVALID_POSITION;
    Boolean res;
    RemoveCallbacks(mFlingRunnable, &res);
    if (mPositionScroller != NULL) {
        mPositionScroller->Stop();
    }
    mTouchMode = TOUCH_MODE_REST;
    ClearScrollingCache();
    mSpecificTop = selectedTop;
    selectedPos = LookForSelectablePosition(selectedPos, down);
    Int32 pos;
    if (selectedPos >= firstPosition
        && selectedPos <= (GetLastVisiblePosition(&pos), pos)) {
        mLayoutMode = LAYOUT_SPECIFIC;
        UpdateSelectorState();
        SetSelectionInt(selectedPos);
        InvokeOnItemScrollListener();
    }
    else {
        selectedPos = IAdapterView::INVALID_POSITION;
    }
    ReportScrollStateChange(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);

    return selectedPos >= 0;

}

void AbsListView::ConfirmCheckedPositionsById()
{
    using Elastos::Core::Math;

    // Clear out the positional check states, we'll rebuild it below from IDs.
    mCheckStates->Clear();

    Int64 id, lastPosId, searchId;
    Int32 lastPos, start, end;
    Boolean found = FALSE;
    Boolean checkedCountChanged = FALSE;
    Int32 size;
    mCheckedIdStates->GetSize(&size);
    for (Int32 checkedIndex = 0; checkedIndex < size; checkedIndex++) {
        mCheckedIdStates->KeyAt(checkedIndex, &id);
        AutoPtr<IInterface> obj;
        mCheckedIdStates->ValueAt(checkedIndex, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&lastPos);
        IAdapter::Probe(mAdapter)->GetItemId(lastPos, &lastPosId);
        if (id != lastPosId) {
            // Look around to see if the ID is nearby. If not, uncheck it.
            start = Math::Max(0, lastPos - CHECK_POSITION_SEARCH_DISTANCE);
            end = Math::Min(lastPos + CHECK_POSITION_SEARCH_DISTANCE, mItemCount);

            for (Int32 searchPos = start; searchPos < end; searchPos++) {
                IAdapter::Probe(mAdapter)->GetItemId(searchPos, &searchId);
                if (id == searchId) {
                    found = TRUE;
                    mCheckStates->Put(searchPos, TRUE);
                    mCheckedIdStates->Put(id, CoreUtils::Convert(searchPos));
                    break;
                }
            }

            if (!found) {
                mCheckedIdStates->Delete(id);
                checkedIndex--;
                mCheckedItemCount--;
                checkedCountChanged = TRUE;
                if (mChoiceActionMode != NULL && mMultiChoiceModeCallback != NULL) {
                    mMultiChoiceModeCallback->OnItemCheckedStateChanged(mChoiceActionMode,
                        lastPos, id, FALSE);
                }
            }
        }
        else {
            mCheckStates->Put(lastPos, TRUE);
        }
    }

    if (checkedCountChanged && mChoiceActionMode != NULL) {
        mChoiceActionMode->Invalidate();
    }
}

void AbsListView::HandleDataChanged()
{
    Int32 count = mItemCount;

    Int32 lastHandledItemCount = mLastHandledItemCount;
    mLastHandledItemCount = mItemCount;

    Boolean hasStableIds;
    IAdapter::Probe(mAdapter)->HasStableIds(&hasStableIds);
    if (mChoiceMode != IAbsListView::CHOICE_MODE_NONE && mAdapter != NULL && hasStableIds) {
        ConfirmCheckedPositionsById();
    }

    // TODO: In the future we can recycle these views based on stable ID instead.
    mRecycler->ClearTransientStateViews();

    Boolean res;

    if (count > 0) {
        Int32 newPos;
        Int32 selectablePos;

        // Find the row we are supposed to sync to
        if (mNeedSync) {
            // Update this first, since setNextSelectedPositionInt inspects it
            mNeedSync = FALSE;
            mPendingSync = NULL;

            if (mTranscriptMode == IAbsListView::AbsListView_TRANSCRIPT_MODE_ALWAYS_SCROLL) {
                mLayoutMode = LAYOUT_FORCE_BOTTOM;
                return;
            }
            else if (mTranscriptMode == IAbsListView::AbsListView_TRANSCRIPT_MODE_NORMAL) {
                if (mForceTranscriptScroll) {
                    mForceTranscriptScroll = FALSE;
                    mLayoutMode = LAYOUT_FORCE_BOTTOM;
                    return;
                }
                Int32 childCount;
                GetChildCount(&childCount);
                Int32 height, bottom;
                GetHeight(&height);
                GetPaddingBottom(&bottom);
                Int32 listBottom = height - bottom;
                AutoPtr<IView> lastChild;
                GetChildAt(childCount - 1, (IView**)&lastChild);
                Int32 lastChildBottom;
                lastChild->GetBottom(&lastChildBottom);
                Int32 lastBottom = lastChild != NULL ? lastChildBottom : listBottom;
                if (mFirstPosition + childCount >= lastHandledItemCount &&
                    lastBottom <= listBottom) {
                        mLayoutMode = LAYOUT_FORCE_BOTTOM;
                        return;
                }
                // Something new came in and we didn't scroll; give the user a clue that
                // there's something new.
                AwakenScrollBars();
            }

            switch (mSyncMode) {
                case SYNC_SELECTED_POSITION:
                    if (IsInTouchMode(&res), res) {
                        // We saved our state when not in touch mode. (We know this because
                        // mSyncMode is SYNC_SELECTED_POSITION.) Now we are trying to
                        // restore in touch mode. Just leave mSyncPosition as it is (possibly
                        // adjusting if the available range changed) and return.
                        mLayoutMode = LAYOUT_SYNC;
                        mSyncPosition = Elastos::Core::Math::Min(Elastos::Core::Math::Max(0, mSyncPosition), count - 1);

                        return;
                    }
                    else {
                        // See if we can find a position in the new data with the same
                        // id as the old selection. This will change mSyncPosition.
                        newPos = FindSyncPosition();
                        if (newPos >= 0) {
                            // Found it. Now verify that new selection is still selectable
                            selectablePos = LookForSelectablePosition(newPos, TRUE);
                            if (selectablePos == newPos) {
                                // Same row id is selected
                                mSyncPosition = newPos;

                                Int32 height;
                                GetHeight(&height);
                                if (mSyncHeight == height) {
                                    // If we are at the same height as when we saved state, try
                                    // to restore the scroll position too.
                                    mLayoutMode = LAYOUT_SYNC;
                                }
                                else {
                                    // We are not the same height as when the selection was saved, so
                                    // don't try to restore the exact position
                                    mLayoutMode = LAYOUT_SET_SELECTION;
                                }

                                // Restore selection
                                SetNextSelectedPositionInt(newPos);
                                return;
                            }
                        }
                    }
                    break;
                case SYNC_FIRST_POSITION:
                    // Leave mSyncPosition as it is -- just pin to available range
                    mLayoutMode = LAYOUT_SYNC;
                    mSyncPosition = Elastos::Core::Math::Min(Elastos::Core::Math::Max(0, mSyncPosition), count - 1);

                    return;
                default:
                    break;
            }
        }

        if (IsInTouchMode(&res), !res) {
            // We couldn't find matching data -- try to use the same position
            GetSelectedItemPosition(&newPos);

            // Pin position to the available range
            if (newPos >= count) {
                newPos = count - 1;
            }

            if (newPos < 0) {
                newPos = 0;
            }

            // Make sure we select something selectable -- first look down
            selectablePos = LookForSelectablePosition(newPos, TRUE);

            if (selectablePos >= 0) {
                SetNextSelectedPositionInt(selectablePos);
                return;
            }
            else {
                // Looking down didn't work -- try looking up
                selectablePos = LookForSelectablePosition(newPos, FALSE);
                if (selectablePos >= 0) {
                    SetNextSelectedPositionInt(selectablePos);
                    return;
                }
            }
        }
        else {
            // We already know where we want to resurrect the selection
            if (mResurrectToPosition >= 0) {
                return;
            }
        }
    }

    // Nothing is selected. Give up and reset everything.
    mLayoutMode = mStackFromBottom ? LAYOUT_FORCE_BOTTOM : LAYOUT_FORCE_TOP;
    mSelectedPosition = IAdapterView::INVALID_POSITION;
    mSelectedRowId = IAdapterView::INVALID_ROW_ID;
    mNextSelectedPosition = IAdapterView::INVALID_POSITION;
    mNextSelectedRowId = IAdapterView::INVALID_ROW_ID;
    mNeedSync = FALSE;
    mPendingSync = NULL;
    mSelectorPosition = IAdapterView::INVALID_POSITION;
    CheckSelectionChanged();
}

void AbsListView::OnDisplayHint(
    /* [in] */ Int32 hint)
{
    AdapterView::OnDisplayHint(hint);
    switch (hint) {
        case IView::INVISIBLE:
            if (mPopup != NULL) {
                Boolean showing;
                mPopup->IsShowing(&showing);
                if(showing)
                DismissPopup();
            }
            break;
        case IView::VISIBLE:
            if (mFiltered && mPopup != NULL) {
                Boolean showing;
                mPopup->IsShowing(&showing);
                if(showing)
                ShowPopup();
            }
            break;
    }
    mPopupHidden = hint == IView::INVISIBLE;
}

void AbsListView::DismissPopup()
{
    if (mPopup != NULL) {
        mPopup->Dismiss();
    }
}

void AbsListView::ShowPopup()
{
    // Make sure we have a window before showing the popup
    Int32 visibility;
    GetWindowVisibility(&visibility);
    if (visibility == IView::VISIBLE) {
        CreateTextFilter(TRUE);
        PositionPopup();
        // Make sure we get focus if we are showing the popup
        CheckFocus();
    }
}

void AbsListView::PositionPopup()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 screenHeight;
    dm->GetHeightPixels(&screenHeight);
    AutoPtr< ArrayOf<Int32> > xy = ArrayOf<Int32>::Alloc(2);
    GetLocationOnScreen((ArrayOf<Int32>**)&xy);
    // TODO: The 20 below should come from the theme
    // TODO: And the gravity should be defined in the theme as well
    Int32 height;
    GetHeight(&height);
    Int32 bottomGap = screenHeight - (*xy)[1] - height + (Int32)(mDensityScale * 20);

    Boolean showing;
    if (mPopup->IsShowing(&showing), !showing) {
        mPopup->ShowAtLocation(this,
                IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL, (*xy)[0], bottomGap);
    }
    else {
        mPopup->Update((*xy)[0], bottomGap, -1, -1);
    }
}

ECode AbsListView::GetDistance(
    /* [in] */ IRect* sourceRect,
    /* [in] */ IRect* destRect,
    /* [in] */ Int32 direction,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(sourceRect);
    VALIDATE_NOT_NULL(destRect);

    CRect* source = (CRect*)sourceRect;
    CRect* dest = (CRect*)destRect;

    Int32 sX, sY; // source x, y
    Int32 dX, dY; // dest x, y

    Int32 sHeight, sWidth, dHeight, dWidth;
    source->GetHeight(&sHeight);
    source->GetWidth(&sWidth);
    dest->GetHeight(&dHeight);
    dest->GetWidth(&dWidth);

    switch (direction) {
        case IView::FOCUS_RIGHT: {
                sX = source->mRight;
                sY = source->mTop + sHeight / 2;
                dX = dest->mLeft;
                dY = dest->mTop + dHeight / 2;
            }
            break;
        case IView::FOCUS_DOWN: {
                sX = source->mLeft + sWidth / 2;
                sY = source->mBottom;
                dX = dest->mLeft + dWidth / 2;
                dY = dest->mTop;
            }
            break;
        case IView::FOCUS_LEFT: {
                sX = source->mLeft;
                sY = source->mTop + sHeight / 2;
                dX = dest->mRight;
                dY = dest->mTop + dHeight / 2;
            }
            break;
        case IView::FOCUS_UP: {
                sX = source->mLeft + sWidth / 2;
                sY = source->mTop;
                dX = dest->mLeft + dWidth / 2;
                dY = dest->mBottom;
            }
            break;
        case IView::FOCUS_FORWARD:
        case IView::FOCUS_BACKWARD:
            sX = source->mRight + sWidth / 2;
            sY = source->mTop + sHeight / 2;
            dX = dest->mLeft + dWidth / 2;
            dY = dest->mTop + dHeight / 2;
            break;
        default:
            Logger::E("AbsListView",
                    "direction must be one of {FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, FOCUS_RIGHT, FOCUS_FORWARD, FOCUS_BACKWARD}.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 deltaX = dX - sX;
    Int32 deltaY = dY - sY;

    *result = deltaY * deltaY + deltaX * deltaX;
    return NOERROR;
}

ECode AbsListView::IsInFilterMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFiltered;
    return NOERROR;
}

Boolean AbsListView::SendToTextFilter(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event)
{
    if (!AcceptFilter()) {
        return FALSE;
    }

    Boolean handled = FALSE;
    Boolean okToSend = TRUE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_UP:
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_ENTER:
            okToSend = FALSE;
            break;
        case IKeyEvent::KEYCODE_BACK:
            if (mFiltered && mPopup != NULL) {
                Boolean showing;
                mPopup->IsShowing(&showing);
                if(showing) {
                    Int32 action, repeatCount;
                    Boolean tracking, canceled;
                    event->GetAction(&action);
                    event->GetRepeatCount(&repeatCount);
                    event->IsTracking(&tracking);
                    event->IsCanceled(&canceled);
                    if (action == IKeyEvent::ACTION_DOWN && repeatCount == 0) {
                        AutoPtr<IDispatcherState> state;
                        GetKeyDispatcherState((IDispatcherState**)&state);
                        state->StartTracking(event, TO_IINTERFACE(this));
                        handled = TRUE;
                    }
                    else if (action == IKeyEvent::ACTION_UP && tracking && !canceled) {
                        handled = TRUE;
                        ITextView::Probe(mTextFilter)->SetText(CoreUtils::Convert(""));
                    }
                }
            }
            okToSend = FALSE;
            break;

        case IKeyEvent::KEYCODE_SPACE:
            // Only send spaces once we are filtered
            okToSend = mFiltered;
            break;
    }

    if (okToSend) {
        CreateTextFilter(TRUE);

        AutoPtr<IKeyEvent> forwardEvent = event;
        Int32 repeatCount;
        forwardEvent->GetRepeatCount(&repeatCount);
        if (repeatCount > 0) {
            Int64 eventTime;
            IInputEvent::Probe(event)->GetEventTime(&eventTime);
            forwardEvent = NULL;
            KeyEvent::ChangeTimeRepeat(
                    event, eventTime, 0, (IKeyEvent**)&forwardEvent);
        }

        Int32 action;
        event->GetAction(&action);
        AutoPtr<IKeyEventCallback> keyCallback = IKeyEventCallback::Probe(mTextFilter);
        switch (action) {
            case IKeyEvent::ACTION_DOWN:
                keyCallback->OnKeyDown(keyCode, forwardEvent, &handled);
                break;
            case IKeyEvent::ACTION_UP:
                keyCallback->OnKeyUp(keyCode, forwardEvent, &handled);
                break;
            case IKeyEvent::ACTION_MULTIPLE:
                keyCallback->OnKeyMultiple(keyCode, count, event, &handled);
                break;
        }
    }
    return handled;
}

AbsListView::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs,
    /* [out] */ IInputConnection** connection)
{
    VALIDATE_NOT_NULL(connection);
    *connection = NULL;

    Boolean res;
    if (IsTextFilterEnabled(&res), res) {
        if (mPublicInputConnection == NULL) {
            CBaseInputConnection::New(this, FALSE, (IInputConnection**)&mDefInputConnection);
            mPublicInputConnection = new InputConnectionWrapper(this, outAttrs);
        }
        outAttrs->SetInputType(IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_FILTER);
        outAttrs->SetImeOptions(IEditorInfo::IME_ACTION_DONE);
        *connection = mPublicInputConnection;
        return NOERROR;
    }
    return NOERROR;
}

ECode AbsListView::CheckInputConnectionProxy(
    /* [in] */ IView* view,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = view == IView::Probe(mTextFilter);
    return NOERROR;
}

void AbsListView::CreateTextFilter(
    /* [in] */ Boolean animateEntrance)
{
    if (mPopup == NULL) {
        AutoPtr<IContext> c;
        GetContext((IContext**)&c);
        AutoPtr<IPopupWindow> p;
        CPopupWindow::New(c, (IPopupWindow**)&p);

        p->SetFocusable(FALSE);
        p->SetTouchable(FALSE);

        p->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
        p->SetContentView(IView::Probe(GetTextFilterInput()));
        p->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
        p->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
        p->SetBackgroundDrawable(NULL);
        mPopup = p;
        AutoPtr<IViewTreeObserver> treeObserver;
        GetViewTreeObserver((IViewTreeObserver**)&treeObserver);
        treeObserver->AddOnGlobalLayoutListener(this);
        mGlobalLayoutListenerAddedFilter = TRUE;
    }
    if (animateEntrance) {
        mPopup->SetAnimationStyle(R::style::Animation_TypingFilter);
    }
    else {
        mPopup->SetAnimationStyle(R::style::Animation_TypingFilterRestore);
    }
}

AutoPtr<IEditText> AbsListView::GetTextFilterInput()
{
    if (mTextFilter == NULL) {
        AutoPtr<IContext> c;
        GetContext((IContext**)&c);
        AutoPtr<ILayoutInflater> layoutInflater;
        LayoutInflater::From(c, (ILayoutInflater**)&layoutInflater);
        AutoPtr<IView> view;
        layoutInflater->Inflate(R::layout::typing_filter, NULL, (IView**)&view);
        mTextFilter = IEditText::Probe(view);
        // For some reason setting this as the "real" input type changes
        // the text view in some way that it doesn't work, and I don't
        // want to figure out why this is.
        AutoPtr<ITextView> textview = ITextView::Probe(mTextFilter);
        textview->SetRawInputType(IInputType::TYPE_CLASS_TEXT
                | IInputType::TYPE_TEXT_VARIATION_FILTER);
        textview->SetImeOptions(IEditorInfo::IME_FLAG_NO_EXTRACT_UI);
        textview->AddTextChangedListener(this);
    }
    return mTextFilter;
}

ECode AbsListView::ClearTextFilter()
{
    if (mFiltered) {
        AutoPtr<IEditText> text = GetTextFilterInput();
        ITextView::Probe(text)->SetText(CoreUtils::Convert(""));
        mFiltered = FALSE;
        Boolean showing;
        if (mPopup != NULL && (mPopup->IsShowing(&showing), showing)) {
            DismissPopup();
        }
    }
    return NOERROR;
}

ECode AbsListView::HasTextFilter(
    /* [out] */ Boolean* hasTextFilter)
{
    VALIDATE_NOT_NULL(hasTextFilter);
    *hasTextFilter = mFiltered;
    return NOERROR;
}

ECode AbsListView::OnGlobalLayout()
{
    Boolean res;
    if (IsShown(&res), res) {
        // Show the popup if we are filtered
        if (mFiltered && mPopup != NULL) {
            Boolean showing;
            mPopup->IsShowing(&showing);
            if(!showing && !mPopupHidden) {
                ShowPopup();
            }
        }
    }
    else {
        // Hide the popup when we are no longer visible
        if (mPopup != NULL) {
            Boolean showing;
            mPopup->IsShowing(&showing);
            if(showing) {
                DismissPopup();
            }
        }
    }

    return NOERROR;
}

ECode AbsListView::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode AbsListView::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    Boolean res;
    if (IsTextFilterEnabled(&res), res) {
        CreateTextFilter(TRUE);
        Int32 length;
        s->GetLength(&length);
        Boolean showing;
        mPopup->IsShowing(&showing);
        if (!showing && length > 0) {
            // Show the filter popup if necessary
            ShowPopup();
            mFiltered = TRUE;
        }
        else if (showing && length == 0) {
            // Remove the filter popup if the user has cleared all text
            DismissPopup();
            mFiltered = FALSE;
        }
        AutoPtr<IFilterable> filterable = (IFilterable*)mAdapter->Probe(EIID_IFilterable);
        if (filterable != NULL) {
            AutoPtr<IFilter> f;
            filterable->GetFilter((IFilter**)&f);
            // Filter should not be NULL when we reach this part
            if (f != NULL) {
                f->DoFilter(s, this);
            }
            else {
                Logger::E("AbsListView", "You cannot call onTextChanged with a non filterable adapter.");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode AbsListView::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}

ECode AbsListView::OnFilterComplete(
    /* [in] */ Int32 count)
{
    if (mSelectedPosition < 0 && count > 0) {
        mResurrectToPosition = IAdapterView::INVALID_POSITION;
        ResurrectSelection();
    }
    return NOERROR;
}

ECode AbsListView::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IAbsListViewLayoutParams> temp;
    CAbsListViewLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT, (IAbsListViewLayoutParams**)&temp);
    *result = IViewGroupLayoutParams::Probe(temp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> AbsListView::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<IAbsListViewLayoutParams> temp;
    CAbsListViewLayoutParams::New(p, (IAbsListViewLayoutParams**)&temp);
    AutoPtr<IViewGroupLayoutParams> params = IViewGroupLayoutParams::Probe(temp);
    return params;
}

ECode AbsListView::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IAbsListViewLayoutParams> lp;
    FAIL_RETURN(CAbsListViewLayoutParams::New(context, attrs, (IAbsListViewLayoutParams**)&lp));
    *params = IViewGroupLayoutParams::Probe(lp);
    REFCOUNT_ADD(*params);
    return NOERROR;
}

Boolean AbsListView::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return (p != NULL) && (IAbsListViewLayoutParams::Probe(p) != NULL);
}

ECode AbsListView::SetTranscriptMode(
    /* [in] */ Int32 mode)
{
    mTranscriptMode = mode;

    return NOERROR;
}

ECode AbsListView::GetTranscriptMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mTranscriptMode;
    return NOERROR;
}

ECode AbsListView::GetSolidColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mCacheColorHint;
    return NOERROR;
}

ECode AbsListView::SetCacheColorHint(
    /* [in] */ Int32 color)
{
    if (color != mCacheColorHint) {
        mCacheColorHint = color;
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            child->SetDrawingCacheBackgroundColor(color);
        }
        mRecycler->SetCacheColorHint(color);
    }

    return NOERROR;
}

ECode AbsListView::GetCacheColorHint(
    /* [out] */ Int32* colorHint)
{
    VALIDATE_NOT_NULL(colorHint);
    *colorHint = mCacheColorHint;
    return NOERROR;
}

ECode AbsListView::ReclaimViews(
    /* [in] */ IList* views)
{
    Int32 childCount;
    GetChildCount(&childCount);
    AutoPtr<IRecyclerListener> listener = mRecycler->mRecyclerListener;

    // Reclaim views on screen
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> vglp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&vglp);
        IAbsListViewLayoutParams* lp = IAbsListViewLayoutParams::Probe(vglp);
        // Don't reclaim header or footer views, or views that should be ignored

        if (lp != NULL && mRecycler->ShouldRecycleViewType(
                ((CAbsListViewLayoutParams*)lp)->mViewType)) {
            views->Add(child);
            child->SetAccessibilityDelegate(NULL);
            if (listener != NULL) {
                // Pretend they went through the scrap heap
                listener->OnMovedToScrapHeap(child);
            }
        }
    }
    mRecycler->ReclaimScrapViews(views);
    RemoveAllViewsInLayout();

    return NOERROR;
}

void AbsListView::FinishGlows()
{
    if (mEdgeGlowTop != NULL) {
        mEdgeGlowTop->Finish();
        mEdgeGlowBottom->Finish();
    }
}

ECode AbsListView::SetRemoteViewsAdapter(
    /* [in] */ IIntent* intent)
{
    // Ensure that we don't already have a RemoteViewsAdapter that is bound to an existing
    // service handling the specified intent.
    if (mRemoteAdapter != NULL) {
        AutoPtr<IIntentFilterComparison> fcNew;
        CIntentFilterComparison::New(intent, (IIntentFilterComparison**)&fcNew);
        AutoPtr<IIntentFilterComparison> fcOld;
        AutoPtr<IIntent> adapterIntent;
        mRemoteAdapter->GetRemoteViewsServiceIntent((IIntent**)&adapterIntent);
        CIntentFilterComparison::New(adapterIntent, (IIntentFilterComparison**)&fcOld);
        Boolean equals;
        if (fcNew->Equals(fcOld, &equals), equals) {
            return NOERROR;
        }
    }
    mDeferNotifyDataSetChanged = FALSE;
    // Otherwise, create a new RemoteViewsAdapter for binding
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CRemoteViewsAdapter::New(context, intent, this, (IRemoteViewsAdapter**)&mRemoteAdapter);
    Boolean ready;
    if (mRemoteAdapter->IsDataReady(&ready), ready) {
        SetAdapter(IAdapter::Probe(mRemoteAdapter));
    }
    return NOERROR;
}

ECode AbsListView::SetRemoteViewsOnClickHandler(
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    // Ensure that we don't already have a RemoteViewsAdapter that is bound to an existing
    // service handling the specified intent.
    if (mRemoteAdapter != NULL) {
        mRemoteAdapter->SetRemoteViewsOnClickHandler(handler);
    }
    return NOERROR;
}

ECode AbsListView::DeferNotifyDataSetChanged()
{
    mDeferNotifyDataSetChanged = TRUE;
    return NOERROR;
}

ECode AbsListView::OnRemoteAdapterConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IRemoteViewsAdapter> temp = IRemoteViewsAdapter::Probe(mAdapter);
    if (temp != NULL && mRemoteAdapter != temp) {
        SetAdapter(IAdapter::Probe(mRemoteAdapter));
        if (mDeferNotifyDataSetChanged) {
            IBaseAdapter::Probe(mRemoteAdapter)->NotifyDataSetChanged();
            mDeferNotifyDataSetChanged = FALSE;
        }
        return NOERROR;
    }
    else if (mRemoteAdapter != NULL) {
        mRemoteAdapter->SuperNotifyDataSetChanged();
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode AbsListView::OnRemoteAdapterDisconnected()
{
    // If the remote adapter disconnects, we keep it around
    // since the currently displayed items are still cached.
    // Further, we want the service to eventually reconnect
    // when necessary, as triggered by this view requesting
    // items from the Adapter.
    return NOERROR;
}

void AbsListView::SetVisibleRangeHint(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (mRemoteAdapter != NULL) {
        mRemoteAdapter->SetVisibleRangeHint(start, end);
    }
}

ECode AbsListView::SetRecyclerListener(
    /* [in] */ IRecyclerListener* listener)
{
    mRecycler->mRecyclerListener = listener;

    return NOERROR;
}

Int32 AbsListView::GetHeightForPosition(
    /* [in] */ Int32 position)
{
    Int32 firstVisiblePosition;
    GetFirstVisiblePosition(&firstVisiblePosition);
    Int32 childCount;
    GetChildCount(&childCount);
    const Int32 index = position - firstVisiblePosition;
    if (index >= 0 && index < childCount) {
        // Position is on-screen, use existing view.
        AutoPtr<IView> view;
        GetChildAt(index, (IView**)&view);
        Int32 height;
        view->GetHeight(&height);
        return height;
    }
    else {
        // Position is off-screen, obtain & recycle view.
        AutoPtr<IView> view = ObtainView(position, mIsScrap);
        view->Measure(mWidthMeasureSpec, MeasureSpec::UNSPECIFIED);
        Int32 height;
        view->GetMeasuredHeight(&height);
        mRecycler->AddScrapView(view, position);
        return height;
    }
}

ECode AbsListView::SetSelectionFromTop(
    /* [in] */ Int32 position,
    /* [in] */ Int32 y)
{
    if (mAdapter == NULL) {
        return NOERROR;
    }

    Boolean res;
    if (IsInTouchMode(&res), !res) {
        position = AbsListView::LookForSelectablePosition(position, TRUE);
        if (position >= 0) {
            SetNextSelectedPositionInt(position);
        }
    }
    else {
        mResurrectToPosition = position;
    }

    if (position >= 0) {
        mLayoutMode = LAYOUT_SPECIFIC;

        Int32 topTmp = 0;
        mListPadding->GetTop(&topTmp);
        mSpecificTop = topTmp + y;

        if (mNeedSync) {
            mSyncPosition = position;
            IAdapter::Probe(mAdapter)->GetItemId(position, &mSyncRowId);
        }

        if (mPositionScroller != NULL) {
            mPositionScroller->Stop();
        }
        RequestLayout();
    }
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
