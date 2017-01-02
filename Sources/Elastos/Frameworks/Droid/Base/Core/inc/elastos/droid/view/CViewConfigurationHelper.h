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

#ifndef __ELASTOS_DROID_VIEW_CVIEWCONFIGURATIONHELPER_H__
#define __ELASTOS_DROID_VIEW_CVIEWCONFIGURATIONHELPER_H__

#include "_Elastos_Droid_View_CViewConfigurationHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CViewConfigurationHelper)
    , public Singleton
    , public IViewConfigurationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a configuration for the specified context. The configuration depends on
     * various parameters of the context, like the dimension of the display or the
     * density of the display.
     *
     * @param context The application context used to initialize the view configuration.
     */
    CARAPI Get(
        /* [in] */ IContext* context,
        /* [out] */ IViewConfiguration** viewConfig);

    /**
     * @return The width of the horizontal scrollbar and the height of the vertical
     *         scrollbar in pixels
     *
     * @deprecated Use {@link #getScaledScrollBarSize()} instead.
     */
    CARAPI GetScrollBarSize(
        /* [out] */ Int32* scrollBarSize);

    /**
     * @return Duration of the fade when scrollbars fade away in milliseconds
     */
    CARAPI GetScrollBarFadeDuration(
        /* [out] */ Int32* scrollBarFadeDuration);

    /**
     * @return Default delay before the scrollbars fade in milliseconds
     */
    CARAPI GetScrollDefaultDelay(
        /* [out] */ Int32* scrollDefaultDelay);

    /**
     * @return the length of the fading edges in pixels
     *
     * @deprecated Use {@link #getScaledFadingEdgeLength()} instead.
     */
    CARAPI GetFadingEdgeLength(
        /* [out] */ Int32* fadingEdgeLength);

    /**
     * @return the duration in milliseconds of the pressed state in child
     * components.
     */
    CARAPI GetPressedStateDuration(
        /* [out] */ Int32* pressedStateDuration);

    /**
     * @return the duration in milliseconds before a press turns into
     * a Int64 press
     */
    CARAPI GetLongPressTimeout(
        /* [out] */ Int32* longPressTimeout);

    /**
     * @return the time before the first key repeat in milliseconds.
     */
    CARAPI GetKeyRepeatTimeout(
        /* [out] */ Int32* keyRepeatTimeout);

    /**
     * @return the time between successive key repeats in milliseconds.
     */
    CARAPI GetKeyRepeatDelay(
        /* [out] */ Int32* keyRepeatDelay);

    /**
     * @return the duration in milliseconds we will wait to see if a touch event
     * is a tap or a scroll. If the user does not move within this interval, it is
     * considered to be a tap.
     */
    CARAPI GetTapTimeout(
        /* [out] */ Int32* tapTimeout);

    /**
     * @return the duration in milliseconds we will wait to see if a touch event
     * is a jump tap. If the user does not move within this interval, it is
     * considered to be a tap.
     */
    CARAPI GetJumpTapTimeout(
        /* [out] */ Int32* jumpTapTimeout);

    /**
     * @return the duration in milliseconds between the first tap's up event and
     * the second tap's down event for an interaction to be considered a
     * double-tap.
     */
    CARAPI GetDoubleTapTimeout(
        /* [out] */ Int32* doubleTapTimeout);

    /**
     * @return the minimum duration in milliseconds between the first tap's
     * up event and the second tap's down event for an interaction to be considered a
     * double-tap.
     *
     * @hide
     */
    CARAPI GetDoubleTapMinTime(
        /* [out] */ Int32* doubleTapMinTime);

    /**
     * @return the maximum duration in milliseconds between a touch pad
     * touch and release for a given touch to be considered a tap (click) as
     * opposed to a hover movement gesture.
     * @hide
     */
    CARAPI GetHoverTapTimeout(
        /* [out] */ Int32* hoverTapTimeout);

    /**
     * @return the maximum distance in pixels that a touch pad touch can move
     * before being released for it to be considered a tap (click) as opposed
     * to a hover movement gesture.
     * @hide
     */
    CARAPI GetHoverTapSlop(
        /* [out] */ Int32* hoverTapSlop);

    /**
     * @return Inset in pixels to look for touchable content when the user touches the edge of the
     *         screen
     *
     * @deprecated Use {@link #getScaledEdgeSlop()} instead.
     */
    CARAPI GetEdgeSlop(
        /* [out] */ Int32* edgeSlop);

    /**
     * @return Distance a touch can wander before we think the user is scrolling in pixels
     *
     * @deprecated Use {@link #getScaledTouchSlop()} instead.
     */
    CARAPI GetTouchSlop(
        /* [out] */ Int32* touchSlop);

    /**
     * @return Distance between the first touch and second touch to still be
     *         considered a double tap
     * @deprecated Use {@link #getScaledDoubleTapSlop()} instead.
     * @hide The only client of this should be GestureDetector, which needs this
     *       for clients that still use its deprecated constructor.
     */
    CARAPI GetDoubleTapSlop(
        /* [out] */ Int32* doubleTapSlop);

    /**
     * Interval for dispatching a recurring accessibility event in milliseconds.
     * This interval guarantees that a recurring event will be send at most once
     * during the {@link #getSendRecurringAccessibilityEventsInterval()} time frame.
     *
     * @return The delay in milliseconds.
     *
     * @hide
     */
    CARAPI GetSendRecurringAccessibilityEventsInterval(
        /* [out] */ Int64* sendRecurringAccessibilityEventsInterval);

    /**
     * @return Distance a touch must be outside the bounds of a window for it
     * to be counted as outside the window for purposes of dismissing that
     * window.
     *
     * @deprecated Use {@link #getScaledWindowTouchSlop()} instead.
     */
    CARAPI GetWindowTouchSlop(
        /* [out] */ Int32* windowTouchSlop);

    /**
     * @return Minimum velocity to initiate a fling, as measured in pixels per second.
     *
     * @deprecated Use {@link #getScaledMinimumFlingVelocity()} instead.
     */
    CARAPI GetMinimumFlingVelocity(
        /* [out] */ Int32* minimumFlingVelocity);

    /**
     * @return Maximum velocity to initiate a fling, as measured in pixels per second.
     *
     * @deprecated Use {@link #getScaledMaximumFlingVelocity()} instead.
     */
    CARAPI GetMaximumFlingVelocity(
        /* [out] */ Int32* maximumFlingVelocity);

    /**
     * The maximum drawing cache size expressed in bytes.
     *
     * @return the maximum size of View's drawing cache expressed in bytes
     *
     * @deprecated Use {@link #getScaledMaximumDrawingCacheSize()} instead.
     */
    CARAPI GetMaximumDrawingCacheSize(
        /* [out] */ Int32* maximumDrawingCacheSize);

    /**
     * The amount of time that the zoom controls should be
     * displayed on the screen expressed in milliseconds.
     *
     * @return the time the zoom controls should be visible expressed
     * in milliseconds.
     */
    CARAPI GetZoomControlsTimeout(
        /* [out] */ Int64* zoomControlsTimeout);

    /**
     * The amount of time a user needs to press the relevant key to bring up
     * the global actions dialog.
     *
     * @return how Int64 a user needs to press the relevant key to bring up
     *   the global actions dialog.
     * @deprecated This timeout should not be used by applications
     */
    //@Deprecated
    CARAPI GetGlobalActionKeyTimeout(
        /* [out] */ Int64* globalActionKeyTimeout);

    /**
     * The amount of friction applied to scrolls and flings.
     *
     * @return A scalar dimensionless value representing the coefficient of
     *         friction.
     */
    CARAPI GetScrollFriction(
        /* [out] */ Float* scrollFriction);
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CVIEWCONFIGURATIONHELPER_H__
