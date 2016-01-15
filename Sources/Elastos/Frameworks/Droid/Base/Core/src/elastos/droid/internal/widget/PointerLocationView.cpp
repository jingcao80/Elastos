
#include "Elastos.Droid.Content.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/internal/widget/PointerLocationView.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/CInputDevice.h"
// #include "elastos/droid/view/CInputDeviceHelper.h"
#include "elastos/droid/view/CPointerCoords.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CPaintFontMetricsInt.h"
#include "elastos/droid/os/CSystemProperties.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPaintFontMetricsInt;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Hardware::Input::IInputDeviceListener;
using Elastos::Droid::Hardware::Input::EIID_IInputDeviceListener;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::CInputDevice;
using Elastos::Droid::View::IInputDeviceHelper;
// using Elastos::Droid::View::CInputDeviceHelper;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::CPointerCoords;
using Elastos::Droid::View::EIID_IPointerEventListener;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionRange;

using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

const String PointerLocationView::TAG("Pointer");
const String PointerLocationView::ALT_STRATEGY_PROPERY_KEY("debug.velocitytracker.alt");

/*-----------------------------------PointerState-----------------------------------*/
PointerLocationView::PointerState::PointerState()
    : mTraceCount(0)
    , mCurDown(FALSE)
    , mToolType(0)
    , mXVelocity(0.0f)
    , mYVelocity(0.0f)
    , mAltXVelocity(0.0f)
    , mAltYVelocity(0.0f)
    , mHasBoundingBox(FALSE)
    , mBoundingLeft(0.0f)
    , mBoundingTop(0.0f)
    , mBoundingRight(0.0f)
    , mBoundingBottom(0.0f)
{
    mTraceX = ArrayOf<Float>::Alloc(32);
    mTraceY = ArrayOf<Float>::Alloc(32);
    mTraceCurrent = ArrayOf<Boolean>::Alloc(32);
    CPointerCoords::New((IPointerCoords**)&mCoords);
    mEstimator = new Estimator();
    mAltEstimator = new Estimator();
}

void PointerLocationView::PointerState::ClearTrace()
{
    mTraceCount = 0;
}

void PointerLocationView::PointerState::AddTrace(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean current)
{
    Int32 traceCapacity = mTraceX->GetLength();
    if (mTraceCount == traceCapacity) {
        traceCapacity *= 2;
        AutoPtr<ArrayOf<Float> > newTraceX = ArrayOf<Float>::Alloc(traceCapacity);
        newTraceX->Copy(mTraceX, mTraceCount);
        mTraceX = newTraceX;

        AutoPtr<ArrayOf<Float> > newTraceY = ArrayOf<Float>::Alloc(traceCapacity);
        newTraceY->Copy(mTraceY, mTraceCount);
        mTraceY = newTraceY;

        AutoPtr<ArrayOf<Boolean> > newTraceCurrent = ArrayOf<Boolean>::Alloc(traceCapacity);
        newTraceCurrent->Copy(mTraceCurrent, mTraceCount);
        mTraceCurrent= newTraceCurrent;
    }

    (*mTraceX)[mTraceCount] = x;
    (*mTraceY)[mTraceCount] = y;
    (*mTraceCurrent)[mTraceCount] = current;
    mTraceCount += 1;
}

/*-----------------------------------FasterStringBuilder-----------------------------------*/
PointerLocationView::FasterStringBuilder::FasterStringBuilder()
    : mLength(0)
{
    mChars = ArrayOf<Char32>::Alloc(64);
}

AutoPtr<PointerLocationView::FasterStringBuilder> PointerLocationView::FasterStringBuilder::Clear()
{
    mLength = 0;
    return this;
}

AutoPtr<PointerLocationView::FasterStringBuilder> PointerLocationView::FasterStringBuilder::Append(
    /* [in] */ const String& value)
{
    Int32 valueLength = value.GetLength();
    Int32 index = Reserve(valueLength);
    AutoPtr<ArrayOf<Char32> > subArray = value.GetChars();
    mChars->Copy(index, subArray, valueLength);
    mLength += valueLength;
    return this;
}

AutoPtr<PointerLocationView::FasterStringBuilder> PointerLocationView::FasterStringBuilder::Append(
    /* [in] */ Int32 value)
{
    return Append(value, 0);
}

AutoPtr<PointerLocationView::FasterStringBuilder> PointerLocationView::FasterStringBuilder::Append(
    /* [in] */ Int32 value,
    /* [in] */ Int32 zeroPadWidth)
{
    Boolean negative = value < 0;
    if (negative) {
        value = - value;
        if (value < 0) {
            Append(String("-2147483648"));
            return this;
        }
    }

    Int32 index = Reserve(11);
    AutoPtr<ArrayOf<Char32> > chars = mChars;

    if (value == 0) {
        (*chars)[index++] = '0';
        mLength += 1;
        return this;
    }

    if (negative) {
        (*chars)[index++] = '-';
    }

    Int32 divisor = 1000000000;
    Int32 numberWidth = 10;
    while (value < divisor) {
        divisor /= 10;
        numberWidth -= 1;
        if (numberWidth < zeroPadWidth) {
            (*chars)[index++] = '0';
        }
    }

    do {
        Int32 digit = value / divisor;
        value -= digit * divisor;
        divisor /= 10;
        (*chars)[index++] = (Char32) (digit + '0');
    } while (divisor != 0);

    mLength = index;
    return this;
}

AutoPtr<PointerLocationView::FasterStringBuilder> PointerLocationView::FasterStringBuilder::Append(
    /* [in] */ Float value,
    /* [in] */ Int32 precision)
{
    Int32 scale = 1;
    for (Int32 i = 0; i < precision; i++) {
        scale *= 10;
    }
    value = (Float) (Elastos::Core::Math::Rint(value * scale) / scale);

    Append((Int32) value);

    if (precision != 0) {
        Append(String("."));
        value = Elastos::Core::Math::Abs(value);
        value -= Elastos::Core::Math::Floor(value);
        Append((Int32) (value * scale), precision);
    }

    return this;
}

String PointerLocationView::FasterStringBuilder::ToString()
{
    return String(*mChars, 0, mLength);
}

Int32 PointerLocationView::FasterStringBuilder::Reserve(
    /* [in] */ Int32 length)
{
    Int32 oldLength = mLength;
    Int32 newLength = mLength + length;
    AutoPtr<ArrayOf<Char32> > oldChars = mChars;
    Int32 oldCapacity = oldChars->GetLength();
    if (newLength > oldCapacity) {
        Int32 newCapacity = oldCapacity * 2;
        AutoPtr<ArrayOf<Char32> > newChars = ArrayOf<Char32>::Alloc(newCapacity);
        newChars->Copy(oldChars, oldLength);
        mChars = newChars;
    }
    return oldLength;
}

/*-----------------------------------PointerLocationView-----------------------------------*/
CAR_INTERFACE_IMPL_3(PointerLocationView, View, IInputDeviceListener, IPointerEventListener, IPointerLocationView)

PointerLocationView::PointerLocationView()
    : ESTIMATE_PAST_POINTS(4)
    , ESTIMATE_FUTURE_POINTS(2)
    , ESTIMATE_INTERVAL(0.02f)
    , mHeaderBottom(0)
    , mCurDown(FALSE)
    , mCurNumPointers(0)
    , mMaxNumPointers(0)
    , mActivePointerId(0)
    , mPrintCoords(TRUE)
{
    CPaintFontMetricsInt::New((IPaintFontMetricsInt**)&mTextMetrics);
    CArrayList::New((IArrayList**)&mPointers);
    CPointerCoords::New((IPointerCoords**)&mTempCoords);
    mText = new FasterStringBuilder();
    CRectF::New((IRectF**)&mReusableOvalRect);
}

ECode PointerLocationView::constructor(
    /* [in] */ IContext* c)
{
    View::constructor(c);
    SetFocusableInTouchMode(TRUE);

    AutoPtr<IInterface> svTemp;
    c->GetSystemService(IContext::INPUT_SERVICE, (IInterface**)&svTemp);
    mIm = IInputManager::Probe(svTemp);

    mVC = CViewConfiguration::Get(c);
    CPaint::New((IPaint**)&mTextPaint);
    mTextPaint->SetAntiAlias(TRUE);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);
    mTextPaint->SetTextSize(10 * density);
    mTextPaint->SetARGB(255, 0, 0, 0);
    CPaint::New((IPaint**)&mTextBackgroundPaint);
    mTextBackgroundPaint->SetAntiAlias(FALSE);
    mTextBackgroundPaint->SetARGB(128, 255, 255, 255);
    CPaint::New((IPaint**)&mTextLevelPaint);
    mTextLevelPaint->SetAntiAlias(FALSE);
    mTextLevelPaint->SetARGB(192, 255, 0, 0);
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetARGB(255, 255, 255, 255);
    mPaint->SetStyle(PaintStyle_STROKE);
    mPaint->SetStrokeWidth(2);
    CPaint::New((IPaint**)&mCurrentPointPaint);
    mCurrentPointPaint->SetAntiAlias(TRUE);
    mCurrentPointPaint->SetARGB(255, 255, 0, 0);
    mCurrentPointPaint->SetStyle(PaintStyle_STROKE);
    mCurrentPointPaint->SetStrokeWidth(2);
    CPaint::New((IPaint**)&mTargetPaint);
    mTargetPaint->SetAntiAlias(FALSE);
    mTargetPaint->SetARGB(255, 0, 0, 192);
    CPaint::New((IPaint**)&mPathPaint);
    mPathPaint->SetAntiAlias(FALSE);
    mPathPaint->SetARGB(255, 0, 96, 255);
    mPaint->SetStyle(PaintStyle_STROKE);
    mPaint->SetStrokeWidth(1);

    AutoPtr<PointerState> ps = new PointerState();
    mPointers->Add(IInterface::Probe((IObject*)ps));
    mActivePointerId = 0;

    mVelocity = VelocityTracker::Obtain();

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String altStrategy;
    sp->Get(ALT_STRATEGY_PROPERY_KEY, &altStrategy);
    if (altStrategy.GetLength() != 0) {
        Logger::D(TAG, "Comparing default velocity tracker strategy with %s", (const char*)altStrategy);
        mAltVelocity = VelocityTracker::Obtain(altStrategy);
    }
    else {
        mAltVelocity = NULL;
    }
    return NOERROR;
}

ECode PointerLocationView::SetPrintCoords(
    /* [in] */ Boolean state)
{
    mPrintCoords = state;
    return NOERROR;
}

void PointerLocationView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    View::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    Int32 fontMetrics = 0;
    mTextPaint->GetFontMetricsInt(mTextMetrics, &fontMetrics);
    Int32 ascent = 0, descent = 0;
    mTextMetrics->GetAscent(&ascent);
    mTextMetrics->GetDescent(&descent);
    mHeaderBottom = -ascent + descent + 2;
    if (FALSE) {
        Int32 leading = 0, top = 0, bottom = 0;
        mTextMetrics->GetLeading(&leading);
        mTextMetrics->GetTop(&top);
        mTextMetrics->GetBottom(&bottom);
        SLOGGERI(String("PointerLocationView::foo"), String("Metrics: ascent=") + StringUtils::ToString(ascent)
                + " descent=" + StringUtils::ToString(descent)
                + " leading=" + StringUtils::ToString(leading)
                + " top=" + StringUtils::ToString(top)
                + " bottom=" + StringUtils::ToString(bottom));
    }
}

void PointerLocationView::DrawOval(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float major,
    /* [in] */ Float minor,
    /* [in] */ Float angle,
    /* [in] */ IPaint* paint)
{
    Int32 rst = 0;
    canvas->Save(ICanvas::MATRIX_SAVE_FLAG, &rst);
    canvas->Rotate((Float) (angle * 180 / Elastos::Core::Math::PI), x, y);
    mReusableOvalRect->Set(x - minor / 2, y - major / 2, x + minor / 2, y + major / 2);
    canvas->DrawOval(mReusableOvalRect, paint);
    canvas->Restore();
}

void PointerLocationView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 w = 0;
    GetWidth(&w);
    Int32 itemW = w/7;
    Int32 ascent = 0;
    mTextMetrics->GetAscent(&ascent);
    Int32 base = -ascent + 1;
    Int32 bottom = mHeaderBottom;

    Int32 NP = 0;
    mPointers->GetSize(&NP);

    // Labels
    if (mActivePointerId >= 0) {
        AutoPtr<IInterface> node;
        mPointers->Get(mActivePointerId, (IInterface**)&node);
        AutoPtr<PointerState> ps = (PointerState*)(IObject*)node.Get();

        canvas->DrawRect(0, 0, itemW - 1, bottom, mTextBackgroundPaint);
        mText->Clear();
        mText->Append(String("P: "));
        mText->Append(mCurNumPointers);
        mText->Append(String(" / "));
        mText->Append(mMaxNumPointers);
        canvas->DrawText(mText->ToString(), 1, base, mTextPaint);

        Int32 N = ps->mTraceCount;
        if ((mCurDown && ps->mCurDown) || N == 0) {
            Float x, y;
            ps->mCoords->GetX(&x);
            ps->mCoords->GetY(&y);
            canvas->DrawRect(itemW, 0, (itemW * 2) - 1, bottom, mTextBackgroundPaint);
            mText->Clear();
            mText->Append(String("X: "));
            mText->Append(x, 1);
            canvas->DrawText(mText->ToString(), 1 + itemW, base, mTextPaint);
            canvas->DrawRect(itemW * 2, 0, (itemW * 3) - 1, bottom, mTextBackgroundPaint);
            mText->Clear();
            mText->Append(String("Y: "));
            mText->Append(y, 1);
            canvas->DrawText(mText->ToString(), 1 + itemW * 2, base, mTextPaint);
        }
        else {
            using Elastos::Core::Math;
            Float dx = (*(ps->mTraceX))[N - 1] - (*(ps->mTraceX))[0];
            Float dy = (*(ps->mTraceY))[N - 1] - (*(ps->mTraceY))[0];
            Int32 touchTop = 0;
            mVC->GetScaledTouchSlop(&touchTop);
            canvas->DrawRect(itemW, 0, (itemW * 2) - 1, bottom,
                Math::Abs(dx) < touchTop ? mTextBackgroundPaint : mTextLevelPaint);
            mText->Clear();
            mText->Append(String("dX: "));
            mText->Append(dx, 1);
            canvas->DrawText(mText->ToString(), 1 + itemW, base, mTextPaint);
            canvas->DrawRect(itemW * 2, 0, (itemW * 3) - 1, bottom,
                Math::Abs(dy) < touchTop ? mTextBackgroundPaint : mTextLevelPaint);
            mText->Clear();
            mText->Append(String("dY: "));
            mText->Append(dy, 1);
            canvas->DrawText(mText->ToString(), 1 + itemW * 2, base, mTextPaint);
        }

        canvas->DrawRect(itemW * 3, 0, (itemW * 4) - 1, bottom, mTextBackgroundPaint);
        mText->Clear();
        mText->Append(String("Xv: "));
        mText->Append(ps->mXVelocity, 3);
        canvas->DrawText(mText->ToString(), 1 + itemW * 3, base, mTextPaint);

        canvas->DrawRect(itemW * 4, 0, (itemW * 5) - 1, bottom, mTextBackgroundPaint);
        mText->Clear();
        mText->Append(String("Yv: "));
        mText->Append(ps->mYVelocity, 3);
        canvas->DrawText(mText->ToString(), 1 + itemW * 4, base, mTextPaint);

        canvas->DrawRect(itemW * 5, 0, (itemW * 6) - 1, bottom, mTextBackgroundPaint);
        Float pressure = 0.0f, size = 0.0f;
        ps->mCoords->GetPressure(&pressure);
        ps->mCoords->GetSize(&size);
        canvas->DrawRect(itemW * 5, 0, (itemW * 5) + (pressure * itemW) - 1,
            bottom, mTextLevelPaint);
        mText->Clear();
        mText->Append(String("Prs: "));
        mText->Append(pressure, 2);
        canvas->DrawText(mText->ToString(), 1 + itemW * 5, base, mTextPaint);

        canvas->DrawRect(itemW * 6, 0, w, bottom, mTextBackgroundPaint);
        canvas->DrawRect(itemW * 6, 0, (itemW * 6) + (size * itemW) - 1,
            bottom, mTextLevelPaint);
        mText->Clear();
        mText->Append(String("Size: "));
        mText->Append(size, 2);
        canvas->DrawText(mText->ToString(), 1 + itemW * 6, base, mTextPaint);
    }

    // Pointer trace.
    for (Int32 p = 0; p < NP; p++) {
        AutoPtr<IInterface> node;
        mPointers->Get(p, (IInterface**)&node);
        AutoPtr<PointerState> ps = (PointerState*)(IObject*)node.Get();

        // Draw path.
        Int32 N = ps->mTraceCount;
        Float lastX = 0, lastY = 0;
        Boolean haveLast = FALSE;
        Boolean drawn = FALSE;
        mPaint->SetARGB(255, 128, 255, 255);
        for (Int32 i = 0; i < N; i++) {
            Float x = (*(ps->mTraceX))[i];
            Float y = (*(ps->mTraceY))[i];
            if (Elastos::Core::Math::IsNaN(x)) {
                haveLast = FALSE;
                continue;
            }
            if (haveLast) {
                canvas->DrawLine(lastX, lastY, x, y, mPathPaint);
                AutoPtr<IPaint> paint = (*(ps->mTraceCurrent))[i] ? mCurrentPointPaint : mPaint;
                canvas->DrawPoint(lastX, lastY, paint);
                drawn = TRUE;
            }
            lastX = x;
            lastY = y;
            haveLast = TRUE;
        }

        if (drawn) {
            // Draw movement estimate curve.
            mPaint->SetARGB(128, 128, 0, 128);
            Float lx = 0.0f;
            ps->mEstimator->EstimateX(-ESTIMATE_PAST_POINTS * ESTIMATE_INTERVAL, &lx);
            Float ly = 0.0f;
            ps->mEstimator->EstimateY(-ESTIMATE_PAST_POINTS * ESTIMATE_INTERVAL, &ly);
            for (Int32 i = -ESTIMATE_PAST_POINTS + 1; i <= ESTIMATE_FUTURE_POINTS; i++) {
                Float x = 0.0f;
                ps->mEstimator->EstimateX(i * ESTIMATE_INTERVAL, &x);
                Float y = 0.0f;
                ps->mEstimator->EstimateY(i * ESTIMATE_INTERVAL, &y);
                canvas->DrawLine(lx, ly, x, y, mPaint);
                lx = x;
                ly = y;
            }

            // Draw velocity vector.
            mPaint->SetARGB(255, 255, 64, 128);
            Float xVel = ps->mXVelocity * (1000 / 60);
            Float yVel = ps->mYVelocity * (1000 / 60);
            canvas->DrawLine(lastX, lastY, lastX + xVel, lastY + yVel, mPaint);

            // Draw alternate estimate.
            if (mAltVelocity != NULL) {
                mPaint->SetARGB(128, 0, 128, 128);
                ps->mAltEstimator->EstimateX(-ESTIMATE_PAST_POINTS * ESTIMATE_INTERVAL, &lx);
                ps->mAltEstimator->EstimateY(-ESTIMATE_PAST_POINTS * ESTIMATE_INTERVAL, &ly);
                for (Int32 i = -ESTIMATE_PAST_POINTS + 1; i <= ESTIMATE_FUTURE_POINTS; i++) {
                    Float x = 0.0f;
                    ps->mAltEstimator->EstimateX(i * ESTIMATE_INTERVAL, &x);
                    Float y = 0.0f;
                    ps->mAltEstimator->EstimateY(i * ESTIMATE_INTERVAL, &y);
                    canvas->DrawLine(lx, ly, x, y, mPaint);
                    lx = x;
                    ly = y;
                }

                mPaint->SetARGB(255, 64, 255, 128);
                xVel = ps->mAltXVelocity * (1000 / 60);
                yVel = ps->mAltYVelocity * (1000 / 60);
                canvas->DrawLine(lastX, lastY, lastX + xVel, lastY + yVel, mPaint);
            }
        }

        if (mCurDown && ps->mCurDown) {
            using Elastos::Core::Math;
            Float x = 0.0f, y = 0.0f;
            Float orientation = 0.0f, pressure = 0.0f;
            Float touchMajor = 0.0f, touchMinor = 0.0f, toolMajor = 0.0f, toolMinor = 0.0f;
            ps->mCoords->GetX(&x);
            ps->mCoords->GetY(&y);
            ps->mCoords->GetOrientation(&orientation);
            ps->mCoords->GetPressure(&pressure);
            ps->mCoords->GetTouchMajor(&touchMajor);
            ps->mCoords->GetTouchMinor(&touchMinor);
            ps->mCoords->GetToolMajor(&toolMajor);
            ps->mCoords->GetToolMinor(&toolMinor);
            // Draw crosshairs.
            Int32 w = 0, h = 0;
            GetWidth(&w);
            GetHeight(&h);
            canvas->DrawLine(0, y, w, y, mTargetPaint);
            canvas->DrawLine(x, 0, x, h, mTargetPaint);

            // Draw current point.
            Int32 pressureLevel = (Int32)(pressure * 255);
            mPaint->SetARGB(255, pressureLevel, 255, 255 - pressureLevel);
            canvas->DrawPoint(x, y, mPaint);

            // Draw current touch ellipse.
            mPaint->SetARGB(255, pressureLevel, 255 - pressureLevel, 128);
            DrawOval(canvas, x, y, touchMajor, touchMinor, orientation, mPaint);

            // Draw current tool ellipse.
            mPaint->SetARGB(255, pressureLevel, 128, 255 - pressureLevel);
            DrawOval(canvas, x, y, toolMajor, toolMinor, orientation, mPaint);

            // Draw the orientation arrow.
            Float arrowSize = toolMajor * 0.7f;
            if (arrowSize < 20) {
                arrowSize = 20;
            }
            mPaint->SetARGB(255, pressureLevel, 255, 0);
            Float orientationVectorX = (Float) (Elastos::Core::Math::Sin(orientation) * arrowSize);
            Float orientationVectorY = (Float) (-Elastos::Core::Math::Cos(orientation) * arrowSize);
            if (ps->mToolType == IMotionEvent::TOOL_TYPE_STYLUS
                    || ps->mToolType == IMotionEvent::TOOL_TYPE_ERASER) {
                // Show full circle orientation.
                canvas->DrawLine(x, y,
                    x + orientationVectorX,
                    y + orientationVectorY,
                    mPaint);
            }
            else {
                // Show half circle orientation.
                canvas->DrawLine(
                    x - orientationVectorX,
                    y - orientationVectorY,
                    x + orientationVectorX,
                    y + orientationVectorY,
                    mPaint);
            }

            // Draw the tilt point along the orientation arrow.
            Float axis = 0.0f;
            ps->mCoords->GetAxisValue(IMotionEvent::AXIS_TILT, &axis);
            Float tiltScale = (Float) Elastos::Core::Math::Sin(axis);
            canvas->DrawCircle(
                x + orientationVectorX * tiltScale,
                y + orientationVectorY * tiltScale,
                3.0f, mPaint);

            // Draw the current bounding box
            if (ps->mHasBoundingBox) {
                canvas->DrawRect(ps->mBoundingLeft, ps->mBoundingTop,
                        ps->mBoundingRight, ps->mBoundingBottom, mPaint);
            }
        }
    }
}

void PointerLocationView::LogMotionEvent(
    /* [in] */ const String& type,
    /* [in] */ IMotionEvent* event)
{
    Int32 action = 0;
    event->GetAction(&action);
    Int32 N = 0;
    event->GetHistorySize(&N);
    Int32 NI = 0;
    event->GetPointerCount(&NI);
    for (Int32 historyPos = 0; historyPos < N; historyPos++) {
        for (Int32 i = 0; i < NI; i++) {
            Int32 id = 0;
            event->GetPointerId(i, &id);
            event->GetHistoricalPointerCoords(i, historyPos, mTempCoords);
            LogCoords(type, action, i, mTempCoords, id, event);
        }
    }
    for (Int32 i = 0; i < NI; i++) {
        Int32 id = 0;
        event->GetPointerId(i, &id);
        event->GetPointerCoords(i, mTempCoords);
        LogCoords(type, action, i, mTempCoords, id, event);
    }
}

void PointerLocationView::LogCoords(
    /* [in] */ const String& type,
    /* [in] */ Int32 action,
    /* [in] */ Int32 index,
    /* [in] */ IPointerCoords* coords,
    /* [in] */ Int32 id,
    /* [in] */ IMotionEvent* event)
{
    Int32 toolType = 0;
    event->GetToolType(index, &toolType);
    Int32 buttonState = 0;
    event->GetButtonState(&buttonState);
    String prefix;
    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN:
            prefix = String("DOWN");
            break;
        case IMotionEvent::ACTION_UP:
            prefix = String("UP");
            break;
        case IMotionEvent::ACTION_MOVE:
            prefix = String("MOVE");
            break;
        case IMotionEvent::ACTION_CANCEL:
            prefix = String("CANCEL");
            break;
        case IMotionEvent::ACTION_OUTSIDE:
            prefix = String("OUTSIDE");
            break;
        case IMotionEvent::ACTION_POINTER_DOWN:
            if (index == ((action & IMotionEvent::ACTION_POINTER_INDEX_MASK)
                    >> IMotionEvent::ACTION_POINTER_INDEX_SHIFT)) {
                prefix = String("DOWN");
            }
            else {
                prefix = String("MOVE");
            }
            break;
        case IMotionEvent::ACTION_POINTER_UP:
            if (index == ((action & IMotionEvent::ACTION_POINTER_INDEX_MASK)
                    >> IMotionEvent::ACTION_POINTER_INDEX_SHIFT)) {
                prefix = String("UP");
            }
            else {
                prefix = String("MOVE");
            }
            break;
        case IMotionEvent::ACTION_HOVER_MOVE:
            prefix = String("HOVER MOVE");
            break;
        case IMotionEvent::ACTION_HOVER_ENTER:
            prefix = String("HOVER ENTER");
            break;
        case IMotionEvent::ACTION_HOVER_EXIT:
            prefix = String("HOVER EXIT");
            break;
        case IMotionEvent::ACTION_SCROLL:
            prefix = String("SCROLL");
            break;
        default:
            prefix = StringUtils::ToString(action);
            break;
    }

    using Elastos::Core::Math;
    Float valueTemp, orientation;
    coords->GetOrientation(&orientation);
    SLOGGERI(TAG, mText->Clear()
        ->Append(type)->Append(String(" id "))->Append(id + 1)
        ->Append(String(": "))
        ->Append(prefix)
        ->Append(String(" ("))->Append((coords->GetX(&valueTemp), valueTemp), 3)->Append(String(", "))
        ->Append((coords->GetY(&valueTemp), valueTemp), 3)
        ->Append(String(") Pressure="))->Append((coords->GetPressure(&valueTemp), valueTemp), 3)
        ->Append(String(" Size="))->Append((coords->GetSize(&valueTemp), valueTemp), 3)
        ->Append(String(" TouchMajor="))->Append((coords->GetTouchMajor(&valueTemp), valueTemp), 3)
        ->Append(String(" TouchMinor="))->Append((coords->GetTouchMinor(&valueTemp), valueTemp), 3)
        ->Append(String(" ToolMajor="))->Append((coords->GetToolMajor(&valueTemp), valueTemp), 3)
        ->Append(String(" ToolMinor="))->Append((coords->GetToolMinor(&valueTemp), valueTemp), 3)
        ->Append(String(" Orientation="))->Append((Float)(orientation * 180 / Math::PI), 1)
        ->Append(String("deg"))
        ->Append(String(" Tilt="))->Append(
            (Float)((coords->GetAxisValue(IMotionEvent::AXIS_TILT, &valueTemp), valueTemp) * 180 / Math::PI), 1)
        ->Append(String("deg"))
        ->Append(String(" Distance="))->Append((coords->GetAxisValue(IMotionEvent::AXIS_DISTANCE, &valueTemp), valueTemp), 1)
        ->Append(String(" VScroll="))->Append((coords->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &valueTemp), valueTemp), 1)
        ->Append(String(" HScroll="))->Append((coords->GetAxisValue(IMotionEvent::AXIS_HSCROLL, &valueTemp), valueTemp), 1)
        ->Append(String(" BoundingBox=[("))
        ->Append((event->GetAxisValue(IMotionEvent::AXIS_GENERIC_1, &valueTemp), valueTemp), 3)
        ->Append(String(", "))->Append((event->GetAxisValue(IMotionEvent::AXIS_GENERIC_2, &valueTemp), valueTemp), 3)->Append(String(")"))
        ->Append(String(", ("))->Append((event->GetAxisValue(IMotionEvent::AXIS_GENERIC_3, &valueTemp), valueTemp), 3)
        ->Append(String(", "))->Append((event->GetAxisValue(IMotionEvent::AXIS_GENERIC_4, &valueTemp), valueTemp), 3)
        ->Append(String(")]"))
        ->Append(String(" ToolType="))->Append(CMotionEvent::ToolTypeToString(toolType))
        ->Append(String(" ButtonState="))->Append(CMotionEvent::ButtonStateToString(buttonState))
        ->ToString());
}


ECode PointerLocationView::OnPointerEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 action = 0;
    event->GetAction(&action);
    Int32 NP = 0;
    mPointers->GetSize(&NP);

    if (action == IMotionEvent::ACTION_DOWN
            || (action & IMotionEvent::ACTION_MASK) == IMotionEvent::ACTION_POINTER_DOWN) {
        Int32 index = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK)
                >> IMotionEvent::ACTION_POINTER_INDEX_SHIFT; // will be 0 for down
        if (action == IMotionEvent::ACTION_DOWN) {
            for (Int32 p = 0; p < NP; p++) {
                AutoPtr<IInterface> node;
                mPointers->Get(p, (IInterface**)&node);
                AutoPtr<PointerState> ps = (PointerState*)(IObject*)node.Get();
                ps->ClearTrace();
                ps->mCurDown = FALSE;
            }
            mCurDown = TRUE;
            mCurNumPointers = 0;
            mMaxNumPointers = 0;
            mVelocity->Clear();
            if (mAltVelocity != NULL) {
                mAltVelocity->Clear();
            }
        }

        mCurNumPointers += 1;
        if (mMaxNumPointers < mCurNumPointers) {
            mMaxNumPointers = mCurNumPointers;
        }

        Int32 id = 0;
        event->GetPointerId(index, &id);
        while (NP <= id) {
            AutoPtr<PointerState> ps = new PointerState();
            mPointers->Add((IObject*)ps.Get());
            NP++;
        }

        AutoPtr<IInterface> p;
        mPointers->Get(mActivePointerId, (IInterface**)&p);
        AutoPtr<PointerState> cp = (PointerState*)(IObject*)p.Get();
        if (mActivePointerId < 0 ||
                !cp->mCurDown) {
            mActivePointerId = id;
        }

        AutoPtr<IInterface> pID;
        mPointers->Get(id, (IInterface**)&pID);
        AutoPtr<PointerState> ps = (PointerState*)(IObject*)pID.Get();
        ps->mCurDown = TRUE;
        AutoPtr<IInputDeviceHelper> hlp;
