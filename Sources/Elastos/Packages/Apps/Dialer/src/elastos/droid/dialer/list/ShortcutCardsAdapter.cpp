
#include "elastos/droid/dialer/list/ShortcutCardsAdapter.h"
#include "elastos/droid/dialer/list/ListsFragment.h"
#include "elastos/droid/dialer/calllog/CCallLogListItemView.h"
#include "elastos/droid/dialer/calllog/CallLogNotificationsHelper.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::Droid::Dialer::CallLog::CCallLogListItemView;
using Elastos::Droid::Dialer::CallLog::CallLogNotificationsHelper;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogQueryHandlerListener;
using Elastos::Droid::Dialer::List::EIID_ISwipeHelperCallback;
using Elastos::Droid::Dialer::List::EIID_IShortcutCardsAdapter;
using Elastos::Droid::Dialer::List::EIID_ISwipeHelperOnItemGestureListener;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// ShortcutCardsAdapter::CustomDataSetObserver
//=================================================================
ShortcutCardsAdapter::CustomDataSetObserver::CustomDataSetObserver(
    /* [in] */ ShortcutCardsAdapter* host)
    : mHost(host)
{}

ECode ShortcutCardsAdapter::CustomDataSetObserver::OnChanged()
{
    mHost->NotifyDataSetChanged();
    return NOERROR;
}


//=================================================================
// ShortcutCardsAdapter::CallLogOnItemSwipeListener
//=================================================================
CAR_INTERFACE_IMPL(ShortcutCardsAdapter::CallLogOnItemSwipeListener, Object, ISwipeHelperOnItemGestureListener)

ShortcutCardsAdapter::CallLogOnItemSwipeListener::CallLogOnItemSwipeListener(
    /* [in] */ ShortcutCardsAdapter* host)
    : mHost(host)
{}

ECode ShortcutCardsAdapter::CallLogOnItemSwipeListener::OnSwipe(
    /* [in] */ IView* view)
{
    mHost->mCallLogQueryHandler->MarkNewCallsAsOld();
    mHost->mCallLogQueryHandler->MarkNewVoicemailsAsOld();
    CallLogNotificationsHelper::RemoveMissedCallNotifications(mHost->mContext);
    CallLogNotificationsHelper::UpdateVoicemailNotifications(mHost->mContext);
    mHost->mFragment->DismissShortcut(view);
    return NOERROR;
}

ECode ShortcutCardsAdapter::CallLogOnItemSwipeListener::OnTouch()
{
    return NOERROR;
}

ECode ShortcutCardsAdapter::CallLogOnItemSwipeListener::IsSwipeEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}


//=================================================================
// ShortcutCardsAdapter::CallLogQueryHandlerListener
//=================================================================
CAR_INTERFACE_IMPL(ShortcutCardsAdapter::CallLogQueryHandlerListener, Object, ICallLogQueryHandlerListener)

ShortcutCardsAdapter::CallLogQueryHandlerListener::CallLogQueryHandlerListener(
    /* [in] */ ShortcutCardsAdapter* host)
    : mHost(host)
{}

ECode ShortcutCardsAdapter::CallLogQueryHandlerListener::OnVoicemailStatusFetched(
    /* [in] */ ICursor* statusCursor)
{
    return NOERROR;
}

ECode ShortcutCardsAdapter::CallLogQueryHandlerListener::OnCallsFetched(
    /* [in] */ ICursor* combinedCursor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->mCallLogAdapter->InvalidateCache();
    mHost->mCallLogAdapter->ChangeCursor(combinedCursor);
    mHost->mCallLogAdapter->NotifyDataSetChanged();
    // Return true; took ownership of cursor
    *result = TRUE;
    return NOERROR;
}


//=================================================================
// ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback
//=================================================================
CAR_INTERFACE_IMPL(ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback, Object, ISwipeHelperCallback)

