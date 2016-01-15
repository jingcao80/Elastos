#ifndef __ELASTOS_DROID_GESTURE_GESTUREOVERLAYVIEW_H__
#define __ELASTOS_DROID_GESTURE_GESTUREOVERLAYVIEW_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Gesture.h"
#include "elastos/core/Object.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Gesture::IGestureOverlayView;
using Elastos::Droid::Gesture::IFadeOutRunnable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::IList;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Utility::IArrayList;
using Elastos::Droid::View::Animation::IAccelerateDecelerateInterpolator;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;

//WAITING
//using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace Gesture {

class GestureOverlayView
    : public Object
    , public IGestureOverlayView
//WAITING
//    , public FrameLayout
{
public:
    /*
     * Android: private class FadeOutRunnable implements Runnable
     */
    class FadeOutRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();

        FadeOutRunnable();

        virtual ~FadeOutRunnable();

        CARAPI constructor();

        CARAPI Run();

        CARAPI GetFireActionPerformed(
            /* [out] */ Boolean *fireActionPerformed);

        CARAPI GetResetMultipleStrokes(
            /* [out] */ Boolean *resetMultipleStrokes);

        CARAPI SetFireActionPerformed(
            /* [in] */ Boolean fireActionPerformed);

        CARAPI SetResetMultipleStrokes(
            /* [in] */ Boolean resetMultipleStrokes);

    public:
        Boolean mFireActionPerformed;
        Boolean mResetMultipleStrokes;
    };

public:
    CAR_INTERFACE_DECL();

    GestureOverlayView();

    virtual ~GestureOverlayView();

    CARAPI constructor(
        /* [in] */ IContext *context);

    CARAPI constructor(
        /* [in] */ IContext *context,
        /* [in] */ IAttributeSet *attrs);

    CARAPI constructor(
        /* [in] */ IContext *context,
        /* [in] */ IAttributeSet *attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext *context,
        /* [in] */ IAttributeSet *attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI GetCurrentStroke(
        /* [out] */ IArrayList **stroke);

    CARAPI GetOrientation(
        /* [out] */ Int32 *orientation);

    CARAPI SetOrientation(
        /* [in] */ Int32 orientation);

    CARAPI SetGestureColor(
        /* [in] */ Int32 color);

    CARAPI SetUncertainGestureColor(
        /* [in] */ Int32 color);

    CARAPI GetUncertainGestureColor(
        /* [out] */ Int32 *color);

    CARAPI GetGestureColor(
        /* [out] */ Int32 *color);

    CARAPI GetGestureStrokeWidth(
        /* [out] */ Float *gestureStrokeWidth);

    CARAPI SetGestureStrokeWidth(
        /* [in] */ Float gestureStrokeWidth);

    CARAPI GetGestureStrokeType(
        /* [out] */ Int32 *gestureStrokeType);

    CARAPI SetGestureStrokeType(
        /* [in] */ Int32 gestureStrokeType);

    CARAPI GetGestureStrokeLengthThreshold(
        /* [out] */ Float *gestureStrokeLengthThreshold);

    CARAPI SetGestureStrokeLengthThreshold(
        /* [in] */ Float gestureStrokeLengthThreshold);

    CARAPI GetGestureStrokeSquarenessTreshold(
        /* [out] */ Float *gestureStrokeSquarenessTreshold);

    CARAPI SetGestureStrokeSquarenessTreshold(
        /* [in] */ Float gestureStrokeSquarenessTreshold);

    CARAPI GetGestureStrokeAngleThreshold(
        /* [out] */ Float *gestureStrokeAngleThreshold);

    CARAPI SetGestureStrokeAngleThreshold(
        /* [in] */ Float gestureStrokeAngleThreshold);

    CARAPI IsEventsInterceptionEnabled(
        /* [out] */ Boolean *isEventsInterceptionEnabled);

    CARAPI SetEventsInterceptionEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsFadeEnabled(
        /* [out] */ Boolean *isFadeEnabled);

    CARAPI SetFadeEnabled(
        /* [in] */ Boolean fadeEnabled);

    CARAPI GetGesture(
        /* [out] */ IGesture **gesture);

    CARAPI SetGesture(
        /* [in] */ IGesture *gesture);

    CARAPI GetGesturePath(
        /* [out] */ IPath **gesturePath);

    CARAPI GetGesturePath(
        /* [in] */ IPath *path,
        /* [out] */ IPath **gesturePath);

    CARAPI IsGestureVisible(
        /* [out] */ Boolean *isGestureVisible);

    CARAPI SetGestureVisible(
       /* [in] */ Boolean visible);

    CARAPI GetFadeOffset(
        /* [out] */ Int64 *fadeOffset);

    CARAPI SetFadeOffset(
        /* [in] */ Int64 fadeOffset);

    CARAPI AddOnGestureListener(
        /* [in] */ IOnGestureListener *listener);

    CARAPI RemoveOnGestureListener(
        /* [in] */ IOnGestureListener *listener);

    CARAPI RemoveAllOnGestureListeners();

    CARAPI AddOnGesturePerformedListener(
        /* [in] */ IOnGesturePerformedListener *listener);

    CARAPI RemoveOnGesturePerformedListener(
        /* [in] */ IOnGesturePerformedListener *listener);

    CARAPI RemoveAllOnGesturePerformedListeners();

    CARAPI AddOnGesturingListener(
        /* [in] */ IOnGesturingListener *listener);

    CARAPI RemoveOnGesturingListener(
        /* [in] */ IOnGesturingListener *listener);

    CARAPI RemoveAllOnGesturingListeners();

    CARAPI IsGesturing(
        /* [out] */ Boolean *isGesturing);

    /**
     * @hide
     */
    CARAPI GetGesturePaint(
        /* [out] */ IPaint **gesturePaint);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas *canvas);

    CARAPI Clear(
        /* [in] */ Boolean animated);

    CARAPI CancelClearAnimation();

    CARAPI CancelGesture();

    //@Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent *event,
        /* [out] */ Boolean *isDispatched);

