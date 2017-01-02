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

#ifndef __ELASTOS_DROID_APPWIDGET_APPWIDGETPROVIDER_H__
#define __ELASTOS_DROID_APPWIDGET_APPWIDGETPROVIDER_H__

#include <Elastos.Droid.AppWidget.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::AppWidget::IAppWidgetProvider;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace AppWidget {

/**
 * A convenience class to aid in implementing an AppWidget provider.
 * Everything you can do with AppWidgetProvider, you can do with a regular {@link BroadcastReceiver}.
 * AppWidgetProvider merely parses the relevant fields out of the Intent that is received in
 * {@link #onReceive(Context,Intent) onReceive(Context,Intent)}, and calls hook methods
 * with the received extras.
 *
 * <p>Extend this class and override one or more of the {@link #onUpdate}, {@link #onDeleted},
 * {@link #onEnabled} or {@link #onDisabled} methods to implement your own AppWidget functionality.
 * </p>
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about how to write an app widget provider, read the
 * <a href="{@docRoot}guide/topics/appwidgets/index.html#AppWidgetProvider">App Widgets</a>
 * developer guide.</p>
 * </div>
 */
class ECO_PUBLIC AppWidgetProvider
    : public BroadcastReceiver
    , public IAppWidgetProvider
{
public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("AppWidgetProvider")

    /**
     * Constructor to initialize AppWidgetProvider.
     */
    AppWidgetProvider();

    ~AppWidgetProvider();

    /**
     * Implements {@link BroadcastReceiver#onReceive} to dispatch calls to the various
     * other methods on AppWidgetProvider.
     *
     * @param context The Context in which the receiver is running.
     * @param intent The Intent being received.
     */
    // BEGIN_INCLUDE(onReceive)
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called in response to the {@link AppWidgetManager#ACTION_APPWIDGET_UPDATE} and
     * {@link AppWidgetManager#ACTION_APPWIDGET_RESTORED} broadcasts when this AppWidget
     * provider is being asked to provide {@link android.widget.RemoteViews RemoteViews}
     * for a set of AppWidgets.  Override this method to implement your own AppWidget functionality.
     *
     * {@more}
     *
     * @param context   The {@link android.content.Context Context} in which this receiver is
     *                  running.
     * @param appWidgetManager A {@link AppWidgetManager} object you can call {@link
     *                  AppWidgetManager#updateAppWidget} on.
     * @param appWidgetIds The appWidgetIds for which an update is needed.  Note that this
     *                  may be all of the AppWidget instances for this provider, or just
     *                  a subset of them.
     *
     * @see AppWidgetManager#ACTION_APPWIDGET_UPDATE
     */
    CARAPI OnUpdate(
        /* [in] */ IContext* context,
        /* [in] */ IAppWidgetManager* appWidgetManager,
        /* [in] */ ArrayOf<Int32>* appWidgetIds);

    /**
     * Called in response to the {@link AppWidgetManager#ACTION_APPWIDGET_OPTIONS_CHANGED}
     * broadcast when this widget has been layed out at a new size.
     *
     * {@more}
     *
     * @param context   The {@link android.content.Context Context} in which this receiver is
     *                  running.
     * @param appWidgetManager A {@link AppWidgetManager} object you can call {@link
     *                  AppWidgetManager#updateAppWidget} on.
     * @param appWidgetId The appWidgetId of the widget whose size changed.
     * @param newOptions The appWidgetId of the widget whose size changed.
     *
     * @see AppWidgetManager#ACTION_APPWIDGET_OPTIONS_CHANGED
     */
    CARAPI OnAppWidgetOptionsChanged(
        /* [in] */ IContext* context,
        /* [in] */ IAppWidgetManager* appWidgetManager,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IBundle* newOptions);

    /**
     * Called in response to the {@link AppWidgetManager#ACTION_APPWIDGET_DELETED} broadcast when
     * one or more AppWidget instances have been deleted.  Override this method to implement
     * your own AppWidget functionality.
     *
     * {@more}
     *
     * @param context   The {@link android.content.Context Context} in which this receiver is
     *                  running.
     * @param appWidgetIds The appWidgetIds that have been deleted from their host.
     *
     * @see AppWidgetManager#ACTION_APPWIDGET_DELETED
     */
    CARAPI OnDeleted(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<Int32>* appWidgetIds);

    /**
     * Called in response to the {@link AppWidgetManager#ACTION_APPWIDGET_ENABLED} broadcast when
     * the a AppWidget for this provider is instantiated.  Override this method to implement your
     * own AppWidget functionality.
     *
     * {@more}
     * When the last AppWidget for this provider is deleted,
     * {@link AppWidgetManager#ACTION_APPWIDGET_DISABLED} is sent by the AppWidget manager, and
     * {@link #onDisabled} is called.  If after that, an AppWidget for this provider is created
     * again, onEnabled() will be called again.
     *
     * @param context   The {@link android.content.Context Context} in which this receiver is
     *                  running.
     *
     * @see AppWidgetManager#ACTION_APPWIDGET_ENABLED
     */
    CARAPI OnEnabled(
        /* [in] */ IContext* context);

    /**
     * Called in response to the {@link AppWidgetManager#ACTION_APPWIDGET_DISABLED} broadcast, which
     * is sent when the last AppWidget instance for this provider is deleted.  Override this method
     * to implement your own AppWidget functionality.
     *
     * {@more}
     *
     * @param context   The {@link android.content.Context Context} in which this receiver is
     *                  running.
     *
     * @see AppWidgetManager#ACTION_APPWIDGET_DISABLED
     */
    CARAPI OnDisabled(
        /* [in] */ IContext* context);

    /**
     * Called in response to the {@link AppWidgetManager#ACTION_APPWIDGET_RESTORED} broadcast
     * when instances of this AppWidget provider have been restored from backup.  If your
     * provider maintains any persistent data about its widget instances, override this method
     * to remap the old AppWidgetIds to the new values and update any other app state that may
     * be relevant.
     *
     * <p>This callback will be followed immediately by a call to {@link #onUpdate} so your
     * provider can immediately generate new RemoteViews suitable for its newly-restored set
     * of instances.
     *
     * {@more}
     *
     * @param context
     * @param oldWidgetIds
     * @param newWidgetIds
     */
    CARAPI OnRestored(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<Int32>* oldWidgetIds,
        /* [in] */ ArrayOf<Int32>* newWidgetIds);
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
#endif //__ELASTOS_DROID_APPWIDGET_APPWIDGETPROVIDER_H__
