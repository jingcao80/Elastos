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

#ifndef __ELASTOS_SECURITY_SPEC_CECPOINT_H__
#define __ELASTOS_SECURITY_SPEC_CECPOINT_H__

#include "_Elastos_Security_Spec_CECPoint.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECPoint)
    , public Object
    , public IECPoint
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(const AutoPtr<IECPoint>) InitStatic();

    CARAPI GetAffineX(
        /* [out] */ IBigInteger **affineX);

    CARAPI GetAffineY(
        /* [out] */ IBigInteger **affineY);

    CARAPI Equals(
        /* [in] */ IInterface *other,
        /* [out] */ Boolean *result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBigInteger *affineX,
        /* [in] */ IBigInteger *affineY);

    /**
     * The point on an Elliptic Curve at infinity.
    */
    static const AutoPtr<IECPoint> POINT_INFINITY;

private:
    // affine X coordinate of this point
    AutoPtr<IBigInteger> mAffineX;
    // affine Y coordinate of this point
    AutoPtr<IBigInteger> mAffineY;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECPOINT_H__
