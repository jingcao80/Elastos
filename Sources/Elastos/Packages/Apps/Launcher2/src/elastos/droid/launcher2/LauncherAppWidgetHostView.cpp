
#include "elastos/droid/launcher2/LauncherAppWidgetHostView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherAppWidgetHostView, AppWidgetHostView,
        ILauncherAppWidgetHostView);

LauncherAppWidgetHostView::LauncherAppWidgetHostView()
    : mPreviousOrientation(0)
{
}

ECode LauncherAppWidgetHostView::constructor(
    /* [in] */ IContext* context)
{
    AppWidgetHostView::constructor(context);
    mContext = context;
    mLongPressHelper = new CheckLongPressHelper(this);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    return NOERROR;
}

ECode LauncherAppWidgetHostView::GetErrorView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    return mInflater->Inflate(Elastos::Droid::Launcher2::R::layout::appwidget_error,
            this, FALSE, view);
}

ECode LauncherAppWidgetHostView::UpdateAppWidget(
    /* [in] */ IRemoteViews* remoteViews)
{
    // Store the orientation in which the widget was inflated
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    config->GetOrientation(&mPreviousOrientation);
    return AppWidgetHostView::UpdateAppWidget(remoteViews);
}

ECode LauncherAppWidgetHostView::OrientationChangedSincedInflation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    if (mPreviousOrientation != orientation) {
       *result = TRUE;
       return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LauncherAppWidgetHostView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Consume any touch events for ourselves after longpress is triggered
    if (mLongPressHelper->HasPerformedLongPress()) {
        mLongPressHelper->CancelLongPress();
        *result = TRUE;
        return NOERROR;
    }

    // Watch for longpress events at this level to make sure
    // users can always pick up this widget
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            mLongPressHelper->PostCheckForLongPress();
            break;
        }

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mLongPressHelper->CancelLongPress();
            break;
    }

    // Otherwise continue letting touch events fall through to children
    *result = FALSE;
    return NOERROR;
}

ECode LauncherAppWidgetHostView::CancelLongPress()
{
    AppWidgetHostView::CancelLongPress();

    mLongPressHelper->CancelLongPress();
    return NOERROR;
}

ECode LauncherAppWidgetHostView::GetDescendantFocusability(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = IViewGroup::FOCUS_BLOCK_DESCENDANTS;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos