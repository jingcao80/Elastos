
#ifndef __GECKOEVENT_H__
#define __GECKOEVENT_H__

#include "Elastos.Droid.Core.h"
#include <elautoptr.h>

using namespace Elastos;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Location::IAddress;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

class GeckoEvent : public ElRefBase
{
public:
    ~GeckoEvent();

    GeckoEvent();

    GeckoEvent(
        /* [in] */ Int32 evType);

    GeckoEvent(
        /* [in] */ IKeyEvent* k);

    CARAPI AddMotionPoint(
        /* [in] */ Int32 index,
        /* [in] */ Int32 eventIndex,
        /* [in] */ IMotionEvent* event);

    GeckoEvent(
        /* [in] */ IMotionEvent* m);

//    GeckoEvent(
//        /* [in] */ ISensorEvent* s);

    GeckoEvent(
        /* [in] */ ILocation* l,
        /* [in] */ IAddress* a);

    GeckoEvent(
        /* [in] */ Int32 imeAction,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    GeckoEvent(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Int32 rangeType,
        /* [in] */ Int32 rangeStyles,
        /* [in] */ Int32 rangeForeColor,
        /* [in] */ Int32 rangeBackColor,
        /* [in] */ const String& text);

    GeckoEvent(
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Int32 rangeType,
        /* [in] */ Int32 rangeStyles,
        /* [in] */ Int32 rangeForeColor,
        /* [in] */ Int32 rangeBackColor);

    GeckoEvent(
        /* [in] */ Int32 etype,
        /* [in] */ IRect * pDirty);

    GeckoEvent(
        /* [in] */ Int32 etype,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 screenw,
        /* [in] */ Int32 screenh);

    GeckoEvent(
        /* [in] */ const String&  subject,
        /* [in] */ const String&  data);

    GeckoEvent(
        /* [in] */ const String& uri);

    GeckoEvent(
        /* [in] */ Double bandwidth,
        /* [in] */ Boolean canBeMetered);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

public:
    const static Int32 INVALID;
    const static Int32 NATIVE_POKE;
    const static Int32 KEY_EVENT;
    const static Int32 MOTION_EVENT;
    const static Int32 ORIENTATION_EVENT;
    const static Int32 ACCELERATION_EVENT;
    const static Int32 LOCATION_EVENT;
    const static Int32 IME_EVENT;
    const static Int32 DRAW;
    const static Int32 SIZE_CHANGED;
    const static Int32 ACTIVITY_STOPPING;
    const static Int32 ACTIVITY_PAUSING;
    const static Int32 ACTIVITY_SHUTDOWN;
    const static Int32 LOAD_URI;
    const static Int32 SURFACE_CREATED;
    const static Int32 SURFACE_DESTROYED;
    const static Int32 GECKO_EVENT_SYNC;
    const static Int32 ACTIVITY_START;
    const static Int32 SAVE_STATE;
    const static Int32 BROADCAST;
    const static Int32 VIEWPORT;
    const static Int32 VISITED;
    const static Int32 NETWORK_CHANGED;
    const static Int32 PROXIMITY_EVENT;

    const static Int32 IME_COMPOSITION_END;
    const static Int32 IME_COMPOSITION_BEGIN;
    const static Int32 IME_SET_TEXT;
    const static Int32 IME_GET_TEXT;
    const static Int32 IME_DELETE_TEXT;
    const static Int32 IME_SET_SELECTION;
    const static Int32 IME_GET_SELECTION;
    const static Int32 IME_ADD_RANGE;

    const static Int32 IME_RANGE_CARETPOSITION;
    const static Int32 IME_RANGE_RAWINPUT;
    const static Int32 IME_RANGE_SELECTEDRAWTEXT;
    const static Int32 IME_RANGE_CONVERTEDTEXT;
    const static Int32 IME_RANGE_SELECTEDCONVERTEDTEXT;

    const static Int32 IME_RANGE_UNDERLINE;
    const static Int32 IME_RANGE_FORECOLOR;
    const static Int32 IME_RANGE_BACKCOLOR;

public:
    Int32 mType;
    Int32 mAction;
    Int64 mTime;
    AutoPtr<ArrayOf<IPoint*> > mPoints;
    AutoPtr<ArrayOf<Int32> > mPointIndicies;
    Int32 mPointerIndex;
    AutoPtr<ArrayOf<Float> > mOrientations;
    AutoPtr<ArrayOf<Float> > mPressures;
    AutoPtr<ArrayOf<IPoint*> > mPointRadii;
    Double mDistance;

    AutoPtr<IRect> mRect;
    Double mX, mY, mZ;
    Double mAlpha, mBeta, mGamma;

    Int32 mMetaState, mFlags;
    Int32 mKeyCode, mUnicodeChar;
    Int32 mOffset, mCount;
    String mCharacters, mCharactersExtra;
    Int32 mRangeType, mRangeStyles;
    Int32 mRangeForeColor, mRangeBackColor;
    AutoPtr<ILocation> mLocation;
    AutoPtr<IAddress>  mAddress;

    Double mBandwidth;
    Boolean mCanBeMetered;

    Int32 mNativeWindow;

};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __GECKOEVENT_H__

