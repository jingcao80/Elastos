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

#ifndef __ELASTOS_DROID_SERVICE_DREAMS_DREAMSERVICE_H__
#define __ELASTOS_DROID_SERVICE_DREAMS_DREAMSERVICE_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/app/Service.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

/**
 * Extend this class to implement a custom Dream (displayed to the user as a "Sleep Mode").
 *
 * <p>Dreams are interactive screensavers launched when a charging device is idle, or docked in a
 * desk dock. Dreams provide another modality for apps to express themselves, tailored for
 * an exhibition/lean-back experience.</p>
 *
 * <p>The Dream lifecycle is as follows:</p>
 * <ol>
 *   <li>{@link #onAttachedToWindow}
 *     <p>Use this for initial setup, such as calling {@link #setContentView setContentView()}.</li>
 *   <li>{@link #onDreamingStarted}
 *     <p>Your dream has started, so you should begin animations or other behaviors here.</li>
 *   <li>{@link #onDreamingStopped}
 *     <p>Use this to stop the things you started in {@link #onDreamingStarted}.</li>
 *   <li>{@link #onDetachedFromWindow}
 *     <p>Use this to dismantle resources your dream set up. For example, detach from handlers
 *        and listeners.</li>
 * </ol>
 *
 * <p>In addition, onCreate and onDestroy (from the Service interface) will also be called, but
 * initialization and teardown should be done by overriding the hooks above.</p>
 *
 * <p>To be available to the system, Dreams should be declared in the manifest as follows:</p>
 * <pre>
 * &lt;service
 *     android:name=".MyDream"
 *     android:exported="true"
 *     android:icon="@drawable/my_icon"
 *     android:label="@string/my_dream_label" >
 *
 *     &lt;intent-filter>
 *         &lt;action android:name="android.service.dreams.DreamService" />
 *         &lt;category android:name="android.intent.category.DEFAULT" />
 *     &lt;/intent-filter>
 *
 *     &lt;!-- Point to additional information for this dream (optional) -->
 *     &lt;meta-data
 *         android:name="android.service.dream"
 *         android:resource="@xml/my_dream" />
 * &lt;/service>
 * </pre>
 *
 * <p>If specified with the {@code &lt;meta-data&gt;} element,
 * additional information for the dream is defined using the
 * {@link android.R.styleable#Dream &lt;dream&gt;} element in a separate XML file.
 * Currently, the only addtional
 * information you can provide is for a settings activity that allows the user to configure
 * the dream behavior. For example:</p>
 * <p class="code-caption">res/xml/my_dream.xml</p>
 * <pre>
 * &lt;dream xmlns:android="http://schemas.android.com/apk/res/android"
 *     android:settingsActivity="com.example.app/.MyDreamSettingsActivity" />
 * </pre>
 * <p>This makes a Settings button available alongside your dream's listing in the
 * system settings, which when pressed opens the specified activity.</p>
 *
 *
 * <p>To specify your dream layout, call {@link #setContentView}, typically during the
 * {@link #onAttachedToWindow} callback. For example:</p>
 * <pre>
 * public class MyDream extends DreamService {
 *
 *     &#64;Override
 *     public void onAttachedToWindow() {
 *         super.onAttachedToWindow();
 *
 *         // Exit dream upon user touch
 *         setInteractive(false);
 *         // Hide system UI
 *         setFullscreen(true);
 *         // Set the dream layout
 *         setContentView(R.layout.dream);
 *     }
 * }
 * </pre>
 */
