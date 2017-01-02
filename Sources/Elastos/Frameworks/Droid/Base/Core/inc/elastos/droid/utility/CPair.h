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

#ifndef __ELASTOS_DROID_UTILITY_CPAIR_H__
#define __ELASTOS_DROID_UTILITY_CPAIR_H__

#include "_Elastos_Droid_Utility_CPair.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CPair)
    , public Object
    , public IPair
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetFirst(
        /* [out] */ IInterface **first);

    CARAPI GetSecond(
        /* [out] */ IInterface **second);

    CARAPI Equals(
        /* [in] */ IInterface *o,
        /* [out] */ Boolean *isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ IInterface *first,
        /* [in] */ IInterface *second);

    static CARAPI Create(
        /* [in] */ IInterface *A,
        /* [in] */ IInterface *B,
        /* [out] */ IPair **pair);

public:
    AutoPtr<IInterface> mFirst;
    AutoPtr<IInterface> mSecond;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CPAIR_H__
