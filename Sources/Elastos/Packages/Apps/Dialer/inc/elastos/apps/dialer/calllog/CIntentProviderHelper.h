
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CINTENTPROVIDERHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CINTENTPROVIDERHELPER_H__

#include "_Elastos_Apps_Dialer_CallLog_CIntentProviderHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Telecomm.h"

using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CarClass(CIntentProviderHelper)
    , public Singleton
    , public IIntentProviderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetReturnCallIntentProvider(
        /* [in] */ const String& number,
        /* [out] */ IIntentProvider** provider);

    CARAPI GetReturnCallIntentProvider(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntentProvider** provider);

    CARAPI GetReturnVideoCallIntentProvider(
        /* [in] */ const String& number,
        /* [out] */ IIntentProvider** provider);

    CARAPI GetReturnVideoCallIntentProvider(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntentProvider** provider);

    CARAPI GetPlayVoicemailIntentProvider(
        /* [in] */ Int64 rowId,
        /* [in] */ const String& voicemailUri,
        /* [out] */ IIntentProvider** provider);

    /**
     * Retrieves the call details intent provider for an entry in the call log.
     *
     * @param id The call ID of the first call in the call group.
     * @param extraIds The call ID of the other calls grouped together with the call.
     * @param voicemailUri If call log entry is for a voicemail, the voicemail URI.
     * @return The call details intent provider.
     */
   CARAPI GetCallDetailIntentProvider(
        /* [in] */ Int64 id,
        /* [in] */ ArrayOf<Int64>* extraIds,
        /* [in] */ const String& voicemailUri,
        /* [out] */ IIntentProvider** provider);
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CINTENTPROVIDERHELPER_H__
