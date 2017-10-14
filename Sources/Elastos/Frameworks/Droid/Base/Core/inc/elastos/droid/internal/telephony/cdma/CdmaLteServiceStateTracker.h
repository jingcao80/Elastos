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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMALTESERVICESTATETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMALTESERVICESTATETRACKER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/internal/telephony/cdma/CdmaServiceStateTracker.h"

// import android.content.Intent;
// import com.android.internal.telephony.TelephonyProperties;
// import com.android.internal.telephony.MccTable;
// import com.android.internal.telephony.EventLogTags;
// import com.android.internal.telephony.uicc.RuimRecords;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;
// import com.android.internal.telephony.TelephonyIntents;
// import com.android.internal.telephony.Phone;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.PhoneFactory;
// import com.android.internal.telephony.PhoneConstants;
// import android.telephony.CellInfo;
// import android.telephony.CellSignalStrengthLte;
// import android.telephony.SignalStrength;
// import android.telephony.ServiceState;
// import android.telephony.cdma.CdmaCellLocation;
// import android.telephony.TelephonyManager;
// import android.text.TextUtils;
// import android.os.UserHandle;
// import android.os.SystemClock;
// import android.os.SystemProperties;
// import android.telephony.Rlog;
// import android.util.EventLog;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.dataconnection.DcTrackerBase;
// import com.android.internal.telephony.ProxyController;
// import android.telephony.SubscriptionManager;
// import com.android.internal.telephony.uicc.UiccController;
// import java.util.ArrayList;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Telephony::ICellInfoLte;
using Elastos::Droid::Telephony::ICellIdentityLte;
using Elastos::Droid::Internal::Telephony::DataConnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

class CdmaLteServiceStateTracker
    : public CdmaServiceStateTracker
    , public ICdmaLteServiceStateTracker
{
    friend class CDMALTEPhone;
public:
    TO_STRING_IMPL("CdmaLteServiceStateTracker")

    CAR_INTERFACE_DECL();

    CdmaLteServiceStateTracker();

    CARAPI constructor(
        /* [in] */ ICDMALTEPhone* phone);

    // @Override
    CARAPI Dispose();

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    // @Override
    CARAPI PollState();

    // @Override
    CARAPI IsConcurrentVoiceAndDataAllowed(
        /* [out] */ Boolean* resutl);

    /**
      * TODO: Remove when we get new ril/modem for Galaxy Nexus.
      *
      * @return all available cell information, the returned List maybe empty but never null.
      */
    // @Override // ICellInfo
    CARAPI GetAllCellInfo(
        /* [out] */ IList** result);

    /**
      * Clean up existing voice and data connection then turn off radio power.
      *
      * Hang up the existing voice calls to decrease call drop rate.
      */
    // @Override
    CARAPI PowerOffRadioSafely(
        /* [in] */ IDcTrackerBase* dcTracker);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    /**
      * Handle the result of one of the pollState()-related requests
      */
    // @Override
    CARAPI HandlePollStateResultMessage(
        /* [in] */ Int32 what,
        /* [in] */ AsyncResult* ar);

    // @Override
    CARAPI PollStateDone();

    // @Override
    CARAPI OnSignalStrengthResult(
        /* [in] */ AsyncResult* ar,
        /* [in] */ Boolean isGsm,
        /* [out] */ Boolean* resutl);

    // @Override
    CARAPI_(AutoPtr<IUiccCardApplication>) GetUiccCardApplication();

    virtual CARAPI_(void) UpdateCdmaSubscription();

    // @Override
    CARAPI UpdatePhoneObject();

    // @Override
    CARAPI Log(
        /* [in] */ const String& s);

    // @Override
    CARAPI Loge(
        /* [in] */ const String& s);

private:
    /**
      * Check whether the specified SID and NID pair appears in the HOME SID/NID list
      * read from NV or SIM.
      *
      * @return true if provided sid/nid pair belongs to operator's home network.
      */
    CARAPI_(Boolean) IsInHomeSidNid(
        /* [in] */ Int32 sid,
        /* [in] */ Int32 nid);

protected:
    Int32 mNewRilRadioTechnology;

private:
    AutoPtr<ICDMALTEPhone> mCdmaLtePhone;
    AutoPtr<ICellInfoLte> mCellInfoLte;
    static const Int32 EVENT_ALL_DATA_DISCONNECTED = 1001;
    AutoPtr<ICellIdentityLte> mNewCellIdentityLte;
    AutoPtr<ICellIdentityLte> mLasteCellIdentityLte;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMALTESERVICESTATETRACKER_H__
