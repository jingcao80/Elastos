
#include "elastos/droid/launcher2/LauncherAppWidgetHost.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

LauncherAppWidgetHost::LauncherAppWidgetHost(
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 hostId)
    : AppWidgetHost(launcher, hostId)
    , mLauncher(launcher)
{
}

AutoPtr<IAppWidgetHostView> LauncherAppWidgetHost::OnCreateView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* appWidget)
{
    AutoPtr<IAppWidgetHostView> view = new LauncherAppWidgetHostView(context);
    return view;
}

ECode LauncherAppWidgetHost::StopListening()
{
    AppWidgetHost::StopListening();
    return ClearViews();
}

void LauncherAppWidgetHost::OnProvidersChanged()
{
    // Once we get the message that widget packages are updated, we need to rebind items
    // in AppsCustomize accordingly.
    AutoPtr<IArrayList> list;
    LauncherModel::GetSortedWidgetsAndShortcuts(IContext::Probe(mLauncher), (IArrayList**)&list);
    mLauncher->BindPackagesUpdated(list);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos