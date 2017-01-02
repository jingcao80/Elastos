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

#ifndef __ELASTOS_DROID_VIEW_INPUTEVENT_H__
#define __ELASTOS_DROID_VIEW_INPUTEVENT_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace View {

class InputEvent
    : public Object
    , public IInputEvent
    , public IParcelable
{
protected:
    InputEvent();

public:
    CAR_INTERFACE_DECL()

    /**
     * Gets the device that this event came from.
     *
     * @return The device, or null if unknown.
     */
    CARAPI GetDevice(
        /* [out] */ IInputDevice** device);

    /**
     * Determines whether the event is from the given source.
     *
     * @param source The input source to check against. This can be a specific device type, such as
     * {@link InputDevice#SOURCE_TOUCH_NAVIGATION}, or a more generic device class, such as
     * {@link InputDevice#SOURCE_CLASS_POINTER}.
     * @return Whether the event is from the given source.
     */
    CARAPI IsFromSource(
        /* [in] */ Int32 source,
        /* [out] */ Boolean* result);


    /**
     * Recycles the event.
     * This method should only be used by the system since applications do not
     * expect {@link KeyEvent} objects to be recycled, although {@link MotionEvent}
     * objects are fine.  See {@link KeyEvent#recycle()} for details.
     * @hide
     */
    virtual CARAPI Recycle();

    /**
     * Conditionally recycled the event if it is appropriate to do so after
     * dispatching the event to an application.
     *
     * If the event is a {@link MotionEvent} then it is recycled.
     *
     * If the event is a {@link KeyEvent} then it is NOT recycled, because applications
     * expect key events to be immutable so once the event has been dispatched to
     * the application we can no longer recycle it.
     * @hide
     */
    virtual CARAPI RecycleIfNeededAfterDispatch();

    /**
     * Gets the unique sequence number of this event.
     * Every input event that is created or received by a process has a
     * unique sequence number.  Moreover, a new sequence number is obtained
     * each time an event object is recycled.
     *
     * Sequence numbers are only guaranteed to be locally unique within a process.
     * Sequence numbers are not preserved when events are parceled.
     *
     * @return The unique sequence number of this event.
     * @hide
     */
    virtual CARAPI GetSequenceNumber(
        /* [out] */ Int32* seq);

protected:
    virtual CARAPI_(void) PrepareForReuse();

protected:
    /** @hide */
    static const Int32 PARCEL_TOKEN_MOTION_EVENT = 1;
    /** @hide */
    static const Int32 PARCEL_TOKEN_KEY_EVENT = 2;

    Int32 mSeq;
    Boolean mRecycled;

private:
    static Int32 sNextSeq;
    static Object sNextSeqLock;
    static const Boolean TRACK_RECYCLED_LOCATION = FALSE;
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTEVENT_H__
