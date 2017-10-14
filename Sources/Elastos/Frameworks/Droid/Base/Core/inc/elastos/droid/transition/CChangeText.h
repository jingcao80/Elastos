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

#ifndef __ELASTOS_DROID_TRANSITION_CCHANGETEXT_H__
#define __ELASTOS_DROID_TRANSITION_CCHANGETEXT_H__

#include "_Elastos_Droid_Transition_CChangeText.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/transition/Transition.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This transition tracks changes to the text in TextView targets. If the text
 * changes between the start and end scenes, the transition ensures that the
 * starting text stays until the transition ends, at which point it changes
 * to the end text.  This is useful in situations where you want to resize a
 * text view to its new size before displaying the text that goes there.
 *
 * @hide
 */
CarClass(CChangeText)
    , public Transition
    , public IChangeText
{
private:
    class TransitionListenerAdapter_1
        : public TransitionListenerAdapter
    {
    public:
        TransitionListenerAdapter_1(
            /* [in] */ Int32 behavior,
            /* [in] */ ITextView* v,
            /* [in] */ Int32 startSelectionStart,
            /* [in] */ Int32 startSelectionEnd,
            /* [in] */ Int32 endSelectionStart,
            /* [in] */ Int32 endSelectionEnd,
            /* [in] */ ICharSequence* startText,
            /* [in] */ ICharSequence* endText,
            /* [in] */ Int32 endColor,
            /* [in] */ CChangeText* owner);

        CARAPI OnTransitionPause(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionResume(
            /* [in] */ ITransition* transition);

    public:
        Int32 mPausedColor;
        Int32 mBehavior;
        ITextView* mView;
        Int32 mStartSelectionStart;
        Int32 mStartSelectionEnd;
        Int32 mEndSelectionStart;
        Int32 mEndSelectionEnd;
        ICharSequence* mStartText;
        ICharSequence* mEndText;
        Int32 mEndColor;
        CChangeText* mOwner;
    };

    class AnimatorListenerAdapter_1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_1(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 endColor);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    public:
        AutoPtr<ITextView> mView;
        Int32 mEndColor;
    };

    class AnimatorListenerAdapter_2
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_2(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 endSelectionStart,
            /* [in] */ Int32 endSelectionEnd,
            /* [in] */ ICharSequence* startText,
            /* [in] */ ICharSequence* endText,
            /* [in] */ Int32 endColor,
            /* [in] */ CChangeText* owner);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        ITextView* mView;
        Int32 mEndSelectionStart;
        Int32 mEndSelectionEnd;
        ICharSequence* mStartText;
        ICharSequence* mEndText;
        Int32 mEndColor;
        CChangeText* mOwner;
    };

    class AnimatorListenerAdapter_3
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_3(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 endSelectionStart,
            /* [in] */ Int32 endSelectionEnd,
            /* [in] */ ICharSequence* startText,
            /* [in] */ ICharSequence* endText,
            /* [in] */ CChangeText* owner);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        ITextView* mView;
        Int32 mEndSelectionStart;
        Int32 mEndSelectionEnd;
        ICharSequence* mStartText;
        ICharSequence* mEndText;
        CChangeText* mOwner;
    };

    class AnimatorUpdateListener_1
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener_1(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 startColor);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    public:
        AutoPtr<ITextView> mView;
        Int32 mStartColor;
    };

    class AnimatorUpdateListener_2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorUpdateListener_2(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 endColor);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    public:
        AutoPtr<ITextView> mView;
        Int32 mEndColor;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CChangeText();

    CARAPI SetChangeBehavior(
        /* [in] */ Int32 changeBehavior);

    CARAPI GetTransitionProperties(
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetChangeBehavior(
        /* [out] */ Int32* result);

    CARAPI CaptureStartValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CaptureEndValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    using Transition::CaptureValues;

    CARAPI_(void) CaptureValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI_(void) SetSelection(
        /* [in] */ IEditText* editText,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI CloneImpl(
        /* [in] */ IChangeText* obj);

private:
    static const String TAG;

    static const String PROPNAME_TEXT;
    static const String PROPNAME_TEXT_SELECTION_START;
    static const String PROPNAME_TEXT_SELECTION_END;
    static const String PROPNAME_TEXT_COLOR;

    Int32 mChangeBehavior;
    static AutoPtr<ArrayOf<String> > sTransitionProperties;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CCHANGETEXT_H__
