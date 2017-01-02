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

#include "elastos/droid/internal/telephony/cdma/sms/CCdmaSmsAddressHelper.h"
#include "elastos/droid/internal/telephony/cdma/sms/CdmaSmsAddress.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CAR_INTERFACE_IMPL(CCdmaSmsAddressHelper, Singleton, ICdmaSmsAddressHelper)
CAR_SINGLETON_IMPL(CCdmaSmsAddressHelper)

ECode CCdmaSmsAddressHelper::Parse(
    /* [in] */ const String& address,
    /* [out] */ ICdmaSmsAddress** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaSmsAddress> csa = CdmaSmsAddress::Parse(address);
    *result = csa;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
