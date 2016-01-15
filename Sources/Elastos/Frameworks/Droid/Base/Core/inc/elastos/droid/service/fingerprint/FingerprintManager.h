
#ifndef __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTMANAGER_H__
#define __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Service::Fingerprint::IFingerprintManagerReceiver;
using Elastos::Droid::Service::Fingerprint::IIFingerprintService;
using Elastos::Droid::Service::Fingerprint::IIFingerprintServiceReceiver;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

class FingerprintManager
    : public Object
    , public IFingerprintManager
{
private:
    class MHandler
        : public Handler
    {
        friend class FingerprintManager;
    public:
        MHandler(
            /* [in] */ FingerprintManager* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        FingerprintManager* mHost;
    };

    class MServiceReceiver
        : public Object
        , public IIFingerprintServiceReceiver
        , public IBinder
    {
        friend class FingerprintManager;
    public:
        CAR_INTERFACE_DECL()

        MServiceReceiver(
            /* [in] */ FingerprintManager* host);

        CARAPI OnEnrollResult(
            /* [in] */ Int32 fingerprintId,
            /* [in] */ Int32 remaining);

        CARAPI OnAcquired(
            /* [in] */ Int32 acquireInfo);

        CARAPI OnProcessed(
            /* [in] */ Int32 fingerprintId);

        CARAPI OnError(
            /* [in] */ Int32 error);

        CARAPI OnRemoved(
            /* [in] */ Int32 fingerprintId);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        FingerprintManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    FingerprintManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIFingerprintService* service);

    /**
     * Determine whether the user has at least one fingerprint enrolled and enabled.
     *
     * @return true if at least one is enrolled and enabled
     */
    CARAPI EnrolledAndEnabled(
        /* [out] */ Boolean* result);

    /**
     * Start the enrollment process.  Timeout dictates how long to wait for the user to
     * enroll a fingerprint.
     *
     * @param timeout
     */
    CARAPI Enroll(
        /* [in] */ Int64 timeout);

    /**
     * Remove the given fingerprintId from the system.  FingerprintId of 0 has special meaning
     * which is to delete all fingerprint data for the current user. Use with caution.
     * @param fingerprintId
     */
    CARAPI Remove(
        /* [in] */ Int32 fingerprintId);

    /**
     * Starts listening for fingerprint events.  When a finger is scanned or recognized, the
     * client will be notified via the callback.
     */
    CARAPI StartListening(
        /* [in] */ IFingerprintManagerReceiver* receiver);

    /**
     * Stops the client from listening to fingerprint events.
     */
    CARAPI StopListening();

    CARAPI EnrollCancel();

private:
    CARAPI GetCurrentUserId(
        /* [out] */ Int32* userId);

    CARAPI SendError(
        /* [in] */ Int32 msg,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

private:
    const static String TAG;
    const static Boolean DEBUG;
    const static Int32 MSG_ENROLL_RESULT = 100;
    const static Int32 MSG_ACQUIRED = 101;
    const static Int32 MSG_PROCESSED = 102;
    const static Int32 MSG_ERROR = 103;
    const static Int32 MSG_REMOVED = 104;

    AutoPtr<IIFingerprintService> mService;
    AutoPtr<IFingerprintManagerReceiver> mClientReceiver;
    AutoPtr<IContext> mContext;
    AutoPtr<IBinder> mToken;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IIFingerprintServiceReceiver> mServiceReceiver;
};

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTMANAGER_H__
