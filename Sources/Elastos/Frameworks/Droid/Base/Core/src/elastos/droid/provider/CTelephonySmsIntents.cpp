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

#include "elastos/droid/provider/CTelephonySmsIntents.h"
#include "elastos/droid/provider/Telephony.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL(CTelephonySmsIntents, Singleton, ITelephonySmsIntents);
CAR_SINGLETON_IMPL(CTelephonySmsIntents);

ECode CTelephonySmsIntents::GetMessagesFromIntent(
    /* [in] */ IIntent* intent,
    /* [out, callee] */ ArrayOf<ISmsMessage*>** result)
{
    return Telephony::Sms::Intents::GetMessagesFromIntent(intent, result);
}

ECode CTelephonySmsIntents::GetNormalizedAddressesFromPdus(
    /* [in] */ ArrayOf<ArrayOf<Byte>*>* pdus,
    /* [in] */ const String& format,
    /* [out] */ IList** result)
{
    return Telephony::Sms::Intents::GetNormalizedAddressesFromPdus(pdus, format, result);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
