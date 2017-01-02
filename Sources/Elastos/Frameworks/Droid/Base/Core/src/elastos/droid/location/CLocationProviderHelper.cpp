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

#include "elastos/droid/location/CLocationProviderHelper.h"
#include "elastos/droid/location/LocationProvider.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_SINGLETON_IMPL(CLocationProviderHelper)

CAR_INTERFACE_IMPL(CLocationProviderHelper, Singleton, ILocationProviderHelper)

ECode CLocationProviderHelper::PropertiesMeetCriteria(
        /* [in] */ const String& name,
        /* [in] */ IProviderProperties* properties,
        /* [in] */ ICriteria* criteria,
        /* [out] */ Boolean* rst)
{
    *rst = LocationProvider::PropertiesMeetCriteria(name, properties, criteria);
    return NOERROR;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos
