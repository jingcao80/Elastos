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

#include "elastos/droid/incallui/InCallAnimationUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::View::IView;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// InCallAnimationUtils::CrossFadeDrawable
//================================================================
InCallAnimationUtils::CrossFadeDrawable::CrossFadeDrawable(
    /* [in] */ ArrayOf<IDrawable*>* layers)
{
    LayerDrawable::constructor(layers);
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Int32> > atts = ArrayOf<Int32>::Alloc(2);
    (*atts)[0] = 0xff;
    (*atts)[1] = 0;
    helper->OfInt32(TO_IINTERFACE(this), String("crossFadeAlpha"), atts, (IObjectAnimator**)&mAnimator);
}

void InCallAnimationUtils::CrossFadeDrawable::SetCrossFadeAlpha(
    /* [in] */ Int32 alpha)
{
    mCrossFadeAlpha = alpha;
    InvalidateSelf();
}

AutoPtr<IObjectAnimator> InCallAnimationUtils::CrossFadeDrawable::GetAnimator()
{
    return mAnimator;
}

ECode InCallAnimationUtils::CrossFadeDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IDrawable> first;
    GetDrawable(0, (IDrawable**)&first);
    AutoPtr<IDrawable> second;
    GetDrawable(1, (IDrawable**)&second);

    if (mCrossFadeAlpha > 0) {
        first->SetAlpha(mCrossFadeAlpha);
        first->Draw(canvas);
        first->SetAlpha(255);
    }

    if (mCrossFadeAlpha < 0xff) {
        second->SetAlpha(0xff - mCrossFadeAlpha);
        second->Draw(canvas);
        second->SetAlpha(0xff);
    }
    return NOERROR;
}


//================================================================
// InCallAnimationUtils::AnimatorListener
//================================================================
ECode InCallAnimationUtils::AnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    if (FADE_DBG) {
        Int32 toHashcode, fromHashcode;
        IObject::Probe(mFrom)->GetHashCode(&fromHashcode);
        IObject::Probe(mTo)->GetHashCode(&toHashcode);
        Logger::D(TAG, "cross-fade animation start (%s -> %s)"
                , StringUtils::ToHexString(fromHashcode).string(), StringUtils::ToHexString(toHashcode).string());
    }
    return NOERROR;
}

ECode InCallAnimationUtils::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (FADE_DBG) {
        Int32 toHashcode, fromHashcode;
        IObject::Probe(mFrom)->GetHashCode(&fromHashcode);
        IObject::Probe(mTo)->GetHashCode(&toHashcode);
        Logger::D(TAG, "cross-fade animation ended (%s -> %s)"
                , StringUtils::ToHexString(fromHashcode).string(), StringUtils::ToHexString(toHashcode).string());
    }
    animation->RemoveAllListeners();
    // Workaround for issue 6300562; this will force the drawable to the
    // resultant one regardless of animation glitch.
    mImageView->SetImageDrawable(mTo);
    return NOERROR;
}


//================================================================
// InCallAnimationUtils
//================================================================
const Int32 InCallAnimationUtils::ANIMATION_DURATION;
const String InCallAnimationUtils::TAG("InCallAnimationUtils");
const Boolean InCallAnimationUtils::FADE_DBG;

AutoPtr<InCallAnimationUtils::CrossFadeDrawable> InCallAnimationUtils::NewCrossFadeDrawable(
    /* [in] */ IDrawable* first,
    /* [in] */ IDrawable* second)
{
    AutoPtr<ArrayOf<IDrawable*> > layers = ArrayOf<IDrawable*>::Alloc(2);
    layers->Set(0, first);
    layers->Set(1, second);
    AutoPtr<CrossFadeDrawable> d = new CrossFadeDrawable(layers);
    return d;
}

void InCallAnimationUtils::StartCrossFade(
    /* [in] */ IImageView* imageView,
    /* [in] */ IDrawable* from,
    /* [in] */ IDrawable* to)
{
    // We skip the cross-fade when those two Drawables are equal, or they are BitmapDrawables
    // pointing to the same Bitmap.
    Boolean equals;
    Boolean drawableIsEqual = (from != NULL && to != NULL && (IObject::Probe(from)->Equals(to, &equals), equals));
    AutoPtr<IBitmapDrawable> fromDrawable = IBitmapDrawable::Probe(from);
    AutoPtr<IBitmap> fromBm;
    if (fromDrawable != NULL) {
        fromDrawable->GetBitmap((IBitmap**)&fromBm);
    }
    Boolean hasFromImage = fromBm != NULL;

    AutoPtr<IBitmapDrawable> toDrawable = IBitmapDrawable::Probe(from);
    AutoPtr<IBitmap> toBm;
    if (toDrawable != NULL) {
        toDrawable->GetBitmap((IBitmap**)&toBm);
    }
    Boolean hasToImage = toBm != NULL;

    Boolean areSameImage = drawableIsEqual || (hasFromImage && hasToImage &&
            (IObject::Probe(fromBm)->Equals(toBm, &equals), equals));

    if (!areSameImage) {
        if (FADE_DBG) {
            Int32 toHashcode, fromHashcode;
            IObject::Probe(from)->GetHashCode(&fromHashcode);
            IObject::Probe(to)->GetHashCode(&toHashcode);
            Logger::D(TAG, "Start cross-fade animation for %s(%s -> %s)", TO_CSTR(imageView)
                    , StringUtils::ToHexString(fromHashcode).string(), StringUtils::ToHexString(toHashcode).string());
        }

        AutoPtr<CrossFadeDrawable> crossFadeDrawable = NewCrossFadeDrawable(from, to);
        AutoPtr<IAnimator> animator = IAnimator::Probe(crossFadeDrawable->GetAnimator());
        imageView->SetImageDrawable(IDrawable::Probe(crossFadeDrawable));
        animator->SetDuration(ANIMATION_DURATION);
        AutoPtr<IAnimatorListener> listener = IAnimatorListener::Probe(new AnimatorListener(imageView, from, to));
        animator->AddListener(listener);
        animator->Start();

        /* We could use TransitionDrawable here, but it may cause some weird animation in
         * some corner cases. See issue 6300562
         * TODO: decide which to be used in the long run. TransitionDrawable is old but system
         * one. Ours uses new animation framework and thus have callback (great for testing),
         * while no framework support for the exact class.

        Drawable[] layers = new Drawable[2];
        layers[0] = from;
        layers[1] = to;
        TransitionDrawable transitionDrawable = new TransitionDrawable(layers);
        imageView.setImageDrawable(transitionDrawable);
        transitionDrawable.startTransition(ANIMATION_DURATION); */
        IView::Probe(imageView)->SetTag(to);
    }
    else if (!hasFromImage && hasToImage) {
        imageView->SetImageDrawable(to);
        IView::Probe(imageView)->SetTag(to);
    }
    else {
        if (FADE_DBG) {
            Logger::D(TAG, "*Not* start cross-fade. %s", TO_CSTR(imageView));
        }
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
