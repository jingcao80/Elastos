
#ifndef __ELASTOS_DROID_CONTENT_BROADCASTRECEIVER_H__
#define __ELASTOS_DROID_CONTENT_BROADCASTRECEIVER_H__

#include "Elastos.Droid.Content.h"
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Droid {
namespace Content {

class ECO_PUBLIC BroadcastReceiver
    : public Object
    , public IBroadcastReceiver
{
public:

    /**
     * State for a result that is pending for a broadcast receiver.  Returned
     * by {@link BroadcastReceiver#goAsync() goAsync()}
     * while in {@link BroadcastReceiver#onReceive BroadcastReceiver.onReceive()}.
     * This allows you to return from onReceive() without having the broadcast
     * terminate; you must call {@link #finish()} once you are done with the
     * broadcast.  This allows you to process the broadcast off of the main
     * thread of your app.
     *
     * <p>Note on threading: the state inside of this class is not itself
     * thread-safe, however you can use it from any thread if you properly
     * sure that you do not have races.  Typically this means you will hand
     * the entire object to another thread, which will be solely responsible
     * for setting any results and finally calling {@link #finish()}.
     */
    class PendingResult
        : public Object
        , public IPendingResult
    {
    private:
        class FinishRunnable
            : public Runnable
        {
        public:
            FinishRunnable(
                /* [in] */ PendingResult* host);

            CARAPI Run();
        private:
            AutoPtr<PendingResult> mPendingResult;
        };
    public:
        CAR_INTERFACE_DECL()

        PendingResult();

        PendingResult(
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& resultData,
            /* [in] */ IBundle* resultExtras,
            /* [in] */ Int32 type,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ IBinder* token,
            /* [in] */ Int32 userId);

        virtual ~PendingResult();

        CARAPI constructor(
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& resultData,
            /* [in] */ IBundle* resultExtras,
            /* [in] */ Int32 type,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ IBinder* token,
            /* [in] */ Int32 userId);

        /**
         * Version of {@link BroadcastReceiver#setResultCode(int)
         * BroadcastReceiver.setResultCode(int)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResultCode(
            /* [in] */ Int32 code);

        /**
         * Version of {@link BroadcastReceiver#getResultCode()
         * BroadcastReceiver.getResultCode()} for
         * asynchronous broadcast handling.
         */
        CARAPI GetResultCode(
            /* [out] */ Int32* resultCode);

        /**
         * Version of {@link BroadcastReceiver#setResultData(String)
         * BroadcastReceiver.setResultData(String)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResultData(
            /* [in] */ const String& data);

        /**
         * Version of {@link BroadcastReceiver#getResultData()
         * BroadcastReceiver.getResultData()} for
         * asynchronous broadcast handling.
         */
        CARAPI GetResultData(
            /* [out] */ String* resultData);

        /**
         * Version of {@link BroadcastReceiver#setResultExtras(Bundle)
         * BroadcastReceiver.setResultExtras(Bundle)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResultExtras(
            /* [in] */ IBundle* extras);

        /**
         * Version of {@link BroadcastReceiver#getResultExtras(boolean)
         * BroadcastReceiver.getResultExtras(boolean)} for
         * asynchronous broadcast handling.
         */
        CARAPI GetResultExtras(
            /* [in] */ Boolean makeMap,
            /* [out] */ IBundle** resultExtras);

        /**
         * Version of {@link BroadcastReceiver#setResult(int, String, Bundle)
         * BroadcastReceiver.setResult(int, String, Bundle)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResult(
            /* [in] */ Int32 code,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras);

        /**
         * Version of {@link BroadcastReceiver#getAbortBroadcast()
         * BroadcastReceiver.getAbortBroadcast()} for
         * asynchronous broadcast handling.
         */
        CARAPI GetAbortBroadcast(
            /* [out] */ Boolean* isAborted);

        /**
         * Version of {@link BroadcastReceiver#abortBroadcast()
         * BroadcastReceiver.abortBroadcast()} for
         * asynchronous broadcast handling.
         */
        CARAPI AbortBroadcast();

        /**
         * Version of {@link BroadcastReceiver#clearAbortBroadcast()
         * BroadcastReceiver.clearAbortBroadcast()} for
         * asynchronous broadcast handling.
         */
        CARAPI ClearAbortBroadcast();

        /**
         * Finish the broadcast.  The current result will be sent and the
         * next broadcast will proceed.
         */
        CARAPI Finish();

        /** @hide */
        CARAPI SetExtrasClassLoader(
            /* [in] */ IClassLoader* cl);

        /** @hide */
        CARAPI SendFinished(
            /* [in] */ IIActivityManager* am);

        /** @hide */
        CARAPI GetSendingUserId(
            /* [out] */ Int32* userId);

        CARAPI GetOrderedHint(
            /* [out] */ Boolean* orderedHint);

        CARAPI GetInitialStickyHint(
            /* [out] */ Boolean* initialStickyHint);

    private:
        CARAPI CheckSynchronousHint();

    public:
        /** @hide */
        const static Int32 TYPE_COMPONENT = 0;
        /** @hide */
        const static Int32 TYPE_REGISTERED = 1;
        /** @hide */
        const static Int32 TYPE_UNREGISTERED = 2;

    private:
        Int32 mType;
        Boolean mOrderedHint;
        Boolean mInitialStickyHint;
        AutoPtr<IBinder> mToken;
        Int32 mSendingUser;
        Int32 mResultCode;
        String mResultData;
        AutoPtr<IBundle> mResultExtras;
        Boolean mAbortBroadcast;
        Boolean mFinished;
        Object mPendingResultLock;
    };

