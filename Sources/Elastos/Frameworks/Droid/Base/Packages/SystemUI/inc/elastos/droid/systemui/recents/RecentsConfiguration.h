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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSCONFIGURATION_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSCONFIGURATION_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::SystemUI::Recents::Misc::SystemServicesProxy;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

/** A static Recents configuration for the current context
 * NOTE: We should not hold any references to a Context from a static instance */
class RecentsConfiguration
    : public Object
    , public IRecentsConfiguration
{
public:
    CAR_INTERFACE_DECL()

    /** Updates the configuration to the current context */
    static CARAPI_(AutoPtr<RecentsConfiguration>) Reinitialize(
        /* [in] */ IContext* context,
        /* [in] */ SystemServicesProxy* ssp);

    /** Returns the current recents configuration */
    static CARAPI_(AutoPtr<RecentsConfiguration>) GetInstance();

    /** Updates the state, given the specified context */
    CARAPI Update(
        /* [in] */ IContext* context);

    /** Updates the system insets */
    CARAPI UpdateSystemInsets(
        /* [in] */ IRect* insets);

    /** Updates the search bar app widget */
    CARAPI UpdateSearchBarAppWidgetId(
        /* [in] */ IContext* context,
        /* [in] */ Int32 appWidgetId);

    /** Updates the states that need to be re-read whenever we re-initialize. */
    CARAPI UpdateOnReinitialize(
        /* [in] */ IContext* context,
        /* [in] */ SystemServicesProxy* ssp);

    /** Called when the configuration has changed, and we want to reset any configuration specific
     * members. */
    CARAPI UpdateOnConfigurationChange();

    /** Returns whether the search bar app widget exists. */
    CARAPI_(Boolean) HasSearchBarAppWidget();

    /** Returns whether the status bar scrim should be animated when shown for the first time. */
    CARAPI_(Boolean) ShouldAnimateStatusBarScrim();

    /** Returns whether the status bar scrim should be visible. */
    CARAPI_(Boolean) HasStatusBarScrim();

    /** Returns whether the nav bar scrim should be animated when shown for the first time. */
    CARAPI_(Boolean) ShouldAnimateNavBarScrim();

    /** Returns whether the nav bar scrim should be visible. */
    CARAPI_(Boolean) HasNavBarScrim();

    /** Returns whether the current layout is horizontal. */
    CARAPI_(Boolean) HasHorizontalLayout();

    /**
     * Returns the task stack bounds in the current orientation. These bounds do not account for
     * the system insets.
     */
    CARAPI GetTaskStackBounds(
        /* [in] */ Int32 windowWidth,
        /* [in] */ Int32 windowHeight,
        /* [in] */ Int32 topInset,
        /* [in] */ Int32 rightInset,
        /* [in] */ IRect* taskStackBounds);

    /**
     * Returns the search bar bounds in the current orientation.  These bounds do not account for
     * the system insets.
     */
    CARAPI GetSearchBarBounds(
        /* [in] */ Int32 windowWidth,
        /* [in] */ Int32 windowHeight,
        /* [in] */ Int32 topInset,
        /* [in] */ IRect* searchBarSpaceBounds);

private:
    /** Private constructor */
    RecentsConfiguration(
        /* [in] */ IContext* context);

    static AutoPtr<RecentsConfiguration> sInstance;
    static Int32 sPrevConfigurationHashCode;

public:
    /** Animations */
    Float mAnimationPxMovementPerSecond;

    /** Interpolators */
    AutoPtr<ITimeInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<ITimeInterpolator> mFastOutLinearInInterpolator;
    AutoPtr<ITimeInterpolator> mLinearOutSlowInInterpolator;
    AutoPtr<ITimeInterpolator> mQuintOutInterpolator;

    /** Filtering */
    Int32 mFilteringCurrentViewsAnimDuration;
    Int32 mFilteringNewViewsAnimDuration;

    /** Insets */
    AutoPtr<IRect> mSystemInsets;
    AutoPtr<IRect> mDisplayRect;

    /** Layout */
    Boolean mIsLandscape;
    Boolean mHasTransposedSearchBar;
    Boolean mHasTransposedNavBar;

    /** Loading */
    Int32 mMaxNumTasksToLoad;

    /** Search bar */
    Int32 mSearchBarAppWidgetId;
    Int32 mSearchBarSpaceHeightPx;

    /** Task stack */
    Int32 mTaskStackScrollDuration;
    Int32 mTaskStackMaxDim;
    Int32 mTaskStackTopPaddingPx;
    Float mTaskStackWidthPaddingPct;
    Float mTaskStackOverscrollPct;

    /** Task view animation and styles */
    Int32 mTaskViewEnterFromHomeDelay;
    Int32 mTaskViewEnterFromHomeDuration;
    Int32 mTaskViewEnterFromHomeStaggerDelay;
    Int32 mTaskViewEnterFromHomeStaggerDuration;
    Int32 mTaskViewExitToHomeDuration;
    Int32 mTaskViewRemoveAnimDuration;
    Int32 mTaskViewRemoveAnimTranslationXPx;
    Int32 mTaskViewTranslationZMinPx;
    Int32 mTaskViewTranslationZMaxPx;
    Int32 mTaskViewRoundedCornerRadiusPx;
    Int32 mTaskViewHighlightPx;
    Int32 mTaskViewAffiliateGroupEnterOffsetPx;
    Float mTaskViewThumbnailAlpha;

    /** Task bar colors */
    Int32 mTaskBarViewDefaultBackgroundColor;
    Int32 mTaskBarViewLightTextColor;
    Int32 mTaskBarViewDarkTextColor;
    Int32 mTaskBarViewHighlightColor;
    Float mTaskBarViewAffiliationColorMinAlpha;

    /** Task bar size & animations */
    Int32 mTaskBarHeight;
    Int32 mTaskBarEnterAnimDuration;
    Int32 mTaskBarEnterAnimDelay;
    Int32 mTaskBarExitAnimDuration;
    Int32 mTaskBarDismissDozeDelaySeconds;

    /** Lock to app */
    Int32 mTaskViewLockToAppButtonHeight;
    Int32 mTaskViewLockToAppShortAnimDuration;
    Int32 mTaskViewLockToAppLongAnimDuration;

    /** Nav bar scrim */
    Int32 mNavBarScrimEnterDuration;

    /** Launch states */
    Boolean mLaunchedWithAltTab;
    Boolean mLaunchedWithNoRecentTasks;
    Boolean mLaunchedFromAppWithThumbnail;
    Boolean mLaunchedFromAppWithScreenshot;
    Boolean mLaunchedFromHome;
    Int32 mLaunchedToTaskId;

    /** Misc **/
    Boolean mUseHardwareLayers;
    Int32 mAltTabKeyDelay;
    Boolean mFakeShadows;

    /** Dev options and global settings */
    Boolean mLockToAppEnabled;
    Boolean mDeveloperOptionsEnabled;
    Boolean mDebugModeEnabled;
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSCONFIGURATION_H__
