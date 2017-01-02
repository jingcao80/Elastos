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

#ifndef __ELASTOS_DROID_SERVER_POWER_DISPLAYPOWERSTATE_H__
#define __ELASTOS_DROID_SERVER_POWER_DISPLAYPOWERSTATE_H__

#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/display/ColorFade.h"
#include "elastos/droid/server/lights/Light.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/utility/Int32Property.h>
#include <elastos/droid/utility/FloatProperty.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Server::Lights::Light;
using Elastos::Droid::Server::Lights::ILight;
using Elastos::Droid::View::IChoreographer;
using Elastos::Droid::Utility::FloatProperty;
using Elastos::Droid::Utility::Int32Property;
using Elastos::Droid::Utility::IFloatProperty;
using Elastos::Droid::Utility::IInt32Property;

using Elastos::Core::Thread;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Controls the display power state.
 * <p>
 * This component is similar in nature to a {@link android.view.View} except that it
 * describes the properties of a display.  When properties are changed, the component
 * invalidates itself and posts a callback to apply the changes in a consistent order.
 * This mechanism enables multiple properties of the display power state to be animated
 * together smoothly by the animation framework.  Some of the work to blank or unblank
 * the display is done on a separate thread to avoid blocking the looper.
 * </p><p>
 * This component must only be created or accessed by the {@link Looper} thread
 * that belongs to the {@link DisplayPowerController}.
 * </p><p>
 * We don't need to worry about holding a suspend blocker here because the
 * power manager does that for us whenever there is a change in progress.
 * </p>
 */
class DisplayPowerState
    : public Object
    , public IDisplayPowerState
{
public:
    class DisplayPowerStateFloatProperty
        : public FloatProperty
    {
    public :

        DisplayPowerStateFloatProperty(
            /* [in] */ const String& name);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Float value);
    };

    class DisplayPowerStateInt32Property
        : public Int32Property
    {
    public :
        DisplayPowerStateInt32Property(
            /* [in] */ const String& name);

        CARAPI Get(
            /* [in] */ IInterface* obj,
            /* [out] */ IInterface** rst);

        CARAPI SetValue(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 value);
    };

private:
    /**
     * Updates the state of the screen and backlight asynchronously on a separate thread.
     */
    class PhotonicModulator
        : public Thread
    {
    public:
        PhotonicModulator(
            /* [in] */ DisplayPowerState* host);

        CARAPI_(Boolean) SetState(
            /* [in] */ Int32 on,
            /* [in] */ Int32 backlight);

        // CARAPI_(void) Dump(
        //     /* [in] */ IPrintWriter* pw);

        void RequestDisplayState(
            /* [in] */ Int32 state);

        void SetBrightness(
            /* [in] */ Int32 backlight);

        CARAPI Run();

    private:
        friend class TaskRunnable;

        static const Int32 INITIAL_SCREEN_STATE;
        static const Int32 INITIAL_BACKLIGHT;

        Object mLock;

        Int32 mPendingState;
        Int32 mPendingBacklight;
        Int32 mActualState;
        Int32 mActualBacklight;
        Boolean mChangeInProgress;
        DisplayPowerState* mHost;
    };

    class ScreenUpdateRunnable
        : public Runnable
    {
    public:
        ScreenUpdateRunnable(
            /* [in] */ DisplayPowerState* host);

        CARAPI Run();

    private:
        DisplayPowerState* mHost;
    };

    class ColorFadeDrawRunnable
        : public Runnable
    {
    public:
        ColorFadeDrawRunnable(
            /* [in] */ DisplayPowerState* host);

        CARAPI Run();

    private:
        DisplayPowerState* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    DisplayPowerState();

    CARAPI constructor(
        /* [in] */ IDisplayBlanker* blanker,
        /* [in] */ ILight* backlight,
        /* [in] */ IColorFade* electronBeam);

    /**
     * Gets the desired screen state.
     */
    CARAPI_(void) SetScreenState(
        /* [in] */ Int32 state);

    /**
     * Returns true if the screen is on.
     */
    CARAPI_(Int32) GetScreenState();

    /**
     * Sets the display brightness.
     *
     * @param brightness The brightness, ranges from 0 (minimum / off) to 255 (brightest).
     */
    CARAPI_(void) SetScreenBrightness(
        /* [in] */ Int32 brightness);

    /**
     * Gets the screen brightness.
     */
    CARAPI_(Int32) GetScreenBrightness();

    /**
     * Prepares the electron beam to turn on or off.
     * This method should be called before starting an animation because it
     * can take a fair amount of time to prepare the electron beam surface.
     *
     * @param mode The electron beam animation mode to prepare.
     * @return True if the electron beam was prepared.
     */
    CARAPI_(Boolean) PrepareColorFade(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    /**
     * Dismisses the electron beam surface.
     */
    CARAPI_(void) DismissColorFade();

    /**
     * Sets the level of the electron beam steering current.
     *
     * The display is blanked when the level is 0.0.  In normal use, the electron
     * beam should have a value of 1.0.  The electron beam is unstable in between
     * these states and the picture quality may be compromised.  For best effect,
     * the electron beam should be warmed up or cooled off slowly.
     *
     * Warning: Electron beam emits harmful radiation.  Avoid direct exposure to
     * skin or eyes.
     *
     * @param level The level, ranges from 0.0 (full off) to 1.0 (full on).
     */
    CARAPI_(void) SetColorFadeLevel(
        /* [in] */ Float level);

    /**
     * Gets the level of the electron beam steering current.
     */
    CARAPI_(Float) GetColorFadeLevel();

    /**
     * Returns true if no properties have been invalidated.
     * Otherwise, returns false and promises to invoke the specified listener
     * when the properties have all been applied.
     * The listener always overrides any previously set listener.
     */
    CARAPI_(Boolean) WaitUntilClean(
        /* [in] */ IRunnable* listener);

    // CARAPI_(void) Dump(
    //     /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(void) ScheduleScreenUpdate();

    CARAPI_(void) PostScreenUpdateThreadSafe();

    CARAPI_(void) ScheduleColorFadeDraw();

    CARAPI_(void) InvokeCleanListenerIfNeeded();

public:
    const static String fName;
    const static String iName;

    static AutoPtr<IFloatProperty> COLOR_FADE_LEVEL;
    static AutoPtr<IInt32Property> SCREEN_BRIGHTNESS;

private:
    static const String TAG;// = "DisplayPowerState";

    static Boolean DEBUG;// = FALSE;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IChoreographer> mChoreographer;
    AutoPtr<IDisplayBlanker> mBlanker;
    AutoPtr<Light> mBacklight;
    AutoPtr<ColorFade> mColorFade;
    AutoPtr<PhotonicModulator> mPhotonicModulator;

    Int32 mScreenState;
    Int32 mScreenBrightness;
    Boolean mScreenReady;
    Boolean mScreenUpdatePending;

    Boolean mColorFadePrepared;
    Float mColorFadeLevel;
    Boolean mColorFadeReady;
    Boolean mColorFadeDrawPending;

    AutoPtr<IRunnable> mCleanListener;
    AutoPtr<IRunnable> mScreenUpdateRunnable;
    AutoPtr<IRunnable> mColorFadeDrawRunnable;

    friend class PhotonicModulator;
    friend class ScreenUpdateRunnable;
    friend class ColorFadeDrawRunnable;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_POWER_DISPLAYPOWERSTATE_H__
