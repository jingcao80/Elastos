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

#include "elastos/droid/systemui/statusbar/phone/KeyguardClockPositionAlgorithm.h"
#include "../R.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::CPathInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

static AutoPtr<IPathInterpolator> InitSlowDownInterpolator()
{
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    path->MoveTo(0, 0);
    path->CubicTo(0.3f, 0.875f, 0.6f, 1.0f, 1.0f, 1.0f);
    AutoPtr<IPathInterpolator> pi;
    CPathInterpolator::New(path, (IPathInterpolator**)&pi);
    return pi;
}

const Float KeyguardClockPositionAlgorithm::SLOW_DOWN_FACTOR = 0.4f;
const Float KeyguardClockPositionAlgorithm::CLOCK_RUBBERBAND_FACTOR_MIN = 0.08f;
const Float KeyguardClockPositionAlgorithm::CLOCK_RUBBERBAND_FACTOR_MAX = 0.8f;
const Float KeyguardClockPositionAlgorithm::CLOCK_SCALE_FADE_START = 0.95f;
const Float KeyguardClockPositionAlgorithm::CLOCK_SCALE_FADE_END = 0.75f;
const Float KeyguardClockPositionAlgorithm::CLOCK_SCALE_FADE_END_NO_NOTIFS = 0.5f;
const Float KeyguardClockPositionAlgorithm::CLOCK_ADJ_TOP_PADDING_MULTIPLIER_MIN = 1.4f;
const Float KeyguardClockPositionAlgorithm::CLOCK_ADJ_TOP_PADDING_MULTIPLIER_MAX = 3.2f;
AutoPtr<IPathInterpolator> KeyguardClockPositionAlgorithm::sSlowDownInterpolator = InitSlowDownInterpolator();

KeyguardClockPositionAlgorithm::Result::Result()
    : mClockY(0)
    , mClockScale(0)
    , mClockAlpha(0)
    , mStackScrollerPadding(0)
    , mStackScrollerPaddingAdjustment(0)
{}


KeyguardClockPositionAlgorithm::KeyguardClockPositionAlgorithm()
    : mClockNotificationsMarginMin(0)
    , mClockNotificationsMarginMax(0)
    , mClockYFractionMin(0)
    , mClockYFractionMax(0)
    , mMaxKeyguardNotifications(0)
    , mMaxPanelHeight(0)
    , mExpandedHeight(0)
    , mNotificationCount(0)
    , mHeight(0)
    , mKeyguardStatusHeight(0)
    , mEmptyDragAmount(0)
    , mDensity(0)
    , mMoreCardNotificationAmount(0)
{
    CAccelerateInterpolator::New((IAccelerateInterpolator**)&mAccelerateInterpolator);
}

void KeyguardClockPositionAlgorithm::LoadDimens(
    /* [in] */ IResources* res)
{
    res->GetDimensionPixelSize(R::dimen::keyguard_clock_notifications_margin_min, &mClockNotificationsMarginMin);
    res->GetDimensionPixelSize(R::dimen::keyguard_clock_notifications_margin_max, &mClockNotificationsMarginMax);
    res->GetFraction(R::fraction::keyguard_clock_y_fraction_min, 1, 1, &mClockYFractionMin);
    res->GetFraction(R::fraction::keyguard_clock_y_fraction_max, 1, 1, &mClockYFractionMax);
    Int32 iv1 = 0, iv2 = 0;
    res->GetDimensionPixelSize(R::dimen::notification_summary_height, &iv1);
    res->GetDimensionPixelSize(R::dimen::notification_min_height, &iv2);
    mMoreCardNotificationAmount = (Float) iv1 / iv2;
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&mDensity);
}

void KeyguardClockPositionAlgorithm::Setup(
    /* [in] */ Int32 maxKeyguardNotifications,
    /* [in] */ Int32 maxPanelHeight,
    /* [in] */ Float expandedHeight,
    /* [in] */ Int32 notificationCount,
    /* [in] */ Int32 height,
    /* [in] */ Int32 keyguardStatusHeight,
    /* [in] */ Float emptyDragAmount)
{
    mMaxKeyguardNotifications = maxKeyguardNotifications;
    mMaxPanelHeight = maxPanelHeight;
    mExpandedHeight = expandedHeight;
    mNotificationCount = notificationCount;
    mHeight = height;
    mKeyguardStatusHeight = keyguardStatusHeight;
    mEmptyDragAmount = emptyDragAmount;
}

void KeyguardClockPositionAlgorithm::Run(
    /* [in] */ Result* result)
{
    Int32 y = GetClockY() - mKeyguardStatusHeight / 2;
    Float clockAdjustment = GetClockYExpansionAdjustment();
    Float topPaddingAdjMultiplier = GetTopPaddingAdjMultiplier();
    result->mStackScrollerPaddingAdjustment = (Int32) (clockAdjustment*topPaddingAdjMultiplier);
    Int32 clockNotificationsPadding = GetClockNotificationsPadding()
            + result->mStackScrollerPaddingAdjustment;
    Int32 padding = y + clockNotificationsPadding;
    result->mClockY = y;
    result->mStackScrollerPadding = mKeyguardStatusHeight + padding;
    result->mClockScale = GetClockScale(result->mStackScrollerPadding,
            result->mClockY,
            y + GetClockNotificationsPadding() + mKeyguardStatusHeight);
    result->mClockAlpha = GetClockAlpha(result->mClockScale);
}

Float KeyguardClockPositionAlgorithm::GetClockScale(
    /* [in] */ Int32 notificationPadding,
    /* [in] */ Int32 clockY,
    /* [in] */ Int32 startPadding)
{
    Float scaleMultiplier = GetNotificationAmountT() == 0 ? 6.0f : 5.0f;
    Float scaleEnd = clockY - mKeyguardStatusHeight * scaleMultiplier;
    Float distanceToScaleEnd = notificationPadding - scaleEnd;
    Float progress = distanceToScaleEnd / (startPadding - scaleEnd);
    progress = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(progress, 1.0f));
    ITimeInterpolator::Probe(mAccelerateInterpolator)->GetInterpolation(progress, &progress);
    progress *= Elastos::Core::Math::Pow(1 + mEmptyDragAmount / mDensity / 300, 0.3f);
    return progress;
}

Int32 KeyguardClockPositionAlgorithm::GetClockNotificationsPadding()
{
    Float t = GetNotificationAmountT();
    t = Elastos::Core::Math::Min(t, 1.0f);
    return (Int32) (t * mClockNotificationsMarginMin + (1 - t) * mClockNotificationsMarginMax);
}

Float KeyguardClockPositionAlgorithm::GetClockYFraction()
{
    Float t = GetNotificationAmountT();
    t = Elastos::Core::Math::Min(t, 1.0f);
    return (1 - t) * mClockYFractionMax + t * mClockYFractionMin;
}

Int32 KeyguardClockPositionAlgorithm::GetClockY()
{
    return (Int32) (GetClockYFraction() * mHeight);
}

Float KeyguardClockPositionAlgorithm::GetClockYExpansionAdjustment()
{
    Float rubberbandFactor = GetClockYExpansionRubberbandFactor();
    Float value = (rubberbandFactor * (mMaxPanelHeight - mExpandedHeight));
    Float t = value / mMaxPanelHeight;
    Float fv = 0;
    ITimeInterpolator::Probe(sSlowDownInterpolator)->GetInterpolation(t, &fv);
    Float slowedDownValue = -fv * SLOW_DOWN_FACTOR * mMaxPanelHeight;
    if (mNotificationCount == 0) {
        return (-2 * value + slowedDownValue) / 3;
    }
    return slowedDownValue;
}

Float KeyguardClockPositionAlgorithm::GetClockYExpansionRubberbandFactor()
{
    Float t = GetNotificationAmountT();
    t = Elastos::Core::Math::Min(t, 1.0f);
    t = (Float) Elastos::Core::Math::Pow(t, 0.3f);
    return (1 - t) * CLOCK_RUBBERBAND_FACTOR_MAX + t * CLOCK_RUBBERBAND_FACTOR_MIN;
}

Float KeyguardClockPositionAlgorithm::GetTopPaddingAdjMultiplier()
{
    Float t = GetNotificationAmountT();
    t = Elastos::Core::Math::Min(t, 1.0f);
    return (1 - t) * CLOCK_ADJ_TOP_PADDING_MULTIPLIER_MIN
            + t * CLOCK_ADJ_TOP_PADDING_MULTIPLIER_MAX;
}

Float KeyguardClockPositionAlgorithm::GetClockAlpha(
    /* [in] */ Float scale)
{
    Float fadeEnd = GetNotificationAmountT() == 0.0f
            ? CLOCK_SCALE_FADE_END_NO_NOTIFS
            : CLOCK_SCALE_FADE_END;
    Float alpha = (scale - fadeEnd)
            / (CLOCK_SCALE_FADE_START - fadeEnd);
    return Elastos::Core::Math::Max((Float)0, Elastos::Core::Math::Min((Float)1, alpha));
}

Float KeyguardClockPositionAlgorithm::GetNotificationAmountT()
{
    return mNotificationCount
            / (mMaxKeyguardNotifications + mMoreCardNotificationAmount);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
