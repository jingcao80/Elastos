
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/internal/widget/CAbsListViewAutoScroller.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/view/animation/CAccelerateDecelerateInterpolator.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/widget/CLinearLayout.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/widget/ListPopupWindow.h"
#include "elastos/droid/widget/CPopupWindow.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Internal::Widget::CAbsListViewAutoScroller;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateDecelerateInterpolator;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::MotionEvent;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::EIID_IViewOnAttachStateChangeListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//        ListPopupWindow::ForwardingListener::DisallowIntercept
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::ForwardingListener::DisallowIntercept, Object, IRunnable)

ListPopupWindow::ForwardingListener::DisallowIntercept::DisallowIntercept(
    /* [in] */ ForwardingListener* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::ForwardingListener::DisallowIntercept::Run()
{
    AutoPtr<IViewParent> viewParent;
    mOwner->mSrc->GetParent((IViewParent**)&viewParent);
    return viewParent->RequestDisallowInterceptTouchEvent(TRUE);
}


//=====================================================================
//        ListPopupWindow::ForwardingListener::TriggerLongPress
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::ForwardingListener::TriggerLongPress, Object, IRunnable)

ListPopupWindow::ForwardingListener::TriggerLongPress::TriggerLongPress(
    /* [in] */ ForwardingListener* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::ForwardingListener::TriggerLongPress::Run()
{
    mOwner->OnLongPress();
    return NOERROR;
}


//=====================================================================
//                 ListPopupWindow::ForwardingListener
//=====================================================================
CAR_INTERFACE_IMPL_2(ListPopupWindow::ForwardingListener, Object, IViewOnTouchListener, IViewOnAttachStateChangeListener)

ListPopupWindow::ForwardingListener::ForwardingListener()
{}

ECode ListPopupWindow::ForwardingListener::constructor(
    /* [in] */ IView* src)
{
    mSrc = src;
    AutoPtr<IContext> context;
    src->GetContext((IContext**)&context);
    Int32 scaledTouchSlop;
    ViewConfiguration::Get(context)->GetScaledTouchSlop(&scaledTouchSlop);
    mScaledTouchSlop = scaledTouchSlop;
    mTapTimeout = ViewConfiguration::GetTapTimeout();

    // Use a medium-press timeout. Halfway between tap and long-press.
    mLongPressTimeout = (mTapTimeout + ViewConfiguration::GetLongPressTimeout()) / 2;

    src->AddOnAttachStateChangeListener(this);
    return NOERROR;
}

ECode ListPopupWindow::ForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** result)
{
    // abstract function, nothing to do here
    return NOERROR;
}

ECode ListPopupWindow::ForwardingListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean wasForwarding = mForwarding;
    Boolean forwarding = FALSE;
    if (wasForwarding) {
        if (mWasLongPress) {
            // If we started forwarding as a result of a long-press,
            // just silently stop forwarding events so that the window
            // stays open.
            forwarding = OnTouchForwarded(event);
        }
        else {
            forwarding = OnTouchForwarded(event) || !OnForwardingStopped();
        }
    }
    else {
        forwarding = OnTouchObserved(event) && OnForwardingStarted();

        if (forwarding) {
            // Make sure we cancel any ongoing source event stream.
            Int64 now = SystemClock::GetUptimeMillis();
            AutoPtr<IMotionEvent> e;
            MotionEvent::Obtain(now, now, IMotionEvent::ACTION_CANCEL,
                    0.0f, 0.0f, 0, (IMotionEvent**)&e);
            Boolean resTmp;
            mSrc->OnTouchEvent(e, &resTmp);
            IInputEvent::Probe(e)->Recycle();
        }
    }

    mForwarding = forwarding;
    *result = forwarding || wasForwarding;
    return NOERROR;
}

ECode ListPopupWindow::ForwardingListener::OnViewAttachedToWindow(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode ListPopupWindow::ForwardingListener::OnViewDetachedFromWindow(
    /* [in] */ IView* v)
{
    mForwarding = FALSE;
    mActivePointerId = IMotionEvent::INVALID_POINTER_ID;

    if (mDisallowIntercept != NULL) {
        Boolean resTmp;
        mSrc->RemoveCallbacks(mDisallowIntercept, &resTmp);
    }
    return NOERROR;
}

Boolean ListPopupWindow::ForwardingListener::OnForwardingStarted()
{
    AutoPtr<IListPopupWindow> popup;
    GetPopup((IListPopupWindow**)&popup);
    Boolean isShowing = FALSE;
    if (popup != NULL && (popup->IsShowing(&isShowing), !isShowing)) {
        popup->Show();
    }
    return TRUE;
}

Boolean ListPopupWindow::ForwardingListener::OnForwardingStopped()
{
    AutoPtr<IListPopupWindow> popup;
    GetPopup((IListPopupWindow**)&popup);
    Boolean isShowing = FALSE;
    if (popup != NULL && (popup->IsShowing(&isShowing), !isShowing)) {
        popup->Dismiss();
    }
    return TRUE;
}

Boolean ListPopupWindow::ForwardingListener::OnTouchObserved(
    /* [in] */ IMotionEvent* srcEvent)
{
    AutoPtr<IView> src = mSrc;
    Boolean isEnabled = FALSE;
    if (src->IsEnabled(&isEnabled), !isEnabled) {
        return FALSE;
    }

    Int32 actionMasked = 0;
    srcEvent->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN:
            {
                srcEvent->GetPointerId(0, &mActivePointerId);
                mWasLongPress = FALSE;

                if (mDisallowIntercept == NULL) {
                    mDisallowIntercept = new DisallowIntercept(this);
                }
                Boolean resTmp;
                src->PostDelayed(mDisallowIntercept, mTapTimeout, &resTmp);

                if (mTriggerLongPress == NULL) {
                    mTriggerLongPress = new TriggerLongPress(this);
                }
                src->PostDelayed(mTriggerLongPress, mLongPressTimeout, &resTmp);
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            {
                Int32 activePointerIndex = 0;
                srcEvent->FindPointerIndex(mActivePointerId, &activePointerIndex);
                if (activePointerIndex >= 0) {
                    Float x, y;
                    srcEvent->GetX(activePointerIndex, &x);
                    srcEvent->GetY(activePointerIndex, &y);

                    // Has the pointer has moved outside of the view?
                    Elastos::Droid::View::View* viewTmp = (Elastos::Droid::View::View*)src.Get();
                    Boolean resTmp = FALSE;
                    if (viewTmp->PointInView(x, y, mScaledTouchSlop, &resTmp), !resTmp) {
                        ClearCallbacks();

                        // Don't let the parent intercept our events.
                        AutoPtr<IViewParent> viewParent;
                        src->GetParent((IViewParent**)&viewParent);
                        viewParent->RequestDisallowInterceptTouchEvent(TRUE);
                        return TRUE;
                    }
                }
            }
            break;
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            ClearCallbacks();
            break;
    }
    return FALSE;
}

void ListPopupWindow::ForwardingListener::ClearCallbacks()
{
    Boolean resTmp;
    if (mTriggerLongPress != NULL) {
        mSrc->RemoveCallbacks(mTriggerLongPress, &resTmp);
    }

    if (mDisallowIntercept != NULL) {
        mSrc->RemoveCallbacks(mDisallowIntercept, &resTmp);
    }
}

void ListPopupWindow::ForwardingListener::OnLongPress()
{
    ClearCallbacks();

    AutoPtr<IView> src = mSrc;
    Boolean isEnabled = FALSE;
    src->IsEnabled(&isEnabled);
    if (!isEnabled) {
        return;
    }

    if (!OnForwardingStarted()) {
        return;
    }

    // Don't let the parent intercept our events.
    AutoPtr<IViewParent> viewParent;
    mSrc->GetParent((IViewParent**)&viewParent);
    viewParent->RequestDisallowInterceptTouchEvent(TRUE);

    // Make sure we cancel any ongoing source event stream.
    Int64 now = SystemClock::GetUptimeMillis();
    AutoPtr<IMotionEvent> e;
    MotionEvent::Obtain(now, now, IMotionEvent::ACTION_CANCEL, 0.0f, 0.0f, 0, (IMotionEvent**)&e);
    Boolean resTmp;
    mSrc->OnTouchEvent(e, &resTmp);
    IInputEvent::Probe(e)->Recycle();

    mForwarding = TRUE;
    mWasLongPress = TRUE;
}

Boolean ListPopupWindow::ForwardingListener::OnTouchForwarded(
    /* [in] */ IMotionEvent* srcEvent)
{
    AutoPtr<IView> src = mSrc;
    AutoPtr<IListPopupWindow> popup;
    GetPopup((IListPopupWindow**)&popup);
    Boolean isShowing = FALSE;
    if (popup == NULL || (popup->IsShowing(&isShowing), !isShowing)) {
        return FALSE;
    }

    AutoPtr<DropDownListView> dst = ((ListPopupWindow*)popup.Get())->mDropDownList;
    Boolean isShown = FALSE;
    if (dst == NULL || (dst->IsShown(&isShown), !isShown)) {
        return FALSE;
    }

    // Convert event to destination-local coordinates.
    AutoPtr<IMotionEvent> dstEvent;
    MotionEvent::ObtainNoHistory(srcEvent, (IMotionEvent**)&dstEvent);

    Boolean resTmp;
    src->ToGlobalMotionEvent(dstEvent, &resTmp);
    dst->ToLocalMotionEvent(dstEvent, &resTmp);

    // Forward converted event to destination view, then recycle it.
    Boolean handled = FALSE;
    dst->OnForwardedEvent(dstEvent, mActivePointerId, &handled);
    IInputEvent::Probe(dstEvent)->Recycle();

    // Always cancel forwarding when the touch stream ends.
    Int32 action = 0;
    srcEvent->GetActionMasked(&action);
    Boolean keepForwarding = action != IMotionEvent::ACTION_UP
            && action != IMotionEvent::ACTION_CANCEL;

    return handled && keepForwarding;
}


//=====================================================================
//              ListPopupWindow::InnerForwardingListener
//=====================================================================
ListPopupWindow::InnerForwardingListener::InnerForwardingListener(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::InnerForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


//=====================================================================
//   ListPopupWindow::DropDownListView::InnerAnimatorListenerAdapter
//=====================================================================
ListPopupWindow::DropDownListView::InnerAnimatorListenerAdapter::InnerAnimatorListenerAdapter(
    /* [in] */ DropDownListView* owner,
    /* [in] */ IView* child,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
    : mOwner(owner)
    , mChild(child)
    , mPosition(position)
    , mId(id)
{}

ECode ListPopupWindow::DropDownListView::InnerAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    Boolean resTmp;
    return mOwner->PerformItemClick(mChild, mPosition, mId, &resTmp);
}


//=====================================================================
//                  ListPopupWindow::DropDownListView
//=====================================================================
const Int64 ListPopupWindow::DropDownListView::CLICK_ANIM_DURATION;
const Int32 ListPopupWindow::DropDownListView::CLICK_ANIM_ALPHA;
const AutoPtr<Int32Property> ListPopupWindow::DropDownListView::DRAWABLE_ALPHA = new ListPopupWindow::InnerInt32Property(String("alpha"));

ListPopupWindow::DropDownListView::DropDownListView()
    : mListSelectionHidden(FALSE)
    , mHijackFocus(FALSE)
    , mDrawsInPressedState(FALSE)
{}

ECode ListPopupWindow::DropDownListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean hijackFocus)
{
    FAIL_RETURN(ListView::constructor(context, NULL, R::attr::dropDownListViewStyle));
    mHijackFocus = hijackFocus;
    // TODO: Add an API to control this
    SetCacheColorHint(0); // Transparent, since the background drawable could be anything.
    return NOERROR;
}

ECode ListPopupWindow::DropDownListView::OnForwardedEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 activePointerId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean handledEvent = TRUE;
    Boolean clearPressedItem = FALSE;

    Int32 actionMasked = 0;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_CANCEL:
            handledEvent = FALSE;
            break;
        case IMotionEvent::ACTION_UP:
            handledEvent = FALSE;
            // $FALL-THROUGH$
        case IMotionEvent::ACTION_MOVE:
            {
                Int32 activeIndex = 0;
                event->FindPointerIndex(activePointerId, &activeIndex);
                if (activeIndex < 0) {
                    handledEvent = FALSE;
                    break;
                }

                Float xTmp, yTmp;
                event->GetX(activeIndex, &xTmp);
                event->GetY(activeIndex, &yTmp);
                Int32 x = (Int32) xTmp;
                Int32 y = (Int32) yTmp;
                Int32 position = 0;
                PointToPosition(x, y, &position);
                if (position == INVALID_POSITION) {
                    clearPressedItem = TRUE;
                    break;
                }

                Int32 visiblePosition = 0;
                GetFirstVisiblePosition(&visiblePosition);
                AutoPtr<IView> child;
                GetChildAt(position - visiblePosition, (IView**)&child);
                SetPressedItem(child, position, x, y);
                handledEvent = TRUE;

                if (actionMasked == IMotionEvent::ACTION_UP) {
                    ClickPressedItem(child, position);
                }
            }
            break;
    }

    // Failure to handle the event cancels forwarding.
    if (!handledEvent || clearPressedItem) {
        ClearPressedItem();
    }

    // Manage automatic scrolling.
    if (handledEvent) {
        if (mScrollHelper == NULL) {
            CAbsListViewAutoScroller::New(this, (IAbsListViewAutoScroller**)&mScrollHelper);
        }
        IView::Probe(mScrollHelper)->SetEnabled(TRUE);
        Boolean resTmp;
        IViewOnTouchListener::Probe(mScrollHelper)->OnTouch(this, event, &resTmp);
    }
    else if (mScrollHelper != NULL) {
        IView::Probe(mScrollHelper)->SetEnabled(FALSE);
    }

    *result = handledEvent;
    return NOERROR;
}

Boolean ListPopupWindow::DropDownListView::TouchModeDrawsInPressedState()
{
    return mDrawsInPressedState || ListView::TouchModeDrawsInPressedState();
}

AutoPtr<IView> ListPopupWindow::DropDownListView::ObtainView(
    /* [in] */ Int32 position,
    /* [in] */ ArrayOf<Boolean>* isScrap)
{
    AutoPtr<IView> view = ListView::ObtainView(position, isScrap);

    ITextView* textView = ITextView::Probe(view);
    if (textView) {
        textView->SetHorizontallyScrolling(TRUE);
    }
    return view;
}

ECode ListPopupWindow::DropDownListView::IsInTouchMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // WARNING: Please read the comment where mListSelectionHidden is declared
    Boolean resTmp = FALSE;
    *result = (mHijackFocus && mListSelectionHidden) || (ListView::IsInTouchMode(&resTmp), resTmp);
    return NOERROR;
}

ECode ListPopupWindow::DropDownListView::HasWindowFocus(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean resTmp = FALSE;
    *result = mHijackFocus || (ListView::HasWindowFocus(&resTmp), resTmp);
    return NOERROR;
}

ECode ListPopupWindow::DropDownListView::IsFocused(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean resTmp = FALSE;
    *result = mHijackFocus || (ListView::IsFocused(&resTmp), resTmp);
    return NOERROR;
}

ECode ListPopupWindow::DropDownListView::HasFocus(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean resTmp = FALSE;
    *result = mHijackFocus || (ListView::HasFocus(&resTmp), resTmp);
    return NOERROR;
}

void ListPopupWindow::DropDownListView::ClickPressedItem(
    /* [in] */ IView* child,
    /* [in] */ Int32 position)
{
    Int64 id = 0;
    GetItemIdAtPosition(position, &id);

    AutoPtr< ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(3);
    values->Set(0, 0xFF);
    values->Set(1, CLICK_ANIM_ALPHA);
    values->Set(2, 0xFF);
    AutoPtr<IObjectAnimator> animTmp = CObjectAnimator::OfInt32(
            mSelector.Get(), IProperty::Probe(DRAWABLE_ALPHA), values);
    IAnimator* anim = IAnimator::Probe(animTmp);
    anim->SetDuration(CLICK_ANIM_DURATION);
    AutoPtr<ITimeInterpolator> interpolator;
    CAccelerateDecelerateInterpolator::New((ITimeInterpolator**)&interpolator);
    anim->SetInterpolator(interpolator);
    AutoPtr<IAnimatorListener> listener = new InnerAnimatorListenerAdapter(this, child, position, id);
    anim->AddListener(listener);
    anim->Start();

    if (mClickAnimation != NULL) {
        mClickAnimation->Cancel();
    }
    mClickAnimation = anim;
}

void ListPopupWindow::DropDownListView::ClearPressedItem()
{
    mDrawsInPressedState = FALSE;
    SetPressed(FALSE);
    UpdateSelectorState();

    if (mClickAnimation != NULL) {
        mClickAnimation->Cancel();
        mClickAnimation = NULL;
    }
}

void ListPopupWindow::DropDownListView::SetPressedItem(
    /* [in] */ IView* child,
    /* [in] */ Int32 position,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mDrawsInPressedState = TRUE;

    // Ordering is essential. First update the pressed state and layout
    // the children. This will ensure the selector actually gets drawn.
    SetPressed(TRUE);
    LayoutChildren();

    // Ensure that keyboard focus starts from the last touched position.
    SetSelectedPositionInt(position);
    PositionSelectorLikeTouch(position, child, x, y);

    // Refresh the drawable state to reflect the new pressed state,
    // which will also update the selector state.
    RefreshDrawableState();

    if (mClickAnimation != NULL) {
        mClickAnimation->Cancel();
        mClickAnimation = NULL;
    }
}


//=====================================================================
//                  ListPopupWindow::InnerInt32Property
//=====================================================================
ListPopupWindow::InnerInt32Property::InnerInt32Property(
    /* [in] */ const String& property)
    : Int32Property(property)
{}

ECode ListPopupWindow::InnerInt32Property::SetValue(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 value)
{
    IDrawable::Probe(object)->SetAlpha(value);
    return NOERROR;
}

ECode ListPopupWindow::InnerInt32Property::Get(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 alpha = 0;
    IDrawable::Probe(object)->GetAlpha(&alpha);
    AutoPtr<IInteger32> resTmp = CoreUtils::Convert(alpha);
    *result = resTmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


//=====================================================================
//                ListPopupWindow::PopupDataSetObserver
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::PopupDataSetObserver, Object, IDataSetObserver)

ListPopupWindow::PopupDataSetObserver::PopupDataSetObserver(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::PopupDataSetObserver::OnChanged()
{
    Boolean isShowing = FALSE;
    mOwner->IsShowing(&isShowing);
    if (isShowing) {
        mOwner->Show();
    }
    return NOERROR;
}

ECode ListPopupWindow::PopupDataSetObserver::OnInvalidated()
{
    mOwner->Dismiss();
    return NOERROR;
}


//=====================================================================
//                  ListPopupWindow::ListSelectorHider
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::ListSelectorHider, Object, IRunnable)

ListPopupWindow::ListSelectorHider::ListSelectorHider(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::ListSelectorHider::Run()
{
    mOwner->ClearListSelection();
    return NOERROR;
}


//=====================================================================
//                 ListPopupWindow::ResizePopupRunnable
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::ResizePopupRunnable, Object, IRunnable)

ListPopupWindow::ResizePopupRunnable::ResizePopupRunnable(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::ResizePopupRunnable::Run()
{
    if (mOwner->mDropDownList != NULL) {
        Int32 count = 0;
        mOwner->mDropDownList->GetCount(&count);
        Int32 childCount = 0;
        mOwner->mDropDownList->GetChildCount(&childCount);
        if (count > childCount && childCount <= mOwner->mListItemExpandMaximum) {
            mOwner->mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
            mOwner->Show();
        }
    }
    return NOERROR;
}


//=====================================================================
//                ListPopupWindow::PopupTouchInterceptor
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::PopupTouchInterceptor, Object, IViewOnTouchListener)