public:
    CAR_INTERFACE_DECL()

    BroadcastReceiver();

    virtual ~BroadcastReceiver();

    CARAPI constructor();

    CARAPI ToString(
        /* [out] */ String* info);

    /**
     * This method is called when the BroadcastReceiver is receiving an Intent
     * broadcast.  During this time you can use the other methods on
     * BroadcastReceiver to view/modify the current result values.  This method
     * is always called within the main thread of its process, unless you
     * explicitly asked for it to be scheduled on a different thread using
     * {@link android.content.Context#registerReceiver(BroadcastReceiver,
     * IntentFilter, String, android.os.Handler)}. When it runs on the main
     * thread you should
     * never perform long-running operations in it (there is a timeout of
     * 10 seconds that the system allows before considering the receiver to
     * be blocked and a candidate to be killed). You cannot launch a popup dialog
     * in your implementation of onReceive().
     *
     * <p><b>If this BroadcastReceiver was launched through a &lt;receiver&gt; tag,
     * then the object is no longer alive after returning from this
     * function.</b>  This means you should not perform any operations that
     * return a result to you asynchronously -- in particular, for interacting
     * with services, you should use
     * {@link Context#startService(Intent)} instead of
     * {@link Context#bindService(Intent, ServiceConnection, int)}.  If you wish
     * to interact with a service that is already running, you can use
     * {@link #peekService}.
     *
     * <p>The Intent filters used in {@link android.content.Context#registerReceiver}
     * and in application manifests are <em>not</em> guaranteed to be exclusive. They
     * are hints to the operating system about how to find suitable recipients. It is
     * possible for senders to force delivery to specific recipients, bypassing filter
     * resolution.  For this reason, {@link #onReceive(Context, Intent) onReceive()}
     * implementations should respond only to known actions, ignoring any unexpected
     * Intents that they may receive.
     *
     * @param context The Context in which the receiver is running.
     * @param intent The Intent being received.
     */
    virtual CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent) = 0;

    /**
     * This can be called by an application in {@link #onReceive} to allow
     * it to keep the broadcast active after returning from that function.
     * This does <em>not</em> change the expectation of being relatively
     * responsive to the broadcast (finishing it within 10s), but does allow
     * the implementation to move work related to it over to another thread
     * to avoid glitching the main UI thread due to disk IO.
     *
     * @return Returns a {@link PendingResult} representing the result of
     * the active broadcast.  The BroadcastRecord itself is no longer active;
     * all data and other interaction must go through {@link PendingResult}
     * APIs.  The {@link PendingResult#finish PendingResult.finish()} method
     * must be called once processing of the broadcast is done.
     */
    virtual CARAPI GoAsync(
        /* [out] */ IPendingResult** pendingResult);

    /**
     * Provide a binder to an already-running service.  This method is synchronous
     * and will not start the target service if it is not present, so it is safe
     * to call from {@link #onReceive}.
     *
     * @param myContext The Context that had been passed to {@link #onReceive(Context, Intent)}
     * @param service The Intent indicating the service you wish to use.  See {@link
     * Context#startService(Intent)} for more information.
     */
    virtual CARAPI PeekService(
        /* [in] */ IContext* myContext,
        /* [in] */ IIntent* service,
        /* [out] */ IBinder** binder);

    /**
     * Change the current result code of this broadcast; only works with
     * broadcasts sent through
     * {@link Context#sendOrderedBroadcast(Intent, String)
     * Context.sendOrderedBroadcast}.  Often uses the
     * Activity {@link android.app.Activity#RESULT_CANCELED} and
     * {@link android.app.Activity#RESULT_OK} constants, though the
     * actual meaning of this value is ultimately up to the broadcaster.
     *
     * <p class="note">This method does not work with non-ordered broadcasts such
     * as those sent with {@link Context#sendBroadcast(Intent)
     * Context.sendBroadcast}</p>
     *
     * @param code The new result code.
     *
     * @see #setResult(int, String, Bundle)
     */
    virtual CARAPI SetResultCode(
        /* [in] */ Int32 code);

    /**
     * Retrieve the current result code, as set by the previous receiver.
     *
     * @return int The current result code.
     */
    virtual CARAPI GetResultCode(
        /* [out] */ Int32* code);

    /**
     * Change the current result data of this broadcast; only works with
     * broadcasts sent through
     * {@link Context#sendOrderedBroadcast(Intent, String)
     * Context.sendOrderedBroadcast}.  This is an arbitrary
     * string whose interpretation is up to the broadcaster.
     *
     * <p><strong>This method does not work with non-ordered broadcasts such
     * as those sent with {@link Context#sendBroadcast(Intent)
     * Context.sendBroadcast}</strong></p>
     *
     * @param data The new result data; may be null.
     *
     * @see #setResult(int, String, Bundle)
     */
    virtual CARAPI SetResultData(
        /* [in] */ const String& data);

    /**
     * Retrieve the current result data, as set by the previous receiver.
     * Often this is null.
     *
     * @return String The current result data; may be null.
     */
    virtual CARAPI GetResultData(
        /* [out] */ String* data);

    /**
     * Change the current result extras of this broadcast; only works with
     * broadcasts sent through
     * {@link Context#sendOrderedBroadcast(Intent, String)
     * Context.sendOrderedBroadcast}.  This is a Bundle
     * holding arbitrary data, whose interpretation is up to the
     * broadcaster.  Can be set to null.  Calling this method completely
     * replaces the current map (if any).
     *
     * <p><strong>This method does not work with non-ordered broadcasts such
     * as those sent with {@link Context#sendBroadcast(Intent)
     * Context.sendBroadcast}</strong></p>
     *
     * @param extras The new extra data map; may be null.
     *
     * @see #setResult(int, String, Bundle)
     */
    virtual CARAPI SetResultExtras(
        /* [in] */ IBundle* extras);

    /**
     * Retrieve the current result extra data, as set by the previous receiver.
     * Any changes you make to the returned Map will be propagated to the next
     * receiver.
     *
     * @param makeMap If true then a new empty Map will be made for you if the
     *                current Map is null; if false you should be prepared to
     *                receive a null Map.
     *
     * @return Map The current extras map.
     */
    virtual CARAPI GetResultExtras(
        /* [in] */ Boolean makeMap,
        /* [out] */ IBundle** extras);

    /**
     * Change all of the result data returned from this broadcasts; only works
     * with broadcasts sent through
     * {@link Context#sendOrderedBroadcast(Intent, String)
     * Context.sendOrderedBroadcast}.  All current result data is replaced
     * by the value given to this method.
     *
     * <p><strong>This method does not work with non-ordered broadcasts such
     * as those sent with {@link Context#sendBroadcast(Intent)
     * Context.sendBroadcast}</strong></p>
     *
     * @param code The new result code.  Often uses the
     * Activity {@link android.app.Activity#RESULT_CANCELED} and
     * {@link android.app.Activity#RESULT_OK} constants, though the
     * actual meaning of this value is ultimately up to the broadcaster.
     * @param data The new result data.  This is an arbitrary
     * string whose interpretation is up to the broadcaster; may be null.
     * @param extras The new extra data map.  This is a Bundle
     * holding arbitrary data, whose interpretation is up to the
     * broadcaster.  Can be set to null.  This completely
     * replaces the current map (if any).
     */
    virtual CARAPI SetResult(
        /* [in] */ Int32 code,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras);

    /**
     * Returns the flag indicating whether or not this receiver should
     * abort the current broadcast.
     *
     * @return True if the broadcast should be aborted.
     */
    virtual CARAPI GetAbortBroadcast(
        /* [out] */ Boolean* isAborted);

    /**
     * Sets the flag indicating that this receiver should abort the
     * current broadcast; only works with broadcasts sent through
     * {@link Context#sendOrderedBroadcast(Intent, String)
     * Context.sendOrderedBroadcast}.  This will prevent
     * any other broadcast receivers from receiving the broadcast. It will still
     * call {@link #onReceive} of the BroadcastReceiver that the caller of
     * {@link Context#sendOrderedBroadcast(Intent, String)
     * Context.sendOrderedBroadcast} passed in.
     *
     * <p><strong>This method does not work with non-ordered broadcasts such
     * as those sent with {@link Context#sendBroadcast(Intent)
     * Context.sendBroadcast}</strong></p>
     */
    virtual CARAPI AbortBroadcast();

    /**
     * Clears the flag indicating that this receiver should abort the current
     * broadcast.
     */
    virtual CARAPI ClearAbortBroadcast();

    /**
     * Returns true if the receiver is currently processing an ordered
     * broadcast.
     */
    virtual CARAPI IsOrderedBroadcast(
        /* [out] */ Boolean* isOrdered);

    /**
     * Returns true if the receiver is currently processing the initial
     * value of a sticky broadcast -- that is, the value that was last
     * broadcast and is currently held in the sticky cache, so this is
     * not directly the result of a broadcast right now.
     */
    virtual CARAPI IsInitialStickyBroadcast(
        /* [out] */ Boolean* isInitial);

    /**
     * For internal use, sets the hint about whether this BroadcastReceiver is
     * running in ordered mode.
     */
    virtual CARAPI SetOrderedHint(
        /* [in] */ Boolean isOrdered);

    /**
     * For internal use to set the result data that is active. @hide
     */
    virtual CARAPI SetPendingResult(
        /* [in] */ IPendingResult* result);

    /**
     * For internal use to set the result data that is active. @hide
     */
    virtual CARAPI GetPendingResult(
        /* [out] */ IPendingResult** pendingResult);

    /** @hide */
    virtual CARAPI GetSendingUserId(
        /* [out] */ Int32* userId);

    /**
     * Control inclusion of debugging help for mismatched
     * calls to {@link Context#registerReceiver(BroadcastReceiver, IntentFilter)
     * Context.registerReceiver()}.
     * If called with true, before given to registerReceiver(), then the
     * callstack of the following {@link Context#unregisterReceiver(BroadcastReceiver)
     * Context.unregisterReceiver()} call is retained, to be printed if a later
     * incorrect unregister call is made.  Note that doing this requires retaining
     * information about the BroadcastReceiver for the lifetime of the app,
     * resulting in a leak -- this should only be used for debugging.
     */
    virtual CARAPI SetDebugUnregister(
        /* [in] */ Boolean debug);

    /**
     * Return the last value given to {@link #setDebugUnregister}.
     */
    virtual CARAPI GetDebugUnregister(
        /* [out] */ Boolean* debugUnregister);

private:
    ECO_LOCAL CARAPI CheckSynchronousHint();

private:
    AutoPtr<IPendingResult> mPendingResult;
    Boolean mDebugUnregister;
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_BROADCASTRECEIVER_H__
