
#ifndef __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSESSION_H__
#define __ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSESSION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.InputMethodService.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/service/voice/VoiceInteractionSessionService.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::InputMethodService::ISoftInputWindow;
using Elastos::Droid::InputMethodService::ISoftInputWindowCallback;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Internal::App::IIVoiceInteractorCallback;
using Elastos::Droid::Internal::App::IIVoiceInteractorRequest;
using Elastos::Droid::Internal::Os::IHandlerCaller;
using Elastos::Droid::Internal::Os::IHandlerCallerCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Voice::VoiceInteractionSessionService;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IInternalInsetsInfo;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IOnComputeInternalInsetsListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IFrameLayout;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

/**
 * An active interaction session, started by a {@link VoiceInteractionService}.
 */
class VoiceInteractionSession
    : public Object
    , public IKeyEventCallback
    , public IVoiceInteractionSession
{
private:
    class MInteractor
        : public Object
        , public IIVoiceInteractor
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MInteractor(
            /* [in] */ VoiceInteractionSession* host);

        // @Override
        CARAPI StartConfirmation(
            /* [in] */ const String& callingPackage,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [in] */ ICharSequence* prompt,
            /* [in] */ IBundle* extras,
            /* [out] */ IIVoiceInteractorRequest** result);

        // @Override
        CARAPI StartCompleteVoice(
            /* [in] */ const String& callingPackage,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [in] */ ICharSequence* message,
            /* [in] */ IBundle* extras,
            /* [out] */ IIVoiceInteractorRequest** result);

        // @Override
        CARAPI StartAbortVoice(
            /* [in] */ const String& callingPackage,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [in] */ ICharSequence* message,
            /* [in] */ IBundle* extras,
            /* [out] */ IIVoiceInteractorRequest** result);

        // @Override
        CARAPI StartCommand(
            /* [in] */ const String& callingPackage,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [in] */ const String& command,
            /* [in] */ IBundle* extras,
            /* [out] */ IIVoiceInteractorRequest** result);

        // @Override
        CARAPI SupportsCommands(
            /* [in] */ const String& callingPackage,
            /* [in] */ ArrayOf<String>* commands,
            /* [out, callee] */ ArrayOf<Boolean>** results);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        VoiceInteractionSession* mHost;
    };

    class MSession
        : public Object
        , public IIVoiceInteractionSession
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MSession(
            /* [in] */ VoiceInteractionSession* host);

        // @Override
        CARAPI TaskStarted(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 taskId);

        // @Override
        CARAPI TaskFinished(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 taskId);

        // @Override
        CARAPI CloseSystemDialogs();

        // @Override
        CARAPI Destroy();

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        VoiceInteractionSession* mHost;

    };

protected:
    class MyCallbacks
        : public Object
        , public IHandlerCallerCallback
        , public ISoftInputWindowCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyCallbacks(
            /* [in] */ VoiceInteractionSession* host);

        // @Override
        CARAPI ExecuteMessage(
            /* [in] */ IMessage* msg);

        // @Override
        CARAPI OnBackPressed();

    private:
        VoiceInteractionSession* mHost;
    };

public:
    /**
     * @hide
     */
    // @SystemApi
    class Request
        : public Object
        , public IVoiceInteractionSessionRequest
    {
    private:
        class MInterface
            : public Object
            , public IIVoiceInteractorRequest
            , public IBinder
        {
        public:
            CAR_INTERFACE_DECL()

            MInterface(
                /* [in] */ Request* host);

            // @Override
            CARAPI Cancel();

            ///pay for someone's error,he write a ToString in Binder.car
            CARAPI ToString(
                /* [out] */ String* str)
            {
                return NOERROR;
            }
            ///pay for someone's error

        private:
            Request* mHost;
        };

        friend class MInteractor;
        friend class MyCallbacks;
        friend class VoiceInteractionSession;
    public:
        CAR_INTERFACE_DECL()

        Request();

        CARAPI constructor(
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [in] */ IVoiceInteractionSession* session);

        CARAPI SendConfirmResult(
            /* [in] */ Boolean confirmed,
            /* [in] */ IBundle* result);

        CARAPI SendCompleteVoiceResult(
            /* [in] */ IBundle* result);

        CARAPI SendAbortVoiceResult(
            /* [in] */ IBundle* result);

        CARAPI SendCommandResult(
            /* [in] */ Boolean complete,
            /* [in] */ IBundle* result);

        CARAPI SendCancelResult();

    protected:
        CARAPI FinishRequest();

    protected:
        AutoPtr<IIVoiceInteractorRequest> mInterface;
        AutoPtr<IIVoiceInteractorCallback> mCallback;
        AutoPtr<IWeakReference> mSession;
    };

   /**
     * @hide
     */
    // @SystemApi
    class Caller
        : public Object
        , public IVoiceInteractionSessionCaller
    {
    public:
        CAR_INTERFACE_DECL()

        Caller();

        CARAPI constructor(
            /* [in] */ const String& _packageName,
            /* [in] */ Int32 _uid);

    public:
        String mPackageName;
        Int32 mUid;
    };

public:
    /**
     * @hide
     * Information about where interesting parts of the input method UI appear.
     */
    // @SystemApi
    class Insets
        : public Object
        , public IVoiceInteractionSessionInsets
    {
    public:
        CAR_INTERFACE_DECL()

        Insets();

        CARAPI constructor();

        /**
         * This is the part of the UI that is the main content.  It is
         * used to determine the basic space needed, to resize/pan the
         * application behind.  It is assumed that this inset does not
         * change very much, since any change will cause a full resize/pan
         * of the application behind.  This value is relative to the top edge
         * of the input method window.
         */
        const AutoPtr<IRect> mContentInsets;

        /**
         * This is the region of the UI that is touchable.  It is used when
         * {@link #touchableInsets} is set to {@link #TOUCHABLE_INSETS_REGION}.
         * The region should be specified relative to the origin of the window frame.
         */
        const AutoPtr<IRegion> mTouchableRegion;

        /**
         * Determine which area of the window is touchable by the user.  May
         * be one of: {@link #TOUCHABLE_INSETS_FRAME},
         * {@link #TOUCHABLE_INSETS_CONTENT}, or {@link #TOUCHABLE_INSETS_REGION}.
         */
        Int32 mTouchableInsets;
    };

private:
    class MInsetsComputer
        : public Object
        , public IOnComputeInternalInsetsListener
    {
    public:
        CAR_INTERFACE_DECL()

        MInsetsComputer(
            /* [in] */ VoiceInteractionSession* host);

        CARAPI OnComputeInternalInsets(
            /* [in] */ IInternalInsetsInfo* info);

    private:
        VoiceInteractionSession* mHost;
    };

    friend class VoiceInteractionSessionService;
