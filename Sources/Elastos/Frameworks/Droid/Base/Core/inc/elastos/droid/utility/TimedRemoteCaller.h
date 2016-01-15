
#ifndef __ELASTOS_DROID_UTILITY_TIMED_REMOTED_CALLER_H__
#define __ELASTOS_DROID_UTILITY_TIMED_REMOTED_CALLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Object.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * This is a helper class for making an async one way call and
 * its async one way response response in a sync fashion within
 * a timeout. The key idea is to call the remote method with a
 * sequence number and a callback and then starting to wait for
 * the response. The remote method calls back with the result and
 * the sequence number. If the response comes within the timeout
 * and its sequence number is the one sent in the method invocation,
 * then the call succeeded. If the response does not come within
 * the timeout then the call failed. Older result received when
 * waiting for the result are ignored.
 * <p>
 * Typical usage is:
 * </p>
 * <p><pre><code>
 * public class MyMethodCaller extends TimeoutRemoteCallHelper<Object> {
 *     // The one way remote method to call.
 *     final IRemoteInterface mTarget;
 *
 *     // One way callback invoked when the remote method is done.
 *     final IRemoteCallback mCallback = new IRemoteCallback.Stub() {
 *         public void onCompleted(Object result, Int32 sequence) {
 *             onRemoteMethodResult(result, sequence);
 *         }
 *     };
 *
 *     public MyMethodCaller(IRemoteInterface target) {
 *         mTarget = target;
 *     }
 *
 *     public Object onCallMyMethod(Object arg) throws RemoteException {
 *         final Int32 sequence = onBeforeRemoteCall();
 *         mTarget.myMethod(arg, sequence);
 *         return getResultTimed(sequence);
 *     }
 * }
 * </code></pre></p>
 *
 * @param <T> The type of the expected result.
 *
 * @hide
 */
template<typename T>
class TimedRemoteCaller
    : public Object
{
public:
    TimedRemoteCaller(
        /* [in] */ Int64 callTimeoutMillis)
        : mCallTimeoutMillis(callTimeoutMillis)
        , mSequenceCounter(0)
        , mReceivedSequence(-1)
        , mAwaitedSequence(-1)
    {}

    Int32 OnBeforeRemoteCall()
    {
        AutoLock lock(mLock);
        mAwaitedSequence = mSequenceCounter++;
        return mAwaitedSequence;
    }

    CARAPI GetResultTimed(
        /* [in] */ Int32 sequence,
        /* [out] */ T** result)
    {
        VALIDATE_NOT_NULL(result)
        *result = NULL;

        synchronized(mLock) {
            Boolean success = WaitForResultTimedLocked(sequence);
            if (!success) {
                //throw new TimeoutException("No reponse for sequence: " + sequence);
                return E_TIMEOUT_EXCEPTION;
            }
            AutoPtr<T> r = mResult;
            mResult = NULL;
            *result = r;
            REFCOUNT_ADD(*result)
        }

        return NOERROR;
    }

    void OnRemoteMethodResult(
        /* [in] */ T* result,
        /* [in] */ Int32 sequence)
    {
        synchronized(mLock) {
            if (sequence == mAwaitedSequence) {
                mReceivedSequence = sequence;
                mResult = result;
                mLock.NotifyAll();
            }
        }
    }

private:
    Boolean WaitForResultTimedLocked(
        /* [in] */ Int32 sequence)
    {
        Int64 startMillis = SystemClock::GetUptimeMillis();
        while (true) {
            // try {
                if (mReceivedSequence == sequence) {
                    return TRUE;
                }
                Int64 elapsedMillis = SystemClock::GetUptimeMillis() - startMillis;
                Int64 waitMillis = mCallTimeoutMillis - elapsedMillis;
                if (waitMillis <= 0) {
                    return FALSE;
                }
                mLock.Wait(waitMillis);
            // } catch (InterruptedException ie) {
            //     /* ignore */
            // }
        }
    }

public:
    static const Int64 DEFAULT_CALL_TIMEOUT_MILLIS;// = 5000;

private:
    static const Int32 UNDEFINED_SEQUENCE;// = -1;

    Object mLock;// = new Object();

    Int64 mCallTimeoutMillis;

    Int32 mSequenceCounter;

    Int32 mReceivedSequence;// = UNDEFINED_SEQUENCE;

    Int32 mAwaitedSequence;// = UNDEFINED_SEQUENCE;

    AutoPtr<T> mResult;

};

template<typename T>
const Int64 TimedRemoteCaller<T>::DEFAULT_CALL_TIMEOUT_MILLIS = 5000;

template<typename T>
const Int32 TimedRemoteCaller<T>::UNDEFINED_SEQUENCE = -1;


}
}
}

#endif // __ELASTOS_DROID_UTILITY_TIMED_REMOTED_CALLER_H__
