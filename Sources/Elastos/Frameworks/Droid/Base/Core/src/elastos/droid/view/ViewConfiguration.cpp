
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/view/CViewConfiguration.h"
// #include "elastos/droid/view/CWindowManagerGlobal.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CSparseArray;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 ViewConfiguration::SCROLL_BAR_SIZE = 10;
const Int32 ViewConfiguration::SCROLL_BAR_FADE_DURATION = 250;
const Int32 ViewConfiguration::SCROLL_BAR_DEFAULT_DELAY = 300;
const Int32 ViewConfiguration::FADING_EDGE_LENGTH = 12;
const Int32 ViewConfiguration::PRESSED_STATE_DURATION = 64;
const Int32 ViewConfiguration::DEFAULT_LONG_PRESS_TIMEOUT = 500;
const Int32 ViewConfiguration::KEY_REPEAT_DELAY = 50;
const Int32 ViewConfiguration::GLOBAL_ACTIONS_KEY_TIMEOUT = 500;
const Int32 ViewConfiguration::TAP_TIMEOUT = 100;
const Int32 ViewConfiguration::JUMP_TAP_TIMEOUT = 500;
const Int32 ViewConfiguration::DOUBLE_TAP_TIMEOUT = 300;
const Int32 ViewConfiguration::DOUBLE_TAP_MIN_TIME = 40;
const Int32 ViewConfiguration::HOVER_TAP_TIMEOUT = 150;
const Int32 ViewConfiguration::HOVER_TAP_SLOP = 20;
const Int32 ViewConfiguration::ZOOM_CONTROLS_TIMEOUT = 3000;
const Int32 ViewConfiguration::EDGE_SLOP = 12;
const Int32 ViewConfiguration::TOUCH_SLOP = 8;
const Int32 ViewConfiguration::DOUBLE_TAP_TOUCH_SLOP = TOUCH_SLOP;
const Int32 ViewConfiguration::PAGING_TOUCH_SLOP = TOUCH_SLOP * 2;
const Int32 ViewConfiguration::DOUBLE_TAP_SLOP = 100;
const Int32 ViewConfiguration::WINDOW_TOUCH_SLOP = 16;
const Int32 ViewConfiguration::MINIMUM_FLING_VELOCITY = 50;
const Int32 ViewConfiguration::MAXIMUM_FLING_VELOCITY = 8000;
const Int64 ViewConfiguration::SEND_RECURRING_ACCESSIBILITY_EVENTS_INTERVAL_MILLIS = 100;
const Int32 ViewConfiguration::MAXIMUM_DRAWING_CACHE_SIZE = 480 * 800 * 4; // ARGB8888
const Float ViewConfiguration::SCROLL_FRICTION = 0.015f;
const Int32 ViewConfiguration::OVERSCROLL_DISTANCE = 0;
const Int32 ViewConfiguration::OVERFLING_DISTANCE = 6;
const Int32 ViewConfiguration::HAS_PERMANENT_MENU_KEY_AUTODETECT = 0;
const Int32 ViewConfiguration::HAS_PERMANENT_MENU_KEY_TRUE = 1;
const Int32 ViewConfiguration::HAS_PERMANENT_MENU_KEY_FALSE = 2;

static AutoPtr<ISparseArray> InitsConfigurations(
    /* [in] */ Int32 data)
{
    AutoPtr<CSparseArray> array;
    CSparseArray::NewByFriend(data, (CSparseArray**)&array);
    return (ISparseArray*)array.Get();
}

AutoPtr<ISparseArray> ViewConfiguration::sConfigurations = InitsConfigurations(2);

CAR_INTERFACE_IMPL(ViewConfiguration, Object, IViewConfiguration);

ViewConfiguration::ViewConfiguration()
    : mEdgeSlop(0)
    , mFadingEdgeLength(0)
    , mMinimumFlingVelocity(0)
    , mMaximumFlingVelocity(0)
    , mScrollbarSize(0)
    , mTouchSlop(0)
    , mDoubleTapTouchSlop(0)
    , mPagingTouchSlop(0)
    , mDoubleTapSlop(0)
    , mWindowTouchSlop(0)
    , mMaximumDrawingCacheSize(0)
    , mOverscrollDistance(0)
    , mOverflingDistance(0)
    , mFadingMarqueeEnabled(FALSE)
    , mGlobalActionsKeyTimeout(0)
    , sHasPermanentMenuKey(FALSE)
    , sHasPermanentMenuKeySet(FALSE)
{
}

