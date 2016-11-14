
#ifndef __ELASTOS_DROID_OS_REMOTECALLBACK_H__
#define __ELASTOS_DROID_OS_REMOTECALLBACK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * TODO: Make this a public API?  Let's see how it goes with a few use
 * cases first.
 * @hide
 */
class ECO_PUBLIC RemoteCallback
    : public Object
    , public IParcelable
    , public IRemoteCallback
{
public:
    class ECO_LOCAL LocalCallback
        : public Object
        , public IIRemoteCallback
        , public IBinder
    {
    public:
        TO_STRING_IMPL("RemoteCallback::LocalCallback")

        CAR_INTERFACE_DECL()

        LocalCallback();

        CARAPI constructor(
            /* [in] */ IWeakReference* wr);

        CARAPI SendResult(
            /* [in] */ IBundle* bundle);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

private:
    class ECO_LOCAL DeliverResult
        : public Runnable
    {
    public:
        TO_STRING_IMPL("RemoteCallback::DeliverResult")

        DeliverResult(
            /* [in] */ IBundle* result,
            /* [in] */ IWeakReference* wr);

        CARAPI Run();
    private:
        AutoPtr<IBundle> mResult;
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    TO_STRING_IMPL("RemoteCallback")

    CAR_INTERFACE_DECL()

    RemoteCallback();

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI constructor(
        /* [in] */ IIRemoteCallback* target);

    CARAPI SendResult(
        /* [in] */ IBundle* bundle);

    CARAPI_(void) OnResult(
        /* [in] */ IBundle* bundle);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IIRemoteCallback> mTarget;
};


class RemoteCallbackProxy
    : public RemoteCallback
{
public:
    TO_STRING_IMPL("RemoteCallbackProxy")

    CARAPI constructor(
        /* [in] */ IIRemoteCallback* target);

    CARAPI_(void) OnResult(
        /* [in] */ IBundle* bundle);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_REMOTECALLBACK_H__
