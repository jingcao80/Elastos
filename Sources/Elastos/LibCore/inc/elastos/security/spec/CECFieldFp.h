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

#ifndef __ELASTOS_SECURITY_SPEC_CECFIELDFP_H__
#define __ELASTOS_SECURITY_SPEC_CECFIELDFP_H__

#include "_Elastos_Security_Spec_CECFieldFp.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECFieldFp)
    , public Object
    , public IECFieldFp
    , public IECField
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetFieldSize(
        /* [out] */ Int32 *size);

    CARAPI GetP(
        /* [out] */ IBigInteger **p);

    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ IBigInteger *p);

private:
    // Prime
    AutoPtr<IBigInteger> mP;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECFIELDFP_H__
