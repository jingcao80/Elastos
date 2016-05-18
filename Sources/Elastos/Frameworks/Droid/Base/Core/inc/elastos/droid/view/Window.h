
#ifndef __ELASTOS_DROID_VIEW_WINDOW_H__
#define __ELASTOS_DROID_VIEW_WINDOW_H__

#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Media::Session::IMediaController;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Transition::IScene;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Transition::ITransitionManager;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class Window
    : public Object
    , public IWindow
{
    friend class CWindowManagerGlobal;

public:
    CAR_INTERFACE_DECL()

    Window();

    Window(
        /* [in] */ IContext* context);

    virtual ~Window();

    /**
     * Return the Context this window policy is running in, for retrieving
     * resources and other information.
     *
     * @return Context The Context that was supplied to the constructor.
     */
    CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * Return the {@link android.R.styleable#Window} attributes from this
     * window's theme.
     */
    CARAPI GetWindowStyle(
        /* [out] */ ITypedArray** attrs);

    /**
     * Set the container for this window.  If not set, the DecorWindow
     * operates as a top-level window; otherwise, it negotiates with the
     * container to display itself appropriately.
     *
     * @param container The desired containing Window.
     */
    CARAPI SetContainer(
        /* [in] */ IWindow* container);

    /**
     * Return the container for this Window.
     *
     * @return Window The containing window, or null if this is a
     *         top-level window.
     */
    CARAPI GetContainer(
        /* [out] */ IWindow** container);

    CARAPI HasChildren(
        /* [out] */  Boolean* hasChildren);

    /** @hide */
    CARAPI Destroy();

    /** @hide */
    CARAPI IsDestroyed(
        /* [out] */ Boolean* isDestroyed);

    /**
     * Set the window manager for use by this Window to, for example,
     * display panels.  This is <em>not</em> used for displaying the
     * Window itself -- that must be done by the client.
     *
     * @param wm The ViewManager for adding new windows.
     */
    CARAPI SetWindowManager(
        /* [in] */ IWindowManager* wm,
        /* [in] */ IBinder* appToken,
        /* [in] */ const String& appName);

    /**
     * Set the window manager for use by this Window to, for example,
     * display panels.  This is <em>not</em> used for displaying the
     * Window itself -- that must be done by the client.
     *
     * @param wm The ViewManager for adding new windows.
     */
    CARAPI SetWindowManager(
        /* [in] */ IWindowManager* wm,
        /* [in] */ IBinder* appToken,
        /* [in] */ const String& appName,
        /* [in] */ Boolean hardwareAccelerated);

    /**
     * Return the window manager allowing this Window to display its own
     * windows.
     *
     * @return WindowManager The ViewManager.
     */
    CARAPI GetWindowManager(
        /* [out] */ IWindowManager** wm);

    CARAPI SetCallback(
        /* [in] */ IWindowCallback* cb);

    CARAPI GetCallback(
        /* [out] */ IWindowCallback** cb);

    /** @hide */
    CARAPI SetOnWindowDismissedCallback(
        /* [in] */ IOnWindowDismissedCallback* dcb);

    /** @hide */
    CARAPI DispatchOnWindowDismissed();

    /**
     * Take ownership of this window's surface.  The window's view hierarchy
     * will no longer draw into the surface, though it will otherwise continue
     * to operate (such as for receiving input events).  The given SurfaceHolder
     * callback will be used to tell you about state changes to the surface.
     */
    CARAPI TakeSurface(
        /* [in] */ ISurfaceHolderCallback2* cb) = 0;

    /**
     * Take ownership of this window's InputQueue.  The window will no
     * longer read and dispatch input events from the queue; it is your
     * responsibility to do so.
     */
    CARAPI TakeInputQueue(
        /* [in] */ IInputQueueCallback* cb) = 0;

    /**
     * Return whether this window is being displayed with a floating style
     * (based on the {@link android.R.attr#windowIsFloating} attribute in
     * the style/theme).
     *
     * @return Returns true if the window is configured to be displayed floating
     * on top of whatever is behind it.
     */
    CARAPI IsFloating(
        /* [out] */ Boolean* isFloating) = 0;

    /**
     * Set the width and height layout parameters of the window.  The default
     * for both of these is MATCH_PARENT; you can change them to WRAP_CONTENT to
     * make a window that is not full-screen.
     *
     * @param width The desired layout width of the window.
     * @param height The desired layout height of the window.
     */
    CARAPI SetLayout(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Set the gravity of the window, as per the Gravity constants.  This
     * controls how the window manager is positioned in the overall window; it
     * is only useful when using WRAP_CONTENT for the layout width or height.
     *
     * @param gravity The desired gravity constant.
     *
     * @see Gravity
     * @see #setLayout
     */
    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    /**
     * Set the type of the window, as per the WindowManager.LayoutParams
     * types.
     *
     * @param type The new window type (see WindowManager.LayoutParams).
     */
    CARAPI SetType(
        /* [in] */ Int32 type);

    /**
     * Set the format of window, as per the PixelFormat types.  This overrides
     * the default format that is selected by the Window based on its
     * window decorations.
     *
     * @param format The new window format (see PixelFormat).  Use
     *               PixelFormat.UNKNOWN to allow the Window to select
     *               the format.
     *
     * @see PixelFormat
     */
    CARAPI SetFormat(
        /* [in] */ Int32 format);

    /**
     * Specify custom animations to use for the window, as per
     * {@link WindowManager.LayoutParams#windowAnimations
     * WindowManager.LayoutParams.windowAnimations}.  Providing anything besides
     * 0 here will override the animations the window would
     * normally retrieve from its theme.
     */
    CARAPI SetWindowAnimations(
        /* [in] */ Int32 resId);

    /**
     * Specify an explicit soft input mode to use for the window, as per
     * {@link WindowManager.LayoutParams#softInputMode
     * WindowManager.LayoutParams.softInputMode}.  Providing anything besides
     * "unspecified" here will override the input mode the window would
     * normally retrieve from its theme.
     */
    CARAPI SetSoftInputMode(
        /* [in] */ Int32 mode);

    /**
     * Convenience function to set the flag bits as specified in flags, as
     * per {@link #setFlags}.
     * @param flags The flag bits to be set.
     * @see #setFlags
     */
    CARAPI AddFlags(
        /* [in] */ Int32 flags);

    /** @hide */
    virtual CARAPI_(void) AddPrivateFlags(
        /* [in] */ Int32 flags);

    /** @hide */
    virtual CARAPI_(void) SetBlurMaskAlphaThreshold(
        /* [in] */ Float alpha);

    /**
     * Convenience function to clear the flag bits as specified in flags, as
     * per {@link #setFlags}.
     * @param flags The flag bits to be cleared.
     * @see #setFlags
     */
    CARAPI ClearFlags(
        /* [in] */ Int32 flags);

    /** @hide */
    virtual CARAPI_(void) ClearPrivateFlags(
        /* [in] */ Int32 flags);

    /**
     * Set the flags of the window, as per the
     * {@link WindowManager.LayoutParams WindowManager.LayoutParams}
     * flags.
     *
     * <p>Note that some flags must be set before the window decoration is
     * created (by the first call to
     * {@link #setContentView(View, android.view.ViewGroup.LayoutParams)} or
     * {@link #getDecorView()}:
     * {@link WindowManager.LayoutParams#FLAG_LAYOUT_IN_SCREEN} and
     * {@link WindowManager.LayoutParams#FLAG_LAYOUT_INSET_DECOR}.  These
     * will be set for you based on the {@link android.R.attr#windowIsFloating}
     * attribute.
     *
     * @param flags The new window flags (see WindowManager.LayoutParams).
     * @param mask Which of the window flag bits to modify.
     */
    CARAPI SetFlags(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

    /**
     * Set the amount of dim behind the window when using
     * {@link WindowManager.LayoutParams#FLAG_DIM_BEHIND}.  This overrides
     * the default dim amount of that is selected by the Window based on
     * its theme.
     *
     * @param amount The new dim amount, from 0 for no dim to 1 for full dim.
     */
    CARAPI SetDimAmount(
        /* [in] */ Float amount);

    /**
     * Specify custom window attributes.  <strong>PLEASE NOTE:</strong> the
     * layout params you give here should generally be from values previously
     * retrieved with {@link #getAttributes()}; you probably do not want to
     * blindly create and apply your own, since this will blow away any values
     * set by the framework that you are not interested in.
     *
     * @param a The new window attributes, which will completely override any
     *          current values.
     */
    CARAPI SetAttributes(
        /* [in] */ IWindowManagerLayoutParams* a);

    CARAPI GetAttributes(
        /* [out] */ IWindowManagerLayoutParams** params);

    /** @hide */
    CARAPI SetCloseOnTouchOutside(
        /* [in] */ Boolean close);

    /** @hide */
    CARAPI SetCloseOnTouchOutsideIfNotSet(
        /* [in] */ Boolean close);

    /** @hide */
    CARAPI AlwaysReadCloseOnTouchAttr() = 0;

    /** @hide */
    CARAPI ShouldCloseOnTouch(
        /* [in] */ IContext* context,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Enable extended screen features.  This must be called before
     * setContentView().  May be called as many times as desired as long as it
     * is before setContentView().  If not called, no extended features
     * will be available.  You can not turn off a feature once it is requested.
     * You canot use other title features with {@link #FEATURE_CUSTOM_TITLE}.
     *
     * @param featureId The desired features, defined as constants by Window.
     * @return The features that are now set.
     */
    CARAPI RequestFeature(
        /* [in] */ Int32 featureId,
        /* [out] */ Boolean* result);

    CARAPI MakeActive();

    CARAPI IsActive(
        /* [out] */ Boolean* isActive);

    CARAPI FindViewById(
        /* [in] */ Int32 id,
        /* [out] */ IView** view);

    CARAPI SetContentView(
        /* [in] */ Int32 layoutResID) = 0;

    CARAPI SetContentView(
        /* [in] */ IView* view) = 0;

    CARAPI SetContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params) = 0;

    /**
     * Variation on
     * {@link #setContentView(View, android.view.ViewGroup.LayoutParams)}
     * to add an additional content view to the screen.  Added after any existing
     * ones in the screen -- existing views are NOT removed.
     *
     * @param view The desired content to display.
     * @param params Layout parameters for the view.
     */
    CARAPI AddContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params) = 0;

    /**
     * Return the view in this Window that currently has focus, or null if
     * there are none.  Note that this does not look in any containing
     * Window.
     *
     * @return View The current View with focus or null.
     */
    CARAPI GetCurrentFocus(
        /* [out] */ IView** view) = 0;

    /**
     * Quick access to the {@link LayoutInflater} instance that this Window
     * retrieved from its Context.
     *
     * @return LayoutInflater The shared LayoutInflater.
     */
    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater) = 0;

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title) = 0;

    CARAPI SetTitleColor(
        /* [in] */ Int32 textColor) = 0;

    CARAPI OpenPanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IKeyEvent* event) = 0;

    CARAPI ClosePanel(
        /* [in] */ Int32 featureId) = 0;

    CARAPI TogglePanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IKeyEvent* event) = 0;

    CARAPI InvalidatePanelMenu(
        /* [in] */ Int32 featureId) = 0;

    CARAPI PerformPanelShortcut(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded) = 0;

    CARAPI PerformPanelIdentifierAction(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 id,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded) = 0;

    CARAPI CloseAllPanels() = 0;

    CARAPI PerformContextMenuIdentifierAction(
        /* [in] */ Int32 id,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded) = 0;

    /**
     * Should be called when the configuration is changed.
     *
     * @param newConfig The new configuration.
     */
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig) = 0;

    /**
     * Change the background of this window to a Drawable resource. Setting the
     * background to null will make the window be opaque. To make the window
     * transparent, you can use an empty drawable (for instance a ColorDrawable
     * with the color 0 or the system drawable android:drawable/empty.)
     *
     * @param resid The resource identifier of a drawable resource which will be
     *              installed as the new background.
     */
    CARAPI SetBackgroundDrawableResource(
        /* [in] */ Int32 resid);

    /**
     * Change the background of this window to a custom Drawable. Setting the
     * background to null will make the window be opaque. To make the window
     * transparent, you can use an empty drawable (for instance a ColorDrawable
     * with the color 0 or the system drawable android:drawable/empty.)
     *
     * @param drawable The new Drawable to use for this window's background.
     */
    CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* drawable) = 0;

    /**
     * Set the value for a drawable feature of this window, from a resource
     * identifier.  You must have called requestFeauture(featureId) before
     * calling this function.
     *
     * @see android.content.res.Resources#getDrawable(int)
     *
     * @param featureId The desired drawable feature to change, defined as a
     * constant by Window.
     * @param resId Resource identifier of the desired image.
     */
    CARAPI SetFeatureDrawableResource(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 resId) = 0;

    /**
     * Set the value for a drawable feature of this window, from a URI. You
     * must have called requestFeature(featureId) before calling this
     * function.
     *
     * <p>The only URI currently supported is "content:", specifying an image
     * in a content provider.
     *
     * @see android.widget.ImageView#setImageURI
     *
     * @param featureId The desired drawable feature to change. Features are
     * constants defined by Window.
     * @param uri The desired URI.
     */
    CARAPI SetFeatureDrawableUri(
        /* [in] */ Int32 featureId,
        /* [in] */ IUri* uri) = 0;

    /**
     * Set an explicit Drawable value for feature of this window. You must
     * have called requestFeature(featureId) before calling this function.
     *
     * @param featureId The desired drawable feature to change.
     * Features are constants defined by Window.
     * @param drawable A Drawable object to display.
     */
    CARAPI SetFeatureDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable) = 0;

    /**
     * Set a custom alpha value for the given drawale feature, controlling how
     * much the background is visible through it.
     *
     * @param featureId The desired drawable feature to change.
     * Features are constants defined by Window.
     * @param alpha The alpha amount, 0 is completely transparent and 255 is
     *              completely opaque.
     */
    CARAPI SetFeatureDrawableAlpha(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 alpha) = 0;

    /**
     * Set the integer value for a feature.  The range of the value depends on
     * the feature being set.  For FEATURE_PROGRESSS, it should go from 0 to
     * 10000. At 10000 the progress is complete and the indicator hidden.
     *
     * @param featureId The desired feature to change.
     * Features are constants defined by Window.
     * @param value The value for the feature.  The interpretation of this
     *              value is feature-specific.
     */
    CARAPI SetFeatureInt(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 value) = 0;

    /**
     * Request that key events come to this activity. Use this if your
     * activity has no views with focus, but the activity still wants
     * a chance to process key events.
     */
    CARAPI TakeKeyEvents(
        /* [in] */ Boolean get) = 0;

    /**
     * Used by custom windows, such as Dialog, to pass the key press event
     * further down the view hierarchy. Application developers should
     * not need to implement or call this.
     *
     */
    CARAPI SuperDispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* succeeded) = 0;

    /**
     * Used by custom windows, such as Dialog, to pass the key shortcut press event
     * further down the view hierarchy. Application developers should
     * not need to implement or call this.
     *
     */
    CARAPI SuperDispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* succeeded) = 0;

    /**
     * Used by custom windows, such as Dialog, to pass the touch screen event
     * further down the view hierarchy. Application developers should
     * not need to implement or call this.
     *
     */
    CARAPI SuperDispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* succeeded) = 0;

    /**
     * Used by custom windows, such as Dialog, to pass the trackball event
     * further down the view hierarchy. Application developers should
     * not need to implement or call this.
     *
     */
    CARAPI SuperDispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* succeeded) = 0;

    /**
     * Used by custom windows, such as Dialog, to pass the generic motion event
     * further down the view hierarchy. Application developers should
     * not need to implement or call this.
     *
     */
    CARAPI SuperDispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* succeeded) = 0;

    CARAPI GetDecorView(
        /* [out] */ IView** view) = 0;

    CARAPI PeekDecorView(
        /* [out] */ IView** view) = 0;

    CARAPI SaveHierarchyState(
        /* [out] */ IBundle** instanceState) = 0;

    CARAPI RestoreHierarchyState(
        /* [in] */ IBundle* savedInstanceState) = 0;

    /**
     * Query for the availability of a certain feature.
     *
     * @param feature The feature ID to check
     * @return true if the feature is enabled, false otherwise.
     */
    CARAPI HasFeature(
        /* [in] */ Int32 feature,
        /* [out] */ Boolean* hasFeature);

    CARAPI SetChildDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable) = 0;

    CARAPI SetChildInt(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 value) = 0;

    /**
     * Is a keypress one of the defined shortcut keys for this window.
     * @param keyCode the key code from {@link android.view.KeyEvent} to check.
     * @param event the {@link android.view.KeyEvent} to use to help check.
     */
    CARAPI IsShortcutKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* isShortcutKey) = 0;

    /**
     * @see android.app.Activity#setVolumeControlStream(int)
     */
    CARAPI SetVolumeControlStream(
        /* [in] */ Int32 streamType) = 0;

    /**
     * @see android.app.Activity#getVolumeControlStream()
     */
    CARAPI GetVolumeControlStream(
        /* [out] */ Int32* streamType) = 0;

    /**
     * Sets a {@link MediaController} to send media keys and volume changes to.
     * If set, this should be preferred for all media keys and volume requests
     * sent to this window.
     *
     * @param controller The controller for the session which should receive
     *            media keys and volume changes.
     * @see android.app.Activity#setMediaController(android.media.session.MediaController)
     */
    CARAPI SetMediaController(
        /* [in] */ IMediaController* controller);

    /**
     * Gets the {@link MediaController} that was previously set.
     *
     * @return The controller which should receive events.
     * @see #setMediaController(android.media.session.MediaController)
     * @see android.app.Activity#getMediaController()
     */
    CARAPI GetMediaController(
        /* [out] */ IMediaController** controller);

    /**
     * Set extra options that will influence the UI for this window.
     * @param uiOptions Flags specifying extra options for this window.
     */
    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions);

    /**
     * Set extra options that will influence the UI for this window.
     * Only the bits filtered by mask will be modified.
     * @param uiOptions Flags specifying extra options for this window.
     * @param mask Flags specifying which options should be modified. Others will remain unchanged.
     */
    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions,
        /* [in] */ Int32 mask);

    CARAPI AdjustLayoutParamsForSubWindow(
        /* [in] */ IWindowManagerLayoutParams* wp);

    /**
     * Set the primary icon for this window.
     *
     * @param resId resource ID of a drawable to set
     */
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    /**
     * Set the default icon for this window.
     * This will be overridden by any other icon set operation which could come from the
     * theme or another explicit set.
     *
     * @hide
     */
    CARAPI SetDefaultIcon(
        /* [in] */ Int32 resId);

    /**
     * Set the logo for this window. A logo is often shown in place of an
     * {@link #setIcon(int) icon} but is generally wider and communicates window title information
     * as well.
     *
     * @param resId resource ID of a drawable to set
     */
    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    /**
     * Set the default logo for this window.
     * This will be overridden by any other logo set operation which could come from the
     * theme or another explicit set.
     *
     * @hide
     */
    CARAPI SetDefaultLogo(
        /* [in] */ Int32 resId);

    /**
     * Set focus locally. The window should have the
     * {@link WindowManager.LayoutParams#FLAG_LOCAL_FOCUS_MODE} flag set already.
     * @param hasFocus Whether this window has focus or not.
     * @param inTouchMode Whether this window is in touch mode or not.
     */
    CARAPI SetLocalFocus(
        /* [in] */ Boolean hasFocus,
        /* [in] */ Boolean inTouchMode);

    /**
     * Inject an event to window locally.
     * @param event A key or touch event to inject to this window.
     */
    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* event);

    /**
     * Retrieve the {@link TransitionManager} responsible for  for default transitions
     * in this window. Requires {@link #FEATURE_CONTENT_TRANSITIONS}.
     *
     * <p>This method will return non-null after content has been initialized (e.g. by using
     * {@link #setContentView}) if {@link #FEATURE_CONTENT_TRANSITIONS} has been granted.</p>
     *
     * @return This window's content TransitionManager or null if none is set.
     */
    CARAPI GetTransitionManager(
        /* [out] */ ITransitionManager** tm);

    /**
     * Set the {@link TransitionManager} to use for default transitions in this window.
     * Requires {@link #FEATURE_CONTENT_TRANSITIONS}.
     *
     * @param tm The TransitionManager to use for scene changes.
     */
    CARAPI SetTransitionManager(
        /* [in] */ ITransitionManager* tm);

    /**
     * Retrieve the {@link Scene} representing this window's current content.
     * Requires {@link #FEATURE_CONTENT_TRANSITIONS}.
     *
     * <p>This method will return null if the current content is not represented by a Scene.</p>
     *
     * @return Current Scene being shown or null
     */
    CARAPI GetContentScene(
        /* [out] */ IScene** scene);

    /**
     * Sets the Transition that will be used to move Views into the initial scene. The entering
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#INVISIBLE} to {@link View#VISIBLE}. If <code>transition</code> is null,
     * entering Views will remain unaffected.
     *
     * @param transition The Transition to use to move Views into the initial Scene.
     * @attr ref android.R.styleable#Window_windowEnterTransition
     */
    CARAPI SetEnterTransition(
        /* [in] */ ITransition* transition);

    /**
     * Sets the Transition that will be used to move Views out of the scene when the Window is
     * preparing to close, for example after a call to
     * {@link android.app.Activity#finishAfterTransition()}. The exiting
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#VISIBLE} to {@link View#INVISIBLE}. If <code>transition</code> is null,
     * entering Views will remain unaffected. If nothing is set, the default will be to
     * use the same value as set in {@link #setEnterTransition(android.transition.Transition)}.
     *
     * @param transition The Transition to use to move Views out of the Scene when the Window
     *                   is preparing to close.
     * @attr ref android.R.styleable#Window_windowReturnTransition
     */
    CARAPI SetReturnTransition(
        /* [in] */ ITransition* transition);

    /**
     * Sets the Transition that will be used to move Views out of the scene when starting a
     * new Activity. The exiting Views will be those that are regular Views or ViewGroups that
     * have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as exiting is governed by changing visibility
     * from {@link View#VISIBLE} to {@link View#INVISIBLE}. If transition is null, the views will
     * remain unaffected. Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param transition The Transition to use to move Views out of the scene when calling a
     *                   new Activity.
     * @attr ref android.R.styleable#Window_windowExitTransition
     */
    CARAPI SetExitTransition(
        /* [in] */ ITransition* transition);

    /**
     * Sets the Transition that will be used to move Views in to the scene when returning from
     * a previously-started Activity. The entering Views will be those that are regular Views
     * or ViewGroups that have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions
     * will extend {@link android.transition.Visibility} as exiting is governed by changing
     * visibility from {@link View#VISIBLE} to {@link View#INVISIBLE}. If transition is null,
     * the views will remain unaffected. If nothing is set, the default will be to use the same
     * transition as {@link #setExitTransition(android.transition.Transition)}.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param transition The Transition to use to move Views into the scene when reentering from a
     *                   previously-started Activity.
     * @attr ref android.R.styleable#Window_windowReenterTransition
     */
    CARAPI SetReenterTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the transition used to move Views into the initial scene. The entering
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#INVISIBLE} to {@link View#VISIBLE}. If <code>transition</code> is null,
     * entering Views will remain unaffected.  Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @return the Transition to use to move Views into the initial Scene.
     * @attr ref android.R.styleable#Window_windowEnterTransition
     */
    CARAPI GetEnterTransition(
        /* [out] */ ITransition** transition);

    /**
     * Returns he Transition that will be used to move Views out of the scene when the Window is
     * preparing to close, for example after a call to
     * {@link android.app.Activity#finishAfterTransition()}. The exiting
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#VISIBLE} to {@link View#INVISIBLE}.
     *
     * @return The Transition to use to move Views out of the Scene when the Window
     *         is preparing to close.
     * @attr ref android.R.styleable#Window_windowReturnTransition
     */
    CARAPI GetReturnTransition(
        /* [out] */ ITransition** transition);

    /**
     * Returns the Transition that will be used to move Views out of the scene when starting a
     * new Activity. The exiting Views will be those that are regular Views or ViewGroups that
     * have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as exiting is governed by changing visibility
     * from {@link View#VISIBLE} to {@link View#INVISIBLE}. If transition is null, the views will
     * remain unaffected. Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @return the Transition to use to move Views out of the scene when calling a
     * new Activity.
     * @attr ref android.R.styleable#Window_windowExitTransition
     */
    CARAPI GetExitTransition(
        /* [out] */ ITransition** transition);

    /**
     * Returns the Transition that will be used to move Views in to the scene when returning from
     * a previously-started Activity. The entering Views will be those that are regular Views
     * or ViewGroups that have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions
     * will extend {@link android.transition.Visibility} as exiting is governed by changing
     * visibility from {@link View#VISIBLE} to {@link View#INVISIBLE}.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @return The Transition to use to move Views into the scene when reentering from a
     *         previously-started Activity.
     * @attr ref android.R.styleable#Window_windowReenterTransition
     */
    CARAPI GetReenterTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets the Transition that will be used for shared elements transferred into the content
     * Scene. Typical Transitions will affect size and location, such as
     * {@link android.transition.ChangeBounds}. A null
     * value will cause transferred shared elements to blink to the final position.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param transition The Transition to use for shared elements transferred into the content
     *                   Scene.
     * @attr ref android.R.styleable#Window_windowSharedElementEnterTransition
     */
    CARAPI SetSharedElementEnterTransition(
        /* [in] */ ITransition* transition);

    /**
     * Sets the Transition that will be used for shared elements transferred back to a
     * calling Activity. Typical Transitions will affect size and location, such as
     * {@link android.transition.ChangeBounds}. A null
     * value will cause transferred shared elements to blink to the final position.
     * If no value is set, the default will be to use the same value as
     * {@link #setSharedElementEnterTransition(android.transition.Transition)}.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param transition The Transition to use for shared elements transferred out of the content
     *                   Scene.
     * @attr ref android.R.styleable#Window_windowSharedElementReturnTransition
     */
    CARAPI SetSharedElementReturnTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the Transition that will be used for shared elements transferred into the content
     * Scene. Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @return Transition to use for sharend elements transferred into the content Scene.
     * @attr ref android.R.styleable#Window_windowSharedElementEnterTransition
     */
    CARAPI GetSharedElementEnterTransition(
        /* [out] */ ITransition** transition);

    /**
     * Returns the Transition that will be used for shared elements transferred back to a
     * calling Activity. Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @return Transition to use for sharend elements transferred into the content Scene.
     * @attr ref android.R.styleable#Window_windowSharedElementReturnTransition
     */
    CARAPI GetSharedElementReturnTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets the Transition that will be used for shared elements after starting a new Activity
     * before the shared elements are transferred to the called Activity. If the shared elements
     * must animate during the exit transition, this Transition should be used. Upon completion,
     * the shared elements may be transferred to the started Activity.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param transition The Transition to use for shared elements in the launching Window
     *                   prior to transferring to the launched Activity's Window.
     * @attr ref android.R.styleable#Window_windowSharedElementExitTransition
     */
    CARAPI SetSharedElementExitTransition(
        /* [in] */ ITransition* transition);

    /**
     * Sets the Transition that will be used for shared elements reentering from a started
     * Activity after it has returned the shared element to it start location. If no value
     * is set, this will default to
     * {@link #setSharedElementExitTransition(android.transition.Transition)}.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param transition The Transition to use for shared elements in the launching Window
     *                   after the shared element has returned to the Window.
     * @attr ref android.R.styleable#Window_windowSharedElementReenterTransition
     */
    CARAPI SetSharedElementReenterTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the Transition to use for shared elements in the launching Window prior
     * to transferring to the launched Activity's Window.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @return the Transition to use for shared elements in the launching Window prior
     * to transferring to the launched Activity's Window.
     * @attr ref android.R.styleable#Window_windowSharedElementExitTransition
     */
    CARAPI GetSharedElementExitTransition(
        /* [out] */ ITransition** transition);

    /**
     * Returns the Transition that will be used for shared elements reentering from a started
     * Activity after it has returned the shared element to it start location.
     * Requires {@link #FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @return the Transition that will be used for shared elements reentering from a started
     * Activity after it has returned the shared element to it start location.
     * @attr ref android.R.styleable#Window_windowSharedElementReenterTransition
     */
    CARAPI GetSharedElementReenterTransition(
        /* [out] */ ITransition** transition);

    /**
     * Controls how the transition set in
     * {@link #setEnterTransition(android.transition.Transition)} overlaps with the exit
     * transition of the calling Activity. When true, the transition will start as soon as possible.
     * When false, the transition will wait until the remote exiting transition completes before
     * starting.
     *
     * @param allow true to start the enter transition when possible or false to
     *              wait until the exiting transition completes.
     * @attr ref android.R.styleable#Window_windowAllowEnterTransitionOverlap
     */
    CARAPI SetAllowEnterTransitionOverlap(
        /* [in] */ Boolean allow);

    /**
     * Returns how the transition set in
     * {@link #setEnterTransition(android.transition.Transition)} overlaps with the exit
     * transition of the calling Activity. When true, the transition will start as soon as possible.
     * When false, the transition will wait until the remote exiting transition completes before
     * starting.
     *
     * @return true when the enter transition should start as soon as possible or false to
     * when it should wait until the exiting transition completes.
     * @attr ref android.R.styleable#Window_windowAllowEnterTransitionOverlap
     */
    CARAPI GetAllowEnterTransitionOverlap(
        /* [out] */ Boolean* allow);

    /**
     * Controls how the transition set in
     * {@link #setExitTransition(android.transition.Transition)} overlaps with the exit
     * transition of the called Activity when reentering after if finishes. When true,
     * the transition will start as soon as possible. When false, the transition will wait
     * until the called Activity's exiting transition completes before starting.
     *
     * @param allow true to start the transition when possible or false to wait until the
     *              called Activity's exiting transition completes.
     * @attr ref android.R.styleable#Window_windowAllowReturnTransitionOverlap
     */
    CARAPI SetAllowReturnTransitionOverlap(
        /* [in] */ Boolean allow);

    /**
     * TODO: remove this.
     * @hide
     */
    CARAPI SetAllowExitTransitionOverlap(
        /* [in] */ Boolean allow);

    /**
     * Returns how the transition set in
     * {@link #setExitTransition(android.transition.Transition)} overlaps with the exit
     * transition of the called Activity when reentering after if finishes. When true,
     * the transition will start as soon as possible. When false, the transition will wait
     * until the called Activity's exiting transition completes before starting.
     *
     * @return true when the transition should start when possible or false when it should wait
     * until the called Activity's exiting transition completes.
     * @attr ref android.R.styleable#Window_windowAllowReturnTransitionOverlap
     */
    CARAPI GetAllowReturnTransitionOverlap(
        /* [out] */ Boolean* allow);

    /**
     * TODO: remove this.
     * @hide
     */
    CARAPI GetAllowExitTransitionOverlap(
        /* [out] */ Boolean* allow);

    /**
     * Returns the duration, in milliseconds, of the window background fade
     * when transitioning into or away from an Activity when called with an Activity Transition.
     * <p>When executing the enter transition, the background starts transparent
     * and fades in. This requires {@link #FEATURE_ACTIVITY_TRANSITIONS}. The default is
     * 300 milliseconds.</p>
     *
     * @return The duration of the window background fade to opaque during enter transition.
     * @see #getEnterTransition()
     * @attr ref android.R.styleable#Window_windowTransitionBackgroundFadeDuration
     */
    CARAPI GetTransitionBackgroundFadeDuration(
        /* [out] */ Int64* duration);

    /**
     * Sets the duration, in milliseconds, of the window background fade
     * when transitioning into or away from an Activity when called with an Activity Transition.
     * <p>When executing the enter transition, the background starts transparent
     * and fades in. This requires {@link #FEATURE_ACTIVITY_TRANSITIONS}. The default is
     * 300 milliseconds.</p>
     *
     * @param fadeDurationMillis The duration of the window background fade to or from opaque
     *                           during enter transition.
     * @see #setEnterTransition(android.transition.Transition)
     * @attr ref android.R.styleable#Window_windowTransitionBackgroundFadeDuration
     */
    CARAPI SetTransitionBackgroundFadeDuration(
        /* [in] */ Int64 fadeDurationMillis);

    /**
     * Returns <code>true</code> when shared elements should use an Overlay during
     * shared element transitions or <code>false</code> when they should animate as
     * part of the normal View hierarchy. The default value is true.
     *
     * @return <code>true</code> when shared elements should use an Overlay during
     * shared element transitions or <code>false</code> when they should animate as
     * part of the normal View hierarchy.
     * @attr ref android.R.styleable#Window_windowSharedElementsUseOverlay
     */
    CARAPI GetSharedElementsUseOverlay(
        /* [out] */ Boolean* shared);

    /**
     * Sets whether or not shared elements should use an Overlay during shared element transitions.
     * The default value is true.
     *
     * @param sharedElementsUseOverlay <code>true</code> indicates that shared elements should
     *                                 be transitioned with an Overlay or <code>false</code>
     *                                 to transition within the normal View hierarchy.
     * @attr ref android.R.styleable#Window_windowSharedElementsUseOverlay
     */
    CARAPI SetSharedElementsUseOverlay(
        /* [in] */ Boolean sharedElementsUseOverlay);

