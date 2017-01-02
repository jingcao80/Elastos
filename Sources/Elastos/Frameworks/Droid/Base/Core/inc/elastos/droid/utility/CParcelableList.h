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

#ifndef __ELASTOS_DROID_UTILITY_CPARCELABLELIST_H__
#define __ELASTOS_DROID_UTILITY_CPARCELABLELIST_H__

#include "_Elastos_Droid_Utility_CParcelableList.h"
#include <elastos/utility/ArrayList.h>

using Elastos::Utility::ArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CParcelableList)
    , public ArrayList
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    using ArrayList::constructor;

    CARAPI constructor(
        /* [in] */ IList* list);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI_(String) GetClassName() { return String("CParcelableList"); }
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CPARCELABLELIST_H__
