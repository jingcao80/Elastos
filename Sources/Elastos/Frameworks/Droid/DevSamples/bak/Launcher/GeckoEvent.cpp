
#include "elastos/droid/ext/frameworkext.h"
#include "GeckoEvent.h"
#include "GeckoApp.h"
#include <elastos/core/Math.h>
#include "elastos/droid/os/Build.h"
#include <stdio.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

const Int32 GeckoEvent::INVALID = -1;
const Int32 GeckoEvent::NATIVE_POKE = 0;
const Int32 GeckoEvent::KEY_EVENT = 1;
const Int32 GeckoEvent::MOTION_EVENT = 2;
const Int32 GeckoEvent::ORIENTATION_EVENT = 3;
const Int32 GeckoEvent::ACCELERATION_EVENT = 4;
const Int32 GeckoEvent::LOCATION_EVENT = 5;
const Int32 GeckoEvent::IME_EVENT = 6;
const Int32 GeckoEvent::DRAW = 7;
const Int32 GeckoEvent::SIZE_CHANGED = 8;
const Int32 GeckoEvent::ACTIVITY_STOPPING = 9;
const Int32 GeckoEvent::ACTIVITY_PAUSING = 10;
const Int32 GeckoEvent::ACTIVITY_SHUTDOWN = 11;
const Int32 GeckoEvent::LOAD_URI = 12;
const Int32 GeckoEvent::SURFACE_CREATED = 13;
const Int32 GeckoEvent::SURFACE_DESTROYED = 14;
const Int32 GeckoEvent::GECKO_EVENT_SYNC = 15;
const Int32 GeckoEvent::ACTIVITY_START = 17;
const Int32 GeckoEvent::SAVE_STATE = 18;
const Int32 GeckoEvent::BROADCAST = 19;
const Int32 GeckoEvent::VIEWPORT = 20;
const Int32 GeckoEvent::VISITED = 21;
const Int32 GeckoEvent::NETWORK_CHANGED = 22;
const Int32 GeckoEvent::PROXIMITY_EVENT = 23;

const Int32 GeckoEvent::IME_COMPOSITION_END = 0;
const Int32 GeckoEvent::IME_COMPOSITION_BEGIN = 1;
const Int32 GeckoEvent::IME_SET_TEXT = 2;
const Int32 GeckoEvent::IME_GET_TEXT = 3;
const Int32 GeckoEvent::IME_DELETE_TEXT = 4;
const Int32 GeckoEvent::IME_SET_SELECTION = 5;
const Int32 GeckoEvent::IME_GET_SELECTION = 6;
const Int32 GeckoEvent::IME_ADD_RANGE = 7;

const Int32 GeckoEvent::IME_RANGE_CARETPOSITION = 1;
const Int32 GeckoEvent::IME_RANGE_RAWINPUT = 2;
const Int32 GeckoEvent::IME_RANGE_SELECTEDRAWTEXT = 3;
const Int32 GeckoEvent::IME_RANGE_CONVERTEDTEXT = 4;
const Int32 GeckoEvent::IME_RANGE_SELECTEDCONVERTEDTEXT = 5;

const Int32 GeckoEvent::IME_RANGE_UNDERLINE = 1;
const Int32 GeckoEvent::IME_RANGE_FORECOLOR = 2;
const Int32 GeckoEvent::IME_RANGE_BACKCOLOR = 4;

GeckoEvent::~GeckoEvent ()
{
}

GeckoEvent::GeckoEvent()
    : mType(NATIVE_POKE)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{}

GeckoEvent::GeckoEvent(
    /* [in] */ Int32 evType)
    : mType(evType)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{}

GeckoEvent::GeckoEvent(
    /* [in] */ IKeyEvent* k)
    : mType(KEY_EVENT)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{
    ASSERT_SUCCEEDED(k->GetAction(&mAction));
    ASSERT_SUCCEEDED(k->GetEventTime(&mTime));
    ASSERT_SUCCEEDED(k->GetMetaState(&mMetaState));
    ASSERT_SUCCEEDED(k->GetFlags(&mFlags));
    ASSERT_SUCCEEDED(k->GetKeyCode(&mKeyCode));
    ASSERT_SUCCEEDED(k->GetUnicodeChar(&mUnicodeChar));
    ASSERT_SUCCEEDED(k->GetCharacters(&mCharacters));
}

ECode GeckoEvent::AddMotionPoint(
    /* [in] */ Int32 index,
    /* [in] */ Int32 eventIndex,
    /* [in] */ IMotionEvent* event)
{
    Float x;
    Float y;
    event->GetX(eventIndex, &x);
    event->GetY(eventIndex, &y);
    AutoPtr<IPointF> geckoPoint;
    ASSERT_SUCCEEDED(CPointF::New(x, y, (IPointF**)&geckoPoint));


    ASSERT_SUCCEEDED(CPoint::New((Int32)Math::Round(x), (Int32)Math::Round(y), (IPoint**)&((*mPoints)[index])));

    FAIL_RETURN(event->GetPointerId(eventIndex, &((*mPointIndicies)[index])));
    // getToolMajor, getToolMinor and getOrientation are API Level 9 features
    if (Build::VERSION::SDK_INT >= 9) {
        Double radians;
        FAIL_RETURN(event->GetOrientation(eventIndex, (Float*)&radians));
        (*mOrientations)[index] = (Float) Math::ToDegrees(radians);
        // w3c touchevents spec does not allow orientations == 90
        // this shifts it to -90, which will be shifted to zero below
        if ((*mOrientations)[index] == 90)
            (*mOrientations)[index] = -90;

        // w3c touchevent radius are given by an orientation between 0 and 90
        // the radius is found by removing the orientation and measuring the x and y
        // radius of the resulting ellipse
        // for android orientations >= 0 and < 90, the major axis should correspond to
        // just reporting the y radius as the major one, and x as minor
        // however, for a radius < 0, we have to shift the orientation by adding 90, and
        // reverse which radius is major and minor
        if ((*mOrientations)[index] < 0) {
            (*mOrientations)[index] += 90;
            Float pointX;
            FAIL_RETURN(event->GetToolMajor(eventIndex, &pointX));
            Float pointY;
            FAIL_RETURN(event->GetToolMinor(eventIndex, &pointY));
            ASSERT_SUCCEEDED(CPoint::New((Int32)pointX/2, (Int32)pointY/2, (IPoint**)&((*mPointRadii)[index])));
        } else {
            Float pointX;
            FAIL_RETURN(event->GetToolMinor(eventIndex, &pointX));
            Float pointY;
            FAIL_RETURN(event->GetToolMajor(eventIndex, &pointY));
            ASSERT_SUCCEEDED(CPoint::New((Int32)pointX/2, (Int32)pointY/2, (IPoint**)&((*mPointRadii)[index])));
        }
    }
    else {
        Float size;
        FAIL_RETURN(event->GetSize(eventIndex, &size));
        AutoPtr<IDisplayMetrics> displaymetrics;
        ASSERT_SUCCEEDED(CDisplayMetrics::New((IDisplayMetrics**)&displaymetrics));

        AutoPtr<IWindowManager> mgr =
            GeckoApp::sAppContext->GetWindowManager();
        AutoPtr<IDisplay> display;
        FAIL_RETURN(mgr->GetDefaultDisplay((IDisplay**)&display));
        FAIL_RETURN(display->GetMetrics(displaymetrics));
        Int32 heightPixels;
        Int32 widthPixels;
        displaymetrics->GetWidthPixels(&widthPixels);
        displaymetrics->GetHeightPixels(&heightPixels);
        size = size * Math::Min(heightPixels, widthPixels);
        ASSERT_SUCCEEDED(CPoint::New((Int32)size,(Int32)size, (IPoint**)&((*mPointRadii)[index])));
        (*mOrientations)[index] = 0;
    }
    FAIL_RETURN(event->GetPressure(eventIndex, &((*mPressures)[index])));
    return NOERROR;
}

GeckoEvent::GeckoEvent(
    /* [in] */ IMotionEvent* m)
    : mType(MOTION_EVENT)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{
    ASSERT_SUCCEEDED(m->GetAction(&mAction));
    ASSERT_SUCCEEDED(m->GetEventTime(&mTime));
    ASSERT_SUCCEEDED(m->GetMetaState(&mMetaState));

    switch (mAction & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_POINTER_UP:
        case IMotionEvent::ACTION_POINTER_DOWN:
        case IMotionEvent::ACTION_DOWN:
        case IMotionEvent::ACTION_MOVE: {
            m->GetPointerCount(&mCount);
            mPoints = ArrayOf< AutoPtr<IPoint> >::Alloc(mCount);
            mPointIndicies = ArrayOf<Int32>::Alloc(mCount);
            mOrientations = ArrayOf<Float>::Alloc(mCount);
            mPressures = ArrayOf<Float>::Alloc(mCount);
            mPointRadii = ArrayOf< AutoPtr<IPoint> >::Alloc(mCount);
            mPointerIndex = (mAction & IMotionEvent::ACTION_POINTER_INDEX_MASK) >> IMotionEvent::ACTION_POINTER_INDEX_SHIFT;
            for (Int32 i = 0; i < mCount; i++) {
                ASSERT_SUCCEEDED(AddMotionPoint(i, i, m));
            }
            break;
        }
        default: {
            mCount = 0;
            mPointerIndex = -1;
            mPoints = ArrayOf< AutoPtr<IPoint> >::Alloc(mCount);
            mPointIndicies = ArrayOf<Int32>::Alloc(mCount);
            mOrientations = ArrayOf<Float>::Alloc(mCount);
            mPressures = ArrayOf<Float>::Alloc(mCount);
            mPointRadii = ArrayOf< AutoPtr<IPoint> >::Alloc(mCount);
        }
    }
}

/*
GeckoEvent::GeckoEvent(ISensorEvent* s) {
    AutoPtr<ISensor> sensor;
    //s->GetSensor((ISensor**)&sensor);
    Int32 sensor_type;
    sensor->GetType(&sensor_type);
    String name;
    sensor->getName(&name);

    switch(sensor_type) {
    case Sensor_TYPE_ACCELEROMETER:
        mType = ACCELERATION_EVENT;
        ArrayOf<Float>* values;
        //s->GetValues(&values);
        mX = (*values)[0];
        mY = (*values)[1];
        mZ = (*values)[2];
        break;

    case Sensor_TYPE_ORIENTATION:
        mType = ORIENTATION_EVENT;
        ArrayOf<Float>* values;
        //s->GetValues(&values);
        mAlpha = -(*values)[0];
        mBeta = -(*values)[1];
        mGamma = -(*values)[2];
        printf("GeckoEvent" "SensorEvent type = %d %s %d %d ", sensor_type, name , mAlpha, mBeta, mGamma );
        break;

    case Sensor_TYPE_PROXIMITY:
        mType = PROXIMITY_EVENT;
        ArrayOf<Float>* values;
        //s->GetValues(&values);
        mDistance = (*values)[0];
        printf("GeckoEvent" "SensorEvent type = %d %s %d", sensor_type, name, mDistance);
        break;
    }
}
*/


GeckoEvent::GeckoEvent(
    /* [in] */ ILocation* l,
    /* [in] */ IAddress* a)
    : mType(LOCATION_EVENT)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mLocation(l)
    , mAddress(a)
    , mNativeWindow(0)
{}

GeckoEvent::GeckoEvent(
    /* [in] */ Int32 imeAction,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
    : mType(IME_EVENT)
    , mAction(imeAction)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(offset)
    , mCount(count)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{}

GeckoEvent::GeckoEvent(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int32 rangeType,
    /* [in] */ Int32 rangeStyles,
    /* [in] */ Int32 rangeForeColor,
    /* [in] */ Int32 rangeBackColor,
    /* [in] */ const String& text)
    : mType(IME_EVENT)
    , mAction(IME_SET_TEXT)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(offset)
    , mCount(count)
    , mCharacters(text)
    , mRangeType(rangeType)
    , mRangeStyles(rangeStyles)
    , mRangeForeColor(rangeForeColor)
    , mRangeBackColor(rangeBackColor)
    , mNativeWindow(0)
{}

GeckoEvent::GeckoEvent(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int32 rangeType,
    /* [in] */ Int32 rangeStyles,
    /* [in] */ Int32 rangeForeColor,
    /* [in] */ Int32 rangeBackColor)
    : mType(IME_EVENT)
    , mAction(IME_ADD_RANGE)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(offset)
    , mCount(count)
    , mRangeType(rangeType)
    , mRangeStyles(rangeStyles)
    , mRangeForeColor(rangeForeColor)
    , mRangeBackColor(rangeBackColor)
    , mNativeWindow(0)
{}

GeckoEvent::GeckoEvent(
    /* [in] */ Int32 etype,
    /* [in] */ IRect* dirty)
    : mType(0)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{
    if (etype != DRAW) {
        mType = INVALID;
        return;
    }

    mType = etype;
    mRect = dirty;
}

GeckoEvent::GeckoEvent(
    /* [in] */ Int32 etype,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 screenw,
    /* [in] */ Int32 screenh)
    : mType(0)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{
    if (etype != SIZE_CHANGED) {
        mType = INVALID;
        return;
    }

    mType = etype;

    mPoints = ArrayOf< AutoPtr<IPoint> >::Alloc(3);
    CPoint::New(w, h, (IPoint**)&((*mPoints)[0]));
    CPoint::New(screenw, screenh, (IPoint**)&((*mPoints)[1]));
    CPoint::New(0, 0, (IPoint**)&((*mPoints)[2]));
}

GeckoEvent::GeckoEvent(
    /* [in] */ const String& subject,
    /* [in] */ const String& data)
{
    mType = BROADCAST;
    mCharacters = subject;
    mCharactersExtra = data;
}

GeckoEvent::GeckoEvent(
    /* [in] */ const String& uri)
    : mType(LOAD_URI)
    , mAction(0)
    , mTime(0)
    , mX(0)
    , mY(0)
    , mZ(0)
    , mAlpha(0)
    , mBeta(0)
    , mGamma(0)
    , mMetaState(0)
    , mFlags(0)
    , mKeyCode(0)
    , mUnicodeChar(0)
    , mOffset(0)
    , mCount(0)
    , mCharacters(uri)
    , mRangeType(0)
    , mRangeStyles(0)
    , mRangeForeColor(0)
    , mRangeBackColor(0)
    , mNativeWindow(0)
{}

GeckoEvent::GeckoEvent(
    /* [in] */ Double bandwidth,
    /* [in] */ Boolean canBeMetered)
{
    mType = NETWORK_CHANGED;
    mBandwidth = bandwidth;
    mCanBeMetered = canBeMetered;
}

UInt32 GeckoEvent::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GeckoEvent::Release()
{
    return ElRefBase::Release();
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
