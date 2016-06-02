
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODMANAGER_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODMANAGER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_View_InputMethod_CInputMethodManager.h"
#include "elastos/droid/view/InputEventSender.h"
#include "elastos/droid/view/inputmethod/CControlledInputConnectionWrapper.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/utility/Pools.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Internal::View::IIInputMethodManager;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::Pools;

using Elastos::Utility::IMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CInputMethodManager)
    , public Object
    , public IInputMethodManager
{
private:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CInputMethodManager* host)
            : Handler(looper, NULL, TRUE)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CInputMethodManager* mHost;
    };

    class StartInputRunnable
        : public Runnable
    {
    public:
        StartInputRunnable(
            /* [in] */ CInputMethodManager* service)
            : mHost(service)
        {}

        CARAPI Run();

    private:
        CInputMethodManager* mHost;
    };

    class ImeInputEventSender
        : public InputEventSender
    {
    public:
        ImeInputEventSender();

        CARAPI constructor(
            /* [in] */ IInputChannel* inputChannel,
            /* [in] */ ILooper* looper,
            /* [in] */ CInputMethodManager* h);

        CARAPI OnInputEventFinished(
            /* [in] */ Int32 seq,
            /* [in] */ Boolean handled);

    public:
        CInputMethodManager* mHost;
    };

    class PendingEvent
        : public Runnable
    {
    public:
        PendingEvent(
            /* [in] */ CInputMethodManager* host);

        CARAPI_(void) Recycle();

        CARAPI Run();

    public:
        AutoPtr<IInputEvent> mEvent;
        AutoPtr<IInterface> mToken;
        String mInputMethodId;
        AutoPtr<IInputMethodManagerFinishedInputEventCallback> mCallback;
        AutoPtr<IHandler> mHandler;
        Boolean mHandled;
        CInputMethodManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInputMethodManager();

    //TODO: will delete. Because LocalInputMethodManager is private in java code.
    CARAPI constructor(
        /* [in] */ IIInputMethodManager* service,
        /* [in] */ ILooper* looper);

    /**
     * Retrieve the global InputMethodManager instance, creating it if it
     * doesn't already exist.
     * @hide
     */
    static CARAPI_(AutoPtr<IInputMethodManager>) GetInstance();

    /**
     * Private optimization: retrieve the global InputMethodManager instance,
     * if it exists.
     * @hide
     */
    static CARAPI_(AutoPtr<IInputMethodManager>) PeekInstance();

    /** @hide */
    CARAPI GetClient(
        /* [out] */ IInputMethodClient** client);

    /** @hide */
    CARAPI GetInputContext(
        /* [out] */ IIInputContext** context);

    CARAPI GetInputMethodList(
        /* [out] */ IList** infos);

    CARAPI GetEnabledInputMethodList(
        /* [out] */ IList** list);

    CARAPI GetEnabledInputMethodSubtypeList(
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ Boolean allowsImplicitlySelectedSubtypes,
        /* [out] */ IList** infos);

    CARAPI ShowStatusIcon(
        /* [in] */ IBinder* imeToken,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 iconId);

    CARAPI HideStatusIcon(
        /* [in] */ IBinder* imeToken);

    CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* imeToken,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);

    /** @hide */
    CARAPI SetFullscreenMode(
        /* [in] */ Boolean fullScreen);

    /** @hide */
    CARAPI RegisterSuggestionSpansForNotification(
        /* [in] */ ArrayOf<ISuggestionSpan*>* spans);

    /** @hide */
    CARAPI NotifySuggestionPicked(
        /* [in] */ ISuggestionSpan* span,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 index);

    /**
     * Allows you to discover whether the attached input method is running
     * in fullscreen mode.  Return true if it is fullscreen, entirely covering
     * your UI, else returns false.
     */
    CARAPI IsFullscreenMode(
        /* [out] */ Boolean* isfull);

    /**
     * Return true if the given view is the currently active view for the
     * input method.
     */
    CARAPI IsActive(
        /* [in] */ IView* view,
        /* [out] */ Boolean* active);

    /**
     * Return true if any view is currently active in the input method.
     */
    CARAPI IsActive(
        /* [out] */ Boolean* active);

    /**
     * Return true if the currently served view is accepting full text edits.
     * If false, it has no input connection, so can only handle raw key events.
     */
    CARAPI IsAcceptingText(
        /* [out] */ Boolean* isAccepting);

    /**
     * Reset all of the state associated with being bound to an input method.
     */
    /* package */ CARAPI_(void) ClearBindingLocked();

    CARAPI_(void) SetInputChannelLocked(
        /* [in] */ IInputChannel* channel);

    /**
     * Reset all of the state associated with a served view being connected
     * to an input method
     */
    /* package */ CARAPI_(void) ClearConnectionLocked();

    /**
     * Disconnect any existing input connection, clearing the served view.
     */
    CARAPI_(void) FinishInputLocked();

    /**
     * Called from the FINISH_INPUT_CONNECTION message above.
     * @hide
     */
    CARAPI ReportFinishInputConnection(
        /* [in] */ IInputConnection* ic);

    CARAPI DisplayCompletions(
        /* [in] */ IView* view,
        /* [in] */ ArrayOf<ICompletionInfo*>* completions);

    CARAPI UpdateExtractedText(
        /* [in] */ IView* view,
        /* [in] */ Int32 token,
        /* [in] */ IExtractedText* text);

    /**
     * Synonym for {@link #showSoftInput(View, Int32, ResultReceiver)} without
     * a result receiver: explicitly request that the current input method's
     * soft input area be shown to the user, if needed.
     *
     * @param view The currently focused view, which would like to receive
     * soft keyboard input.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link #SHOW_IMPLICIT} bit set.
     */
    CARAPI ShowSoftInput(
        /* [in] */ IView* view,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* show);

    /**
     * Explicitly request that the current input method's soft input area be
     * shown to the user, if needed.  Call this if the user interacts with
     * your view in such a way that they have expressed they would like to
     * start performing input into it.
     *
     * @param view The currently focused view, which would like to receive
     * soft keyboard input.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link #SHOW_IMPLICIT} bit set.
     * @param resultReceiver If non-NULL, this will be called by the IME when
     * it has processed your request to tell you what it has done.  The result
     * code you receive may be either {@link #RESULT_UNCHANGED_SHOWN},
     * {@link #RESULT_UNCHANGED_HIDDEN}, {@link #RESULT_SHOWN}, or
     * {@link #RESULT_HIDDEN}.
     */
    CARAPI ShowSoftInput(
        /* [in] */ IView* view,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver,
        /* [out] */ Boolean* show);

    /** @hide */
    CARAPI ShowSoftInputUnchecked(
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    /**
     * Synonym for {@link #hideSoftInputFromWindow(IBinder, Int32, ResultReceiver)
     * without a result: request to hide the soft input window from the
     * context of the window that is currently accepting input.
     *
     * @param windowToken The token of the window that is making the request,
     * as returned by {@link View#getWindowToken() View.getWindowToken()}.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link #HIDE_IMPLICIT_ONLY} bit set.
     */
    CARAPI HideSoftInputFromWindow(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* hide);

    /**
     * Request to hide the soft input window from the context of the window
     * that is currently accepting input.  This should be called as a result
     * of the user doing some actually than fairly explicitly requests to
     * have the input window hidden.
     *
     * @param windowToken The token of the window that is making the request,
     * as returned by {@link View#getWindowToken() View.getWindowToken()}.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link #HIDE_IMPLICIT_ONLY} bit set.
     * @param resultReceiver If non-NULL, this will be called by the IME when
     * it has processed your request to tell you what it has done.  The result
     * code you receive may be either {@link #RESULT_UNCHANGED_SHOWN},
     * {@link #RESULT_UNCHANGED_HIDDEN}, {@link #RESULT_SHOWN}, or
     * {@link #RESULT_HIDDEN}.
     */
    CARAPI HideSoftInputFromWindow(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver,
        /* [out] */ Boolean* hide);

    /**
     * This method toggles the input method window display.
     * If the input window is already displayed, it gets hidden.
     * If not the input window will be displayed.
     * @param windowToken The token of the window that is making the request,
     * as returned by {@link View#getWindowToken() View.getWindowToken()}.
     * @param showFlags Provides additional operating flags.  May be
     * 0 or have the {@link #SHOW_IMPLICIT},
     * {@link #SHOW_FORCED} bit set.
     * @param hideFlags Provides additional operating flags.  May be
     * 0 or have the {@link #HIDE_IMPLICIT_ONLY},
     * {@link #HIDE_NOT_ALWAYS} bit set.
     **/
    CARAPI ToggleSoftInputFromWindow(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 showFlags,
        /* [in] */ Int32 hideFlags);

    /*
     * This method toggles the input method window display.
     * If the input window is already displayed, it gets hidden.
     * If not the input window will be displayed.
     * @param showFlags Provides additional operating flags.  May be
     * 0 or have the {@link #SHOW_IMPLICIT},
     * {@link #SHOW_FORCED} bit set.
     * @param hideFlags Provides additional operating flags.  May be
     * 0 or have the {@link #HIDE_IMPLICIT_ONLY},
     * {@link #HIDE_NOT_ALWAYS} bit set.
     * @hide
     */
    CARAPI ToggleSoftInput(
        /* [in] */ Int32 showFlags,
        /* [in] */ Int32 hideFlags);

    /**
     * If the input method is currently connected to the given view,
     * restart it with its new contents.  You should call this when the text
     * within your view changes outside of the normal input method or key
     * input flow, such as when an application calls TextView.setText().
     *
     * @param view The view whose text has changed.
     */
    CARAPI RestartInput(
        /* [in] */ IView* view);

    CARAPI_(Boolean) StartInputInner(
        /* [in] */ IBinder* windowGainingFocus,
        /* [in] */ Int32 controlFlags,
        /* [in] */ Int32 softInputMode,
        /* [in] */ Int32 windowFlags);

    /**
     * When the focused window is dismissed, this method is called to finish the
     * input method started before.
     * @hide
     */
    CARAPI WindowDismissed(
        /* [in] */ IBinder* appWindowToken);

    /**
     * Call this when a view receives focus.
     * @hide
     */
    CARAPI FocusIn(
        /* [in] */ IView* view);

    CARAPI_(void) FocusInLocked(
        /* [in] */ IView* view);

    /**
     * Call this when a view loses focus.
     * @hide
     */
    CARAPI FocusOut(
        /* [in] */ IView* view);

    static CARAPI_(void) ScheduleCheckFocusLocked(
        /* [in] */ IView* view);

    /**
     * @hide
     */
    CARAPI CheckFocus();

    CARAPI_(void) CloseCurrentInput();

    /**
     * Called by ViewRoot when its window gets input focus.
     * @hide
     */
    CARAPI OnWindowFocus(
        /* [in] */ IView* rootView,
        /* [in] */ IView* focusedView,
        /* [in] */ Int32 softInputMode,
        /* [in] */ Boolean first,
        /* [in] */ Int32 windowFlags);

    /** @hide */
    CARAPI StartGettingWindowFocus(
        /* [in] */ IView* rootView);

    /**
     * Report the current selection range.
     */
    CARAPI UpdateSelection(
        /* [in] */ IView* view,
        /* [in] */ Int32 selStart,
        /* [in] */ Int32 selEnd,
        /* [in] */ Int32 candidatesStart,
        /* [in] */ Int32 candidatesEnd);

    CARAPI ViewClicked(
        /* [in] */ IView* view);

    /**
     * Returns true if the current input method wants to watch the location
     * of the input editor's cursor in its window.
     */
    CARAPI IsWatchingCursor(
        /* [in] */ IView* view,
        /* [out] */ Boolean* isWatching);

    /**
     * Return true if the current input method wants to be notified when cursor/anchor location
     * is changed.
     *
     * @hide
     */
    CARAPI IsCursorAnchorInfoEnabled(
        /* [out] */ Boolean* result);

    /**
     * Set the requested mode for {@link #updateCursorAnchorInfo(View, CursorAnchorInfo)}.
     *
     * @hide
     */
    CARAPI SetUpdateCursorAnchorInfoMode(
        /* [in] */ Int32 flags);

    /**
     * Report the current cursor location in its window.
     */
    CARAPI UpdateCursor(
        /* [in] */ IView* view,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * Report positional change of the text insertion point and/or characters in the composition
     * string.
     */
    CARAPI UpdateCursorAnchorInfo(
        /* [in] */ IView* view,
        /* [in] */ ICursorAnchorInfo* cursorAnchorInfo);

    /**
     * Call {@link InputMethodSession#appPrivateCommand(String, Bundle)
     * InputMethodSession.appPrivateCommand()} on the current Input Method.
     * @param view Optional View that is sending the command, or NULL if
     * you want to send the command regardless of the view that is attached
     * to the input method.
     * @param action Name of the command to be performed.  This <em>must</em>
     * be a scoped name, i.e. prefixed with a package name you own, so that
     * different developers will not create conflicting commands.
     * @param data Any data to include with the command.
     */
    CARAPI SendAppPrivateCommand(
        /* [in] */ IView* view,
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    /**
     * Force switch to a new input method component.  This can only be called
     * from the currently active input method, as validated by the given token.
     * @param token Supplies the identifying token given to an input method
     * when it was started, which allows it to perform this operation on
     * itself.
     * @param id The unique identifier for the new input method to be switched to.
     */
    CARAPI SetInputMethod(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id);

    CARAPI SetInputMethodAndSubtype(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id,
        /* [in] */ IInputMethodSubtype* subtype);

    /**
     * Close/hide the input method's soft input area, so the user no longer
     * sees it or can interact with it.  This can only be called
     * from the currently active input method, as validated by the given token.
     *
     * @param token Supplies the identifying token given to an input method
     * when it was started, which allows it to perform this operation on
     * itself.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link #HIDE_IMPLICIT_ONLY},
     * {@link #HIDE_NOT_ALWAYS} bit set.
     */
    CARAPI HideSoftInputFromInputMethod(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    /**
     * Show the input method's soft input area, so the user
     * sees the input method window and can interact with it.
     * This can only be called from the currently active input method,
     * as validated by the given token.
     *
     * @param token Supplies the identifying token given to an input method
     * when it was started, which allows it to perform this operation on
     * itself.
     * @param flags Provides additional operating flags.  Currently may be
     * 0 or have the {@link #SHOW_IMPLICIT} or
     * {@link #SHOW_FORCED} bit set.
     */
    CARAPI ShowSoftInputFromInputMethod(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 flags);

    /**
     * Dispatches an input event to the IME.
     *
     * Returns {@link #DISPATCH_HANDLED} if the event was handled.
     * Returns {@link #DISPATCH_NOT_HANDLED} if the event was not handled.
     * Returns {@link #DISPATCH_IN_PROGRESS} if the event is in progress and the
     * callback will be invoked later.
     *
     * @hide
     */
    CARAPI DispatchInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInterface* token,
        /* [in] */ IInputMethodManagerFinishedInputEventCallback* callback,
        /* [in] */ IHandler* handler,
        /* [out] */ Int32* result);

    CARAPI ShowInputMethodPicker();

    /**
     * Show the settings for enabling subtypes of the specified input method.
     * @param imiId An input method, whose subtypes settings will be shown. If imiId is null,
     * subtypes of all input methods will be shown.
     */
    CARAPI ShowInputMethodAndSubtypeEnabler(
        /* [in] */ const String& imiId);

    /**
     * Returns the current input method subtype. This subtype is one of the subtypes in
     * the current input method. This method returns null when the current input method doesn't
     * have any input method subtype.
     */
    CARAPI GetCurrentInputMethodSubtype(
        /* [out] */ IInputMethodSubtype** subtype);

    /**
     * Switch to a new input method subtype of the current input method.
     * @param subtype A new input method subtype to switch.
     * @return true if the current subtype was successfully switched. When the specified subtype is
     * null, this method returns false.
     */
    CARAPI SetCurrentInputMethodSubtype(
        /* [in] */ IInputMethodSubtype* subtype,
        /* [out] */ Boolean* switched);

    /**
     * Notify that a user took some action with this input method.
     * @hide
     */
    CARAPI NotifyUserAction();

    /**
     * Returns a map of all shortcut input method info and their subtypes.
     */
    CARAPI GetShortcutInputMethodsAndSubtypes(
        /* [out] */ IMap** subtypes);

    /**
     * @return The current height of the input method window.
     * @hide
     */
    CARAPI GetInputMethodWindowVisibleHeight(
        /* [out] */ Int32* height);

    /**
     * Force switch to the last used input method and subtype. If the last input method didn't have
     * any subtypes, the framework will simply switch to the last input method with no subtype
     * specified.
     * @param imeToken Supplies the identifying token given to an input method when it was started,
     * which allows it to perform this operation on itself.
     * @return true if the current input method and subtype was successfully switched to the last
     * used input method and subtype.
     */
    CARAPI SwitchToLastInputMethod(
        /* [in] */ IBinder* imeToken,
        /* [out] */ Boolean* switched);

    /**
     * Force switch to the next input method and subtype. If there is no IME enabled except
     * current IME and subtype, do nothing.
     * @param imeToken Supplies the identifying token given to an input method when it was started,
     * which allows it to perform this operation on itself.
     * @param onlyCurrentIme if true, the framework will find the next subtype which
     * belongs to the current IME
     * @return true if the current input method and subtype was successfully switched to the next
     * input method and subtype.
     */
    CARAPI SwitchToNextInputMethod(
        /* [in] */ IBinder* imeToken,
        /* [in] */ Boolean onlyCurrentIme,
        /* [out] */ Boolean* switched);

    /**
     * Returns true if the current IME needs to offer the users ways to switch to a next input
     * method (e.g. a globe key.).
     * When an IME sets supportsSwitchingToNextInputMethod and this method returns true,
     * the IME has to offer ways to to invoke {@link #switchToNextInputMethod} accordingly.
     * <p> Note that the system determines the most appropriate next input method
     * and subtype in order to provide the consistent user experience in switching
     * between IMEs and subtypes.
     * @param imeToken Supplies the identifying token given to an input method when it was started,
     * which allows it to perform this operation on itself.
     */
    CARAPI ShouldOfferSwitchingToNextInputMethod(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    /**
     * Set additional input method subtypes. Only a process which shares the same uid with the IME
     * can add additional input method subtypes to the IME.
     * Please note that a subtype's status is stored in the system.
     * For example, enabled subtypes are remembered by the framework even after they are removed
     * by using this method. If you re-add the same subtypes again,
     * they will just get enabled. If you want to avoid such conflicts, for instance, you may
     * want to create a "different" new subtype even with the same locale and mode,
     * by changing its extra value. The different subtype won't get affected by the stored past
     * status. (You may want to take a look at {@link InputMethodSubtype#hashCode()} to refer
     * to the current implementation.)
     * @param imiId Id of InputMethodInfo which additional input method subtypes will be added to.
     * @param subtypes subtypes will be added as additional subtypes of the current input method.
     */
    CARAPI SetAdditionalInputMethodSubtypes(
        /* [in] */ const String& imiId,
        /* [in] */ ArrayOf<IInputMethodSubtype*>* subtypes);

    CARAPI GetLastInputMethodSubtype(
        /* [out] */ IInputMethodSubtype** subtype);

    //For callback
    CARAPI_(void) HandleBind(
        /* [in] */ IInputBindResult* res);

    CARAPI_(void) HandleUnBind(
        /* [in] */ Int32 sequence);

    CARAPI_(void) HandleSetActive(
        /* [in] */ Boolean active);

    // Must be called on the main looper
    CARAPI_(void) SendInputEventAndReportResultOnMainLooper(
        /* [in] */ PendingEvent* p);

    // Must be called on the main looper
    CARAPI_(Int32) SendInputEventOnMainLooperLocked(
        /* [in] */ PendingEvent* p);

    CARAPI_(void) FinishedInputEvent(
        /* [in] */ Int32 seq,
        /* [in] */ Boolean handled,
        /* [in] */ Boolean timeout);

    // Assumes the event has already been removed from the queue.
    CARAPI_(void) InvokeFinishedInputEventCallback(
        /* [in] */ PendingEvent* p,
        /* [in] */ Boolean handled);

private:
    /**
     * Notifies the served view that the current InputConnection will no longer be used.
     */
    CARAPI_(void) NotifyInputConnectionFinished();

    CARAPI_(Boolean) CheckFocusNoStartInput(
        /* [in] */ Boolean forceNewFocus,
        /* [in] */ Boolean finishComposingText);

    CARAPI_(void) FlushPendingEventsLocked();

    CARAPI_(AutoPtr<PendingEvent>) ObtainPendingEventLocked(
        /* [in] */ IInputEvent* event,
        /* [in] */ IInterface* token,
        /* [in] */ const String& inputMethodId,
        /* [in] */ IInputMethodManagerFinishedInputEventCallback* callback,
        /* [in] */ IHandler* handler);

    CARAPI_(void) RecyclePendingEventLocked(
        /* [in] */ PendingEvent* p);

    CARAPI_(void) ShowInputMethodPickerLocked();

public:
    static const Boolean DEBUG;
    static const String TAG;

    static const String PENDING_EVENT_COUNTER;
    static AutoPtr<IInputMethodManager> sInstance;

    /**
     * Timeout in milliseconds for delivering a key to an IME.
     */
    static const Int64 INPUT_METHOD_NOT_RESPONDING_TIMEOUT;

    static const Int32 MSG_DUMP;
    static const Int32 MSG_BIND;
    static const Int32 MSG_UNBIND;
    static const Int32 MSG_SET_ACTIVE;
    static const Int32 MSG_SEND_INPUT_EVENT;
    static const Int32 MSG_TIMEOUT_INPUT_EVENT;
    static const Int32 MSG_FLUSH_INPUT_EVENT;
    static const Int32 MSG_SET_USER_ACTION_NOTIFICATION_SEQUENCE_NUMBER;

    /** @hide */
    static const Int32 DISPATCH_IN_PROGRESS;

    /** @hide */
    static const Int32 DISPATCH_NOT_HANDLED;

    /** @hide */
    static const Int32 DISPATCH_HANDLED;

    AutoPtr<IIInputMethodManager> mService;
    AutoPtr<ILooper> mMainLooper;

    // For scheduling work on the main thread.  This also serves as our
    // global lock.
    AutoPtr<IHandler> mH;
    Object mHLock;

    // Our generic input connection if the current target does not have its own.
    AutoPtr<IIInputContext> mIInputContext;

    /**
     * True if this input method client is active, initially false.
     */
    Boolean mActive;

    /**
     * Set whenever this client becomes inactive, to know we need to reset
     * state with the IME then next time we receive focus.
     */
    Boolean mHasBeenInactive;

    /**
     * As reported by IME through InputConnection.
     */
    Boolean mFullscreenMode;

    // -----------------------------------------------------------

    /**
     * This is the root view of the overall window that currently has input
     * method focus.
     */
    AutoPtr<IView> mCurRootView;
    /**
     * This is the view that should currently be served by an input method,
     * regardless of the state of setting that up.
     */
    AutoPtr<IView> mServedView;
    /**
     * This is then next view that will be served by the input method, when
     * we get around to updating things.
     */
    AutoPtr<IView> mNextServedView;
    /**
     * This is set when we are in the process of connecting, to determine
     * when we have actually finished.
     */
    Boolean mServedConnecting;
    /**
     * This is non-NULL when we have connected the served view; it holds
     * the attributes that were last retrieved from the served view and given
     * to the input connection.
     */
    AutoPtr<IEditorInfo> mCurrentTextBoxAttribute;
    /**
     * The InputConnection that was last retrieved from the served view.
     */
    AutoPtr<IInputConnection> mServedInputConnection;
    AutoPtr<CControlledInputConnectionWrapper> mServedInputConnectionWrapper;

    /**
     * The completions that were last provided by the served view.
     */
    AutoPtr< ArrayOf<ICompletionInfo*> > mCompletions;

    // Cursor position on the screen.
    AutoPtr<IRect> mTmpCursorRect;
    AutoPtr<IRect> mCursorRect;
    Int32 mCursorSelStart;
    Int32 mCursorSelEnd;
    Int32 mCursorCandStart;
    Int32 mCursorCandEnd;

    // -----------------------------------------------------------

    /**
     * Sequence number of this binding, as returned by the server.
     */
    Int32 mBindSequence;
    /**
     * ID of the method we are bound to.
     */
    String mCurId;
    /**
     * The actual instance of the method to make calls on it.
     */
    AutoPtr<IIInputMethodSession> mCurMethod;

    AutoPtr<IInputChannel> mCurChannel;
    AutoPtr<ImeInputEventSender> mCurSender;

    AutoPtr<Pools::Pool<PendingEvent> > mPendingEventPool;
    AutoPtr<ISparseArray> mPendingEvents;

    AutoPtr<IInputMethodClient> mClient;

    AutoPtr<IInputConnection> mDummyInputConnection;

private:
    /**
     * Represents an invalid action notification sequence number. {@link InputMethodManagerService}
     * always issues a positive integer for action notification sequence numbers. Thus -1 is
     * guaranteed to be different from any valid sequence number.
     */
    static const Int32 NOT_AN_ACTION_NOTIFICATION_SEQUENCE_NUMBER;
    /**
     * The next sequence number that is to be sent to {@link InputMethodManagerService} via
     * {@link IInputMethodManager#notifyUserAction(int)} at once when a user action is observed.
     */
    Int32 mNextUserActionNotificationSequenceNumber;

    /**
     * The last sequence number that is already sent to {@link InputMethodManagerService}.
     */
    Int32 mLastSentUserActionNotificationSequenceNumber;

    /**
     * The instance that has previously been sent to the input method.
     */
    AutoPtr<ICursorAnchorInfo> mCursorAnchorInfo;

    /**
     * The buffer to retrieve the view location in screen coordinates in {@link #updateCursor}.
     */
    AutoPtr<ArrayOf<Int32> > mViewTopLeft;

    /**
     * The matrix to convert the view location into screen coordinates in {@link #updateCursor}.
     */
    AutoPtr<IMatrix> mViewToScreenMatrix;

    static const Int32 REQUEST_UPDATE_CURSOR_ANCHOR_INFO_NONE;

    /**
     * The monitor mode for {@link #updateCursorAnchorInfo(View, CursorAnchorInfo)}.
     */
    Int32 mRequestUpdateCursorAnchorInfoMonitorMode;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::View::InputMethod::CInputMethodManager::PendingEvent, IInterface)

#endif   //__ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODMANAGER_H__
