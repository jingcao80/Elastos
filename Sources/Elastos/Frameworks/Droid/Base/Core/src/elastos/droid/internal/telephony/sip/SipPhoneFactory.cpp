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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/CSipPhone.h"
#include "elastos/droid/internal/telephony/sip/SipPhoneFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                           SipPhoneFactory
//=====================================================================

AutoPtr<ISipPhone> SipPhoneFactory::MakePhone(
    /* [in] */ const String& sipUri,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* phoneNotifier)
{
    // try {
// TODO: Need SipProfile
    // AutoPtr<ISipProfile> profile = new SipProfile.Builder(sipUri).build();
    AutoPtr<ISipPhone> sipPhone;
    CSipPhone::New(context, phoneNotifier, NULL/*TODO: profile*/, (ISipPhone**)&sipPhone);
    // } catch (ParseException e) {
    //     Rlog.w("SipPhoneFactory", "makePhone", e);
    //     return null;
    // }
    return sipPhone;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
