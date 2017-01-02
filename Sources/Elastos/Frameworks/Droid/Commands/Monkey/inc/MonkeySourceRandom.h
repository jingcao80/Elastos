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

#ifndef __MONKEYSOURCERANDOM_H__
#define __MONKEYSOURCERANDOM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "CMonkeyEventQueue.h"
#include "CMonkeyKeyEvent.h"
#include "CMonkeyTouchEvent.h"
#include "CMonkeyActivityEvent.h"
#include "CMonkeyRotationEvent.h"
#include "CMonkeyFlipEvent.h"
#include "CMonkeyTrackballEvent.h"
#include <elautoptr.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Core::IRandom;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeySourceRandom
{
public:
    static void GetKeyName(
        /* [in] */ Int32 keycode,
        /* [out] */ String *ret);

    static void GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode);

    static AutoPtr< ArrayOf<Boolean> > InitStatics();

    MonkeySourceRandom();
    ~MonkeySourceRandom();

    void Init(
    /* [in] */ IRandom *random,
    /* [in] */ IObjectContainer *mainApps,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle);


    CARAPI SetFactors(
        /* [in] */ ArrayOf<Float> *factors);

    CARAPI SetFactors(
    /* [in] */ Int32 index,
    /* [in] */ Float v);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);

    CARAPI GenerateActivity();

    CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

protected:
private:
    Boolean AdjustEventFactors();

    Boolean ValidateKeyCategory(
        /* [in] */const String& catName,
        /* [in] */ ArrayOf<Int32> *keys,
        /* [in] */ Float factor);

    Boolean ValidateKeys();

    void GeneratePointerEvent(
        /* [in] */ IRandom *random,
        /* [in] */ Int32 Gesture);

    AutoPtr<IPointF> RandomPoint(
        /* [in] */ IRandom *random,
        /* [in] */ IDisplay *display);

    AutoPtr<IPointF> RandomVector(
        /* [in] */ IRandom *random);

    void RandomWalk(
        /* [in] */ IRandom *random,
        /* [in] */ IDisplay *display,
        /* [in] */ IPointF *point,
        /* [in] */ IPointF *vector);

    void GenerateTrackballEvent(
        /* [in] */ IRandom *random);

    void GenerateRotationEvent(
        /* [in] */ IRandom *random);

    void GenerateEvents();

private:
    static const Int32 GESTURE_TAP = 0;
    static const Int32 GESTURE_DRAG = 1;
    static const Int32 GESTURE_PINCH_OR_ZOOM = 2;

    /** percentages for each type of event.  These will be remapped to working
     * values after we read any optional values.
     **/
    AutoPtr<ArrayOf<Float> > mFactors;
    AutoPtr<List<AutoPtr<IComponentName> > > mMainApps;
    Int32 mEventCount; //total number of events generated so far
    AutoPtr<IMonkeyEventQueue> mQ;
    AutoPtr<IRandom> mRandom;
    Int32 mVerbose;
    Int64 mThrottle;
    Boolean mKeyboardOpen;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYSOURCERANDOM_H__
