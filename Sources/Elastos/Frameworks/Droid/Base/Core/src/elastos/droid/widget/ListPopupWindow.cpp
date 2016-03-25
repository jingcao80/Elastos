
#include "elastos/droid/animation/CObjectAnimatorHelper.h"
//#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/CTextUtils.h"
#include "elastos/droid/internal/widget/CAbsListViewAutoScroller.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/view/animation/CAccelerateDecelerateInterpolator.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/view/CMotionEventHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/widget/CLinearLayout.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/widget/ListPopupWindow.h"
#include "elastos/droid/widget/CPopupWindow.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Internal::Widget::CAbsListViewAutoScroller;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateDecelerateInterpolator;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
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
{
    assert(mOwner);
}

ECode ListPopupWindow::ForwardingListener::DisallowIntercept::Run()
{
    // ==================before translated======================
    // final ViewParent parent = mSrc.getParent();
    // parent.requestDisallowInterceptTouchEvent(true);

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
{
    assert(mOwner);
}

ECode ListPopupWindow::ForwardingListener::TriggerLongPress::Run()
{
    // ==================before translated======================
    // onLongPress();

    mOwner->OnLongPress();
    return NOERROR;
}

//=====================================================================
//                 ListPopupWindow::ForwardingListener
//=====================================================================
CAR_INTERFACE_IMPL_2(ListPopupWindow::ForwardingListener, Object, IViewOnTouchListener, IViewOnAttachStateChangeListener)

ListPopupWindow::ForwardingListener::ForwardingListener()
    : mSrc(NULL)
{
}

ECode ListPopupWindow::ForwardingListener::constructor(
    /* [in] */ IView* src)
{
    VALIDATE_NOT_NULL(src);
    // ==================before translated======================
    // mSrc = src;
    // mScaledTouchSlop = ViewConfiguration.get(src.getContext()).getScaledTouchSlop();
    // mTapTimeout = ViewConfiguration.getTapTimeout();
    //
    // // Use a medium-press timeout. Halfway between tap and long-press.
    // mLongPressTimeout = (mTapTimeout + ViewConfiguration.getLongPressTimeout()) / 2;
    //
    // src.addOnAttachStateChangeListener(this);

    mSrc = src;
    AutoPtr<IContext> context;
    src->GetContext((IContext**)&context);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> viewConfiguration;
    helper->Get(context, (IViewConfiguration**)&viewConfiguration);

    Int32 mScaledTouchSlop = 0;
    viewConfiguration->GetScaledTouchSlop(&mScaledTouchSlop);
    Int32 mTapTimeout = 0;
    helper->GetTapTimeout(&mTapTimeout);
    // Use a medium-press timeout. Halfway between tap and long-press.
    Int32 longPressTimeout = 0;
    helper->GetLongPressTimeout(&longPressTimeout);
    mLongPressTimeout = (mTapTimeout + longPressTimeout) / 2;
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
    VALIDATE_NOT_NULL(v);
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final boolean wasForwarding = mForwarding;
    // final boolean forwarding;
    // if (wasForwarding) {
    //     if (mWasLongPress) {
    //         // If we started forwarding as a result of a long-press,
    //         // just silently stop forwarding events so that the window
    //         // stays open.
    //         forwarding = onTouchForwarded(event);
    //     } else {
    //         forwarding = onTouchForwarded(event) || !onForwardingStopped();
    //     }
    // } else {
    //     forwarding = onTouchObserved(event) && onForwardingStarted();
    //
    //     if (forwarding) {
    //         // Make sure we cancel any ongoing source event stream.
    //         final long now = SystemClock.uptimeMillis();
    //         final MotionEvent e = MotionEvent.obtain(now, now, MotionEvent.ACTION_CANCEL,
    //                 0.0f, 0.0f, 0);
    //         mSrc.onTouchEvent(e);
    //         e.recycle();
    //     }
    // }
    //
    // mForwarding = forwarding;
    // return forwarding || wasForwarding;

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

            AutoPtr<IMotionEventHelper> helper;
            CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
            AutoPtr<IMotionEvent> e;
            helper->Obtain(now, now, IMotionEvent::ACTION_CANCEL, 0.0f, 0.0f, 0, (IMotionEvent**)&e);

            Boolean resTmp;
            mSrc->OnTouchEvent(e, &resTmp);

            IInputEvent* eTmp = IInputEvent::Probe(e);
            eTmp->Recycle();
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
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // mForwarding = false;
    // mActivePointerId = MotionEvent.INVALID_POINTER_ID;
    //
    // if (mDisallowIntercept != null) {
    //     mSrc.removeCallbacks(mDisallowIntercept);
    // }

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
    // ==================before translated======================
    // final ListPopupWindow popup = getPopup();
    // if (popup != null && !popup.isShowing()) {
    //     popup.show();
    // }
    // return true;

    AutoPtr<IListPopupWindow> popup;
    GetPopup((IListPopupWindow**)&popup);
    Boolean isShowing = FALSE;
    popup->IsShowing(&isShowing);
    if (popup != NULL && !isShowing) {
        popup->Show();
    }
    return TRUE;
}

Boolean ListPopupWindow::ForwardingListener::OnForwardingStopped()
{
    // ==================before translated======================
    // final ListPopupWindow popup = getPopup();
    // if (popup != null && popup.isShowing()) {
    //     popup.dismiss();
    // }
    // return true;

    AutoPtr<IListPopupWindow> popup;
    GetPopup((IListPopupWindow**)&popup);
    Boolean isShowing = FALSE;
    popup->IsShowing(&isShowing);
    if (popup != NULL && !isShowing) {
        popup->Dismiss();
    }
    return TRUE;
}

Boolean ListPopupWindow::ForwardingListener::OnTouchObserved(
    /* [in] */ IMotionEvent* srcEvent)
{
    // ==================before translated======================
    // final View src = mSrc;
    // if (!src.isEnabled()) {
    //     return false;
    // }
    //
    // final int actionMasked = srcEvent.getActionMasked();
    // switch (actionMasked) {
    //     case MotionEvent.ACTION_DOWN:
    //         mActivePointerId = srcEvent.getPointerId(0);
    //         mWasLongPress = false;
    //
    //         if (mDisallowIntercept == null) {
    //             mDisallowIntercept = new DisallowIntercept();
    //         }
    //         src.postDelayed(mDisallowIntercept, mTapTimeout);
    //
    //         if (mTriggerLongPress == null) {
    //             mTriggerLongPress = new TriggerLongPress();
    //         }
    //         src.postDelayed(mTriggerLongPress, mLongPressTimeout);
    //         break;
    //     case MotionEvent.ACTION_MOVE:
    //         final int activePointerIndex = srcEvent.findPointerIndex(mActivePointerId);
    //         if (activePointerIndex >= 0) {
    //             final float x = srcEvent.getX(activePointerIndex);
    //             final float y = srcEvent.getY(activePointerIndex);
    //
    //             // Has the pointer has moved outside of the view?
    //             if (!src.pointInView(x, y, mScaledTouchSlop)) {
    //                 clearCallbacks();
    //
    //                 // Don't let the parent intercept our events.
    //                 src.getParent().requestDisallowInterceptTouchEvent(true);
    //                 return true;
    //             }
    //         }
    //         break;
    //     case MotionEvent.ACTION_CANCEL:
    //     case MotionEvent.ACTION_UP:
    //         clearCallbacks();
    //         break;
    // }
    //
    // return false;

    AutoPtr<IView> src = mSrc;
    Boolean isEnabled = FALSE;
    src->IsEnabled(&isEnabled);
    if (!isEnabled) {
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
                    IObject* objTmp = IObject::Probe(src.Get());
                    Elastos::Droid::View::View* viewTmp = (Elastos::Droid::View::View*)(objTmp);
                    Boolean resTmp = FALSE;
                    viewTmp->PointInView(x, y, mScaledTouchSlop, &resTmp);
                    if (!resTmp) {
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
    // ==================before translated======================
    // if (mTriggerLongPress != null) {
    //     mSrc.removeCallbacks(mTriggerLongPress);
    // }
    //
    // if (mDisallowIntercept != null) {
    //     mSrc.removeCallbacks(mDisallowIntercept);
    // }

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
    // ==================before translated======================
    // clearCallbacks();
    //
    // final View src = mSrc;
    // if (!src.isEnabled()) {
    //     return;
    // }
    //
    // if (!onForwardingStarted()) {
    //     return;
    // }
    //
    // // Don't let the parent intercept our events.
    // mSrc.getParent().requestDisallowInterceptTouchEvent(true);
    //
    // // Make sure we cancel any ongoing source event stream.
    // final long now = SystemClock.uptimeMillis();
    // final MotionEvent e = MotionEvent.obtain(now, now, MotionEvent.ACTION_CANCEL, 0, 0, 0);
    // mSrc.onTouchEvent(e);
    // e.recycle();
    //
    // mForwarding = true;
    // mWasLongPress = true;

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

    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> e;
    helper->Obtain(now, now, IMotionEvent::ACTION_CANCEL, 0.0f, 0.0f, 0, (IMotionEvent**)&e);

    Boolean resTmp;
    mSrc->OnTouchEvent(e, &resTmp);
    IInputEvent* eTmp = IInputEvent::Probe(e);
    eTmp->Recycle();

    mForwarding = TRUE;
    mWasLongPress = TRUE;
}

Boolean ListPopupWindow::ForwardingListener::OnTouchForwarded(
    /* [in] */ IMotionEvent* srcEvent)
{
    // ==================before translated======================
    // final View src = mSrc;
    // final ListPopupWindow popup = getPopup();
    // if (popup == null || !popup.isShowing()) {
    //     return false;
    // }
    //
    // final DropDownListView dst = popup.mDropDownList;
    // if (dst == null || !dst.isShown()) {
    //     return false;
    // }
    //
    // // Convert event to destination-local coordinates.
    // final MotionEvent dstEvent = MotionEvent.obtainNoHistory(srcEvent);
    // src.toGlobalMotionEvent(dstEvent);
    // dst.toLocalMotionEvent(dstEvent);
    //
    // // Forward converted event to destination view, then recycle it.
    // final boolean handled = dst.onForwardedEvent(dstEvent, mActivePointerId);
    // dstEvent.recycle();
    //
    // // Always cancel forwarding when the touch stream ends.
    // final int action = srcEvent.getActionMasked();
    // final boolean keepForwarding = action != MotionEvent.ACTION_UP
    //         && action != MotionEvent.ACTION_CANCEL;
    //
    // return handled && keepForwarding;

    assert(0);
    AutoPtr<IView> src = mSrc;
    AutoPtr<IListPopupWindow> popup;
    GetPopup((IListPopupWindow**)&popup);
    Boolean isShowing = FALSE;
    popup->IsShowing(&isShowing);
    if (popup == NULL || !isShowing) {
        return FALSE;
    }

    AutoPtr<DropDownListView> dst = ((ListPopupWindow*)popup.Get())->mDropDownList;
    Boolean isShown = FALSE;
    dst->IsShown(&isShown);
    if (dst == NULL || !isShown) {
        return FALSE;
    }

    // Convert event to destination-local coordinates.
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> dstEvent;
    helper->ObtainNoHistory(srcEvent, (IMotionEvent**)&dstEvent);

    Boolean resTmp;
    src->ToGlobalMotionEvent(dstEvent, &resTmp);
    dst->ToLocalMotionEvent(dstEvent, &resTmp);

    // Forward converted event to destination view, then recycle it.
    Boolean handled = FALSE;
    dst->OnForwardedEvent(dstEvent, mActivePointerId, &handled);
    IInputEvent* dstEventTmp = IInputEvent::Probe(dstEvent);
    dstEventTmp->Recycle();

    // Always cancel forwarding when the touch stream ends.
    Int32 action = 0;
    srcEvent->GetActionMasked(&action);
    Boolean keepForwarding = action != IMotionEvent::ACTION_UP && action != IMotionEvent::ACTION_CANCEL;
    return handled && keepForwarding;
}

//=====================================================================
//              ListPopupWindow::InnerForwardingListener
//=====================================================================
ListPopupWindow::InnerForwardingListener::InnerForwardingListener(
    /* [in] */ ListPopupWindow* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ListPopupWindow::InnerForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ListPopupWindow.this;

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
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ListPopupWindow::DropDownListView::InnerAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // performItemClick(child, position, id);

    Boolean resTmp;
    return mOwner->PerformItemClick(mChild, mPosition, mId, &resTmp);
}

//=====================================================================
//                  ListPopupWindow::DropDownListView
//=====================================================================
const Int64 ListPopupWindow::DropDownListView::CLICK_ANIM_DURATION;
const Int32 ListPopupWindow::DropDownListView::CLICK_ANIM_ALPHA;
const AutoPtr<Int32Property> ListPopupWindow::DropDownListView::DRAWABLE_ALPHA = ListPopupWindow::DropDownListView::InitDrawableAlpha();

ListPopupWindow::DropDownListView::DropDownListView(
    /* [in] */ IContext* context,
    /* [in] */ Boolean hijackFocus)
{
    // ==================before translated======================
    // super(context, null, com.android.internal.R.attr.dropDownListViewStyle);
    // mHijackFocus = hijackFocus;
    // // TODO: Add an API to control this
    // setCacheColorHint(0); // Transparent, since the background drawable could be anything.

    assert(0);
    ListView::constructor(context, NULL, R::attr::dropDownListViewStyle);
    mHijackFocus = hijackFocus;
    // TODO: Add an API to control this
    SetCacheColorHint(0); // Transparent, since the background drawable could be anything.
}

ECode ListPopupWindow::DropDownListView::OnForwardedEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 activePointerId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean handledEvent = true;
    // boolean clearPressedItem = false;
    //
    // final int actionMasked = event.getActionMasked();
    // switch (actionMasked) {
    //     case MotionEvent.ACTION_CANCEL:
    //         handledEvent = false;
    //         break;
    //     case MotionEvent.ACTION_UP:
    //         handledEvent = false;
    //         // $FALL-THROUGH$
    //     case MotionEvent.ACTION_MOVE:
    //         final int activeIndex = event.findPointerIndex(activePointerId);
    //         if (activeIndex < 0) {
    //             handledEvent = false;
    //             break;
    //         }
    //
    //         final int x = (int) event.getX(activeIndex);
    //         final int y = (int) event.getY(activeIndex);
    //         final int position = pointToPosition(x, y);
    //         if (position == INVALID_POSITION) {
    //             clearPressedItem = true;
    //             break;
    //         }
    //
    //         final View child = getChildAt(position - getFirstVisiblePosition());
    //         setPressedItem(child, position, x, y);
    //         handledEvent = true;
    //
    //         if (actionMasked == MotionEvent.ACTION_UP) {
    //             clickPressedItem(child, position);
    //         }
    //         break;
    // }
    //
    // // Failure to handle the event cancels forwarding.
    // if (!handledEvent || clearPressedItem) {
    //     clearPressedItem();
    // }
    //
    // // Manage automatic scrolling.
    // if (handledEvent) {
    //     if (mScrollHelper == null) {
    //         mScrollHelper = new AbsListViewAutoScroller(this);
    //     }
    //     mScrollHelper.setEnabled(true);
    //     mScrollHelper.onTouch(this, event);
    // } else if (mScrollHelper != null) {
    //     mScrollHelper.setEnabled(false);
    // }
    //
    // return handledEvent;

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
                Int32 x = (event->GetX(activeIndex, &xTmp), (Int32)xTmp);
                Int32 y = (event->GetY(activeIndex, &yTmp), (Int32)yTmp);
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
    // ==================before translated======================
    // return mDrawsInPressedState || super.touchModeDrawsInPressedState();

    assert(0);
    return mDrawsInPressedState || ListView::TouchModeDrawsInPressedState();
}

AutoPtr<IView> ListPopupWindow::DropDownListView::ObtainView(
    /* [in] */ Int32 position,
    /* [in] */ ArrayOf<Boolean>* isScrap)
{
    // ==================before translated======================
    // View view = super.obtainView(position, isScrap);
    //
    // if (view instanceof TextView) {
    //     ((TextView) view).setHorizontallyScrolling(true);
    // }
    //
    // return view;

    AutoPtr<IView> view = ListView::ObtainView(position, isScrap);
    ITextView* textView = ITextView::Probe(view);
    if (textView) {
        textView->SetHorizontallyScrolling(TRUE);
    }
    return view;
}

Boolean ListPopupWindow::DropDownListView::IsInTouchMode()
{
    // ==================before translated======================
    // // WARNING: Please read the comment where mListSelectionHidden is declared
    // return (mHijackFocus && mListSelectionHidden) || super.isInTouchMode();

    Boolean resTmp = FALSE;
    ListView::IsInTouchMode(&resTmp);
    return (mHijackFocus && mListSelectionHidden) || resTmp;
}

Boolean ListPopupWindow::DropDownListView::HasWindowFocus()
{
    // ==================before translated======================
    // return mHijackFocus || super.hasWindowFocus();

    Boolean resTmp = FALSE;
    ListView::HasWindowFocus(&resTmp);
    return mHijackFocus || resTmp;
}

Boolean ListPopupWindow::DropDownListView::IsFocused()
{
    // ==================before translated======================
    // return mHijackFocus || super.isFocused();

    Boolean resTmp = FALSE;
    ListView::IsFocused(&resTmp);
    return mHijackFocus || resTmp;
}

Boolean ListPopupWindow::DropDownListView::HasFocus()
{
    // ==================before translated======================
    // return mHijackFocus || super.hasFocus();

    Boolean resTmp = FALSE;
    ListView::HasFocus(&resTmp);
    return mHijackFocus || resTmp;
}

void ListPopupWindow::DropDownListView::ClickPressedItem(
    /* [in] */ IView* child,
    /* [in] */ Int32 position)
{
    // ==================before translated======================
    // final long id = getItemIdAtPosition(position);
    // final Animator anim = ObjectAnimator.ofInt(
    //         mSelector, DRAWABLE_ALPHA, 0xFF, CLICK_ANIM_ALPHA, 0xFF);
    // anim.setDuration(CLICK_ANIM_DURATION);
    // anim.setInterpolator(new AccelerateDecelerateInterpolator());
    // anim.addListener(new AnimatorListenerAdapter() {
    //         @Override
    //     public void onAnimationEnd(Animator animation) {
    //         performItemClick(child, position, id);
    //     }
    // });
    // anim.start();
    //
    // if (mClickAnimation != null) {
    //     mClickAnimation.cancel();
    // }
    // mClickAnimation = anim;

    assert(0);
    Int64 id = 0;
    GetItemIdAtPosition(position, &id);

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);

    IProperty* alphaPorperty = IProperty::Probe(DRAWABLE_ALPHA);
    AutoPtr< ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(3);
    values->Set(0, 0xFF);
    values->Set(1, CLICK_ANIM_ALPHA);
    values->Set(2, 0xFF);
    AutoPtr<IObjectAnimator> animTmp;
    helper->OfInt32(TO_IINTERFACE(mSelector), alphaPorperty, values, (IObjectAnimator**)&animTmp);
    IAnimator* anim = IAnimator::Probe(animTmp);

    anim->SetDuration(CLICK_ANIM_DURATION);

    AutoPtr<IAccelerateDecelerateInterpolator> decelerateInterpolator;
    CAccelerateDecelerateInterpolator::New((IAccelerateDecelerateInterpolator**)&decelerateInterpolator);
    ITimeInterpolator* interpolator = ITimeInterpolator::Probe(decelerateInterpolator);
    anim->SetInterpolator(interpolator);

    AutoPtr<InnerAnimatorListenerAdapter> listenerAdapter = new InnerAnimatorListenerAdapter(this, child, position, id);
    IAnimatorListener* listener = IAnimatorListener::Probe(listenerAdapter);
    anim->AddListener(listener);
    anim->Start();

    if (mClickAnimation != NULL) {
        mClickAnimation->Cancel();
    }
    mClickAnimation = anim;
}

void ListPopupWindow::DropDownListView::ClearPressedItem()
{
    // ==================before translated======================
    // mDrawsInPressedState = false;
    // setPressed(false);
    // updateSelectorState();
    //
    // if (mClickAnimation != null) {
    //     mClickAnimation.cancel();
    //     mClickAnimation = null;
    // }

    assert(0);
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
    // ==================before translated======================
    // mDrawsInPressedState = true;
    //
    // // Ordering is essential. First update the pressed state and layout
    // // the children. This will ensure the selector actually gets drawn.
    // setPressed(true);
    // layoutChildren();
    //
    // // Ensure that keyboard focus starts from the last touched position.
    // setSelectedPositionInt(position);
    // positionSelectorLikeTouch(position, child, x, y);
    //
    // // Refresh the drawable state to reflect the new pressed state,
    // // which will also update the selector state.
    // refreshDrawableState();
    //
    // if (mClickAnimation != null) {
    //     mClickAnimation.cancel();
    //     mClickAnimation = null;
    // }

    assert(0);
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

AutoPtr<Int32Property> ListPopupWindow::DropDownListView::InitDrawableAlpha()
{
    AutoPtr<Int32Property> result = new ListPopupWindow::InnerInt32Property(String("alpha"));
    return result;
}

//=====================================================================
//                  ListPopupWindow::InnerInt32Property
//=====================================================================
ListPopupWindow::InnerInt32Property::InnerInt32Property(
    /* [in] */ const String& property)
    : Int32Property(property)
{
}

ECode ListPopupWindow::InnerInt32Property::SetValue(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 value)
{
    VALIDATE_NOT_NULL(object);
    // ==================before translated======================
    // object.setAlpha(value);

    IDrawable* objTmp = IDrawable::Probe(object);
    objTmp->SetAlpha(value);
    return NOERROR;
}

ECode ListPopupWindow::InnerInt32Property::Get(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(object);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return object.getAlpha();

    IDrawable* objTmp = IDrawable::Probe(object);
    Int32 alpha = 0;
    objTmp->GetAlpha(&alpha);
    AutoPtr<IInteger32> resTmp = CoreUtils::Convert(alpha);
    *result = TO_IINTERFACE(resTmp);
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
{
    assert(mOwner);
}

ECode ListPopupWindow::PopupDataSetObserver::OnChanged()
{
    // ==================before translated======================
    // if (isShowing()) {
    //     // Resize the popup to fit new content
    //     show();
    // }

    Boolean isShowing = FALSE;
    mOwner->IsShowing(&isShowing);
    if (isShowing) {
        mOwner->Show();
    }
    return NOERROR;
}

ECode ListPopupWindow::PopupDataSetObserver::OnInvalidated()
{
    // ==================before translated======================
    // dismiss();

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
{
    assert(mOwner);
}

ECode ListPopupWindow::ListSelectorHider::Run()
{
    // ==================before translated======================
    // clearListSelection();

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
{
    assert(mOwner);
}

ECode ListPopupWindow::ResizePopupRunnable::Run()
{
    // ==================before translated======================
    // if (mDropDownList != null && mDropDownList.getCount() > mDropDownList.getChildCount() &&
    //         mDropDownList.getChildCount() <= mListItemExpandMaximum) {
    //     mPopup.setInputMethodMode(PopupWindow.INPUT_METHOD_NOT_NEEDED);
    //     show();
    // }

    assert(0);
    Int32 count = 0;
    mOwner->mDropDownList->GetCount(&count);
    Int32 childCount = 0;
    mOwner->mDropDownList->GetChildCount(&childCount);
    if (mOwner->mDropDownList && count > childCount && childCount <= mOwner->mListItemExpandMaximum) {
        mOwner->mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
        mOwner->Show();
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
{
    assert(mOwner);
}

ECode ListPopupWindow::PopupTouchInterceptor::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(v);
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final int action = event.getAction();
    // final int x = (int) event.getX();
    // final int y = (int) event.getY();
    //
    // if (action == MotionEvent.ACTION_DOWN &&
    //         mPopup != null && mPopup.isShowing() &&
    //         (x >= 0 && x < mPopup.getWidth() && y >= 0 && y < mPopup.getHeight())) {
    //     mHandler.postDelayed(mResizePopupRunnable, EXPAND_LIST_TIMEOUT);
    // } else if (action == MotionEvent.ACTION_UP) {
    //     mHandler.removeCallbacks(mResizePopupRunnable);
    // }
    // return false;

    Int32 action = 0;
    event->GetAction(&action);
    Float fx = 0, fy = 0;
    Int32 x = (event->GetX(&fx), (Int32)fx);
    Int32 y = (event->GetY(&fy), (Int32)fy);

    Int32 width = 0, height = 0;
    mOwner->mPopup->GetWidth(&width);
    mOwner->mPopup->GetHeight(&height);

    Boolean isShowing = FALSE;
    mOwner->mPopup->IsShowing(&isShowing);
    if (action == IMotionEvent::ACTION_DOWN && mOwner->mPopup && isShowing && (x >= 0 && x < width && y >= 0 && y < height)) {
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
{
    assert(mOwner);
}

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
    // ==================before translated======================
    // if (scrollState == SCROLL_STATE_TOUCH_SCROLL &&
    //         !isInputMethodNotNeeded() && mPopup.getContentView() != null) {
    //     mHandler.removeCallbacks(mResizePopupRunnable);
    //     mResizePopupRunnable.run();
    // }

    AutoPtr<IView> v;
    mOwner->mPopup->GetContentView((IView**)&v);
    Boolean isNeeded = FALSE;
    mOwner->IsInputMethodNotNeeded(&isNeeded);
    if (scrollState == IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL && isNeeded && v) {
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
{
    assert(mOwner);
}

ECode ListPopupWindow::InnerShowRunnable::Run()
{
    // ==================before translated======================
    // View layout should be all done before displaying the drop down.
    //             View view = getAnchorView();
    //             if (view != null && view.getWindowToken() != null) {
    //                 show();
    //             }

    AutoPtr<IView> view;
    mOwner->GetAnchorView((IView**)&view);
    AutoPtr<IBinder> token;
    view->GetWindowToken((IBinder**)&token);
    if (view != NULL && token != NULL) {
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
{
    assert(mOwner);
}

ECode ListPopupWindow::InnerAdapterViewOnItemSelectedListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    VALIDATE_NOT_NULL(parent);
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // if (position != -1) {
    //     DropDownListView dropDownList = mDropDownList;
    //
    //     if (dropDownList != null) {
    //         dropDownList.mListSelectionHidden = false;
    //     }
    // }

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
    VALIDATE_NOT_NULL(parent);
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
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // this(context, null, com.android.internal.R.attr.listPopupWindowStyle, 0);

    return constructor(context, NULL, R::attr::listPopupWindowStyle, 0);
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, com.android.internal.R.attr.listPopupWindowStyle, 0);

    return constructor(context, attrs, R::attr::listPopupWindowStyle, 0);
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    // ==================before translated======================
    // this(context, attrs, defStyleAttr, 0);

    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ListPopupWindow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    // ==================before translated======================
    // mContext = context;
    //
    // final TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.ListPopupWindow,
    //         defStyleAttr, defStyleRes);
    // mDropDownHorizontalOffset = a.getDimensionPixelOffset(
    //         R.styleable.ListPopupWindow_dropDownHorizontalOffset, 0);
    // mDropDownVerticalOffset = a.getDimensionPixelOffset(
    //         R.styleable.ListPopupWindow_dropDownVerticalOffset, 0);
    // if (mDropDownVerticalOffset != 0) {
    //     mDropDownVerticalOffsetSet = true;
    // }
    // a.recycle();
    //
    // mPopup = new PopupWindow(context, attrs, defStyleAttr, defStyleRes);
    // mPopup.setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);
    // // Set the default layout direction to match the default locale one
    // final Locale locale = mContext.getResources().getConfiguration().locale;
    // mLayoutDirection = TextUtils.getLayoutDirectionFromLocale(locale);

    assert(0);
    mContext = context;

    AutoPtr< ArrayOf<Int32> > attrsArray = ArrayOf<Int32>::Alloc(const_cast<Int32*>(R::styleable::ListPopupWindow),
        ArraySize(R::styleable::ListPopupWindow));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrsArray, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetDimensionPixelOffset(R::styleable::ListPopupWindow_dropDownHorizontalOffset, 0, &mDropDownHorizontalOffset);
    a->GetDimensionPixelOffset(R::styleable::ListPopupWindow_dropDownVerticalOffset, 0, &mDropDownVerticalOffset);
    if (mDropDownVerticalOffset != 0) {
        mDropDownVerticalOffsetSet = TRUE;
    }
    a->Recycle();

    CPopupWindow::New(context, attrs, defStyleAttr, defStyleRes, (IPopupWindow**)&mPopup);
    mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);

    AutoPtr<ITextUtils> utils;
    CTextUtils::AcquireSingleton((ITextUtils**)&utils);
    utils->GetLayoutDirectionFromLocale(locale, &mLayoutDirection);
    return NOERROR;
}

ECode ListPopupWindow::SetAdapter(
    /* [in] */ IListAdapter* adapter)
{
    VALIDATE_NOT_NULL(adapter);
    // ==================before translated======================
    // if (mObserver == null) {
    //     mObserver = new PopupDataSetObserver();
    // } else if (mAdapter != null) {
    //     mAdapter.unregisterDataSetObserver(mObserver);
    // }
    // mAdapter = adapter;
    // if (mAdapter != null) {
    //     adapter.registerDataSetObserver(mObserver);
    // }
    //
    // if (mDropDownList != null) {
    //     mDropDownList.setAdapter(mAdapter);
    // }

    assert(0);
    if (NULL == mObserver) {
        mObserver = new PopupDataSetObserver(this);
    }
    else if (NULL != mAdapter) {
        IAdapter* adapterTmp = IAdapter::Probe(mAdapter);
        adapterTmp->UnregisterDataSetObserver(mObserver);
    }

    mAdapter = adapter;
    if (mAdapter != NULL) {
        IAdapter* adapterTmp = IAdapter::Probe(mAdapter);
        adapterTmp->RegisterDataSetObserver(mObserver);
    }

    if (mDropDownList != NULL) {
        IAdapter* temp = IAdapter::Probe(mAdapter);
        mDropDownList->SetAdapter(temp);
    }
    return NOERROR;
}

ECode ListPopupWindow::SetPromptPosition(
    /* [in] */ Int32 position)
{
    // ==================before translated======================
    // mPromptPosition = position;

    mPromptPosition = position;
    return NOERROR;
}

ECode ListPopupWindow::GetPromptPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPromptPosition;

    *result = mPromptPosition;
    return NOERROR;
}

ECode ListPopupWindow::SetModal(
    /* [in] */ Boolean modal)
{
    // ==================before translated======================
    // mModal = modal;
    // mPopup.setFocusable(modal);

    mModal = modal;
    return mPopup->SetFocusable(modal);
}

ECode ListPopupWindow::IsModal(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mModal;

    *result = mModal;
    return NOERROR;
}

ECode ListPopupWindow::SetForceIgnoreOutsideTouch(
    /* [in] */ Boolean forceIgnoreOutsideTouch)
{
    // ==================before translated======================
    // mForceIgnoreOutsideTouch = forceIgnoreOutsideTouch;

    mForceIgnoreOutsideTouch = forceIgnoreOutsideTouch;
    return NOERROR;
}

ECode ListPopupWindow::SetDropDownAlwaysVisible(
    /* [in] */ Boolean dropDownAlwaysVisible)
{
    // ==================before translated======================
    // mDropDownAlwaysVisible = dropDownAlwaysVisible;

    mDropDownAlwaysVisible = dropDownAlwaysVisible;
    return NOERROR;
}

ECode ListPopupWindow::IsDropDownAlwaysVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDropDownAlwaysVisible;

    *result = mDropDownAlwaysVisible;
    return NOERROR;
}

ECode ListPopupWindow::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    // ==================before translated======================
    // mPopup.setSoftInputMode(mode);

    return mPopup->SetSoftInputMode(mode);
}

ECode ListPopupWindow::GetSoftInputMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPopup.getSoftInputMode();

    return mPopup->GetSoftInputMode(result);
}

ECode ListPopupWindow::SetListSelector(
    /* [in] */ IDrawable* selector)
{
    VALIDATE_NOT_NULL(selector);
    // ==================before translated======================
    // mDropDownListHighlight = selector;

    mDropDownListHighlight = selector;
    return NOERROR;
}

ECode ListPopupWindow::GetBackground(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPopup.getBackground();

    return mPopup->GetBackground(result);
}

ECode ListPopupWindow::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    VALIDATE_NOT_NULL(d);
    // ==================before translated======================
    // mPopup.setBackgroundDrawable(d);

    return mPopup->SetBackgroundDrawable(d);
}

ECode ListPopupWindow::SetAnimationStyle(
    /* [in] */ Int32 animationStyle)
{
    // ==================before translated======================
    // mPopup.setAnimationStyle(animationStyle);

    return mPopup->SetAnimationStyle(animationStyle);
}

ECode ListPopupWindow::GetAnimationStyle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPopup.getAnimationStyle();

    return mPopup->GetAnimationStyle(result);
}

ECode ListPopupWindow::GetAnchorView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDropDownAnchorView;

    *result = mDropDownAnchorView;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ListPopupWindow::SetAnchorView(
    /* [in] */ IView* anchor)
{
    VALIDATE_NOT_NULL(anchor);
    // ==================before translated======================
    // mDropDownAnchorView = anchor;

    mDropDownAnchorView = anchor;
    return NOERROR;
}

ECode ListPopupWindow::GetHorizontalOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDropDownHorizontalOffset;

    *result = mDropDownHorizontalOffset;
    return NOERROR;
}

ECode ListPopupWindow::SetHorizontalOffset(
    /* [in] */ Int32 offset)
{
    // ==================before translated======================
    // mDropDownHorizontalOffset = offset;

    mDropDownHorizontalOffset = offset;
    return NOERROR;
}

ECode ListPopupWindow::GetVerticalOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!mDropDownVerticalOffsetSet) {
    //     return 0;
    // }
    // return mDropDownVerticalOffset;

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
    // ==================before translated======================
    // mDropDownVerticalOffset = offset;
    // mDropDownVerticalOffsetSet = true;

    mDropDownVerticalOffset = offset;
    mDropDownVerticalOffsetSet = TRUE;
    return NOERROR;
}

ECode ListPopupWindow::SetDropDownGravity(
    /* [in] */ Int32 gravity)
{
    // ==================before translated======================
    // mDropDownGravity = gravity;

    mDropDownGravity = gravity;
    return NOERROR;
}

ECode ListPopupWindow::GetWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDropDownWidth;

    *result = mDropDownWidth;
    return NOERROR;
}

ECode ListPopupWindow::SetWidth(
    /* [in] */ Int32 width)
{
    // ==================before translated======================
    // mDropDownWidth = width;

    mDropDownWidth = width;
    return NOERROR;
}

ECode ListPopupWindow::SetContentWidth(
    /* [in] */ Int32 width)
{
    // ==================before translated======================
    // Drawable popupBackground = mPopup.getBackground();
    // if (popupBackground != null) {
    //     popupBackground.getPadding(mTempRect);
    //     mDropDownWidth = mTempRect.left + mTempRect.right + width;
    // } else {
    //     setWidth(width);
    // }

    AutoPtr<IDrawable> popupBackground;
    mPopup->GetBackground((IDrawable**)&popupBackground);
    if (popupBackground) {
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
    // ==================before translated======================
    // return mDropDownHeight;

    *result = mDropDownHeight;
    return NOERROR;
}

ECode ListPopupWindow::SetHeight(
    /* [in] */ Int32 height)
{
    // ==================before translated======================
    // mDropDownHeight = height;

    mDropDownHeight = height;
    return NOERROR;
}

ECode ListPopupWindow::SetOnItemClickListener(
    /* [in] */ IAdapterViewOnItemClickListener* clickListener)
{
    VALIDATE_NOT_NULL(clickListener);
    // ==================before translated======================
    // mItemClickListener = clickListener;

    mItemClickListener = clickListener;
    return NOERROR;
}

ECode ListPopupWindow::SetOnItemSelectedListener(
    /* [in] */ IAdapterViewOnItemSelectedListener* selectedListener)
{
    VALIDATE_NOT_NULL(selectedListener);
    // ==================before translated======================
    // mItemSelectedListener = selectedListener;

    mItemSelectedListener = selectedListener;
    return NOERROR;
}

ECode ListPopupWindow::SetPromptView(
    /* [in] */ IView* prompt)
{
    VALIDATE_NOT_NULL(prompt);
    // ==================before translated======================
    // boolean showing = isShowing();
    // if (showing) {
    //     removePromptView();
    // }
    // mPromptView = prompt;
    // if (showing) {
    //     show();
    // }

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
    // ==================before translated======================
    // mHandler.post(mShowDropDownRunnable);

    Boolean resTmp;
    mHandler->Post(mShowDropDownRunnable, &resTmp);
    return NOERROR;
}

ECode ListPopupWindow::Show()
{
    // ==================before translated======================
    // int height = buildDropDown();
    //
    // int widthSpec = 0;
    // int heightSpec = 0;
    //
    // boolean noInputMethod = isInputMethodNotNeeded();
    // mPopup.setAllowScrollingAnchorParent(!noInputMethod);
    //
    // if (mPopup.isShowing()) {
    //     if (mDropDownWidth == ViewGroup.LayoutParams.MATCH_PARENT) {
    //         // The call to PopupWindow's update method below can accept -1 for any
    //         // value you do not want to update.
    //         widthSpec = -1;
    //     } else if (mDropDownWidth == ViewGroup.LayoutParams.WRAP_CONTENT) {
    //         widthSpec = getAnchorView().getWidth();
    //     } else {
    //         widthSpec = mDropDownWidth;
    //     }
    //
    //     if (mDropDownHeight == ViewGroup.LayoutParams.MATCH_PARENT) {
    //         // The call to PopupWindow's update method below can accept -1 for any
    //         // value you do not want to update.
    //         heightSpec = noInputMethod ? height : ViewGroup.LayoutParams.MATCH_PARENT;
    //         if (noInputMethod) {
    //             mPopup.setWindowLayoutMode(
    //                     mDropDownWidth == ViewGroup.LayoutParams.MATCH_PARENT ?
    //                             ViewGroup.LayoutParams.MATCH_PARENT : 0, 0);
    //         } else {
    //             mPopup.setWindowLayoutMode(
    //                     mDropDownWidth == ViewGroup.LayoutParams.MATCH_PARENT ?
    //                             ViewGroup.LayoutParams.MATCH_PARENT : 0,
    //                     ViewGroup.LayoutParams.MATCH_PARENT);
    //         }
    //     } else if (mDropDownHeight == ViewGroup.LayoutParams.WRAP_CONTENT) {
    //         heightSpec = height;
    //     } else {
    //         heightSpec = mDropDownHeight;
    //     }
    //
    //     mPopup.setOutsideTouchable(!mForceIgnoreOutsideTouch && !mDropDownAlwaysVisible);
    //
    //     mPopup.update(getAnchorView(), mDropDownHorizontalOffset,
    //             mDropDownVerticalOffset, widthSpec, heightSpec);
    // } else {
    //     if (mDropDownWidth == ViewGroup.LayoutParams.MATCH_PARENT) {
    //         widthSpec = ViewGroup.LayoutParams.MATCH_PARENT;
    //     } else {
    //         if (mDropDownWidth == ViewGroup.LayoutParams.WRAP_CONTENT) {
    //             mPopup.setWidth(getAnchorView().getWidth());
    //         } else {
    //             mPopup.setWidth(mDropDownWidth);
    //         }
    //     }
    //
    //     if (mDropDownHeight == ViewGroup.LayoutParams.MATCH_PARENT) {
    //         heightSpec = ViewGroup.LayoutParams.MATCH_PARENT;
    //     } else {
    //         if (mDropDownHeight == ViewGroup.LayoutParams.WRAP_CONTENT) {
    //             mPopup.setHeight(height);
    //         } else {
    //             mPopup.setHeight(mDropDownHeight);
    //         }
    //     }
    //
    //     mPopup.setWindowLayoutMode(widthSpec, heightSpec);
    //     mPopup.setClipToScreenEnabled(true);
    //
    //     // use outside touchable to dismiss drop down when touching outside of it, so
    //     // only set this if the dropdown is not always visible
    //     mPopup.setOutsideTouchable(!mForceIgnoreOutsideTouch && !mDropDownAlwaysVisible);
    //     mPopup.setTouchInterceptor(mTouchInterceptor);
    //     mPopup.showAsDropDown(getAnchorView(), mDropDownHorizontalOffset,
    //             mDropDownVerticalOffset, mDropDownGravity);
    //     mDropDownList.setSelection(ListView.INVALID_POSITION);
    //
    //     if (!mModal || mDropDownList.isInTouchMode()) {
    //         clearListSelection();
    //     }
    //     if (!mModal) {
    //         mHandler.post(mHideSelector);
    //     }
    // }

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
        mPopup->Update(anchorView, mDropDownHorizontalOffset, mDropDownVerticalOffset, widthSpec, heightSpec);
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

        Boolean touchMode = mDropDownList->IsInTouchMode();
        if(!mModal || touchMode) {
            ClearListSelection();
        }

        if(!mModal) {
            Boolean result = FALSE;
            mHandler->Post(mHideSelector, &result);
        }
    }
    return NOERROR;
}

ECode ListPopupWindow::Dismiss()
{
    // ==================before translated======================
    // mPopup.dismiss();
    // removePromptView();
    // mPopup.setContentView(null);
    // mDropDownList = null;
    // mHandler.removeCallbacks(mResizePopupRunnable);

    mPopup->Dismiss();
    RemovePromptView();
    AutoPtr<IView> v;
    mPopup->SetContentView(v);
    mDropDownList = NULL;
    mHandler->RemoveCallbacks(mResizePopupRunnable);
    return NOERROR;
}

ECode ListPopupWindow::SetOnDismissListener(
    /* [in] */ IPopupWindowOnDismissListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // mPopup.setOnDismissListener(listener);

    return mPopup->SetOnDismissListener(listener);
}

ECode ListPopupWindow::SetInputMethodMode(
    /* [in] */ Int32 mode)
{
    // ==================before translated======================
    // mPopup.setInputMethodMode(mode);

    return mPopup->SetInputMethodMode(mode);
}

ECode ListPopupWindow::GetInputMethodMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPopup.getInputMethodMode();

    return mPopup->GetInputMethodMode(result);
}

ECode ListPopupWindow::SetSelection(
    /* [in] */ Int32 position)
{
    // ==================before translated======================
    // DropDownListView list = mDropDownList;
    // if (isShowing() && list != null) {
    //     list.mListSelectionHidden = false;
    //     list.setSelection(position);
    //     if (list.getChoiceMode() != ListView.CHOICE_MODE_NONE) {
    //         list.setItemChecked(position, true);
    //     }
    // }

    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    AutoPtr<DropDownListView> list = mDropDownList;
    if (isShowing && list) {
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
    // ==================before translated======================
    // final DropDownListView list = mDropDownList;
    // if (list != null) {
    //     // WARNING: Please read the comment where mListSelectionHidden is declared
    //     list.mListSelectionHidden = true;
    //     list.hideSelector();
    //     list.requestLayout();
    // }

    AutoPtr<DropDownListView> list = mDropDownList;
    if (list != NULL) {
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
    // ==================before translated======================
    // return mPopup.isShowing();

    return mPopup->IsShowing(result);
}

ECode ListPopupWindow::IsInputMethodNotNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPopup.getInputMethodMode() == INPUT_METHOD_NOT_NEEDED;

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
    // ==================before translated======================
    // if (isShowing()) {
    //     if (mItemClickListener != null) {
    //         final DropDownListView list = mDropDownList;
    //         final View child = list.getChildAt(position - list.getFirstVisiblePosition());
    //         final ListAdapter adapter = list.getAdapter();
    //         mItemClickListener.onItemClick(list, child, position, adapter.getItemId(position));
    //     }
    //     return true;
    // }
    // return false;

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
    // ==================before translated======================
    // if (!isShowing()) {
    //     return null;
    // }
    // return mDropDownList.getSelectedItem();

    assert(0);
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
    // ==================before translated======================
    // if (!isShowing()) {
    //     return ListView.INVALID_POSITION;
    // }
    // return mDropDownList.getSelectedItemPosition();

    assert(0);
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
    // ==================before translated======================
    // if (!isShowing()) {
    //     return ListView.INVALID_ROW_ID;
    // }
    // return mDropDownList.getSelectedItemId();

    assert(0);
    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if(!isShowing) {
        *result = IAdapterView::INVALID_POSITION;
        return NOERROR;
    }

    mDropDownList->GetSelectedItemId(result);
    return NOERROR;
}

ECode ListPopupWindow::GetSelectedView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!isShowing()) {
    //     return null;
    // }
    // return mDropDownList.getSelectedView();

    assert(0);
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
    // ==================before translated======================
    // return mDropDownList;

    *result = IListView::Probe(mDropDownList);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ListPopupWindow::SetListItemExpandMax(
    /* [in] */ Int32 max)
{
    // ==================before translated======================
    // mListItemExpandMaximum = max;

    mListItemExpandMaximum = max;
    return NOERROR;
}

ECode ListPopupWindow::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // when the drop down is shown, we drive it directly
    // if (isShowing()) {
    //     // the key events are forwarded to the list in the drop down view
    //     // note that ListView handles space but we don't want that to happen
    //     // also if selection is not currently in the drop down, then don't
    //     // let center or enter presses go there since that would cause it
    //     // to select one of its items
    //     if (keyCode != KeyEvent.KEYCODE_SPACE
    //             && (mDropDownList.getSelectedItemPosition() >= 0
    //                     || !KeyEvent.isConfirmKey(keyCode))) {
    //         int curIndex = mDropDownList.getSelectedItemPosition();
    //         boolean consumed;
    //
    //         final boolean below = !mPopup.isAboveAnchor();
    //
    //         final ListAdapter adapter = mAdapter;
    //
    //         boolean allEnabled;
    //         int firstItem = Integer.MAX_VALUE;
    //         int lastItem = Integer.MIN_VALUE;
    //
    //         if (adapter != null) {
    //             allEnabled = adapter.areAllItemsEnabled();
    //             firstItem = allEnabled ? 0 :
    //                     mDropDownList.lookForSelectablePosition(0, true);
    //             lastItem = allEnabled ? adapter.getCount() - 1 :
    //                     mDropDownList.lookForSelectablePosition(adapter.getCount() - 1, false);
    //         }
    //
    //         if ((below && keyCode == KeyEvent.KEYCODE_DPAD_UP && curIndex <= firstItem) ||
    //                 (!below && keyCode == KeyEvent.KEYCODE_DPAD_DOWN && curIndex >= lastItem)) {
    //             // When the selection is at the top, we block the key
    //             // event to prevent focus from moving.
    //             clearListSelection();
    //             mPopup.setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);
    //             show();
    //             return true;
    //         } else {
    //             // WARNING: Please read the comment where mListSelectionHidden
    //             //          is declared
    //             mDropDownList.mListSelectionHidden = false;
    //         }
    //
    //         consumed = mDropDownList.onKeyDown(keyCode, event);
    //         if (DEBUG) Log.v(TAG, "Key down: code=" + keyCode + " list consumed=" + consumed);
    //
    //         if (consumed) {
    //             // If it handled the key event, then the user is
    //             // navigating in the list, so we should put it in front.
    //             mPopup.setInputMethodMode(PopupWindow.INPUT_METHOD_NOT_NEEDED);
    //             // Here's a little trick we need to do to make sure that
    //             // the list view is actually showing its focus indicator,
    //             // by ensuring it has focus and getting its window out
    //             // of touch mode.
    //             mDropDownList.requestFocusFromTouch();
    //             show();
    //
    //             switch (keyCode) {
    //                 // avoid passing the focus from the text view to the
    //                 // next component
    //                 case KeyEvent.KEYCODE_ENTER:
    //                 case KeyEvent.KEYCODE_DPAD_CENTER:
    //                 case KeyEvent.KEYCODE_DPAD_DOWN:
    //                 case KeyEvent.KEYCODE_DPAD_UP:
    //                     return true;
    //             }
    //         } else {
    //             if (below && keyCode == KeyEvent.KEYCODE_DPAD_DOWN) {
    //                 // when the selection is at the bottom, we block the
    //                 // event to avoid going to the next focusable widget
    //                 if (curIndex == lastItem) {
    //                     return true;
    //                 }
    //             } else if (!below && keyCode == KeyEvent.KEYCODE_DPAD_UP &&
    //                     curIndex == firstItem) {
    //                 return true;
    //             }
    //         }
    //     }
    // }
    //
    // return false;

    assert(0);
    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if (isShowing) {
        Int32 itemPosition = 0;
        mDropDownList->GetSelectedItemPosition(&itemPosition);

        AutoPtr<IKeyEventHelper> helper;
        CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
        Boolean isConfirmKey = FALSE;
        helper->IsConfirmKey(keyCode, &isConfirmKey);
        if (keyCode != IKeyEvent::KEYCODE_SPACE && (itemPosition >= 0 || !isConfirmKey)) {
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

                IAdapter* adapterTmp = IAdapter::Probe(adapter);
                Int32 count = 0;
                adapterTmp->GetCount(&count);
                lastItem = allEnabled ? count - 1 : mDropDownList->LookForSelectablePosition(count - 1, FALSE);
            }

            if((below && IKeyEvent::KEYCODE_DPAD_UP && curIndex <= firstItem) ||
                (!below && IKeyEvent::KEYCODE_DPAD_DOWN && curIndex >= lastItem)) {
                ClearListSelection();
                mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
                Show();
                *result = TRUE;
                return NOERROR;
            }
            else {
                mDropDownList->mListSelectionHidden = FALSE;
            }

            mDropDownList->OnKeyDown(keyCode, event, &consumed);
            if (consumed) {
                mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NEEDED);
                Boolean res = FALSE;
                mDropDownList->RequestFocusFromTouch(&res);
                Show();

                switch(keyCode) {
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
                    if (curIndex == lastItem) {
                        *result = TRUE;
                        return NOERROR;
                    }
                }
                else if(!below && keyCode == IKeyEvent::KEYCODE_DPAD_UP && curIndex == firstItem) {
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
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (isShowing() && mDropDownList.getSelectedItemPosition() >= 0) {
    //     boolean consumed = mDropDownList.onKeyUp(keyCode, event);
    //     if (consumed && KeyEvent.isConfirmKey(keyCode)) {
    //         // if the list accepts the key events and the key event was a click, the text view
    //         // gets the selected item from the drop down as its content
    //         dismiss();
    //     }
    //     return consumed;
    // }
    // return false;

    assert(0);
    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if (isShowing) {
        Int32 itemPosition;
        mDropDownList->GetSelectedItemPosition(&itemPosition);
        if (itemPosition >= 0) {
            Boolean consumed = FALSE;
            mDropDownList->OnKeyUp(keyCode, event, &consumed);
            AutoPtr<IKeyEventHelper> helper;
            CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
            Boolean isConfirmKey = FALSE;
            helper->IsConfirmKey(keyCode, &isConfirmKey);
            if (consumed  && isConfirmKey) {
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
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (keyCode == KeyEvent.KEYCODE_BACK && isShowing()) {
    //     // special case for the back key, we do not even try to send it
    //     // to the drop down list but instead, consume it immediately
    //     final View anchorView = mDropDownAnchorView;
    //     if (event.getAction() == KeyEvent.ACTION_DOWN && event.getRepeatCount() == 0) {
    //         KeyEvent.DispatcherState state = anchorView.getKeyDispatcherState();
    //         if (state != null) {
    //             state.startTracking(event, this);
    //         }
    //         return true;
    //     } else if (event.getAction() == KeyEvent.ACTION_UP) {
    //         KeyEvent.DispatcherState state = anchorView.getKeyDispatcherState();
    //         if (state != null) {
    //             state.handleUpEvent(event);
    //         }
    //         if (event.isTracking() && !event.isCanceled()) {
    //             dismiss();
    //             return true;
    //         }
    //     }
    // }
    // return false;

    assert(0);
    Boolean isShowing = FALSE;
    IsShowing(&isShowing);
    if (keyCode == IKeyEvent::KEYCODE_BACK && isShowing) {
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
    VALIDATE_NOT_NULL(src);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new ForwardingListener(src) {
    //     @Override
    //     public ListPopupWindow getPopup() {
    //         return ListPopupWindow.this;
    //     }
    // };

    *result = new InnerForwardingListener(this);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ListPopupWindow::RemovePromptView()
{
    // ==================before translated======================
    // if (mPromptView != null) {
    //     final ViewParent parent = mPromptView.getParent();
    //     if (parent instanceof ViewGroup) {
    //         final ViewGroup group = (ViewGroup) parent;
    //         group.removeView(mPromptView);
    //     }
    // }

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
    // ==================before translated======================
    // ViewGroup dropDownView;
    // int otherHeights = 0;
    //
    // if (mDropDownList == null) {
    //     Context context = mContext;
    //
    //     /**
    //      * This Runnable exists for the sole purpose of checking if the view layout has got
    //      * completed and if so call showDropDown to display the drop down. This is used to show
    //      * the drop down as soon as possible after user opens up the search dialog, without
    //      * waiting for the normal UI pipeline to do it's job which is slower than this method.
    //      */
    //     mShowDropDownRunnable = new Runnable() {
    //         public void run() {
    //             // View layout should be all done before displaying the drop down.
    //             View view = getAnchorView();
    //             if (view != null && view.getWindowToken() != null) {
    //                 show();
    //             }
    //         }
    //     };
    //
    //     mDropDownList = new DropDownListView(context, !mModal);
    //     if (mDropDownListHighlight != null) {
    //         mDropDownList.setSelector(mDropDownListHighlight);
    //     }
    //     mDropDownList.setAdapter(mAdapter);
    //     mDropDownList.setOnItemClickListener(mItemClickListener);
    //     mDropDownList.setFocusable(true);
    //     mDropDownList.setFocusableInTouchMode(true);
    //     mDropDownList.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
    //         public void onItemSelected(AdapterView<?> parent, View view,
    //                 int position, long id) {
    //
    //             if (position != -1) {
    //                 DropDownListView dropDownList = mDropDownList;
    //
    //                 if (dropDownList != null) {
    //                     dropDownList.mListSelectionHidden = false;
    //                 }
    //             }
    //         }
    //
    //         public void onNothingSelected(AdapterView<?> parent) {
    //         }
    //     });
    //     mDropDownList.setOnScrollListener(mScrollListener);
    //
    //     if (mItemSelectedListener != null) {
    //         mDropDownList.setOnItemSelectedListener(mItemSelectedListener);
    //     }
    //
    //     dropDownView = mDropDownList;
    //
    //     View hintView = mPromptView;
    //     if (hintView != null) {
    //         // if a hint has been specified, we accomodate more space for it and
    //         // add a text view in the drop down menu, at the bottom of the list
    //         LinearLayout hintContainer = new LinearLayout(context);
    //         hintContainer.setOrientation(LinearLayout.VERTICAL);
    //
    //         LinearLayout.LayoutParams hintParams = new LinearLayout.LayoutParams(
    //                 ViewGroup.LayoutParams.MATCH_PARENT, 0, 1.0f
    //         );
    //
    //         switch (mPromptPosition) {
    //         case POSITION_PROMPT_BELOW:
    //             hintContainer.addView(dropDownView, hintParams);
    //             hintContainer.addView(hintView);
    //             break;
    //
    //         case POSITION_PROMPT_ABOVE:
    //             hintContainer.addView(hintView);
    //             hintContainer.addView(dropDownView, hintParams);
    //             break;
    //
    //         default:
    //             Log.e(TAG, "Invalid hint position " + mPromptPosition);
    //             break;
    //         }
    //
    //         // measure the hint's height to find how much more vertical space
    //         // we need to add to the drop down's height
    //         int widthSpec = MeasureSpec.makeMeasureSpec(mDropDownWidth, MeasureSpec.AT_MOST);
    //         int heightSpec = MeasureSpec.UNSPECIFIED;
    //         hintView.measure(widthSpec, heightSpec);
    //
    //         hintParams = (LinearLayout.LayoutParams) hintView.getLayoutParams();
    //         otherHeights = hintView.getMeasuredHeight() + hintParams.topMargin
    //                 + hintParams.bottomMargin;
    //
    //         dropDownView = hintContainer;
    //     }
    //
    //     mPopup.setContentView(dropDownView);
    // } else {
    //     dropDownView = (ViewGroup) mPopup.getContentView();
    //     final View view = mPromptView;
    //     if (view != null) {
    //         LinearLayout.LayoutParams hintParams =
    //                 (LinearLayout.LayoutParams) view.getLayoutParams();
    //         otherHeights = view.getMeasuredHeight() + hintParams.topMargin
    //                 + hintParams.bottomMargin;
    //     }
    // }
    //
    // // getMaxAvailableHeight() subtracts the padding, so we put it back
    // // to get the available height for the whole window
    // int padding = 0;
    // Drawable background = mPopup.getBackground();
    // if (background != null) {
    //     background.getPadding(mTempRect);
    //     padding = mTempRect.top + mTempRect.bottom;
    //
    //     // If we don't have an explicit vertical offset, determine one from the window
    //     // background so that content will line up.
    //     if (!mDropDownVerticalOffsetSet) {
    //         mDropDownVerticalOffset = -mTempRect.top;
    //     }
    // } else {
    //     mTempRect.setEmpty();
    // }
    //
    // // Max height available on the screen for a popup.
    // boolean ignoreBottomDecorations =
    //         mPopup.getInputMethodMode() == PopupWindow.INPUT_METHOD_NOT_NEEDED;
    // final int maxHeight = mPopup.getMaxAvailableHeight(
    //         getAnchorView(), mDropDownVerticalOffset, ignoreBottomDecorations);
    //
    // if (mDropDownAlwaysVisible || mDropDownHeight == ViewGroup.LayoutParams.MATCH_PARENT) {
    //     return maxHeight + padding;
    // }
    //
    // final int childWidthSpec;
    // switch (mDropDownWidth) {
    //     case ViewGroup.LayoutParams.WRAP_CONTENT:
    //         childWidthSpec = MeasureSpec.makeMeasureSpec(
    //                 mContext.getResources().getDisplayMetrics().widthPixels -
    //                 (mTempRect.left + mTempRect.right),
    //                 MeasureSpec.AT_MOST);
    //         break;
    //     case ViewGroup.LayoutParams.MATCH_PARENT:
    //         childWidthSpec = MeasureSpec.makeMeasureSpec(
    //                 mContext.getResources().getDisplayMetrics().widthPixels -
    //                 (mTempRect.left + mTempRect.right),
    //                 MeasureSpec.EXACTLY);
    //         break;
    //     default:
    //         childWidthSpec = MeasureSpec.makeMeasureSpec(mDropDownWidth, MeasureSpec.EXACTLY);
    //         break;
    // }
    // final int listContent = mDropDownList.measureHeightOfChildren(childWidthSpec,
    //         0, ListView.NO_POSITION, maxHeight - otherHeights, -1);
    // // add padding only if the list has items in it, that way we don't show
    // // the popup if it is not needed
    // if (listContent > 0) otherHeights += padding;
    //
    // return listContent + otherHeights;

    assert(0);
    AutoPtr<IViewGroup> dropDownView;
    Int32 otherHeights = 0;
    if (mDropDownList == NULL) {
        AutoPtr<IContext> context = mContext;

        mShowDropDownRunnable = new InnerShowRunnable(this);
        mDropDownList = new DropDownListView(context, !mModal);
        if (mDropDownListHighlight != NULL) {
            mDropDownList->SetSelector(mDropDownListHighlight);
        }

        IAdapter* temp = IAdapter::Probe(mAdapter);
        mDropDownList->SetAdapter(temp);
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

        if (hintView) {
            AutoPtr<ILinearLayout> hintcontainer;
            FAIL_RETURN(CLinearLayout::New(context, (ILinearLayout**)&hintcontainer));
            hintcontainer->SetOrientation(ILinearLayout::VERTICAL);
            AutoPtr<ILinearLayoutLayoutParams> hintParams;
            FAIL_RETURN(CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                0, (Int32)1.0f, (ILinearLayoutLayoutParams**)&hintParams));

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
        if (view) {
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

    Int32 padding = 0;
    AutoPtr<IDrawable> background;
    mPopup->GetBackground((IDrawable**)&background);
    if (background) {
        Boolean paddingRes = FALSE;
        background->GetPadding(mTempRect, &paddingRes);
        AutoPtr<CRect> cTempRect = (CRect*)mTempRect.Get();
        padding = cTempRect->mTop + cTempRect->mBottom;

        if(!mDropDownVerticalOffsetSet) {
            mDropDownVerticalOffset = -cTempRect->mTop;
        }
    }
    else {
        mTempRect->SetEmpty();
    }

    Int32 ipmMode = 0;
    mPopup->GetInputMethodMode(&ipmMode);
    Boolean ignoreBottomDecorations = ipmMode == IPopupWindow::INPUT_METHOD_NOT_NEEDED;
    Int32 maxHeight = 0;
    AutoPtr<IView> anchorView;
    GetAnchorView((IView**)&anchorView);
    mPopup->GetMaxAvailableHeight(anchorView, mDropDownVerticalOffset, ignoreBottomDecorations, &maxHeight);

    if (mDropDownAlwaysVisible || mDropDownHeight == IViewGroupLayoutParams::MATCH_PARENT) {
        return maxHeight + padding;
    }

    Int32 childWidthSpec = 0;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> disMetrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&disMetrics);
    AutoPtr<CDisplayMetrics> cDisMetrics = (CDisplayMetrics*)disMetrics.Get();
    AutoPtr<CRect> cTempRect = (CRect*)mTempRect.Get();
    switch (mDropDownWidth) {
        case IViewGroupLayoutParams::WRAP_CONTENT:
            childWidthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
                cDisMetrics->mWidthPixels - (cTempRect->mLeft + cTempRect->mRight),
                Elastos::Droid::View::View::MeasureSpec::AT_MOST);
            break;
        case IViewGroupLayoutParams::MATCH_PARENT:
            childWidthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
                cDisMetrics->mWidthPixels - (cTempRect->mLeft + cTempRect->mRight),
                Elastos::Droid::View::View::MeasureSpec::EXACTLY);
            break;
        default:
            childWidthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
                mDropDownWidth, Elastos::Droid::View::View::MeasureSpec::EXACTLY);
            break;
    }

    Int32 listContent = mDropDownList->MeasureHeightOfChildren(childWidthSpec, 0, ListView::NO_POSITION, maxHeight - otherHeights, -1);
    if (listContent > 0)
        otherHeights += padding;
    return listContent + otherHeights;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos

