
#ifndef __ELASTOS_DROID_OS_ASYNCRESULT_H__
#define __ELASTOS_DROID_OS_ASYNCRESULT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Os::IIResultReceiver;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Generic interface for receiving a callback result from someone.  Use this
 * by creating a subclass and implement {@link #onReceiveResult}, which you can
 * then pass to others and send through IPC, and receive results they
 * supply with {@link #send}.
 *
 * <p>Note: the implementation underneath is just a simple wrapper around
 * a {@link Binder} that is used to perform the communication.  This means
 * semantically you should treat it as such: this class does not impact process
 * lifecycle management (you must be using some higher-level component to tell
 * the system that your process needs to continue running), the connection will
 * break if your process goes away for any reason, etc.</p>
 */
class ResultReceiver
    : public Object
    , public IResultReceiver
    , public IParcelable
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        Int32 mResultCode;
        AutoPtr<IBundle> mResultData;

        MyRunnable(
            /* [in] */ Int32 resultCode,
            /* [in] */ IBundle* resultData,
            /* [in] */ IWeakReference* wr);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class MyResultReceiver
        : public Object
        , public IIResultReceiver
    {
    public:
        CAR_INTERFACE_DECL()

        MyResultReceiver(
            /* [in] */ IWeakReference* wr);

        CARAPI Send(
            /* [in] */ Int32 resultCode,
            /* [in] */ IBundle* resultData);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CAR_INTERFACE_DECL()

    ResultReceiver();

    CARAPI constructor();

    /**
     * Create a new ResultReceive to receive results.  Your
     * {@link #onReceiveResult} method will be called from the thread running
     * <var>handler</var> if given, or from an arbitrary thread if null.
     */
    CARAPI constructor(
        /* [in] */ IHandler* handler);

    /**
     * Deliver a result to this receiver.  Will call {@link #onReceiveResult},
     * always asynchronously if the receiver has supplied a Handler in which
     * to dispatch the result.
     * @param resultCode Arbitrary result code to deliver, as defined by you.
     * @param resultData Any additional data provided by you.
     */
    CARAPI Send(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* resultData);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

protected:
    /**
     * Override to receive results delivered to this object.
     *
     * @param resultCode Arbitrary result code delivered by the sender, as
     * defined by the sender.
     * @param resultData Any additional data provided by the sender.
     */
    void OnReceiveResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* resultData) {
    }

private:
    Boolean mLocal;
    AutoPtr<IHandler> mHandler;

    AutoPtr<IIResultReceiver> mReceiver;

    friend class MyRunnable;
    friend class MyResultReceiver;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_ASYNCRESULT_H__
