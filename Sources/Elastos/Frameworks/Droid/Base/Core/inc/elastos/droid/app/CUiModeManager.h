#ifndef __ELASTOS_DROID_APP_CUIMODEMANAGER_H__
#define __ELASTOS_DROID_APP_CUIMODEMANAGER_H__

#include "_Elastos_Droid_App_CUiModeManager.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CUiModeManager)
    , public Object
    , public IUiModeManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUiModeManager();

    ~CUiModeManager();

    CARAPI constructor();

    /**
     * Force device into car mode, like it had been placed in the car dock.
     * This will cause the device to switch to the car home UI as part of
     * the mode switch.
     * @param flags Must be 0.
     */
    CARAPI EnableCarMode(
        /* [in] */ Int32 flags);

    /**
     * Turn off special mode if currently in car mode.
     * @param flags May be 0 or {@link #DISABLE_CAR_MODE_GO_HOME}.
     */
    CARAPI DisableCarMode(
        /* [in] */ Int32 flags);

    /**
     * Return the current running mode type.  May be one of
     * {@link Configuration#UI_MODE_TYPE_NORMAL Configuration.UI_MODE_TYPE_NORMAL},
     * {@link Configuration#UI_MODE_TYPE_DESK Configuration.UI_MODE_TYPE_DESK}, or
     * {@link Configuration#UI_MODE_TYPE_CAR Configuration.UI_MODE_TYPE_CAR}, or
     * {@link Configuration#UI_MODE_TYPE_TELEVISION Configuration.UI_MODE_TYPE_APPLIANCE}.
     */
    CARAPI GetCurrentModeType(
        /* [out] */ Int32 * type);

    /**
     * Sets the night mode.  Changes to the night mode are only effective when
     * the car or desk mode is enabled on a device.
     *
     * <p>The mode can be one of:
     * <ul>
     *   <li><em>{@link #MODE_NIGHT_NO}<em> - sets the device into notnight
     *       mode.</li>
     *   <li><em>{@link #MODE_NIGHT_YES}</em> - sets the device into night mode.
     *   </li>
     *   <li><em>{@link #MODE_NIGHT_AUTO}</em> - automatic night/notnight switching
     *       depending on the location and certain other sensors.</li>
     * </ul>
     */
    CARAPI SetNightMode(
        /* [in] */ Int32 mode);

    /**
     * Returns the currently configured night mode.
     *
     * @return {@link #MODE_NIGHT_NO}, {@link #MODE_NIGHT_YES}, or
     *  {@link #MODE_NIGHT_AUTO}.  When an error occurred -1 is returned.
     */
    CARAPI GetNightMode(
        /* [out] */ Int32 * mode);

public:
    AutoPtr<IIUiModeManager> mService;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CUIMODEMANAGER_H__
