//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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