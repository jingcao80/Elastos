
#include "elastos/droid/systemui/recents/Constants.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/droid/R.h>
#include "R.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::Animation::AnimationUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

AutoPtr<RecentsConfiguration> RecentsConfiguration::sInstance;
Int32 RecentsConfiguration::sPrevConfigurationHashCode;

CAR_INTERFACE_IMPL(RecentsConfiguration, Object, IRecentsConfiguration)

RecentsConfiguration::RecentsConfiguration(
    /* [in] */ IContext* context)
    : mAnimationPxMovementPerSecond(0.0f)
    , mFilteringCurrentViewsAnimDuration(0)
    , mFilteringNewViewsAnimDuration(0)
    , mIsLandscape(FALSE)
    , mHasTransposedSearchBar(FALSE)
    , mHasTransposedNavBar(FALSE)
    , mMaxNumTasksToLoad(0)
    , mSearchBarAppWidgetId(-1)
    , mSearchBarSpaceHeightPx(0)
    , mTaskStackScrollDuration(0)
    , mTaskStackMaxDim(0)
    , mTaskStackTopPaddingPx(0)
    , mTaskStackWidthPaddingPct(0.0f)
    , mTaskStackOverscrollPct(0.0f)
    , mTaskViewEnterFromHomeDelay(0)
    , mTaskViewEnterFromHomeDuration(0)
    , mTaskViewEnterFromHomeStaggerDelay(0)
    , mTaskViewEnterFromHomeStaggerDuration(0)
    , mTaskViewExitToHomeDuration(0)
    , mTaskViewRemoveAnimDuration(0)
    , mTaskViewRemoveAnimTranslationXPx(0)
    , mTaskViewTranslationZMinPx(0)
    , mTaskViewTranslationZMaxPx(0)
    , mTaskViewRoundedCornerRadiusPx(0)
    , mTaskViewHighlightPx(0)
    , mTaskViewAffiliateGroupEnterOffsetPx(0)
    , mTaskViewThumbnailAlpha(0.0f)
    , mTaskBarViewDefaultBackgroundColor(0)
    , mTaskBarViewLightTextColor(0)
    , mTaskBarViewDarkTextColor(0)
    , mTaskBarViewHighlightColor(0)
    , mTaskBarViewAffiliationColorMinAlpha(0.0f)
    , mTaskBarHeight(0)
    , mTaskBarEnterAnimDuration(0)
    , mTaskBarEnterAnimDelay(0)
    , mTaskBarExitAnimDuration(0)
    , mTaskBarDismissDozeDelaySeconds(0)
    , mTaskViewLockToAppButtonHeight(0)
    , mTaskViewLockToAppShortAnimDuration(0)
    , mTaskViewLockToAppLongAnimDuration(0)
    , mNavBarScrimEnterDuration(0)
    , mLaunchedWithAltTab(FALSE)
    , mLaunchedWithNoRecentTasks(FALSE)
    , mLaunchedFromAppWithThumbnail(FALSE)
    , mLaunchedFromAppWithScreenshot(FALSE)
    , mLaunchedFromHome(FALSE)
    , mLaunchedToTaskId(0)
    , mUseHardwareLayers(FALSE)
    , mAltTabKeyDelay(0)
    , mFakeShadows(FALSE)
    , mLockToAppEnabled(FALSE)
    , mDeveloperOptionsEnabled(FALSE)
    , mDebugModeEnabled(FALSE)
{
    CRect::New((IRect**)&mSystemInsets);
    CRect::New((IRect**)&mDisplayRect);
    // Properties that don't have to be reloaded with each configuration change can be loaded
    // here.

    // Interpolators
    AutoPtr<IInterpolator> interpolator;
    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_slow_in,
        (IInterpolator**)&interpolator);
    mFastOutSlowInInterpolator = ITimeInterpolator::Probe(interpolator);
    interpolator = NULL;

    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::fast_out_linear_in,
        (IInterpolator**)&interpolator);
    mFastOutLinearInInterpolator = ITimeInterpolator::Probe(interpolator);
    interpolator = NULL;

    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::linear_out_slow_in,
        (IInterpolator**)&interpolator);
    mLinearOutSlowInInterpolator = ITimeInterpolator::Probe(interpolator);
    interpolator = NULL;

    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::decelerate_quint,
        (IInterpolator**)&interpolator);
    mQuintOutInterpolator = ITimeInterpolator::Probe(interpolator);
}

AutoPtr<RecentsConfiguration> RecentsConfiguration::Reinitialize(
    /* [in] */ IContext* context,
    /* [in] */ SystemServicesProxy* ssp)
{
    if (sInstance == NULL) {
        sInstance = new RecentsConfiguration(context);
    }
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 configHashCode = Object::GetHashCode(config);
    if (sPrevConfigurationHashCode != configHashCode) {
        sInstance->Update(context);
        sPrevConfigurationHashCode = configHashCode;
    }
    sInstance->UpdateOnReinitialize(context, ssp);
    return sInstance;
}

/** Returns the current recents configuration */
AutoPtr<RecentsConfiguration> RecentsConfiguration::GetInstance()
{
    return sInstance;
}

/** Updates the state, given the specified context */
ECode RecentsConfiguration::Update(
    /* [in] */ IContext* context)
{
    String pkgName;
    context->GetPackageName(&pkgName);
    AutoPtr<ISharedPreferences> settings;
    context->GetSharedPreferences(pkgName, 0, (ISharedPreferences**)&settings);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);

    // Debug mode
    settings->GetBoolean(Constants::Values::App::Key_DebugModeEnabled, FALSE, &mDebugModeEnabled);
    if (mDebugModeEnabled) {
        // assert(0);
        // Console::Enabled = TRUE;
    }

    // Layout
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    mIsLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;
    res->GetBoolean(R::bool_::recents_has_transposed_search_bar, &mHasTransposedSearchBar);
    res->GetBoolean(R::bool_::recents_has_transposed_nav_bar, &mHasTransposedNavBar);

    // Insets
    Int32 w, h;
    dm->GetWidthPixels(&w);
    dm->GetHeightPixels(&h);
    mDisplayRect->Set(0, 0, w, h);

    // Animations
    Int32 pixelSize;
    res->GetDimensionPixelSize(R::dimen::recents_animation_movement_in_dps_per_second, &pixelSize);
    mAnimationPxMovementPerSecond = pixelSize;

    // Filtering
    res->GetInteger(R::integer::recents_filter_animate_current_views_duration,
            &mFilteringCurrentViewsAnimDuration);
    res->GetInteger(R::integer::recents_filter_animate_new_views_duration,
            &mFilteringNewViewsAnimDuration);

    // Loading
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->GetMaxRecentTasksStatic(&mMaxNumTasksToLoad);

    // Search Bar
    res->GetDimensionPixelSize(R::dimen::recents_search_bar_space_height, &mSearchBarSpaceHeightPx);
    settings->GetInt32(Constants::Values::App::Key_SearchAppWidgetId, -1, &mSearchBarAppWidgetId);

    // Task stack
    res->GetInteger(R::integer::recents_animate_task_stack_scroll_duration,
            &mTaskStackScrollDuration);
    AutoPtr<ITypedValue> widthPaddingPctValue;
    CTypedValue::New((ITypedValue**)&widthPaddingPctValue);
    res->GetValue(R::dimen::recents_stack_width_padding_percentage, widthPaddingPctValue, TRUE);
    widthPaddingPctValue->GetFloat(&mTaskStackWidthPaddingPct);
    AutoPtr<ITypedValue> stackOverscrollPctValue;
    CTypedValue::New((ITypedValue**)&stackOverscrollPctValue);
    res->GetValue(R::dimen::recents_stack_overscroll_percentage, stackOverscrollPctValue, TRUE);
    stackOverscrollPctValue->GetFloat(&mTaskStackOverscrollPct);
    res->GetInteger(R::integer::recents_max_task_stack_view_dim, &mTaskStackMaxDim);
    res->GetDimensionPixelSize(R::dimen::recents_stack_top_padding, &mTaskStackTopPaddingPx);

    // Task view animation and styles
    res->GetInteger(R::integer::recents_animate_task_enter_from_home_delay,
            &mTaskViewEnterFromHomeDelay);
    res->GetInteger(R::integer::recents_animate_task_enter_from_home_duration,
            &mTaskViewEnterFromHomeDuration);
    res->GetInteger(R::integer::recents_animate_task_enter_from_home_stagger_delay,
            &mTaskViewEnterFromHomeStaggerDelay);
    res->GetInteger(R::integer::recents_animate_task_exit_to_home_duration,
            &mTaskViewExitToHomeDuration);
    res->GetInteger(R::integer::recents_animate_task_view_remove_duration,
            &mTaskViewRemoveAnimDuration);
    res->GetDimensionPixelSize(R::dimen::recents_task_view_remove_anim_translation_x,
            &mTaskViewRemoveAnimTranslationXPx);
    res->GetDimensionPixelSize(R::dimen::recents_task_view_rounded_corners_radius,
            &mTaskViewRoundedCornerRadiusPx);
    res->GetDimensionPixelSize(R::dimen::recents_task_view_highlight, &mTaskViewHighlightPx);
    res->GetDimensionPixelSize(R::dimen::recents_task_view_z_min, &mTaskViewTranslationZMinPx);
    res->GetDimensionPixelSize(R::dimen::recents_task_view_z_max, &mTaskViewTranslationZMaxPx);
    res->GetDimensionPixelSize(R::dimen::recents_task_view_affiliate_group_enter_offset,
            &mTaskViewAffiliateGroupEnterOffsetPx);
    AutoPtr<ITypedValue> thumbnailAlphaValue;
    CTypedValue::New((ITypedValue**)&thumbnailAlphaValue);
    res->GetValue(R::dimen::recents_task_view_thumbnail_alpha, thumbnailAlphaValue, TRUE);
    thumbnailAlphaValue->GetFloat(&mTaskViewThumbnailAlpha);

    // Task bar colors
    res->GetColor(R::color::recents_task_bar_default_background_color,
            &mTaskBarViewDefaultBackgroundColor);
    res->GetColor(R::color::recents_task_bar_light_text_color,
            &mTaskBarViewLightTextColor);
    res->GetColor(R::color::recents_task_bar_dark_text_color,
            &mTaskBarViewDarkTextColor);
    res->GetColor(R::color::recents_task_bar_highlight_color,
            &mTaskBarViewHighlightColor);
    AutoPtr<ITypedValue> affMinAlphaPctValue;
    CTypedValue::New((ITypedValue**)&affMinAlphaPctValue);
    res->GetValue(R::dimen::recents_task_affiliation_color_min_alpha_percentage, affMinAlphaPctValue, TRUE);
    affMinAlphaPctValue->GetFloat(&mTaskBarViewAffiliationColorMinAlpha);

    // Task bar size & animations
    res->GetDimensionPixelSize(R::dimen::recents_task_bar_height, &mTaskBarHeight);
    res->GetInteger(R::integer::recents_animate_task_bar_enter_duration,
            &mTaskBarEnterAnimDuration);
    res->GetInteger(R::integer::recents_animate_task_bar_enter_delay,
            &mTaskBarEnterAnimDelay);
    res->GetInteger(R::integer::recents_animate_task_bar_exit_duration,
            &mTaskBarExitAnimDuration);
    res->GetInteger(R::integer::recents_task_bar_dismiss_delay_seconds,
            &mTaskBarDismissDozeDelaySeconds);

    // Lock to app
    res->GetDimensionPixelSize(R::dimen::recents_task_view_lock_to_app_button_height,
            &mTaskViewLockToAppButtonHeight);
    res->GetInteger(R::integer::recents_animate_lock_to_app_button_short_duration,
            &mTaskViewLockToAppShortAnimDuration);
    res->GetInteger(R::integer::recents_animate_lock_to_app_button_long_duration,
            &mTaskViewLockToAppLongAnimDuration);

    // Nav bar scrim
    res->GetInteger(R::integer::recents_nav_bar_scrim_enter_duration,
            &mNavBarScrimEnterDuration);

    // Misc
    res->GetBoolean(R::bool_::config_recents_use_hardware_layers, &mUseHardwareLayers);
    res->GetInteger(R::integer::recents_alt_tab_key_delay, &mAltTabKeyDelay);
    res->GetBoolean(R::bool_::config_recents_fake_shadows, &mFakeShadows);
    return NOERROR;
}

/** Updates the system insets */
ECode RecentsConfiguration::UpdateSystemInsets(
    /* [in] */ IRect* insets)
{
    return mSystemInsets->Set(insets);
}

/** Updates the search bar app widget */
ECode RecentsConfiguration::UpdateSearchBarAppWidgetId(
    /* [in] */ IContext* context,
    /* [in] */ Int32 appWidgetId)
{
    mSearchBarAppWidgetId = appWidgetId;
    String pkgName;
    context->GetPackageName(&pkgName);
    AutoPtr<ISharedPreferences> settings;
    context->GetSharedPreferences(pkgName, 0, (ISharedPreferences**)&settings);
    AutoPtr<ISharedPreferencesEditor> edit;
    settings->Edit((ISharedPreferencesEditor**)&edit);
    edit->PutInt32(Constants::Values::App::Key_SearchAppWidgetId,
            appWidgetId);
    return edit->Apply();
}

/** Updates the states that need to be re-read whenever we re-initialize. */
ECode RecentsConfiguration::UpdateOnReinitialize(
    /* [in] */ IContext* context,
    /* [in] */ SystemServicesProxy* ssp)
{
    // Check if the developer options are enabled
    mDeveloperOptionsEnabled = ssp->GetGlobalSetting(context,
            ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED) != 0;
    mLockToAppEnabled = ssp->GetSystemSetting(context,
            ISettingsSystem::LOCK_TO_APP_ENABLED) != 0;
    return NOERROR;
}

