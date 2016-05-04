/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IVoLteServiceState;

using Elastos::Utility::IList;

/**
 * {@hide}
 */
public interface PhoneNotifier {

    CARAPI NotifyPhoneState(Phone sender);

    CARAPI NotifyServiceState(Phone sender);

    CARAPI NotifyCellLocation(Phone sender);

    CARAPI NotifySignalStrength(Phone sender);

    CARAPI NotifyMessageWaitingChanged(Phone sender);

    CARAPI NotifyCallForwardingChanged(Phone sender);

    /** TODO - reason should never be NULL */
    CARAPI NotifyDataConnection(Phone sender, String reason, String apnType,
            PhoneConstants.DataState state);

    CARAPI NotifyDataConnectionFailed(Phone sender, String reason, String apnType);

    CARAPI NotifyDataActivity(Phone sender);

    CARAPI NotifyOtaspChanged(Phone sender, Int32 otaspMode);

    CARAPI NotifyCellInfo(Phone sender, List<CellInfo> cellInfo);

    CARAPI NotifyPreciseCallState(Phone sender);

    CARAPI NotifyDisconnectCause(Int32 cause, Int32 preciseCause);

    CARAPI NotifyPreciseDataConnectionFailed(Phone sender, String reason, String apnType,
            String apn, String failCause);

    CARAPI NotifyDataConnectionRealTimeInfo(Phone sender, DataConnectionRealTimeInfo dcRtInfo);

    CARAPI NotifyVoLteServiceStateChanged(Phone sender, VoLteServiceState lteState);

    CARAPI NotifyOemHookRawEventForSubscriber(Int64 subId, Byte[] rawData);
}
