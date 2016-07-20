
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/widget/RotarySelector.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::R;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String RotarySelector::TAG("RotarySelector");
const Boolean RotarySelector::DBG = FALSE;
const Boolean RotarySelector::VISUAL_DEBUG = FALSE;
const Int64 RotarySelector::VIBRATE_SHORT = 20;  // msec
const Int64 RotarySelector::VIBRATE_LONG = 20;  // msec
const Int32 RotarySelector::ARROW_SCRUNCH_DIP = 6;
const Int32 RotarySelector::EDGE_PADDING_DIP = 9;
const Int32 RotarySelector::EDGE_TRIGGER_DIP = 100;
const Int32 RotarySelector::OUTER_ROTARY_RADIUS_DIP = 390;
const Int32 RotarySelector::ROTARY_STROKE_WIDTH_DIP = 83;
const Int32 RotarySelector::SNAP_BACK_ANIMATION_DURATION_MILLIS = 300;
const Int32 RotarySelector::SPIN_ANIMATION_DURATION_MILLIS = 800;
AutoPtr<IAudioAttributes> RotarySelector::VIBRATION_ATTRIBUTES;

CAR_INTERFACE_IMPL(RotarySelector, View, IRotarySelector)

RotarySelector::RotarySelector()
    : mDensity(0.0f)
    , mLeftHandleX(0)
    , mRightHandleX(0)
    , mRotaryOffsetX(0)
    , mAnimating(FALSE)
    , mAnimationStartTime(0)
    , mAnimationDuration(0)
    , mAnimatingDeltaXStart(0)
    , mAnimatingDeltaXEnd(0)
    , mGrabbedState(IRotarySelector::NOTHING_GRABBED)
    , mTriggered(FALSE)
    , mEdgeTriggerThresh(0)
    , mDimpleWidth(0)
    , mBackgroundWidth(0)
    , mBackgroundHeight(0)
    , mOuterRadius(0)
    , mInnerRadius(0)
    , mDimpleSpacing(0)
    , mMinimumVelocity(0)
    , mMaximumVelocity(0)
    , mDimplesOfFling(0)
    , mOrientation(0)
{
    CPaint::New((IPaint**)&mPaint);

    CMatrix::New((IMatrix**)&mBgMatrix);
    CMatrix::New((IMatrix**)&mArrowMatrix);

    AutoPtr<IAudioAttributesBuilder> p;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&p);
    p->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    p->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    p->Build((IAudioAttributes**)&VIBRATION_ATTRIBUTES);
}

ECode RotarySelector::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode RotarySelector::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::RotarySelector);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetInt32(R::styleable::RotarySelector_orientation, IRotarySelector::HORIZONTAL, &mOrientation);
    a->Recycle();

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    AutoPtr<IDisplayMetrics> dm;
    r->GetDisplayMetrics((IDisplayMetrics**)&dm);
    dm->GetDensity(&mDensity);
    if (DBG) {
        SLOGGERD(TAG, String("- Density: ") + StringUtils::ToString(mDensity));
    }

    // Assets (all are BitmapDrawables).
    mBackground = GetBitmapFor(R::drawable::jog_dial_bg);
    mDimple = GetBitmapFor(R::drawable::jog_dial_dimple);
    mDimpleDim = GetBitmapFor(R::drawable::jog_dial_dimple_dim);

    mArrowLongLeft = GetBitmapFor(R::drawable::jog_dial_arrow_long_left_green);
    mArrowLongRight = GetBitmapFor(R::drawable::jog_dial_arrow_long_right_red);
    mArrowShortLeftAndRight = GetBitmapFor(R::drawable::jog_dial_arrow_short_left_and_right);

    CDecelerateInterpolator::New(1.0f, (IDecelerateInterpolator**)&mInterpolator);

    mEdgeTriggerThresh = (Int32) (mDensity * EDGE_TRIGGER_DIP);

    mDimple->GetWidth(&mDimpleWidth);

    mBackground->GetWidth(&mBackgroundWidth);
    mBackground->GetHeight(&mBackgroundHeight);
    mOuterRadius = (Int32) (mDensity * OUTER_ROTARY_RADIUS_DIP);
    mInnerRadius = (Int32) ((OUTER_ROTARY_RADIUS_DIP - ROTARY_STROKE_WIDTH_DIP) * mDensity);

    AutoPtr<IViewConfigurationHelper> hlp;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&hlp);
    AutoPtr<IViewConfiguration> configuration;
    hlp->Get(mContext, (IViewConfiguration**)&configuration);
    Int32 temp = 0;
    configuration->GetScaledMinimumFlingVelocity(&temp);
    mMinimumVelocity = temp * 2;
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    return NOERROR;
}

AutoPtr<IBitmap> RotarySelector::GetBitmapFor(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IResources> rs;
    cxt->GetResources((IResources**)&rs);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> bm;
    factory->DecodeResource(rs, resId, (IBitmap**)&bm);
    return bm;
}

void RotarySelector::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    View::OnSizeChanged(w, h, oldw, oldh);

    Int32 edgePadding = (Int32) (EDGE_PADDING_DIP * mDensity);
    mLeftHandleX = edgePadding + mDimpleWidth / 2;
    Int32 length = IsHoriz() ? w : h;
    mRightHandleX = length - edgePadding - mDimpleWidth / 2;
    mDimpleSpacing = (length / 2) - mLeftHandleX;

    // bg matrix only needs to be calculated once
    mBgMatrix->SetTranslate(0, 0);
    if (!IsHoriz()) {
        // set up matrix for translating drawing of background and arrow assets
        Int32 left = w - mBackgroundHeight;
        Boolean rst = FALSE;
        mBgMatrix->PreRotate(-90.f, 0.f, 0.f, &rst);
        mBgMatrix->PostTranslate(left, h, &rst);

    }
    else {
        Boolean rst = FALSE;
        mBgMatrix->PostTranslate(0.f, h - mBackgroundHeight, &rst);
    }
}

Boolean RotarySelector::IsHoriz()
{
    return mOrientation == IRotarySelector::HORIZONTAL;
}

ECode RotarySelector::SetLeftHandleResource(
    /* [in] */ Int32 resId)
{
    if (resId != 0) {
        mLeftHandleIcon = GetBitmapFor(resId);
    }
    Invalidate();

    return NOERROR;
}

ECode RotarySelector::SetRightHandleResource(
    /* [in] */ Int32 resId)
{
    if (resId != 0) {
        mRightHandleIcon = GetBitmapFor(resId);
    }
    Invalidate();

    return NOERROR;
}

ECode RotarySelector::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 length = IsHoriz() ?
        MeasureSpec::GetSize(widthMeasureSpec) :
        MeasureSpec::GetSize(heightMeasureSpec);
    Int32 arrowScrunch = (Int32) (ARROW_SCRUNCH_DIP * mDensity);
    Int32 arrowH = 0;
    mArrowShortLeftAndRight->GetHeight(&arrowH);

    // by making the height less than arrow + bg, arrow and bg will be scrunched together,
    // overlaying somewhat (though on transparent portions of the drawable).
    // this works because the arrows are drawn from the top, and the rotary bg is drawn
    // from the bottom.
    Int32 height = mBackgroundHeight + arrowH - arrowScrunch;

    if (IsHoriz()) {
        SetMeasuredDimension(length, height);
    }
    else {
        SetMeasuredDimension(height, length);
    }
    return NOERROR;
}