ListPopupWindow::PopupTouchInterceptor::PopupTouchInterceptor(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::PopupTouchInterceptor::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 action = 0;
    event->GetAction(&action);
    Float fx = 0, fy = 0;
    event->GetX(&fx);
    event->GetY(&fy);
    Int32 x = (Int32)fx;
    Int32 y = (Int32)fy;

    Int32 width = 0, height = 0;
    Boolean isShowing = FALSE;
    if (action == IMotionEvent::ACTION_DOWN &&
            mOwner->mPopup != NULL && (mOwner->mPopup->IsShowing(&isShowing), isShowing) &&
            (x >= 0 && x < (mOwner->mPopup->GetWidth(&width), width) &&
                y >= 0 && y < (mOwner->mPopup->GetHeight(&height), height))) {
        Boolean result;
        mOwner->mHandler->PostDelayed(mOwner->mResizePopupRunnable, EXPAND_LIST_TIMEOUT, &result);
    }
    else if (action == IMotionEvent::ACTION_UP) {
        mOwner->mHandler->RemoveCallbacks(mOwner->mResizePopupRunnable);
    }
    *result = FALSE;
    return NOERROR;
}


//=====================================================================
//                 ListPopupWindow::PopupScrollListener
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::PopupScrollListener, Object, IAbsListViewOnScrollListener)

ListPopupWindow::PopupScrollListener::PopupScrollListener(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::PopupScrollListener::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    return NOERROR;
}

ECode ListPopupWindow::PopupScrollListener::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> v;
    Boolean isNeeded = FALSE;
    if (scrollState == IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL &&
            (mOwner->IsInputMethodNotNeeded(&isNeeded), !isNeeded) &&
            (mOwner->mPopup->GetContentView((IView**)&v), v != NULL)) {
        mOwner->mHandler->RemoveCallbacks(mOwner->mResizePopupRunnable);
        mOwner->mResizePopupRunnable->Run();
    }
    return NOERROR;
}


//=====================================================================
//                 ListPopupWindow::InnerShowRunnable
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::InnerShowRunnable, Object, IRunnable)

ListPopupWindow::InnerShowRunnable::InnerShowRunnable(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::InnerShowRunnable::Run()
{
    AutoPtr<IView> view;
    AutoPtr<IBinder> token;
    if ((mOwner->GetAnchorView((IView**)&view), view != NULL) &&
            (view->GetWindowToken((IBinder**)&token), token != NULL)) {
        mOwner->Show();
    }
    return NOERROR;
}


//=====================================================================
//         ListPopupWindow::InnerAdapterViewOnItemSelectedListener
//=====================================================================
CAR_INTERFACE_IMPL(ListPopupWindow::InnerAdapterViewOnItemSelectedListener, Object, IRunnable)

ListPopupWindow::InnerAdapterViewOnItemSelectedListener::InnerAdapterViewOnItemSelectedListener(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{}

ECode ListPopupWindow::InnerAdapterViewOnItemSelectedListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (position != -1) {
        DropDownListView* dropDownList = mOwner->mDropDownList;

        if (dropDownList != NULL) {
            dropDownList->mListSelectionHidden = FALSE;
        }
    }
    return NOERROR;
}

ECode ListPopupWindow::InnerAdapterViewOnItemSelectedListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return NOERROR;
}


//=====================================================================
//                           ListPopupWindow
//=====================================================================
const Int32 ListPopupWindow::POSITION_PROMPT_ABOVE;
const Int32 ListPopupWindow::POSITION_PROMPT_BELOW;
const Int32 ListPopupWindow::MATCH_PARENT;
const Int32 ListPopupWindow::WRAP_CONTENT;
const Int32 ListPopupWindow::INPUT_METHOD_FROM_FOCUSABLE;
const Int32 ListPopupWindow::INPUT_METHOD_NEEDED;
const Int32 ListPopupWindow::INPUT_METHOD_NOT_NEEDED;
const String ListPopupWindow::TAG("ListPopupWindow");
const Boolean ListPopupWindow::DEBUG = FALSE;
const Int32 ListPopupWindow::EXPAND_LIST_TIMEOUT;

CAR_INTERFACE_IMPL(ListPopupWindow, Object, IListPopupWindow)

ListPopupWindow::ListPopupWindow()
    : mListItemExpandMaximum(Elastos::Core::Math::INT32_MAX_VALUE)
    , mDropDownHeight(IViewGroupLayoutParams::WRAP_CONTENT)
    , mDropDownWidth(IViewGroupLayoutParams::WRAP_CONTENT)
    , mDropDownHorizontalOffset(0)
    , mDropDownVerticalOffset(0)
    , mDropDownVerticalOffsetSet(FALSE)
    , mDropDownAlwaysVisible(FALSE)
    , mForceIgnoreOutsideTouch(FALSE)
    , mPromptPosition(IListPopupWindow::POSITION_PROMPT_ABOVE)
    , mModal(FALSE)
    , mLayoutDirection(0)
{
    mResizePopupRunnable = new ResizePopupRunnable(this);
    mTouchInterceptor = new PopupTouchInterceptor(this);
    mScrollListener = new PopupScrollListener(this);
    mHideSelector = new ListSelectorHider(this);
    CHandler::New((IHandler**)&mHandler);
    CRect::New((IRect**)&mTempRect);
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, R::attr::listPopupWindowStyle, 0);
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::listPopupWindowStyle, 0);
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    mContext = context;

    AutoPtr< ArrayOf<Int32> > attrsArray = TO_ATTRS_ARRAYOF(R::styleable::ListPopupWindow);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrsArray, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetDimensionPixelOffset(R::styleable::ListPopupWindow_dropDownHorizontalOffset, 0,
            &mDropDownHorizontalOffset);
    a->GetDimensionPixelOffset(R::styleable::ListPopupWindow_dropDownVerticalOffset, 0,
            &mDropDownVerticalOffset);
    if (mDropDownVerticalOffset != 0) {
        mDropDownVerticalOffsetSet = TRUE;
    }
    a->Recycle();

    CPopupWindow::New(context, attrs, defStyleAttr, defStyleRes, (IPopupWindow**)&mPopup);
    mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
    // Set the default layout direction to match the default locale one
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    mLayoutDirection = TextUtils::GetLayoutDirectionFromLocale(locale);
    return NOERROR;
}

ECode ListPopupWindow::SetAdapter(
    /* [in] */ IListAdapter* adapter)
{
    if (mObserver = NULL) {
        mObserver = new PopupDataSetObserver(this);
    }
    else if (mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(mObserver);
    }
    mAdapter = adapter;
    if (mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->RegisterDataSetObserver(mObserver);
    }

    if (mDropDownList != NULL) {
        mDropDownList->SetAdapter(IAdapter::Probe(mAdapter));
    }
    return NOERROR;
}

