#include "elastos/droid/systemui/statusbar/policy/IntruderAlertView.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
// #include "elastos/droid/systemui/CSwipeHelper.h"

using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::SystemUI::SystemUIR;
// using Elastos::Droid::SystemUI::CSwipeHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

IntruderAlertView::IntruderAlertView()
{}

IntruderAlertView::IntruderAlertView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
{
    InitImpl(context, attrs, 0);
}

IntruderAlertView::IntruderAlertView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : LinearLayout(context, attrs, defStyle)
{
    InitImpl(context, attrs, defStyle);
}

ECode IntruderAlertView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs, 0);
}

ECode IntruderAlertView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::Init(context, attrs, defStyle);
    return InitImpl(context, attrs, defStyle);
}

ECode IntruderAlertView::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    SetOrientation(ILinearLayout::VERTICAL);
    return NOERROR;
}

//@Override
ECode IntruderAlertView::OnAttachedToWindow()
{
    AutoPtr<IResources> res = GetResources();
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale;
    dm->GetDensity(&densityScale);
    AutoPtr<IContext> context = GetContext();

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> config;
    helper->Get(context, (IViewConfiguration**)&config);
    Int32 pagingTouchSlop;
    config->GetScaledPagingTouchSlop(&pagingTouchSlop);

    mSwipeHelper = NULL;
    // CSwipeHelper::New(ISwipeHelper::X, THIS_PROBE(IView), densityScale,
    //     pagingTouchSlop, (ISwipeHelper**)&mSwipeHelper);

    AutoPtr<IView> tmpView = FindViewById(SystemUIR::id::contentHolder);

    mContentHolder = IViewGroup::Probe(tmpView.Get());
    if (mIntruderRemoteViews != NULL) {
        // whoops, we're on already!
        ApplyIntruderContent(mIntruderRemoteViews, mOnClickListener);
    }
    return NOERROR;
}

ECode IntruderAlertView::SetBar(
    /* [in] */ IBaseStatusBar* bar)
{
    mBar = bar;
    return NOERROR;
}

Boolean IntruderAlertView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    // if (DEBUG) Log.v(TAG, "onInterceptTouchEvent()");
    Boolean result;
    mSwipeHelper->OnInterceptTouchEvent(ev, &result);
    if (!result) {
        result = LinearLayout::OnInterceptTouchEvent(ev);
    }
    return result;
}

Boolean IntruderAlertView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Boolean result;
    mSwipeHelper->OnTouchEvent(ev, &result);
    if (!result) {
        result = LinearLayout::OnTouchEvent(ev);
    }
    return result;
}

Boolean IntruderAlertView::CanChildBeDismissed(
    /* [in] */ IView* v)
{
    return TRUE;
}

AutoPtr<IView> IntruderAlertView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev)
{
    return mContentHolder;
}

AutoPtr<IView> IntruderAlertView::GetChildContentView(
    /* [in] */ IView* v)
{
    AutoPtr<IView> result = v;
    return result;
}

ECode IntruderAlertView::OnBeginDrag(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode IntruderAlertView::OnChildDismissed(
    /* [in] */ IView* v)
{
    // Slog.v(TAG, "User swiped intruder to dismiss");
    return mBar->DismissIntruder();
}

ECode IntruderAlertView::OnDragCancelled(
    /* [in] */ IView* v)
{
    mContentHolder->SetAlpha(1.0f); // sometimes this isn't quite reset
    return NOERROR;
}

//override
void IntruderAlertView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<IResources> res = GetResources();
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale;
    dm->GetDensity(&densityScale);
    AutoPtr<IContext> context = GetContext();

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> config;
    helper->Get(context, (IViewConfiguration**)&config);
    Int32 pagingTouchSlop;
    config->GetScaledPagingTouchSlop(&pagingTouchSlop);

    mSwipeHelper->SetPagingTouchSlop(pagingTouchSlop);
}

void IntruderAlertView::OnDraw(
    /* [in] */ ICanvas* c)
{
    LinearLayout::OnDraw(c);
    // if (DEBUG) {
    //     //Slog.d(TAG, "onDraw: canvas height: " + c.getHeight() + "px; measured height: "
    //     //        + getMeasuredHeight() + "px");
    //     c.save();
    //     c.clipRect(6, 6, c.getWidth() - 6, getMeasuredHeight() - 6,
    //             android.graphics.Region.Op.DIFFERENCE);
    //     c.drawColor(0xFFcc00cc);
    //     c.restore();
    // }
}

ECode IntruderAlertView::ApplyIntruderContent(
    /* [in] */ IRemoteViews* intruderView,
    /* [in] */ IViewOnClickListener* listener)
{
    // if (DEBUG) {
    //     Slog.v(TAG, "applyIntruderContent: view=" + intruderView + " listener=" + listener);
    // }
    mIntruderRemoteViews = intruderView;
    mOnClickListener = listener;
    if (mContentHolder == NULL) {
        // too soon!
        return NOERROR;
    }
    mContentHolder->SetX(0);
    mContentHolder->SetVisibility(IView::VISIBLE);
    mContentHolder->SetAlpha(1.0f);
    mContentHolder->RemoveAllViews();

    AutoPtr<IContext> context = GetContext();
    AutoPtr<IView> content;
    intruderView->Apply(context, mContentHolder, (IView**)&content);
    if (listener != NULL) {
        content->SetOnClickListener(listener);

        //content.setBackgroundResource(R.drawable.intruder_row_bg);
        AutoPtr<IResources> res = GetResources();
        AutoPtr<IDrawable> bg;
        res->GetDrawable(SystemUIR::drawable::intruder_row_bg, (IDrawable**)&bg);
        if (bg == NULL) {
            // Log.e(TAG, String.format("Can't find background drawable id=0x%08x", R.drawable.intruder_row_bg));
        } else {
            content->SetBackgroundDrawable(bg);
        }
    }
    mContentHolder->AddView(content);
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
