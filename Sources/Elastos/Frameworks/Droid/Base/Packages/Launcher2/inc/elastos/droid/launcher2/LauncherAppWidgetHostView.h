#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOSTVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOSTVIEW_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * {@inheritDoc}
 */
class LauncherAppWidgetHostView
    : public AppWidgetHostView
{
public:
    LauncherAppWidgetHostView(
        /* [in] */ IContext* context);

    //@Override
    CARAPI UpdateAppWidget(
        /* [in] */ IRemoteViews* remoteViews);

    CARAPI OrientationChangedSincedInflation(
        /* [out] */ Boolean* result);

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI CancelLongPress();

    //@Override
    CARAPI GetDescendantFocusability(
        /* [out] */ Int32* result);

protected:
    //@Override
    CARAPI GetErrorView(
        /* [out] */ IView** view);

private:
    AutoPtr<CheckLongPressHelper> mLongPressHelper;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IContext> mContext;
    Int32 mPreviousOrientation;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOSTVIEW_H__