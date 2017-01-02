//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
using Elastos::Droid::Os::IBinder;
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
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IPhoneStateListener* host);

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

        CARAPI ToString(
            /* [out] */ String* str);

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
    static const Boolean DBG; // STOPSHIP if true

private:
    static const String TAG;;

    AutoPtr<MyHandler> mHandler;
    AutoPtr<IIPhoneStateListener> mCallback;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_PHONESTATELISTENER_H__
