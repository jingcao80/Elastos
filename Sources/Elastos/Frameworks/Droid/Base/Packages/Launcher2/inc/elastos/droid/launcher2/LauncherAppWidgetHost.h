#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOST_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOST_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Specific {@link AppWidgetHost} that creates our {@link LauncherAppWidgetHostView}
 * which correctly captures all long-press events. This ensures that users can
 * always pick up and move widgets.
 */
class LauncherAppWidgetHost
    : public AppWidgetHost
{
public:
    LauncherAppWidgetHost(
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 hostId);

    //@Override
    CARAPI StopListening();

protected:
    //@Override
    CARAPI_(AutoPtr<IAppWidgetHostView>) OnCreateView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* appWidget);

    CARAPI_(void) OnProvidersChanged();

public:
    AutoPtr<ILauncher> mLauncher;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOST_H__