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

#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CCANDIDATEVIEW_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CCANDIDATEVIEW_H__

#include "_Elastos_Droid_InputMethodService_CCandidateView.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/GestureDetector.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::View;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CarClass(CCandidateView)
    , public Elastos::Droid::View::View
    , public ICandidateView
{
protected:
    class SelfSimpleOnGestureListener
        : public GestureDetector::SimpleOnGestureListener
    {
    public:
        SelfSimpleOnGestureListener(
            /* [in] */ CCandidateView* host);

        ~SelfSimpleOnGestureListener();

        CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

    private:
        CCandidateView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CCandidateView();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    /**
     * A connection back to the service to communicate with the text field
     * @param listener
     */
    CARAPI SetService(
        /* [in] */ ISoftKeyboard* listener);

    CARAPI_(Int32) ComputeHorizontalScrollRange();

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI SetSuggestions(
        /* [in] */ ArrayOf<ICharSequence*>* suggestions,
        /* [in] */ Boolean completions,
        /* [in] */ Boolean typedWordValid);

    CARAPI Clear();

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

protected:
    /**
     * For flick through from keyboard, call this method with the x coordinate of the flick
     * gesture.
     * @param x
     */
    CARAPI_(void) TakeSuggestionAt(
        /* [in] */ Float x);

    CARAPI_(void) ScrollToTarget();

    CARAPI_(void) RemoveHighlight();

protected:
    static const Int32 OUT_OF_BOUNDS = -1;
    static const Int32 MAX_SUGGESTIONS = 32;
    static const Int32 SCROLL_PIXELS = 20;
    static const Int32 X_GAP = 10;
    static AutoPtr< ArrayOf<ICharSequence*> > EMPTY_LIST;

    AutoPtr<ISoftKeyboard> mService;
    AutoPtr< ArrayOf<ICharSequence*> > mSuggestions;
    Int32 mSelectedIndex;
    Int32 mTouchX;
    AutoPtr<IDrawable> mSelectionHighlight;
    Boolean mTypedWordValid;

    AutoPtr<IRect> mBgPadding;

    AutoPtr< ArrayOf<Int32> > mWordWidth;
    AutoPtr< ArrayOf<Int32> > mWordX;

    Int32 mColorNormal;
    Int32 mColorRecommended;
    Int32 mColorOther;
    Int32 mVerticalPadding;
    AutoPtr<IPaint> mPaint;
    Boolean mScrolled;
    Int32 mTargetScrollX;

    Int32 mTotalWidth;
    AutoPtr<IGestureDetector> mGestureDetector;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CCANDIDATEVIEW_H__
