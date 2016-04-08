
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_SYSTEMSERVICESPROXY_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_SYSTEMSERVICESPROXY_H__

#include "_SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

/**
 * Acts as a shim around the real system services that we need to access data from, and provides
 * a point of injection when testing UI.
 */
class SystemServicesProxy
    : public Object
{
public:
    /** Private constructor */
    SystemServicesProxy(
        /* [in] */ IContext* context);

    /** Returns a list of the recents tasks */
    CARAPI_(AutoPtr<IList>) GetRecentTasks(
        /* [in] */ Int32 numLatestTasks,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean isTopTaskHome);

    /** Returns a list of the running tasks */
    CARAPI_(AutoPtr<IList>) GetRunningTasks(
        /* [in] */ Int32 numTasks);

    /** Returns whether the specified task is in the home stack */
    CARAPI_(Boolean) IsInHomeStack(
        /* [in] */ Int32 taskId);

    /** Returns the top task thumbnail for the given task id */
    CARAPI_(AutoPtr<IBitmap>) GetTaskThumbnail(
        /* [in] */ Int32 taskId);

    /**
     * Returns a task thumbnail from the activity manager
     */
    static CARAPI_(AutoPtr<IBitmap>) GetThumbnail(
        /* [in] */ IActivityManager* activityManager,
        /* [in] */ Int32 taskId);

    /** Moves a task to the front with the specified activity options */
    CARAPI_(void) MoveTaskToFront(
        /* [in] */ Int32 taskId,
        /* [in] */ IActivityOptions* opts);

    /** Removes the task and kills the process */
    CARAPI_(void) RemoveTask(
        /* [in] */ Int32 taskId,
        /* [in] */ Boolean isDocument);

    /**
     * Returns the activity info for a given component name.
     *
     * @param cn The component name of the activity.
     * @param userId The userId of the user that this is for.
     */
    CARAPI_(AutoPtr<IActivityInfo>) GetActivityInfo(
        /* [in] */ IComponentName* cn,
        /* [in] */ Int32 userId);

    /**
     * Returns the activity info for a given component name.
     *
     * @param cn The component name of the activity.
     */
    CARAPI_(AutoPtr<IActivityInfo>) GetActivityInfo(
        /* [in] */ IComponentName* cn);

    /** Returns the activity label */
    CARAPI_(String) GetActivityLabel(
        /* [in] */ IActivityInfo* info);

    /**
     * Returns the activity icon for the ActivityInfo for a user, badging if
     * necessary.
     */
    CARAPI_(AutoPtr<IDrawable>) GetActivityIcon(
        /* [in] */ IActivityInfo* info,
        /* [in] */ Int32 userId);

    /**
     * Returns the given icon for a user, badging if necessary.
     */
    CARAPI_(AutoPtr<IDrawable>) GetBadgedIcon(
        /* [in] */ IDrawable* icon,
        /* [in] */ Int32 userId);

    /** Returns the package name of the home activity. */
    CARAPI_(String) GetHomeActivityPackageName();

    /**
     * Resolves and returns the first Recents widget from the same package as the global
     * assist activity.
     */
    CARAPI_(AutoPtr<IAppWidgetProviderInfo>) ResolveSearchAppWidget();

    /**
     * Resolves and binds the search app widget that is to appear in the recents.
     */
    CARAPI_(AutoPtr<IPair>) BindSearchAppWidget(
        /* [in] */ IAppWidgetHost* host);

    /**
     * Returns the app widget info for the specified app widget id.
     */
    CARAPI_(AutoPtr<IAppWidgetProviderInfo>) GetAppWidgetInfo(
        /* [in] */ Int32 appWidgetId);

    /**
     * Destroys the specified app widget.
     */
    CARAPI_(void) UnbindSearchAppWidget(
        /* [in] */ IAppWidgetHost* host,
        /* [in] */ Int32 appWidgetId);

    /**
     * Returns whether touch exploration is currently enabled.
     */
    CARAPI_(Boolean) IsTouchExplorationEnabled();

    /**
     * Returns a global setting.
     */
    CARAPI_(Int32) GetGlobalSetting(
        /* [in] */ IContext* context,
        /* [in] */ const String& setting);

    /**
     * Returns a system setting.
     */
    CARAPI_(Int32) GetSystemSetting(
        /* [in] */ IContext* context,
        /* [in] */ const String& setting);

    /**
     * Returns the window rect.
     */
    CARAPI_(AutoPtr<IRect>) GetWindowRect();

    /**
     * Locks the current task.
     */
    CARAPI_(void) LockCurrentTask();

    /**
     * Takes a screenshot of the current surface.
     */
    CARAPI_(AutoPtr<IBitmap>) TakeScreenshot();

    /**
     * Takes a screenshot of the current app.
     */
    CARAPI_(AutoPtr<IBitmap>) TakeAppScreenshot();

    /** Starts an activity from recents. */
    CARAPI_(Boolean) StartActivityFromRecents(
        /* [in] */ IContext* context,
        /* [in] */ Int32 taskId,
        /* [in] */ const String& taskName,
        /* [in] */ IActivityOptions* options);

public:
    const static String TAG;

    const static AutoPtr<IBitmapFactoryOptions> sBitmapOptions;

    AutoPtr<IAccessibilityManager> mAccm;
    AutoPtr<IActivityManager> mAm;
    AutoPtr<IIActivityManager> mIam;
    AutoPtr<IAppWidgetManager> mAwm;
    AutoPtr<IPackageManager> mPm;
    AutoPtr<IIPackageManager> mIpm;
    AutoPtr<ISearchManager> mSm;
    AutoPtr<IWindowManager> mWm;
    AutoPtr<IDisplay> mDisplay;
    String mRecentsPackage;
    AutoPtr<IComponentName> mAssistComponent;

    AutoPtr<IBitmap> mDummyIcon;
    Int32 mDummyThumbnailWidth;
    Int32 mDummyThumbnailHeight;
    AutoPtr<IPaint> mBgProtectionPaint;
    AutoPtr<ICanvas> mBgProtectionCanvas;

};

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_SYSTEMSERVICESPROXY_H__
