#ifndef __ELASTOS_DROID_CONTACTS_COMMON_WIDGET_CFLOATINGACTIONBUTTONCONTROLLER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_WIDGET_CFLOATINGACTIONBUTTONCONTROLLER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Contacts_Common_Widget_CFloatingActionButtonController.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Graphics::IInterpolator;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Widget {

/**
 * Controls the movement and appearance of the FAB (Floating Action Button).
 */
CarClass(CFloatingActionButtonController)
    , public Object
    , public IFloatingActionButtonController
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IActivity* activity,
        /* [in] */ IView* container,
        /* [in] */ IView* button);

    /**
     * Passes the screen width into the class. Necessary for translation calculations.
     * Should be called as soon as parent View width is available.
     *
     * @param screenWidth The width of the screen in pixels.
     */
    CARAPI SetScreenWidth(
        /* [in] */ Int32 screenWidth);

    /**
     * Sets FAB as View.VISIBLE or View.GONE.
     *
     * @param visible Whether or not to make the container visible.
     */
    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    /**
     * Updates the FAB location (middle to right position) as the PageView scrolls.
     *
     * @param positionOffset A fraction used to calculate position of the FAB during page scroll.
     */
    CARAPI OnPageScrolled(
        /* [in] */ Float positionOffset);

    /**
     * Aligns the FAB to the described location plus specified additional offsets.
     *
     * @param align One of ALIGN_MIDDLE, ALIGN_QUARTER_RIGHT, or ALIGN_RIGHT.
     * @param offsetX Additional offsetX to translate by.
     * @param animate Whether or not to animate the transition.
     */
    CARAPI Align(
        /* [in] */ Int32 align,
        /* [in] */ Int32 offsetX,
        /* [in] */ Int32 offsetY,
        /* [in] */ Boolean animate);

    /**
     * Resizes width and height of the floating action bar container.
     * @param dimension The new dimensions for the width and height.
     * @param animate Whether to animate this change.
     */
    CARAPI Resize(
        /* [in] */ Int32 dimension,
        /* [in] */ Boolean animate);

    /**
     * Scales the floating action button from no height and width to its actual dimensions. This is
     * an animation for showing the floating action button.
     * @param delayMs The delay for the effect, in milliseconds.
     */
    CARAPI ScaleIn(
        /* [in] */ Int32 delayMs);

    /**
     * Scales the floating action button from its actual dimensions to no height and width. This is
     * an animation for hiding the floating action button.
     */
    CARAPI ScaleOut();

    /**
     * Calculates the X offset of the FAB to the given alignment, adjusted for whether or not the
     * view is in RTL mode.
     *
     * @param align One of ALIGN_MIDDLE, ALIGN_QUARTER_RIGHT, or ALIGN_RIGHT.
     * @return The translationX for the given alignment.
     */
    CARAPI GetTranslationXForAlignment(
        /* [in] */ Int32 align,
        /* [out] */ Int32* result);

private:
    CARAPI_(Boolean) IsLayoutRtl();

private:
    static const Int32 FAB_SCALE_IN_DURATION;
    static const Int32 FAB_SCALE_IN_FADE_IN_DELAY;
    static const Int32 FAB_ICON_FADE_OUT_DURATION;

    Int32 mAnimationDuration;
    Int32 mFloatingActionButtonWidth;
    Int32 mFloatingActionButtonMarginRight;
    AutoPtr<IView> mFloatingActionButtonContainer;
    AutoPtr<IView> mFloatingActionButton;
    AutoPtr<IInterpolator> mFabInterpolator;
    Int32 mScreenWidth;
};

} // Widget
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_WIDGET_CFLOATINGACTIONBUTTONCONTROLLER_H__