void RotarySelector::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);

    Int32 width = 0;
    GetWidth(&width);

    if (VISUAL_DEBUG) {
        // draw bounding box around widget
        mPaint->SetColor(0xffff0000);
        mPaint->SetStyle(PaintStyle_STROKE);
        Int32 h = 0;
        GetHeight(&h);
        canvas->DrawRect(0, 0, width, h, mPaint);
    }

    Int32 height = 0;
    GetHeight(&height);

    // update animating state before we draw anything
    if (mAnimating) {
        UpdateAnimation();
    }

    // Background:
    canvas->DrawBitmap(mBackground, mBgMatrix, mPaint);

    // Draw the correct arrow(s) depending on the current state:
    mArrowMatrix->Reset();
    switch (mGrabbedState) {
        case IRotarySelector::NOTHING_GRABBED:
            //mArrowShortLeftAndRight;
            break;
        case IRotarySelector::LEFT_HANDLE_GRABBED:
            mArrowMatrix->SetTranslate(0, 0);
            if (!IsHoriz()) {
                Boolean r = FALSE;
                mArrowMatrix->PreRotate(-90.f, 0.f, 0.f, &r);
                mArrowMatrix->PostTranslate(0.f, height, &r);
            }
            canvas->DrawBitmap(mArrowLongLeft, mArrowMatrix, mPaint);
            break;
        case IRotarySelector::RIGHT_HANDLE_GRABBED:
            mArrowMatrix->SetTranslate(0, 0);
            if (!IsHoriz()) {
                Boolean r = FALSE;
                mArrowMatrix->PreRotate(-90.f, 0.f, 0.f, &r);
                // since bg width is > height of screen in landscape mode...
                mArrowMatrix->PostTranslate(0.f, height + (mBackgroundWidth - height), &r);
            }
            canvas->DrawBitmap(mArrowLongRight, mArrowMatrix, mPaint);
            break;
        default:
            break;
            //throw new IllegalStateException("invalid mGrabbedState: " + mGrabbedState);
    }

    Int32 bgHeight = mBackgroundHeight;
    Int32 bgTop = IsHoriz() ?
            height - bgHeight:
            width - bgHeight;

    if (VISUAL_DEBUG) {
        // draw circle bounding arc drawable: good sanity check we're doing the math correctly
        Float orV = OUTER_ROTARY_RADIUS_DIP * mDensity;
        Int32 vOffset = mBackgroundWidth - height;
        Int32 midX = IsHoriz() ? width / 2 : mBackgroundWidth / 2 - vOffset;
        if (IsHoriz()) {
            canvas->DrawCircle(midX, orV + bgTop, orV, mPaint);
        }
        else {
            canvas->DrawCircle(orV + bgTop, midX, orV, mPaint);
        }
    }

    // left dimple / icon
    {
        Int32 xOffset = mLeftHandleX + mRotaryOffsetX;
        Int32 drawableY = GetYOnArc(
                mBackgroundWidth,
                mInnerRadius,
                mOuterRadius,
                xOffset);
        Int32 x = IsHoriz() ? xOffset : drawableY + bgTop;
        Int32 y = IsHoriz() ? drawableY + bgTop : height - xOffset;
        if (mGrabbedState != IRotarySelector::RIGHT_HANDLE_GRABBED) {
            DrawCentered(mDimple, canvas, x, y);
            DrawCentered(mLeftHandleIcon, canvas, x, y);
        }
        else {
            DrawCentered(mDimpleDim, canvas, x, y);
        }
    }

    // center dimple
    {
        Int32 xOffset = IsHoriz() ?
                width / 2 + mRotaryOffsetX:
                height / 2 + mRotaryOffsetX;
        Int32 drawableY = GetYOnArc(
                mBackgroundWidth,
                mInnerRadius,
                mOuterRadius,
                xOffset);

        if (IsHoriz()) {
            DrawCentered(mDimpleDim, canvas, xOffset, drawableY + bgTop);
        }
        else {
            // vertical
            DrawCentered(mDimpleDim, canvas, drawableY + bgTop, height - xOffset);
        }
    }

    // right dimple / icon
    {
        Int32 xOffset = mRightHandleX + mRotaryOffsetX;
        Int32 drawableY = GetYOnArc(
                mBackgroundWidth,
                mInnerRadius,
                mOuterRadius,
                xOffset);

        Int32 x = IsHoriz() ? xOffset : drawableY + bgTop;
        Int32 y = IsHoriz() ? drawableY + bgTop : height - xOffset;
        if (mGrabbedState != IRotarySelector::LEFT_HANDLE_GRABBED) {
            DrawCentered(mDimple, canvas, x, y);
            DrawCentered(mRightHandleIcon, canvas, x, y);
        }
        else {
            DrawCentered(mDimpleDim, canvas, x, y);
        }
    }

    // draw extra left hand dimples
    Int32 dimpleLeft = mRotaryOffsetX + mLeftHandleX - mDimpleSpacing;
    Int32 halfdimple = mDimpleWidth / 2;
    while (dimpleLeft > -halfdimple) {
        Int32 drawableY = GetYOnArc(
                mBackgroundWidth,
                mInnerRadius,
                mOuterRadius,
                dimpleLeft);

        if (IsHoriz()) {
            DrawCentered(mDimpleDim, canvas, dimpleLeft, drawableY + bgTop);
        }
        else {
            DrawCentered(mDimpleDim, canvas, drawableY + bgTop, height - dimpleLeft);
        }
        dimpleLeft -= mDimpleSpacing;
    }

    // draw extra right hand dimples
    Int32 dimpleRight = mRotaryOffsetX + mRightHandleX + mDimpleSpacing;
    Int32 rightThresh = mRight + halfdimple;
    while (dimpleRight < rightThresh) {
        Int32 drawableY = GetYOnArc(
                mBackgroundWidth,
                mInnerRadius,
                mOuterRadius,
                dimpleRight);

        if (IsHoriz()) {
            DrawCentered(mDimpleDim, canvas, dimpleRight, drawableY + bgTop);
        }
        else {
            DrawCentered(mDimpleDim, canvas, drawableY + bgTop, height - dimpleRight);
        }
        dimpleRight += mDimpleSpacing;
    }
}

Int32 RotarySelector::GetYOnArc(
    /* [in] */ Int32 backgroundWidth,
    /* [in] */ Int32 innerRadius,
    /* [in] */ Int32 outerRadius,
    /* [in] */ Int32 x)
{

    // the hypotenuse
    Int32 halfWidth = (outerRadius - innerRadius) / 2;
    Int32 middleRadius = innerRadius + halfWidth;

    // the bottom leg of the triangle
    Int32 triangleBottom = (backgroundWidth / 2) - x;

    // "Our offense is like the pythagorean theorem: There is no answer!" - Shaquille O'Neal
    Int32 triangleY =
        (Int32) Elastos::Core::Math::Sqrt(middleRadius * middleRadius - triangleBottom * triangleBottom);

    // convert to drawing coordinates:
    // middleRadius - triangleY =
    //   the vertical distance from the outer edge of the circle to the desired point
    // from there we add the distance from the top of the drawable to the middle circle
    return middleRadius - triangleY + halfWidth;
}

ECode RotarySelector::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mAnimating) {
        *result = TRUE;
        return NOERROR;
    }
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
    mVelocityTracker->AddMovement(event);

    Int32 height = 0;
    GetHeight(&height);

    Float x = 0.0f, y = 0.0f;
    event->GetX(&x);
    event->GetY(&y);
    Int32 eventX = IsHoriz() ? (Int32)x : height - (Int32)y;
    Int32 hitWindow = mDimpleWidth;

    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            if (DBG)
                SLOGGERD(TAG, "touch-down")
            mTriggered = FALSE;
            if (mGrabbedState != IRotarySelector::NOTHING_GRABBED) {
                Reset();
                Invalidate();
            }
            if (eventX < mLeftHandleX + hitWindow) {
                mRotaryOffsetX = eventX - mLeftHandleX;
                SetGrabbedState(IRotarySelector::LEFT_HANDLE_GRABBED);
                Invalidate();
                Vibrate(VIBRATE_SHORT);
            }
            else if (eventX > mRightHandleX - hitWindow) {
                mRotaryOffsetX = eventX - mRightHandleX;
                SetGrabbedState(IRotarySelector::RIGHT_HANDLE_GRABBED);
                Invalidate();
                Vibrate(VIBRATE_SHORT);
            }
            break;

        case IMotionEvent::ACTION_MOVE:
            if (DBG)
                SLOGGERD(TAG, "touch-move");
            if (mGrabbedState == IRotarySelector::LEFT_HANDLE_GRABBED) {
                mRotaryOffsetX = eventX - mLeftHandleX;
                Invalidate();
                Int32 r = 0;
                GetRight(&r);
                Int32 rightThresh = IsHoriz() ? r : height;
                if (eventX >= rightThresh - mEdgeTriggerThresh && !mTriggered) {
                    mTriggered = TRUE;
                    DispatchTriggerEvent(IOnDialTriggerListener::LEFT_HANDLE);
                    AutoPtr<VelocityTracker> velocityTracker = mVelocityTracker;
                    velocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
                    Float x = 0.0f, y = 0.0f;
                    velocityTracker->GetXVelocity(&x);
                    velocityTracker->GetYVelocity(&y);
                    Int32 rawVelocity = IsHoriz() ?
                            (Int32)x : -(Int32)y;
                    Int32 velocity = Elastos::Core::Math::Max(mMinimumVelocity, rawVelocity);
                    mDimplesOfFling = Elastos::Core::Math::Max(8,
                            Elastos::Core::Math::Abs(velocity / mDimpleSpacing));
                    StartAnimationWithVelocity(
                            eventX - mLeftHandleX,
                            mDimplesOfFling * mDimpleSpacing,
                            velocity);
                }
            }
            else if (mGrabbedState == IRotarySelector::RIGHT_HANDLE_GRABBED) {
                mRotaryOffsetX = eventX - mRightHandleX;
                Invalidate();
                if (eventX <= mEdgeTriggerThresh && !mTriggered) {
                    mTriggered = TRUE;
                    DispatchTriggerEvent(IOnDialTriggerListener::RIGHT_HANDLE);
                    AutoPtr<VelocityTracker> velocityTracker = mVelocityTracker;
                    velocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
                    Float x = 0.0f, y = 0.0f;
                    velocityTracker->GetXVelocity(&x);
                    velocityTracker->GetYVelocity(&y);
                    Int32 rawVelocity = IsHoriz() ?
                            (Int32)x : - (Int32)y;
                    Int32 velocity = Elastos::Core::Math::Min(-mMinimumVelocity, rawVelocity);
                    mDimplesOfFling = Elastos::Core::Math::Max(8,
                            Elastos::Core::Math::Abs(velocity / mDimpleSpacing));
                    StartAnimationWithVelocity(
                            eventX - mRightHandleX,
                            -(mDimplesOfFling * mDimpleSpacing),
                            velocity);
                }
            }
            break;
        case IMotionEvent::ACTION_UP:
            if (DBG) {
                SLOGGERD(TAG, "touch-up")
            }
            // handle animating back to start if they didn't trigger
            if (mGrabbedState == IRotarySelector::LEFT_HANDLE_GRABBED
                && Elastos::Core::Math::Abs(eventX - mLeftHandleX) > 5) {
                // set up "snap back" animation
                StartAnimation(eventX - mLeftHandleX, 0, SNAP_BACK_ANIMATION_DURATION_MILLIS);
            }
            else if (mGrabbedState == IRotarySelector::RIGHT_HANDLE_GRABBED
                && Elastos::Core::Math::Abs(eventX - mRightHandleX) > 5) {
                // set up "snap back" animation
                StartAnimation(eventX - mRightHandleX, 0, SNAP_BACK_ANIMATION_DURATION_MILLIS);
            }
            mRotaryOffsetX = 0;
            SetGrabbedState(IRotarySelector::NOTHING_GRABBED);
            Invalidate();
            if (mVelocityTracker != NULL) {
                mVelocityTracker->Recycle(); // wishin' we had generational GC
                mVelocityTracker = NULL;
            }
            break;
        case IMotionEvent::ACTION_CANCEL:
            if (DBG) {
                SLOGGERD(TAG, "touch-cancel");
            }
            Reset();
            Invalidate();
            if (mVelocityTracker != NULL) {
                mVelocityTracker->Recycle();
                mVelocityTracker = NULL;
            }
            break;
    }
    *result = TRUE;
    return NOERROR;
}