protected:
    //@Override
    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) init();

    CARAPI SetCurrentColor(
        /* [in] */ Int32 color);

    CARAPI SetPaintAlpha(
        /* [in] */ Int32 alpha);

    CARAPI Clear(
        /* [in] */ Boolean animated,
        /* [in] */ Boolean fireActionPerformed,
        /* [in] */ Boolean immediate);

    CARAPI ProcessEvent(
        /* [in] */ IMotionEvent *event,
        /* [out] */ Boolean *isProcessed);

    CARAPI TouchDown(
        /* [in] */ IMotionEvent *event);

    CARAPI TouchMove(
        /* [in] */ IMotionEvent *event,
        /* [out] */ IRect **rect);

    CARAPI TouchUp(
        /* [in] */ IMotionEvent * event,
        /* [in] */ Boolean cancel);

    CARAPI CancelGesture(
        /* [in] */ IMotionEvent * event);

    CARAPI FireOnGesturePerformed();

private:
    static const Int32 FADE_ANIMATION_RATE = 16;
    static const Boolean GESTURE_RENDERING_ANTIALIAS = TRUE;
    static const Boolean DITHER_FLAG = TRUE;
/*
 * these constants are defined in GestureOverlayView.car

    const Int32 GESTURE_STROKE_TYPE_SINGLE = 0;
    const Int32 GESTURE_STROKE_TYPE_MULTIPLE = 1;

    const Int32 ORIENTATION_HORIZONTAL = 0;
    const Int32 ORIENTATION_VERTICAL = 1;

    const Int32 FADE_ANIMATION_RATE = 16;
    const Int32 GESTURE_RENDERING_ANTIALIAS = 1;
    const Int32 DITHER_FLAG = 1;
*/


    AutoPtr<IPaint> mGesturePaint;

    Int64 mFadeDuration;
    Int64 mFadeOffset;
    Int64 mFadingStart;
    Boolean mFadingHasStarted;
    Boolean mFadeEnabled;

    Int32 mCurrentColor;
    Int32 mCertainGestureColor;
    Int32 mUncertainGestureColor;
    Float mGestureStrokeWidth;
    Int32 mInvalidateExtraBorder;

    Int32 mGestureStrokeType;
    Float mGestureStrokeLengthThreshold;
    Float mGestureStrokeSquarenessTreshold;
    Float mGestureStrokeAngleThreshold;

    Int32 mOrientation;

    AutoPtr<IRect> mInvalidRect;
    AutoPtr<IPath> mPath;
    Boolean mGestureVisible;

    Float mX;
    Float mY;

    Float mCurveEndX;
    Float mCurveEndY;

    Float mTotalLength;
    Boolean mIsGesturing;
    Boolean mPreviousWasGesturing;
    Boolean mInterceptEvents;
    Boolean mIsListeningForGestures;
    Boolean mResetGesture;

    // current gesture
    AutoPtr<IGesture> mCurrentGesture;
    AutoPtr<IArrayList> mStrokeBuffer;

    AutoPtr<IArrayList> mOnGestureListeners;
    AutoPtr<IArrayList> mOnGesturePerformedListeners;
    AutoPtr<IArrayList> mOnGesturingListeners;

    Boolean mHandleGestureActions;

    // fading out effect
    Boolean mIsFadingOut;
    Float mFadingAlpha;
    AutoPtr<IAccelerateDecelerateInterpolator> mInterpolator;
    AutoPtr<IFadeOutRunnable> mFadingOut;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_GESTUREOVERLAYVIEW_H__
