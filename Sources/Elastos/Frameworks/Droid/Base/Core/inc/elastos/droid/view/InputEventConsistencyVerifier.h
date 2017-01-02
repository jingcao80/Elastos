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

#ifndef __ELASTOS_DROID_VIEW_INPUTEVENTCONSISTENCYVERIFIER_H__
#define __ELASTOS_DROID_VIEW_INPUTEVENTCONSISTENCYVERIFIER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Checks whether a sequence of input events is self-consistent.
 * Logs a description of each problem detected.
 * <p>
 * When a problem is detected, the event is tainted.  This mechanism prevents the same
 * error from being reported multiple times.
 * </p>
 *
 * @hide
 */
class InputEventConsistencyVerifier
    : public Object
    , public IInputEventConsistencyVerifier
{
private:
    static const Boolean IS_ENG_BUILD;

    static const String EVENT_TYPE_KEY;
    static const String EVENT_TYPE_TRACKBALL;
    static const String EVENT_TYPE_TOUCH;
    static const String EVENT_TYPE_GENERIC_MOTION;

    // The number of recent events to log when a problem is detected.
    // Can be set to 0 to disable logging recent events but the runtime overhead of
    // this feature is negligible on current hardware.
    static const Int32 RECENT_EVENTS_TO_LOG = 5;

private:
    class KeyState
        : public Object
    {
    public:
        KeyState();

        static CARAPI_(AutoPtr<KeyState>) Obtain(
            /* [in] */ Int32 deviceId,
            /* [in] */ Int32 source,
            /* [in] */ Int32 keyCode);

        CARAPI Recycle();

    private:
        static Object mRecycledListLock;
        static AutoPtr<KeyState> mRecycledList;

    public:
        AutoPtr<KeyState> mNext;
        Int32 mDeviceId;
        Int32 mSource;
        Int32 mKeyCode;
        Boolean mUnhandled;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an input consistency verifier.
     * @param caller The object to which the verifier is attached.
     * @param flags Flags to the verifier, or 0 if none.
     * @param logTag Tag for logging. If NULL defaults to the short class name.
     */
    InputEventConsistencyVerifier(
        /* [in] */ IInterface* caller,
        /* [in] */ Int32 flags,
        /* [in] */ const String& logTag = String(NULL));

    /**
     * Determines whether the instrumentation should be enabled.
     * @return True if it should be enabled.
     */
    static CARAPI_(Boolean) IsInstrumentationEnabled();

    /**
     * Resets the state of the input event consistency verifier.
     */
    CARAPI Reset();

    /**
     * Checks an arbitrary input event.
     * @param event The event.
     * @param nestingLevel The nesting level: 0 if called from the base class,
     * or 1 from a subclass.  If the event was already checked by this consistency verifier
     * at a higher nesting level, it will not be checked again.  Used to handle the situation
     * where a subclass dispatching method delegates to its superclass's dispatching method
     * and both dispatching methods call into the consistency verifier.
     */
    CARAPI OnInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 nestingLevel);

    /**
     * Checks a key event.
     * @param event The event.
     * @param nestingLevel The nesting level: 0 if called from the base class,
     * or 1 from a subclass.  If the event was already checked by this consistency verifier
     * at a higher nesting level, it will not be checked again.  Used to handle the situation
     * where a subclass dispatching method delegates to its superclass's dispatching method
     * and both dispatching methods call into the consistency verifier.
     */
    CARAPI OnKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 nestingLevel);

    /**
     * Checks a trackball event.
     * @param event The event.
     * @param nestingLevel The nesting level: 0 if called from the base class,
     * or 1 from a subclass.  If the event was already checked by this consistency verifier
     * at a higher nesting level, it will not be checked again.  Used to handle the situation
     * where a subclass dispatching method delegates to its superclass's dispatching method
     * and both dispatching methods call into the consistency verifier.
     */
    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 nestingLevel);

    /**
     * Checks a touch event.
     * @param event The event.
     * @param nestingLevel The nesting level: 0 if called from the base class,
     * or 1 from a subclass.  If the event was already checked by this consistency verifier
     * at a higher nesting level, it will not be checked again.  Used to handle the situation
     * where a subclass dispatching method delegates to its superclass's dispatching method
     * and both dispatching methods call into the consistency verifier.
     */
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 nestingLevel);

    /**
     * Checks a generic motion event.
     * @param event The event.
     * @param nestingLevel The nesting level: 0 if called from the base class,
     * or 1 from a subclass.  If the event was already checked by this consistency verifier
     * at a higher nesting level, it will not be checked again.  Used to handle the situation
     * where a subclass dispatching method delegates to its superclass's dispatching method
     * and both dispatching methods call into the consistency verifier.
     */
    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 nestingLevel);

    /**
     * Notifies the verifier that a given event was unhandled and the rest of the
     * trace for the event should be ignored.
     * This method should only be called if the event was previously checked by
     * the consistency verifier using {@link #onInputEvent} and other methods.
     * @param event The event.
     * @param nestingLevel The nesting level: 0 if called from the base class,
     * or 1 from a subclass.  If the event was already checked by this consistency verifier
     * at a higher nesting level, it will not be checked again.  Used to handle the situation
     * where a subclass dispatching method delegates to its superclass's dispatching method
     * and both dispatching methods call into the consistency verifier.
     */
    CARAPI OnUnhandledEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 nestingLevel);

