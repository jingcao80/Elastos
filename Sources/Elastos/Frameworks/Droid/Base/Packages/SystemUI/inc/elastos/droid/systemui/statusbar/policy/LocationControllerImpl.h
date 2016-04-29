
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_LOCATIONCONTROLLERIMPL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_LOCATIONCONTROLLERIMPL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * A controller to manage changes of location related states and update the views accordingly.
 */
class LocationControllerImpl
    : public Object
    , public ILocationController
{
public:
    CAR_INTERFACE_DECL();

    LocationControllerImpl();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Add a callback to listen for changes in location settings.
     */
    CARAPI AddSettingsChangedCallback(
        /* [in] */ ILocationSettingsChangeCallback* cb);

    CARAPI RemoveSettingsChangedCallback(
        /* [in] */ ILocationSettingsChangeCallback* cb);

    /**
     * Enable or disable location in settings.
     *
     * <p>This will attempt to enable/disable every type of location setting
     * (e.g. high and balanced power).
     *
     * <p>If enabling, a user consent dialog will pop up prompting the user to accept.
     * If the user doesn't accept, network location won't be enabled.
     *
     * @return TRUE if attempt to change setting was successful.
     */
    CARAPI SetLocationEnabled(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* result);

    /**
     * Returns TRUE if location isn't disabled in settings.
     */
    CARAPI IsLocationEnabled(
        /* [out] */ Boolean* result);

    // Reads the active location requests and updates the status view if necessary.
    CARAPI_(void) UpdateActiveLocationRequests();

    CARAPI_(void) LocationSettingsChanged();

private:
    /**
     * Returns TRUE if the current user is restricted from using location.
     */
    CARAPI_(Boolean) IsUserLocationRestricted(
        /* [in] */ Int32 userId);

    /**
     * Returns TRUE if there currently exist active high power location requests.
     */
    CARAPI_(Boolean) AreActiveHighPowerLocationRequests();

    // Updates the status view based on the current state of location requests.
    CARAPI_(void) RefreshViews();

    CARAPI_(void) LocationSettingsChanged(
        /* [in] */ ILocationSettingsChangeCallback* cb);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitStatic();

public:
    // The name of the placeholder corresponding to the location request status icon.
    // This string corresponds to config_statusBarIcons in core/res/res/values/config.xml.
    static const String LOCATION_STATUS_ICON_PLACEHOLDER;
    static const Int32 LOCATION_STATUS_ICON_ID;

private:
    static AutoPtr<ArrayOf<Int32> > mHighPowerRequestAppOpArray;

    AutoPtr<IContext> mContext;

    AutoPtr<IAppOpsManager> mAppOpsManager;
    AutoPtr<IStatusBarManager> mStatusBarManager;

    Boolean mAreActiveLocationRequests;

    AutoPtr<IArrayList> mSettingsChangeCallbacks;  /*<LocationSettingsChangeCallback*/
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_LOCATIONCONTROLLERIMPL_H__
