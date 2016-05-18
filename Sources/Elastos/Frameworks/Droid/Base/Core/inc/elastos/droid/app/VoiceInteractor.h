
#ifndef __ELASTOS_DROID_APP_VOICE_INTERACTOR_H__
#define __ELASTOS_DROID_APP_VOICE_INTERACTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#define HASH_FOR_OS
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Internal::App::IIVoiceInteractorCallback;
using Elastos::Droid::Internal::App::IIVoiceInteractorRequest;
using Elastos::Droid::Internal::Os::IHandlerCaller;
using Elastos::Droid::Internal::Os::IHandlerCallerCallback;

using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * @hide
 * Interface for an {@link Activity} to interact with the user through voice.  Use
 * {@link android.app.Activity#getVoiceInteractor() Activity.getVoiceInteractor}
 * to retrieve the interface, if the activity is currently involved in a voice interaction.
 *
 * <p>The voice interactor revolves around submitting voice interaction requests to the
 * back-end voice interaction service that is working with the user.  These requests are
 * submitted with {@link #submitRequest}, providing a new instance of a
 * {@link Request} subclass describing the type of operation to perform -- currently the
 * possible requests are {@link ConfirmationRequest} and {@link CommandRequest}.
 *
 * <p>Once a request is submitted, the voice system will process it and eventually deliver
 * the result to the request object.  The application can cancel a pending request at any
 * time.
 *
 * <p>The VoiceInteractor is integrated with Activity's state saving mechanism, so that
 * if an activity is being restarted with retained state, it will retain the current
 * VoiceInteractor and any outstanding requests.  Because of this, you should always use
 * {@link Request#getActivity() Request.getActivity} to get back to the activity of a
 * request, rather than holding on to the activity instance yourself, either explicitly
 * or implicitly through a non-static inner class.
 */
//@SystemApi
class VoiceInteractor
    : public Object
    , public IVoiceInteractor
{
public:
    class HandlerCallerCallback
        : public Object
        , public IHandlerCallerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        HandlerCallerCallback(
            /* [in] */ VoiceInteractor* host);

        CARAPI ExecuteMessage(
            /* [in] */ IMessage* msg);
    private:
        VoiceInteractor* mHost;
    };

    class VoiceInteractorCallback
        : public Object
        , public IIVoiceInteractorCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        VoiceInteractorCallback();

        virtual ~VoiceInteractorCallback();

        CARAPI constructor(
            /* [in] */ IObject* handlerCaller);

        //@Override
        CARAPI DeliverConfirmationResult(
            /* [in] */ IIVoiceInteractorRequest* request,
            /* [in] */ Boolean confirmed,
            /* [in] */ IBundle* result);

        //@Override
        CARAPI DeliverCompleteVoiceResult(
            /* [in] */ IIVoiceInteractorRequest* request,
            /* [in] */ IBundle* result);

        //@Override
        CARAPI DeliverAbortVoiceResult(
            /* [in] */ IIVoiceInteractorRequest* request,
            /* [in] */ IBundle* result);

        //@Override
        CARAPI DeliverCommandResult(
            /* [in] */ IIVoiceInteractorRequest* request,
            /* [in] */ Boolean complete,
            /* [in] */ IBundle* result);

        //@Override
        CARAPI DeliverCancel(
            /* [in] */ IIVoiceInteractorRequest* request);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHandlerCaller> mHandlerCaller;
    };

    class Request
        : public Object
        , public IVoiceInteractorRequest
    {
    public:
        CAR_INTERFACE_DECL()

        Request();

        virtual ~Request();

        CARAPI Cancel();

        CARAPI GetContext(
            /* [out] */ IContext** ctx);

        CARAPI GetActivity(
            /* [out] */ IActivity** activity);

        CARAPI OnCancel();

        CARAPI OnAttached(
            /* [in] */ IActivity* activity);

        CARAPI OnDetached();

        CARAPI Clear();

        virtual CARAPI Submit(
            /* [in] */ IIVoiceInteractor* interactor,
            /* [in] */ const String& packageName,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [out] */ IIVoiceInteractorRequest** request) = 0;

    private:
        friend class VoiceInteractor;

        AutoPtr<IIVoiceInteractorRequest> mRequestInterface;
        AutoPtr<IContext> mContext;
        AutoPtr<IActivity> mActivity;
    };

    class ConfirmationRequest
        : public Request
        , public IVoiceInteractorConfirmationRequest
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Confirms an operation with the user via the trusted system
         * VoiceInteractionService.  This allows an Activity to complete an unsafe operation that
         * would require the user to touch the screen when voice interaction mode is not enabled.
         * The result of the confirmation will be returned through an asynchronous call to
         * either {@link #onConfirmationResult(Boolean, android.os.Bundle)} or
         * {@link #onCancel()}.
         *
         * <p>In some cases this may be a simple yes / no confirmation or the confirmation could
         * include context information about how the action will be completed
         * (e.g. booking a cab might include details about how long until the cab arrives)
         * so the user can give a confirmation.
         * @param prompt Optional confirmation text to read to the user as the action being
         * confirmed.
         * @param extras Additional optional information.
         */
        CARAPI constructor(
            /* [in] */ ICharSequence* prompt,
            /* [in] */ IBundle* extras);

        CARAPI OnConfirmationResult(
            /* [in] */ Boolean confirmed,
            /* [in] */ IBundle* result);

        CARAPI Submit(
            /* [in] */ IIVoiceInteractor* interactor,
            /* [in] */ const String& packageName,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [out] */ IIVoiceInteractorRequest** request);
    private:
        AutoPtr<ICharSequence> mPrompt;
        AutoPtr<IBundle> mExtras;
    };

    class CompleteVoiceRequest
        : public Request
        , public IVoiceInteractorCompleteVoiceRequest
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Reports that the current interaction was successfully completed with voice, so the
         * application can report the final status to the user. When the response comes back, the
         * voice system has handled the request and is ready to switch; at that point the
         * application can start a new non-voice activity or finish.  Be sure when starting the new
         * activity to use {@link android.content.Intent#FLAG_ACTIVITY_NEW_TASK
         * Intent.FLAG_ACTIVITY_NEW_TASK} to keep the new activity out of the current voice
         * interaction task.
         *
         * @param message Optional message to tell user about the completion status of the task.
         * @param extras Additional optional information.
         */
        CARAPI constructor(
            /* [in] */ ICharSequence* message,
            /* [in] */ IBundle* extras);

        CARAPI OnCompleteResult(
            /* [in] */ IBundle* result);

        CARAPI Submit(
            /* [in] */ IIVoiceInteractor* interactor,
            /* [in] */ const String& packageName,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [out] */ IIVoiceInteractorRequest** request);

    private:
        AutoPtr<ICharSequence> mMessage;
        AutoPtr<IBundle> mExtras;
    };

    class AbortVoiceRequest
        : public Request
        , public IVoiceInteractorAbortVoiceRequest
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Reports that the current interaction can not be complete with voice, so the
         * application will need to switch to a traditional input UI.  Applications should
         * only use this when they need to completely bail out of the voice interaction
         * and switch to a traditional UI.  When the response comes back, the voice
         * system has handled the request and is ready to switch; at that point the application
         * can start a new non-voice activity.  Be sure when starting the new activity
         * to use {@link android.content.Intent#FLAG_ACTIVITY_NEW_TASK
         * Intent.FLAG_ACTIVITY_NEW_TASK} to keep the new activity out of the current voice
         * interaction task.
         *
         * @param message Optional message to tell user about not being able to complete
         * the interaction with voice.
         * @param extras Additional optional information.
         */
        CARAPI constructor(
            /* [in] */ ICharSequence* message,
            /* [in] */ IBundle* extras);

        CARAPI OnAbortResult(
            /* [in] */ IBundle* result);

        CARAPI Submit(
            /* [in] */ IIVoiceInteractor* interactor,
            /* [in] */ const String& packageName,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [out] */ IIVoiceInteractorRequest** request);

    private:
        AutoPtr<ICharSequence> mMessage;
        AutoPtr<IBundle> mExtras;
    };

    class CommandRequest
        : public Request
        , public IVoiceInteractorCommandRequest
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Execute a command using the trusted system VoiceInteractionService.
         * This allows an Activity to request additional information from the user needed to
         * complete an action (e.g. booking a table might have several possible times that the
         * user could select from or an app might need the user to agree to a terms of service).
         * The result of the confirmation will be returned through an asynchronous call to
         * either {@link #onCommandResult(android.os.Bundle)} or
         * {@link #onCancel()}.
         *
         * <p>The command is a string that describes the generic operation to be performed.
         * The command will determine how the properties in extras are interpreted and the set of
         * available commands is expected to grow over time.  An example might be
         * "com.google.voice.commands.REQUEST_NUMBER_BAGS" to request the number of bags as part of
         * airline check-in.  (This is not an actual working example.)
         *
         * @param command The desired command to perform.
         * @param args Additional arguments to control execution of the command.
         */
        CARAPI constructor(
            /* [in] */ const String& command,
            /* [in] */ IBundle* args);

        CARAPI OnCommandResult(
            /* [in] */ IBundle* result);

        CARAPI Submit(
            /* [in] */ IIVoiceInteractor* interactor,
            /* [in] */ const String& packageName,
            /* [in] */ IIVoiceInteractorCallback* callback,
            /* [out] */ IIVoiceInteractorRequest** request);

    private:
        String mCommand;
        AutoPtr<IBundle> mArgs;
   };

public:
    CAR_INTERFACE_DECL()

    VoiceInteractor();

    virtual ~VoiceInteractor();

    CARAPI constructor(
        /* [in] */ IIVoiceInteractor* interactor,
        /* [in] */ IContext* context,
        /* [in] */ IActivity* activity,
        /* [in] */ ILooper* looper);

    AutoPtr<IVoiceInteractorRequest> PullRequest(
        /* [in] */ IIVoiceInteractorRequest* request,
        /* [in] */ Boolean complete);

    AutoPtr<IArrayList> MakeRequestList();

    CARAPI AttachActivity(
        /* [in] */ IActivity* activity);

    CARAPI DetachActivity();

    CARAPI SubmitRequest(
        /* [in] */ IVoiceInteractorRequest* request,
        /* [out] */ Boolean* result);

    /**
     * Queries the supported commands available from the VoiceinteractionService.
     * The command is a string that describes the generic operation to be performed.
     * An example might be "com.google.voice.commands.REQUEST_NUMBER_BAGS" to request the number
     * of bags as part of airline check-in.  (This is not an actual working example.)
     *
     * @param commands
     */
    CARAPI SupportsCommands(
        /* [in] */ ArrayOf<String>* commands,
        /* [out, callee] */ ArrayOf<Boolean>** results);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IIVoiceInteractor> mInteractor;

    AutoPtr<IContext> mContext;
    AutoPtr<IActivity> mActivity;

    AutoPtr<IHandlerCaller> mHandlerCaller;
    AutoPtr<IHandlerCallerCallback> mHandlerCallerCallback;

    AutoPtr<IIVoiceInteractorCallback> mCallback;

    HashMap<AutoPtr<IBinder>, AutoPtr<IVoiceInteractorRequest> > mActiveRequests;
    Object mActiveRequestsLock;

    static const Int32 MSG_CONFIRMATION_RESULT;
    static const Int32 MSG_COMPLETE_VOICE_RESULT;
    static const Int32 MSG_ABORT_VOICE_RESULT;
    static const Int32 MSG_COMMAND_RESULT;
    static const Int32 MSG_CANCEL_RESULT;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_VOICE_INTERACTOR_H__

