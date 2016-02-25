#include "elastos/droid/systemui/statusbar/policy/NotificationRowLayout.h"
#include "elastos/droid/systemui/statusbar/NotificationData.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/animation/CLayoutTransition.h"
#include "elastos/droid/systemui/CSwipeHelper.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::SystemUI::CSwipeHelper;
using Elastos::Droid::SystemUI::StatusBar::NotificationData;
using Elastos::Droid::Animation::CLayoutTransition;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String NotificationRowLayout::TAG("NotificationRowLayout");
const Boolean NotificationRowLayout::DEBUG = FALSE;
const Boolean NotificationRowLayout::SLOW_ANIMATIONS = FALSE; //DEBUG
const Int32 NotificationRowLayout::APPEAR_ANIM_LEN = 250;//SLOW_ANIMATIONS ? 5000 : 250;
const Int32 NotificationRowLayout::DISAPPEAR_ANIM_LEN = 250; //APPEAR_ANIM_LEN

NotificationRowLayout::NotificationRowLayout()
{}

NotificationRowLayout::NotificationRowLayout(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
    , mAnimateBounds(TRUE)
    , mRemoveViews(TRUE)
{
    InitImpl(context, attrs, 0);
}

NotificationRowLayout::NotificationRowLayout(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : LinearLayout(context, attrs, defStyle)
    , mAnimateBounds(TRUE)
    , mRemoveViews(TRUE)
{
    InitImpl(context, attrs, defStyle);
}

ECode NotificationRowLayout::Init(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(ctx, attrs, 0);
}

ECode NotificationRowLayout::Init(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::Init(ctx, attrs, defStyle);
    return InitImpl(ctx, attrs, defStyle);
}

ECode NotificationRowLayout::InitImpl(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    CLayoutTransition::New((ILayoutTransition**)&mRealLayoutTransition);
    mRealLayoutTransition->SetAnimateParentHierarchy(TRUE);
    SetLayoutTransitionsEnabled(TRUE);

    SetOrientation(ILinearLayout::VERTICAL);

    // if (DEBUG) {
    // setOnHierarchyChangeListener(new ViewGroup.OnHierarchyChangeListener() {
    // @Override
    // public void onChildViewAdded(View parent, View child) {
    // Slog.d(TAG, "view added: " + child + "; new count: " + getChildCount());
    // }
    // @Override
    // public void onChildViewRemoved(View parent, View child) {
    // Slog.d(TAG, "view removed: " + child + "; new count: " + (getChildCount() - 1));
    // }
    // });

    // setBackgroundColor(0x80FF8000);
    // }

    Float densityScale;
    AutoPtr<IDisplayMetrics> dm;
    GetResources()->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&densityScale);

    Float pagingTouchSlop;
    AutoPtr<IViewConfiguration> vc;
    AutoPtr<IViewConfigurationHelper> helper;
    ASSERT_SUCCEEDED(CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper));
    helper->Get(mContext, (IViewConfiguration**)&vc);
    vc->GetScaledPagingTouchSlop((Int32*)&pagingTouchSlop);
    CSwipeHelper::New(ISwipeHelper::X, THIS_PROBE(ISwipeHelperCallback),
        densityScale, pagingTouchSlop, (ISwipeHelper**)&mSwipeHelper);

    return NOERROR;
}

ECode NotificationRowLayout::SetLongPressListener(
    /* [in] */ IViewOnLongClickListener* listener)
{
    return mSwipeHelper->SetLongPressListener(listener);
}

ECode NotificationRowLayout::SetOnSizeChangedListener(
    /* [in] */ IOnSizeChangedListener* l)
{
    mOnSizeChangedListener = l;
    return NOERROR;
}

ECode NotificationRowLayout::SetAnimateBounds(
    /* [in] */ Boolean anim)
{
    mAnimateBounds = anim;
    return NOERROR;
}

Boolean NotificationRowLayout::CanChildBeDismissed(
    /* [in] */ IView* v)
{
    AutoPtr<IView> veto;
    v->FindViewById(SystemUIR::id::veto, (IView**)&veto);
    Int32 val;
    veto->GetVisibility(&val);
    return (veto != NULL && val != IView::GONE);
}

Boolean NotificationRowLayout::CanChildBeExpanded(
    /* [in] */ IView* v)
{
    return NotificationData::GetIsExpandable(v);
}

Boolean NotificationRowLayout::SetUserExpandedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userExpanded)
{
    return NotificationData::SetUserExpanded(v, userExpanded);
}

Boolean NotificationRowLayout::SetUserLockedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userLocked)
{
    return NotificationData::SetUserLocked(v, userLocked);
}

ECode NotificationRowLayout::OnChildDismissed(
    /* [in] */ IView* v)
{
    // if (DEBUG) Slog.v(TAG, "onChildDismissed: " + v + " mRemoveViews=" + mRemoveViews);
    AutoPtr<IView> veto;
    v->FindViewById(SystemUIR::id::veto, (IView**)&veto);
    Int32 val;
    veto->GetVisibility(&val);
    if (veto != NULL && val != IView::GONE && mRemoveViews) {
        Boolean b;
        veto->PerformClick(&b);
    }
    return NOERROR;
}

ECode NotificationRowLayout::OnBeginDrag(
    /* [in] */ IView* v)
{
    // We need to prevent the surrounding ScrollView from intercepting us now;
    // the scroll position will be locked while we swipe
    return RequestDisallowInterceptTouchEvent(TRUE);
}