ECode ListPopupWindow::SetPromptPosition(
    /* [in] */ Int32 position)
{
    mPromptPosition = position;
    return NOERROR;
}

ECode ListPopupWindow::GetPromptPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPromptPosition;
    return NOERROR;
}

ECode ListPopupWindow::SetModal(
    /* [in] */ Boolean modal)
{
    mModal = modal;
    return mPopup->SetFocusable(modal);
}

ECode ListPopupWindow::IsModal(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mModal;
    return NOERROR;
}

ECode ListPopupWindow::SetForceIgnoreOutsideTouch(
    /* [in] */ Boolean forceIgnoreOutsideTouch)
{
    mForceIgnoreOutsideTouch = forceIgnoreOutsideTouch;
    return NOERROR;
}

ECode ListPopupWindow::SetDropDownAlwaysVisible(
    /* [in] */ Boolean dropDownAlwaysVisible)
{
    mDropDownAlwaysVisible = dropDownAlwaysVisible;
    return NOERROR;
}

ECode ListPopupWindow::IsDropDownAlwaysVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDropDownAlwaysVisible;
    return NOERROR;
}

ECode ListPopupWindow::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    return mPopup->SetSoftInputMode(mode);
}

ECode ListPopupWindow::GetSoftInputMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPopup->GetSoftInputMode(result);
}

ECode ListPopupWindow::SetListSelector(
    /* [in] */ IDrawable* selector)
{
    mDropDownListHighlight = selector;
    return NOERROR;
}

ECode ListPopupWindow::GetBackground(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result);
    return mPopup->GetBackground(result);
}

ECode ListPopupWindow::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    return mPopup->SetBackgroundDrawable(d);
}

ECode ListPopupWindow::SetAnimationStyle(
    /* [in] */ Int32 animationStyle)
{
    return mPopup->SetAnimationStyle(animationStyle);
}

ECode ListPopupWindow::GetAnimationStyle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPopup->GetAnimationStyle(result);
}

ECode ListPopupWindow::GetAnchorView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDropDownAnchorView;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ListPopupWindow::SetAnchorView(
    /* [in] */ IView* anchor)
{
    mDropDownAnchorView = anchor;
    return NOERROR;
}

ECode ListPopupWindow::GetHorizontalOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDropDownHorizontalOffset;
    return NOERROR;
}

ECode ListPopupWindow::SetHorizontalOffset(
    /* [in] */ Int32 offset)
{
    mDropDownHorizontalOffset = offset;
    return NOERROR;
}

ECode ListPopupWindow::GetVerticalOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if(!mDropDownVerticalOffsetSet) {
        *result = 0;
        return NOERROR;
    }
    *result = mDropDownVerticalOffset;
    return NOERROR;
}

ECode ListPopupWindow::SetVerticalOffset(
    /* [in] */ Int32 offset)
{
    mDropDownVerticalOffset = offset;
    mDropDownVerticalOffsetSet = TRUE;
    return NOERROR;
}

ECode ListPopupWindow::SetDropDownGravity(
    /* [in] */ Int32 gravity)
{
    mDropDownGravity = gravity;
    return NOERROR;
}

ECode ListPopupWindow::GetWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDropDownWidth;
    return NOERROR;
}

ECode ListPopupWindow::SetWidth(
    /* [in] */ Int32 width)
{
    mDropDownWidth = width;
    return NOERROR;
}

ECode ListPopupWindow::SetContentWidth(
    /* [in] */ Int32 width)
{
    AutoPtr<IDrawable> popupBackground;
    mPopup->GetBackground((IDrawable**)&popupBackground);
    if (popupBackground != NULL) {
        Boolean res = FALSE;
        popupBackground->GetPadding(mTempRect, &res);
        Int32 left, right;
        mTempRect->GetLeft(&left);
        mTempRect->GetRight(&right);
        mDropDownWidth = left + right + width;
    }
    else {
        SetWidth(width);
    }
    return NOERROR;
}

ECode ListPopupWindow::GetHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDropDownHeight;
    return NOERROR;
}

ECode ListPopupWindow::SetHeight(
    /* [in] */ Int32 height)
{
    mDropDownHeight = height;
    return NOERROR;
}

ECode ListPopupWindow::SetOnItemClickListener(
    /* [in] */ IAdapterViewOnItemClickListener* clickListener)
{
    mItemClickListener = clickListener;
    return NOERROR;
}

ECode ListPopupWindow::SetOnItemSelectedListener(
    /* [in] */ IAdapterViewOnItemSelectedListener* selectedListener)
{
    mItemSelectedListener = selectedListener;
    return NOERROR;
}

ECode ListPopupWindow::SetPromptView(
    /* [in] */ IView* prompt)
{
    Boolean showing = FALSE;
    IsShowing(&showing);
    if (showing) {
        RemovePromptView();
    }
    mPromptView = prompt;
    if (showing) {
        Show();
    }
    return NOERROR;
}

ECode ListPopupWindow::PostShow()
{
    Boolean resTmp;
    mHandler->Post(mShowDropDownRunnable, &resTmp);
    return NOERROR;
}

