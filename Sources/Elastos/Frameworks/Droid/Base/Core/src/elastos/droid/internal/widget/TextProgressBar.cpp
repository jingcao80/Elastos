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

#include "elastos/droid/internal/widget/TextProgressBar.h"
#include "elastos/droid/os/SystemClock.h"

using Elastos::Droid::Internal::Widget::EIID_ITextProgressBar;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::EIID_IOnChronometerTickListener;
using Elastos::Droid::Widget::IRelativeLayoutLayoutParams;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const Int32 TextProgressBar::PROGRESSBAR_ID;
const Int32 TextProgressBar::CHRONOMETER_ID;
const String TextProgressBar::TAG("TextProgressBar");


CAR_INTERFACE_IMPL(TextProgressBar::InnerListener, Object, IOnChronometerTickListener)

TextProgressBar::InnerListener::InnerListener(
    /* [in] */ TextProgressBar* host)
    : mHost(host)
{
}

ECode TextProgressBar::InnerListener::OnChronometerTick(
    /* [in] */ IChronometer* chronometer)
{
    return mHost->OnChronometerTick(chronometer);
}

CAR_INTERFACE_IMPL(TextProgressBar, RelativeLayout, ITextProgressBar)

TextProgressBar::TextProgressBar()
   : mDurationBase(-1)
   , mDuration(-1)
   , mChronometerFollow(FALSE)
   , mChronometerGravity(IGravity::NO_GRAVITY)
{
}

ECode TextProgressBar::constructor(
    /* [in] */ IContext* context)
{
    return RelativeLayout::constructor(context);
}

ECode TextProgressBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

ECode TextProgressBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return RelativeLayout::constructor(context, attrs, defStyleAttr);
}

ECode TextProgressBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return RelativeLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
}

/**
 * Catch any interesting children when they are added.
 */
ECode TextProgressBar::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    RelativeLayout::AddView(child, index, params);

    Int32 childId;
    child->GetId(&childId);

    if (childId == CHRONOMETER_ID && IChronometer::Probe(child)) {
        AutoPtr<InnerListener> listener = new InnerListener(this);
        mChronometer = IChronometer::Probe(child);
        mChronometer->SetOnChronometerTickListener(listener);

        // Check if Chronometer should move with with ProgressBar
        Int32 width;
        params->GetWidth(&width);
        mChronometerFollow = (width== IViewGroupLayoutParams::WRAP_CONTENT);

        Int32 gravity;
        ITextView::Probe(mChronometer)->GetGravity(&gravity);
        mChronometerGravity = (gravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK);
    }
    else if (childId == PROGRESSBAR_ID && IProgressBar::Probe(child)) {
        mProgressBar = IProgressBar::Probe(child);
    }
    return NOERROR;
}

ECode TextProgressBar::SetDurationBase(
    /* [in] */ Int64 durationBase)
{
    mDurationBase = durationBase;
    if (mProgressBar == NULL || mChronometer == NULL) {
        return E_RUNTIME_EXCEPTION;
    }

    // Update the ProgressBar maximum relative to Chronometer base
    Int64 base;
    mChronometer->GetBase(&base);
    mDuration = (Int32)(durationBase - base);
    if (mDuration <= 0) {
        mDuration = 1;
    }

    mProgressBar->SetMax(mDuration);
    return NOERROR;
}

ECode TextProgressBar::OnChronometerTick(
    /* [in] */ IChronometer* chronometer)
{
    if (mProgressBar == NULL) {
        return E_RUNTIME_EXCEPTION;
    }

    // Stop Chronometer if we're past duration
    Int64 now = SystemClock::GetElapsedRealtime();
    if (now >= mDurationBase) {
        mChronometer->Stop();
    }

    // Update the ProgressBar status
    Int32 remaining = (Int32)(mDurationBase - now);
    mProgressBar->SetProgress(mDuration - remaining);

    // Move the Chronometer if gravity is set correctly
    if (mChronometerFollow) {
        AutoPtr<IViewGroupLayoutParams> params;

        // Calculate estimate of ProgressBar leading edge position
        IView::Probe(mProgressBar)->GetLayoutParams((IViewGroupLayoutParams**)&params);

        Int32 contentWidth;
        IView::Probe(mProgressBar)->GetWidth(&contentWidth);
        Int32 leftMargin, rightMargin;
        IViewGroupMarginLayoutParams::Probe(params)->GetLeftMargin(&leftMargin);
        IViewGroupMarginLayoutParams::Probe(params)->GetRightMargin(&rightMargin);
        contentWidth -= leftMargin + rightMargin;

        Int32 progress, max;
        mProgressBar->GetProgress(&progress);
        mProgressBar->GetMax(&max);
        Int32 leadingEdge = ((contentWidth * progress) / max) + leftMargin;

        // Calculate any adjustment based on gravity
        Int32 adjustLeft = 0;
        Int32 textWidth;
        IView::Probe(mChronometer)->GetWidth(&textWidth);
        if (mChronometerGravity == IGravity::END) {
            adjustLeft = -textWidth;
        }
        else if (mChronometerGravity == IGravity::CENTER_HORIZONTAL) {
            adjustLeft = -(textWidth / 2);
        }

        // Limit margin to keep text inside ProgressBar bounds
        leadingEdge += adjustLeft;
        Int32 rightLimit = contentWidth - rightMargin - textWidth;
        if (leadingEdge < leftMargin) {
            leadingEdge = leftMargin;
        }
        else if (leadingEdge > rightLimit) {
            leadingEdge = rightLimit;
        }
        params = NULL;
        IView::Probe(mChronometer)->GetLayoutParams((IViewGroupLayoutParams**)&params);
        IViewGroupMarginLayoutParams::Probe(params)->SetLeftMargin(leadingEdge);

        // Request layout to move Chronometer
        IView::Probe(mChronometer)->RequestLayout();
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