protected:
    /**
     * Return the window flags that have been explicitly set by the client,
     * so will not be modified by {@link #getDecorView}.
     */
    CARAPI_(Int32) GetForcedWindowFlags();

    /**
     * Has the app specified their own soft input mode?
     */
    CARAPI_(Boolean) HasSoftInputMode();

    /**
     * @hide Used internally to help resolve conflicting features.
     */
    virtual CARAPI_(void) RemoveFeature(
        /* [in] */ Int32 featureId);

    virtual CARAPI_(void) OnActive() = 0;

    /**
     * Return the feature bits that are enabled.  This is the set of features
     * that were given to requestFeature(), and are being handled by this
     * Window itself or its container.  That is, it is the set of
     * requested features that you can actually use.
     *
     * <p>To do: add a public version of this API that allows you to check for
     * features by their feature ID.
     *
     * @return int The feature bits.
     */
    CARAPI_(Int32) GetFeatures();

    /**
     * Return the feature bits that are being implemented by this Window.
     * This is the set of features that were given to requestFeature(), and are
     * being handled by only this Window itself, not by its containers.
     *
     * @return int The feature bits.
     */
    CARAPI_(Int32) GetLocalFeatures();

    /**
     * Set the default format of window, as per the PixelFormat types.  This
     * is the format that will be used unless the client specifies in explicit
     * format with setFormat();
     *
     * @param format The new window format (see PixelFormat).
     *
     * @see #setFormat
     * @see PixelFormat
     */
    virtual CARAPI_(void) SetDefaultWindowFormat(
        /* [in] */ Int32 format);

    /** @hide */
    virtual CARAPI_(Boolean) HaveDimAmount();

    /**
     * {@hide}
     */
    virtual CARAPI_(void) DispatchWindowAttributesChanged(
        /* [in] */ IWindowManagerLayoutParams* attrs);

