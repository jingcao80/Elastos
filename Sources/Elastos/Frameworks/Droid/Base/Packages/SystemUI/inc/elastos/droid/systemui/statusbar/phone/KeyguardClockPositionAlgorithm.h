
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDCLOCKPOSITIONALGORITHM_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDCLOCKPOSITIONALGORITHM_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::Animation::IPathInterpolator;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Utility class to calculate the clock position and top padding of notifications on Keyguard.
 */
class KeyguardClockPositionAlgorithm:
    public Object
{
public:
    class Result: public Object
    {
    public:
        Result();

    public:
        /**
         * The y translation of the clock.
         */
        Int32 mClockY;

        /**
         * The scale of the Clock
         */
        Float mClockScale;

        /**
         * The alpha value of the clock.
         */
        Float mClockAlpha;

        /**
         * The top padding of the stack scroller, in pixels.
         */
        Int32 mStackScrollerPadding;

        /**
         * The top padding adjustment of the stack scroller, in pixels. This value is used to adjust
         * the padding, but not the overall panel size.
         */
        Int32 mStackScrollerPaddingAdjustment;
    };

public:
    KeyguardClockPositionAlgorithm();

    /**
     * Refreshes the dimension values.
     */
    CARAPI_(void) LoadDimens(
        /* [in] */ IResources* res);

    CARAPI_(void) Setup(
        /* [in] */ Int32 maxKeyguardNotifications,
        /* [in] */ Int32 maxPanelHeight,
        /* [in] */ Float expandedHeight,
        /* [in] */ Int32 notificationCount,
        /* [in] */ Int32 height,
        /* [in] */ Int32 keyguardStatusHeight,
        /* [in] */ Float emptyDragAmount);

    CARAPI_(void) Run(
        /* [in] */ Result* result);

private:
    CARAPI_(Float) GetClockScale(
        /* [in] */ Int32 notificationPadding,
        /* [in] */ Int32 clockY,
        /* [in] */ Int32 startPadding);

    CARAPI_(Int32) GetClockNotificationsPadding();

    CARAPI_(Float) GetClockYFraction();

    CARAPI_(Int32) GetClockY();

    CARAPI_(Float) GetClockYExpansionAdjustment();

    CARAPI_(Float) GetClockYExpansionRubberbandFactor();

    CARAPI_(Float) GetTopPaddingAdjMultiplier();

    CARAPI_(Float) GetClockAlpha(
        /* [in] */ Float scale);

    /**
     * @return a value from 0 to 1 depending on how many notification there are
     */
    CARAPI_(Float) GetNotificationAmountT();

private:
    static const Float SLOW_DOWN_FACTOR;
    static const Float CLOCK_RUBBERBAND_FACTOR_MIN;
    static const Float CLOCK_RUBBERBAND_FACTOR_MAX;
    static const Float CLOCK_SCALE_FADE_START;
    static const Float CLOCK_SCALE_FADE_END;
    static const Float CLOCK_SCALE_FADE_END_NO_NOTIFS;
    static const Float CLOCK_ADJ_TOP_PADDING_MULTIPLIER_MIN;
    static const Float CLOCK_ADJ_TOP_PADDING_MULTIPLIER_MAX;

    Int32 mClockNotificationsMarginMin;
    Int32 mClockNotificationsMarginMax;
    Float mClockYFractionMin;
    Float mClockYFractionMax;
    Int32 mMaxKeyguardNotifications;
    Int32 mMaxPanelHeight;
    Float mExpandedHeight;
    Int32 mNotificationCount;
    Int32 mHeight;
    Int32 mKeyguardStatusHeight;
    Float mEmptyDragAmount;
    Float mDensity;

    /**
     * The number (fractional) of notifications the "more" card counts when calculating how many
     * notifications are currently visible for the y positioning of the clock.
     */
    Float mMoreCardNotificationAmount;

    static AutoPtr<IPathInterpolator> sSlowDownInterpolator;
    AutoPtr<IAccelerateInterpolator> mAccelerateInterpolator;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_KEYGUARDCLOCKPOSITIONALGORITHM_H__
