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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CINTENTPROVIDERHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CINTENTPROVIDERHELPER_H__

#include "_Elastos_Droid_Dialer_CallLog_CIntentProviderHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Telecom.h"

using Elastos::Droid::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CINTENTPROVIDERHELPER_H__
