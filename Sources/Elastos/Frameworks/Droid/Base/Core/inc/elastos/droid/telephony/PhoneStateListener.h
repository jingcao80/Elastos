
#ifndef __ELASTOS_DROID_TELEPHONY_PHONESTATELISTENER_H__
#define __ELASTOS_DROID_TELEPHONY_PHONESTATELISTENER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Internal::Telephony::IIPhoneStateListener;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telephony {

class ECO_PUBLIC PhoneStateListener
    : public Object
    , public IPhoneStateListener
{
    friend class CTelephonyManager;

public:
    class PhoneStateListenerCallback
        : public Object
        , public IIPhoneStateListener
    {
        CAR_INTERFACE_DECL();

        PhoneStateListenerCallback(
            /* [in] */ PhoneStateListener* host);

        CARAPI OnServiceStateChanged(
            /* [in] */ IServiceState* serviceState);

        CARAPI OnSignalStrengthChanged(
            /* [in] */ Int32 asu);

        CARAPI OnMessageWaitingIndicatorChanged(
            /* [in] */ Boolean mwi);

        CARAPI OnCallForwardingIndicatorChanged(
            /* [in] */ Boolean cfi);

        CARAPI OnCellLocationChanged(
            /* [in] */ IBundle* location);

        CARAPI OnCallStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ const String& incomingNumber);

        CARAPI OnDataConnectionStateChanged(
            /* [in] */ Int32 state,
            /* [in] */ Int32 networkType);

        CARAPI OnDataActivity(
            /* [in] */ Int32 direction);

        CARAPI OnSignalStrengthsChanged(
            /* [in] */ ISignalStrength* signalStrength);

        CARAPI OnOtaspChanged(
            /* [in] */ Int32 otaspMode);

        CARAPI OnCellInfoChanged(
            /* [in] */ IList* cellInfo);

        CARAPI OnPreciseCallStateChanged(
            /* [in] */ IPreciseCallState* callState);

        CARAPI OnPreciseDataConnectionStateChanged(
            /* [in] */ IPreciseDataConnectionState* dataConnectionState);

        CARAPI OnDataConnectionRealTimeInfoChanged(
            /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo);

        CARAPI OnVoLteServiceStateChanged(
            /* [in] */ IVoLteServiceState* stateInfo);

        CARAPI OnOemHookRawEvent(
            /* [in] */ ArrayOf<Byte>* rawData);

    private:
        PhoneStateListener* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PhoneStateListener::MyHandler")

        MyHandler(
            /* [in] */ PhoneStateListener* host,
            /* [in] */ ILooper* looper)
            : Handler(looper)
            , mHost(host)
        {}

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PhoneStateListener* mHost;
    };

public:
    PhoneStateListener();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILooper* looper);

    CARAPI constructor(
        /* [in] */ Int64 subId);

    CARAPI constructor(
        /* [in] */ Int64 subId,
        /* [in] */ ILooper* looper);

    CARAPI OnServiceStateChanged(
        /* [in] */ IServiceState* serviceState);

    CARAPI OnSignalStrengthChanged(
        /* [in] */ Int32 asu);

    CARAPI OnMessageWaitingIndicatorChanged(
        /* [in] */ Boolean mwi);

    CARAPI OnCallForwardingIndicatorChanged(
        /* [in] */ Boolean cfi);

    CARAPI OnCellLocationChanged(
        /* [in] */ ICellLocation* location);

    CARAPI OnCallStateChanged(
        /* [in] */ Int32 state,
        /* [in] */ const String& incomingNumber);

    CARAPI OnDataConnectionStateChanged(
        /* [in] */ Int32 state);

    CARAPI OnDataConnectionStateChanged(
        /* [in] */ Int32 state,
        /* [in] */ Int32 networkType);

    CARAPI OnDataActivity(
        /* [in] */ Int32 direction);

    CARAPI OnSignalStrengthsChanged(
        /* [in] */ ISignalStrength* signalStrength);

    CARAPI OnOtaspChanged(
        /* [in] */ Int32 otaspMode);

    CARAPI OnCellInfoChanged(
        /* [in] */ IList* cellInfo);

    CARAPI OnPreciseCallStateChanged(
        /* [in] */ IPreciseCallState* callState);

    CARAPI OnPreciseDataConnectionStateChanged(
        /* [in] */ IPreciseDataConnectionState* dataConnectionState);

    CARAPI OnDataConnectionRealTimeInfoChanged(
        /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo);

    CARAPI OnVoLteServiceStateChanged(
        /* [in] */ IVoLteServiceState* stateInfo);

    CARAPI OnOemHookRawEvent(
        /* [in] */ ArrayOf<Byte>* rawData);

protected:
     /*
     * Subscription used to listen to the phone state changes
     * @hide
     */
    /** @hide */
    Int64 mSubId;

private:
    static const String TAG;;
    static const Boolean DBG; // STOPSHIP if true

    AutoPtr<MyHandler> mHandler;
    AutoPtr<IIPhoneStateListener> callback;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_PHONESTATELISTENER_H__