ECode ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** child)
{
    return mHost->GetChildAtPosition(ev, child);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** child)
{
    return mHost->GetChildContentView(v, child);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback::OnScroll()
{
    return mHost->OnScroll();
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    return mHost->CanChildBeDismissed(v, result);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback::OnBeginDrag(
    /* [in] */ IView* v)
{
    return mHost->OnBeginDrag(v);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback::OnChildDismissed(
    /* [in] */ IView* v)
{
    return mHost->OnChildDismissed(v);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::InnerCallback::OnDragCancelled(
    /* [in] */ IView* v)
{
    return mHost->OnDragCancelled(v);
}


//=================================================================
// ShortcutCardsAdapter::SwipeableShortcutCard
//=================================================================
ShortcutCardsAdapter::SwipeableShortcutCard::SwipeableShortcutCard(
    /* [in] */ ShortcutCardsAdapter* host)
    : mHost(host)
    , mPreviousTranslationZ(0)
{
    CRect::New((IRect**)&mClipRect);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(FrameLayout::constructor(context))
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float densityScale;
    metrics->GetDensity(&densityScale);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> config;
    helper->Get(context, (IViewConfiguration**)&config);
    Int32 pagingTouchSlop;
    config->GetScaledPagingTouchSlop(&pagingTouchSlop);
    AutoPtr<ISwipeHelperCallback> cb = (ISwipeHelperCallback*)new InnerCallback(this);
    mSwipeHelper = new SwipeHelper(context, SwipeHelper::X, cb, densityScale, pagingTouchSlop);

    return NOERROR;
}

void ShortcutCardsAdapter::SwipeableShortcutCard::PrepareChildView(
    /* [in] */ IView* view)
{
    // Override CallLogAdapter's accessibility behavior; don't expand the shortcut card.
    view->SetAccessibilityDelegate(NULL);
    view->SetBackgroundResource(Elastos::Droid::Dialer::R::drawable::rounded_corner_bg);

    AutoPtr<IFrameLayoutLayoutParams> params;
    CFrameLayoutLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT,
            (IFrameLayoutLayoutParams**)&params);
    IViewGroupMarginLayoutParams::Probe(params)->SetMargins(
            mHost->mCallLogMarginHorizontal,
            mHost->mCallLogMarginTop,
            mHost->mCallLogMarginHorizontal,
            mHost->mCallLogMarginBottom);
    view->SetLayoutParams(IViewGroupLayoutParams::Probe(params));

    AutoPtr<IView> actionView;
    view->FindViewById(Elastos::Droid::Dialer::R::id::primary_action_view, (IView**)&actionView);
    Int32 end;
    actionView->GetPaddingEnd(&end);
    IView::Probe(actionView)->SetPaddingRelative(
            mHost->mCallLogPaddingStart, mHost->mCallLogPaddingTop, end, mHost->mCallLogPaddingBottom);

    // TODO: Set content description including type/location and time information.
    AutoPtr<IView> nameView;
    actionView->FindViewById(Elastos::Droid::Dialer::R::id::name, (IView**)&nameView);
    AutoPtr<ICharSequence> text;
    ITextView::Probe(nameView)->GetText((ICharSequence**)&text);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    String action;
    AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(1);
    attrs->Set(0, text);
    res->GetString(Elastos::Droid::Dialer::R::string::description_call_back_action, attrs, &action);
    actionView->SetContentDescription(CoreUtils::Convert(action));

    res->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::dimen::recent_call_log_item_translation_z, (Int32*)&mPreviousTranslationZ);
    view->SetTranslationZ(mPreviousTranslationZ);

    AutoPtr<IView> callLogItem;
    view->FindViewById(Elastos::Droid::Dialer::R::id::call_log_list_item, (IView**)&callLogItem);
    // Reset the internal call log item view if it is being recycled
    callLogItem->SetTranslationX(0);
    callLogItem->SetTranslationY(0);
    callLogItem->SetAlpha(1);
    callLogItem->SetClipBounds(NULL);
    SetChildrenOpacity(IViewGroup::Probe(callLogItem), 1.0f);

    AutoPtr<IView> temp;
    callLogItem->FindViewById(Elastos::Droid::Dialer::R::id::call_log_row, (IView**)&temp);
    temp->SetBackgroundColor(mHost->mShortCardBackgroundColor);

    temp = NULL;
    callLogItem->FindViewById(Elastos::Droid::Dialer::R::id::call_indicator_icon, (IView**)&temp);
    temp->SetVisibility(IView::VISIBLE);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** child)
{
    VALIDATE_NOT_NULL(child)
    *child = NULL;
    Int32 count;
    GetChildCount(&count);
    if (count > 0) {
        return GetChildAt(0, child);
    }
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** child)
{
    VALIDATE_NOT_NULL(child)
    return v->FindViewById(Elastos::Droid::Dialer::R::id::call_log_list_item, child);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnScroll()
{
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnBeginDrag(
    /* [in] */ IView* v)
{
    // We do this so the underlying ScrollView knows that it won't get
    // the chance to intercept events anymore
    RequestDisallowInterceptTouchEvent(TRUE);
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnChildDismissed(
    /* [in] */ IView* v)
{
    if (v != NULL && mOnItemSwipeListener != NULL) {
        mOnItemSwipeListener->OnSwipe(v);
    }
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnDragCancelled(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSwipeHelper != NULL) {
        Boolean superRes;
        *result = mSwipeHelper->OnInterceptTouchEvent(ev)
                || (FrameLayout::OnInterceptTouchEvent(ev, &superRes), superRes);
        return NOERROR;
    }
    else {
        return FrameLayout::OnInterceptTouchEvent(ev, result);
    }
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSwipeHelper != NULL) {
        Boolean superRes;
        *result = mSwipeHelper->OnTouchEvent(ev)
                || (FrameLayout::OnTouchEvent(ev, &superRes), superRes);
        return NOERROR;
    }
    else {
        return FrameLayout::OnTouchEvent(ev, result);
    }
}

void ShortcutCardsAdapter::SwipeableShortcutCard::SetOnItemSwipeListener(
    /* [in] */ ISwipeHelperOnItemGestureListener* listener)
{
    mOnItemSwipeListener = listener;
}

void ShortcutCardsAdapter::SwipeableShortcutCard::ClipCard(
    /* [in] */ Float ratioHidden)
{
    AutoPtr<IView> viewToClip;
    GetChildAt(0, (IView**)&viewToClip);
    if (viewToClip == NULL) {
        return;
    }
    Int32 width;
    viewToClip->GetWidth(&width);
    Int32 height;
    viewToClip->GetHeight(&height);

    Float z;
    if (ratioHidden <= CLIP_CARD_BARELY_HIDDEN_RATIO) {
        viewToClip->SetTranslationZ(mPreviousTranslationZ);
    }
    else if (viewToClip->GetTranslationZ(&z), z != 0){
        viewToClip->GetTranslationZ(&mPreviousTranslationZ);
        viewToClip->SetTranslationZ(0);
    }

    if (ratioHidden > CLIP_CARD_MOSTLY_HIDDEN_RATIO) {
        mClipRect->Set(0, 0 , 0, 0);
        SetVisibility(IView::INVISIBLE);
    }
    else {
        SetVisibility(IView::VISIBLE);
        Int32 newTop = (Int32)(ratioHidden * height);
        mClipRect->Set(0, newTop, width, height);

        // Since the pane will be overlapping with the action bar, apply a vertical offset
        // to top align the clipped card in the viewable area;
        viewToClip->SetTranslationY(-newTop);
    }
    viewToClip->SetClipBounds(mClipRect);

    // If the view has any children, fade them out of view.
    AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(viewToClip);
    SetChildrenOpacity(
            viewGroup, Elastos::Core::Math::Max(0.0, 1.0 - (CLIP_CARD_OPACITY_RATIO  * ratioHidden)));
}

void ShortcutCardsAdapter::SwipeableShortcutCard::SetChildrenOpacity(
    /* [in] */ IViewGroup* viewGroup,
    /* [in] */ Float alpha)
{
    Int32 count;
    viewGroup->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        viewGroup->GetChildAt(i, (IView**)&child);
        child->SetAlpha(alpha);
    }
}


//=================================================================
// ShortcutCardsAdapter
//=================================================================
const String ShortcutCardsAdapter::TAG("ShortcutCardsAdapter");
const Float ShortcutCardsAdapter::CLIP_CARD_BARELY_HIDDEN_RATIO;
const Float ShortcutCardsAdapter::CLIP_CARD_MOSTLY_HIDDEN_RATIO;
const Float ShortcutCardsAdapter::CLIP_CARD_OPACITY_RATIO;

CAR_INTERFACE_IMPL(ShortcutCardsAdapter, BaseAdapter, IShortcutCardsAdapter)

ShortcutCardsAdapter::ShortcutCardsAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ListsFragment* fragment,
    /* [in] */ CallLogAdapter* callLogAdapter)
    : mCallLogMarginHorizontal(0)
    , mCallLogMarginTop(0)
    , mCallLogMarginBottom(0)
    , mCallLogPaddingStart(0)
    , mCallLogPaddingTop(0)
    , mCallLogPaddingBottom(0)
    , mCardMaxHorizontalClip(0)
    , mShortCardBackgroundColor(0)
{
    mCallLogOnItemSwipeListener = (ISwipeHelperOnItemGestureListener*)new CallLogOnItemSwipeListener(this);
    mCallLogQueryHandlerListener = (ICallLogQueryHandlerListener*)new CallLogQueryHandlerListener(this);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    mContext = context;
    mFragment = fragment;

    resources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::recent_call_log_item_horizontal_clip_limit,
            &mCardMaxHorizontalClip);
    resources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::recent_call_log_item_margin_horizontal,
            &mCallLogMarginHorizontal);
    resources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::recent_call_log_item_margin_top,
            &mCallLogMarginTop);
    resources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::recent_call_log_item_margin_bottom,
            &mCallLogMarginBottom);
    resources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::recent_call_log_item_padding_start,
            &mCallLogPaddingStart);
    resources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::recent_call_log_item_padding_top,
            &mCallLogPaddingTop);
    resources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::recent_call_log_item_padding_bottom,
            &mCallLogPaddingBottom);
    resources->GetColor(Elastos::Droid::Dialer::R::color::call_log_expanded_background_color,
            &mShortCardBackgroundColor);

    mCallLogAdapter = callLogAdapter;
    mObserver = new CustomDataSetObserver(this);
    mCallLogAdapter->RegisterDataSetObserver((IDataSetObserver*)mObserver);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    mCallLogQueryHandler = new CallLogQueryHandler();
    mCallLogQueryHandler->constructor(resolver, mCallLogQueryHandlerListener);
}