ECode ListPopupWindow::Show()
{
    Int32 height = BuildDropDown();
    Int32 widthSpec = 0;
    Int32 heightSpec = 0;

    Boolean noInputMethod = FALSE;
    IsInputMethodNotNeeded(&noInputMethod);
    mPopup->SetAllowScrollingAnchorParent(!noInputMethod);

    AutoPtr<IView> anchorView;
    GetAnchorView((IView**)&anchorView);

    Boolean isShowing = FALSE;
    mPopup->IsShowing(&isShowing);
    if (isShowing) {
        if (mDropDownWidth == IViewGroupLayoutParams::MATCH_PARENT) {
            // The call to PopupWindow's update method below can accept -1 for any
            // value you do not want to update.
            widthSpec = -1;
        }
        else if (mDropDownWidth == IViewGroupLayoutParams::WRAP_CONTENT) {
            anchorView->GetWidth(&widthSpec);
        }
        else {
            widthSpec = mDropDownWidth;
        }

        if (mDropDownHeight == IViewGroupLayoutParams::MATCH_PARENT) {
            heightSpec = noInputMethod ? height : IViewGroupLayoutParams::MATCH_PARENT;
            if (noInputMethod) {
                mPopup->SetWindowLayoutMode(mDropDownWidth == IViewGroupLayoutParams::MATCH_PARENT ?
                    IViewGroupLayoutParams::MATCH_PARENT : 0, 0);
            }
            else {
                mPopup->SetWindowLayoutMode(mDropDownWidth == IViewGroupLayoutParams::MATCH_PARENT ?
                    IViewGroupLayoutParams::MATCH_PARENT : 0, IViewGroupLayoutParams::MATCH_PARENT);
            }
        }
        else if (mDropDownHeight == IViewGroupLayoutParams::WRAP_CONTENT) {
            heightSpec = height;
        }
        else {
            heightSpec = mDropDownHeight;
        }

        mPopup->SetOutsideTouchable(!mForceIgnoreOutsideTouch && ! mDropDownAlwaysVisible);
        mPopup->Update(anchorView, mDropDownHorizontalOffset, mDropDownVerticalOffset,
                widthSpec, heightSpec);
    }
    else {
        if (mDropDownWidth == IViewGroupLayoutParams::MATCH_PARENT) {
            widthSpec = IViewGroupLayoutParams::MATCH_PARENT;
        }
        else {
            if (mDropDownWidth == IViewGroupLayoutParams::WRAP_CONTENT) {
                Int32 getWidth = 0;
                anchorView->GetWidth(&getWidth);
                mPopup->SetWidth(getWidth);
            }
            else {
                mPopup->SetWidth(mDropDownWidth);
            }
        }

        if (mDropDownHeight == IViewGroupLayoutParams::MATCH_PARENT) {
            heightSpec = IViewGroupLayoutParams::MATCH_PARENT;
        }
        else {
            if (mDropDownHeight == IViewGroupLayoutParams::WRAP_CONTENT) {
                mPopup->SetHeight(height);
            }
            else {
                mPopup->SetHeight(mDropDownHeight);
            }
        }

        mPopup->SetWindowLayoutMode(widthSpec, heightSpec);
        mPopup->SetClipToScreenEnabled(TRUE);

        // use outside touchable to dismiss drop down when touching outside of it, so
        // only set this if the dropdown is not always visible
        mPopup->SetOutsideTouchable(!mForceIgnoreOutsideTouch && !mDropDownAlwaysVisible);
        mPopup->SetTouchInterceptor(mTouchInterceptor);
        mPopup->ShowAsDropDown(anchorView, mDropDownHorizontalOffset, mDropDownVerticalOffset, mDropDownGravity);
        mDropDownList->SetSelection(IAdapterView::INVALID_POSITION);

        Boolean touchMode;
        if (!mModal || (mDropDownList->IsInTouchMode(&touchMode), touchMode)) {
            ClearListSelection();
        }

        if (!mModal) {
            Boolean result = FALSE;
            mHandler->Post(mHideSelector, &result);
        }
    }
    return NOERROR;
}

ECode ListPopupWindow::Dismiss()
{
    mPopup->Dismiss();
    RemovePromptView();
    mPopup->SetContentView(NULL);
    mDropDownList = NULL;
    mHandler->RemoveCallbacks(mResizePopupRunnable);
    return NOERROR;
}

ECode ListPopupWindow::SetOnDismissListener(
    /* [in] */ IPopupWindowOnDismissListener* listener)
{
    return mPopup->SetOnDismissListener(listener);
}

ECode ListPopupWindow::SetInputMethodMode(
    /* [in] */ Int32 mode)
{
    return mPopup->SetInputMethodMode(mode);
}

ECode ListPopupWindow::GetInputMethodMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPopup->GetInputMethodMode(result);
}

ECode ListPopupWindow::SetSelection(
    /* [in] */ Int32 position)
{
    Boolean isShowing = FALSE;
    AutoPtr<DropDownListView> list = mDropDownList;
    if ((IsShowing(&isShowing), isShowing) && list != NULL) {
        list->mListSelectionHidden = FALSE;
        list->SetSelection(position);
        Int32 mode = 0;
        list->GetChoiceMode(&mode);
        if (mode != IAbsListView::CHOICE_MODE_NONE) {
            list->SetItemChecked(position, TRUE);
        }
    }
    return NOERROR;
}

ECode ListPopupWindow::ClearListSelection()
{
    AutoPtr<DropDownListView> list = mDropDownList;
    if (list != NULL) {
        // WARNING: Please read the comment where mListSelectionHidden is declared
        list->mListSelectionHidden = TRUE;
        list->HideSelector();
        list->RequestLayout();
    }
    return NOERROR;
}

ECode ListPopupWindow::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPopup->IsShowing(result);
}

ECode ListPopupWindow::IsInputMethodNotNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 mode = 0;
    mPopup->GetInputMethodMode(&mode);
    *result = (mode == IPopupWindow::INPUT_METHOD_NOT_NEEDED);
    return NOERROR;
}

ECode ListPopupWindow::PerformItemClick(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if (isShowing) {
        if (mItemClickListener != NULL) {
            AutoPtr<DropDownListView> list = mDropDownList;
            Int32 visiblePosition = 0;
            list->GetFirstVisiblePosition(&visiblePosition);
            AutoPtr<IView> child;
            list->GetChildAt(position - visiblePosition, (IView**)&child);
            AutoPtr<IAdapter> adapter;
            list->GetAdapter((IAdapter**)&adapter);
            Int64 id = 0;
            adapter->GetItemId(position, &id);
            mItemClickListener->OnItemClick(list, child, position, id);
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ListPopupWindow::GetSelectedItem(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if(!isShowing) {
        *result = NULL;
        return NOERROR;
    }
    mDropDownList->GetSelectedItem(result);
    return NOERROR;
}

ECode ListPopupWindow::GetSelectedItemPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if(!isShowing) {
        *result = IAdapterView::INVALID_POSITION;
        return NOERROR;
    }
    mDropDownList->GetSelectedItemPosition(result);
    return NOERROR;
}

ECode ListPopupWindow::GetSelectedItemId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if(!isShowing) {
        *result = IAdapterView::INVALID_ROW_ID;
        return NOERROR;
    }
    mDropDownList->GetSelectedItemId(result);
    return NOERROR;
}

ECode ListPopupWindow::GetSelectedView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if(!isShowing) {
        *result = NULL;
        return NOERROR;
    }
    mDropDownList->GetSelectedView(result);
    return NOERROR;
}

ECode ListPopupWindow::GetListView(
    /* [out] */ IListView** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IListView::Probe(mDropDownList);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ListPopupWindow::SetListItemExpandMax(
    /* [in] */ Int32 max)
{
    mListItemExpandMaximum = max;
    return NOERROR;
}

ECode ListPopupWindow::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // when the drop down is shown, we drive it directly
    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if (isShowing) {
        // the key events are forwarded to the list in the drop down view
        // note that ListView handles space but we don't want that to happen
        // also if selection is not currently in the drop down, then don't
        // let center or enter presses go there since that would cause it
        // to select one of its items
        Int32 itemPosition = 0;
        if (keyCode != IKeyEvent::KEYCODE_SPACE
                && ((mDropDownList->GetSelectedItemPosition(&itemPosition), itemPosition >= 0)
                    || !CKeyEvent::IsConfirmKey(keyCode))) {
            Int32 curIndex = 0;
            mDropDownList->GetSelectedItemPosition(&curIndex);
            Boolean consumed = FALSE;

            Boolean aboveAnchor = FALSE;
            mPopup->IsAboveAnchor(&aboveAnchor);
            Boolean below = !aboveAnchor;

            AutoPtr<IListAdapter> adapter = mAdapter;

            Boolean allEnabled = FALSE;
            Int32 firstItem = Elastos::Core::Math::INT32_MAX_VALUE;
            Int32 lastItem = Elastos::Core::Math::INT32_MIN_VALUE;

            if (adapter != NULL) {
                adapter->AreAllItemsEnabled(&allEnabled);
                firstItem = allEnabled ? 0 : mDropDownList->LookForSelectablePosition(0, TRUE);

                Int32 count = 0;
                IAdapter::Probe(adapter)->GetCount(&count);
                lastItem = allEnabled ? count - 1 : mDropDownList->LookForSelectablePosition(count - 1, FALSE);
            }

            if ((below && IKeyEvent::KEYCODE_DPAD_UP && curIndex <= firstItem) ||
                    (!below && IKeyEvent::KEYCODE_DPAD_DOWN && curIndex >= lastItem)) {
                // When the selection is at the top, we block the key
                // event to prevent focus from moving.
                ClearListSelection();
                mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
                Show();
                *result = TRUE;
                return NOERROR;
            }
            else {
                // WARNING: Please read the comment where mListSelectionHidden
                //          is declared
                mDropDownList->mListSelectionHidden = FALSE;
            }

            mDropDownList->OnKeyDown(keyCode, event, &consumed);
            if (DEBUG) Logger::V(TAG, "Key down: code=%d list consumed=%d", keyCode, consumed);

            if (consumed) {
                // If it handled the key event, then the user is
                // navigating in the list, so we should put it in front.
                mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
                // Here's a little trick we need to do to make sure that
                // the list view is actually showing its focus indicator,
                // by ensuring it has focus and getting its window out
                // of touch mode.
                Boolean res = FALSE;
                mDropDownList->RequestFocusFromTouch(&res);
                Show();

                switch(keyCode) {
                    // avoid passing the focus from the text view to the
                    // next component
                    case IKeyEvent::KEYCODE_ENTER:
                    case IKeyEvent::KEYCODE_DPAD_CENTER:
                    case IKeyEvent::KEYCODE_DPAD_DOWN:
                    case IKeyEvent::KEYCODE_DPAD_UP:
                        *result = TRUE;
                        return NOERROR;
                }
            }
            else {
                if (below && keyCode == IKeyEvent::KEYCODE_DPAD_DOWN) {
                    // when the selection is at the bottom, we block the
                    // event to avoid going to the next focusable widget
                    if (curIndex == lastItem) {
                        *result = TRUE;
                        return NOERROR;
                    }
                }
                else if(!below && keyCode == IKeyEvent::KEYCODE_DPAD_UP &&
                        curIndex == firstItem) {
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode ListPopupWindow::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if (isShowing) {
        Int32 itemPosition;
        mDropDownList->GetSelectedItemPosition(&itemPosition);
        if (itemPosition >= 0) {
            Boolean consumed = FALSE;
            mDropDownList->OnKeyUp(keyCode, event, &consumed);
            if (consumed  && CKeyEvent::IsConfirmKey(keyCode)) {
                // if the list accepts the key events and the key event was a click, the text view
                // gets the selected item from the drop down as its content
                Dismiss();
            }
            *result = consumed;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ListPopupWindow::OnKeyPreIme(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isShowing = FALSE;
    if (keyCode == IKeyEvent::KEYCODE_BACK && (IsShowing(&isShowing), isShowing)) {
        // special case for the back key, we do not even try to send it
        // to the drop down list but instead, consume it immediately
        AutoPtr<IView> anchorView = mDropDownAnchorView;
        Int32 action = 0;
        event->GetAction(&action);
        Int32 repeatCount = 0;
        event->GetRepeatCount(&repeatCount);
        if (action == IKeyEvent::ACTION_DOWN && repeatCount == 0) {
            AutoPtr<IDispatcherState> state;
            anchorView->GetKeyDispatcherState((IDispatcherState**)&state);
            if (state != NULL) {
                state->StartTracking(event, TO_IINTERFACE(this));
            }
            *result = TRUE;
            return NOERROR;
        }
        else if (action == IKeyEvent::ACTION_UP) {
            AutoPtr<IDispatcherState> state;
            anchorView->GetKeyDispatcherState((IDispatcherState**)&state);
            if (state != NULL) {
                state->HandleUpEvent(event);
            }
            Boolean isTracking = FALSE;
            event->IsTracking(&isTracking);
            Boolean isCanceled = FALSE;
            event->IsCanceled(&isCanceled);
            if (isTracking && !isCanceled) {
                Dismiss();
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ListPopupWindow::CreateDragToOpenListener(
    /* [in] */ IView* src,
    /* [out] */ IViewOnTouchListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = new InnerForwardingListener(this);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ListPopupWindow::RemovePromptView()
{
    if (mPromptView != NULL) {
        AutoPtr<IViewParent> parent;
        mPromptView->GetParent((IViewParent**)&parent);
        IViewGroup* group = IViewGroup::Probe(parent);
        if (group) {
            group->RemoveViewInLayout(mPromptView);
        }
    }
    return NOERROR;
}

Int32 ListPopupWindow::BuildDropDown()
{
    AutoPtr<IViewGroup> dropDownView;
    Int32 otherHeights = 0;

    if (mDropDownList == NULL) {
        AutoPtr<IContext> context = mContext;

        /**
         * This Runnable exists for the sole purpose of checking if the view layout has got
         * completed and if so call showDropDown to display the drop down. This is used to show
         * the drop down as soon as possible after user opens up the search dialog, without
         * waiting for the normal UI pipeline to do it's job which is slower than this method.
         */
        mShowDropDownRunnable = new InnerShowRunnable(this);

        mDropDownList = new DropDownListView();
        mDropDownList->constructor(context, !mModal);
        if (mDropDownListHighlight != NULL) {
            mDropDownList->SetSelector(mDropDownListHighlight);
        }
        mDropDownList->SetAdapter(IAdapter::Probe(mAdapter));
        mDropDownList->SetOnItemClickListener(mItemClickListener);
        mDropDownList->SetFocusable(TRUE);
        mDropDownList->SetFocusableInTouchMode(TRUE);
        AutoPtr<InnerAdapterViewOnItemSelectedListener> listener = new InnerAdapterViewOnItemSelectedListener(this);
        mDropDownList->SetOnItemSelectedListener(listener);
        mDropDownList->SetOnScrollListener(mScrollListener);

        if (mItemSelectedListener != NULL) {
            mDropDownList->SetOnItemSelectedListener(mItemSelectedListener);
        }

        dropDownView = IViewGroup::Probe(mDropDownList);
        AutoPtr<IView> hintView = mPromptView;
        if (hintView != NULL) {
            // if a hint has been specified, we accomodate more space for it and
            // add a text view in the drop down menu, at the bottom of the list
            AutoPtr<ILinearLayout> hintcontainer;
            FAIL_RETURN(CLinearLayout::New(context, (ILinearLayout**)&hintcontainer));
            hintcontainer->SetOrientation(ILinearLayout::VERTICAL);

            AutoPtr<ILinearLayoutLayoutParams> hintParams;
            FAIL_RETURN(CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                    0, 1.0f, (ILinearLayoutLayoutParams**)&hintParams));

            IViewGroup* hintcontainerViewGroupTmp = IViewGroup::Probe(hintcontainer);
            switch (mPromptPosition) {
                case IListPopupWindow::POSITION_PROMPT_BELOW:
                    hintcontainerViewGroupTmp->AddView(IView::Probe(dropDownView), IViewGroupLayoutParams::Probe(hintParams));
                    hintcontainerViewGroupTmp->AddView(hintView);
                    break;

                case IListPopupWindow::POSITION_PROMPT_ABOVE:
                    hintcontainerViewGroupTmp->AddView(hintView);
                    hintcontainerViewGroupTmp->AddView(IView::Probe(dropDownView), IViewGroupLayoutParams::Probe(hintParams));
                    break;

                default:
                    Logger::E(TAG, "Invalid hint position %d", mPromptPosition);
                    break;
            }

            // measure the hint's height to find how much more vertical space
            // we need to add to the drop down's height
            Int32 widthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(mDropDownWidth, Elastos::Droid::View::View::MeasureSpec::AT_MOST);
            Int32 heightSpec = Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED;
            hintView->Measure(widthSpec, heightSpec);

            AutoPtr<IViewGroupLayoutParams> pms;
            hintView->GetLayoutParams((IViewGroupLayoutParams**)&pms);
            hintParams = ILinearLayoutLayoutParams::Probe(pms);

            Int32 measureHeight = 0;
            hintView->GetMeasuredHeight(&measureHeight);
            IViewGroupMarginLayoutParams* layoutParams = IViewGroupMarginLayoutParams::Probe(hintParams);
            Int32 topMargin = 0;
            layoutParams->GetTopMargin(&topMargin);
            Int32 bottomMargin = 0;
            layoutParams->GetBottomMargin(&bottomMargin);
            otherHeights = measureHeight + topMargin + bottomMargin;

            dropDownView = IViewGroup::Probe(hintcontainer);
        }

        mPopup->SetContentView(IView::Probe(dropDownView));
    }
    else {
        AutoPtr<IView> view;
        mPopup->GetContentView((IView**)&view);
        dropDownView = IViewGroup::Probe(view);
        view = mPromptView;
        if (view != NULL) {
            AutoPtr<IViewGroupLayoutParams> pms;
            view->GetLayoutParams((IViewGroupLayoutParams**)&pms);
            AutoPtr<ILinearLayoutLayoutParams> hintParams = ILinearLayoutLayoutParams::Probe(pms);

            Int32 measureHeight = 0;
            view->GetMeasuredHeight(&measureHeight);
            IViewGroupMarginLayoutParams* layoutParams = IViewGroupMarginLayoutParams::Probe(hintParams);
            Int32 topMargin = 0;
            layoutParams->GetTopMargin(&topMargin);
            Int32 bottomMargin = 0;
            layoutParams->GetBottomMargin(&bottomMargin);
            otherHeights = measureHeight + topMargin + bottomMargin;
        }
    }

    // getMaxAvailableHeight() subtracts the padding, so we put it back
    // to get the available height for the whole window
    Int32 padding = 0;
    AutoPtr<IDrawable> background;
    mPopup->GetBackground((IDrawable**)&background);
    if (background != NULL) {
        Boolean paddingRes = FALSE;
        background->GetPadding(mTempRect, &paddingRes);
        Int32 top, bottom;
        mTempRect->GetTop(&top);
        mTempRect->GetBottom(&bottom);
        padding = top + bottom;

        // If we don't have an explicit vertical offset, determine one from the window
        // background so that content will line up.
        if (!mDropDownVerticalOffsetSet) {
            mDropDownVerticalOffset = -top;
        }
    }
    else {
        mTempRect->SetEmpty();
    }

    // Max height available on the screen for a popup.
    Int32 ipmMode = 0;
    mPopup->GetInputMethodMode(&ipmMode);
    Boolean ignoreBottomDecorations = ipmMode == IPopupWindow::INPUT_METHOD_NOT_NEEDED;
    AutoPtr<IView> anchorView;
    GetAnchorView((IView**)&anchorView);
    Int32 maxHeight = 0;
    mPopup->GetMaxAvailableHeight(anchorView, mDropDownVerticalOffset, ignoreBottomDecorations, &maxHeight);

    if (mDropDownAlwaysVisible || mDropDownHeight == IViewGroupLayoutParams::MATCH_PARENT) {
        return maxHeight + padding;
    }

    Int32 childWidthSpec = 0;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    switch (mDropDownWidth) {
        case IViewGroupLayoutParams::WRAP_CONTENT: {
            Int32 wp, left, right;
            dm->GetWidthPixels(&wp);
            mTempRect->GetLeft(&left);
            mTempRect->GetRight(&right);
            childWidthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
                wp - (left + right),
                Elastos::Droid::View::View::MeasureSpec::AT_MOST);
            break;
        }
        case IViewGroupLayoutParams::MATCH_PARENT: {
            Int32 wp, left, right;
            dm->GetWidthPixels(&wp);
            mTempRect->GetLeft(&left);
            mTempRect->GetRight(&right);
            childWidthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
                wp - (left + right),
                Elastos::Droid::View::View::MeasureSpec::EXACTLY);
            break;
        }
        default:
            childWidthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
                mDropDownWidth, Elastos::Droid::View::View::MeasureSpec::EXACTLY);
            break;
    }
    Int32 listContent = mDropDownList->MeasureHeightOfChildren(childWidthSpec,
            0, ListView::NO_POSITION, maxHeight - otherHeights, -1);
    // add padding only if the list has items in it, that way we don't show
    // the popup if it is not needed
    if (listContent > 0) otherHeights += padding;

    return listContent + otherHeights;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos

