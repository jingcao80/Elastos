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

#ifndef __ELASTOS_DROID_VIEW_VIEWCONFIGURATION_H__
#define __ELASTOS_DROID_VIEW_VIEWCONFIGURATION_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/utility/CSparseArray.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace View {

class ViewConfiguration
    : public Object
    , public IViewConfiguration
{
public:
    CAR_INTERFACE_DECL()

    ViewConfiguration();

    ~ViewConfiguration();

    /**
     * @deprecated Use {@link android.view.ViewConfiguration#get(android.content.Context)} instead.
     */
    CARAPI constructor();

    /**
     * Creates a new configuration for the specified context. The configuration depends on
     * various parameters of the context, like the dimension of the display or the density
     * of the display.
     *
     * @param context The application context used to initialize this view configuration.
     *
     * @see #get(android.content.Context)
     * @see android.util.DisplayMetrics
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * @return The width of the horizontal scrollbar and the height of the vertical
     *         scrollbar in pixels
     */
    CARAPI GetScaledScrollBarSize(
        /* [out] */ Int32* scrollbarSize);

    /**
     * @return the length of the fading edges in pixels
     */
    CARAPI GetScaledFadingEdgeLength(
        /* [out] */ Int32* fadingEdgeLength);

    /**
     * @return Inset in pixels to look for touchable content when the user touches the edge of the
     *         screen
     */
    CARAPI GetScaledEdgeSlop(
        /* [out] */ Int32* edgeSlop);

    /**
     * @return Distance a touch can wander before we think the user is scrolling in pixels
     */
    CARAPI GetScaledTouchSlop(
        /* [out] */ Int32* touchSlop);

    /**
     * @return Distance in pixels the first touch can wander before we do not consider this a
     * potential double tap event
     * @hide
     */
    CARAPI GetScaledDoubleTapTouchSlop(
        /* [out] */ Int32* doubleTapTouchSlop);

    /**
     * @return Distance a touch can wander before we think the user is scrolling a full page
     *         in dips
     */
    CARAPI GetScaledPagingTouchSlop(
        /* [out] */ Int32* pagingTouchSlop);

    /**
     * @return Distance between the first touch and second touch to still be
     *         considered a double tap
     */
    CARAPI GetScaledDoubleTapSlop(
        /* [out] */ Int32* doubleTapSlop);

    /**
     * @return Distance a touch must be outside the bounds of a window for it
     * to be counted as outside the window for purposes of dismissing that
     * window.
     */
    CARAPI GetScaledWindowTouchSlop(
        /* [out] */ Int32* windowTouchSlop);

    /**
     * @return Minimum velocity to initiate a fling, as measured in pixels per second.
     */
    CARAPI GetScaledMinimumFlingVelocity(
        /* [out] */ Int32* minimumFlingVelocity);

    /**
     * @return Maximum velocity to initiate a fling, as measured in pixels per second.
     */
    CARAPI GetScaledMaximumFlingVelocity(
        /* [out] */ Int32* maximumFlingVelocity);

    /**
     * The maximum drawing cache size expressed in bytes.
     *
     * @return the maximum size of View's drawing cache expressed in bytes
     */
    CARAPI GetScaledMaximumDrawingCacheSize(
        /* [out] */ Int32* maximumDrawingCacheSize);

    /**
     * @return The maximum distance a View should overscroll by when showing edge effects.
     */
    CARAPI GetScaledOverscrollDistance(
        /* [out] */ Int32* overscrollDistance);

    /**
     * @return The maximum distance a View should overfling by when showing edge effects.
     */
    CARAPI GetScaledOverflingDistance(
        /* [out] */ Int32* overflingDistance);

    /**
     * The amount of time a user needs to press the relevant key to bring up
     * the global actions dialog.
     *
     * @return how long a user needs to press the relevant key to bring up
     *   the global actions dialog.
     * @hide
     */
    CARAPI GetDeviceGlobalActionKeyTimeout(
        /* [out] */ Int64* timeout);

    /**
     * Report if the device has a permanent menu key available to the user.
     *
     * <p>As of Android 3.0, devices may not have a permanent menu key available.
     * Apps should use the action bar to present menu options to users.
     * However, there are some apps where the action bar is inappropriate
     * or undesirable. This method may be used to detect if a menu key is present.
     * If not, applications should provide another on-screen affordance to access
     * functionality.
     *
     * @return true if a permanent menu key is present, false otherwise.
     */
    CARAPI HasPermanentMenuKey(
        /* [out] */ Boolean* hasPermanentMenuKey);

    /**
     * @hide
     * @return Whether or not marquee should use fading edges.
     */
    CARAPI IsFadingMarqueeEnabled(
        /* [out] */ Boolean* fadingMarqueeEnabled);

    /**
     * Returns a configuration for the specified context. The configuration depends on
     * various parameters of the context, like the dimension of the display or the
     * density of the display.
     *
     * @param context The application context used to initialize the view configuration.
     */
    static CARAPI_(AutoPtr<IViewConfiguration>) Get(
        /* [in] */ IContext* context);

    /**
     * @return The width of the horizontal scrollbar and the height of the vertical
     *         scrollbar in pixels
     *
     * @deprecated Use {@link #getScaledScrollBarSize()} instead.
     */
    static CARAPI_(Int32) GetScrollBarSize();

    /**
     * @return Duration of the fade when scrollbars fade away in milliseconds
     */
    static CARAPI_(Int32) GetScrollBarFadeDuration();

    /**
     * @return Default delay before the scrollbars fade in milliseconds
     */
    static CARAPI_(Int32) GetScrollDefaultDelay();

    /**
     * @return the length of the fading edges in pixels
     *
     * @deprecated Use {@link #getScaledFadingEdgeLength()} instead.
     */
    static CARAPI_(Int32) GetFadingEdgeLength();

    /**
     * @return the duration in milliseconds of the pressed state in child
     * components.
     */
    static CARAPI_(Int32) GetPressedStateDuration();

    /**
     * @return the duration in milliseconds before a press turns into
     * a Int64 press
     */
    static CARAPI_(Int32) GetLongPressTimeout();

    /**
     * @return the time before the first key repeat in milliseconds.
     */
    static CARAPI_(Int32) GetKeyRepeatTimeout();

    /**
     * @return the time between successive key repeats in milliseconds.
     */
    static CARAPI_(Int32) GetKeyRepeatDelay();

    /**
     * @return the duration in milliseconds we will wait to see if a touch event
     * is a tap or a scroll. If the user does not move within this interval, it is
     * considered to be a tap.
     */
    static CARAPI_(Int32) GetTapTimeout();

    /**
     * @return the duration in milliseconds we will wait to see if a touch event
     * is a jump tap. If the user does not move within this interval, it is
     * considered to be a tap.
     */
    static CARAPI_(Int32) GetJumpTapTimeout();

    /**
     * @return the duration in milliseconds between the first tap's up event and
     * the second tap's down event for an interaction to be considered a
     * double-tap.
     */
    static CARAPI_(Int32) GetDoubleTapTimeout();

    /**
     * @return the minimum duration in milliseconds between the first tap's
     * up event and the second tap's down event for an interaction to be considered a
     * double-tap.
     *
     * @hide
     */
    static CARAPI_(Int32) GetDoubleTapMinTime();

    /**
     * @return the maximum duration in milliseconds between a touch pad
     * touch and release for a given touch to be considered a tap (click) as
     * opposed to a hover movement gesture.
     * @hide
     */
    static CARAPI_(Int32) GetHoverTapTimeout();

    /**
     * @return the maximum distance in pixels that a touch pad touch can move
     * before being released for it to be considered a tap (click) as opposed
     * to a hover movement gesture.
     * @hide
     */
    static CARAPI_(Int32) GetHoverTapSlop();

    /**
     * @return Inset in pixels to look for touchable content when the user touches the edge of the
     *         screen
     *
     * @deprecated Use {@link #getScaledEdgeSlop()} instead.
     */
    static CARAPI_(Int32) GetEdgeSlop();

    /**
     * @return Distance a touch can wander before we think the user is scrolling in pixels
     *
     * @deprecated Use {@link #getScaledTouchSlop()} instead.
     */
    static CARAPI_(Int32) GetTouchSlop();

    /**
     * @return Distance between the first touch and second touch to still be
     *         considered a double tap
     * @deprecated Use {@link #getScaledDoubleTapSlop()} instead.
     * @hide The only client of this should be GestureDetector, which needs this
     *       for clients that still use its deprecated constructor.
     */
    static CARAPI_(Int32) GetDoubleTapSlop();

    /**
     * Interval for dispatching a recurring accessibility event in milliseconds.
     * This interval guarantees that a recurring event will be send at most once
     * during the {@link #getSendRecurringAccessibilityEventsInterval()} time frame.
     *
     * @return The delay in milliseconds.
     *
     * @hide
     */
    static CARAPI_(Int64) GetSendRecurringAccessibilityEventsInterval();

    /**
     * @return Distance a touch must be outside the bounds of a window for it
     * to be counted as outside the window for purposes of dismissing that
     * window.
     *
     * @deprecated Use {@link #getScaledWindowTouchSlop()} instead.
     */
    static CARAPI_(Int32) GetWindowTouchSlop();

    /**
     * @return Minimum velocity to initiate a fling, as measured in pixels per second.
     *
     * @deprecated Use {@link #getScaledMinimumFlingVelocity()} instead.
     */
    static CARAPI_(Int32) GetMinimumFlingVelocity();

    /**
     * @return Maximum velocity to initiate a fling, as measured in pixels per second.
     *
     * @deprecated Use {@link #getScaledMaximumFlingVelocity()} instead.
     */
    static CARAPI_(Int32) GetMaximumFlingVelocity();

    /**
     * The maximum drawing cache size expressed in bytes.
     *
     * @return the maximum size of View's drawing cache expressed in bytes
     *
     * @deprecated Use {@link #getScaledMaximumDrawingCacheSize()} instead.
     */
    static CARAPI_(Int32) GetMaximumDrawingCacheSize();

    /**
     * The amount of time that the zoom controls should be
     * displayed on the screen expressed in milliseconds.
     *
     * @return the time the zoom controls should be visible expressed
     * in milliseconds.
     */
    static CARAPI_(Int64) GetZoomControlsTimeout();

    /**
     * The amount of time a user needs to press the relevant key to bring up
     * the global actions dialog.
     *
     * @return how long a user needs to press the relevant key to bring up
     *   the global actions dialog.
     * @deprecated This timeout should not be used by applications
     */
    //@Deprecated
    static CARAPI_(Int64) GetGlobalActionKeyTimeout();

    /**
     * The amount of friction applied to scrolls and flings.
     *
     * @return A scalar dimensionless value representing the coefficient of
     *         friction.
     */
    static CARAPI_(Float) GetScrollFriction();

private:
    /**
     * Defines the width of the horizontal scrollbar and the height of the vertical scrollbar in
     * dips
     */
    static const Int32 SCROLL_BAR_SIZE;

    /**
     * Duration of the fade when scrollbars fade away in milliseconds
     */
    static const Int32 SCROLL_BAR_FADE_DURATION;

    /**
     * Default delay before the scrollbars fade in milliseconds
     */
    static const Int32 SCROLL_BAR_DEFAULT_DELAY;

    /**
     * Defines the length of the fading edges in dips
     */
    static const Int32 FADING_EDGE_LENGTH;

    /**
     * Defines the duration in milliseconds of the pressed state in child
     * components.
     */
    static const Int32 PRESSED_STATE_DURATION;

    /**
     * Defines the default duration in milliseconds before a press turns into
     * a long press
     */
    static const Int32 DEFAULT_LONG_PRESS_TIMEOUT;

    /**
     * Defines the time between successive key repeats in milliseconds.
     */
    static const Int32 KEY_REPEAT_DELAY;

    /**
     * Defines the duration in milliseconds a user needs to hold down the
     * appropriate button to bring up the global actions dialog (power off,
     * lock screen, etc).
     */
    static const Int32 GLOBAL_ACTIONS_KEY_TIMEOUT;

    /**
     * Defines the duration in milliseconds we will wait to see if a touch event
     * is a tap or a scroll. If the user does not move within this interval, it is
     * considered to be a tap.
     */
    static const Int32 TAP_TIMEOUT;

    /**
     * Defines the duration in milliseconds we will wait to see if a touch event
     * is a jump tap. If the user does not complete the jump tap within this interval, it is
     * considered to be a tap.
     */
    static const Int32 JUMP_TAP_TIMEOUT;

    /**
     * Defines the duration in milliseconds between the first tap's up event and
     * the second tap's down event for an interaction to be considered a
     * double-tap.
     */
    static const Int32 DOUBLE_TAP_TIMEOUT;

    /**
     * Defines the minimum duration in milliseconds between the first tap's up event and
     * the second tap's down event for an interaction to be considered a
     * double-tap.
     */
    static const Int32 DOUBLE_TAP_MIN_TIME;

    /**
     * Defines the maximum duration in milliseconds between a touch pad
     * touch and release for a given touch to be considered a tap (click) as
     * opposed to a hover movement gesture.
     */
    static const Int32 HOVER_TAP_TIMEOUT;

    /**
     * Defines the maximum distance in pixels that a touch pad touch can move
     * before being released for it to be considered a tap (click) as opposed
     * to a hover movement gesture.
     */
    static const Int32 HOVER_TAP_SLOP;

    /**
     * Defines the duration in milliseconds we want to display zoom controls in response
     * to a user panning within an application.
     */
    static const Int32 ZOOM_CONTROLS_TIMEOUT;

    /**
     * Inset in dips to look for touchable content when the user touches the edge of the screen
     */
    static const Int32 EDGE_SLOP;

    /**
     * Distance a touch can wander before we think the user is scrolling in dips.
     * Note that this value defined here is only used as a fallback by legacy/misbehaving
     * applications that do not provide a Context for determining density/configuration-dependent
     * values.
     *
     * To alter this value, see the configuration resource config_viewConfigurationTouchSlop
     * in frameworks/base/core/res/res/values/config.xml or the appropriate device resource overlay.
     * It may be appropriate to tweak this on a device-specific basis in an overlay based on
     * the characteristics of the touch panel and firmware.
     */
    static const Int32 TOUCH_SLOP;

    /**
     * Distance the first touch can wander before we stop considering this event a double tap
     * (in dips)
     */
    static const Int32 DOUBLE_TAP_TOUCH_SLOP;

    /**
     * Distance a touch can wander before we think the user is attempting a paged scroll
     * (in dips)
     *
     * Note that this value defined here is only used as a fallback by legacy/misbehaving
     * applications that do not provide a Context for determining density/configuration-dependent
     * values.
     *
     * See the note above on {@link #TOUCH_SLOP} regarding the dimen resource
     * config_viewConfigurationTouchSlop. ViewConfiguration will report a paging touch slop of
     * config_viewConfigurationTouchSlop * 2 when provided with a Context.
     */
    static const Int32 PAGING_TOUCH_SLOP;

    /**
     * Distance in dips between the first touch and second touch to still be considered a double tap
     */
    static const Int32 DOUBLE_TAP_SLOP;

    /**
     * Distance in dips a touch needs to be outside of a window's bounds for it to
     * count as outside for purposes of dismissing the window.
     */
    static const Int32 WINDOW_TOUCH_SLOP;

    /**
     * Minimum velocity to initiate a fling, as measured in dips per second
     */
    static const Int32 MINIMUM_FLING_VELOCITY;

    /**
     * Maximum velocity to initiate a fling, as measured in dips per second
     */
    static const Int32 MAXIMUM_FLING_VELOCITY;

    /**
     * Delay before dispatching a recurring accessibility event in milliseconds.
     * This delay guarantees that a recurring event will be send at most once
     * during the {@link #SEND_RECURRING_ACCESSIBILITY_EVENTS_INTERVAL_MILLIS} time
     * frame.
     */
    static const Int64 SEND_RECURRING_ACCESSIBILITY_EVENTS_INTERVAL_MILLIS;

    /**
     * The maximum size of View's drawing cache, expressed in bytes. This size
     * should be at least equal to the size of the screen in ARGB888 format.
     */
    //@Deprecated
    static const Int32 MAXIMUM_DRAWING_CACHE_SIZE; // ARGB8888

    /**
     * The coefficient of friction applied to flings/scrolls.
     */
    static const Float SCROLL_FRICTION;

    /**
     * Max distance in dips to overscroll for edge effects
     */
    static const Int32 OVERSCROLL_DISTANCE;

    /**
     * Max distance in dips to overfling for edge effects
     */
    static const Int32 OVERFLING_DISTANCE;

    /**
     * Configuration values for overriding {@link #hasPermanentMenuKey()} behavior.
     * These constants must match the definition in res/values/config.xml.
     */
    static const Int32 HAS_PERMANENT_MENU_KEY_AUTODETECT;
    static const Int32 HAS_PERMANENT_MENU_KEY_TRUE;
    static const Int32 HAS_PERMANENT_MENU_KEY_FALSE;

    Int32 mEdgeSlop;
    Int32 mFadingEdgeLength;
    Int32 mMinimumFlingVelocity;
    Int32 mMaximumFlingVelocity;
    Int32 mScrollbarSize;
    Int32 mTouchSlop;
    Int32 mDoubleTapTouchSlop;
    Int32 mPagingTouchSlop;
    Int32 mDoubleTapSlop;
    Int32 mWindowTouchSlop;
    Int32 mMaximumDrawingCacheSize;
    Int32 mOverscrollDistance;
    Int32 mOverflingDistance;
    Boolean mFadingMarqueeEnabled;
    Int64 mGlobalActionsKeyTimeout;

    Boolean sHasPermanentMenuKey;
    Boolean sHasPermanentMenuKeySet;
    static AutoPtr<ISparseArray> sConfigurations;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_VIEWCONFIGURATION_H__
