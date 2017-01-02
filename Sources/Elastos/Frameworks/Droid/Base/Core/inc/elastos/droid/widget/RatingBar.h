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

#ifndef __ELASTOS_DROID_WIDGET_RATINGBAR_H__
#define __ELASTOS_DROID_WIDGET_RATINGBAR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/AbsSeekBar.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class RatingBar
    : public AbsSeekBar
    , public IRatingBar
{
public:
    CAR_INTERFACE_DECL()

    RatingBar();

    ~RatingBar();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Sets the listener to be called when the rating changes.
     *
     * @param listener The listener.
     */
    CARAPI SetOnRatingBarChangeListener(
        /* [in] */ IOnRatingBarChangeListener* listener);

    /**
     * @return The listener (may be null) that is listening for rating change
     *         events.
     */
    CARAPI GetOnRatingBarChangeListener(
        /* [out] */ IOnRatingBarChangeListener** listener);

    /**
     * Whether this rating bar should only be an indicator (thus non-changeable
     * by the user).
     *
     * @param isIndicator Whether it should be an indicator.
     *
     * @attr ref android.R.styleable#RatingBar_isIndicator
     */
    CARAPI SetIsIndicator(
        /* [in] */ Boolean isIndicator);

    /**
     * @return Whether this rating bar is only an indicator.
     *
     * @attr ref android.R.styleable#RatingBar_isIndicator
     */
    CARAPI IsIndicator(
        /* [out] */ Boolean* isIndicator);

    /**
     * Sets the number of stars to show. In order for these to be shown
     * properly, it is recommended the layout width of this widget be wrap
     * content.
     *
     * @param numStars The number of stars.
     */
    CARAPI SetNumStars(
        /* [in] */ Int32 numStars);

    /**
     * Returns the number of stars shown.
     * @return The number of stars shown.
     */
    CARAPI GetNumStars(
        /* [out] */ Int32* numStars);

    /**
     * Sets the rating (the number of stars filled).
     *
     * @param rating The rating to set.
     */
    CARAPI SetRating(
        /* [in] */ Float rating);

    /**
     * Gets the current rating (number of stars filled).
     *
     * @return The current rating.
     */
    CARAPI GetRating(
        /* [out] */ Float* rating);

    /**
     * Sets the step size (granularity) of this rating bar.
     *
     * @param stepSize The step size of this rating bar. For example, if
     *            half-star granularity is wanted, this would be 0.5.
     */
    CARAPI SetStepSize(
        /* [in] */ Float stepSize);

    /**
     * Gets the step size of this rating bar.
     *
     * @return The step size.
     */
    CARAPI GetStepSize(
        /* [out] */ Float* stepSize);

    // @Override
    CARAPI SetMax(
        /* [in] */ Int32 max);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    // @Override
    CARAPI_(AutoPtr<IShape>) GetDrawableShape();

    // @Override
    CARAPI OnProgressRefresh(
        /* [in] */ Float scale,
        /* [in] */ Boolean fromUser);

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnStartTrackingTouch();

    // @Override
    CARAPI OnStopTrackingTouch();

    // @Override
    CARAPI OnKeyChange();

    virtual CARAPI AnimateSetProgress(
        /* [in] */ Int32 progress);

    virtual CARAPI DispatchRatingChange(
        /* [in] */ Boolean fromUser);

private:
    CARAPI_(Float) GetProgressPerStar();

    CARAPI_(void) UpdateSecondaryProgress(
        /* [in] */ Int32 progress);

private:
    static const String RATINGBAR_NAME;
    Int32 mNumStars;

    Int32 mProgressOnStartTracking;

    AutoPtr<IOnRatingBarChangeListener> mOnRatingBarChangeListener;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_RATINGBAR_H__