ECode NotificationRowLayout::OnDragCancelled(
    /* [in] */ IView* v)
{
    return NOERROR;
}

AutoPtr<IView> NotificationRowLayout::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev)
{
    Float x,y;
    ev->GetX(&x);
    ev->GetY(&y);
    return GetChildAtPosition(x, y);
}

AutoPtr<IView> NotificationRowLayout::GetChildAtRawPosition(
    /* [in] */ Float touchX,
    /* [in] */ Float touchY)
{
    Int32 x, y;
    GetLocationOnScreen(&x, &y);
    return GetChildAtPosition((Float) (touchX - x), (Float) (touchY - y));
}

AutoPtr<IView> NotificationRowLayout::GetChildAtPosition(
    /* [in] */ Float touchX,
    /* [in] */ Float touchY)
{
    // find the view under the pointer, accounting for GONE views
    Int32 count = GetChildCount();
    Int32 y = 0;
    Int32 childIdx = 0;
    AutoPtr<IView> slidingChild;
    for (; childIdx < count; childIdx++) {
        slidingChild = GetChildAt(childIdx);
        Int32 val;
        slidingChild->GetVisibility(&val);
        if (val == IView::GONE) {
            continue;
        }
        Int32 h;
        slidingChild->GetMeasuredHeight(&h);
        y += h;
        if (touchY < y) {
            return slidingChild;
        }
    }
    return NULL;
}

AutoPtr<IView> NotificationRowLayout::GetChildContentView(
    /* [in] */ IView* v)
{
    return v;
}

ECode NotificationRowLayout::SetViewRemoval(
    /* [in] */ Boolean removeViews)
{
    // if (DEBUG) Slog.v(TAG, "setViewRemoval: " + removeViews);
    mRemoveViews = removeViews;
    return NOERROR;
}

ECode NotificationRowLayout::SetLayoutTransitionsEnabled(
    /* [in] */ Boolean b)
{
    // Boolean result;
    // mRealLayoutTransition->IsRunning(&result);
    // if (b) {
    //     SetLayoutTransition(mRealLayoutTransition);
    // } else {
    //     if (result) {
    //         mRealLayoutTransition->Cancel();
    //     }
    //     SetLayoutTransition(NULL);
    // }
    return E_NOT_IMPLEMENTED;
}

ECode NotificationRowLayout::DismissRowAnimated(
    /* [in] */ IView* child)
{
    return DismissRowAnimated(child, 0);
}

ECode NotificationRowLayout::DismissRowAnimated(
    /* [in] */ IView* child,
    /* [in] */ Int32 vel)
{
    return mSwipeHelper->DismissChild(child, vel);
}

ECode NotificationRowLayout::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    LinearLayout::OnWindowFocusChanged(hasWindowFocus);
    if (!hasWindowFocus) {
        mSwipeHelper->RemoveLongPressCallback();
    }
    return NOERROR;
}

Boolean NotificationRowLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    // if (DEBUG) Log.v(TAG, "onInterceptTouchEvent()");
    // if (DEBUG) logLayoutTransition();
    Boolean b;
    mSwipeHelper->OnInterceptTouchEvent(ev, &b);
    return b || LinearLayout::OnInterceptTouchEvent(ev);
}

Boolean NotificationRowLayout::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    // if (DEBUG) Log.v(TAG, "onTouchEvent()");
    // if (DEBUG) logLayoutTransition();
    Boolean b;
    mSwipeHelper->OnTouchEvent(ev, &b);
    return  b || LinearLayout::OnTouchEvent(ev);
}

ECode NotificationRowLayout::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();
    // if (DEBUG) setWillNotDraw(false);
    return NOERROR;
}

void NotificationRowLayout::OnDraw(
    /* [in] */ ICanvas* c)
{
    LinearLayout::OnDraw(c);
    // if (DEBUG) logLayoutTransition();
    // if (DEBUG) {
    //     //Slog.d(TAG, "onDraw: canvas height: " + c.getHeight() + "px; measured height: "
    //     //        + getMeasuredHeight() + "px");
    //     c.save();
    //     c.clipRect(6, 6, c.getWidth() - 6, getMeasuredHeight() - 6,
    //             android.graphics.Region.Op.DIFFERENCE);
    //     c.drawColor(0xFFFF8000);
    //     c.restore();
    // }
}

void NotificationRowLayout::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    LinearLayout::OnConfigurationChanged(newConfig);
    Float densityScale;
    AutoPtr<IDisplayMetrics> dm;
    GetResources()->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&densityScale);
    mSwipeHelper->SetDensityScale(densityScale);
    Float pagingTouchSlop;
    AutoPtr<IViewConfiguration> vc;
    AutoPtr<IViewConfigurationHelper> helper;
    ASSERT_SUCCEEDED(CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper));
    helper->Get(mContext, (IViewConfiguration**)&vc);
    vc->GetScaledPagingTouchSlop((Int32*)&pagingTouchSlop);
    mSwipeHelper->SetPagingTouchSlop(pagingTouchSlop);
}

void NotificationRowLayout::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    if (mOnSizeChangedListener != NULL) {
        mOnSizeChangedListener->OnSizeChanged((IView*)this, w, h, oldw, oldh);
    }
}

void NotificationRowLayout::LogLayoutTransition()
{
    // Log.v(TAG, "layout " +
    //       (mRealLayoutTransition.isChangingLayout() ? "is " : "is not ") +
    //       "in transition and animations " +
    //       (mRealLayoutTransition.isRunning() ? "are " : "are not ") +
    //       "running.");
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
