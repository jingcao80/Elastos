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

#include "elastos/droid/teleservice/phone/CarrierLogo.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Utility::CHashMap;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

AutoPtr<IMap> CarrierLogo::sLogoMap;

Int32 CarrierLogo::GetLogo(
    /* [in] */ const String& name)
{
    AutoPtr<IMap> map = GetLogoMap();
    AutoPtr<ICharSequence> cs = CoreUtils::Convert(name);
    AutoPtr<IInterface> obj;
    map->Get(TO_IINTERFACE(cs), (IInterface**)&obj);
    AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
    if (intObj != NULL) {
        Int32 res;
        intObj->GetValue(&res);
        return res;
    }

    return -1;
}

AutoPtr<IMap> CarrierLogo::GetLogoMap()
{
    if (sLogoMap == NULL) {
        CHashMap::New((IMap**)&sLogoMap);

        // TODO: Load up sLogoMap with known carriers, like:
        // sLogoMap.put("CarrierName",
        //    Integer.valueOf(R.drawable.mobile_logo_carriername));

        // TODO: ideally, read the mapping from a config file
        // rather than manually creating it here.
    }

    return sLogoMap;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos