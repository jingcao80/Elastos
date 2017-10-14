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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_POINTERLOCATIONVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_POINTERLOCATIONVIEW_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/view/CMotionEvent.h"

using Elastos::Droid::View::View;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::CMotionEvent;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::View::IPointerCoords;
using Elastos::Droid::View::IPointerEventListener;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class PointerLocationView
    : public Elastos::Droid::View::View
    , public IInputDeviceListener
    , public IPointerEventListener
    , public IPointerLocationView
{
public:
    typedef VelocityTracker::Estimator Estimator;
    class PointerState
        : public Object
    {
    public:
        PointerState();

        CARAPI_(void) ClearTrace();

        CARAPI_(void) AddTrace(
            /* [in] */ Float x,
            /* [in] */ Float y,
            /* [in] */ Boolean current);

    private:
        // Trace of previous points.
        AutoPtr<ArrayOf<Float> > mTraceX;
        AutoPtr<ArrayOf<Float> > mTraceY;
        AutoPtr<ArrayOf<Boolean> > mTraceCurrent;
        Int32 mTraceCount;

        // True if the pointer is down.
        Boolean mCurDown;

        // Most recent coordinates.
        AutoPtr<IPointerCoords> mCoords;
        Int32 mToolType;

        // Most recent velocity.
        Float mXVelocity;
        Float mYVelocity;
        Float mAltXVelocity;
        Float mAltYVelocity;

        // Current bounding box, if any
        Boolean mHasBoundingBox;
        Float mBoundingLeft;
        Float mBoundingTop;
        Float mBoundingRight;
        Float mBoundingBottom;

        // Position estimator.
        AutoPtr<Estimator> mEstimator;
        AutoPtr<Estimator> mAltEstimator;
        friend class PointerLocationView;
    };

private:
    // HACK
    // A quick and dirty string builder implementation optimized for GC.
    // Using String.format causes the application grind to a halt when
    // more than a couple of pointers are down due to the number of
    // temporary objects allocated while formatting strings for drawing or logging.
    class FasterStringBuilder
        : public Object
    {
    public:
        FasterStringBuilder();

        CARAPI_(AutoPtr<FasterStringBuilder>) Clear();

        CARAPI_(AutoPtr<FasterStringBuilder>) Append(
            /* [in] */ const String& value);

        CARAPI_(AutoPtr<FasterStringBuilder>) Append(
            /* [in] */ Int32 value);

        CARAPI_(AutoPtr<FasterStringBuilder>) Append(
            /* [in] */ Int32 value,
            /* [in] */ Int32 zeroPadWidth);

        CARAPI_(AutoPtr<FasterStringBuilder>) Append(
            /* [in] */ Float value,
            /* [in] */ Int32 precision);

        using Object::ToString;

        CARAPI_(String) ToString();

    private:
        CARAPI_(Int32) Reserve(
            /* [in] */ Int32 length);

    private:
        AutoPtr<ArrayOf<Char32> > mChars;
        Int32 mLength;
    };

public:
    CAR_INTERFACE_DECL()

    PointerLocationView();

    CARAPI constructor(
        /* [in] */ IContext* c);

    CARAPI SetPrintCoords(
        /* [in] */ Boolean state);

    CARAPI OnPointerEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI OnInputDeviceAdded(
        /* [in] */ Int32 deviceId);

    CARAPI OnInputDeviceChanged(
        /* [in] */ Int32 deviceId);

    CARAPI OnInputDeviceRemoved(
        /* [in] */ Int32 deviceId);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    static CARAPI_(Boolean) ShouldLogKey(
        /* [in] */ Int32 keyCode);

    CARAPI_(void) DrawOval(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float major,
        /* [in] */ Float minor,
        /* [in] */ Float angle,
        /* [in] */ IPaint* paint);

    CARAPI_(void) LogMotionEvent(
        /* [in] */ const String& type,
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) LogCoords(
        /* [in] */ const String& type,
        /* [in] */ Int32 action,
        /* [in] */ Int32 index,
        /* [in] */ IPointerCoords* coords,
        /* [in] */ Int32 id,
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) LogInputDevices();

    CARAPI_(void) LogInputDeviceState(
        /* [in] */ Int32 deviceId,
        /* [in] */ const String& state);

private:
    // Draw an oval.  When angle is 0 radians, orients the major axis vertically,
    // angles less than or greater than 0 radians rotate the major axis left or right.
    AutoPtr<IRectF> mReusableOvalRect;

    static const String TAG;

    // The system property key used to specify an alternate velocity tracker strategy
    // to plot alongside the default one.  Useful for testing and comparison purposes.
    static const String ALT_STRATEGY_PROPERY_KEY;

    Int32 ESTIMATE_PAST_POINTS;
    Int32 ESTIMATE_FUTURE_POINTS;
    Float ESTIMATE_INTERVAL;

    AutoPtr<IInputManager> mIm;

    AutoPtr<IViewConfiguration> mVC;
    AutoPtr<IPaint> mTextPaint;
    AutoPtr<IPaint> mTextBackgroundPaint;
    AutoPtr<IPaint> mTextLevelPaint;
    AutoPtr<IPaint> mPaint;
    AutoPtr<IPaint> mCurrentPointPaint;
    AutoPtr<IPaint> mTargetPaint;
    AutoPtr<IPaint> mPathPaint;
    AutoPtr<IPaintFontMetricsInt> mTextMetrics;
    Int32 mHeaderBottom;
    Boolean mCurDown;
    Int32 mCurNumPointers;
    Int32 mMaxNumPointers;
    Int32 mActivePointerId;
    AutoPtr<IArrayList> mPointers;
    AutoPtr<IPointerCoords> mTempCoords;

    AutoPtr<IVelocityTracker> mVelocity;
    AutoPtr<IVelocityTracker> mAltVelocity;

    AutoPtr<FasterStringBuilder> mText;

    Boolean mPrintCoords;

    // HACK
    // A quick and dirty string builder implementation optimized for GC.
    // Using String.format causes the application grind to a halt when
    // more than a couple of pointers are down due to the number of
    // temporary objects allocated while formatting strings for drawing or logging.

};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_POINTERLOCATIONVIEW_H__