//         CInputDeviceHelper::AcquireSingleton((IInputDeviceHelper**)&hlp);
        Int32 Did = 0;
        IInputEvent::Probe(event)->GetDeviceId(&Did);
        AutoPtr<IInputDevice> device;
        hlp->GetDevice(Did, (IInputDevice**)&device);
        AutoPtr<IMotionRange> mr;
        ps->mHasBoundingBox = device != NULL &&
                (device->GetMotionRange(IMotionEvent::AXIS_GENERIC_1, (IMotionRange**)&mr), mr) != NULL;
    }

    Int32 NI = 0;
    event->GetPointerCount(&NI);

    mVelocity->AddMovement(event);
    mVelocity->ComputeCurrentVelocity(1);
    if (mAltVelocity != NULL) {
        mAltVelocity->AddMovement(event);
        mAltVelocity->ComputeCurrentVelocity(1);
    }

    Int32 N = 0;
    event->GetHistorySize(&N);
    for (Int32 historyPos = 0; historyPos < N; historyPos++) {
        for (Int32 i = 0; i < NI; i++) {
            Int32 id = 0;
            event->GetPointerId(i, &id);
            AutoPtr<IInterface> pPs;
            mPointers->Get(id, (IInterface**)&pPs);
            AutoPtr<PointerState> ps = mCurDown ? (PointerState*)(IObject*)pPs.Get() : NULL;
            AutoPtr<IPointerCoords> coords = ps != NULL ? ps->mCoords : mTempCoords;
            event->GetHistoricalPointerCoords(i, historyPos, coords);
            if (mPrintCoords) {
                LogCoords(String("Pointer"), action, i, coords, id, event);
            }
            if (ps != NULL) {
                Float x = 0.0f, y = 0.0f;
                coords->GetX(&x);
                coords->GetY(&y);
                ps->AddTrace(x, y, FALSE);
            }
        }
    }
    for (Int32 i = 0; i < NI; i++) {
        Int32 id = 0;
        event->GetPointerId(i, &id);
        AutoPtr<IInterface> pPs;
        mPointers->Get(id, (IInterface**)&pPs);
        AutoPtr<PointerState> ps = mCurDown ? (PointerState*)(IObject*)pPs.Get() : NULL;
        AutoPtr<IPointerCoords> coords = ps != NULL ? ps->mCoords : mTempCoords;
        event->GetPointerCoords(i, coords);
        if (mPrintCoords) {
            LogCoords(String("Pointer"), action, i, coords, id, event);
        }
        if (ps != NULL) {
            Float x = 0.0f, y = 0.0f;
            coords->GetX(&x);
            coords->GetY(&y);
            ps->AddTrace(x, y, TRUE);
            mVelocity->GetXVelocity(id, &(ps->mXVelocity));
            mVelocity->GetYVelocity(id, &(ps->mYVelocity));
            Boolean rst = FALSE;
            mVelocity->GetEstimator(id, ps->mEstimator, &rst);
            if (mAltVelocity != NULL) {
                mAltVelocity->GetXVelocity(id, &(ps->mAltXVelocity));
                mAltVelocity->GetYVelocity(id, &(ps->mAltYVelocity));
                mAltVelocity->GetEstimator(id, ps->mAltEstimator, &rst);
            }
            event->GetToolType(i, &(ps->mToolType));

            if (ps->mHasBoundingBox) {
                event->GetAxisValue(IMotionEvent::AXIS_GENERIC_1, i, &(ps->mBoundingLeft));
                event->GetAxisValue(IMotionEvent::AXIS_GENERIC_2, i, &(ps->mBoundingTop));
                event->GetAxisValue(IMotionEvent::AXIS_GENERIC_3, i, &(ps->mBoundingRight));
                event->GetAxisValue(IMotionEvent::AXIS_GENERIC_4, i, &(ps->mBoundingBottom));
            }
        }
    }

    if (action == IMotionEvent::ACTION_UP
            || action == IMotionEvent::ACTION_CANCEL
            || (action & IMotionEvent::ACTION_MASK) == IMotionEvent::ACTION_POINTER_UP) {
        Int32 index = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK)
                >> IMotionEvent::ACTION_POINTER_INDEX_SHIFT; // will be 0 for UP

        Int32 id = 0;
        event->GetPointerId(index, &id);
        AutoPtr<IInterface> pPs;
        mPointers->Get(id, (IInterface**)&pPs);
        AutoPtr<PointerState> ps = (PointerState*)(IObject*)pPs.Get();
        ps->mCurDown = FALSE;

        if (action == IMotionEvent::ACTION_UP
                || action == IMotionEvent::ACTION_CANCEL) {

            mCurDown = FALSE;
            mCurNumPointers = 0;
        }
        else {
            mCurNumPointers -= 1;
            if (mActivePointerId == id) {
                event->GetPointerId(index == 0 ? 1 : 0, &mActivePointerId);
            }
            ps->AddTrace(Elastos::Core::Math::FLOAT_NAN, Elastos::Core::Math::FLOAT_NAN, FALSE);
        }
    }

    Invalidate();
    return NOERROR;
}

ECode PointerLocationView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    OnPointerEvent(event);

    Int32 action = 0;
    event->GetAction(&action);
    Boolean bF = FALSE;
    if (action == IMotionEvent::ACTION_DOWN && !(IsFocused(&bF), bF)) {
        RequestFocus(res);
    }
    *res = TRUE;
    return NOERROR;
}

ECode PointerLocationView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 source = 0;
    IInputEvent::Probe(event)->GetSource(&source);
    if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        OnPointerEvent(event);
    }
    else if ((source & IInputDevice::SOURCE_CLASS_JOYSTICK) != 0) {
        LogMotionEvent(String("Joystick"), event);
    }
    else if ((source & IInputDevice::SOURCE_CLASS_POSITION) != 0) {
        LogMotionEvent(String("Position"), event);
    }
    else {
        LogMotionEvent(String("Generic"), event);
    }
    *res = TRUE;
    return NOERROR;
}

ECode PointerLocationView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    if (ShouldLogKey(keyCode)) {
        Int32 repeatCount = 0;
        event->GetRepeatCount(&repeatCount);
        if (repeatCount == 0) {
            SLOGGERI(TAG, String("Key Down: ") + ((CKeyEvent*)event)->ToString());
        }
        else {
            SLOGGERI(TAG, String("Key Repeat #") + StringUtils::ToString(repeatCount) + ": " + ((CKeyEvent*)event)->ToString());
        }
        *res = TRUE;
        return NOERROR;
    }
    return View::OnKeyDown(keyCode, event, res);
}

ECode PointerLocationView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    if (ShouldLogKey(keyCode)) {
        SLOGGERI(TAG, String("Key Up: ") +((CKeyEvent*)event)->ToString());
        *res = TRUE;
        return NOERROR;
    }
    return View::OnKeyUp(keyCode, event, res);
}

Boolean PointerLocationView::ShouldLogKey(
    /* [in] */ Int32 keyCode)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_UP:
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        case IKeyEvent::KEYCODE_DPAD_CENTER:
            return TRUE;
        default:
            return CKeyEvent::IsGamepadButton(keyCode)
                || CKeyEvent::IsModifierKey(keyCode);
    }
}

ECode PointerLocationView::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    LogMotionEvent(String("Trackball"), event);
    *res = TRUE;
    return NOERROR;
}

ECode PointerLocationView::OnAttachedToWindow()
{
    View::OnAttachedToWindow();

    AutoPtr<IHandler> h;
    GetHandler((IHandler**)&h);
    mIm->RegisterInputDeviceListener(THIS_PROBE(IInputDeviceListener), h);
    LogInputDevices();
    return NOERROR;
}

ECode PointerLocationView::OnDetachedFromWindow()
{
    View::OnDetachedFromWindow();

    mIm->UnregisterInputDeviceListener(THIS_PROBE(IInputDeviceListener));
    return NOERROR;
}

ECode PointerLocationView::OnInputDeviceAdded(
    /* [in] */ Int32 deviceId)
{
    LogInputDeviceState(deviceId, String("Device Added"));
    return NOERROR;
}

ECode PointerLocationView::OnInputDeviceChanged(
    /* [in] */ Int32 deviceId)
{
    LogInputDeviceState(deviceId, String("Device Changed"));
    return NOERROR;
}

ECode PointerLocationView::OnInputDeviceRemoved(
    /* [in] */ Int32 deviceId)
{
    LogInputDeviceState(deviceId, String("Device Removed"));
    return NOERROR;
}

void PointerLocationView::LogInputDevices()
{
    AutoPtr<IInputDeviceHelper> hlp;
//     CInputDeviceHelper::AcquireSingleton((IInputDeviceHelper**)&hlp);
    AutoPtr<ArrayOf<Int32> > deviceIds;
    hlp->GetDeviceIds((ArrayOf<Int32>**)&deviceIds);
    for (Int32 i = 0; i < deviceIds->GetLength(); i++) {
        LogInputDeviceState((*deviceIds)[i], String("Device Enumerated"));
    }
}

void PointerLocationView::LogInputDeviceState(
    /* [in] */ Int32 deviceId,
    /* [in] */ const String& state)
{
    AutoPtr<IInputDevice> device;
    mIm->GetInputDevice(deviceId, (IInputDevice**)&device);
    if (device != NULL) {
        String str;
        device->ToString(&str);
        SLOGGERI(TAG, state + ": " + str);
    }
    else {
        SLOGGERI(TAG, state + ": " +  StringUtils::ToString(deviceId));
    }
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
