
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_LOGICALDISPLAY_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_LOGICALDISPLAY_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/display/DisplayDevice.h"
#include "elastos/droid/server/display/DisplayDeviceInfo.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IDisplayInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Describes how a logical display is configured.
 * <p>
 * At this time, we only support logical displays that are coupled to a particular
 * primary display device from which the logical display derives its basic properties
 * such as its size, density and refresh rate.
 * </p><p>
 * A logical display may be mirrored onto multiple display devices in addition to its
 * primary display device.  Note that the contents of a logical display may not
 * always be visible, even on its primary display device, such as in the case where
 * the primary display device is currently mirroring content from a different
 * logical display.
 * </p><p>
 * This object is designed to encapsulate as much of the policy of logical
 * displays as possible.  The idea is to make it easy to implement new kinds of
 * logical displays mostly by making local changes to this class.
 * </p><p>
 * Note: The display manager architecture does not actually require logical displays
 * to be associated with any individual display device.  Logical displays and
 * display devices are orthogonal concepts.  Some mapping will exist between
 * logical displays and display devices but it can be many-to-many and
 * and some might have no relation at all.
 * </p><p>
 * Logical displays are guarded by the {@link DisplayManagerService.SyncRoot} lock.
 * </p>
 */
class LogicalDisplay
    : public Object
{
public:
    LogicalDisplay(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 layerStack,
        /* [in] */ DisplayDevice* primaryDisplayDevice);

    /**
     * Gets the logical display id of this logical display.
     *
     * @return The logical display id.
     */
    CARAPI_(Int32) GetDisplayIdLocked();

    /**
     * Gets the primary display device associated with this logical display.
     *
     * @return The primary display device.
     */
    CARAPI_(AutoPtr<DisplayDevice>) GetPrimaryDisplayDeviceLocked();

    /**
     * Gets information about the logical display.
     *
     * @return The device info, which should be treated as immutable by the caller.
     * The logical display should allocate a new display info object whenever
     * the data changes.
     */
    CARAPI_(AutoPtr<IDisplayInfo>) GetDisplayInfoLocked();

    /**
     * Sets overridden logical display information from the window manager.
     * This method can be used to adjust application insets, rotation, and other
     * properties that the window manager takes care of.
     *
     * @param info The logical display information, may be NULL.
     */
    CARAPI_(Boolean) SetDisplayInfoOverrideFromWindowManagerLocked(
        /* [in] */ IDisplayInfo* info);

    /**
     * Returns true if the logical display is in a valid state.
     * This method should be checked after calling {@link #updateLocked} to handle the
     * case where a logical display should be removed because all of its associated
     * display devices are gone or if it is otherwise no longer needed.
     *
     * @return True if the logical display is still valid.
     */
    CARAPI_(Boolean) IsValidLocked();

    /**
     * Updates the state of the logical display based on the available display devices.
     * The logical display might become invalid if it is attached to a display device
     * that no longer exists.
     *
     * @param devices The list of all connected display devices.
     */
    CARAPI_(void) UpdateLocked(
        /* [in] */ const List< AutoPtr<DisplayDevice> >& devices);

    /**
     * Applies the layer stack and transformation to the given display device
     * so that it shows the contents of this logical display.
     *
     * We know that the given display device is only ever showing the contents of
     * a single logical display, so this method is expected to blow away all of its
     * transformation properties to make it happen regardless of what the
     * display device was previously showing.
     *
     * The caller must have an open Surface transaction.
     *
     * The display device may not be the primary display device, in the case
     * where the display is being mirrored.
     *
     * @param device The display device to modify.
     * @param isBlanked True if the device is being blanked.
     */
    CARAPI_(void) ConfigureDisplayInTransactionLocked(
        /* [in] */ DisplayDevice* device,
        /* [in] */ Boolean isBlanked);

    /**
     * Returns true if the logical display has unique content.
     * <p>
     * If the display has unique content then we will try to ensure that it is
     * visible on at least its primary display device.  Otherwise we will ignore the
     * logical display and perhaps show mirrored content on the primary display device.
     * </p>
     *
     * @return True if the display has unique content.
     */
    CARAPI_(Boolean) HasContentLocked();

    /**
     * Sets whether the logical display has unique content.
     *
     * @param hasContent True if the display has unique content.
     */
    CARAPI_(void) SetHasContentLocked(
        /* [in] */ Boolean hasContent);

    /**
     * Requests the given refresh rate.
     * @param requestedRefreshRate The desired refresh rate.
     */
    void SetRequestedRefreshRateLocked(
        /* [in] */ Float requestedRefreshRate);

    /**
     * Gets the pending requested refresh rate.
     *
     * @return The pending refresh rate requested
     */
    CARAPI_(Float) GetRequestedRefreshRateLocked();

    CARAPI_(void) DumpLocked(
        /* [in] */ IPrintWriter* pw);

private:
    // The layer stack we use when the display has been blanked to prevent any
    // of its content from appearing.
    static const Int32 BLANK_LAYER_STACK = -1;

    AutoPtr<IDisplayInfo> mBaseDisplayInfo;

    Int32 mDisplayId;
    Int32 mLayerStack;
    AutoPtr<IDisplayInfo> mOverrideDisplayInfo; // set by the window manager
    AutoPtr<IDisplayInfo> mInfo;

    // The display device that this logical display is based on and which
    // determines the base metrics that it uses.
    AutoPtr<DisplayDevice> mPrimaryDisplayDevice;
    AutoPtr<DisplayDeviceInfo> mPrimaryDisplayDeviceInfo;

    // True if the logical display has unique content.
    Boolean mHasContent;

    // The pending requested refresh rate. 0 if no request is pending.
    Float mRequestedRefreshRate;

    // Temporary rectangle used when needed.
    AutoPtr<IRect> mTempLayerStackRect;
    AutoPtr<IRect> mTempDisplayRect;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_LOGICALDISPLAY_H__
