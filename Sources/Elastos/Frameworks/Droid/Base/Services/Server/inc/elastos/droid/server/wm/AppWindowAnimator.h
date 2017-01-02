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

#ifndef __ELASTOS_DROID_SERVER_WM_APPWINDOWANIMATOR_H__
#define __ELASTOS_DROID_SERVER_WM_APPWINDOWANIMATOR_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.View.h>
#include "elastos/droid/view/animation/Animation.h"
#include "elastos/droid/server/wm/WindowStateAnimator.h"
#include "elastos/droid/server/wm/WindowAnimator.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::Animation::Animation;
using Elastos::Droid::View::Animation::ITransformation;
using Elastos::Droid::View::Animation::IAnimationAnimationListener;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class AppWindowAnimator
    : public Object
{
public:
    // This is an animation that does nothing: it just immediately finishes
    // itself every time it is called.  It is used as a stub animation in cases
    // where we want to synchronize multiple things that may be animating.
    class DummyAnimation : public Elastos::Droid::View::Animation::Animation
    {
    public:
        CARAPI GetTransformation(
            /* [in] */ Int64 currentTime,
            /* [in, out] */ ITransformation* outTransformation,
            /* [out] */ Boolean* result);

        CARAPI Clone(
            /* [out] */ IInterface** obj);
    };

public:
    TO_STRING_IMPL("AppWindowAnimator")

    AppWindowAnimator(
        /* [in] */ AppWindowToken* atoken);

    ~AppWindowAnimator();

    CARAPI Init();

    CARAPI_(void) SetAnimation(
        /* [in] */ IAnimation* anim,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) SetDummyAnimation();

    CARAPI_(void) ClearAnimation();

    CARAPI_(void) ClearThumbnail();

    CARAPI_(void) ClearDeferredThumbnail();

    CARAPI_(void) UpdateLayers();

    // This must be called while inside a transaction.
    CARAPI_(Boolean) StepAnimationLocked(
        /* [in] */ Int64 currentTime);

    CARAPI_(Boolean) ShowAllWindowsLocked();

    CARAPI_(AutoPtr<AppWindowToken>) GetAppToken();

private:
    CARAPI_(void) StepThumbnailAnimation(
        /* [in] */ Int64 currentTime);

    CARAPI_(Boolean) StepAnimation(
        /* [in] */ Int64 currentTime);

public:
    static const String TAG;

    AutoPtr<IWeakReference> mWeakAppToken; // AppWindowToken has AppWindowAnimator's ref
    CWindowManagerService* mService;
    AutoPtr<WindowAnimator> mAnimator;

    Boolean mAnimating;
    AutoPtr<IAnimation> mAnimation;
    Boolean mHasTransformation;
    AutoPtr<ITransformation> mTransformation;

    // Have we been asked to have this token keep the screen frozen?
    // Protect with mAnimator.
    Boolean mFreezingScreen;

    /**
     * How long we last kept the screen frozen.
     */
    Int32 mLastFreezeDuration;

    // Offset to the window of all layers in the token, for use by
    // AppWindowToken animations.
    Int32 mAnimLayerAdjustment;

    // Propagated from AppWindowToken.allDrawn, to determine when
    // the state changes.
    Boolean mAllDrawn;

    // Special surface for thumbnail animation.
    AutoPtr<ISurfaceControl> mThumbnail;
    Int32 mThumbnailTransactionSeq;
    Int32 mThumbnailX;
    Int32 mThumbnailY;
    Int32 mThumbnailLayer;
    Int32 mThumbnailForceAboveLayer;
    AutoPtr<IAnimation> mThumbnailAnimation;
    AutoPtr<ITransformation> mThumbnailTransformation;
    // This flag indicates that the destruction of the thumbnail surface is synchronized with
    // another animation, so do not pre-emptively destroy the thumbnail surface when the animation
    // completes
    Boolean mDeferThumbnailDestruction;
    // This is the thumbnail surface that has been bestowed upon this animator, and when the
    // surface for this animator's animation is complete, we will destroy the thumbnail surface
    // as well.  Do not animate or do anything with this surface.
    AutoPtr<ISurfaceControl> mDeferredThumbnail;

    /** WindowStateAnimator from mAppAnimator.allAppWindows as of last performLayout */
    List< AutoPtr<IWeakReference> > mAllAppWinAnimators;    // WindowStateAnimator*

    static const AutoPtr<IAnimation> sDummyAnimation;

};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_APPWINDOWANIMATOR_H__