/** Called when the configuration has changed, and we want to reset any configuration specific
 * members. */
ECode RecentsConfiguration::UpdateOnConfigurationChange()
{
    mLaunchedWithAltTab = FALSE;
    mLaunchedWithNoRecentTasks = FALSE;
    mLaunchedFromAppWithThumbnail = FALSE;
    mLaunchedFromAppWithScreenshot = FALSE;
    mLaunchedFromHome = FALSE;
    mLaunchedToTaskId = -1;
    return NOERROR;
}

/** Returns whether the search bar app widget exists. */
Boolean RecentsConfiguration::HasSearchBarAppWidget()
{
    return mSearchBarAppWidgetId >= 0;
}

/** Returns whether the status bar scrim should be animated when shown for the first time. */
Boolean RecentsConfiguration::ShouldAnimateStatusBarScrim()
{
    return mLaunchedFromHome;
}

/** Returns whether the status bar scrim should be visible. */
Boolean RecentsConfiguration::HasStatusBarScrim()
{
    return !mLaunchedWithNoRecentTasks;
}

/** Returns whether the nav bar scrim should be animated when shown for the first time. */
Boolean RecentsConfiguration::ShouldAnimateNavBarScrim()
{
    return TRUE;
}

/** Returns whether the nav bar scrim should be visible. */
Boolean RecentsConfiguration::HasNavBarScrim()
{
    // Only show the scrim if we have recent tasks, and if the nav bar is not transposed
    return !mLaunchedWithNoRecentTasks && (!mHasTransposedNavBar || !mIsLandscape);
}

/** Returns whether the current layout is horizontal. */
Boolean RecentsConfiguration::HasHorizontalLayout()
{
    return mIsLandscape && mHasTransposedSearchBar;
}

/**
 * Returns the task stack bounds in the current orientation. These bounds do not account for
 * the system insets.
 */
ECode RecentsConfiguration::GetTaskStackBounds(
    /* [in] */ Int32 windowWidth,
    /* [in] */ Int32 windowHeight,
    /* [in] */ Int32 topInset,
    /* [in] */ Int32 rightInset,
    /* [in] */ IRect* taskStackBounds)
{
    AutoPtr<IRect> searchBarBounds;
    CRect::New((IRect**)&searchBarBounds);
    GetSearchBarBounds(windowWidth, windowHeight, topInset, searchBarBounds);
    if (mIsLandscape && mHasTransposedSearchBar) {
        // In landscape, the search bar appears on the left, but we overlay it on top
        taskStackBounds->Set(0, topInset, windowWidth - rightInset, windowHeight);
    }
    else {
        // In portrait, the search bar appears on the top (which already has the inset)
        Int32 bottom;
        searchBarBounds->GetBottom(&bottom);
        taskStackBounds->Set(0, bottom, windowWidth, windowHeight);
    }
    return NOERROR;
}

/**
 * Returns the search bar bounds in the current orientation.  These bounds do not account for
 * the system insets.
 */
ECode RecentsConfiguration::GetSearchBarBounds(
    /* [in] */ Int32 windowWidth,
    /* [in] */ Int32 windowHeight,
    /* [in] */ Int32 topInset,
    /* [in] */ IRect* searchBarSpaceBounds)
{
    // Return empty rects if search is not enabled
    Int32 searchBarSize = mSearchBarSpaceHeightPx;
    if (!Constants::DebugFlags::App::EnableSearchLayout || !HasSearchBarAppWidget()) {
        searchBarSize = 0;
    }

    if (mIsLandscape && mHasTransposedSearchBar) {
        // In landscape, the search bar appears on the left
        searchBarSpaceBounds->Set(0, topInset, searchBarSize, windowHeight);
    }
    else {
        // In portrait, the search bar appears on the top
        searchBarSpaceBounds->Set(0, topInset, windowWidth, topInset + searchBarSize);
    }

    return NOERROR;
}

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
