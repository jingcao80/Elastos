#ifndef __ELASTOS_DROID_OS_LOOPER_H__
#define __ELASTOS_DROID_OS_LOOPER_H__

#include "Elastos.Droid.Os.h"
#include <pthread.h>
#include <elastos/core/Object.h>

using Elastos::Core::IThread;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Os::IMessageQueue;

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC Looper
    : public Object
    , public ILooper
{
public:
    CAR_INTERFACE_DECL()

    /** Initialize the current thread as a looper.
      * This gives you a chance to create handlers that then reference
      * this looper, before actually starting the loop. Be sure to call
      * {@link #loop()} after calling this method, and end it by calling
      * {@link #quit()}.
      */
    static CARAPI Prepare();

    /**
     * Initialize the current thread as a looper, marking it as an
     * application's main looper. The main looper for your application
     * is created by the Android environment, so you should never need
     * to call this function yourself.  See also: {@link #prepare()}
     */
    static CARAPI PrepareMainLooper();

    /** Returns the application's main looper, which lives in the main thread of the application.
     */
    static CARAPI_(AutoPtr<ILooper>) GetMainLooper();

    /**
     * Run the message queue in this thread. Be sure to call
     * {@link #quit()} to end the loop.
     */
    static CARAPI Loop();

    /**
     * Return the Looper object associated with the current thread.  Returns
     * null if the calling thread is not associated with a Looper.
     */
    static CARAPI_(AutoPtr<ILooper>) GetMyLooper();

    /**
     * Control logging of messages as they are processed by this Looper.  If
     * enabled, a log message will be written to <var>printer</var>
     * at the beginning and ending of each message dispatch, identifying the
     * target Handler and message contents.
     *
     * @param printer A Printer object that will receive log messages, or
     * null to disable message logging.
     */
    CARAPI SetMessageLogging(
        /* [in] */ IPrinter* printer);

    /**
     * Return the {@link MessageQueue} object associated with the current
     * thread.  This must be called from a thread running a Looper, or a
     * NullPointerException will be thrown.
     */
    static CARAPI_(AutoPtr<IMessageQueue>) GetMyQueue();

    CARAPI_(Boolean) IsCurrentThread();

    CARAPI IsCurrentThread(
        /* [out] */ Boolean* result);

    /**
     * Quits the looper.
     * <p>
     * Causes the {@link #loop} method to terminate without processing any
     * more messages in the message queue.
     * </p><p>
     * Any attempt to post messages to the queue after the looper is asked to quit will fail.
     * For example, the {@link Handler#sendMessage(Message)} method will return false.
     * </p><p class="note">
     * Using this method may be unsafe because some messages may not be delivered
     * before the looper terminates.  Consider using {@link #quitSafely} instead to ensure
     * that all pending work is completed in an orderly manner.
     * </p>
     *
     * @see #quitSafely
     */
    CARAPI Quit();

    /**
     * Quits the looper safely.
     * <p>
     * Causes the {@link #loop} method to terminate as soon as all remaining messages
     * in the message queue that are already due to be delivered have been handled.
     * However pending delayed messages with due times in the future will not be
     * delivered before the loop terminates.
     * </p><p>
     * Any attempt to post messages to the queue after the looper is asked to quit will fail.
     * For example, the {@link Handler#sendMessage(Message)} method will return false.
     * </p>
     */
    CARAPI QuitSafely();

    /**
     * Posts a synchronization barrier to the Looper's message queue.
     *
     * Message processing occurs as usual until the message queue encounters the
     * synchronization barrier that has been posted.  When the barrier is encountered,
     * later synchronous messages in the queue are stalled (prevented from being executed)
     * until the barrier is released by calling {@link #removeSyncBarrier} and specifying
     * the token that identifies the synchronization barrier.
     *
     * This method is used to immediately postpone execution of all subsequently posted
     * synchronous messages until a condition is met that releases the barrier.
     * Asynchronous messages (see {@link Message#isAsynchronous} are exempt from the barrier
     * and continue to be processed as usual.
     *
     * This call must be always matched by a call to {@link #removeSyncBarrier} with
     * the same token to ensure that the message queue resumes normal operation.
     * Otherwise the application will probably hang!
     *
     * @return A token that uniquely identifies the barrier.  This token must be
     * passed to {@link #removeSyncBarrier} to release the barrier.
     *
     * @hide
     */
    CARAPI PostSyncBarrier(
        /* [out] */ Int32* token);

    /**
     * Removes a synchronization barrier.
     *
     * @param token The synchronization barrier token that was returned by
     * {@link #postSyncBarrier}.
     *
     * @throws IllegalStateException if the barrier was not found.
     *
     * @hide
     */
    CARAPI RemoveSyncBarrier(
        /* [in] */ Int32 token);

    /**
     * Return the Thread associated with this Looper.
     */
    CARAPI GetThread(
        /* [out] */ IThread** thread);

    CARAPI_(AutoPtr<IThread>) GetThread();

    /** @hide */
    CARAPI_(AutoPtr<IMessageQueue>) GetQueue();

    CARAPI GetQueue(
        /* [out] */ IMessageQueue** queue);

    /**
     * Return whether this looper's thread is currently idle, waiting for new work
     * to do.  This is intrinsically racy, since its state can change before you get
     * the result back.
     * @hide
     */
    CARAPI_(Boolean) IsIdling();

    CARAPI IsIdling(
    /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* string);

private:
    ECO_LOCAL Looper(
        /* [in] */ Boolean quitAllowed);

    ECO_LOCAL static CARAPI Prepare(
        /* [in] */ Boolean quitAllowed);

    ECO_LOCAL static CARAPI_(void) InitTLS();

public:
    AutoPtr<IMessageQueue> mQueue;
    AutoPtr<IThread> mThread;

private:
    static const String TAG;

    static AutoPtr<ILooper> sMainLooper;  // guarded by Looper.class

    AutoPtr<IPrinter> mLogging;

    static Object sClassLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_LOOPER_H__