class ECO_PUBLIC DreamService
    : public Elastos::Droid::App::Service
    , public IDreamService
    , public IWindowCallback
{
public:
    class DreamServiceWrapper
        : public Object
        , public IIDreamService
        , public IBinder
    {
        friend class DreamService;
    private:
        class AttachRunnable
            : public Runnable
        {
            friend class DreamServiceWrapper;
        public:
            AttachRunnable(
                /* [in] */ DreamServiceWrapper* host);

            CARAPI Run();

        private:
            DreamServiceWrapper* mHost;
        };

        class DetachRunnable
            : public Runnable
        {
            friend class DreamServiceWrapper;
        public:
            DetachRunnable(
                /* [in] */ DreamServiceWrapper* host);

            CARAPI Run();

        private:
            DreamServiceWrapper* mHost;
        };

        class WakeUpRunnable
            : public Runnable
        {
            friend class DreamServiceWrapper;
        public:
            WakeUpRunnable(
                /* [in] */ DreamServiceWrapper* host);

            CARAPI Run();

        private:
            DreamServiceWrapper* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        DreamServiceWrapper();

        CARAPI constructor(
            /* [in] */ IDreamService* host);

        // @Override
        CARAPI Attach(
            /* [in] */ IBinder* windowToken,
            /* [in] */ Boolean canDoze);

        // @Override
        CARAPI Detach();

        // @Override
        CARAPI WakeUp();

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        DreamService* mHost;
        AutoPtr<IBinder> mWindowToken;
        Boolean mCanDoze;
    };

private:
    class DreamService_Attach_Runnable
        : public Runnable
    {
        friend class DreamService;
    public:
        DreamService_Attach_Runnable(
            /* [in] */ DreamService* host);

        CARAPI Run();

    private:
        DreamService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DreamService();

    CARAPI constructor();

    /**
     * @hide
     */
    CARAPI SetDebug(
        /* [in] */ Boolean dbg);

    // begin Window.Callback methods
    /** {@inheritDoc} */
    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnCreatePanelView(
        /* [in] */ Int32 featureId,
        /* [out] */ IView** result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnCreatePanelMenu(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnPreparePanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IView* view,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnMenuOpened(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnMenuItemSelected(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnWindowAttributesChanged(
        /* [in] */ IWindowManagerLayoutParams* attrs);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnContentChanged();

    /** {@inheritDoc} */
    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnAttachedToWindow();

    /** {@inheritDoc} */
    //@Override
    CARAPI OnDetachedFromWindow();

    /** {@inheritDoc} */
    //@Override
    CARAPI OnPanelClosed(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnSearchRequested(
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnWindowStartingActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** result);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnActionModeStarted(
        /* [in] */ IActionMode* mode);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnActionModeFinished(
        /* [in] */ IActionMode* mode);

    // end Window.Callback methods

    // begin public api
    /**
     * Retrieves the current {@link android.view.WindowManager} for the dream.
     * Behaves similarly to {@link android.app.Activity#getWindowManager()}.
     *
     * @return The current window manager, or null if the dream is not started.
     */
    CARAPI GetWindowManager(
        /* [out] */ IWindowManager** result);

    /**
     * Retrieves the current {@link android.view.Window} for the dream.
     * Behaves similarly to {@link android.app.Activity#getWindow()}.
     *
     * @return The current window, or null if the dream is not started.
     */
    CARAPI GetWindow(
        /* [out] */ IWindow** result);

   /**
     * Inflates a layout resource and set it to be the content view for this Dream.
     * Behaves similarly to {@link android.app.Activity#setContentView(int)}.
     *
     * <p>Note: Requires a window, do not call before {@link #onAttachedToWindow()}</p>
     *
     * @param layoutResID Resource ID to be inflated.
     *
     * @see #setContentView(android.view.View)
     * @see #setContentView(android.view.View, android.view.ViewGroup.LayoutParams)
     */
    CARAPI SetContentView(
        /* [in] */ Int32 layoutResID);

    /**
     * Sets a view to be the content view for this Dream.
     * Behaves similarly to {@link android.app.Activity#setContentView(android.view.View)} in an activity,
     * including using {@link ViewGroup.LayoutParams#MATCH_PARENT} as the layout height and width of the view.
     *
     * <p>Note: This requires a window, so you should usually call it during
     * {@link #onAttachedToWindow()} and never earlier (you <strong>cannot</strong> call it
     * during {@link #onCreate}).</p>
     *
     * @see #setContentView(int)
     * @see #setContentView(android.view.View, android.view.ViewGroup.LayoutParams)
     */
    CARAPI SetContentView(
        /* [in] */ IView* view);

    /**
     * Sets a view to be the content view for this Dream.
     * Behaves similarly to
     * {@link android.app.Activity#setContentView(android.view.View, android.view.ViewGroup.LayoutParams)}
     * in an activity.
     *
     * <p>Note: This requires a window, so you should usually call it during
     * {@link #onAttachedToWindow()} and never earlier (you <strong>cannot</strong> call it
     * during {@link #onCreate}).</p>
     *
     * @param view The desired content to display.
     * @param params Layout parameters for the view.
     *
     * @see #setContentView(android.view.View)
     * @see #setContentView(int)
     */
    CARAPI SetContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    /**
     * Adds a view to the Dream's window, leaving other content views in place.
     *
     * <p>Note: Requires a window, do not call before {@link #onAttachedToWindow()}</p>
     *
     * @param view The desired content to display.
     * @param params Layout parameters for the view.
     */
    CARAPI AddContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    /**
     * Finds a view that was identified by the id attribute from the XML that
     * was processed in {@link #onCreate}.
     *
     * <p>Note: Requires a window, do not call before {@link #onAttachedToWindow()}</p>
     *
     * @return The view if found or null otherwise.
     */
    CARAPI FindViewById(
        /* [in] */ Int32 id,
        /* [out] */ IView** result);

    /**
     * Marks this dream as interactive to receive input events.
     *
     * <p>Non-interactive dreams (default) will dismiss on the first input event.</p>
     *
     * <p>Interactive dreams should call {@link #finish()} to dismiss themselves.</p>
     *
     * @param interactive True if this dream will handle input events.
     */
    CARAPI SetInteractive(
        /* [in] */ Boolean interactive);

    /**
     * Returns whether or not this dream is interactive.  Defaults to false.
     *
     * @see #setInteractive(boolean)
     */
    CARAPI IsInteractive(
        /* [out] */ Boolean* result);

    /**
     * Sets View.SYSTEM_UI_FLAG_LOW_PROFILE on the content view.
     *
     * @param lowProfile True to set View.SYSTEM_UI_FLAG_LOW_PROFILE
     * @hide There is no reason to have this -- dreams can set this flag
     * on their own content view, and from there can actually do the
     * correct interactions with it (seeing when it is cleared etc).
     */
    CARAPI SetLowProfile(
        /* [in] */ Boolean lowProfile);

    /**
     * Returns whether or not this dream is in low profile mode. Defaults to true.
     *
     * @see #setLowProfile(boolean)
     * @hide
     */
    CARAPI IsLowProfile(
        /* [out] */ Boolean* result);

    /**
     * Controls {@link android.view.WindowManager.LayoutParams#FLAG_FULLSCREEN}
     * on the dream's window.
     *
     * @param fullscreen If true, the fullscreen flag will be set; else it
     * will be cleared.
     */
    CARAPI SetFullscreen(
        /* [in] */ Boolean fullscreen);

    /**
     * Returns whether or not this dream is in fullscreen mode. Defaults to false.
     *
     * @see #setFullscreen(boolean)
     */
    CARAPI IsFullscreen(
        /* [out] */ Boolean* result);

    /**
     * Marks this dream as keeping the screen bright while dreaming.
     *
     * @param screenBright True to keep the screen bright while dreaming.
     */
    CARAPI SetScreenBright(
        /* [in] */ Boolean screenBright);

    /**
     * Returns whether or not this dream keeps the screen bright while dreaming. Defaults to false,
     * allowing the screen to dim if necessary.
     *
     * @see #setScreenBright(boolean)
     */
    CARAPI IsScreenBright(
        /* [out] */ Boolean* result);

    /**
     * Marks this dream as windowless.  Only available to doze dreams.
     *
     * @hide
     */
    CARAPI SetWindowless(
        /* [in] */ Boolean windowless);

    /**
     * Returns whether or not this dream is windowless.  Only available to doze dreams.
     *
     * @hide
     */
    CARAPI IsWindowless(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this dream is allowed to doze.
     * <p>
     * The value returned by this method is only meaningful when the dream has started.
     * </p>
     *
     * @return True if this dream can doze.
     * @see #startDozing
     * @hide For use by system UI components only.
     */
    CARAPI CanDoze(
        /* [out] */ Boolean* result);

    /**
     * Starts dozing, entering a deep dreamy sleep.
     * <p>
     * Dozing enables the system to conserve power while the user is not actively interacting
     * with the device.  While dozing, the display will remain on in a low-power state
     * and will continue to show its previous contents but the application processor and
     * other system components will be allowed to suspend when possible.
     * </p><p>
     * While the application processor is suspended, the dream may stop executing code
     * for long periods of time.  Prior to being suspended, the dream may schedule periodic
     * wake-ups to render new content by scheduling an alarm with the {@link AlarmManager}.
     * The dream may also keep the CPU awake by acquiring a
     * {@link android.os.PowerManager#PARTIAL_WAKE_LOCK partial wake lock} when necessary.
     * Note that since the purpose of doze mode is to conserve power (especially when
     * running on battery), the dream should not wake the CPU very often or keep it
     * awake for very long.
     * </p><p>
     * It is a good idea to call this method some time after the dream's entry animation
     * has completed and the dream is ready to doze.  It is important to completely
     * finish all of the work needed before dozing since the application processor may
     * be suspended at any moment once this method is called unless other wake locks
     * are being held.
     * </p><p>
     * Call {@link #stopDozing} or {@link #finish} to stop dozing.
     * </p>
     *
     * @see #stopDozing
     * @hide For use by system UI components only.
     */
    CARAPI StartDozing();

    /**
     * Stops dozing, returns to active dreaming.
     * <p>
     * This method reverses the effect of {@link #startDozing}.  From this moment onward,
     * the application processor will be kept awake as long as the dream is running
     * or until the dream starts dozing again.
     * </p>
     *
     * @see #startDozing
     * @hide For use by system UI components only.
     */
    CARAPI StopDozing();

    /**
     * Returns true if the dream will allow the system to enter a low-power state while
     * it is running without actually turning off the screen.  Defaults to false,
     * keeping the application processor awake while the dream is running.
     *
     * @return True if the dream is dozing.
     *
     * @see #setDozing(boolean)
     * @hide For use by system UI components only.
     */
    CARAPI IsDozing(
        /* [out] */ Boolean* result);

    /**
     * Gets the screen state to use while dozing.
     *
     * @return The screen state to use while dozing, such as {@link Display#STATE_ON},
     * {@link Display#STATE_DOZE}, {@link Display#STATE_DOZE_SUSPEND},
     * or {@link Display#STATE_OFF}, or {@link Display#STATE_UNKNOWN} for the default
     * behavior.
     *
     * @see #setDozeScreenState
     * @hide For use by system UI components only.
     */
    CARAPI GetDozeScreenState(
        /* [out] */ Int32* result);

    /**
     * Sets the screen state to use while dozing.
     * <p>
     * The value of this property determines the power state of the primary display
     * once {@link #startDozing} has been called.  The default value is
     * {@link Display#STATE_UNKNOWN} which lets the system decide.
     * The dream may set a different state before starting to doze and may
     * perform transitions between states while dozing to conserve power and
     * achieve various effects.
     * </p><p>
     * It is recommended that the state be set to {@link Display#STATE_DOZE_SUSPEND}
     * once the dream has completely finished drawing and before it releases its wakelock
     * to allow the display hardware to be fully suspended.  While suspended, the
     * display will preserve its on-screen contents or hand off control to dedicated
     * doze hardware if the devices supports it.  If the doze suspend state is
     * used, the dream must make sure to set the mode back
     * to {@link Display#STATE_DOZE} or {@link Display#STATE_ON} before drawing again
     * since the display updates may be ignored and not seen by the user otherwise.
     * </p><p>
     * The set of available display power states and their behavior while dozing is
     * hardware dependent and may vary across devices.  The dream may therefore
     * need to be modified or configured to correctly support the hardware.
     * </p>
     *
     * @param state The screen state to use while dozing, such as {@link Display#STATE_ON},
     * {@link Display#STATE_DOZE}, {@link Display#STATE_DOZE_SUSPEND},
     * or {@link Display#STATE_OFF}, or {@link Display#STATE_UNKNOWN} for the default
     * behavior.
     *
     * @hide For use by system UI components only.
     */
    CARAPI SetDozeScreenState(
        /* [in] */ Int32 state);

    /**
     * Gets the screen brightness to use while dozing.
     *
     * @return The screen brightness while dozing as a value between
     * {@link PowerManager#BRIGHTNESS_OFF} (0) and {@link PowerManager#BRIGHTNESS_ON} (255),
     * or {@link PowerManager#BRIGHTNESS_DEFAULT} (-1) to ask the system to apply
     * its default policy based on the screen state.
     *
     * @see #setDozeScreenBrightness
     * @hide For use by system UI components only.
     */
    CARAPI GetDozeScreenBrightness(
        /* [out] */ Int32* result);

    /**
     * Sets the screen brightness to use while dozing.
     * <p>
     * The value of this property determines the power state of the primary display
     * once {@link #startDozing} has been called.  The default value is
     * {@link PowerManager#BRIGHTNESS_DEFAULT} which lets the system decide.
     * The dream may set a different brightness before starting to doze and may adjust
     * the brightness while dozing to conserve power and achieve various effects.
     * </p><p>
     * Note that dream may specify any brightness in the full 0-255 range, including
     * values that are less than the minimum value for manual screen brightness
     * adjustments by the user.  In particular, the value may be set to 0 which may
     * turn off the backlight entirely while still leaving the screen on although
     * this behavior is device dependent and not guaranteed.
     * </p><p>
     * The available range of display brightness values and their behavior while dozing is
     * hardware dependent and may vary across devices.  The dream may therefore
     * need to be modified or configured to correctly support the hardware.
     * </p>
     *
     * @param brightness The screen brightness while dozing as a value between
     * {@link PowerManager#BRIGHTNESS_OFF} (0) and {@link PowerManager#BRIGHTNESS_ON} (255),
     * or {@link PowerManager#BRIGHTNESS_DEFAULT} (-1) to ask the system to apply
     * its default policy based on the screen state.
     *
     * @hide For use by system UI components only.
     */
    CARAPI SetDozeScreenBrightness(
        /* [in] */ Int32 brightness);


    /**
     * Called when this Dream is constructed.
     */
    //@Override
    CARAPI OnCreate();

    /**
     * Called when the dream's window has been created and is visible and animation may now begin.
     */
    CARAPI OnDreamingStarted();

    /**
     * Called when this Dream is stopped, either by external request or by calling finish(),
     * before the window has been removed.
     */
    CARAPI OnDreamingStopped();

    /**
     * Called when the dream is being asked to stop itself and wake.
     * <p>
     * The default implementation simply calls {@link #finish} which ends the dream
     * immediately.  Subclasses may override this function to perform a smooth exit
     * transition then call {@link #finish} afterwards.
     * </p><p>
     * Note that the dream will only be given a short period of time (currently about
     * five seconds) to wake up.  If the dream does not finish itself in a timely manner
     * then the system will forcibly finish it once the time allowance is up.
     * </p>
     */
    CARAPI OnWakeUp();

    /** {@inheritDoc} */
    //@Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

    /**
     * Stops the dream, detaches from the window, and wakes up.
     */
    CARAPI Finish();

    /**
     * Wakes the dream up gently.
     * <p>
     * Calls {@link #onWakeUp} to give the dream a chance to perform an exit transition.
     * When the transition is over, the dream should call {@link #finish}.
     * </p>
     */
    CARAPI WakeUp();

    /** {@inheritDoc} */
    //@Override
    CARAPI OnDestroy();

// protected:
    // @Override
    // protected void dump(final FileDescriptor fd, PrintWriter pw, final String[] args) {
    //     DumpUtils.dumpAsync(mHandler, new Dump() {
    //         @Override
    //         public void dump(PrintWriter pw) {
    //             dumpOnHandler(fd, pw, args);
    //         }
    //     }, pw, 1000);
    // }

    // /** @hide */
    // protected void dumpOnHandler(FileDescriptor fd, PrintWriter pw, String[] args) {
    //     pw.print(TAG + ": ");
    //     if (mWindowToken == null) {
    //         pw.println("stopped");
    //     } else {
    //         pw.println("running (token=" + mWindowToken + ")");
    //     }
    //     pw.println("  window: " + mWindow);
    //     pw.print("  flags:");
    //     if (isInteractive()) pw.print(" interactive");
    //     if (isLowProfile()) pw.print(" lowprofile");
    //     if (isFullscreen()) pw.print(" fullscreen");
    //     if (isScreenBright()) pw.print(" bright");
    //     if (isWindowless()) pw.print(" windowless");
    //     if (isDozing()) pw.print(" dozing");
    //     else if (canDoze()) pw.print(" candoze");
    //     pw.println();
    //     if (canDoze()) {
    //         pw.println("  doze screen state: " + Display.stateToString(mDozeScreenState));
    //         pw.println("  doze screen brightness: " + mDozeScreenBrightness);
    //     }
    // }

private:
    CARAPI UpdateDoze();


    CARAPI WakeUp(
        /* [in] */ Boolean fromSystem);

    /**
     * Called by DreamController.stopDream() when the Dream is about to be unbound and destroyed.
     *
     * Must run on mHandler.
     */
    CARAPI Detach();

    using Service::Attach;

    /**
     * Called when the Dream is ready to be shown.
     *
     * Must run on mHandler.
     *
     * @param windowToken A window token that will allow a window to be created in the correct layer.
     */
    CARAPI Attach(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Boolean canDoze);

    CARAPI GetWindowFlagValue(
        /* [in] */ Int32 flag,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* result);

    CARAPI ApplyWindowFlags(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

    CARAPI GetSystemUiVisibilityFlagValue(
        /* [in] */ Int32 flag,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* result);

    CARAPI ApplySystemUiVisibilityFlags(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

    CARAPI ApplyFlags(
        /* [in] */ Int32 oldFlags,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask,
        /* [out] */ Int32* result);

    static CARAPI ClampAbsoluteBrightness(
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

private:
    const static String TAG;
    AutoPtr<IIDreamManager> mSandman;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IBinder> mWindowToken;
    AutoPtr<IWindow> mWindow;
    Boolean mInteractive;
    Boolean mLowProfile;
    Boolean mFullscreen;
    Boolean mScreenBright;
    Boolean mStarted;
    Boolean mWaking;
    Boolean mFinished;
    Boolean mCanDoze;
    Boolean mDozing;
    Boolean mWindowless;
    Int32 mDozeScreenState;
    Int32 mDozeScreenBrightness;

    Boolean mDebug;
};

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_DREAMS_DREAMSERVICE_H__