private:
    CARAPI_(Boolean) IsOutOfBounds(
        /* [in] */ IContext* context,
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) SetPrivateFlags(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

protected:
    static const Int32 DEFAULT_FEATURES;
    AutoPtr<ITypedArray> mWindowStyle;
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IBinder> mAppToken;
    String mAppName;
    AutoPtr<Window> mContainer;
    AutoPtr<Window> mActiveChild;

    Int32 mForcedWindowFlags;

    Int32 mFeatures;
    Int32 mLocalFeatures;
    Int32 mDefaultWindowFormat;

    // The current window attributes.
    AutoPtr<CWindowManagerLayoutParams> mWindowAttributes;

    IWindowCallback* mCallback;

    AutoPtr<IOnWindowDismissedCallback> mOnWindowDismissedCallback;

    // if mContext is IActiviy, this must not have it's reference
    //
    IContext* mContext;
    Boolean mRetainContext;

    Boolean mHaveWindowFormat;
    Boolean mHaveDimAmount;
    Boolean mHasSoftInputMode;
    Boolean mDestroyed;

    Boolean mHardwareAccelerated;
    Boolean mIsActive;
    Boolean mHasChildren;
    Boolean mCloseOnTouchOutside;
    Boolean mSetCloseOnTouchOutside;

private:
    static const String PROPERTY_HARDWARE_UI;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_WINDOW_H__
