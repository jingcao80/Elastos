
#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_INPUTMETHODSERVICE_H__
#define  __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_INPUTMETHODSERVICE_H__

#ifdef DROID_CORE
#include "elastos/droid/inputmethodservice/CSoftInputWindow.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodImpl.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodService.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodSessionImpl.h"
#include "elastos/droid/view/ViewTreeObserver.h"
#else
#include "Elastos.Droid.Core.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodService.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodImpl.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodSessionImpl.h"
#endif

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IInternalInsetsInfo;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IOnComputeInternalInsetsListener;
using Elastos::Droid::View::IViewOnClickListener;;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::IInputBinding;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::ICursorAnchorInfo;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IFrameLayout;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

class InputMethodService
    : public AbstractInputMethodService
    , public IInputMethodService
{
private:
    class _OnComputeInternalInsetsListener
        : public Object
        , public IOnComputeInternalInsetsListener
    {
    public:
        CAR_INTERFACE_DECL();

        _OnComputeInternalInsetsListener(
            /* [in] */ InputMethodService* host);

        ~_OnComputeInternalInsetsListener();

        CARAPI OnComputeInternalInsets(
            /* [in] */ IInternalInsetsInfo* info);

    private:
        InputMethodService* mHost;
    };

    class _OnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        _OnClickListener(
            /* [in] */ InputMethodService* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        InputMethodService* mHost;
    };

public:
    /**
     * Concrete implementation of
     * {@link AbstractInputMethodService.AbstractInputMethodImpl} that provides
     * all of the standard behavior for an input method.
     */
    class InputMethodImpl : public AbstractInputMethodImpl
    {
    public:
        InputMethodImpl(
            /* [in] */ InputMethodService* host);

        ~InputMethodImpl();

        /**
         * Take care of attaching the given window token provided by the system.
         */
        CARAPI AttachToken(
            /* [in] */ IBinder* token);

        /**
         * Handle a new input binding, calling
         * {@link InputMethodService#onBindInput InputMethodService.onBindInput()}
         * when done.
         */
        CARAPI BindInput(
            /* [in] */ IInputBinding* binding);

        /**
         * Clear the current input binding.
         */
        CARAPI UnbindInput();

        CARAPI StartInput(
            /* [in] */ IInputConnection* ic,
            /* [in] */ IEditorInfo* attribute);

        CARAPI RestartInput(
            /* [in] */ IInputConnection* ic,
            /* [in] */ IEditorInfo* attribute);

        /**
         * Handle a request by the system to hide the soft input area.
         */
        CARAPI HideSoftInput(
            /* [in] */ Int32 flags,
            /* [in] */ IResultReceiver* resultReceiver);

        /**
         * Handle a request by the system to show the soft input area.
         */
        CARAPI ShowSoftInput(
            /* [in] */ Int32 flags,
            /* [in] */ IResultReceiver* resultReceiver);

        CARAPI ChangeInputMethodSubtype(
            /* [in] */ IInputMethodSubtype* subtype);

    private:
        InputMethodService* mHost;
    };

    /**
     * Concrete implementation of
     * {@link AbstractInputMethodService.AbstractInputMethodSessionImpl} that provides
     * all of the standard behavior for an input method session.
     */
    class InputMethodSessionImpl : public AbstractInputMethodSessionImpl
    {
    public:
        InputMethodSessionImpl(
            /* [in] */ InputMethodService* host);

        ~InputMethodSessionImpl();

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI FinishInput();

        /**
         * Call {@link InputMethodService#onDisplayCompletions
         * InputMethodService.onDisplayCompletions()}.
         */
        CARAPI DisplayCompletions(
            /* [in] */ ArrayOf<ICompletionInfo*>* completions);

        /**
         * Call {@link InputMethodService#onUpdateExtractedText
         * InputMethodService.onUpdateExtractedText()}.
         */
        CARAPI UpdateExtractedText(
            /* [in] */ Int32 token,
            /* [in] */ IExtractedText* text);

        /**
         * Call {@link InputMethodService#onUpdateSelection
         * InputMethodService.onUpdateSelection()}.
         */
        CARAPI UpdateSelection(
            /* [in] */ Int32 oldSelStart,
            /* [in] */ Int32 oldSelEnd,
            /* [in] */ Int32 newSelStart,
            /* [in] */ Int32 newSelEnd,
            /* [in] */ Int32 candidatesStart,
            /* [in] */ Int32 candidatesEnd);

        CARAPI ViewClicked(
            /* [in] */ Boolean focusChanged);

        /**
         * Call {@link InputMethodService#onUpdateCursor
         * InputMethodService.onUpdateCursor()}.
         */
        CARAPI UpdateCursor(
            /* [in] */ IRect* newCursor);

        /**
         * Call {@link InputMethodService#onAppPrivateCommand
         * InputMethodService.onAppPrivateCommand()}.
         */
        CARAPI AppPrivateCommand(
            /* [in] */ const String& action,
            /* [in] */ IBundle* data);

        /**
         *
         */
        CARAPI ToggleSoftInput(
            /* [in] */ Int32 showFlags,
            /* [in] */ Int32 hideFlags);

        /**
         * Call {@link InputMethodService#onUpdateCursorAnchorInfo
         * InputMethodService.onUpdateCursorAnchorInfo()}.
         */
        CARAPI UpdateCursorAnchorInfo(
            /* [in] */ ICursorAnchorInfo* info);

    private:
        InputMethodService* mHost;
    };

    /**
     * Information about where interesting parts of the input method UI appear.
     */
    // static final class Insets {
    class Insets : public Object
    {
    public:
        Insets();

    public:
        /**
         * This is the top part of the UI that is the main content.  It is
         * used to determine the basic space needed, to resize/pan the
         * application behind.  It is assumed that this inset does not
         * change very much, since any change will cause a full resize/pan
         * of the application behind.  This value is relative to the top edge
         * of the input method window.
         */
        Int32 mContentTopInsets;

        /**
         * This is the top part of the UI that is visibly covering the
         * application behind it.  This provides finer-grained control over
         * visibility, allowing you to change it relatively frequently (such
         * as hiding or showing candidates) without disrupting the underlying
         * UI too much.  For example, this will never resize the application
         * UI, will only pan if needed to make the current focus visible, and
         * will not aggressively move the pan position when this changes unless
         * needed to make the focus visible.  This value is relative to the top edge
         * of the input method window.
         */
        Int32 mVisibleTopInsets;

        /**
         * This is the region of the UI that is touchable.  It is used when
         * {@link #touchableInsets} is set to {@link #TOUCHABLE_INSETS_REGION}.
         * The region should be specified relative to the origin of the window frame.
         */
        AutoPtr<IRegion> mTouchableRegion;

        /**
         * Option for {@link #touchableInsets}: the entire window frame
         * can be touched.
         */
        static const Int32 TOUCHABLE_INSETS_FRAME;

        /**
         * Option for {@link #touchableInsets}: the area inside of
         * the content insets can be touched.
         */
        static const Int32 TOUCHABLE_INSETS_CONTENT;

        /**
         * Option for {@link #touchableInsets}: the area inside of
         * the visible insets can be touched.
         */
        static const Int32 TOUCHABLE_INSETS_VISIBLE;

        /**
         * Option for {@link #touchableInsets}: the region specified by
         * {@link #touchableRegion} can be touched.
         */
        static const Int32 TOUCHABLE_INSETS_REGION;

        /**
         * Determine which area of the window is touchable by the user.  May
         * be one of: {@link #TOUCHABLE_INSETS_FRAME},
         * {@link #TOUCHABLE_INSETS_CONTENT}, {@link #TOUCHABLE_INSETS_VISIBLE},
         * or {@link #TOUCHABLE_INSETS_REGION}.
         */
        Int32 mTouchableInsets;
    };

public:
    CAR_INTERFACE_DECL();

    InputMethodService();

    virtual ~InputMethodService();

    /**
     * You can call this to customize the theme used by your IME's window.
     * This theme should typically be one that derives from
     * {@link android.R.style#Theme_InputMethod}, which is the default theme
     * you will get.  This must be set before {@link #onCreate}, so you
     * will typically call it in your constructor with the resource ID
     * of your custom theme.
     */
    virtual CARAPI SetTheme(
        /* [in] */ Int32 theme);

    /**
     * You can call this to try to enable hardware accelerated drawing for
     * your IME. This must be set before {@link #onCreate}, so you
     * will typically call it in your constructor.  It is not always possible
     * to use hardware accelerated drawing in an IME (for example on low-end
     * devices that do not have the resources to support this), so the call
     * returns true if it succeeds otherwise false if you will need to draw
     * in software.  You must be able to handle either case.
     *
     * @deprecated Starting in API 21, hardware acceleration is always enabled
     *             on capable devices.
     */
    CARAPI EnableHardwareAcceleration(
        /* [out] */ Boolean* enable);

    // @Override
    CARAPI OnCreate();

    /**
     * This is a hook that subclasses can use to perform initialization of
     * their interface.  It is called for you prior to any of your UI objects
     * being created, both after the service is first created and after a
     * configuration change happens.
     */
    virtual CARAPI OnInitializeInterface();

    /* package */ virtual CARAPI Initialize();

    // @Override
    CARAPI OnDestroy();

    /**
     * Take care of handling configuration changes.  Subclasses of
     * InputMethodService generally don't need to deal directly with
     * this on their own; the standard implementation here takes care of
     * regenerating the input method UI as a result of the configuration
     * change, so you can rely on your {@link #onCreateInputView} and
     * other methods being called as appropriate due to a configuration change.
     *
     * <p>When a configuration change does happen,
     * {@link #onInitializeInterface()} is guaranteed to be called the next
     * time prior to any of the other input or UI creation callbacks.  The
     * following will be called immediately depending if appropriate for current
     * state: {@link #onStartInput} if input is active, and
     * {@link #onCreateInputView} and {@link #onStartInputView} and related
     * appropriate functions if the UI is displayed.
     */
    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * Implement to return our standard {@link InputMethodImpl}.  Subclasses
     * can override to provide their own customized version.
     */
    virtual CARAPI OnCreateInputMethodInterface(
        /* [out] */ IAbstractInputMethodImpl** inputMethodImpl);

    /**
     * Implement to return our standard {@link InputMethodSessionImpl}.  Subclasses
     * can override to provide their own customized version.
     */
    virtual CARAPI OnCreateInputMethodSessionInterface(
        /* [out] */ IAbstractInputMethodSessionImpl** abstractInputMethodSessionImpl);

    virtual CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    virtual CARAPI GetWindow(
        /* [out] */ IDialog** dialog);

    virtual CARAPI SetBackDisposition(
        /* [in] */ Int32 disposition);

    virtual CARAPI GetBackDisposition(
        /* [out] */ Int32* pos);

    /**
     * Return the maximum width, in pixels, available the input method.
     * Input methods are positioned at the bottom of the screen and, unless
     * running in fullscreen, will generally want to be as short as possible
     * so should compute their height based on their contents.  However, they
     * can stretch as much as needed horizontally.  The function returns to
     * you the maximum amount of space available horizontally, which you can
     * use if needed for UI placement.
     *
     * <p>In many cases this is not needed, you can just rely on the normal
     * view layout mechanisms to position your views within the full horizontal
     * space given to the input method.
     *
     * <p>Note that this value can change dynamically, in particular when the
     * screen orientation changes.
     */
    virtual CARAPI GetMaxWidth(
        /* [out] */ Int32* maxWidth);

    /**
     * Return the currently active InputBinding for the input method, or
     * NULL if there is none.
     */
    virtual CARAPI GetCurrentInputBinding(
        /* [out] */ IInputBinding** binding);

    /**
     * Retrieve the currently active InputConnection that is bound to
     * the input method, or NULL if there is none.
     */
    virtual CARAPI GetCurrentInputConnection(
        /* [out] */ IInputConnection** inputConnection);

    virtual CARAPI GetCurrentInputStarted(
        /* [out] */ Boolean* state);

    virtual CARAPI GetCurrentInputEditorInfo(
        /* [out] */ IEditorInfo** editorInfo);

    /**
     * Re-evaluate whether the input method should be running in fullscreen
     * mode, and update its UI if this has changed since the last time it
     * was evaluated.  This will call {@link #onEvaluateFullscreenMode()} to
     * determine whether it should currently run in fullscreen mode.  You
     * can use {@link #isFullscreenMode()} to determine if the input method
     * is currently running in fullscreen mode.
     */
    virtual CARAPI UpdateFullscreenMode();

    /**
     * Update the given window's parameters for the given mode.  This is called
     * when the window is first displayed and each time the fullscreen or
     * candidates only mode changes.
     *
     * <p>The default implementation makes the layout for the window
     * MATCH_PARENT x MATCH_PARENT when in fullscreen mode, and
     * MATCH_PARENT x WRAP_CONTENT when in non-fullscreen mode.
     *
     * @param win The input method's window.
     * @param isFullscreen If TRUE, the window is running in fullscreen mode
     * and intended to cover the entire application display.
     * @param isCandidatesOnly If TRUE, the window is only showing the
     * candidates view and none of the rest of its UI.  This is mutually
     * exclusive with fullscreen mode.
     */
    virtual CARAPI OnConfigureWindow(
        /* [in] */ IWindow* win,
        /* [in] */ Boolean isFullscreen,
        /* [in] */ Boolean isCandidatesOnly);

    /**
     * Return whether the input method is <em>currently</em> running in
     * fullscreen mode.  This is the mode that was last determined and
     * applied by {@link #updateFullscreenMode()}.
     */
    virtual CARAPI IsFullscreenMode(
        /* [out] */ Boolean* screenMode);

    /**
     * Override this to control when the input method should run in
     * fullscreen mode.  The default implementation runs in fullsceen only
     * when the screen is in landscape mode.  If you change what
     * this returns, you will need to call {@link #updateFullscreenMode()}
     * yourself whenever the returned value may have changed to have it
     * re-evaluated and applied.
     */
    virtual CARAPI OnEvaluateFullscreenMode(
        /* [out] */ Boolean* screenMode);

    /**
     * Controls the visibility of the extracted text area.  This only applies
     * when the input method is in fullscreen mode, and thus showing extracted
     * text.  When FALSE, the extracted text will not be shown, allowing some
     * of the application to be seen behind.  This is normally set for you
     * by {@link #onUpdateExtractingVisibility}.  This controls the visibility
     * of both the extracted text and candidate view; the latter since it is
     * not useful if there is no text to see.
     */
    virtual CARAPI SetExtractViewShown(
        /* [in] */ Boolean shown);

    /**
     * Return whether the fullscreen extract view is shown.  This will only
     * return TRUE if {@link #isFullscreenMode()} returns TRUE, and in that
     * case its value depends on the last call to
     * {@link #setExtractViewShown(Boolean)}.  This effectively lets you
     * determine if the application window is entirely covered (when this
     * returns TRUE) or if some part of it may be shown (if this returns
     * FALSE, though if {@link #isFullscreenMode()} returns TRUE in that case
     * then it is probably only a sliver of the application).
     */
    virtual CARAPI IsExtractViewShown(
        /* [out] */ Boolean* shown);

    /**
     * Compute the interesting insets into your UI.  The default implementation
     * uses the top of the candidates frame for the visible insets, and the
     * top of the input frame for the content insets.  The default touchable
     * insets are {@link Insets#TOUCHABLE_INSETS_VISIBLE}.
     *
     * <p>Note that this method is not called when
     * {@link #isExtractViewShown} returns TRUE, since
     * in that case the application is left as-is behind the input method and
     * not impacted by anything in its UI.
     *
     * @param outInsets Fill in with the current UI insets.
     */
    virtual CARAPI OnComputeInsets(
        /* [in] */ Insets* outInsets);

    /**
     * Re-evaluate whether the soft input area should currently be shown, and
     * update its UI if this has changed since the last time it
     * was evaluated.  This will call {@link #onEvaluateInputViewShown()} to
     * determine whether the input view should currently be shown.  You
     * can use {@link #isInputViewShown()} to determine if the input view
     * is currently shown.
     */
    virtual CARAPI UpdateInputViewShown();

    /**
     * Returns TRUE if we have been asked to show our input view.
     */
    virtual CARAPI IsShowInputRequested(
        /* [out] */ Boolean* requested);

    /**
     * Return whether the soft input view is <em>currently</em> shown to the
     * user.  This is the state that was last determined and
     * applied by {@link #updateInputViewShown()}.
     */
    virtual CARAPI IsInputViewShown(
        /* [out] */ Boolean* shown);

    /**
     * Override this to control when the soft input area should be shown to
     * the user.  The default implementation only shows the input view when
     * there is no hard keyboard or the keyboard is hidden.  If you change what
     * this returns, you will need to call {@link #updateInputViewShown()}
     * yourself whenever the returned value may have changed to have it
     * re-evalauted and applied.
     */
    virtual CARAPI OnEvaluateInputViewShown(
        /* [out] */ Boolean* shown);

    /**
     * Controls the visibility of the candidates display area.  By default
     * it is hidden.
     */
    virtual CARAPI SetCandidatesViewShown(
        /* [in] */ Boolean shown);

    /**
     * Returns the visibility mode (either {@link View#INVISIBLE View.INVISIBLE}
     * or {@link View#GONE View.GONE}) of the candidates view when it is not
     * shown.  The default implementation returns GONE when
     * {@link #isExtractViewShown} returns TRUE,
     * otherwise VISIBLE.  Be careful if you change this to return GONE in
     * other situations -- if showing or hiding the candidates view causes
     * your window to resize, this can cause temporary drawing artifacts as
     * the resize takes place.
     */
    virtual CARAPI GetCandidatesHiddenVisibility(
        /* [out] */ Int32* vis);

    virtual CARAPI ShowStatusIcon(
        /* [in] */ Int32 iconResId);

    virtual CARAPI HideStatusIcon();

    /**
     * Force switch to a new input method, as identified by <var>id</var>.  This
     * input method will be destroyed, and the requested one started on the
     * current input field.
     *
     * @param id Unique identifier of the new input method ot start.
     */
    virtual CARAPI SwitchInputMethod(
        /* [in] */ const String& id);

    virtual CARAPI SetExtractView(
        /* [in] */ IView* view);

    /**
     * Replaces the current candidates view with a new one.  You only need to
     * call this when dynamically changing the view; normally, you should
     * implement {@link #onCreateCandidatesView()} and create your view when
     * first needed by the input method.
     */
    virtual CARAPI SetCandidatesView(
        /* [in] */ IView* view);

    /**
     * Replaces the current input view with a new one.  You only need to
     * call this when dynamically changing the view; normally, you should
     * implement {@link #onCreateInputView()} and create your view when
     * first needed by the input method.
     */
    virtual CARAPI SetInputView(
        /* [in] */ IView* view);

    /**
     * Called by the framework to create the layout for showing extacted text.
     * Only called when in fullscreen mode.  The returned view hierarchy must
     * have an {@link ExtractEditText} whose ID is
     * {@link android.R.id#inputExtractEditText}.
     */
    virtual CARAPI OnCreateExtractTextView(
        /* [out] */ IView** view);

    /**
     * Create and return the view hierarchy used to show candidates.  This will
     * be called once, when the candidates are first displayed.  You can return
     * NULL to have no candidates view; the default implementation returns NULL.
     *
     * <p>To control when the candidates view is displayed, use
     * {@link #setCandidatesViewShown(Boolean)}.
     * To change the candidates view after the first one is created by this
     * function, use {@link #setCandidatesView(View)}.
     */
    virtual CARAPI OnCreateCandidatesView(
        /* [out] */ IView** view);

    /**
     * Create and return the view hierarchy used for the input area (such as
     * a soft keyboard).  This will be called once, when the input area is
     * first displayed.  You can return NULL to have no input area; the default
     * implementation returns NULL.
     *
     * <p>To control when the input view is displayed, implement
     * {@link #onEvaluateInputViewShown()}.
     * To change the input view after the first one is created by this
     * function, use {@link #setInputView(View)}.
     */
    virtual CARAPI OnCreateInputView(
        /* [out] */ IView** view);

    /**
     * Called when the input view is being shown and input has started on
     * a new editor.  This will always be called after {@link #onStartInput},
     * allowing you to do your general setup there and just view-specific
     * setup here.  You are guaranteed that {@link #onCreateInputView()} will
     * have been called some time before this function is called.
     *
     * @param info Description of the type of text being edited.
     * @param restarting Set to TRUE if we are restarting input on the
     * same text field as before.
     */
    virtual CARAPI OnStartInputView(
        /* [in] */ IEditorInfo* info,
        /* [in] */ Boolean restarting);

    /**
     * Called when the input view is being hidden from the user.  This will
     * be called either prior to hiding the window, or prior to switching to
     * another target for editing.
     *
     * <p>The default
     * implementation uses the InputConnection to clear any active composing
     * text; you can override this (not calling the base class implementation)
     * to perform whatever behavior you would like.
     *
     * @param finishingInput If TRUE, {@link #onFinishInput} will be
     * called immediately after.
     */
    virtual CARAPI OnFinishInputView(
        /* [in] */ Boolean finishingInput);

    /**
     * Called when only the candidates view has been shown for showing
     * processing as the user enters text through a hard keyboard.
     * This will always be called after {@link #onStartInput},
     * allowing you to do your general setup there and just view-specific
     * setup here.  You are guaranteed that {@link #onCreateCandidatesView()}
     * will have been called some time before this function is called.
     *
     * <p>Note that this will <em>not</em> be called when the input method
     * is running in full editing mode, and thus receiving
     * {@link #onStartInputView} to initiate that operation.  This is only
     * for the case when candidates are being shown while the input method
     * editor is hidden but wants to show its candidates UI as text is
     * entered through some other mechanism.
     *
     * @param info Description of the type of text being edited.
     * @param restarting Set to TRUE if we are restarting input on the
     * same text field as before.
     */
    virtual CARAPI OnStartCandidatesView(
        /* [in] */ IEditorInfo* info,
        /* [in] */ Boolean restarting);

    /**
     * Called when the candidates view is being hidden from the user.  This will
     * be called either prior to hiding the window, or prior to switching to
     * another target for editing.
     *
     * <p>The default
     * implementation uses the InputConnection to clear any active composing
     * text; you can override this (not calling the base class implementation)
     * to perform whatever behavior you would like.
     *
     * @param finishingInput If TRUE, {@link #onFinishInput} will be
     * called immediately after.
     */
    virtual CARAPI OnFinishCandidatesView(
        /* [in] */ Boolean finishingInput);

    /**
     * The system has decided that it may be time to show your input method.
     * This is called due to a corresponding call to your
     * {@link InputMethod#showSoftInput InputMethod.showSoftInput()}
     * method.  The default implementation uses
     * {@link #onEvaluateInputViewShown()}, {@link #onEvaluateFullscreenMode()},
     * and the current configuration to decide whether the input view should
     * be shown at this point.
     *
     * @param flags Provides additional information about the show request,
     * as per {@link InputMethod#showSoftInput InputMethod.showSoftInput()}.
     * @param configChange This is TRUE if we are re-showing due to a
     * configuration change.
     * @return Returns TRUE to indicate that the window should be shown.
     */
    virtual CARAPI OnShowInputRequested(
        /* [in] */ Int32 flags,
        /* [in] */ Boolean configChange,
        /* [out] */ Boolean* requested);

    virtual CARAPI ShowWindow(
        /* [in] */ Boolean showInput);

    virtual CARAPI HideWindow();

    /**
     * Called when the input method window has been shown to the user, after
     * previously not being visible.  This is done after all of the UI setup
     * for the window has occurred (creating its views etc).
     */
    virtual CARAPI OnWindowShown();

    /**
     * Called when the input method window has been hidden from the user,
     * after previously being visible.
     */
    virtual CARAPI OnWindowHidden();

    /**
     * Called when a new client has bound to the input method.  This
     * may be followed by a series of {@link #onStartInput(EditorInfo, Boolean)}
     * and {@link #onFinishInput()} calls as the user navigates through its
     * UI.  Upon this call you know that {@link #getCurrentInputBinding}
     * and {@link #getCurrentInputConnection} return valid objects.
     */
    virtual CARAPI OnBindInput();

    /**
     * Called when the previous bound client is no longer associated
     * with the input method.  After returning {@link #getCurrentInputBinding}
     * and {@link #getCurrentInputConnection} will no longer return
     * valid objects.
     */
    virtual CARAPI OnUnbindInput();

    /**
     * Called to inform the input method that text input has started in an
     * editor.  You should use this callback to initialize the state of your
     * input to match the state of the editor given to it.
     *
     * @param attribute The attributes of the editor that input is starting
     * in.
     * @param restarting Set to TRUE if input is restarting in the same
     * editor such as because the application has changed the text in
     * the editor.  Otherwise will be FALSE, indicating this is a new
     * session with the editor.
     */
    virtual CARAPI OnStartInput(
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Boolean restarting);

    /**
     * Called to inform the input method that text input has finished in
     * the last editor.  At this point there may be a call to
     * {@link #onStartInput(EditorInfo, Boolean)} to perform input in a
     * new editor, or the input method may be left idle.  This method is
     * <em>not</em> called when input restarts in the same editor.
     *
     * <p>The default
     * implementation uses the InputConnection to clear any active composing
     * text; you can override this (not calling the base class implementation)
     * to perform whatever behavior you would like.
     */
    virtual CARAPI OnFinishInput();

    /**
     * Called when the application has reported auto-completion candidates that
     * it would like to have the input method displayed.  Typically these are
     * only used when an input method is running in full-screen mode, since
     * otherwise the user can see and interact with the pop-up window of
     * completions shown by the application.
     *
     * <p>The default implementation here does nothing.
     */
    virtual CARAPI OnDisplayCompletions(
        /* [in] */ ArrayOf<ICompletionInfo*>* completions);

    /**
     * Called when the application has reported new extracted text to be shown
     * due to changes in its current text state.  The default implementation
     * here places the new text in the extract edit text, when the input
     * method is running in fullscreen mode.
     */
    virtual CARAPI OnUpdateExtractedText(
        /* [in] */ Int32 token,
        /* [in] */ IExtractedText* text);

    /**
     * Called when the application has reported a new selection region of
     * the text.  This is called whether or not the input method has requested
     * extracted text updates, although if so it will not receive this call
     * if the extracted text has changed as well.
     *
     * <p>Be careful about changing the text in reaction to this call with
     * methods such as setComposingText, commitText or
     * deleteSurroundingText. If the cursor moves as a result, this method
     * will be called again, which may result in an infinite loop.
     *
     * <p>The default implementation takes care of updating the cursor in
     * the extract text, if it is being shown.
     */
    virtual CARAPI OnUpdateSelection(
        /* [in] */ Int32 oldSelStart,
        /* [in] */ Int32 oldSelEnd,
        /* [in] */ Int32 newSelStart,
        /* [in] */ Int32 newSelEnd,
        /* [in] */ Int32 candidatesStart,
        /* [in] */ Int32 candidatesEnd);

    /**
     * Called when the user tapped or clicked a text view.
     * IMEs can't rely on this method being called because this was not part of the original IME
     * protocol, so applications with custom text editing written before this method appeared will
     * not call to inform the IME of this interaction.
     * @param focusChanged true if the user changed the focused view by this click.
     */
    virtual CARAPI OnViewClicked(
        /* [in] */ Boolean focusChanged);

    /**
     * Called when the application has reported a new location of its text
     * cursor.  This is only called if explicitly requested by the input method.
     * The default implementation does nothing.
     */
    virtual CARAPI OnUpdateCursor(
        /* [in] */ IRect* newCursor);

    /**
     * Called when the application has reported a new location of its text insertion point and
     * characters in the composition string.  This is only called if explicitly requested by the
     * input method. The default implementation does nothing.
     * @param cursorAnchorInfo The positional information of the text insertion point and the
     * composition string.
     */
    CARAPI OnUpdateCursorAnchorInfo(
        /* [in] */ ICursorAnchorInfo* cursorAnchorInfo);

    /**
     * Close this input method's soft input area, removing it from the display.
     * The input method will continue running, but the user can no longer use
     * it to generate input by touching the screen.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link InputMethodManager#HIDE_IMPLICIT_ONLY
     * InputMethodManager.HIDE_IMPLICIT_ONLY} bit set.
     */
    virtual CARAPI RequestHideSelf(
        /* [in] */ Int32 flags);

    /**
     * Override this to intercept key down events before they are processed by the
     * application.  If you return true, the application will not
     * process the event itself.  If you return false, the normal application processing
     * will occur as if the IME had not seen the event at all.
     *
     * <p>The default implementation intercepts {@link KeyEvent#KEYCODE_BACK
     * KeyEvent.KEYCODE_BACK} if the IME is currently shown, to
     * possibly hide it when the key goes up (if not canceled or long pressed).  In
     * addition, in fullscreen mode only, it will consume DPAD movement
     * events to move the cursor in the extracted text view, not allowing
     * them to perform navigation in the underlying application.
     */
    virtual CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* state);

    /**
     * Default implementation of {@link KeyEvent.Callback#onKeyLongPress(Int32, KeyEvent)
     * KeyEvent.Callback.onKeyLongPress()}: always returns FALSE (doesn't handle
     * the event).
     */
    virtual CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* state);

    /**
     * Override this to intercept special key multiple events before they are
     * processed by the
     * application.  If you return false, the application will not itself
     * process the event.  If you return TRUE, the normal application processing
     * will occur as if the IME had not seen the event at all.
     *
     * <p>The default implementation always returns FALSE, except when
     * in fullscreen mode, where it will consume DPAD movement
     * events to move the cursor in the extracted text view, not allowing
     * them to perform navigation in the underlying application.
     */
    virtual CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* state);

    /**
     * Override this to intercept key up events before they are processed by the
     * application.  If you return false, the application will not itself
     * process the event.  If you return TRUE, the normal application processing
     * will occur as if the IME had not seen the event at all.
     *
     * <p>The default implementation intercepts {@link KeyEvent#KEYCODE_BACK
     * KeyEvent.KEYCODE_BACK} to hide the current IME UI if it is shown.  In
     * addition, in fullscreen mode only, it will consume DPAD movement
     * events to move the cursor in the extracted text view, not allowing
     * them to perform navigation in the underlying application.
     */
    virtual CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* state);

    /**
     * Override this to intercept trackball motion events before they are
     * processed by the application.
     * If you return true, the application will not itself process the event.
     * If you return false, the normal application processing will occur as if
     * the IME had not seen the event at all.
     */
    virtual CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* state);

    /**
     * Override this to intercept generic motion events before they are
     * processed by the application.
     * If you return true, the application will not itself process the event.
     * If you return false, the normal application processing will occur as if
     * the IME had not seen the event at all.
     */
    //@Override
    virtual CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* state);

    virtual CARAPI OnAppPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    /**
     * Send the given key event code (as defined by {@link KeyEvent}) to the
     * current input connection is a key down + key up event pair.  The sent
     * events have {@link KeyEvent#FLAG_SOFT_KEYBOARD KeyEvent.FLAG_SOFT_KEYBOARD}
     * set, so that the recipient can identify them as coming from a software
     * input method, and
     * {@link KeyEvent#FLAG_KEEP_TOUCH_MODE KeyEvent.FLAG_KEEP_TOUCH_MODE}, so
     * that they don't impact the current touch mode of the UI.
     *
     * <p>Note that it's discouraged to send such key events in normal operation;
     * this is mainly for use with {@link android.text.InputType#TYPE_NULL} type
     * text fields, or for non-rich input methods. A reasonably capable software
     * input method should use the
     * {@link android.view.inputmethod.InputConnection#commitText} family of methods
     * to send text to an application, rather than sending key events.</p>
     *
     * @param keyEventCode The raw key code to send, as defined by
     * {@link KeyEvent}.
     */
    virtual CARAPI SendDownUpKeyEvents(
        /* [in] */ Int32 keyEventCode);

    /**
     * Ask the input target to execute its default action via
     * {@link InputConnection#performEditorAction
     * InputConnection.performEditorAction()}.
     *
     * @param fromEnterKey If TRUE, this will be executed as if the user had
     * pressed an enter key on the keyboard, that is it will <em>not</em>
     * be done if the editor has set {@link EditorInfo#IME_FLAG_NO_ENTER_ACTION
     * EditorInfo.IME_FLAG_NO_ENTER_ACTION}.  If FALSE, the action will be
     * sent regardless of how the editor has set that flag.
     *
     * @return Returns a Boolean indicating whether an action has been sent.
     * If FALSE, either the editor did not specify a default action or it
     * does not want an action from the enter key.  If TRUE, the action was
     * sent (or there was no input connection at all).
     */
    virtual CARAPI SendDefaultEditorAction(
        /* [in] */ Boolean fromEnterKey,
        /* [out] */ Boolean* state);

    /**
     * Send the given UTF-16 character to the current input connection.  Most
     * characters will be delivered simply by calling
     * {@link InputConnection#commitText InputConnection.commitText()} with
     * the character; some, however, may be handled different.  In particular,
     * the enter character ('\n') will either be delivered as an action code
     * or a raw key event, as appropriate.  Consider this as a convenience
     * method for IMEs that do not have a full implementation of actions; a
     * fully complying IME will decide of the right action for each event and
     * will likely never call this method except maybe to handle events coming
     * from an actual hardware keyboard.
     *
     * @param charCode The UTF-16 character code to send.
     */
    virtual CARAPI SendKeyChar(
        /* [in] */ Char32 charCode);

    /**
     * This is called when the user has moved the cursor in the extracted
     * text view, when running in fullsreen mode.  The default implementation
     * performs the corresponding selection change on the underlying text
     * editor.
     */
    virtual CARAPI OnExtractedSelectionChanged(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * @hide
     */
    virtual CARAPI OnExtractedDeleteText(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * @hide
     */
    virtual CARAPI OnExtractedReplaceText(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ICharSequence* text);

    /**
     * @hide
     */
    virtual CARAPI OnExtractedSetSpan(
        /* [in] */ IInterface* span,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    /**
     * This is called when the user has clicked on the extracted text view,
     * when running in fullscreen mode.  The default implementation hides
     * the candidates view when this happens, but only if the extracted text
     * editor has a vertical scroll bar because its text doesn't fit.
     * Re-implement this to provide whatever behavior you want.
     */
    virtual CARAPI OnExtractedTextClicked();

    /**
     * This is called when the user has performed a cursor movement in the
     * extracted text view, when it is running in fullscreen mode.  The default
     * implementation hides the candidates view when a vertical movement
     * happens, but only if the extracted text editor has a vertical scroll bar
     * because its text doesn't fit.
     * Re-implement this to provide whatever behavior you want.
     * @param dx The amount of cursor movement in the x dimension.
     * @param dy The amount of cursor movement in the y dimension.
     */
    virtual CARAPI OnExtractedCursorMovement(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    /**
     * This is called when the user has selected a context menu item from the
     * extracted text view, when running in fullscreen mode.  The default
     * implementation sends this action to the current InputConnection's
     * {@link InputConnection#performContextMenuAction(Int32)}, for it
     * to be processed in underlying "real" editor.  Re-implement this to
     * provide whatever behavior you want.
     */
    virtual CARAPI OnExtractTextContextMenuItem(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* state);

    /**
     * Return text that can be used as a button label for the given
     * {@link EditorInfo#imeOptions EditorInfo.imeOptions}.  Returns NULL
     * if there is no action requested.  Note that there is no guarantee that
     * the returned text will be relatively short, so you probably do not
     * want to use it as text on a soft keyboard key label.
     *
     * @param imeOptions The value from @link EditorInfo#imeOptions EditorInfo.imeOptions}.
     *
     * @return Returns a label to use, or NULL if there is no action.
     */
    virtual CARAPI GetTextForImeAction(
        /* [in] */ Int32 imeOptions,
        /* [out] */ ICharSequence** text);

    /**
     * Called when the fullscreen-mode extracting editor info has changed,
     * to determine whether the extracting (extract text and candidates) portion
     * of the UI should be shown.  The standard implementation hides or shows
     * the extract area depending on whether it makes sense for the
     * current editor.  In particular, a {@link InputType#TYPE_NULL}
     * input type or {@link EditorInfo#IME_FLAG_NO_EXTRACT_UI} flag will
     * turn off the extract area since there is no text to be shown.
     */
    virtual CARAPI OnUpdateExtractingVisibility(
        /* [in] */ IEditorInfo* ei);

    /**
     * Called when the fullscreen-mode extracting editor info has changed,
     * to update the state of its UI such as the action buttons shown.
     * You do not need to deal with this if you are using the standard
     * full screen extract UI.  If replacing it, you will need to re-implement
     * this to put the appropriate action button in your own UI and handle it,
     * and perform any other changes.
     *
     * <p>The standard implementation turns on or off its accessory area
     * depending on whether there is an action button, and hides or shows
     * the entire extract area depending on whether it makes sense for the
     * current editor.  In particular, a {@link InputType#TYPE_NULL} or
     * {@link InputType#TYPE_TEXT_VARIATION_FILTER} input type will turn off the
     * extract area since there is no text to be shown.
     */
    virtual CARAPI OnUpdateExtractingViews(
        /* [in] */ IEditorInfo* ei);

    /**
     * This is called when, while currently displayed in extract mode, the
     * current input target changes.  The default implementation will
     * auto-hide the IME if the new target is not a full editor, since this
     * can be a confusing experience for the user.
     */
    virtual CARAPI OnExtractingInputChanged(
        /* [in] */ IEditorInfo* ei);

    /**
     * @return The recommended height of the input method window.
     * An IME author can get the last input method's height as the recommended height
     * by calling this in
     * {@link android.inputmethodservice.InputMethodService#onStartInputView(EditorInfo, boolean)}.
     * If you don't need to use a predefined fixed height, you can avoid the window-resizing of IME
     * switching by using this value as a visible inset height. It's efficient for the smooth
     * transition between different IMEs. However, note that this may return 0 (or possibly
     * unexpectedly low height). You should thus avoid relying on the return value of this method
     * all the time. Please make sure to use a reasonable height for the IME.
     */
    virtual CARAPI GetInputMethodWindowRecommendedHeight(
        /* [out] */ Int32* height);

protected:
    virtual CARAPI_(void) InitViews();

    virtual CARAPI_(void) UpdateExtractFrameVisibility();

    virtual CARAPI_(void) UpdateCandidatesVisibility(
        /* [in] */ Boolean shown);

    virtual CARAPI_(void) ShowWindowInner(
        /* [in] */ Boolean showInput);

    virtual CARAPI_(void) DoFinishInput();

    virtual CARAPI_(void) DoStartInput(
        /* [in] */ IInputConnection* ic,
        /* [in] */ IEditorInfo* attribute,
        /* [in] */ Boolean restarting);

    virtual CARAPI_(void) ReportExtractedMovement(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count);

    virtual CARAPI_(Boolean) DoMovementKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 count);

    virtual CARAPI_(void) StartExtractingText(
        /* [in] */ Boolean inputChanged);

    // TODO: Handle the subtype change event
    /**
     * Called when the subtype was changed.
     * @param newSubtype the subtype which is being changed to.
     */
    virtual CARAPI_(void) OnCurrentInputMethodSubtypeChanged(
        /* [in] */ IInputMethodSubtype* newSubtype);

private:
    /**
     * Show the input method. This is a call back to the
     * IMF to handle showing the input method.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link InputMethodManager#SHOW_FORCED
     * InputMethodManager.} bit set.
     */
    CARAPI RequestShowSelf(
        /* [in] */ Int32 flags);

    CARAPI_(Boolean) HandleBack(
        /* [in] */ Boolean doIt);

    /**
     * Handle a request by the system to toggle the soft input area.
     */
    CARAPI_(void) OnToggleSoftInput(
        /* [in] */ Int32 showFlags,
        /* [in] */ Int32 hideFlags);

    CARAPI_(void) FinishViews();

    CARAPI_(void) DoHideWindow();


private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IInputMethodManager> mImm;
    Int32 mTheme;
    Boolean mHardwareAccelerated;

    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<ITypedArray> mThemeAttrs;
    AutoPtr<IView> mRootView;
    AutoPtr<ISoftInputWindow> mWindow;
    Boolean mInitialized;
    Boolean mWindowCreated;
    Boolean mWindowAdded;
    Boolean mWindowVisible;
    Boolean mWindowWasVisible;
    Boolean mInShowWindow;
    AutoPtr<IViewGroup> mFullscreenArea;
    AutoPtr<IFrameLayout> mExtractFrame;
    AutoPtr<IFrameLayout> mCandidatesFrame;
    AutoPtr<IFrameLayout> mInputFrame;

    AutoPtr<IBinder> mToken;

    AutoPtr<IInputBinding> mInputBinding;
    AutoPtr<IInputConnection> mInputConnection;
    Boolean mInputStarted;
    Boolean mInputViewStarted;
    Boolean mCandidatesViewStarted;
    AutoPtr<IInputConnection> mStartedInputConnection;
    AutoPtr<IEditorInfo> mInputEditorInfo;

    Int32 mShowInputFlags;
    Boolean mShowInputRequested;
    Boolean mLastShowInputRequested;
    Int32 mCandidatesVisibility;
    AutoPtr<ArrayOf<ICompletionInfo*> > mCurCompletions;

    Boolean mShowInputForced;

    Boolean mFullscreenApplied;
    Boolean mIsFullscreen;
    AutoPtr<IView> mExtractView;
    Boolean mExtractViewHidden;
    AutoPtr<IExtractEditText> mExtractEditText;
    AutoPtr<IViewGroup> mExtractAccessories;
    AutoPtr<IButton> mExtractAction;
    AutoPtr<IExtractedText> mExtractedText;
    Int32 mExtractedToken;

    AutoPtr<IView> mInputView;
    Boolean mIsInputViewShown;

    Int32 mStatusIcon;
    Int32 mBackDisposition;

    AutoPtr<Insets> mTmpInsets;
    AutoPtr< ArrayOf<Int32> > mTmpLocation;

    AutoPtr<IOnComputeInternalInsetsListener> mInsetsComputer;
    AutoPtr<IViewOnClickListener> mActionClickListener;

    static const Int32 MOVEMENT_DOWN = -1;
    static const Int32 MOVEMENT_UP = -2;

    friend class _OnComputeInternalInsetsListener;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_INPUTMETHODSERVICE_H__
