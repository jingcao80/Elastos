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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/ViewConfiguration.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CViewConfigurationHelper, Singleton, IViewConfigurationHelper)

CAR_SINGLETON_IMPL(CViewConfigurationHelper)

ECode CViewConfigurationHelper::Get(
    /* [in] */ IContext* context,
    /* [out] */ IViewConfiguration** viewConfig)
{
    VALIDATE_NOT_NULL(viewConfig);
    AutoPtr<IViewConfiguration> temp = ViewConfiguration::Get(context);
    *viewConfig = temp;
    REFCOUNT_ADD(*viewConfig);

    return NOERROR;
}

ECode CViewConfigurationHelper::GetScrollBarSize(
    /* [out] */ Int32* scrollBarSize)
{
    VALIDATE_NOT_NULL(scrollBarSize);
    *scrollBarSize = ViewConfiguration::GetScrollBarSize();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetScrollBarFadeDuration(
    /* [out] */ Int32* scrollBarFadeDuration)
{
    VALIDATE_NOT_NULL(scrollBarFadeDuration);
    *scrollBarFadeDuration = ViewConfiguration::GetScrollBarFadeDuration();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetScrollDefaultDelay(
    /* [out] */ Int32* scrollDefaultDelay)
{
    VALIDATE_NOT_NULL(scrollDefaultDelay);
    *scrollDefaultDelay = ViewConfiguration::GetScrollDefaultDelay();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetFadingEdgeLength(
    /* [out] */ Int32* fadingEdgeLength)
{
    VALIDATE_NOT_NULL(fadingEdgeLength);
    *fadingEdgeLength = ViewConfiguration::GetFadingEdgeLength();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetPressedStateDuration(
    /* [out] */ Int32* pressedStateDuration)
{
    VALIDATE_NOT_NULL(pressedStateDuration);
    *pressedStateDuration = ViewConfiguration::GetPressedStateDuration();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetLongPressTimeout(
    /* [out] */ Int32* longPressTimeout)
{
    VALIDATE_NOT_NULL(longPressTimeout);
    *longPressTimeout = ViewConfiguration::GetLongPressTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetKeyRepeatTimeout(
    /* [out] */ Int32* keyRepeatTimeout)
{
    VALIDATE_NOT_NULL(keyRepeatTimeout);
    *keyRepeatTimeout = ViewConfiguration::GetKeyRepeatTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetKeyRepeatDelay(
    /* [out] */ Int32* keyRepeatDelay)
{
    VALIDATE_NOT_NULL(keyRepeatDelay);
    *keyRepeatDelay = ViewConfiguration::GetKeyRepeatDelay();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetTapTimeout(
    /* [out] */ Int32* tapTimeout)
{
    VALIDATE_NOT_NULL(tapTimeout);
    *tapTimeout = ViewConfiguration::GetTapTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetJumpTapTimeout(
    /* [out] */ Int32* jumpTapTimeout)
{
    VALIDATE_NOT_NULL(jumpTapTimeout);
    *jumpTapTimeout = ViewConfiguration::GetJumpTapTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetDoubleTapTimeout(
    /* [out] */ Int32* doubleTapTimeout)
{
    VALIDATE_NOT_NULL(doubleTapTimeout);
    *doubleTapTimeout = ViewConfiguration::GetDoubleTapTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetDoubleTapMinTime(
    /* [out] */ Int32* doubleTapMinTime)
{
    VALIDATE_NOT_NULL(doubleTapMinTime);
    *doubleTapMinTime = ViewConfiguration::GetDoubleTapMinTime();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetHoverTapTimeout(
    /* [out] */ Int32* hoverTapTimeout)
{
    VALIDATE_NOT_NULL(hoverTapTimeout);
    *hoverTapTimeout = ViewConfiguration::GetHoverTapTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetHoverTapSlop(
    /* [out] */ Int32* hoverTapSlop)
{
    VALIDATE_NOT_NULL(hoverTapSlop);
    *hoverTapSlop = ViewConfiguration::GetHoverTapSlop();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetEdgeSlop(
    /* [out] */ Int32* edgeSlop)
{
    VALIDATE_NOT_NULL(edgeSlop);
    *edgeSlop = ViewConfiguration::GetEdgeSlop();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetTouchSlop(
    /* [out] */ Int32* touchSlop)
{
    VALIDATE_NOT_NULL(touchSlop);
    *touchSlop = ViewConfiguration::GetTouchSlop();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetDoubleTapSlop(
    /* [out] */ Int32* doubleTapSlop)
{
    VALIDATE_NOT_NULL(doubleTapSlop);
    *doubleTapSlop = ViewConfiguration::GetDoubleTapSlop();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetSendRecurringAccessibilityEventsInterval(
    /* [out] */ Int64* sendRecurringAccessibilityEventsInterval)
{
    VALIDATE_NOT_NULL(sendRecurringAccessibilityEventsInterval);
    *sendRecurringAccessibilityEventsInterval =
            ViewConfiguration::GetSendRecurringAccessibilityEventsInterval();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetWindowTouchSlop(
    /* [out] */ Int32* windowTouchSlop)
{
    VALIDATE_NOT_NULL(windowTouchSlop);
    *windowTouchSlop = ViewConfiguration::GetWindowTouchSlop();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetMinimumFlingVelocity(
    /* [out] */ Int32* minimumFlingVelocity)
{
    VALIDATE_NOT_NULL(minimumFlingVelocity);
    *minimumFlingVelocity = ViewConfiguration::GetMinimumFlingVelocity();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetMaximumFlingVelocity(
    /* [out] */ Int32* maximumFlingVelocity)
{
    VALIDATE_NOT_NULL(maximumFlingVelocity);
    *maximumFlingVelocity = ViewConfiguration::GetMaximumFlingVelocity();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetMaximumDrawingCacheSize(
    /* [out] */ Int32* maximumDrawingCacheSize)
{
    VALIDATE_NOT_NULL(maximumDrawingCacheSize);
    *maximumDrawingCacheSize = ViewConfiguration::GetMaximumDrawingCacheSize();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetZoomControlsTimeout(
    /* [out] */ Int64* zoomControlsTimeout)
{
    VALIDATE_NOT_NULL(zoomControlsTimeout);
    *zoomControlsTimeout = ViewConfiguration::GetZoomControlsTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetGlobalActionKeyTimeout(
    /* [out] */ Int64* globalActionKeyTimeout)
{
    VALIDATE_NOT_NULL(globalActionKeyTimeout);
    *globalActionKeyTimeout = ViewConfiguration::GetGlobalActionKeyTimeout();

    return NOERROR;
}

ECode CViewConfigurationHelper::GetScrollFriction(
    /* [out] */ Float* scrollFriction)
{
    VALIDATE_NOT_NULL(scrollFriction);
    *scrollFriction = ViewConfiguration::GetScrollFriction();

    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