ViewConfiguration::~ViewConfiguration()
{}

ECode ViewConfiguration::constructor()
{
    mEdgeSlop = EDGE_SLOP;
    mFadingEdgeLength = FADING_EDGE_LENGTH;
    mMinimumFlingVelocity = MINIMUM_FLING_VELOCITY;
    mMaximumFlingVelocity = MAXIMUM_FLING_VELOCITY;
    mScrollbarSize = SCROLL_BAR_SIZE;
    mTouchSlop = TOUCH_SLOP;
    mDoubleTapTouchSlop = DOUBLE_TAP_TOUCH_SLOP;
    mPagingTouchSlop = PAGING_TOUCH_SLOP;
    mDoubleTapSlop = DOUBLE_TAP_SLOP;
    mWindowTouchSlop = WINDOW_TOUCH_SLOP;
    //noinspection deprecation
    mMaximumDrawingCacheSize = MAXIMUM_DRAWING_CACHE_SIZE;
    mOverscrollDistance = OVERSCROLL_DISTANCE;
    mOverflingDistance = OVERFLING_DISTANCE;
    mFadingMarqueeEnabled = TRUE;
    mGlobalActionsKeyTimeout = GLOBAL_ACTIONS_KEY_TIMEOUT;

    return NOERROR;
}

ECode ViewConfiguration::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Float density;
    metrics->GetDensity(&density);
    Float sizeAndDensity = 0.f;
    Boolean result;
    config->IsLayoutSizeAtLeast(IConfiguration::SCREENLAYOUT_SIZE_XLARGE, &result);
    if (result) {
        sizeAndDensity = density * 1.5f;
    }
    else {
        sizeAndDensity = density;
    }

    mEdgeSlop = (Int32)(density * EDGE_SLOP + 0.5f);
    mFadingEdgeLength = (Int32)(density * FADING_EDGE_LENGTH + 0.5f);
    mScrollbarSize = (Int32)(density * SCROLL_BAR_SIZE + 0.5f);
    mDoubleTapSlop = (Int32)(density * DOUBLE_TAP_SLOP + 0.5f);
    mWindowTouchSlop = (Int32)(density * WINDOW_TOUCH_SLOP + 0.5f);

    // Size of the screen in bytes, in ARGB_8888 format
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service);
    AutoPtr<IWindowManager> win = IWindowManager::Probe(service);
    assert(win != NULL);
    AutoPtr<IDisplay> display;
    win->GetDefaultDisplay((IDisplay**)&display);
    AutoPtr<CPoint> size;
    CPoint::NewByFriend((CPoint**)&size);
    display->GetRealSize(size.Get());
    mMaximumDrawingCacheSize = 4 * size->mX * size->mY;

    mOverscrollDistance = (Int32)(sizeAndDensity * OVERSCROLL_DISTANCE + 0.5f);
    mOverflingDistance = (Int32)(sizeAndDensity * OVERFLING_DISTANCE + 0.5f);

    if (!sHasPermanentMenuKeySet) {
        Int32 configVal;
        res->GetInteger(R::integer::config_overrideHasPermanentMenuKey, &configVal);

        switch (configVal) {
            default:
            case HAS_PERMANENT_MENU_KEY_AUTODETECT: {
                assert (0 && "TODO");
                // AutoPtr<IIWindowManager> wm = CWindowManagerGlobal::GetWindowManagerService();
                // Boolean hasNavigationBar;
                // if (FAILED(wm->HasNavigationBar(&hasNavigationBar))) {
                //     sHasPermanentMenuKey = FALSE;
                // }
                // else {
                //     sHasPermanentMenuKey = !hasNavigationBar;
                //     sHasPermanentMenuKeySet = TRUE;
                // }
            }
            break;

            case HAS_PERMANENT_MENU_KEY_TRUE:
                sHasPermanentMenuKey = TRUE;
                sHasPermanentMenuKeySet = TRUE;
                break;

            case HAS_PERMANENT_MENU_KEY_FALSE:
                sHasPermanentMenuKey = FALSE;
                sHasPermanentMenuKeySet = TRUE;
                break;
        }
    }

    res->GetBoolean(R::bool_::config_ui_enableFadingMarquee, &mFadingMarqueeEnabled);
    res->GetDimensionPixelSize(R::dimen::config_viewConfigurationTouchSlop, &mTouchSlop);
    mPagingTouchSlop = mTouchSlop * 2;

    mDoubleTapTouchSlop = mTouchSlop;

    res->GetDimensionPixelSize(R::dimen::config_viewMinFlingVelocity, &mMinimumFlingVelocity);
    res->GetDimensionPixelSize(R::dimen::config_viewMaxFlingVelocity, &mMaximumFlingVelocity);
    Int32 data;
    res->GetInteger(R::integer::config_globalActionsKeyTimeout, &data);
    mGlobalActionsKeyTimeout = (Int64)data;

    return NOERROR;
}

