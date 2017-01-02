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

#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperStatusMessage.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Voicemail {

VoicemailStatusHelperStatusMessage::VoicemailStatusHelperStatusMessage(
    /* [in] */ const String& sourcePackage,
    /* [in] */ Int32 callLogMessageId,
    /* [in] */ Int32 callDetailsMessageId,
    /* [in] */ Int32 actionMessageId,
    /* [in] */ IUri* actionUri)
    : mSourcePackage(sourcePackage)
    , mCallLogMessageId(callLogMessageId)
    , mCallDetailsMessageId(callDetailsMessageId)
    , mActionMessageId(actionMessageId)
    , mActionUri(actionUri)
{}

Boolean VoicemailStatusHelperStatusMessage::ShowInCallLog()
{
    return mCallLogMessageId != -1;
}

Boolean VoicemailStatusHelperStatusMessage::ShowInCallDetails()
{
    return mCallDetailsMessageId != -1;
}

} // Voicemail
} // Dialer
} // Droid
} // Elastos
