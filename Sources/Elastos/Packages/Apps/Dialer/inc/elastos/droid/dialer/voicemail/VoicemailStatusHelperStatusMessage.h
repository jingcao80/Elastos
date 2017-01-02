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

#ifndef __ELASTOS_DROID_DIALER_VOICEMAIL_VOICEMAILSTATUSHELPERSTATUSMESSAGE_H__
#define __ELASTOS_DROID_DIALER_VOICEMAIL_VOICEMAILSTATUSHELPERSTATUSMESSAGE_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Voicemail {

class VoicemailStatusHelperStatusMessage : public Object
{
public:
    VoicemailStatusHelperStatusMessage(
        /* [in] */ const String& sourcePackage,
        /* [in] */ Int32 callLogMessageId,
        /* [in] */ Int32 callDetailsMessageId,
        /* [in] */ Int32 actionMessageId,
        /* [in] */ IUri* actionUri);

    /** Whether this message should be shown in the call log page. */
    CARAPI_(Boolean) ShowInCallLog();

    /** Whether this message should be shown in the call details page. */
    CARAPI_(Boolean) ShowInCallDetails();

public:
/** Package of the source on behalf of which this message has to be shown.*/
    String mSourcePackage;
    /**
     * The string resource id of the status message that should be shown in the call log
     * page. Set to -1, if this message is not to be shown in call log.
     */
    Int32 mCallLogMessageId;
    /**
     * The string resource id of the status message that should be shown in the call details
     * page. Set to -1, if this message is not to be shown in call details page.
     */
    Int32 mCallDetailsMessageId;
    /** The string resource id of the action message that should be shown. */
    Int32 mActionMessageId;
    /** URI for the corrective action, where applicable. Null if no action URI is available. */
    AutoPtr<IUri> mActionUri;
};

} // Voicemail
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_VOICEMAIL_VOICEMAILSTATUSHELPERSTATUSMESSAGE_H__
