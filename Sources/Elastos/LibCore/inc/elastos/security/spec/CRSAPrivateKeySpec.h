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

#ifndef __ELASTOS_SECURITY_SPEC_CRSAPRIVATEKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CRSAPRIVATEKEYSPEC_H__

#include "_Elastos_Security_Spec_CRSAPrivateKeySpec.h"
#include "RSAPrivateKeySpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAPrivateKeySpec), public RSAPrivateKeySpec
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */IBigInteger *modulus,
        /* [in] */ IBigInteger *privateExponent);
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAPRIVATEKEYSPEC_H__
