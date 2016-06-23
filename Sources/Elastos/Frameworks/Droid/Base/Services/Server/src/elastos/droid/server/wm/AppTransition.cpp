
#include "elastos/droid/server/wm/AppTransition.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::EIID_IInterpolator;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::IScaleAnimation;
using Elastos::Droid::View::Animation::CScaleAnimation;
using Elastos::Droid::View::Animation::IAlphaAnimation;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Animation::IAnimationSet;
using Elastos::Droid::View::Animation::CAnimationSet;
using Elastos::Droid::View::Animation::ITranslateAnimation;
using Elastos::Droid::View::Animation::CTranslateAnimation;
using Elastos::Droid::View::Animation::CClipRectAnimation;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

//==============================================================================
//                  AppTransition::FadeInInterpolator
//==============================================================================

CAR_INTERFACE_IMPL_2(AppTransition::FadeInInterpolator, Object, IInterpolator, ITimeInterpolator)

ECode AppTransition::FadeInInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)
    // Linear response for first fraction, then complete after that.
    if (input < AppTransition::RECENTS_THUMBNAIL_FADEIN_FRACTION) {
        *output = 0.0;
        return NOERROR;
    }
    *output = (input - AppTransition::RECENTS_THUMBNAIL_FADEIN_FRACTION) /
            (1 - RECENTS_THUMBNAIL_FADEIN_FRACTION);
    return NOERROR;
}

ECode AppTransition::FadeInInterpolator:: HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}


//==============================================================================
//                  AppTransition::FadeOutInterpolator
//==============================================================================

CAR_INTERFACE_IMPL_2(AppTransition::FadeOutInterpolator, Object, IInterpolator, ITimeInterpolator)

ECode AppTransition::FadeOutInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output)
    // Linear response for first fraction, then complete after that.
    if (input < AppTransition::RECENTS_THUMBNAIL_FADEOUT_FRACTION) {
        *output = input / AppTransition::RECENTS_THUMBNAIL_FADEOUT_FRACTION;
        return NOERROR;
    }
    *output = 1;
    return NOERROR;
}

ECode AppTransition::FadeOutInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}



//==============================================================================
//                  AppTransition
//==============================================================================

const Int32 AppTransition::TRANSIT_UNSET;
const Int32 AppTransition::TRANSIT_NONE;
const Int32 AppTransition::TRANSIT_ACTIVITY_OPEN;
const Int32 AppTransition::TRANSIT_ACTIVITY_CLOSE;
const Int32 AppTransition::TRANSIT_TASK_OPEN;
const Int32 AppTransition::TRANSIT_TASK_CLOSE;
const Int32 AppTransition::TRANSIT_TASK_TO_FRONT;
const Int32 AppTransition::TRANSIT_TASK_TO_BACK;
const Int32 AppTransition::TRANSIT_WALLPAPER_CLOSE;
const Int32 AppTransition::TRANSIT_WALLPAPER_OPEN;
const Int32 AppTransition::TRANSIT_WALLPAPER_INTRA_OPEN;
const Int32 AppTransition::TRANSIT_WALLPAPER_INTRA_CLOSE;
const Int32 AppTransition::TRANSIT_TASK_OPEN_BEHIND;
const String AppTransition::TAG("AppTransition");
const Boolean AppTransition::DEBUG_APP_TRANSITIONS = CWindowManagerService::DEBUG_APP_TRANSITIONS;
const Boolean AppTransition::DEBUG_ANIM = CWindowManagerService::DEBUG_ANIM;
const Float AppTransition::RECENTS_THUMBNAIL_FADEIN_FRACTION;
const Float AppTransition::RECENTS_THUMBNAIL_FADEOUT_FRACTION;
const Int32 AppTransition::DEFAULT_APP_TRANSITION_DURATION;
const Int32 AppTransition::THUMBNAIL_APP_TRANSITION_DURATION;
const Int32 AppTransition::THUMBNAIL_APP_TRANSITION_ALPHA_DURATION;
const Int32 AppTransition::NEXT_TRANSIT_TYPE_NONE;
const Int32 AppTransition::NEXT_TRANSIT_TYPE_CUSTOM;
const Int32 AppTransition::NEXT_TRANSIT_TYPE_SCALE_UP;
const Int32 AppTransition::NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_UP;
const Int32 AppTransition::NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_DOWN;
const Int32 AppTransition::NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP;
const Int32 AppTransition::NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_DOWN;
const Int32 AppTransition::THUMBNAIL_TRANSITION_ENTER_SCALE_UP;
const Int32 AppTransition::THUMBNAIL_TRANSITION_EXIT_SCALE_UP;
const Int32 AppTransition::THUMBNAIL_TRANSITION_ENTER_SCALE_DOWN;
const Int32 AppTransition::THUMBNAIL_TRANSITION_EXIT_SCALE_DOWN;
const Int32 AppTransition::APP_STATE_IDLE;
const Int32 AppTransition::APP_STATE_READY;
const Int32 AppTransition::APP_STATE_RUNNING;
const Int32 AppTransition::APP_STATE_TIMEOUT;


AppTransition::AppTransition(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* h)
    : mContext(context)
    , mH(h)
    , mNextAppTransition(TRANSIT_UNSET)
    , mNextAppTransitionType(NEXT_TRANSIT_TYPE_NONE)
    , mNextAppTransitionScaleUp(FALSE)
    , mNextAppTransitionEnter(0)
    , mNextAppTransitionExit(0)
    , mNextAppTransitionStartX(0)
    , mNextAppTransitionStartY(0)
    , mNextAppTransitionStartWidth(0)
    , mNextAppTransitionStartHeight(0)
    , mAppTransitionState(APP_STATE_IDLE)
    , mConfigShortAnimTime(0)
    , mCurrentUserId(0)
{
    CRect::New((IRect**)&mNextAppTransitionInsets);
    CRect::New((IRect**)&mTmpFromClipRect);
    CRect::New((IRect**)&mTmpToClipRect);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetInteger(R::integer::config_shortAnimTime, &mConfigShortAnimTime);
    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    utils->LoadInterpolator(context, R::interpolator::decelerate_cubic, (IInterpolator**)&mDecelerateInterpolator);
    utils->LoadInterpolator(context, R::interpolator::fast_out_slow_in, (IInterpolator**)&mThumbnailFastOutSlowInInterpolator);
    mThumbnailFadeInInterpolator = new FadeInInterpolator();
    mThumbnailFadeOutInterpolator = new FadeOutInterpolator();
}

Boolean AppTransition::IsTransitionSet()
{
    return mNextAppTransition != TRANSIT_UNSET;
}

Boolean AppTransition::IsTransitionNone()
{
    return mNextAppTransition == TRANSIT_NONE;
}

Boolean AppTransition::IsTransitionEqual(
    /* [in] */ Int32 transit)
{
    return mNextAppTransition == transit;
}

Int32 AppTransition::GetAppTransition()
{
    return mNextAppTransition;
}

void AppTransition::SetAppTransition(
    /* [in] */ Int32 transit)
{
    mNextAppTransition = transit;
}

Boolean AppTransition::IsReady()
{
    return mAppTransitionState == APP_STATE_READY
            || mAppTransitionState == APP_STATE_TIMEOUT;
}

void AppTransition::SetReady()
{
    mAppTransitionState = APP_STATE_READY;
}

Boolean AppTransition::IsRunning()
{
    return mAppTransitionState == APP_STATE_RUNNING;
}

void AppTransition::SetIdle()
{
    mAppTransitionState = APP_STATE_IDLE;
}

Boolean AppTransition::IsTimeout()
{
    return mAppTransitionState == APP_STATE_TIMEOUT;
}

void AppTransition::SetTimeout()
{
    mAppTransitionState = APP_STATE_TIMEOUT;
}

AutoPtr<IBitmap> AppTransition::GetNextAppTransitionThumbnail()
{
    return mNextAppTransitionThumbnail;
}

Boolean AppTransition::IsNextThumbnailTransitionAspectScaled()
{
    return mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP ||
            mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_DOWN;
}

Boolean AppTransition::IsNextThumbnailTransitionScaleUp()
{
    return mNextAppTransitionScaleUp;
}

Int32 AppTransition::GetStartingX()
{
    return mNextAppTransitionStartX;
}

Int32 AppTransition::GetStartingY()
{
    return mNextAppTransitionStartY;
}

void AppTransition::Prepare()
{
    if (!IsRunning()) {
        mAppTransitionState = APP_STATE_IDLE;
    }
}

void AppTransition::GoodToGo()
{
    mNextAppTransition = TRANSIT_UNSET;
    mAppTransitionState = APP_STATE_RUNNING;
}

void AppTransition::Clear()
{
    mNextAppTransitionType = NEXT_TRANSIT_TYPE_NONE;
    mNextAppTransitionPackage = NULL;
    mNextAppTransitionThumbnail = NULL;
}

void AppTransition::Freeze()
{
    SetAppTransition(TRANSIT_UNSET);
    Clear();
    SetReady();
}

AutoPtr<Entry> AppTransition::GetCachedAnimations(
    /* [in] */ IWindowManagerLayoutParams* lp)
{
    if (DEBUG_ANIM) {
        String packageName(NULL);
        String animationsStr(NULL);
        Int32 animations;
        if (lp != NULL) {
            lp->GetPackageName(&packageName);
            lp->GetWindowAnimations(&animations);
            animationsStr = StringUtils::ToString(animations);
        }
        Slogger::V(TAG, "Loading animations: layout params pkg=%s resId=0x%s", packageName.string()
                , animationsStr.string());
    }
    Int32 animations;
    if (lp != NULL && (lp->GetWindowAnimations(&animations), animations != 0)) {
        // If this is a system resource, don't try to load it from the
        // application resources.  It is nice to avoid loading application
        // resources if we can.
        String packageName("android");
        String pn;
        if (lp->GetPackageName(&pn), !pn.IsNull()) {
            packageName = pn;
        }
        Int32 resId;
        lp->GetWindowAnimations(&resId);
        if ((resId & 0xFF000000) == 0x01000000) {
            packageName = "android";
        }
        if (DEBUG_ANIM) Slogger::V(TAG, "Loading animations: picked package=%s", packageName.string());
        AutoPtr<ArrayOf<Int32> > attrs = TO_ATTRS_ARRAYOF(R::styleable::WindowAnimation);
        return AttributeCache::GetInstance()->Get(packageName, resId, attrs, mCurrentUserId);
    }
    return NULL;
}

AutoPtr<Entry> AppTransition::GetCachedAnimations(
    /* [in] */ const String& _packageName,
    /* [in] */ Int32 resId)
{
    String packageName = _packageName;
    if (DEBUG_ANIM) {
        Slogger::V(TAG, "Loading animations: package=%s resId=0x%s"
                , packageName.string(), StringUtils::ToString(resId).string());
    }
    if (!packageName.IsNull()) {
        if ((resId & 0xFF000000) == 0x01000000) {
            packageName = String("android");
        }
        if (DEBUG_ANIM) Slogger::V(TAG, "Loading animations: picked package=%s", packageName.string());
        AutoPtr<ArrayOf<Int32> > attrs = TO_ATTRS_ARRAYOF(R::styleable::WindowAnimation);
        return AttributeCache::GetInstance()->Get(packageName, resId, attrs, mCurrentUserId);
    }
    return NULL;
}

AutoPtr<IAnimation> AppTransition::LoadAnimationAttr(
    /* [in] */ IWindowManagerLayoutParams* lp,
    /* [in] */ Int32 animAttr)
{
    Int32 anim = 0;
    AutoPtr<IContext> context = mContext;
    if (animAttr >= 0) {
        AutoPtr<Entry> ent = GetCachedAnimations(lp);
        if (ent != NULL) {
            context = ent->mContext;
            ent->mArray->GetResourceId(animAttr, 0, &anim);
        }
    }
    if (anim != 0) {
        AutoPtr<IAnimationUtils> utils;
        CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
        AutoPtr<IAnimation> a;
        utils->LoadAnimation(context, anim, (IAnimation**)&a);
        return a;
    }
    return NULL;
}

AutoPtr<IAnimation> AppTransition::LoadAnimationRes(
    /* [in] */ IWindowManagerLayoutParams* lp,
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context = mContext;
    if (resId >= 0) {
        AutoPtr<Entry> ent = GetCachedAnimations(lp);
        if (ent != NULL) {
            context = ent->mContext;
        }
        AutoPtr<IAnimationUtils> utils;
        CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
        AutoPtr<IAnimation> a;
        utils->LoadAnimation(context, resId, (IAnimation**)&a);
        return a;
    }
    return NULL;
}

AutoPtr<IAnimation> AppTransition::LoadAnimationRes(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resId)
{
    Int32 anim = 0;
    AutoPtr<IContext> context = mContext;
    if (resId >= 0) {
        AutoPtr<Entry> ent = GetCachedAnimations(packageName, resId);
        if (ent != NULL) {
            context = ent->mContext;
            anim = resId;
        }
    }
    if (anim != 0) {
        AutoPtr<IAnimationUtils> utils;
        CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
        AutoPtr<IAnimation> a;
        utils->LoadAnimation(context, anim, (IAnimation**)&a);
        return a;
    }
    return NULL;
}

Float AppTransition::ComputePivot(
    /* [in] */ Int32 startPos,
    /* [in] */ Float finalScale)
{
    Float denom = finalScale - 1;
    if (Elastos::Core::Math::Abs(denom) < .0001f) {
        return startPos;
    }
    return -startPos / denom;
}

AutoPtr<IAnimation> AppTransition::CreateScaleUpAnimationLocked(
    /* [in] */ Int32 transit,
    /* [in] */ Boolean enter,
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight)
{
    AutoPtr<IAnimation> a;
    if (enter) {
        // Entering app zooms out from the center of the initial rect.
        Float scaleW = mNextAppTransitionStartWidth / (Float) appWidth;
        Float scaleH = mNextAppTransitionStartHeight / (Float) appHeight;
        AutoPtr<IAnimation> scale;
        CScaleAnimation::New(scaleW, 1, scaleH, 1,
            ComputePivot(mNextAppTransitionStartX, scaleW),
            ComputePivot(mNextAppTransitionStartY, scaleH), (IAnimation**)&scale);
        scale->SetInterpolator(mDecelerateInterpolator);

        AutoPtr<IAnimation> alpha;
        CAlphaAnimation::New(0.0, 1.0, (IAnimation**)&alpha);
        alpha->SetInterpolator(mThumbnailFadeOutInterpolator);

        AutoPtr<IAnimationSet> set;
        CAnimationSet::New(FALSE, (IAnimationSet**)&set);
        set->AddAnimation(scale);
        set->AddAnimation(alpha);
        a = IAnimation::Probe(set);
        a->SetDetachWallpaper(TRUE);
    }
    else  if (transit == TRANSIT_WALLPAPER_INTRA_OPEN ||
            transit == TRANSIT_WALLPAPER_INTRA_CLOSE) {
        // If we are on top of the wallpaper, we need an animation that
        // correctly handles the wallpaper staying static behind all of
        // the animated elements.  To do this, will just have the existing
        // element fade out.
        AutoPtr<IAlphaAnimation> aa;
        CAlphaAnimation::New(1.0, 0.0, (IAlphaAnimation**)&aa);
        a = IAnimation::Probe(aa);
        a->SetDetachWallpaper(TRUE);
    }
    else {
        // For normal animations, the exiting element just holds in place.
        AutoPtr<IAlphaAnimation> aa;
        CAlphaAnimation::New(1.0, 1.0, (IAlphaAnimation**)&aa);
        a = IAnimation::Probe(aa);
    }

    // Pick the desired duration.  If this is an inter-activity transition,
    // it  is the standard duration for that.  Otherwise we use the longer
    // task transition duration.
    Int64 duration;
    switch (transit) {
        case TRANSIT_ACTIVITY_OPEN:
        case TRANSIT_ACTIVITY_CLOSE:
            duration = mConfigShortAnimTime;
            break;
        default:
            duration = DEFAULT_APP_TRANSITION_DURATION;
            break;
    }
    a->SetDuration(duration);
    a->SetFillAfter(TRUE);
    a->SetInterpolator(mDecelerateInterpolator);
    a->Initialize(appWidth, appHeight, appWidth, appHeight);
    return a;
}

ECode AppTransition::PrepareThumbnailAnimationWithDuration(
    /* [in] */ IAnimation* a,
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight,
    /* [in] */ Int32 duration,
    /* [in] */ IInterpolator* interpolator)
{
    if (duration > 0) {
        a->SetDuration(duration);
    }
    a->SetFillAfter(TRUE);
    a->SetInterpolator(interpolator);
    return a->Initialize(appWidth, appHeight, appWidth, appHeight);
}

ECode AppTransition::PrepareThumbnailAnimation(
    /* [in] */ IAnimation* a,
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight,
    /* [in] */ Int32 transit)
{
    // Pick the desired duration.  If this is an inter-activity transition,
    // it  is the standard duration for that.  Otherwise we use the longer
    // task transition duration.
    Int32 duration;
    switch (transit) {
        case TRANSIT_ACTIVITY_OPEN:
        case TRANSIT_ACTIVITY_CLOSE:
            duration = mConfigShortAnimTime;
            break;
        default:
            duration = DEFAULT_APP_TRANSITION_DURATION;
            break;
    }
    return PrepareThumbnailAnimationWithDuration(a, appWidth, appHeight, duration,
            mDecelerateInterpolator);
}

Int32 AppTransition::GetThumbnailTransitionState(
    /* [in] */ Boolean enter)
{
    if (enter) {
        if (mNextAppTransitionScaleUp) {
            return THUMBNAIL_TRANSITION_ENTER_SCALE_UP;
        }
        else {
            return THUMBNAIL_TRANSITION_ENTER_SCALE_DOWN;
        }
    }
    else {
        if (mNextAppTransitionScaleUp) {
            return THUMBNAIL_TRANSITION_EXIT_SCALE_UP;
        }
        else {
            return THUMBNAIL_TRANSITION_EXIT_SCALE_DOWN;
        }
    }
}

AutoPtr<IAnimation> AppTransition::CreateThumbnailAspectScaleAnimationLocked(
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight,
    /* [in] */ Int32 deviceWidth,
    /* [in] */ Int32 transit)
{
    AutoPtr<IAnimation> a;
    Int32 thumbWidthI;
    mNextAppTransitionThumbnail->GetWidth(&thumbWidthI);
    Float thumbWidth = thumbWidthI > 0 ? thumbWidthI : 1;
    Int32 thumbHeightI;
    mNextAppTransitionThumbnail->GetHeight(&thumbHeightI);
    Float thumbHeight = thumbHeightI > 0 ? thumbHeightI : 1;

    Float scaleW = deviceWidth / thumbWidth;
    Float unscaledHeight = thumbHeight * scaleW;
    Float unscaledStartY = mNextAppTransitionStartY - (unscaledHeight - thumbHeight) / 2;
    if (mNextAppTransitionScaleUp) {
        // Animation up from the thumbnail to the full screen
        AutoPtr<IAnimation> scale;
        CScaleAnimation::New(1, scaleW, 1, scaleW,
            mNextAppTransitionStartX + (thumbWidth / 2),
            mNextAppTransitionStartY + (thumbHeight / 2), (IAnimation**)&scale);
        scale->SetInterpolator(mThumbnailFastOutSlowInInterpolator);
        scale->SetDuration(THUMBNAIL_APP_TRANSITION_DURATION);

        AutoPtr<IAnimation> alpha;
        CAlphaAnimation::New(1.0, 0.0, (IAnimation**)&alpha);
        alpha->SetInterpolator(mThumbnailFadeOutInterpolator);
        alpha->SetDuration(THUMBNAIL_APP_TRANSITION_ALPHA_DURATION);

        Int32 top;
        mNextAppTransitionInsets->GetTop(&top);
        AutoPtr<IAnimation> translate;
        CTranslateAnimation::New(0, 0, 0, -unscaledStartY + top, (IAnimation**)&translate);
        translate->SetInterpolator(mThumbnailFastOutSlowInInterpolator);
        translate->SetDuration(THUMBNAIL_APP_TRANSITION_DURATION);

        // This AnimationSet uses the Interpolators assigned above.
        AutoPtr<IAnimationSet> set;
        CAnimationSet::New(FALSE, (IAnimationSet**)&set);
        set->AddAnimation(scale);
        set->AddAnimation(alpha);
        set->AddAnimation(translate);
        a = IAnimation::Probe(set);
    }
    else {
        // Animation down from the full screen to the thumbnail
        AutoPtr<IAnimation> scale;
        CScaleAnimation::New(scaleW, 1, scaleW, 1,
            mNextAppTransitionStartX + (thumbWidth / 2),
            mNextAppTransitionStartY + (thumbHeight / 2), (IAnimation**)&scale);
        scale->SetInterpolator(mThumbnailFastOutSlowInInterpolator);
        scale->SetDuration(THUMBNAIL_APP_TRANSITION_DURATION);

        AutoPtr<IAnimation> alpha;
        CAlphaAnimation::New(0.0, 1.0, (IAnimation**)&alpha);
        alpha->SetInterpolator(mThumbnailFadeOutInterpolator);
        alpha->SetDuration(THUMBNAIL_APP_TRANSITION_ALPHA_DURATION);

        Int32 top;
        mNextAppTransitionInsets->GetTop(&top);
        AutoPtr<IAnimation> translate;
        CTranslateAnimation::New(0, 0, -unscaledStartY + top, 0, (IAnimation**)&translate);
        translate->SetInterpolator(mThumbnailFastOutSlowInInterpolator);
        translate->SetDuration(THUMBNAIL_APP_TRANSITION_DURATION);

        // This AnimationSet uses the Interpolators assigned above.
        AutoPtr<IAnimationSet> set;
        CAnimationSet::New(FALSE, (IAnimationSet**)&set);
        set->AddAnimation(scale);
        set->AddAnimation(alpha);
        set->AddAnimation(translate);
        a = IAnimation::Probe(set);
    }
    PrepareThumbnailAnimationWithDuration(a, appWidth, appHeight, 0,
        mThumbnailFastOutSlowInInterpolator);
    return a;
}

AutoPtr<IAnimation> AppTransition::CreateAspectScaledThumbnailEnterExitAnimationLocked(
    /* [in] */ Int32 thumbTransitState,
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight,
    /* [in] */ Int32 orientation,
    /* [in] */ Int32 transit,
    /* [in] */ IRect* containingFrame,
    /* [in] */ IRect* contentInsets,
    /* [in] */ Boolean isFullScreen)
{
    AutoPtr<IAnimation> a;
    Int32 thumbWidthI = mNextAppTransitionStartWidth;
    Float thumbWidth = thumbWidthI > 0 ? thumbWidthI : 1;
    Int32 thumbHeightI = mNextAppTransitionStartHeight;
    Float thumbHeight = thumbHeightI > 0 ? thumbHeightI : 1;

    // Used for the ENTER_SCALE_UP and EXIT_SCALE_DOWN transitions
    Float scale = 1;
    Int32 scaledTopDecor = 0;

    switch (thumbTransitState) {
        case THUMBNAIL_TRANSITION_ENTER_SCALE_UP: {
            // App window scaling up to become full screen
            if (orientation == IConfiguration::ORIENTATION_PORTRAIT) {
                // In portrait, we scale the width and clip to the top/left square
                scale = thumbWidth / appWidth;
                Int32 top;
                contentInsets->GetTop(&top);
                scaledTopDecor = (Int32)(scale * top);
                Int32 unscaledThumbHeight = (Int32)(thumbHeight / scale);
                mTmpFromClipRect->Set(containingFrame);
                if (isFullScreen) {
                    mTmpFromClipRect->SetTop(top);
                }
                Int32 clipRectTop;
                mTmpFromClipRect->GetTop(&clipRectTop);
                mTmpFromClipRect->SetBottom(clipRectTop + unscaledThumbHeight);
                mTmpToClipRect->Set(containingFrame);
            }
            else {
                // In landscape, we scale the height and clip to the top/left square
                Int32 top;
                contentInsets->GetTop(&top);
                scale = thumbHeight / (appHeight - top);
                scaledTopDecor = (Int32)(scale * top);
                Int32 unscaledThumbWidth = (Int32)(thumbWidth / scale);
                Int32 unscaledThumbHeight = (Int32)(thumbHeight / scale);
                mTmpFromClipRect->Set(containingFrame);
                if (isFullScreen) {
                    mTmpFromClipRect->SetTop(top);
                    Int32 clipRectTop;
                    mTmpFromClipRect->GetTop(&clipRectTop);
                    mTmpFromClipRect->SetBottom(clipRectTop + unscaledThumbHeight);
                }
                Int32 clipRectLeft;
                mTmpFromClipRect->GetLeft(&clipRectLeft);
                mTmpFromClipRect->SetRight(clipRectLeft + unscaledThumbWidth);
                mTmpToClipRect->Set(containingFrame);
            }
            mNextAppTransitionInsets->Set(contentInsets);

            AutoPtr<IAnimation> scaleAnim, clipAnim, translateAnim;
            CScaleAnimation::New(scale, 1, scale, 1,
                ComputePivot(mNextAppTransitionStartX, scale),
                ComputePivot(mNextAppTransitionStartY, scale), (IAnimation**)&scaleAnim);
            CClipRectAnimation::New(mTmpFromClipRect, mTmpToClipRect, (IAnimation**)&clipAnim);
            CTranslateAnimation::New(0, 0, -scaledTopDecor, 0, (IAnimation**)&translateAnim);

            AutoPtr<IAnimationSet> set;
            CAnimationSet::New(TRUE, (IAnimationSet**)&set);
            set->AddAnimation(clipAnim);
            set->AddAnimation(scaleAnim);
            set->AddAnimation(translateAnim);
            a = IAnimation::Probe(set);
            break;
        }
        case THUMBNAIL_TRANSITION_EXIT_SCALE_UP: {
            // Previous app window during the scale up
            if (transit == TRANSIT_WALLPAPER_INTRA_OPEN) {
                // Fade out the source activity if we are animating to a wallpaper
                // activity.
                CAlphaAnimation::New(1.0, 0.0, (IAnimation**)&a);
            }
            else {
                CAlphaAnimation::New(1.0, 1.0, (IAnimation**)&a);
            }
            break;
        }
        case THUMBNAIL_TRANSITION_ENTER_SCALE_DOWN: {
            // Target app window during the scale down
            if (transit == TRANSIT_WALLPAPER_INTRA_OPEN) {
                // Fade in the destination activity if we are animating from a wallpaper
                // activity.
                CAlphaAnimation::New(0.0, 1.0, (IAnimation**)&a);
            }
            else {
                CAlphaAnimation::New(1.0, 1.0, (IAnimation**)&a);
            }
            break;
        }
        case THUMBNAIL_TRANSITION_EXIT_SCALE_DOWN: {
            // App window scaling down from full screen
            if (orientation == IConfiguration::ORIENTATION_PORTRAIT) {
                // In portrait, we scale the width and clip to the top/left square
                scale = thumbWidth / appWidth;
                Int32 top;
                contentInsets->GetTop(&top);
                scaledTopDecor = (Int32)(scale * top);
                Int32 unscaledThumbHeight = (Int32)(thumbHeight / scale);
                mTmpFromClipRect->Set(containingFrame);
                mTmpToClipRect->Set(containingFrame);
                if (isFullScreen) {
                    mTmpToClipRect->SetTop(top);
                }
                Int32 clipRectTop;
                mTmpToClipRect->GetTop(&clipRectTop);
                mTmpToClipRect->SetBottom(clipRectTop + unscaledThumbHeight);
            }
            else {
                // In landscape, we scale the height and clip to the top/left square
                Int32 top;
                contentInsets->GetTop(&top);
                scale = thumbHeight / (appHeight - top);
                scaledTopDecor = (Int32)(scale * top);
                Int32 unscaledThumbWidth = (Int32)(thumbWidth / scale);
                Int32 unscaledThumbHeight = (Int32)(thumbHeight / scale);
                mTmpFromClipRect->Set(containingFrame);
                mTmpToClipRect->Set(containingFrame);
                if (isFullScreen) {
                    mTmpToClipRect->SetTop(top);
                    Int32 clipRectTop;
                    mTmpToClipRect->GetTop(&clipRectTop);
                    mTmpToClipRect->SetBottom(clipRectTop + unscaledThumbHeight);
                }
                Int32 clipRectLeft;
                mTmpToClipRect->GetLeft(&clipRectLeft);
                mTmpToClipRect->SetRight(clipRectLeft + unscaledThumbWidth);
            }
            mNextAppTransitionInsets->Set(contentInsets);

            AutoPtr<IAnimation> scaleAnim;
            CScaleAnimation::New(1, scale, 1, scale,
                ComputePivot(mNextAppTransitionStartX, scale),
                ComputePivot(mNextAppTransitionStartY, scale), (IAnimation**)&scaleAnim);
            AutoPtr<IAnimation> clipAnim;
            CClipRectAnimation::New(mTmpFromClipRect, mTmpToClipRect, (IAnimation**)&clipAnim);
            AutoPtr<IAnimation> translateAnim;
            CTranslateAnimation::New(0, 0, 0, -scaledTopDecor, (IAnimation**)&translateAnim);

            AutoPtr<IAnimationSet> set;
            CAnimationSet::New(TRUE, (IAnimationSet**)&set);
            set->AddAnimation(clipAnim);
            set->AddAnimation(scaleAnim);
            set->AddAnimation(translateAnim);

            a = IAnimation::Probe(set);
            a->SetZAdjustment(IAnimation::ZORDER_TOP);
            break;
        }
        default: {
            Slogger::E(TAG, "Invalid thumbnail transition state");
            assert(0);
            // throw new RuntimeException("Invalid thumbnail transition state");
        }
    }

    PrepareThumbnailAnimationWithDuration(a, appWidth, appHeight,
        THUMBNAIL_APP_TRANSITION_DURATION, mThumbnailFastOutSlowInInterpolator);
    return a;
}

AutoPtr<IAnimation> AppTransition::CreateThumbnailScaleAnimationLocked(
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight,
    /* [in] */ Int32 transit)
{
    AutoPtr<IAnimation> a;
    Int32 thumbWidthI;
    mNextAppTransitionThumbnail->GetWidth(&thumbWidthI);
    Float thumbWidth = thumbWidthI > 0 ? thumbWidthI : 1;
    Int32 thumbHeightI;
    mNextAppTransitionThumbnail->GetHeight(&thumbHeightI);
    Float thumbHeight = thumbHeightI > 0 ? thumbHeightI : 1;

    if (mNextAppTransitionScaleUp) {
        // Animation for the thumbnail zooming from its initial size to the full screen
        Float scaleW = appWidth / thumbWidth;
        Float scaleH = appHeight / thumbHeight;
        AutoPtr<IAnimation> scale;
        CScaleAnimation::New(1, scaleW, 1, scaleH,
            ComputePivot(mNextAppTransitionStartX, 1 / scaleW),
            ComputePivot(mNextAppTransitionStartY, 1 / scaleH), (IAnimation**)&scale);
        scale->SetInterpolator(mDecelerateInterpolator);

        AutoPtr<IAnimation> alpha;
        CAlphaAnimation::New(1.0, 0.0, (IAnimation**)&alpha);
        alpha->SetInterpolator(mThumbnailFadeOutInterpolator);

        // This AnimationSet uses the Interpolators assigned above.
        AutoPtr<IAnimationSet> set;
        CAnimationSet::New(FALSE, (IAnimationSet**)&set);
        set->AddAnimation(scale);
        set->AddAnimation(alpha);
        a = IAnimation::Probe(set);
    }
    else {
        // Animation for the thumbnail zooming down from the full screen to its final size
        Float scaleW = appWidth / thumbWidth;
        Float scaleH = appHeight / thumbHeight;
        CScaleAnimation::New(scaleW, 1, scaleH, 1,
            ComputePivot(mNextAppTransitionStartX, 1 / scaleW),
            ComputePivot(mNextAppTransitionStartY, 1 / scaleH), (IAnimation**)&a);
    }

    PrepareThumbnailAnimation(a, appWidth, appHeight, transit);
    return a;
}

AutoPtr<IAnimation> AppTransition::CreateThumbnailEnterExitAnimationLocked(
    /* [in] */ Int32 thumbTransitState,
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight,
    /* [in] */ Int32 transit)
{
    AutoPtr<IAnimation> a;
    Int32 thumbWidthI;
    mNextAppTransitionThumbnail->GetWidth(&thumbWidthI);
    Float thumbWidth = thumbWidthI > 0 ? thumbWidthI : 1;
    Int32 thumbHeightI;
    mNextAppTransitionThumbnail->GetHeight(&thumbHeightI);
    Float thumbHeight = thumbHeightI > 0 ? thumbHeightI : 1;

    switch (thumbTransitState) {
        case THUMBNAIL_TRANSITION_ENTER_SCALE_UP: {
            // Entering app scales up with the thumbnail
            Float scaleW = thumbWidth / appWidth;
            Float scaleH = thumbHeight / appHeight;
            CScaleAnimation::New(scaleW, 1, scaleH, 1,
                ComputePivot(mNextAppTransitionStartX, scaleW),
                ComputePivot(mNextAppTransitionStartY, scaleH), (IAnimation**)&a);
            break;
        }
        case THUMBNAIL_TRANSITION_EXIT_SCALE_UP: {
            // Exiting app while the thumbnail is scaling up should fade or stay in place
            if (transit == TRANSIT_WALLPAPER_INTRA_OPEN) {
                // Fade out while bringing up selected activity. This keeps the
                // current activity from showing through a launching wallpaper
                // activity.
                CAlphaAnimation::New(1.0, 0.0, (IAnimation**)&a);
            }
            else {
                // noop animation
                CAlphaAnimation::New(1.0, 1.0, (IAnimation**)&a);
            }
            break;
        }
        case THUMBNAIL_TRANSITION_ENTER_SCALE_DOWN: {
            // Entering the other app, it should just be visible while we scale the thumbnail
            // down above it
            CAlphaAnimation::New(1.0, 1.0, (IAnimation**)&a);
            break;
        }
        case THUMBNAIL_TRANSITION_EXIT_SCALE_DOWN: {
            // Exiting the current app, the app should scale down with the thumbnail
            Float scaleW = thumbWidth / appWidth;
            Float scaleH = thumbHeight / appHeight;
            AutoPtr<IAnimation> scale;
            CScaleAnimation::New(1, scaleW, 1, scaleH,
                ComputePivot(mNextAppTransitionStartX, scaleW),
                ComputePivot(mNextAppTransitionStartY, scaleH), (IAnimation**)&scale);
            AutoPtr<IAnimation> alpha;
            CAlphaAnimation::New(1.0, 0.0, (IAnimation**)&alpha);

            AutoPtr<IAnimationSet> set;
            CAnimationSet::New(TRUE, (IAnimationSet**)&set);
            set->AddAnimation(scale);
            set->AddAnimation(alpha);
            a = IAnimation::Probe(set);
            a->SetZAdjustment(IAnimation::ZORDER_TOP);
            break;
        }
        default: {
            Slogger::E(TAG, "Invalid thumbnail transition state");
            assert(0);
            // throw new RuntimeException("Invalid thumbnail transition state");
        }
    }

    PrepareThumbnailAnimation(a, appWidth, appHeight, transit);
    return a;
}

