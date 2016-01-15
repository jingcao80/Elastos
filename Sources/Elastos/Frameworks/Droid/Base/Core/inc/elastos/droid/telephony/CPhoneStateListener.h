
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENER_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENER_H__

#include "_Elastos_Droid_Telephony_CPhoneStateListener.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IIPhoneStateListener;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneStateListener)
{
public:

    class CPhoneStateListenerCallback
        : public ElRefBase
        , public IIPhoneStateListener
    {
        CAR_INTERFACE_DECL();

        CPhoneStateListenerCallback(
            /* [in] */ CPhoneStateListener* host);

        CARAPI OnServiceStateChanged(
            /* [in] */ IServiceState* serviceState);

        CARAPI OnSignalStrengthChanged(
            /* [in] */ Int32 asu);

        CARAPI OnMessageWaitingIndicatorChanged(
            /* [in] */ Boolean mwi);

        CARAPI OnCallForwardingIndicatorChanged(
            /* [in] */ Boolean cfi);

        // we use bundle here instead of CellLocation so it can get the right subclass
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
            /* [in] */ ArrayOf<ICellInfo*>* cellInfo);

    private:
        CPhoneStateListener* mHost;
    };

    class MyHandler
        : public HandlerBase
    {
    public:

        MyHandler(
            /* [in] */ CPhoneStateListener* host);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CPhoneStateListener* mHost;
    };

    CARAPI SetContext(
        /* [in] */ IContext* context);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI SetUid(
        /* [in] */ Int32 uid);

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
        /* [in] */ ArrayOf<ICellInfo *>* cellInfo);

    CARAPI constructor();

    AutoPtr<IIPhoneStateListener> callback;
    AutoPtr<MyHandler> mHandler;
    AutoPtr<IContext> mContext;
    String mPackageName;
    Int32 mUid;

private:

    // BEGIN privacy-added
    static const String TAG/* = "PhoneStateListener"*/;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CPHONESTATELISTENER_H__
