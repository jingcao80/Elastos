
#ifndef __ELASTOS_DROID_APPWIDGET_APPWIDGETHOST_H__
#define __ELASTOS_DROID_APPWIDGET_APPWIDGETHOST_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::AppWidget::IIAppWidgetHost;
using Elastos::Droid::Internal::AppWidget::IIAppWidgetService;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Widget::IRemoteViewsOnClickHandler;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace AppWidget {

/**
 * AppWidgetHost provides the interaction with the AppWidget service for apps,
 * like the home screen, that want to embed AppWidgets in their UI.
 */
class ECO_PUBLIC AppWidgetHost
    : public Object
    , public IAppWidgetHost
{
public:
    class Callbacks
        : public Object
        , public IIAppWidgetHost
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        Callbacks();

        ~Callbacks();

        CARAPI constructor(
            /* [in] */ IAppWidgetHost* host);

        CARAPI UpdateAppWidget(
            /* [in] */ Int32 appWidgetId,
            /* [in] */ IRemoteViews* views);

        CARAPI ProviderChanged(
            /* [in] */ Int32 appWidgetId,
            /* [in] */ IAppWidgetProviderInfo* info);

        CARAPI ProvidersChanged();

        CARAPI ViewDataChanged(
            /* [in] */ Int32 appWidgetId,
            /* [in] */ Int32 viewId);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AppWidgetHost* mHost;
    };

private:
    class ECO_LOCAL UpdateHandler : public Handler
    {
    public:
        TO_STRING_IMPL("AppWidgetHost::UpdateHandler")

        UpdateHandler(
            /* [in] */ AppWidgetHost* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        AppWidgetHost* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AppWidgetHost();

    ~AppWidgetHost();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 hostId);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 hostId,
        /* [in] */ IRemoteViewsOnClickHandler* handler,
        /* [in] */ ILooper* looper);

    /**
     * Start receiving onAppWidgetChanged calls for your AppWidgets.  Call this when your activity
     * becomes visible, i.e. from onStart() in your Activity.
     */
    CARAPI StartListening();

    /**
     * Stop receiving onAppWidgetChanged calls for your AppWidgets.  Call this when your activity is
     * no longer visible, i.e. from onStop() in your Activity.
     */
    CARAPI StopListening();

    /**
     * Get a appWidgetId for a host in the calling process.
     *
     * @return a appWidgetId
     */
    CARAPI AllocateAppWidgetId(
        /* [out] */ Int32* appWidgetId);

    /**
     * Starts an app widget provider configure activity for result on behalf of the caller.
     * Use this method if the provider is in another profile as you are not allowed to start
     * an activity in another profile. You can optionally provide a request code that is
     * returned in {@link Activity#onActivityResult(int, int, android.content.Intent)} and
     * an options bundle to be passed to the started activity.
     * <p>
     * Note that the provided app widget has to be bound for this method to work.
     * </p>
     *
     * @param activity The activity from which to start the configure one.
     * @param appWidgetId The bound app widget whose provider's config activity to start.
     * @param requestCode Optional request code retuned with the result.
     * @param intentFlags Optional intent flags.
     *
     * @throws android.content.ActivityNotFoundException If the activity is not found.
     *
     * @see AppWidgetProviderInfo#getProfile()
     */
    CARAPI StartAppWidgetConfigureActivityForResult(
        /* [in] */ IActivity* activity,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int32 intentFlags,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options);

    /**
     * Gets a list of all the appWidgetIds that are bound to the current host
     *
     * @hide
     */
    CARAPI GetAppWidgetIds(
        /* [out, callee] */ ArrayOf<Int32>** ids);

    /**
     * Stop listening to changes for this AppWidget.
     */
    CARAPI DeleteAppWidgetId(
        /* [in] */ Int32 appWidgetId);

    /**
     * Remove all records about this host from the AppWidget manager.
     * <ul>
     *   <li>Call this when initializing your database, as it might be because of a data wipe.</li>
     *   <li>Call this to have the AppWidget manager release all resources associated with your
     *   host.  Any future calls about this host will cause the records to be re-allocated.</li>
     * </ul>
     */
    CARAPI DeleteHost();

    /**
     * Remove all records about all hosts for your package.
     * <ul>
     *   <li>Call this when initializing your database, as it might be because of a data wipe.</li>
     *   <li>Call this to have the AppWidget manager release all resources associated with your
     *   host.  Any future calls about this host will cause the records to be re-allocated.</li>
     * </ul>
     */
    static CARAPI DeleteAllHosts();

    /**
     * Create the AppWidgetHostView for the given widget.
     * The AppWidgetHost retains a pointer to the newly-created View.
     */
    CARAPI CreateView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* appWidget,
        /* [out] */ IAppWidgetHostView** hostView);

    CARAPI UpdateAppWidgetView(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IRemoteViews* views);

    CARAPI ViewDataChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int32 viewId);

protected:
    /**
     * Called to create the AppWidgetHostView.  Override to return a custom subclass if you
     * need it.  {@more}
     */
    virtual CARAPI_(AutoPtr<IAppWidgetHostView>) OnCreateView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* appWidget);

    /**
     * Called when the AppWidget provider for a AppWidget has been upgraded to a new apk.
     */
    virtual CARAPI OnProviderChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* appWidget);

    /**
     * Called when the set of available widgets changes (ie. widget containing packages
     * are added, updated or removed, or widget components are enabled or disabled.)
     */
    virtual CARAPI_(void) OnProvidersChanged();

    /**
     * Clear the list of Views that have been created by this AppWidgetHost.
     */
    virtual CARAPI_(void) ClearViews();

private:
    static CARAPI_(void) BindService();

    CARAPI_(Boolean) IsLocalBinder();

protected:
    static const Int32 HANDLE_UPDATE = 1;
    static const Int32 HANDLE_PROVIDER_CHANGED = 2;
    static const Int32 HANDLE_PROVIDERS_CHANGED = 3;
    static const Int32 HANDLE_VIEW_DATA_CHANGED = 4;

    static Object sServiceLock;
    static AutoPtr<IIAppWidgetService> sService;

    AutoPtr<IContext> mContext;
    AutoPtr<UpdateHandler> mHandler;
    Int32 mHostId;
    AutoPtr<IIAppWidgetHost> mCallbacks;
    /* final HashMap<Integer,AppWidgetHostView> mViews = new HashMap<Integer, AppWidgetHostView>();*/
    AutoPtr<IHashMap> mViews;
    Object mViewsLock;

private:
    AutoPtr<IDisplayMetrics> mDisplayMetrics;
    AutoPtr<IRemoteViewsOnClickHandler> mOnClickHandler;
    friend class CAppWidgetHostCallbacks;
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APPWIDGET_APPWIDGETHOST_H__