public:
    CAR_INTERFACE_DECL()

    VoiceInteractionSession();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    CARAPI NewRequest(
        /* [in] */ IIVoiceInteractorCallback* callback,
        /* [out] */ IVoiceInteractionSessionRequest** request);

    CARAPI RemoveRequest(
        /* [in] */ IBinder* reqInterface,
        /* [out] */ IVoiceInteractionSessionRequest** request);

    CARAPI DoCreate(
        /* [in] */ IIVoiceInteractionManagerService* service,
        /* [in] */ IBinder* token,
        /* [in] */ IBundle* args);

    CARAPI DoDestroy();

    CARAPI InitViews();

    /**
     * @hide
     */
    // @SystemApi
    CARAPI ShowWindow();

    /**
     * @hide
     */
    // @SystemApi
    CARAPI HideWindow();

    /**
     * @hide
     * You can call this to customize the theme used by your IME's window.
     * This must be set before {@link #onCreate}, so you
     * will typically call it in your constructor with the resource ID
     * of your custom theme.
     */
    // @SystemApi
    CARAPI SetTheme(
        /* [in] */ Int32 theme);

    /**
     * @hide
     * Ask that a new activity be started for voice interaction.  This will create a
     * new dedicated task in the activity manager for this voice interaction session;
     * this means that {@link Intent#FLAG_ACTIVITY_NEW_TASK Intent.FLAG_ACTIVITY_NEW_TASK}
     * will be set for you to make it a new task.
     *
     * <p>The newly started activity will be displayed to the user in a special way, as
     * a layer under the voice interaction UI.</p>
     *
     * <p>As the voice activity runs, it can retrieve a {@link android.app.VoiceInteractor}
     * through which it can perform voice interactions through your session.  These requests
     * for voice interactions will appear as callbacks on {@link #onGetSupportedCommands},
     * {@link #onConfirm}, {@link #onCommand}, and {@link #onCancel}.
     *
     * <p>You will receive a call to {@link #onTaskStarted} when the task starts up
     * and {@link #onTaskFinished} when the last activity has finished.
     *
     * @param intent The Intent to start this voice interaction.  The given Intent will
     * always have {@link Intent#CATEGORY_VOICE Intent.CATEGORY_VOICE} added to it, since
     * this is part of a voice interaction.
     */
    // @SystemApi
    CARAPI StartVoiceActivity(
        /* [in] */ IIntent* intent);

    /**
     * @hide
     * Convenience for inflating views.
     */
    // @SystemApi
    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    /**
     * @hide
     * Retrieve the window being used to show the session's UI.
     */
    // @SystemApi
    CARAPI GetWindow(
        /* [out] */ IDialog** window);

    /**
     * Finish the session.
     */
    CARAPI Finish();

    /**
     * Initiatize a new session.
     *
     * @param args The arguments that were supplied to
     * {@link VoiceInteractionService#startSession VoiceInteractionService.startSession}.
     */
    CARAPI OnCreate(
        /* [in] */ IBundle* args);

    /**
     * Last callback to the session as it is being finished.
     */
    CARAPI OnDestroy();

    /**
     * @hide
     * Hook in which to create the session's UI.
     */
    // @SystemApi
    CARAPI OnCreateContentView(
        /* [out] */ IView** view);

    CARAPI SetContentView(
        /* [in] */ IView* view);

    /**
     * @hide
     */
    // @SystemApi
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    // @SystemApi
    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    // @SystemApi
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    // @SystemApi
    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    // @SystemApi
    CARAPI OnBackPressed();

    /**
     * Sessions automatically watch for requests that all system UI be closed (such as when
     * the user presses HOME), which will appear here.  The default implementation always
     * calls {@link #finish}.
     */
    CARAPI OnCloseSystemDialogs();

    /**
     * @hide
     * Compute the interesting insets into your UI.  The default implementation
     * uses the entire window frame as the insets.  The default touchable
     * insets are {@link Insets#TOUCHABLE_INSETS_FRAME}.
     *
     * @param outInsets Fill in with the current UI insets.
     */
    // @SystemApi
    CARAPI OnComputeInsets(
        /* [in] */ IVoiceInteractionSessionInsets* outInsets);

    /**
     * @hide
     * @SystemApi
     * Called when a task initiated by {@link #startVoiceActivity(android.content.Intent)}
     * has actually started.
     *
     * @param intent The original {@link Intent} supplied to
     * {@link #startVoiceActivity(android.content.Intent)}.
     * @param taskId Unique ID of the now running task.
     */
    CARAPI OnTaskStarted(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 taskId);

    /**
     * @hide
     * @SystemApi
     * Called when the last activity of a task initiated by
     * {@link #startVoiceActivity(android.content.Intent)} has finished.  The default
     * implementation calls {@link #finish()} on the assumption that this represents
     * the completion of a voice action.  You can override the implementation if you would
     * like a different behavior.
     *
     * @param intent The original {@link Intent} supplied to
     * {@link #startVoiceActivity(android.content.Intent)}.
     * @param taskId Unique ID of the finished task.
     */
    CARAPI OnTaskFinished(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 taskId);

    /**
     * @hide
     * @SystemApi
     * Request to query for what extended commands the session supports.
     *
     * @param caller Who is making the request.
     * @param commands An array of commands that are being queried.
     * @return Return an array of booleans indicating which of each entry in the
     * command array is supported.  A true entry in the array indicates the command
     * is supported; false indicates it is not.  The default implementation returns
     * an array of all false entries.
     */
    CARAPI OnGetSupportedCommands(
        /* [in] */ IVoiceInteractionSessionCaller* caller,
        /* [in] */ ArrayOf<String>* commands,
        /* [out, callee] */ ArrayOf<Boolean>** results);

    /**
     * @hide
     * @SystemApi
     * Request to confirm with the user before proceeding with an unrecoverable operation,
     * corresponding to a {@link android.app.VoiceInteractor.ConfirmationRequest
     * VoiceInteractor.ConfirmationRequest}.
     *
     * @param caller Who is making the request.
     * @param request The active request.
     * @param prompt The prompt informing the user of what will happen, as per
     * {@link android.app.VoiceInteractor.ConfirmationRequest VoiceInteractor.ConfirmationRequest}.
     * @param extras Any additional information, as per
     * {@link android.app.VoiceInteractor.ConfirmationRequest VoiceInteractor.ConfirmationRequest}.
     */
    virtual CARAPI OnConfirm(
        /* [in] */ IVoiceInteractionSessionCaller* caller,
        /* [in] */ IVoiceInteractionSessionRequest* request,
        /* [in] */ ICharSequence* prompt,
        /* [in] */ IBundle* extras) = 0;

    /**
     * @hide
     * @SystemApi
     * Request to complete the voice interaction session because the voice activity successfully
     * completed its interaction using voice.  Corresponds to
     * {@link android.app.VoiceInteractor.CompleteVoiceRequest
     * VoiceInteractor.CompleteVoiceRequest}.  The default implementation just sends an empty
     * confirmation back to allow the activity to exit.
     *
     * @param caller Who is making the request.
     * @param request The active request.
     * @param message The message informing the user of the problem, as per
     * {@link android.app.VoiceInteractor.CompleteVoiceRequest
     * VoiceInteractor.CompleteVoiceRequest}.
     * @param extras Any additional information, as per
     * {@link android.app.VoiceInteractor.CompleteVoiceRequest
     * VoiceInteractor.CompleteVoiceRequest}.
     */
    CARAPI OnCompleteVoice(
        /* [in] */ IVoiceInteractionSessionCaller* caller,
        /* [in] */ IVoiceInteractionSessionRequest* request,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras);

    /**
     * @hide
     * @SystemApi
     * Request to abort the voice interaction session because the voice activity can not
     * complete its interaction using voice.  Corresponds to
     * {@link android.app.VoiceInteractor.AbortVoiceRequest
     * VoiceInteractor.AbortVoiceRequest}.  The default implementation just sends an empty
     * confirmation back to allow the activity to exit.
     *
     * @param caller Who is making the request.
     * @param request The active request.
     * @param message The message informing the user of the problem, as per
     * {@link android.app.VoiceInteractor.AbortVoiceRequest VoiceInteractor.AbortVoiceRequest}.
     * @param extras Any additional information, as per
     * {@link android.app.VoiceInteractor.AbortVoiceRequest VoiceInteractor.AbortVoiceRequest}.
     */
    CARAPI OnAbortVoice(
        /* [in] */ IVoiceInteractionSessionCaller* caller,
        /* [in] */ IVoiceInteractionSessionRequest* request,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras);

    /**
     * @hide
     * @SystemApi
     * Process an arbitrary extended command from the caller,
     * corresponding to a {@link android.app.VoiceInteractor.CommandRequest
     * VoiceInteractor.CommandRequest}.
     *
     * @param caller Who is making the request.
     * @param request The active request.
     * @param command The command that is being executed, as per
     * {@link android.app.VoiceInteractor.CommandRequest VoiceInteractor.CommandRequest}.
     * @param extras Any additional information, as per
     * {@link android.app.VoiceInteractor.CommandRequest VoiceInteractor.CommandRequest}.
     */
    virtual CARAPI OnCommand(
        /* [in] */ IVoiceInteractionSessionCaller* caller,
        /* [in] */ IVoiceInteractionSessionRequest* request,
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras) = 0;

    /**
     * @hide
     * @SystemApi
     * Called when the {@link android.app.VoiceInteractor} has asked to cancel a {@link Request}
     * that was previously delivered to {@link #onConfirm} or {@link #onCommand}.
     *
     * @param request The request that is being canceled.
     */
    virtual CARAPI OnCancel(
        /* [in] */ IVoiceInteractionSessionRequest* request) = 0;

protected:
    const static String TAG;
    const static Boolean DEBUG;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandlerCaller> mHandlerCaller;

    AutoPtr<IDispatcherState> mDispatcherState;

    AutoPtr<IIVoiceInteractionManagerService> mSystemService;
    AutoPtr<IBinder> mToken;

    Int32 mTheme;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<ITypedArray> mThemeAttrs;
    AutoPtr<IView> mRootView;
    AutoPtr<IFrameLayout> mContentFrame;
    AutoPtr<ISoftInputWindow> mWindow;

    Boolean mInitialized;
    Boolean mWindowAdded;
    Boolean mWindowVisible;
    Boolean mWindowWasVisible;
    Boolean mInShowWindow;

    AutoPtr<IArrayMap> mActiveRequests;

    AutoPtr<IVoiceInteractionSessionInsets> mTmpInsets;
    AutoPtr<ArrayOf<Int32> > mTmpLocation;

    AutoPtr<IWeakReference> mWeakRef;

    AutoPtr<IIVoiceInteractionSession> mSession;
    AutoPtr<IIVoiceInteractor> mInteractor;

    const static Int32 MSG_START_CONFIRMATION = 1;
    const static Int32 MSG_START_COMPLETE_VOICE = 2;
    const static Int32 MSG_START_ABORT_VOICE = 3;
    const static Int32 MSG_START_COMMAND = 4;
    const static Int32 MSG_SUPPORTS_COMMANDS = 5;
    const static Int32 MSG_CANCEL = 6;

    const static Int32 MSG_TASK_STARTED = 100;
    const static Int32 MSG_TASK_FINISHED = 101;
    const static Int32 MSG_CLOSE_SYSTEM_DIALOGS = 102;
    const static Int32 MSG_DESTROY = 103;

    AutoPtr<MyCallbacks> mCallbacks;
    AutoPtr<IOnComputeInternalInsetsListener> mInsetsComputer;
};

}//namespace Voice
}//namespace Service
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVICE_VOICE_VOICEINTERACTIONSESSION_H__