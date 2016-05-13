
#include "elastos/droid/launcher2/LauncherAppWidgetHost.h"
#include "elastos/droid/launcher2/LauncherAppWidgetHostView.h"
#include "elastos/droid/launcher2/LauncherModel.h"
#include "Elastos.Droid.Service.h"

using Elastos::Droid::Launcher2::LauncherAppWidgetHostView;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherAppWidgetHost, AppWidgetHost, ILauncherAppWidgetHost);

LauncherAppWidgetHost::LauncherAppWidgetHost(
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 hostId)
    : mLauncher(launcher)
{
    AppWidgetHost::constructor(IContext::Probe(launcher), hostId);
}

AutoPtr<IAppWidgetHostView> LauncherAppWidgetHost::OnCreateView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* appWidget)
{
    AutoPtr<LauncherAppWidgetHostView> view = new LauncherAppWidgetHostView();
    view->constructor(context);
    AutoPtr<IAppWidgetHostView> tmp = IAppWidgetHostView::Probe(view);
    return view;
}

ECode LauncherAppWidgetHost::StopListening()
{
    AppWidgetHost::StopListening();
    ClearViews();
    return NOERROR;
}

ECode LauncherAppWidgetHost::OnProvidersChanged()
{
    // Once we get the message that widget packages are updated, we need to rebind items
    // in AppsCustomize accordingly.
    AutoPtr<IArrayList> list;
    LauncherModel::GetSortedWidgetsAndShortcuts(IContext::Probe(mLauncher), (IArrayList**)&list);
    return mLauncher->BindPackagesUpdated(list);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos