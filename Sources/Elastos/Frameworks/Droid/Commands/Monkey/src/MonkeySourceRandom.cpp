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

#include "MonkeySourceRandom.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IKeyCharacterMapHelper;
using Elastos::Droid::View::CKeyCharacterMapHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobalHelper;
using Elastos::Droid::Hardware::Display::CDisplayManagerGlobalHelper;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Core::Math;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

/** Key events that move around the UI. */
static const AutoPtr<ArrayOf<Int32> > NAV_KEYS = ArrayOf<Int32>::Alloc(4);

/**
 * Key events that perform major navigation options (so shouldn't be sent
 * as much).
*/
static const AutoPtr<ArrayOf<Int32> > MAJOR_NAV_KEYS = ArrayOf<Int32>::Alloc(2);

/** Key events that perform system operations. */
static const AutoPtr<ArrayOf<Int32> > SYS_KEYS = ArrayOf<Int32>::Alloc(8);

/** Possible screen rotation degrees **/
static const AutoPtr<ArrayOf<Int32> > SCREEN_ROTATION_DEGREES = ArrayOf<Int32>::Alloc(4);

/** If a physical key exists? */
static const AutoPtr<ArrayOf<Boolean> > PHYSICAL_KEY_EXISTS = MonkeySourceRandom::InitStatics();

AutoPtr< ArrayOf<Boolean> > MonkeySourceRandom::InitStatics()
{
    AutoPtr<IKeyEventHelper> helper;
    ASSERT_SUCCEEDED(CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper));
    Int32 length = 0;
    helper->GetMaxKeyCode(&length);
    ++length;
    AutoPtr<ArrayOf<Boolean> > ret = ArrayOf<Boolean>::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        ret->Set(i, TRUE);
    }

    NAV_KEYS->Set(0, IKeyEvent::KEYCODE_DPAD_UP);
    NAV_KEYS->Set(1, IKeyEvent::KEYCODE_DPAD_DOWN);
    NAV_KEYS->Set(2, IKeyEvent::KEYCODE_DPAD_LEFT);
    NAV_KEYS->Set(3, IKeyEvent::KEYCODE_DPAD_RIGHT);

    MAJOR_NAV_KEYS->Set(0, IKeyEvent::KEYCODE_MENU);//KeyEvent.KEYCODE_SOFT_RIGHT
    MAJOR_NAV_KEYS->Set(1, IKeyEvent::KEYCODE_DPAD_CENTER);

    SYS_KEYS->Set(0, IKeyEvent::KEYCODE_HOME);
    SYS_KEYS->Set(1, IKeyEvent::KEYCODE_BACK);
    SYS_KEYS->Set(2, IKeyEvent::KEYCODE_CALL);
    SYS_KEYS->Set(3, IKeyEvent::KEYCODE_ENDCALL);
    SYS_KEYS->Set(4, IKeyEvent::KEYCODE_VOLUME_UP);
    SYS_KEYS->Set(5, IKeyEvent::KEYCODE_VOLUME_DOWN);
    SYS_KEYS->Set(6, IKeyEvent::KEYCODE_VOLUME_MUTE);
    SYS_KEYS->Set(7, IKeyEvent::KEYCODE_MUTE);

    AutoPtr<IKeyCharacterMapHelper> keyMapHelper;
    CKeyCharacterMapHelper::AcquireSingleton((IKeyCharacterMapHelper**)&keyMapHelper);
    // Only examine SYS_KEYS
    length = SYS_KEYS->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        Boolean hasKey;
        keyMapHelper->DeviceHasKey((*SYS_KEYS)[i], &hasKey);
        ret->Set((*SYS_KEYS)[i], hasKey);
    }

    SCREEN_ROTATION_DEGREES->Set(0, ISurface::ROTATION_0);
    SCREEN_ROTATION_DEGREES->Set(1, ISurface::ROTATION_90);
    SCREEN_ROTATION_DEGREES->Set(2, ISurface::ROTATION_180);
    SCREEN_ROTATION_DEGREES->Set(3, ISurface::ROTATION_270);
    return ret;
}

MonkeySourceRandom::MonkeySourceRandom()
    : mFactors(ArrayOf<Float>::Alloc(IMonkeySourceRandom::FACTORZ_COUNT))
    , mEventCount(0)
    , mVerbose(0)
    , mThrottle(0)
{}

MonkeySourceRandom::~MonkeySourceRandom()
{
}

void MonkeySourceRandom::GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String* pRet)
{
    AutoPtr<IKeyEventHelper> helper;
    ASSERT_SUCCEEDED(CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper));
    helper->KeyCodeToString(keycode, pRet);
}

void MonkeySourceRandom::GetKeyCode(
    /* [in] */ const String& keyName,
    /* [out] */ Int32 * pKeycode)
{
    AutoPtr<IKeyEventHelper> helper;
    ASSERT_SUCCEEDED(CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper));
    helper->KeyCodeFromString(keyName, pKeycode);
}

void MonkeySourceRandom::Init(
    /* [in] */ IRandom * pRandom,
    /* [in] */ IObjectContainer * mainApps,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle)
{
    // default values for random distributions
    // note, these are straight percentages, to match user input (cmd line args)
    // but they will be converted to 0..1 values before the main loop runs.
    mFactors->Set(IMonkeySourceRandom::FACTOR_TOUCH, 15.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_MOTION, 10.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_TRACKBALL, 15.0f);
    // Adjust the values if we want to enable rotation by default.
    mFactors->Set(IMonkeySourceRandom::FACTOR_ROTATION, 0.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_NAV, 25.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_MAJORNAV, 15.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_SYSOPS, 2.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_APPSWITCH, 2.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_FLIP, 1.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_ANYTHING, 13.0f);
    mFactors->Set(IMonkeySourceRandom::FACTOR_PINCHZOOM, 2.0f);
    mRandom = pRandom;
    mMainApps = new List<AutoPtr<IComponentName> >();
    AutoPtr<IObjectEnumerator> enumerator;
    mainApps->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;

    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> element;
        enumerator->Current((IInterface**)&element);
        mMainApps->PushBack(IComponentName::Probe(element));
    }

    CMonkeyEventQueue::New(pRandom, throttle, randomizeThrottle,
        (IMonkeyEventQueue**)&mQ);
}

Boolean MonkeySourceRandom::AdjustEventFactors()
{
    // go through all values and compute totals for user & default values
    Float userSum = 0.0f;
    Float defaultSum = 0.0f;
    Int32 defaultCount = 0;
    for (Int32 i = 0; i < IMonkeySourceRandom::FACTORZ_COUNT; ++i) {
        if ((*mFactors)[i] <= 0.0f) {   // user values are zero or negative
            userSum -= (*mFactors)[i];
        }
        else {
            defaultSum += (*mFactors)[i];
            ++defaultCount;
        }
    }

    // if the user request was > 100%, reject it
    if (userSum > 100.0f) {
        PFL_EX("** Event weights > 100%%");
        return FALSE;
    }

    // if the user specified all of the weights, then they need to be 100%
    if (defaultCount == 0 && (userSum < 99.9f || userSum > 100.1f)) {
        PFL_EX("** Event weights != 100%%");
        return FALSE;
    }

    // compute the adjustment necessary
    Float defaultsTarget = (100.0f - userSum);
    Float defaultsAdjustment = defaultsTarget / defaultSum;

    // fix all values, by adjusting defaults, or flipping user values back to >0
    for (Int32 i = 0; i < IMonkeySourceRandom::FACTORZ_COUNT; ++i) {
        if ((*mFactors)[i] <= 0.0f) {   // user values are zero or negative
            (*mFactors)[i] = -(*mFactors)[i];
        }
        else {
            (*mFactors)[i] *= defaultsAdjustment;
        }
    }

    // if verbose, show factors
    if (mVerbose > 0) {
        PFL_EX("// Event percentages:");
        for (Int32 i = 0; i < IMonkeySourceRandom::FACTORZ_COUNT; ++i) {
            PFL_EX("//   %d: %f%%", i, (*mFactors)[i]);
        }
    }

    if (!ValidateKeys()) {
        return FALSE;
    }

    // finally, normalize and convert to running sum
    Float sum = 0.0f;
    for (Int32 i = 0; i < IMonkeySourceRandom::FACTORZ_COUNT; ++i) {
        sum += (*mFactors)[i] / 100.0f;
        (*mFactors)[i] = sum;
    }
    return TRUE;
}

Boolean MonkeySourceRandom::ValidateKeyCategory(
    /* [in] */const String& catName,
    /* [in] */ ArrayOf<Int32> *keys,
    /* [in] */ Float factor)
{
    if (factor < 0.1f) {
        return TRUE;
    }

    Int32 length = keys->GetLength();
    for(Int32 i = 0; i < length; ++i) {
        if((*PHYSICAL_KEY_EXISTS)[(*keys)[i]]) {
            return TRUE;
        }
    }
    PFL_EX("** %s has no physical keys but with factor %f%%.",
        catName.string(), factor);
    return FALSE;
}

Boolean MonkeySourceRandom::ValidateKeys()
{
    return ValidateKeyCategory(
        String("NAV_KEYS"),
        NAV_KEYS,
        (*mFactors)[IMonkeySourceRandom::FACTOR_NAV]) &&
        ValidateKeyCategory(
        String("MAJOR_NAV_KEYS"),
        MAJOR_NAV_KEYS,
        (*mFactors)[IMonkeySourceRandom::FACTOR_MAJORNAV]) &&
        ValidateKeyCategory(
        String("SYS_KEYS"),
        SYS_KEYS,
        (*mFactors)[IMonkeySourceRandom::FACTOR_SYSOPS]);
}


ECode MonkeySourceRandom::SetFactors(
    /* [in] */ ArrayOf<Float> *factors)
{
    Int32 c = IMonkeySourceRandom::FACTORZ_COUNT;
    if (factors->GetLength() < c) {
        c = factors->GetLength();
    }

    for(Int32 i = 0; i < c; ++i) {
        mFactors->Set(i, (*factors)[i]);
    }
    return NOERROR;
}

ECode MonkeySourceRandom::SetFactors(
    /* [in] */ Int32 index,
    /* [in] */ Float v)
{
    mFactors->Set(index, v);
    return NOERROR;
}

void MonkeySourceRandom::GeneratePointerEvent(
    /* [in] */ IRandom *random,
    /* [in] */ Int32 gesture)
{
    AutoPtr<IDisplay> display ;
    AutoPtr<IDisplayManagerGlobal> global;
    AutoPtr<IDisplayManagerGlobalHelper> helper;
    CDisplayManagerGlobalHelper::AcquireSingleton((IDisplayManagerGlobalHelper**)&helper);
    helper->GetInstance((IDisplayManagerGlobal**)&global);
    if (global == NULL) {
        PFL_EX("***** Monkey Error: failed to get IDisplayManagerGlobal.");
        assert(global != NULL && "***** Monkey Error: failed to get IDisplayManagerGlobal.");
        return;
    }
    global->GetRealDisplay(IDisplay::DEFAULT_DISPLAY, (IDisplay**)&display);
    if (display == NULL) {
        PFL_EX("***** Monkey Error: failed to get default display.");
        assert(display != NULL && "***** Monkey Error: failed to get default display.");
        return;
    }

    AutoPtr<IPointF> p1 = RandomPoint(random, display);
    AutoPtr<IPointF> v1 = RandomVector(random);

    Int64  downAt = SystemClock::GetUptimeMillis();

    AutoPtr<IMonkeyTouchEvent> event;
    CMonkeyTouchEvent::New(IMotionEvent::ACTION_DOWN, (IMonkeyTouchEvent**)&event);
    event->SetDownTime(downAt);
    Float x = 0;
    Float y = 0;
    p1->GetX(&x);
    p1->GetY(&y);
    event->AddPointer(0, x, y);
    event->SetIntermediateNote(FALSE);
    mQ->AddLast(IMonkeyEvent::Probe(event));

    // sometimes we'll move during the touch
    if (gesture == GESTURE_DRAG) {
        Int32 count;
        random->NextInt32(10, &count);
        for (Int32 i = 0; i < count; ++i) {
            RandomWalk(random, display, p1, v1);
            event = NULL;
            CMonkeyTouchEvent::New(IMotionEvent::ACTION_MOVE, (IMonkeyTouchEvent**)&event);
            event->SetDownTime(downAt);
            p1->GetX(&x);
            p1->GetY(&y);
            event->AddPointer(0, x, y);
            event->SetIntermediateNote(TRUE);
            mQ->AddLast(IMonkeyEvent::Probe(event));
        }
    }
    else if(gesture == GESTURE_PINCH_OR_ZOOM) {
        AutoPtr<IPointF> p2 = RandomPoint(random, display);
        AutoPtr<IPointF> v2 = RandomVector(random);
        Float x2;
        Float y2;
        RandomWalk(random, display, p1, v1);
        event = NULL;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_POINTER_DOWN | (1 << IMotionEvent::ACTION_POINTER_INDEX_SHIFT),
         (IMonkeyTouchEvent**)&event);
            event->SetDownTime(downAt);
            p1->GetX(&x);
            p1->GetY(&y);
            event->AddPointer(0, x, y);
            p2->GetX(&x2);
            p2->GetY(&y2);
            event->AddPointer(1, x2, y2);
            event->SetIntermediateNote(TRUE);
            mQ->AddLast(IMonkeyEvent::Probe(event));

        Int32 count;
        random->NextInt32(10, &count);
        for (Int32 i = 0; i < count; ++i) {
            RandomWalk(random, display, p1, v1);
            RandomWalk(random, display, p2, v2);

            event = NULL;
            CMonkeyTouchEvent::New(IMotionEvent::ACTION_MOVE, (IMonkeyTouchEvent**)&event);
            event->SetDownTime(downAt);
            p1->GetX(&x);
            p1->GetY(&y);
            p2->GetX(&x2);
            p2->GetY(&y2);
            event->AddPointer(0, x, y);
            event->AddPointer(1, x2, y2);
            event->SetIntermediateNote(TRUE);
            mQ->AddLast(IMonkeyEvent::Probe(event));
        }

        RandomWalk(random, display, p1, v1);
        RandomWalk(random, display, p2, v2);
        event = NULL;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_POINTER_UP | (1 << IMotionEvent::ACTION_POINTER_INDEX_SHIFT),
         (IMonkeyTouchEvent**)&event);
            event->SetDownTime(downAt);
        p1->GetX(&x);
        p1->GetY(&y);
        p2->GetX(&x2);
        p2->GetY(&y2);
        event->AddPointer(0, x, y);
        event->AddPointer(1, x2, y2);
        event->SetIntermediateNote(TRUE);
        mQ->AddLast(IMonkeyEvent::Probe(event));
    }

    RandomWalk(random, display, p1, v1);
    event = NULL;
    CMonkeyTouchEvent::New(IMotionEvent::ACTION_UP, (IMonkeyTouchEvent**)&event);
    event->SetDownTime(downAt);
    p1->GetX(&x);
    p1->GetY(&y);
    event->AddPointer(0, x, y);
    event->SetIntermediateNote(FALSE);
    mQ->AddLast(IMonkeyEvent::Probe(event));
}

AutoPtr<IPointF> MonkeySourceRandom::RandomPoint(
    /* [in] */ IRandom* random,
    /* [in] */ IDisplay* display)
{
    AutoPtr<IPointF> point;
    Int32 width;
    Int32 heigth;
    display->GetWidth(&width);
    display->GetHeight(&heigth);
    random->NextInt32(width, &width);
    random->NextInt32(heigth, &heigth);
    CPointF::New(width, heigth, (IPointF**)&point);
    return point;
}

AutoPtr<IPointF> MonkeySourceRandom::RandomVector(
    /* [in] */ IRandom* random)
{
    AutoPtr<IPointF> point;
    Float nf1;
    Float nf2;
    random->NextFloat(&nf1);
    random->NextFloat(&nf2);
    CPointF::New((nf1 - 0.5f) * 50, (nf2 - 0.5f) * 50, (IPointF**)&point);
    return point;
}

void MonkeySourceRandom::RandomWalk(
    /* [in] */ IRandom* random,
    /* [in] */ IDisplay* display,
    /* [in] */ IPointF* point,
    /* [in] */ IPointF* vector)
{
    using Elastos::Core::Math;
    Float px, py;
    Float vx, vy;
    Float rdmx, rdmy;
    point->GetX(&px);
    point->GetY(&py);
    vector->GetX(&vx);
    vector->GetY(&vy);
    random->NextFloat(&rdmx);
    random->NextFloat(&rdmy);
    Int32 width, height;
    display->GetWidth(&width);
    display->GetHeight(&height);
    point->Set(Math::Max(Math::Min(px + rdmx * vx, (Float)width), 0.0f)
    , Math::Max(Math::Min(py + rdmy * vy, (Float)height), 0.0f));
}

void MonkeySourceRandom::GenerateTrackballEvent(
    /* [in] */ IRandom *random)
{
    for (Int32 i = 0; i < 10; ++i) {
        // generate a small random step
        Int32 dX;
        random->NextInt32(10, &dX);
        dX = dX - 5;
        Int32 dY;
        random->NextInt32(10, &dY);
        dY = dY - 5;

        AutoPtr<IMonkeyMotionEvent> event;
        CMonkeyTrackballEvent::New(IMotionEvent::ACTION_MOVE, (IMonkeyMotionEvent**)&event);
        event->AddPointer(0, dX, dY);
        event->SetIntermediateNote(i > 0);
        mQ->AddLast(IMonkeyEvent::Probe(event));
    }

    // 10% of trackball moves end with a click
    Int32 rdm;
    random->NextInt32(10, &rdm);
    if (0 == rdm) {
        Int64 downAt = SystemClock::GetUptimeMillis();
        AutoPtr<IMonkeyMotionEvent> event;
        CMonkeyTrackballEvent::New(IMotionEvent::ACTION_DOWN, (IMonkeyMotionEvent**)&event);
        event->SetDownTime(downAt);
        event->AddPointer(0, 0, 0);
        event->SetIntermediateNote(TRUE);
        mQ->AddLast(IMonkeyEvent::Probe(event));

        event = NULL;
        CMonkeyTrackballEvent::New(IMotionEvent::ACTION_UP, (IMonkeyMotionEvent**)&event);
        event->SetDownTime(downAt);
        event->AddPointer(0, 0, 0);
        event->SetIntermediateNote(FALSE);
        mQ->AddLast(IMonkeyEvent::Probe(event));
    }
}

void MonkeySourceRandom::GenerateRotationEvent(
        /* [in] */ IRandom *random)
{
    AutoPtr<IMonkeyRotationEvent> event;
    Int32 rdmInt32;
    Boolean rdmBool;
    random->NextInt32(SCREEN_ROTATION_DEGREES->GetLength(), &rdmInt32);
    random->NextBoolean(&rdmBool);
    CMonkeyRotationEvent::New((*SCREEN_ROTATION_DEGREES)[rdmInt32],
        rdmBool, (IMonkeyRotationEvent**)&event);
    mQ->AddLast(IMonkeyEvent::Probe(event));
}

