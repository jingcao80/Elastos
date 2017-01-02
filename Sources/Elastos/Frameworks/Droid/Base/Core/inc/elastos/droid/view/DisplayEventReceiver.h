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

#ifndef __ELASTOS_DROID_VIEW_DISPLAYEVENTRECEIVER_H__
#define __ELASTOS_DROID_VIEW_DISPLAYEVENTRECEIVER_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/view/NativeDisplayEventReceiver.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Provides a low-level mechanism for an application to receive display events
 * such as vertical sync.
 *
 * The display event receive is NOT thread safe.  Moreover, its methods must only
 * be called on the Looper thread to which it is attached.
 *
 * @hide
 */
class ECO_PUBLIC DisplayEventReceiver
    : public Object
    , public IDisplayEventReceiver
{
    friend class NativeDisplayEventReceiver;
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a display event receiver.
     *
     * @param looper The looper to use when invoking callbacks.
     */
    DisplayEventReceiver(
        /* [in] */ ILooper* looper);


    virtual ~DisplayEventReceiver();

    /**
     * Disposes the receiver.
     */
    virtual CARAPI Dispose();

    /**
     * Called when a vertical sync pulse is received.
     * The recipient should render a frame and then call {@link #scheduleVsync}
     * to schedule the next vertical sync pulse.
     *
     * @param timestampNanos The timestamp of the pulse, in the {@link System#nanoTime()}
     * timebase.
     * @param builtInDisplayId The surface flinger built-in display id such as
     * {@link Surface#BUILT_IN_DISPLAY_ID_MAIN}.
     * @param frame The frame number.  Increases by one for each vertical sync interval.
     */
    virtual CARAPI OnVsync(
        /* [in] */ Int64 timestampNanos,
        /* [in] */ Int32 builtInDisplayId,
        /* [in] */ Int32 frame);

    /**
     * Called when a display hotplug event is received.
     *
     * @param timestampNanos The timestamp of the event, in the {@link System#nanoTime()}
     * timebase.
     * @param builtInDisplayId The surface flinger built-in display id such as
     * {@link Surface#BUILT_IN_DISPLAY_ID_HDMI}.
     * @param connected True if the display is connected, false if it disconnected.
     */
    virtual CARAPI OnHotplug(
        /* [in] */ Int64 timestampNanos,
        /* [in] */ Int32 builtInDisplayId,
        /* [in] */ Boolean connected);

    /**
     * Schedules a single vertical sync pulse to be delivered when the next
     * display frame begins.
     */
    virtual CARAPI ScheduleVsync();

private:
    ECO_LOCAL CARAPI_(void) NativeInit();

    ECO_LOCAL CARAPI_(void) Dispose(
        /* [in] */ Boolean finalized);

    // Called from native code.
    //@SuppressWarnings("unused")
    ECO_LOCAL CARAPI_(void) DispatchVsync(
        /* [in] */ Int64 timestampNanos,
        /* [in] */ Int32 builtInDisplayId,
        /* [in] */ Int32 frame);

    // Called from native code.
    //@SuppressWarnings("unused")
    ECO_LOCAL CARAPI_(void) DispatchHotplug(
        /* [in] */ Int64 timestampNanos,
        /* [in] */ Int32 builtInDisplayId,
        /* [in] */ Boolean connected);

private:
    ECO_LOCAL static const String TAG;

    AutoPtr<ICloseGuard> mCloseGuard;// = CloseGuard.get();

    android::sp<NativeDisplayEventReceiver> mNativeReceiver;

    // We keep a reference message queue object here so that it is not
    // GC'd while the native peer of the receiver is using them.
    AutoPtr<IMessageQueue> mMessageQueue;
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_DISPLAYEVENTRECEIVER_H__