private:
    CARAPI EnsureMetaStateIsNormalized(
        /* [in] */ Int32 metaState);

    CARAPI EnsurePointerCountIsOneForThisAction(
        /* [in] */ IMotionEvent* event);

    CARAPI EnsureHistorySizeIsZeroForThisAction(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Boolean) StartEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 nestingLevel,
        /* [in] */ const String& eventType);

    CARAPI FinishEvent();

    static CARAPI AppendEvent(
        /* [in] */ StringBuilder* message,
        /* [in] */ Int32 index,
        /* [in] */ IInputEvent* event,
        /* [in] */ Boolean unhandled);

    CARAPI Problem(
        /* [in] */ const String& message);

    CARAPI_(AutoPtr<KeyState>) FindKeyState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 source,
        /* [in] */ Int32 keyCode,
        /* [in] */ Boolean remove);

    CARAPI AddKeyState(
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 source,
        /* [in] */ Int32 keyCode);

private:
    // The object to which the verifier is attached.
    IInterface* mCaller; // weak-ref

    // Consistency verifier flags.
    Int32 mFlags;

    // Tag for logging which a client can set to help distinguish the output
    // from different verifiers since several can be active at the same time.
    // If not provided defaults to the simple class name.
    String mLogTag;

    // The most recently checked event and the nesting level at which it was checked.
    // This is only set when the verifier is called from a nesting level greater than 0
    // so that the verifier can detect when it has been asked to verify the same event twice.
    // It does not make sense to examine the contents of the last event since it may have
    // been recycled.
    Int32 mLastEventSeq;
    String mLastEventType;
    Int32 mLastNestingLevel;

    // Copy of the most recent events.
    AutoPtr<ArrayOf<IInputEvent*> > mRecentEvents;
    AutoPtr<ArrayOf<Boolean> > mRecentEventsUnhandled;
    Int32 mMostRecentEventIndex;

    // Current event and its type.
    AutoPtr<IInputEvent> mCurrentEvent;
    String mCurrentEventType;

    // Linked list of key state objects.
    AutoPtr<KeyState> mKeyStateList;

    // Current state of the trackball.
    Boolean mTrackballDown;
    Boolean mTrackballUnhandled;

    // Bitfield of pointer ids that are currently down.
    // Assumes that the largest possible pointer id is 31, which is potentially subject to change.
    // (See MAX_POINTER_ID in frameworks/base/include/ui/Input.h)
    Int32 mTouchEventStreamPointers;

    // The device id and source of the current stream of touch events.
    Int32 mTouchEventStreamDeviceId;
    Int32 mTouchEventStreamSource;

    // Set to TRUE when we discover that the touch event stream is inconsistent.
    // Reset on down or cancel.
    Boolean mTouchEventStreamIsTainted;

    // Set to TRUE if the touch event stream is partially unhandled.
    Boolean mTouchEventStreamUnhandled;

    // Set to TRUE if we received hover enter.
    Boolean mHoverEntered;

    // The current violation message.
    AutoPtr<StringBuilder> mViolationMessage;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTEVENTCONSISTENCYVERIFIER_H__
