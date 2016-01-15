
#ifndef __ELASTOS_DROID_SERVER_WM_AppTransition_H__
#define __ELASTOS_DROID_SERVER_WM_AppTransition_H__

#include "AttributeCache.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::ITimeInterpolator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class AppTransition : public Object /* implements Dump */
{
private:
    class FadeInInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);
    };

    class FadeOutInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* output);
    };

public:
    AppTransition(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* h);

    CARAPI_(Boolean) IsTransitionSet();

    CARAPI_(Boolean) IsTransitionNone();

    CARAPI_(Boolean) IsTransitionEqual(
        /* [in] */ Int32 transit);

    CARAPI_(Int32) GetAppTransition();

    CARAPI_(void) SetAppTransition(
        /* [in] */ Int32 transit);

    CARAPI_(Boolean) IsReady();

    CARAPI_(void) SetReady();

    CARAPI_(Boolean) IsRunning();

    CARAPI_(void) SetIdle();

    CARAPI_(Boolean) IsTimeout();

    CARAPI_(void) SetTimeout();

    CARAPI_(AutoPtr<IBitmap>) GetNextAppTransitionThumbnail();

    /** Returns whether the next thumbnail transition is aspect scaled up. */
    CARAPI_(Boolean) IsNextThumbnailTransitionAspectScaled();

    /** Returns whether the next thumbnail transition is scaling up. */
    CARAPI_(Boolean) IsNextThumbnailTransitionScaleUp();

    CARAPI_(Int32) GetStartingX();

    CARAPI_(Int32) GetStartingY();

    CARAPI_(void) Prepare();

    CARAPI_(void) GoodToGo();

    CARAPI_(void) Clear();

    CARAPI_(void) Freeze();

    CARAPI_(AutoPtr<IAnimation>) LoadAnimationAttr(
        /* [in] */ IWindowManagerLayoutParams* lp,
        /* [in] */ Int32 animAttr);

    CARAPI_(AutoPtr<IAnimation>) LoadAnimationRes(
        /* [in] */ IWindowManagerLayoutParams* lp,
        /* [in] */ Int32 resId);

    /**
     * Prepares the specified animation with a standard duration, interpolator, etc.
     */
    CARAPI_(AutoPtr<IAnimation>) PrepareThumbnailAnimationWithDuration(
        /* [in] */ IAnimation* a,
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight,
        /* [in] */ Int32 duration,
        /* [in] */ IInterpolator* interpolator);

    /**
     * Prepares the specified animation with a standard duration, interpolator, etc.
     */
    CARAPI_(AutoPtr<IAnimation>) PrepareThumbnailAnimation(
        /* [in] */ IAnimation* a,
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight,
        /* [in] */ Int32 transit);

    /**
     * Return the current thumbnail transition state.
     */
    CARAPI_(Int32) GetThumbnailTransitionState(
        /* [in] */ Boolean enter);

    /**
     * This animation runs for the thumbnail that gets cross faded with the enter/exit activity
     * when a thumbnail is specified with the activity options.
     */
    CARAPI_(AutoPtr<IAnimation>) CreateThumbnailAspectScaleAnimationLocked(
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight,
        /* [in] */ Int32 deviceWidth,
        /* [in] */ Int32 transit);

    /**
     * This alternate animation is created when we are doing a thumbnail transition, for the
     * activity that is leaving, and the activity that is entering.
     */
    CARAPI_(AutoPtr<IAnimation>) CreateAspectScaledThumbnailEnterExitAnimationLocked(
        /* [in] */ Int32 thumbTransitState,
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight,
        /* [in] */ Int32 orientation,
        /* [in] */ Int32 transit,
        /* [in] */ IRect* containingFrame,
        /* [in] */ IRect* contentInsets,
        /* [in] */ Boolean isFullScreen);

    /**
     * This animation runs for the thumbnail that gets cross faded with the enter/exit activity
     * when a thumbnail is specified with the activity options.
     */
    CARAPI_(AutoPtr<IAnimation>) CreateThumbnailScaleAnimationLocked(
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight,
        /* [in] */ Int32 transit);

    /**
     * This animation is created when we are doing a thumbnail transition, for the activity that is
     * leaving, and the activity that is entering.
     */
    CARAPI_(AutoPtr<IAnimation>) CreateThumbnailEnterExitAnimationLocked(
        /* [in] */ Int32 thumbTransitState,
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight,
        /* [in] */ Int32 transit);

    CARAPI_(AutoPtr<IAnimation>) LoadAnimation(
        /* [in] */ IWindowManagerLayoutParams* lp,
        /* [in] */ Int32 transit,
        /* [in] */ Boolean enter,
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight,
        /* [in] */ Int32 orientation,
        /* [in] */ IRect* containingFrame,
        /* [in] */ IRect* contentInsets,
        /* [in] */ Boolean isFullScreen,
        /* [in] */ Boolean isVoiceInteraction);

    CARAPI_(void) PostAnimationCallback();

    CARAPI_(void) OverridePendingAppTransition(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 enterAnim,
        /* [in] */ Int32 exitAnim,
        /* [in] */ IIRemoteCallback* startedCallback);

    CARAPI_(void) OverridePendingAppTransitionScaleUp(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 startWidth,
        /* [in] */ Int32 startHeight);

    CARAPI_(void) OverridePendingAppTransitionThumb(
        /* [in] */ IBitmap* srcThumb,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ IIRemoteCallback* startedCallback,
        /* [in] */ Boolean scaleUp);

    CARAPI_(void) OverridePendingAppTransitionAspectScaledThumb(
        /* [in] */ IBitmap* srcThumb,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 targetWidth,
        /* [in] */ Int32 targetHeight,
        /* [in] */ IIRemoteCallback* startedCallback,
        /* [in] */ Boolean scaleUp);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns the human readable name of a window transition.
     *
     * @param transition The window transition.
     * @return The transition symbolic name.
     */
    static CARAPI_(String) AppTransitionToString(
        /* [in] */ Int32 transition);

    // public void dump(PrintWriter pw);

    CARAPI_(void) SetCurrentUser(
        /* [in] */ Int32 newUserId);

private:
    CARAPI_(AutoPtr<AttributeCache::Entry>) GetCachedAnimations(
        /* [in] */ IWindowManagerLayoutParams* lp);

    CARAPI_(AutoPtr<AttributeCache::Entry>) GetCachedAnimations(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 resId);

    CARAPI_(AutoPtr<IAnimation>) LoadAnimationRes(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 resId);

    /**
     * Compute the pivot point for an animation that is scaling from a small
     * rect on screen to a larger rect.  The pivot point varies depending on
     * the distance between the inner and outer edges on both sides.  This
     * function computes the pivot point for one dimension.
     * @param startPos  Offset from left/top edge of outer rectangle to
     * left/top edge of inner rectangle.
     * @param finalScale The scaling factor between the size of the outer
     * and inner rectangles.
     */
    static CARAPI_(Float) ComputePivot(
        /* [in] */ Int32 startPos,
        /* [in] */ Float finalScale);

    CARAPI_(AutoPtr<IAnimation>) CreateScaleUpAnimationLocked(
        /* [in] */ Int32 transit,
        /* [in] */ Boolean enter,
        /* [in] */ Int32 appWidth,
        /* [in] */ Int32 appHeight);

    CARAPI_(String) AppStateToString();

    CARAPI_(String) TransitTypeToString();

public:
    /** Not set up for a transition. */
    static const Int32 TRANSIT_UNSET = -1;
    /** No animation for transition. */
    static const Int32 TRANSIT_NONE = 0;
    /** A window in a new activity is being opened on top of an existing one in the same task. */
    static const Int32 TRANSIT_ACTIVITY_OPEN = 6;
    /** The window in the top-most activity is being closed to reveal the
     * previous activity in the same task. */
    static const Int32 TRANSIT_ACTIVITY_CLOSE = 7;
    /** A window in a new task is being opened on top of an existing one
     * in another activity's task. */
    static const Int32 TRANSIT_TASK_OPEN = 8;
    /** A window in the top-most activity is being closed to reveal the
     * previous activity in a different task. */
    static const Int32 TRANSIT_TASK_CLOSE = 9;
    /** A window in an existing task is being displayed on top of an existing one
     * in another activity's task. */
    static const Int32 TRANSIT_TASK_TO_FRONT = 10;
    /** A window in an existing task is being put below all other tasks. */
    static const Int32 TRANSIT_TASK_TO_BACK = 11;
    /** A window in a new activity that doesn't have a wallpaper is being opened on top of one that
     * does, effectively closing the wallpaper. */
    static const Int32 TRANSIT_WALLPAPER_CLOSE = 12;
    /** A window in a new activity that does have a wallpaper is being opened on one that didn't,
     * effectively opening the wallpaper. */
    static const Int32 TRANSIT_WALLPAPER_OPEN = 13;
    /** A window in a new activity is being opened on top of an existing one, and both are on top
     * of the wallpaper. */
    static const Int32 TRANSIT_WALLPAPER_INTRA_OPEN = 14;
    /** The window in the top-most activity is being closed to reveal the previous activity, and
     * both are on top of the wallpaper. */
    static const Int32 TRANSIT_WALLPAPER_INTRA_CLOSE = 15;
    /** A window in a new task is being opened behind an existing one in another activity's task.
     * The new window will show briefly and then be gone. */
    static const Int32 TRANSIT_TASK_OPEN_BEHIND = 16;

private:
    static const String TAG;
    static const Boolean DEBUG_APP_TRANSITIONS;
    static const Boolean DEBUG_ANIM;

    /** Fraction of animation at which the recents thumbnail stays completely transparent */
    static const Float RECENTS_THUMBNAIL_FADEIN_FRACTION = 0.7f;
    /** Fraction of animation at which the recents thumbnail becomes completely transparent */
    static const Float RECENTS_THUMBNAIL_FADEOUT_FRACTION = 0.3f;

    static const Int32 DEFAULT_APP_TRANSITION_DURATION = 250;
    static const Int32 THUMBNAIL_APP_TRANSITION_DURATION = 300;
    static const Int32 THUMBNAIL_APP_TRANSITION_ALPHA_DURATION = 325;

    static const Int32 NEXT_TRANSIT_TYPE_NONE = 0;
    static const Int32 NEXT_TRANSIT_TYPE_CUSTOM = 1;
    static const Int32 NEXT_TRANSIT_TYPE_SCALE_UP = 2;
    static const Int32 NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_UP = 3;
    static const Int32 NEXT_TRANSIT_TYPE_THUMBNAIL_SCALE_DOWN = 4;
    static const Int32 NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_UP = 5;
    static const Int32 NEXT_TRANSIT_TYPE_THUMBNAIL_ASPECT_SCALE_DOWN = 6;

    // These are the possible states for the enter/exit activities during a thumbnail transition
    static const Int32 THUMBNAIL_TRANSITION_ENTER_SCALE_UP = 0;
    static const Int32 THUMBNAIL_TRANSITION_EXIT_SCALE_UP = 1;
    static const Int32 THUMBNAIL_TRANSITION_ENTER_SCALE_DOWN = 2;
    static const Int32 THUMBNAIL_TRANSITION_EXIT_SCALE_DOWN = 3;

    static const Int32 APP_STATE_IDLE = 0;
    static const Int32 APP_STATE_READY = 1;
    static const Int32 APP_STATE_RUNNING = 2;
    static const Int32 APP_STATE_TIMEOUT = 3;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mH;

    Int32 mNextAppTransition;

    Int32 mNextAppTransitionType;

    String mNextAppTransitionPackage;
    AutoPtr<IBitmap> mNextAppTransitionThumbnail;
    // Used for thumbnail transitions. True if we're scaling up, false if scaling down
    Boolean mNextAppTransitionScaleUp;
    AutoPtr<IIRemoteCallback> mNextAppTransitionCallback;
    Int32 mNextAppTransitionEnter;
    Int32 mNextAppTransitionExit;
    Int32 mNextAppTransitionStartX;
    Int32 mNextAppTransitionStartY;
    Int32 mNextAppTransitionStartWidth;
    Int32 mNextAppTransitionStartHeight;
    AutoPtr<IRect> mNextAppTransitionInsets;

    AutoPtr<IRect> mTmpFromClipRect;
    AutoPtr<IRect> mTmpToClipRect;

    Int32 mAppTransitionState;

    Int32 mConfigShortAnimTime;
    AutoPtr<IInterpolator> mDecelerateInterpolator;
    AutoPtr<IInterpolator> mThumbnailFadeInInterpolator;
    AutoPtr<IInterpolator> mThumbnailFadeOutInterpolator;
    AutoPtr<IInterpolator> mThumbnailFastOutSlowInInterpolator;

    Int32 mCurrentUserId;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_AppTransition_H__
