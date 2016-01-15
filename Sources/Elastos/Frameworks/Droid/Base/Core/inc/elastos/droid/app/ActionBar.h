
#ifndef __ELASTOS_DROID_APP_ACTIONBAR_H__
#define __ELASTOS_DROID_APP_ACTIONBAR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace App {

class ActionBar
    : public Object
    , public IActionBar
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Enable or disable the "home" button in the corner of the action bar. (Note that this
     * is the application home/up affordance on the action bar, not the systemwide home
     * button.)
     *
     * <p>This defaults to true for packages targeting &lt; API 14. For packages targeting
     * API 14 or greater, the application should call this method to enable interaction
     * with the home/up affordance.
     *
     * <p>Setting the {@link #DISPLAY_HOME_AS_UP} display option will automatically enable
     * the home button.
     *
     * @param enabled true to enable the home button, false to disable the home button.
     */
    CARAPI SetHomeButtonEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Returns a {@link Context} with an appropriate theme for creating views that
     * will appear in the action bar. If you are inflating or instantiating custom views
     * that will appear in an action bar, you should use the Context returned by this method.
     * (This includes adapters used for list navigation mode.)
     * This will ensure that views contrast properly against the action bar.
     *
     * @return A themed Context for creating views
     */
    CARAPI GetThemedContext(
        /* [out] */ IContext** context);

    /**
     * Returns true if the Title field has been truncated during layout for lack
     * of available space.
     *
     * @return true if the Title field has been truncated
     * @hide pending API approval
     */
    CARAPI IsTitleTruncated(
        /* [out] */ Boolean* bval);

    /**
     * Set an alternate drawable to display next to the icon/logo/title
     * when {@link #DISPLAY_HOME_AS_UP} is enabled. This can be useful if you are using
     * this mode to display an alternate selection for up navigation, such as a sliding drawer.
     *
     * <p>If you pass <code>null</code> to this method, the default drawable from the theme
     * will be used.</p>
     *
     * <p>If you implement alternate or intermediate behavior around Up, you should also
     * call {@link #setHomeActionContentDescription(int) setHomeActionContentDescription()}
     * to provide a correct description of the action for accessibility support.</p>
     *
     * @param indicator A drawable to use for the up indicator, or null to use the theme's default
     *
     * @see #setDisplayOptions(int, int)
     * @see #setDisplayHomeAsUpEnabled(boolean)
     * @see #setHomeActionContentDescription(int)
     */
    CARAPI SetHomeAsUpIndicator(
        /* [in] */ IDrawable* indicator);

    /**
     * Set an alternate drawable to display next to the icon/logo/title
     * when {@link #DISPLAY_HOME_AS_UP} is enabled. This can be useful if you are using
     * this mode to display an alternate selection for up navigation, such as a sliding drawer.
     *
     * <p>If you pass <code>0</code> to this method, the default drawable from the theme
     * will be used.</p>
     *
     * <p>If you implement alternate or intermediate behavior around Up, you should also
     * call {@link #setHomeActionContentDescription(int) setHomeActionContentDescription()}
     * to provide a correct description of the action for accessibility support.</p>
     *
     * @param resId Resource ID of a drawable to use for the up indicator, or null
     *              to use the theme's default
     *
     * @see #setDisplayOptions(int, int)
     * @see #setDisplayHomeAsUpEnabled(boolean)
     * @see #setHomeActionContentDescription(int)
     */
    CARAPI SetHomeAsUpIndicator(
        /* [in] */ Int32 resId);

    /**
     * Set an alternate description for the Home/Up action, when enabled.
     *
     * <p>This description is commonly used for accessibility/screen readers when
     * the Home action is enabled. (See {@link #setDisplayHomeAsUpEnabled(boolean)}.)
     * Examples of this are, "Navigate Home" or "Navigate Up" depending on the
     * {@link #DISPLAY_HOME_AS_UP} display option. If you have changed the home-as-up
     * indicator using {@link #setHomeAsUpIndicator(int)} to indicate more specific
     * functionality such as a sliding drawer, you should also set this to accurately
     * describe the action.</p>
     *
     * <p>Setting this to <code>null</code> will use the system default description.</p>
     *
     * @param description New description for the Home action when enabled
     * @see #setHomeAsUpIndicator(int)
     * @see #setHomeAsUpIndicator(android.graphics.drawable.Drawable)
     */
    CARAPI SetHomeActionContentDescription(
        /* [in] */ ICharSequence* description);

    /**
     * Set an alternate description for the Home/Up action, when enabled.
     *
     * <p>This description is commonly used for accessibility/screen readers when
     * the Home action is enabled. (See {@link #setDisplayHomeAsUpEnabled(boolean)}.)
     * Examples of this are, "Navigate Home" or "Navigate Up" depending on the
     * {@link #DISPLAY_HOME_AS_UP} display option. If you have changed the home-as-up
     * indicator using {@link #setHomeAsUpIndicator(int)} to indicate more specific
     * functionality such as a sliding drawer, you should also set this to accurately
     * describe the action.</p>
     *
     * <p>Setting this to <code>0</code> will use the system default description.</p>
     *
     * @param resId Resource ID of a string to use as the new description
     *              for the Home action when enabled
     * @see #setHomeAsUpIndicator(int)
     * @see #setHomeAsUpIndicator(android.graphics.drawable.Drawable)
     */
    CARAPI SetHomeActionContentDescription(
        /* [in] */ Int32 resId);

    /**
     * Enable hiding the action bar on content scroll.
     *
     * <p>If enabled, the action bar will scroll out of sight along with a
     * {@link View#setNestedScrollingEnabled(boolean) nested scrolling child} view's content.
     * The action bar must be in {@link Window#FEATURE_ACTION_BAR_OVERLAY overlay mode}
     * to enable hiding on content scroll.</p>
     *
     * <p>When partially scrolled off screen the action bar is considered
     * {@link #hide() hidden}. A call to {@link #show() show} will cause it to return to full view.
     * </p>
     * @param hideOnContentScroll true to enable hiding on content scroll.
     */
    CARAPI SetHideOnContentScrollEnabled(
        /* [in] */ Boolean hideOnContentScroll);

    /**
     * Return whether the action bar is configured to scroll out of sight along with
     * a {@link View#setNestedScrollingEnabled(boolean) nested scrolling child}.
     *
     * @return true if hide-on-content-scroll is enabled
     * @see #setHideOnContentScrollEnabled(boolean)
     */
    CARAPI IsHideOnContentScrollEnabled(
        /* [out] */ Boolean* bval);

    /**
     * Return the current vertical offset of the action bar.
     *
     * <p>The action bar's current hide offset is the distance that the action bar is currently
     * scrolled offscreen in pixels. The valid range is 0 (fully visible) to the action bar's
     * current measured {@link #getHeight() height} (fully invisible).</p>
     *
     * @return The action bar's offset toward its fully hidden state in pixels
     */
    CARAPI GetHideOffset(
        /* [out] */ Int32* offset);

    /**
     * Set the current hide offset of the action bar.
     *
     * <p>The action bar's current hide offset is the distance that the action bar is currently
     * scrolled offscreen in pixels. The valid range is 0 (fully visible) to the action bar's
     * current measured {@link #getHeight() height} (fully invisible).</p>
     *
     * @param offset The action bar's offset toward its fully hidden state in pixels.
     */
    CARAPI SetHideOffset(
        /* [in] */ Int32 offset);

    /**
     * Set the Z-axis elevation of the action bar in pixels.
     *
     * <p>The action bar's elevation is the distance it is placed from its parent surface. Higher
     * values are closer to the user.</p>
     *
     * @param elevation Elevation value in pixels
     */
    CARAPI SetElevation(
        /* [in] */ Float elevation);

    /**
     * Get the Z-axis elevation of the action bar in pixels.
     *
     * <p>The action bar's elevation is the distance it is placed from its parent surface. Higher
     * values are closer to the user.</p>
     *
     * @return Elevation value in pixels
     */
    CARAPI GetElevation(
        /* [out] */ Float* fval);

    /** @hide */
    CARAPI SetDefaultDisplayHomeAsUpEnabled(
        /* [in] */ Boolean enabled);

    /** @hide */
    CARAPI SetShowHideAnimationEnabled(
        /* [in] */ Boolean enabled);

    /** @hide */
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* config);

    /** @hide */
    CARAPI DispatchMenuVisibilityChanged(
        /* [in] */ Boolean visible);

    /** @hide */
    CARAPI StartActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** mode);

    /** @hide */
    CARAPI OpenOptionsMenu(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI InvalidateOptionsMenu(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI OnMenuKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI CollapseActionView(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI SetWindowTitle(
        /* [in] */ ICharSequence* title);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIONBAR_H__