AutoPtr<IViewConfiguration> ViewConfiguration::Get(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float fdensity;
    metrics->GetDensity(&fdensity);
    const Int32 density = (Int32)(100.0f * fdensity);

    AutoPtr<IInterface> obj;
    sConfigurations->Get(density, (IInterface**)&obj);
    AutoPtr<IViewConfiguration> configuration = IViewConfiguration::Probe(obj);
    if (configuration == NULL) {
        AutoPtr<CViewConfiguration> config;
        CViewConfiguration::NewByFriend(context, (CViewConfiguration**)&config);
        configuration = (IViewConfiguration*)config.Get();
        sConfigurations->Put(density, configuration);
    }
    return configuration;
}

Int32 ViewConfiguration::GetScrollBarSize()
{
    return SCROLL_BAR_SIZE;
}

ECode ViewConfiguration::GetScaledScrollBarSize(
    /* [out] */ Int32* scrollbarSize)
{
    VALIDATE_NOT_NULL(scrollbarSize);
    *scrollbarSize = mScrollbarSize;

    return NOERROR;
}

Int32 ViewConfiguration::GetScrollBarFadeDuration()
{
    return SCROLL_BAR_FADE_DURATION;
}

Int32 ViewConfiguration::GetScrollDefaultDelay()
{
    return SCROLL_BAR_DEFAULT_DELAY;
}

Int32 ViewConfiguration::GetFadingEdgeLength()
{
    return FADING_EDGE_LENGTH;
}

ECode ViewConfiguration::GetScaledFadingEdgeLength(
    /* [out] */ Int32* fadingEdgeLength)
{
    VALIDATE_NOT_NULL(fadingEdgeLength);
    *fadingEdgeLength = mFadingEdgeLength;

    return NOERROR;
}

Int32 ViewConfiguration::GetPressedStateDuration()
{
    return PRESSED_STATE_DURATION;
}

Int32 ViewConfiguration::GetLongPressTimeout()
{
    return DEFAULT_LONG_PRESS_TIMEOUT;
}

Int32 ViewConfiguration::GetKeyRepeatTimeout()
{
    return GetLongPressTimeout();
}

Int32 ViewConfiguration::GetKeyRepeatDelay()
{
    return KEY_REPEAT_DELAY;
}

Int32 ViewConfiguration::GetTapTimeout()
{
    return TAP_TIMEOUT;
}

Int32 ViewConfiguration::GetJumpTapTimeout()
{
    return JUMP_TAP_TIMEOUT;
}

Int32 ViewConfiguration::GetDoubleTapTimeout()
{
    return DOUBLE_TAP_TIMEOUT;
}

Int32 ViewConfiguration::GetDoubleTapMinTime()
{
    return DOUBLE_TAP_MIN_TIME;
}

Int32 ViewConfiguration::GetHoverTapTimeout()
{
    return HOVER_TAP_TIMEOUT;
}

Int32 ViewConfiguration::GetHoverTapSlop()
{
    return HOVER_TAP_SLOP;
}

Int32 ViewConfiguration::GetEdgeSlop()
{
    return EDGE_SLOP;
}

ECode ViewConfiguration::GetScaledEdgeSlop(
    /* [out] */ Int32* edgeSlop)
{
    VALIDATE_NOT_NULL(edgeSlop);
    *edgeSlop = mEdgeSlop;
    return NOERROR;
}

Int32 ViewConfiguration::GetTouchSlop()
{
    return TOUCH_SLOP;
}

ECode ViewConfiguration::GetScaledTouchSlop(
    /* [out] */ Int32* touchSlop)
{
    VALIDATE_NOT_NULL(touchSlop);
    *touchSlop = mTouchSlop;
    return NOERROR;
}

ECode ViewConfiguration::GetScaledDoubleTapTouchSlop(
    /* [out] */ Int32* doubleTapTouchSlop)
{
    VALIDATE_NOT_NULL(doubleTapTouchSlop);
    *doubleTapTouchSlop = mDoubleTapTouchSlop;
    return NOERROR;
}