void MonkeySourceRandom::GenerateEvents()
{
    Float cls;
    mRandom->NextFloat(&cls);
    Int32 lastKey = 0;

    if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_TOUCH]) {
        GeneratePointerEvent(mRandom, GESTURE_TAP);
        return;
    }
    else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_MOTION]) {
        GeneratePointerEvent(mRandom, GESTURE_DRAG);
        return;
    }
    else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_PINCHZOOM]) {
        GeneratePointerEvent(mRandom, GESTURE_PINCH_OR_ZOOM);
        return;
    }
    else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_TRACKBALL]) {
        GenerateTrackballEvent(mRandom);
        return;
    }
    else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_ROTATION]) {
        GenerateRotationEvent(mRandom);
        return;
    }

    // The remaining event categories are injected as key events
    for (;;) {
        if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_NAV]) {
            mRandom->NextInt32(NAV_KEYS->GetLength(), &lastKey);
            lastKey = (*NAV_KEYS)[lastKey];
        }
        else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_MAJORNAV]) {
            mRandom->NextInt32(MAJOR_NAV_KEYS->GetLength(), &lastKey);
            lastKey = (*MAJOR_NAV_KEYS)[lastKey];
        }
        else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_SYSOPS]) {
            mRandom->NextInt32(SYS_KEYS->GetLength(), &lastKey);
            lastKey = (*SYS_KEYS)[lastKey];
        }
        else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_APPSWITCH]) {
            AutoPtr<IMonkeyActivityEvent> e;
            Int32 size = mMainApps->GetSize();
            mRandom->NextInt32(size, &size);
            CMonkeyActivityEvent::New((*mMainApps)[size], (IMonkeyActivityEvent**)&e);
            mQ->AddLast(IMonkeyEvent::Probe(e));
            return;
        }
        else if (cls < (*mFactors)[IMonkeySourceRandom::FACTOR_FLIP]) {
            AutoPtr<IMonkeyFlipEvent> e;
            CMonkeyFlipEvent::New(mKeyboardOpen, (IMonkeyFlipEvent**)&e);
            mKeyboardOpen = !mKeyboardOpen;
            mQ->AddLast(IMonkeyEvent::Probe(e));
            return;
        }
        else {
            AutoPtr<IKeyEventHelper> helper;
            ASSERT_SUCCEEDED(CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper));
            Int32 length = 0;
            helper->GetMaxKeyCode(&length);
            mRandom->NextInt32(--length, &lastKey);
            ++lastKey;
        }

        if (lastKey != IKeyEvent::KEYCODE_POWER
                && lastKey != IKeyEvent::KEYCODE_ENDCALL
                && (*PHYSICAL_KEY_EXISTS)[lastKey])
        {
            break;
        }
    }

    AutoPtr<IMonkeyKeyEvent> e;
    CMonkeyKeyEvent::New(IKeyEvent::ACTION_DOWN, lastKey, (IMonkeyKeyEvent**)&e);
    mQ->AddLast(IMonkeyEvent::Probe(e));

    e = NULL;
    CMonkeyKeyEvent::New(IKeyEvent::ACTION_UP, lastKey, (IMonkeyKeyEvent**)&e);
    mQ->AddLast(IMonkeyEvent::Probe(e));
}

ECode MonkeySourceRandom::SetVerbose(
        /* [in] */ Int32 verbose)
{
    mVerbose = verbose;
    return NOERROR;
}

ECode MonkeySourceRandom::Validate(
    /* [out] */ Boolean *result)
{
    //check factors
    VALIDATE_NOT_NULL(result);
    *result = AdjustEventFactors();
    return NOERROR;
}

ECode MonkeySourceRandom::GenerateActivity()
{
    AutoPtr<IMonkeyActivityEvent> e;
    Int32 size = mMainApps->GetSize();
    mRandom->NextInt32(size, &size);
    CMonkeyActivityEvent::New((*mMainApps)[size], (IMonkeyActivityEvent**)&e);
    mQ->AddLast(IMonkeyEvent::Probe(e));
    return NOERROR;
}

ECode MonkeySourceRandom::GetNextEvent(
    /* [out] */IMonkeyEvent **event)
{
    VALIDATE_NOT_NULL(event);
    Boolean empty;
    mQ->IsEmpty(&empty);
    if (empty) {
        GenerateEvents();
    }
    ++mEventCount;
    mQ->GetFirst(event);
    mQ->RemoveFirst();
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
