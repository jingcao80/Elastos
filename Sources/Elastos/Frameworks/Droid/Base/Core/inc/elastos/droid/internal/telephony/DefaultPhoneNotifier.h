
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DEFAULTPHONENOTIFIER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DEFAULTPHONENOTIFIER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IPreciseCallState;
using Elastos::Droid::Telephony::IVoLteServiceState;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * broadcast intents
 */
class ECO_PUBLIC DefaultPhoneNotifier
    : public Object
    , public IPhoneNotifier
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI NotifyPhoneState(
        /* [in] */ IPhone* sender);

    // @Override
    CARAPI NotifyServiceState(
        /* [in] */ IPhone* sender);

    // @Override
    CARAPI NotifySignalStrength(
        /* [in] */ IPhone* sender);

    // @Override
    CARAPI NotifyMessageWaitingChanged(
        /* [in] */ IPhone* sender);

    // @Override
    CARAPI NotifyCallForwardingChanged(
        /* [in] */ IPhone* sender);

    // @Override
    CARAPI NotifyDataActivity(
        /* [in] */ IPhone* sender);

    // @Override
    CARAPI NotifyDataConnection(
        /* [in] */ IPhone* sender,
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType,
        /* [in] */ PhoneConstantsDataState state);

    // @Override
    CARAPI NotifyDataConnectionFailed(
        /* [in] */ IPhone* sender,
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType);

    // @Override
    CARAPI NotifyCellLocation(
        /* [in] */ IPhone* sender);

    // @Override
    CARAPI NotifyCellInfo(
        /* [in] */ IPhone* sender,
        /* [in] */ IList* cellInfo);

    // @Override
    CARAPI NotifyDataConnectionRealTimeInfo(
        /* [in] */ IPhone* sender,
        /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo);

    // @Override
    CARAPI NotifyOtaspChanged(
        /* [in] */ IPhone* sender,
        /* [in] */ Int32 otaspMode);

    CARAPI NotifyPreciseCallState(
        /* [in] */ IPhone* sender);

    CARAPI NotifyDisconnectCause(
        /* [in] */ Int32 cause,
        /* [in] */ Int32 preciseCause);

    CARAPI NotifyPreciseDataConnectionFailed(
        /* [in] */ IPhone* sender,
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType,
        /* [in] */ const String& apn,
        /* [in] */ const String& failCause);

    // @Override
    CARAPI NotifyVoLteServiceStateChanged(
        /* [in] */ IPhone* sender,
        /* [in] */ IVoLteServiceState* lteState);

    // @Override
    CARAPI NotifyOemHookRawEventForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ ArrayOf<Byte>* rawData);

    /**
     * Convert the {@link PhoneConstants.State} enum into the TelephonyManager.CALL_STATE_*
     * constants for the public API.
     */
    static CARAPI_(Int32) ConvertCallState1(
        /* [in] */ PhoneConstantsState state);

    /**
     * Convert the TelephonyManager.CALL_STATE_* constants into the
     * {@link PhoneConstants.State} enum for the public API.
     */
    static CARAPI_(PhoneConstantsState) ConvertCallState2(
        /* [in] */ Int32 state);

    /**
     * Convert the {@link PhoneConstants.DataState} enum into the TelephonyManager.DATA_* constants
     * for the public API.
     */
    static CARAPI_(Int32) ConvertDataState1(
        /* [in] */ PhoneConstantsDataState state);

    /**
     * Convert the TelephonyManager.DATA_* constants into {@link PhoneConstants.DataState} enum
     * for the public API.
     */
    static CARAPI_(PhoneConstantsDataState) ConvertDataState2(
        /* [in] */ Int32 state);

    /**
     * Convert the {@link Phone.DataActivityState} enum into the TelephonyManager.DATA_* constants
     * for the public API.
     */
    static CARAPI_(Int32) ConvertDataActivityState1(
        /* [in] */ IPhoneDataActivityState state);

    /**
     * Convert the TelephonyManager.DATA_* constants into the {@link Phone.DataActivityState} enum
     * for the public API.
     */
    static CARAPI_(IPhoneDataActivityState) ConvertDataActivityState2(
        /* [in] */ Int32 state);

    /**
     * Convert the {@link State} enum into the PreciseCallState.PRECISE_CALL_STATE_* constants
     * for the public API.
     */
    static CARAPI_(Int32) ConvertPreciseCallState1(
        /* [in] */ ICallState state);

    /**
     * Convert the Call.State.* constants into the {@link State} enum
     * for the public API.
     */
    static CARAPI_(ICallState) ConvertPreciseCallState2(
        /* [in] */ Int32 state);

protected:
    /*package*/
    DefaultPhoneNotifier();

private:
    CARAPI_(void) DoNotifyDataConnection(
        /* [in] */ IPhone* sender,
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType,
        /* [in] */ PhoneConstantsDataState state);

    CARAPI_(void) Log(
        /* [in] */ const String& s);

protected:
    AutoPtr<IITelephonyRegistry> mRegistry;

private:
    static const String TAG;
    static const Boolean DBG; // STOPSHIP if true
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DEFAULTPHONENOTIFIER_H__