ECode ShortcutCardsAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mCallLogAdapter->GetCount(count);
}

ECode ShortcutCardsAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    return mCallLogAdapter->GetItem(position, item);
}

ECode ShortcutCardsAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = position;
    return NOERROR;
}

ECode ShortcutCardsAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode ShortcutCardsAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mCallLogAdapter->GetViewTypeCount(count);
}

ECode ShortcutCardsAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    return mCallLogAdapter->GetItemViewType(position, type);
}

ECode ShortcutCardsAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;
    AutoPtr<SwipeableShortcutCard> wrapper;
    if (convertView == NULL) {
        wrapper = new SwipeableShortcutCard(this);
        wrapper->constructor(mContext);
        wrapper->SetOnItemSwipeListener(mCallLogOnItemSwipeListener);
    }
    else {
        wrapper = (SwipeableShortcutCard*)convertView;
    }

    // Special case wrapper view for the most recent call log item. This allows
    // us to create a card-like effect for the more recent call log item in
    // the PhoneFavoriteMergedAdapter, but keep the original look of the item in
    // the CallLogAdapter.
    AutoPtr<IView> child;
    if (convertView != NULL){
        wrapper->GetChildAt(0, (IView**)&child);
    }
    AutoPtr<IView> tempView;
    mCallLogAdapter->GetView(position, child, parent, (IView**)&tempView);
    wrapper->RemoveAllViews();
    wrapper->PrepareChildView(tempView);
    wrapper->AddView(tempView);
    *view = IView::Probe(wrapper);
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode ShortcutCardsAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    return mCallLogAdapter->AreAllItemsEnabled(enabled);
}

ECode ShortcutCardsAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    return mCallLogAdapter->IsEnabled(position, enabled);
}

} // List
} // Dialer
} // Droid
} // Elastos
