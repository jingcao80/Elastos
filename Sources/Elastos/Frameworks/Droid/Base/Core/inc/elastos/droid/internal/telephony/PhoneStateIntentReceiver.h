
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESTATEINTENTRECEIVER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESTATEINTENTRECEIVER_H__

#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class PhoneStateIntentReceiver
    : public BroadcastReceiver
    , public IPhoneStateIntentReceiver
{
public:
    CAR_INTERFACE_DECL();

    PhoneStateIntentReceiver();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    CARAPI SetContext(
        /* [in] */ IContext* c);

    CARAPI SetTarget(
        /* [in] */ IHandler* h);

    CARAPI GetPhoneState(
        /* [out] */ PhoneConstantsState* result);

    CARAPI GetServiceState(
        /* [out] */ IServiceState** result);

    /**
     * Returns current signal strength in as an asu 0..31
     *
     * Throws RuntimeException if client has not called NotifySignalStrength()
     */
    CARAPI GetSignalStrengthLevelAsu(
        /* [out] */ Int32* result);

    /**
     * Return current signal strength in "dBm", ranging from -113 - -51dBm
     * or -1 if unknown
     *
     * @return signal strength in dBm, -1 if not yet updated
     * Throws RuntimeException if client has not called NotifySignalStrength()
     */
    CARAPI GetSignalStrengthDbm(
        /* [out] */ Int32* result);

    CARAPI NotifyPhoneCallState(
        /* [in] */ Int32 eventWhat);

    CARAPI GetNotifyPhoneCallState(
        /* [out] */ Boolean* result);

    CARAPI NotifyServiceState(
        /* [in] */ Int32 eventWhat);

    CARAPI GetNotifyServiceState(
        /* [out] */ Boolean* result);

    CARAPI NotifySignalStrength (
        /* [in] */ Int32 eventWhat);

    CARAPI GetNotifySignalStrength(
        /* [out] */ Boolean* result);

    CARAPI RegisterIntent();

    CARAPI UnregisterIntent();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

public:
    PhoneConstantsState mPhoneState;
    AutoPtr<IServiceState> mServiceState;
    AutoPtr<ISignalStrength> mSignalStrength;

private:
    static const String LOGTAG;
    static const Boolean DBG;

    static const Int32 NOTIF_PHONE;
    static const Int32 NOTIF_SERVICE;
    static const Int32 NOTIF_SIGNAL;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mTarget;
    AutoPtr<IIntentFilter> mFilter;
    Int32 mWants;
    Int32 mPhoneStateEventWhat;
    Int32 mServiceStateEventWhat;
    Int32 mAsuEventWhat;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_PHONESTATEINTENTRECEIVER_H__