ECode ViewConfiguration::GetScaledPagingTouchSlop(
    /* [out] */ Int32* pagingTouchSlop)
{
    VALIDATE_NOT_NULL(pagingTouchSlop);
    *pagingTouchSlop = mPagingTouchSlop;
    return NOERROR;
}

Int32 ViewConfiguration::GetDoubleTapSlop()
{
    return DOUBLE_TAP_SLOP;
}

ECode ViewConfiguration::GetScaledDoubleTapSlop(
    /* [out] */ Int32* doubleTapSlop)
{
    VALIDATE_NOT_NULL(doubleTapSlop);
    *doubleTapSlop = mDoubleTapSlop;
    return NOERROR;
}

Int64 ViewConfiguration::GetSendRecurringAccessibilityEventsInterval()
{
    return SEND_RECURRING_ACCESSIBILITY_EVENTS_INTERVAL_MILLIS;
}

Int32 ViewConfiguration::GetWindowTouchSlop()
{
    return WINDOW_TOUCH_SLOP;
}

ECode ViewConfiguration::GetScaledWindowTouchSlop(
    /* [out] */ Int32* windowTouchSlop)
{
    VALIDATE_NOT_NULL(windowTouchSlop);
    *windowTouchSlop = mWindowTouchSlop;
    return NOERROR;
}

Int32 ViewConfiguration::GetMinimumFlingVelocity()
{
    return MINIMUM_FLING_VELOCITY;
}

ECode ViewConfiguration::GetScaledMinimumFlingVelocity(
    /* [out] */ Int32* minimumFlingVelocity)
{
    VALIDATE_NOT_NULL(minimumFlingVelocity);
    *minimumFlingVelocity = mMinimumFlingVelocity;
    return NOERROR;
}

Int32 ViewConfiguration::GetMaximumFlingVelocity()
{
    return MAXIMUM_FLING_VELOCITY;
}

ECode ViewConfiguration::GetScaledMaximumFlingVelocity(
    /* [out] */ Int32* maximumFlingVelocity)
{
    VALIDATE_NOT_NULL(maximumFlingVelocity);
    *maximumFlingVelocity = mMaximumFlingVelocity;
    return NOERROR;
}

Int32 ViewConfiguration::GetMaximumDrawingCacheSize()
{
    //noinspection deprecation
    return MAXIMUM_DRAWING_CACHE_SIZE;
}

ECode ViewConfiguration::GetScaledMaximumDrawingCacheSize(
    /* [out] */ Int32* maximumDrawingCacheSize)
{
    VALIDATE_NOT_NULL(maximumDrawingCacheSize);
    *maximumDrawingCacheSize = mMaximumDrawingCacheSize;
    return NOERROR;
}

ECode ViewConfiguration::GetScaledOverscrollDistance(
    /* [out] */ Int32* overscrollDistance)
{
    VALIDATE_NOT_NULL(overscrollDistance);
    *overscrollDistance = mOverscrollDistance;
    return NOERROR;
}

ECode ViewConfiguration::GetScaledOverflingDistance(
    /* [out] */ Int32* overflingDistance)
{
    VALIDATE_NOT_NULL(overflingDistance);
    *overflingDistance = mOverflingDistance;
    return NOERROR;
}

Int64 ViewConfiguration::GetZoomControlsTimeout()
{
    return ZOOM_CONTROLS_TIMEOUT;
}

Int64 ViewConfiguration::GetGlobalActionKeyTimeout()
{
    return GLOBAL_ACTIONS_KEY_TIMEOUT;
}

ECode ViewConfiguration::GetDeviceGlobalActionKeyTimeout(
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout);
    *timeout = mGlobalActionsKeyTimeout;

    return NOERROR;
}

Float ViewConfiguration::GetScrollFriction()
{
    return SCROLL_FRICTION;
}

ECode ViewConfiguration::HasPermanentMenuKey(
    /* [out] */ Boolean* hasPermanentMenuKey)
{
    VALIDATE_NOT_NULL(hasPermanentMenuKey);
    *hasPermanentMenuKey = sHasPermanentMenuKey;
    return NOERROR;
}

ECode ViewConfiguration::IsFadingMarqueeEnabled(
    /* [out] */ Boolean* fadingMarqueeEnabled)
{
    VALIDATE_NOT_NULL(fadingMarqueeEnabled);
    *fadingMarqueeEnabled = mFadingMarqueeEnabled;
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
