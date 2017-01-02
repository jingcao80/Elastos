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

#ifndef __ELASTOS_SECURITY_SPEC_CECPUBLICKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CECPUBLICKEYSPEC_H__

#include "_Elastos_Security_Spec_CECPublicKeySpec.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECPublicKeySpec)
    , public Object
    , public IECPublicKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetParams(
        /* [out] */ IECParameterSpec **params);

    CARAPI GetW(
        /* [out] */ IECPoint **w);

    CARAPI constructor(
        /* [in] */ IECPoint *w,
        /* [in] */ IECParameterSpec *params);

private:
    // The public point
    AutoPtr<IECPoint> mW;
    // The associated elliptic curve domain parameters
    AutoPtr<IECParameterSpec> mParams;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECPUBLICKEYSPEC_H__
