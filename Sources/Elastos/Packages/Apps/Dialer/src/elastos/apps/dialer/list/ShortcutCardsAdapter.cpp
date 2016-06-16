
#include "list/ShortcutCardsAdapter.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

//=================================================================
// ShortcutCardsAdapter::SwipeableShortcutCard
//=================================================================
CAR_INTERFACE_IMPL(ShortcutCardsAdapter::SwipeableShortcutCard, FrameLayout, ISwipeHelperCallback);

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
    FrameLayout::constructor(context);
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
    Float pagingTouchSlop;
    config->GetScaledPagingTouchSlop(&pagingTouchSlop);
    CSwipeHelper::New(context, ISwipeHelper::X, this,
            densityScale, pagingTouchSlop, (SwipeHelper**)&mSwipeHelper);

    return NOERROR;
}

void ShortcutCardsAdapter::SwipeableShortcutCard::PrepareChildView(
    /* [in] */ IView* view)
{
    // Override CallLogAdapter's accessibility behavior; don't expand the shortcut card.
    view->SetAccessibilityDelegate(NULL);
    view->SetBackgroundResource(R::drawable::rounded_corner_bg);

    AutoPtr<IFrameLayoutLayoutParams> params;
    CFrameLayoutLayoutParams::New(
            IFrameLayoutLayoutParams::MATCH_PARENT,
            iFrameLayoutLayoutParams::WRAP_CONTENT,
            (IFrameLayoutLayoutParams**)&params);
    params->SetMargins(
            mCallLogMarginHorizontal,
            mCallLogMarginTop,
            mCallLogMarginHorizontal,
            mCallLogMarginBottom);
    view->SetLayoutParams(params);

    AutoPtr<IView> actionView;
    view->FindViewById(R::id::primary_action_view, (IView**)&actionView);
    Int32 end;
    actionView->GetPaddingEnd(&end);
    ILinearLayout::Probe(actionView)->SetPaddingRelative(
            mCallLogPaddingStart, mCallLogPaddingTop,
            end, mCallLogPaddingBottom);

    // TODO: Set content description including type/location and time information.
    AutoPtr<IView> nameView;
    actionView->FindViewById(R::id::name, (IView**)&nameView);
    AutoPtr<ICharSequence> text;
    ITextView::Probe(nameView)->GetText((ICharSequence**)&text);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    String action;
    res->GetString(R::string::description_call_back_action, text, &action);
    actionView->SetContentDescription(CoreUtils::Convert(action));

    res->FetDimensionPixelSize(
            R::dimen::recent_call_log_item_translation_z, &mPreviousTranslationZ);
    view->SetTranslationZ(mPreviousTranslationZ);

    AutoPtr<IView> temp;
    view->FindViewById(R::id::call_log_list_item, (IView**)&temp);
    CallLogListItemView* callLogItem = ICallLogListItemView::Probe(temp);
    // Reset the internal call log item view if it is being recycled
    callLogItem->SetTranslationX(0);
    callLogItem->SetTranslationY(0);
    callLogItem->SetAlpha(1);
    callLogItem->SetClipBounds(NULL);
    SetChildrenOpacity(callLogItem, 1.0f);

    temp = NULL;
    callLogItem->FindViewById(R::id::call_log_row, (IView**)&temp);
    temp->SetBackgroundColor(mShortCardBackgroundColor);

    temp = NULL;
    callLogItem->FindViewById(R::id::call_indicator_icon, (IView**)&temp);
    temp->SetVisibility(IView::VISIBLE);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** child)
{
    VALUE_NOT_NULL(child);
    Int32 count;
    GetChildCount(&count), count > 0 ? GetChildAt(0, &child) : *child = NULL;
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** child)
{
    VALUE_NOT_NULL(child);
    return v->FindViewById(R::id::call_log_list_item, child);
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnScroll()
{
    return NOERROR;
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
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
    /* [in] */ IMotionEvent ev,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    if (mSwipeHelper != NULL) {
        Boolean helperRes, superRes;
        *result = mSwipeHelper->OnInterceptTouchEvent(ev, &helperRes), helperRes
                || FrameLayout::OnInterceptTouchEvent(ev, &superRes), superRes;
        return NOERROR;
    }
    else {
        return FrameLayout::OnInterceptTouchEvent(ev, result);
    }
}

ECode ShortcutCardsAdapter::SwipeableShortcutCard::OnTouchEvent(
    /* [in] */ IMotionEvent ev,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    if (mSwipeHelper != NULL) {
        Boolean helperRes, superRes;
        *result = mSwipeHelper->OnTouchEvent(ev, &helperRes), helperRes
                || FrameLayout::OnTouchEvent(ev, &superRes), superRes;
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
    return NOERROR;
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

    Int32 z;
    if (ratioHidden <= CLIP_CARD_BARELY_HIDDEN_RATIO) {
        viewToClip->SetTranslationZ(mPreviousTranslationZ);
    }
    else if (viewToClip->GetTranslationZ(&z), z != 0){
        mPreviousTranslationZ = z;
        viewToClip->SetTranslationZ(0);
    }

    if (ratioHidden > CLIP_CARD_MOSTLY_HIDDEN_RATIO) {
        mClipRect->Set(0, 0 , 0, 0);
        SetVisibility(IView::INVISIBLE);
    }
    else {
        SetVisibility(IView::VISIBLE);
        Int32 newTop = (Int32) (ratioHidden * height);
        mClipRect->Set(0, newTop, width, height);

        // Since the pane will be overlapping with the action bar, apply a vertical offset
        // to top align the clipped card in the viewable area;
        viewToClip->SetTranslationY(-newTop);
    }
    viewToClip->SetClipBounds(mClipRect);

    // If the view has any children, fade them out of view.
    IViewGroup* viewGroup = IViewGroup::Probe(viewToClip);
    SetChildrenOpacity(
            viewGroup, Math::Max(0, 1 - (CLIP_CARD_OPACITY_RATIO  * ratioHidden)));
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
CAR_INTERFACE_IMPL(ShortcutCardsAdapter::CallLogOnItemSwipeListener,
            Object, ISwipeHelperOnItemGestureListener)

ShortcutCardsAdapter::CallLogOnItemSwipeListener::CallLogOnItemSwipeListener(
    /* [in] */ ShortcutCardsAdapter* host)
    : mHost(host)
{}

ECode ShortcutCardsAdapter::CallLogOnItemSwipeListener::OnSwipe(
    /* [in] */ IView* view)
{
    mHost->mCallLogQueryHandler->MarkNewCallsAsOld();
    mHost->mCallLogQueryHandler->MarkNewVoicemailsAsOld();
    CallLogNotificationsHelper::removeMissedCallNotifications(mHost->mContext);
    CallLogNotificationsHelper::updateVoicemailNotifications(mHost->mContext);
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
    VALUE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

//=================================================================
// ShortcutCardsAdapter::CallLogQueryHandlerListener
//=================================================================
CAR_INTERFACE_IMPL(ShortcutCardsAdapter::CallLogQueryHandlerListener,
            Object, ICallLogQueryHandlerListener)

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
    VALUE_NOT_NULL(result);
    mHost->mCallLogAdapter->InvalidateCache();
    mHost->mCallLogAdapter->ChangeCursor(combinedCursor);
    mHost->mCallLogAdapter->NotifyDataSetChanged();
    // Return true; took ownership of cursor
    *result = TRUE;
    return NOERROR;
}

//=================================================================
// ShortcutCardsAdapter
//=================================================================
const String ShortcutCardsAdapter::TAG("ShortcutCardsAdapter");
const Float ShortcutCardsAdapter::CLIP_CARD_BARELY_HIDDEN_RATIO = 0.001f;
const Float ShortcutCardsAdapter::CLIP_CARD_MOSTLY_HIDDEN_RATIO = 0.9f;
// Fade out 5x faster than the hidden ratio.
const Float ShortcutCardsAdapter::CLIP_CARD_OPACITY_RATIO = 5f;

CAR_INTERFACE_IMPL(ShortcutCardsAdapter, BaseAdapter, IShortcutCardsAdapter)

ShortcutCardsAdapter::ShortcutCardsAdapter()
{
    mCallLogOnItemSwipeListener = (ISwipeHelperOnItemGestureListener*)
            new CallLogOnItemSwipeListener(this);
    mCallLogQueryHandlerListener = (ICallLogQueryHandlerListener*)
            new CallLogQueryHandlerListener(this);
}

ECode ShortcutCardsAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IListsFragment* fragment,
    /* [in] */ ICallLogAdapter* callLogAdapter)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    mContext = context;
    mFragment = fragment;

    resources->GetDimensionPixelSize(R::dimen::recent_call_log_item_horizontal_clip_limit,
            &mCardMaxHorizontalClip);
    resources->GetDimensionPixelSize(R::dimen::recent_call_log_item_margin_horizontal,
            &mCallLogMarginHorizontal);
    resources->GetDimensionPixelSize(R::dimen::recent_call_log_item_margin_top,
            &mCallLogMarginTop);
    resources->GetDimensionPixelSize(R::dimen::recent_call_log_item_margin_bottom,
            &mCallLogMarginBottom);
    resources->GetDimensionPixelSize(R::dimen::recent_call_log_item_padding_start,
            &mCallLogPaddingStart);
    resources->GetDimensionPixelSize(R::dimen::recent_call_log_item_padding_top,
            &mCallLogPaddingTop);
    resources->GetDimensionPixelSize(R::dimen::recent_call_log_item_padding_bottom,
            &mCallLogPaddingBottom);
    resources->GetColor(R::color::call_log_expanded_background_color,
            &mShortCardBackgroundColor);


    mCallLogAdapter = callLogAdapter;
    mObserver = new CustomDataSetObserver(this);
    mCallLogAdapter->RegisterDataSetObserver((IDataSetObserver*)mObserver);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    CCallLogQueryHandler::New(resolver,
            mCallLogQueryHandlerListener, (ICallLogQueryHandler**)&mCallLogQueryHandler);

    return NOERROR;
}

ECode ShortcutCardsAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALUE_NOT_NULL(count);
    return mCallLogAdapter->GetCount(count);
}