void RotarySelector::StartAnimation(
    /* [in] */ Int32 startX,
    /* [in] */ Int32 endX,
    /* [in] */ Int32 duration)
{
    mAnimating = TRUE;
    AnimationUtils::CurrentAnimationTimeMillis(&mAnimationStartTime);
    mAnimationDuration = duration;
    mAnimatingDeltaXStart = startX;
    mAnimatingDeltaXEnd = endX;
    SetGrabbedState(IRotarySelector::NOTHING_GRABBED);
    mDimplesOfFling = 0;
    Invalidate();
}

void RotarySelector::StartAnimationWithVelocity(
    /* [in] */ Int32 startX,
    /* [in] */ Int32 endX,
    /* [in] */ Int32 pixelsPerSecond)
{
    mAnimating = TRUE;
    AnimationUtils::CurrentAnimationTimeMillis(&mAnimationStartTime);
    mAnimationDuration = 1000 * (endX - startX) / pixelsPerSecond;
    mAnimatingDeltaXStart = startX;
    mAnimatingDeltaXEnd = endX;
    SetGrabbedState(IRotarySelector::NOTHING_GRABBED);
    Invalidate();
}

void RotarySelector::UpdateAnimation()
{
    Int64 mils = 0;
    AnimationUtils::CurrentAnimationTimeMillis(&mils);
    Int64 millisSoFar = mils - mAnimationStartTime;
    Int64 millisLeft = mAnimationDuration - millisSoFar;
    Int32 totalDeltaX = mAnimatingDeltaXStart - mAnimatingDeltaXEnd;
    Boolean goingRight = totalDeltaX < 0;
    if (DBG) {
        SLOGGERD(TAG, String("millisleft for animating: ") + StringUtils::ToString(millisLeft));
    }
    if (millisLeft <= 0) {
        Reset();
        return;
    }
    // from 0 to 1 as animation progresses
    Float interpolation = 0.0f;
    mInterpolator->GetInterpolation((Float) millisSoFar / mAnimationDuration, &interpolation);
    Int32 dx = (Int32) (totalDeltaX * (1 - interpolation));
    mRotaryOffsetX = mAnimatingDeltaXEnd + dx;

    // once we have gone far enough to animate the current buttons off screen, we start
    // wrapping the offset back to the other side so that when the animation is finished,
    // the buttons will come back into their original places.
    if (mDimplesOfFling > 0) {
        if (!goingRight && mRotaryOffsetX < -3 * mDimpleSpacing) {
            // wrap around on fling left
            mRotaryOffsetX += mDimplesOfFling * mDimpleSpacing;
        }
        else if (goingRight && mRotaryOffsetX > 3 * mDimpleSpacing) {
            // wrap around on fling right
            mRotaryOffsetX -= mDimplesOfFling * mDimpleSpacing;
        }
    }
    Invalidate();
}

void RotarySelector::Reset()
{
    mAnimating = FALSE;
    mRotaryOffsetX = 0;
    mDimplesOfFling = 0;
    SetGrabbedState(IRotarySelector::NOTHING_GRABBED);
    mTriggered = FALSE;
}

ECode RotarySelector::Vibrate(
    /* [in] */ Int64 duration)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 value = 0;
    FAIL_RETURN(Settings::System::GetInt32ForUser(
        cr, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 1,
            IUserHandle::USER_CURRENT, &value))
    Boolean hapticEnabled = value != 0;
    if (hapticEnabled) {
        if (mVibrator == NULL) {
            AutoPtr<IContext> cxt;
            GetContext((IContext**)&cxt);
            AutoPtr<IInterface> temp;
            cxt->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&temp);
            mVibrator = IVibrator::Probe(temp);
        }
        mVibrator->Vibrate(duration, VIBRATION_ATTRIBUTES);
    }
    return NOERROR;
}

void RotarySelector::DrawCentered(
    /* [in] */ IBitmap* d,
    /* [in] */ ICanvas* c,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 w = 0;
    d->GetWidth(&w);
    Int32 h = 0;
    d->GetHeight(&h);

    c->DrawBitmap(d, x - (w / 2), y - (h / 2), mPaint);
}

ECode RotarySelector::SetOnDialTriggerListener(
    /* [in] */ IOnDialTriggerListener* l)
{
    mOnDialTriggerListener = l;

    return NOERROR;
}

ECode RotarySelector::DispatchTriggerEvent(
    /* [in] */ Int32 whichHandle)
{
    Vibrate(VIBRATE_LONG);
    if (mOnDialTriggerListener != NULL) {
        mOnDialTriggerListener->OnDialTrigger(this, whichHandle);
    }
    return NOERROR;
}

void RotarySelector::SetGrabbedState(
    /* [in] */ Int32 newState)
{
    if (newState != mGrabbedState) {
        mGrabbedState = newState;
        if (mOnDialTriggerListener != NULL) {
            mOnDialTriggerListener->OnGrabbedStateChange(this, mGrabbedState);
        }
    }
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
