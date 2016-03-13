// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_VSYNCMONITOR_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_VSYNCMONITOR_H_

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui;
// import android.annotation.SuppressLint;
// import android.content.Context;
// import android.os.Build;
// import android.os.Handler;
// import android.view.Choreographer;
// import android.view.WindowManager;
// import org.chromium.base.TraceEvent;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IChoreographer;
using Elastos::Droid::View::IFrameCallback;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

/**
  * Notifies clients of the default displays's vertical sync pulses.
  * This class works in "burst" mode: once the update is requested, the listener will be
  * called MAX_VSYNC_COUNT times on the vertical sync pulses (on JB) or on every refresh
  * period (on ICS, see below), unless stop() is called.
  * On ICS, VSyncMonitor relies on setVSyncPointForICS() being called to set a reasonable
  * approximation of a vertical sync starting point; see also http://crbug.com/156397.
  */
// @SuppressLint("NewApi")
class VSyncMonitor
    : public Object
{
public:
    /**
      * VSync listener class
      */
    class Listener
        : public Object
    {
    public:
        /**
          * Called very soon after the start of the display's vertical sync period.
          * @param monitor The VSyncMonitor that triggered the signal.
          * @param vsyncTimeMicros Absolute frame time in microseconds.
          */
        virtual CARAPI OnVSync(
            /* [in] */ VSyncMonitor* monitor,
            /* [in] */ Int64 vsyncTimeMicros) = 0;
    };

    class InnerChoreographerFrameCallback
        : public Object
        , public IFrameCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerChoreographerFrameCallback(
            /* [in] */ VSyncMonitor* owner);

        // @Override
        CARAPI DoFrame(
            /* [in] */ Int64 frameTimeNanos);

    private:
        VSyncMonitor* mOwner;
    };

    class InnerVSyncTimerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerVSyncTimerRunnable(
            /* [in] */ VSyncMonitor* owner);

        // @Override
        CARAPI Run();

    private:
        VSyncMonitor* mOwner;
    };

    class InnerVSyncSyntheticRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerVSyncSyntheticRunnable(
            /* [in] */ VSyncMonitor* owner);

        // @Override
        CARAPI Run();

    private:
        VSyncMonitor* mOwner;
    };

public:
    VSyncMonitor();

    /**
      * Constructs a VSyncMonitor
      * @param context The application context.
      * @param listener The listener receiving VSync notifications.
      */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Listener* listener);

    /**
      * Constructs a VSyncMonitor
      * @param context The application context.
      * @param listener The listener receiving VSync notifications.
      * @param enableJBVsync Whether to allow Choreographer-based notifications on JB and up.
      */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Listener* listener,
        /* [in] */ Boolean enableJBVSync);

    /**
      * Returns the time interval between two consecutive vsync pulses in microseconds.
      */
    virtual CARAPI_(Int64) GetVSyncPeriodInMicroseconds();

    /**
      * Stop reporting vsync events. Note that at most one pending vsync event can still be delivered
      * after this function is called.
      */
    virtual CARAPI Stop();

    /**
      * Request to be notified of the closest display vsync events.
      * Listener.onVSync() will be called soon after the upcoming vsync pulses.
      * It will be called at most MAX_AUTO_ONVSYNC_COUNT times unless requestUpdate() is called.
      */
    virtual CARAPI RequestUpdate();

    /**
      * Set the best guess of the point in the past when the vsync has happened.
      * @param goodStartingPointNano Known vsync point in the past.
      */
    virtual CARAPI SetVSyncPointForICS(
        /* [in] */ Int64 goodStartingPointNano);

private:
    /**
      * Determine whether a true vsync signal is available on this platform.
      */
    CARAPI_(Boolean) IsVSyncSignalAvailable();

    CARAPI_(Int64) GetCurrentNanoTime();

    CARAPI OnVSyncCallback(
        /* [in] */ Int64 frameTimeNanos,
        /* [in] */ Int64 currentTimeNanos);

    CARAPI PostCallback();

    CARAPI_(Boolean) PostSyntheticVSync();

    CARAPI_(Int64) EstimateLastVSyncTime(
        /* [in] */ Int64 currentTime);

    CARAPI PostRunnableCallback();

public:
    static const Int32 MAX_AUTO_ONVSYNC_COUNT = 5;

private:
    static const Int64 NANOSECONDS_PER_SECOND = 1000000000;
    static const Int64 NANOSECONDS_PER_MILLISECOND = 1000000;
    static const Int64 NANOSECONDS_PER_MICROSECOND = 1000;
    AutoPtr<Listener> mListener;
    // Display refresh rate as reported by the system.
    /*const*/ Int64 mRefreshPeriodNano;
    Boolean mHaveRequestInFlight;
    Int32 mTriggerNextVSyncCount;
    // Choreographer is used to detect vsync on >= JB.
    /*const*/ AutoPtr<IChoreographer> mChoreographer;
    /*const*/ AutoPtr<IFrameCallback> mVSyncFrameCallback;
    // On ICS we just post a task through the handler (http://crbug.com/156397)
    /*const*/ AutoPtr<IRunnable> mVSyncRunnableCallback;
    Int64 mGoodStartingPointNano;
    Int64 mLastPostedNano;
    // If the monitor is activated after having been idle, we synthesize the first vsync to reduce
    // latency.
    /*const*/ AutoPtr<IHandler> mHandler;
    /*const*/ AutoPtr<IRunnable> mSyntheticVSyncRunnable;
    Int64 mLastVSyncCpuTimeNano;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_VSYNCMONITOR_H_