ECode ShortcutCardsAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALUE_NOT_NULL(item);
    return mCallLogAdapter->GetItem(position, item);
}

ECode ShortcutCardsAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64** id)
{
    VALUE_NOT_NULL(id);
    *id = position;
    return NOERROR;
}

ECode ShortcutCardsAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode ShortcutCardsAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALUE_NOT_NULL(count);
    return mCallLogAdapter->GetViewTypeCount(count);
}

ECode ShortcutCardsAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALUE_NOT_NULL(type);
    return mCallLogAdapter->GetItemViewType(position, type);
}

ECode ShortcutCardsAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);
    AutoPtr<SwipeableShortcutCard> wrapper;
    if (convertView == NULL) {
        wrapper = new SwipeableShortcutCard(this);
        wrapper->constructor(mContext);
        wrapper->SetOnItemSwipeListener(mCallLogOnItemSwipeListener);
    }
    else {
        wrapper = (SwipeableShortcutCard*) convertView;
    }

    // Special case wrapper view for the most recent call log item. This allows
    // us to create a card-like effect for the more recent call log item in
    // the PhoneFavoriteMergedAdapter, but keep the original look of the item in
    // the CallLogAdapter.
    AutoPtr<IView> child;
    if (convertView != NULL){
        wrapper->GetChildAt(0, &child);
    }
    AutoPtr<IView> tempView;
    mCallLogAdapter->GetView(
            position, child, parent,(IView**)&tempView);
    wrapper->RemoveAllViews();
    wrapper->PrepareChildView(tempView);
    wrapper->AddView(tempView);
    *view = (IView*)wrapper;
    REFCOUNT_ADD(*view);

    return NOERROR;
}

ECode ShortcutCardsAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALUE_NOT_NULL(enabled);
    return mCallLogAdapter->AreAllItemsEnabled(enabled);
}

ECode ShortcutCardsAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALUE_NOT_NULL(enabled);
    return mCallLogAdapter->IsEnabled(position, enabled);
}

} // List
} // Dialer
} // Apps
} // Elastos