AutoPtr<IAnimation> AppTransition::LoadAnimation(
    /* [in] */ IWindowManagerLayoutParams* lp,
    /* [in] */ Int32 transit,
    /* [in] */ Boolean enter,
    /* [in] */ Int32 appWidth,
    /* [in] */ Int32 appHeight,
    /* [in] */ Int32 orientation,
    /* [in] */ IRect* containingFrame,
    /* [in] */ IRect* contentInsets,
    /* [in] */ Boolean isFullScreen,
    /* [in] */ Boolean isVoiceInteraction)
{
    AutoPtr<IAnimation> a;
    if (isVoiceInteraction && (transit == TRANSIT_ACTIVITY_OPEN
            || transit == TRANSIT_TASK_OPEN
            || transit == TRANSIT_TASK_TO_FRONT)) {
        a = LoadAnimationRes(lp, enter
                ? R::anim::voice_activity_open_enter : R::anim::voice_activity_open_exit);
        if (DEBUG_APP_TRANSITIONS || DEBUG_ANIM) {
            Slogger::V(TAG, "applyAnimation voice: anim=%p transit=%d isEntrance=%d", a.Get(), transit, enter
                /*+ " Callers=" + Debug.getCallers(3)*/);
        }
    }
    else if (isVoiceInteraction && (transit == TRANSIT_ACTIVITY_CLOSE
            || transit == TRANSIT_TASK_CLOSE
            || transit == TRANSIT_TASK_TO_BACK)) {
        a = LoadAnimationRes(lp, enter
                ? R::anim::voice_activity_close_enter : R::anim::voice_activity_close_exit);
        if (DEBUG_APP_TRANSITIONS || DEBUG_ANIM) {
            Slogger::V(TAG, "applyAnimation voice: anim=%p transit=%d isEntrance=%d", a.Get(), transit, enter
                /*+ " Callers=" + Debug.getCallers(3)*/);
        }
    }
    else if (mNextAppTransitionType == NEXT_TRANSIT_TYPE_CUSTOM) {
        a = LoadAnimationRes(mNextAppTransitionPackage, enter ? mNextAppTransitionEnter : mNextAppTransitionExit);
        if (DEBUG_APP_TRANSITIONS || DEBUG_ANIM) {
            Slogger::V(TAG, "applyAnimation: anim=%p nextAppTransition=ANIM_CUSTOM transit=%d isEntrance=%d"
                    , a.Get(), transit, enter/*+ " Callers=" + Debug.getCallers(3)*/);
        }
    }
    else if (mNextAppTransitionType == NEXT_TRANSIT_TYPE_SCALE_UP) {
        a = CreateScaleUpAnimationLocked(transit, enter, appWidth, appHeight);
        if (DEBUG_APP_TRANSITIONS || DEBUG_ANIM) {
            Slogger::V(TAG, "applyAnimation: anim=%p nextAppTransition=ANIM_SCALE_UP transit=%d isEntrance=%d"
                    , a.Get(), transit, enter/*+ " Callers=" + Debug.getCallers(3)*/);
        }
    }
    else if (mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_UP ||
            mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_DOWN) {
        mNextAppTransitionScaleUp =
                (mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_UP);
        a = CreateThumbnailEnterExitAnimationLocked(GetThumbnailTransitionState(enter),
                appWidth, appHeight, transit);
        if (DEBUG_APP_TRANSITIONS || DEBUG_ANIM) {
            String animName = mNextAppTransitionScaleUp ?
                    String("ANIM_THUMBNAIL_SCALE_UP") : String("ANIM_THUMBNAIL_SCALE_DOWN");
            Slogger::V(TAG, "applyAnimation: anim=%p nextAppTransition=%s transit=%d isEntrance=%d"
                    , a.Get(), animName.string(), transit, enter/*+ " Callers=" + Debug.getCallers(3)*/);
        }
    }
    else if (mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP ||
            mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_DOWN) {
        mNextAppTransitionScaleUp =
                (mNextAppTransitionType == NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP);
        a = CreateAspectScaledThumbnailEnterExitAnimationLocked(
                GetThumbnailTransitionState(enter), appWidth, appHeight, orientation,
                transit, containingFrame, contentInsets, isFullScreen);
        if (DEBUG_APP_TRANSITIONS || DEBUG_ANIM) {
            String animName = mNextAppTransitionScaleUp ?
                    String("ANIM_THUMBNAIL_ASPECT_SCALE_UP") : String("ANIM_THUMBNAIL_ASPECT_SCALE_DOWN");
            Slogger::V(TAG, "applyAnimation: anim=%p nextAppTransition=%s transit=%d isEntrance=%d"
                    , a.Get(), animName.string(), transit, enter/*+ " Callers=" + Debug.getCallers(3)*/);
        }
    }
    else {
        Int32 animAttr = 0;
        switch (transit) {
            case TRANSIT_ACTIVITY_OPEN:
                animAttr = enter
                        ? R::styleable::WindowAnimation_activityOpenEnterAnimation
                        : R::styleable::WindowAnimation_activityOpenExitAnimation;
                break;
            case TRANSIT_ACTIVITY_CLOSE:
                animAttr = enter
                        ? R::styleable::WindowAnimation_activityCloseEnterAnimation
                        : R::styleable::WindowAnimation_activityCloseExitAnimation;
                break;
            case TRANSIT_TASK_OPEN:
                animAttr = enter
                        ? R::styleable::WindowAnimation_taskOpenEnterAnimation
                        : R::styleable::WindowAnimation_taskOpenExitAnimation;
                break;
            case TRANSIT_TASK_CLOSE:
                animAttr = enter
                        ? R::styleable::WindowAnimation_taskCloseEnterAnimation
                        : R::styleable::WindowAnimation_taskCloseExitAnimation;
                break;
            case TRANSIT_TASK_TO_FRONT:
                animAttr = enter
                        ? R::styleable::WindowAnimation_taskToFrontEnterAnimation
                        : R::styleable::WindowAnimation_taskToFrontExitAnimation;
                break;
            case TRANSIT_TASK_TO_BACK:
                animAttr = enter
                        ? R::styleable::WindowAnimation_taskToBackEnterAnimation
                        : R::styleable::WindowAnimation_taskToBackExitAnimation;
                break;
            case TRANSIT_WALLPAPER_OPEN:
                animAttr = enter
                        ? R::styleable::WindowAnimation_wallpaperOpenEnterAnimation
                        : R::styleable::WindowAnimation_wallpaperOpenExitAnimation;
                break;
            case TRANSIT_WALLPAPER_CLOSE:
                animAttr = enter
                        ? R::styleable::WindowAnimation_wallpaperCloseEnterAnimation
                        : R::styleable::WindowAnimation_wallpaperCloseExitAnimation;
                break;
            case TRANSIT_WALLPAPER_INTRA_OPEN:
                animAttr = enter
                        ? R::styleable::WindowAnimation_wallpaperIntraOpenEnterAnimation
                        : R::styleable::WindowAnimation_wallpaperIntraOpenExitAnimation;
                break;
            case TRANSIT_WALLPAPER_INTRA_CLOSE:
                animAttr = enter
                        ? R::styleable::WindowAnimation_wallpaperIntraCloseEnterAnimation
                        : R::styleable::WindowAnimation_wallpaperIntraCloseExitAnimation;
                break;
            case TRANSIT_TASK_OPEN_BEHIND:
                animAttr = enter
                        ? R::styleable::WindowAnimation_launchTaskBehindSourceAnimation
                        : R::styleable::WindowAnimation_launchTaskBehindTargetAnimation;
        }
        a = animAttr != 0 ? LoadAnimationAttr(lp, animAttr) : NULL;
        if (DEBUG_APP_TRANSITIONS || DEBUG_ANIM) {
            Slogger::V(TAG, "applyAnimation: anim=%s  animAttr=0x%08x transit=%d isEntrance=%d",
                TO_CSTR(a), animAttr, transit, enter/*+ " Callers=" + Debug.getCallers(3)*/);
        }
    }
    return a;
}

void AppTransition::PostAnimationCallback()
{
    if (mNextAppTransitionCallback != NULL) {
        AutoPtr<IMessage> msg;
        mH->ObtainMessage(CWindowManagerService::H::DO_ANIMATION_CALLBACK, mNextAppTransitionCallback, (IMessage**)&msg);
        Boolean result;
        mH->SendMessage(msg, &result);
        mNextAppTransitionCallback = NULL;
    }
}

void AppTransition::OverridePendingAppTransition(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 enterAnim,
    /* [in] */ Int32 exitAnim,
    /* [in] */ IIRemoteCallback* startedCallback)
{
    if (IsTransitionSet()) {
        mNextAppTransitionType = NEXT_TRANSIT_TYPE_CUSTOM;
        mNextAppTransitionPackage = packageName;
        mNextAppTransitionThumbnail = NULL;
        mNextAppTransitionEnter = enterAnim;
        mNextAppTransitionExit = exitAnim;
        PostAnimationCallback();
        mNextAppTransitionCallback = startedCallback;
    }
    else {
        PostAnimationCallback();
    }
}

void AppTransition::OverridePendingAppTransitionScaleUp(
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 startWidth,
    /* [in] */ Int32 startHeight)
{
    if (IsTransitionSet()) {
        mNextAppTransitionType = NEXT_TRANSIT_TYPE_SCALE_UP;
        mNextAppTransitionPackage = NULL;
        mNextAppTransitionThumbnail = NULL;
        mNextAppTransitionStartX = startX;
        mNextAppTransitionStartY = startY;
        mNextAppTransitionStartWidth = startWidth;
        mNextAppTransitionStartHeight = startHeight;
        PostAnimationCallback();
        mNextAppTransitionCallback = NULL;
    }
}

void AppTransition::OverridePendingAppTransitionThumb(
    /* [in] */ IBitmap* srcThumb,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ IIRemoteCallback* startedCallback,
    /* [in] */ Boolean scaleUp)
{
    if (IsTransitionSet()) {
        mNextAppTransitionType = scaleUp ? NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_UP
                : NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_DOWN;
        mNextAppTransitionPackage = NULL;
        mNextAppTransitionThumbnail = srcThumb;
        mNextAppTransitionScaleUp = scaleUp;
        mNextAppTransitionStartX = startX;
        mNextAppTransitionStartY = startY;
        PostAnimationCallback();
        mNextAppTransitionCallback = startedCallback;
    }
    else {
        PostAnimationCallback();
    }
}

void AppTransition::OverridePendingAppTransitionAspectScaledThumb(
    /* [in] */ IBitmap* srcThumb,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ IIRemoteCallback* startedCallback,
    /* [in] */ Boolean scaleUp)
{
    if (IsTransitionSet()) {
        mNextAppTransitionType = scaleUp ? NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP
                : NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_DOWN;
        mNextAppTransitionPackage = NULL;
        mNextAppTransitionThumbnail = srcThumb;
        mNextAppTransitionScaleUp = scaleUp;
        mNextAppTransitionStartX = startX;
        mNextAppTransitionStartY = startY;
        mNextAppTransitionStartWidth = targetWidth;
        mNextAppTransitionStartHeight = targetHeight;
        PostAnimationCallback();
        mNextAppTransitionCallback = startedCallback;
    }
    else {
        PostAnimationCallback();
    }
}

ECode AppTransition::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("mNextAppTransition=0x") + StringUtils::ToHexString(mNextAppTransition);
    return NOERROR;
}

String AppTransition::AppTransitionToString(
    /* [in] */ Int32 transition)
{
    switch (transition) {
        case TRANSIT_UNSET: {
            return String("TRANSIT_UNSET");
        }
        case TRANSIT_NONE: {
            return String("TRANSIT_NONE");
        }
        case TRANSIT_ACTIVITY_OPEN: {
            return String("TRANSIT_ACTIVITY_OPEN");
        }
        case TRANSIT_ACTIVITY_CLOSE: {
            return String("TRANSIT_ACTIVITY_CLOSE");
        }
        case TRANSIT_TASK_OPEN: {
            return String("TRANSIT_TASK_OPEN");
        }
        case TRANSIT_TASK_CLOSE: {
            return String("TRANSIT_TASK_CLOSE");
        }
        case TRANSIT_TASK_TO_FRONT: {
            return String("TRANSIT_TASK_TO_FRONT");
        }
        case TRANSIT_TASK_TO_BACK: {
            return String("TRANSIT_TASK_TO_BACK");
        }
        case TRANSIT_WALLPAPER_CLOSE: {
            return String("TRANSIT_WALLPAPER_CLOSE");
        }
        case TRANSIT_WALLPAPER_OPEN: {
            return String("TRANSIT_WALLPAPER_OPEN");
        }
        case TRANSIT_WALLPAPER_INTRA_OPEN: {
            return String("TRANSIT_WALLPAPER_INTRA_OPEN");
        }
        case TRANSIT_WALLPAPER_INTRA_CLOSE: {
            return String("TRANSIT_WALLPAPER_INTRA_CLOSE");
        }
        case TRANSIT_TASK_OPEN_BEHIND: {
            return String("TRANSIT_TASK_OPEN_BEHIND");
        }
        default: {
            return String("<UNKNOWN>");
        }
    }
}

String AppTransition::AppStateToString()
{
    switch (mAppTransitionState) {
        case APP_STATE_IDLE:
            return String("APP_STATE_IDLE");
        case APP_STATE_READY:
            return String("APP_STATE_READY");
        case APP_STATE_RUNNING:
            return String("APP_STATE_RUNNING");
        case APP_STATE_TIMEOUT:
            return String("APP_STATE_TIMEOUT");
        default:
            return String("unknown state=") + StringUtils::ToString(mAppTransitionState);
    }
}

String AppTransition::TransitTypeToString()
{
    switch (mNextAppTransitionType) {
        case NEXT_TRANSIT_TYPE_NONE:
            return String("NEXT_TRANSIT_TYPE_NONE");
        case NEXT_TRANSIT_TYPE_CUSTOM:
            return String("NEXT_TRANSIT_TYPE_CUSTOM");
        case NEXT_TRANSIT_TYPE_SCALE_UP:
            return String("NEXT_TRANSIT_TYPE_SCALE_UP");
        case NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_UP:
            return String("NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_UP");
        case NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_DOWN:
            return String("NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_DOWN");
        case NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP:
            return String("NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP");
        case NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_DOWN:
            return String("NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_DOWN");
        default:
            return String("unknown type=") + StringUtils::ToString(mNextAppTransitionType);
    }
}

void AppTransition::SetCurrentUser(
    /* [in] */ Int32 newUserId)
{
    mCurrentUserId = newUserId;
}

} // Wm
} // Server
} // Droid
} // Elastos
