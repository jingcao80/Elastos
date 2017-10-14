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

#ifndef __ELASTOS_DROID_VIEW_CHOREOGRAPHER_H__
#define __ELASTOS_DROID_VIEW_CHOREOGRAPHER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/DisplayEventReceiver.h"
#include "elastos/droid/os/Handler.h"
#include <pthread.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Coordinates the timing of animations, input and drawing.
 * <p>
 * The choreographer receives timing pulses (such as vertical synchronization)
 * from the display subsystem then schedules work to occur as part of rendering
 * the next display frame.
 * </p><p>
 * Applications typically interact with the choreographer indirectly using
 * higher level abstractions in the animation framework or the view hierarchy.
 * Here are some examples of things you can do using the higher-level APIs.
 * </p>
 * <ul>
 * <li>To post an animation to be processed on a regular time basis synchronized with
 * display frame rendering, use {@link android.animation.ValueAnimator#start}.</li>
 * <li>To post a {@link Runnable} to be invoked once at the beginning of the next display
 * frame, use {@link View#postOnAnimation}.</li>
 * <li>To post a {@link Runnable} to be invoked once at the beginning of the next display
 * frame after a delay, use {@link View#postOnAnimationDelayed}.</li>
 * <li>To post a call to {@link View#invalidate()} to occur once at the beginning of the
 * next display frame, use {@link View#postInvalidateOnAnimation()} or
 * {@link View#postInvalidateOnAnimation(Int32, Int32, Int32, Int32)}.</li>
 * <li>To ensure that the contents of a {@link View} scroll smoothly and are drawn in
 * sync with display frame rendering, do nothing.  This already happens automatically.
 * {@link View#onDraw} will be called at the appropriate time.</li>
 * </ul>
 * <p>
 * However, there are a few cases where you might want to use the functions of the
 * choreographer directly in your application.  Here are some examples.
 * </p>
 * <ul>
 * <li>If your application does its rendering in a different thread, possibly using GL,
 * or does not use the animation framework or view hierarchy at all
 * and you want to ensure that it is appropriately synchronized with the display, then use
 * {@link Choreographer#postFrameCallback}.</li>
 * <li>... and that's about it.</li>
 * </ul>
 * <p>
 * Each {@link Looper} thread has its own choreographer.  Other threads can
 * post callbacks to run on the choreographer but they will run on the {@link Looper}
 * to which the choreographer belongs.
 * </p>
 */
class Choreographer
    : public Object
    , public IChoreographer
{
private:
    class FrameHandler : public Handler
    {
    public:
        TO_STRING_IMPL("Choreographer::FrameHandler")

        FrameHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ Choreographer* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        Choreographer* mHost;
    };

    class Token
        : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* info);
    };

    class FrameDisplayEventReceiver
        : public DisplayEventReceiver
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        FrameDisplayEventReceiver(
            /* [in] */ ILooper* looper,
            /* [in] */ Choreographer* owner);

        //@Override
        CARAPI OnVsync(
            /* [in] */ Int64 timestampNanos,
            /* [in] */ Int32 builtInDisplayId,
            /* [in] */ Int32 frame);

        CARAPI Run();

    private:
        Boolean mHavePendingVsync;
        Int64 mTimestampNanos;
        Int32 mFrame;
        Choreographer* mOwner;
    };

    class CallbackRecord
        : public Object
    {
    public:
        CARAPI_(void) Run(
            /* [in] */ Int64 frameTimeNanos);

    public:
        AutoPtr<CallbackRecord> mNext;
        Int64 mDueTime;
        AutoPtr<IInterface> mAction; // Runnable or FrameCallback
        AutoPtr<IObject> mToken;
    };

    class CallbackQueue
        : public ElRefBase
    {
    public:
        CallbackQueue(
            /* [in] */ Choreographer* owner);

        CARAPI_(Boolean) HasDueCallbacksLocked(
            /* [in] */ Int64 now);

        CARAPI_(AutoPtr<CallbackRecord>) ExtractDueCallbacksLocked(
            /* [in] */ Int64 now);

        CARAPI_(void) AddCallbackLocked(
            /* [in] */ Int64 dueTime,
            /* [in] */ IInterface* action,
            /* [in] */ IObject* token);

        CARAPI_(void) RemoveCallbacksLocked(
            /* [in] */ IInterface* action,
            /* [in] */ IObject* token);

    private:
        AutoPtr<CallbackRecord> mHead;
        Choreographer* mOwner;
    };

public:
    /**
     * Gets the choreographer for the calling thread.  Must be called from
     * a thread that already has a {@link android.os.Looper} associated with it.
     *
     * @return The choreographer for this thread.
     * @throws IllegalStateException if the thread does not have a looper.
     */
    static CARAPI_(AutoPtr<Choreographer>) GetInstance();

    /**
     * The amount of time, in milliseconds, between each frame of the animation.
     * <p>
     * This is a requested time that the animation will attempt to honor, but the actual delay
     * between frames may be different, depending on system load and capabilities. This is a static
     * function because the same delay will be applied to all animations, since they are all
     * run off of a single timing loop.
     * </p><p>
     * The frame delay may be ignored when the animation system uses an external timing
     * source, such as the display refresh rate (vsync), to govern animations.
     * </p>
     *
     * @return the requested time between frames, in milliseconds
     * @hide
     */
    static CARAPI_(Int64) GetFrameDelay();

    /**
     * The amount of time, in milliseconds, between each frame of the animation.
     * <p>
     * This is a requested time that the animation will attempt to honor, but the actual delay
     * between frames may be different, depending on system load and capabilities. This is a static
     * function because the same delay will be applied to all animations, since they are all
     * run off of a single timing loop.
     * </p><p>
     * The frame delay may be ignored when the animation system uses an external timing
     * source, such as the display refresh rate (vsync), to govern animations.
     * </p>
     *
     * @param frameDelay the requested time between frames, in milliseconds
     * @hide
     */
    static CARAPI_(void) SetFrameDelay(
        /* [in] */ Int64 frameDelay);

    /**
     * Subtracts typical frame delay time from a delay interval in milliseconds.
     * <p>
     * This method can be used to compensate for animation delay times that have baked
     * in assumptions about the frame delay.  For example, it's quite common for code to
     * assume a 60Hz frame time and bake in a 16ms delay.  When we call
     * {@link #postAnimationCallbackDelayed} we want to know how Int64 to wait before
     * posting the animation callback but let the animation timer take care of the remaining
     * frame delay time.
     * </p><p>
     * This method is somewhat conservative about how much of the frame delay it
     * subtracts.  It uses the same value returned by {@link #getFrameDelay} which by
     * default is 10ms even though many parts of the system assume 16ms.  Consequently,
     * we might still wait 6ms before posting an animation callback that we want to run
     * on the next frame, but this is much better than waiting a whole 16ms and likely
     * missing the deadline.
     * </p>
     *
     * @param delayMillis The original delay time including an assumed frame delay.
     * @return The adjusted delay time with the assumed frame delay subtracted out.
     * @hide
     */
    static CARAPI_(Int64) SubtractFrameDelay(
        /* [in] */ Int64 delayMillis);

private:
    static CARAPI_(Float) GetRefreshRate();

private:
    Choreographer(
        /* [in] */ ILooper* looper);

public:
    CAR_INTERFACE_DECL()

    /**
     * @return The refresh rate as the nanoseconds between frames
     * @hide
     */
    CARAPI GetFrameIntervalNanos(
        /* [out] */ Int64* res);

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* writer);

    /**
     * Posts a callback to run on the next frame.
     * <p>
     * The callback runs once then is automatically removed.
     * </p>
     *
     * @param callbackType The callback type.
     * @param action The callback action to run during the next frame.
     * @param token The callback token, or NULL if none.
     *
     * @see #removeCallbacks
     * @hide
     */
    CARAPI PostCallback(
        /* [in] */ Int32 callbackType,
        /* [in] */ IRunnable* action,
        /* [in] */ IObject* token);

    /**
     * Posts a callback to run on the next frame after the specified delay.
     * <p>
     * The callback runs once then is automatically removed.
     * </p>
     *
     * @param callbackType The callback type.
     * @param action The callback action to run during the next frame after the specified delay.
     * @param token The callback token, or NULL if none.
     * @param delayMillis The delay time in milliseconds.
     *
     * @see #removeCallback
     * @hide
     */
    CARAPI PostCallbackDelayed(
        /* [in] */ Int32 callbackType,
        /* [in] */ IRunnable* action,
        /* [in] */ IObject* token,
        /* [in] */ Int64 delayMillis);

    /**
     * Removes callbacks that have the specified action and token.
     *
     * @param callbackType The callback type.
     * @param action The action property of the callbacks to remove, or NULL to remove
     * callbacks with any action.
     * @param token The token property of the callbacks to remove, or NULL to remove
     * callbacks with any token.
     *
     * @see #postCallback
     * @see #postCallbackDelayed
     * @hide
     */
    CARAPI RemoveCallbacks(
        /* [in] */ Int32 callbackType,
        /* [in] */ IRunnable* action,
        /* [in] */ IObject* token);

    /**
     * Posts a frame callback to run on the next frame.
     * <p>
     * The callback runs once then is automatically removed.
     * </p>
     *
     * @param callback The frame callback to run during the next frame.
     *
     * @see #postFrameCallbackDelayed
     * @see #removeFrameCallback
     */
    CARAPI PostFrameCallback(
        /* [in] */ IFrameCallback* callback);

    /**
     * Posts a frame callback to run on the next frame after the specified delay.
     * <p>
     * The callback runs once then is automatically removed.
     * </p>
     *
     * @param callback The frame callback to run during the next frame.
     * @param delayMillis The delay time in milliseconds.
     *
     * @see #postFrameCallback
     * @see #removeFrameCallback
     */
    CARAPI PostFrameCallbackDelayed(
        /* [in] */ IFrameCallback* callback,
        /* [in] */ Int64 delayMillis);

    /**
     * Removes a previously posted frame callback.
     *
     * @param callback The frame callback to remove.
     *
     * @see #postFrameCallback
     * @see #postFrameCallbackDelayed
     */
    CARAPI RemoveFrameCallback(
        /* [in] */ IFrameCallback* callback);

    /**
     * Gets the time when the current frame started.
     * <p>
     * This method provides the time in nanoseconds when the frame started being rendered.
     * The frame time provides a stable time base for synchronizing animations
     * and drawing.  It should be used instead of {@link SystemClock#uptimeMillis()}
     * or {@link System#nanoTime()} for animations and drawing in the UI.  Using the frame
     * time helps to reduce inter-frame jitter because the frame time is fixed at the time
     * the frame was scheduled to start, regardless of when the animations or drawing
     * callback actually runs.  All callbacks that run as part of rendering a frame will
     * observe the same frame time so using the frame time also helps to synchronize effects
     * that are performed by different callbacks.
     * </p><p>
     * Please note that the framework already takes care to process animations and
     * drawing using the frame time as a stable time base.  Most applications should
     * not need to use the frame time information directly.
     * </p><p>
     * This method should only be called from within a callback.
     * </p>
     *
     * @return The frame start time, in the {@link SystemClock#uptimeMillis()} time base.
     *
     * @throws IllegalStateException if no frame is in progress.
     * @hide
     */
    CARAPI GetFrameTime(
        /* [out] */ Int64* frameTime);

    /**
     * Same as {@link #getFrameTime()} but with nanosecond precision.
     *
     * @return The frame start time, in the {@link System#nanoTime()} time base.
     *
     * @throws IllegalStateException if no frame is in progress.
     * @hide
     */
    CARAPI GetFrameTimeNanos(
        /* [out] */ Int64* frameTimeNanos);

protected:
    CARAPI_(void) DoFrame(
        /* [in] */ Int64 frameTimeNanos,
        /* [in] */ Int32 frame);

    CARAPI_(void) DoCallbacks(
        /* [in] */ Int32 callbackType,
        /* [in] */ Int64 frameTimeNanos);

    CARAPI_(void) DoScheduleVsync();

    CARAPI_(void) DoScheduleCallback(
        /* [in] */ Int32 callbackType);

private:
    CARAPI_(void) PostCallbackDelayedInternal(
        /* [in] */ Int32 callbackType,
        /* [in] */ IInterface* action,
        /* [in] */ IObject* token,
        /* [in] */ Int64 delayMillis);

    CARAPI_(void) RemoveCallbacksInternal(
        /* [in] */ Int32 callbackType,
        /* [in] */ IInterface* action,
        /* [in] */ IObject* token);

    CARAPI_(void) ScheduleFrameLocked(
        /* [in] */ Int64 now);

    CARAPI_(void) ScheduleVsyncLocked();

    CARAPI_(Boolean) IsRunningOnLooperThreadLocked();

    CARAPI_(AutoPtr<CallbackRecord>) ObtainCallbackLocked(
        /* [in] */ Int64 dueTime,
        /* [in] */ IInterface* action,
        /* [in] */ IObject* token);

    CARAPI_(void) RecycleCallbackLocked(
        /* [in] */ CallbackRecord* callback);

    CARAPI_(void) HandleDoFrame();

public:
    friend class FrameHandler;

    static const Int32 MSG_DO_FRAME = 0;
    static const Int32 MSG_DO_SCHEDULE_VSYNC = 1;
    static const Int32 MSG_DO_SCHEDULE_CALLBACK = 2;

private:
    static const char* TAG;
    static const Boolean DEBUG;

    // The default amount of time in ms between animation frames.
    // When vsync is not enabled, we want to have some idea of how Int64 we should
    // wait before posting the next animation message.  It is important that the
    // default value be less than the TRUE inter-frame delay on all devices to avoid
    // situations where we might skip frames by waiting too Int64 (we must compensate
    // for jitter and hardware variations).  Regardless of this value, the animation
    // and display loop is ultimately rate-limited by how fast new graphics buffers can
    // be dequeued.
    static const Int64 DEFAULT_FRAME_DELAY = 10;

    // The number of milliseconds between animation frames.
    static volatile Int64 sFrameDelay;

    // Thread local storage for the choreographer.
    static pthread_key_t sKey;
    static Boolean sHaveKey;

    // Enable/disable vsync for animations and drawing.
    static const Boolean USE_VSYNC;

    // Enable/disable using the frame time instead of returning now.
    static const Boolean USE_FRAME_TIME;

    // Set a limit to warn about skipped frames.
    // Skipped frames imply jank.
    static const Int32 SKIPPED_FRAME_WARNING_LIMIT;

    // All frame callbacks posted by applications have this token.
    static const AutoPtr<IObject> FRAME_CALLBACK_TOKEN;

    static const Int32 CALLBACK_LAST;

private:
    Object mLock;

    AutoPtr<ILooper> mLooper;
    AutoPtr<FrameHandler> mHandler;

    // The display event receiver can only be accessed by the looper thread to which
    // it is attached.  We take care to ensure that we post message to the looper
    // if appropriate when interacting with the display event receiver.
    AutoPtr<FrameDisplayEventReceiver> mDisplayEventReceiver;

    AutoPtr<CallbackRecord> mCallbackPool;

    AutoPtr<ArrayOf<CallbackQueue*> > mCallbackQueues;

    Boolean mFrameScheduled;
    Boolean mCallbacksRunning;
    Int64 mLastFrameTimeNanos;
    Int64 mFrameIntervalNanos;

};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CHOREOGRAPHER_H__
