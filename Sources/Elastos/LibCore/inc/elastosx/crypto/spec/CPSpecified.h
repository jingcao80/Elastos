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

#ifndef __ELASTOSX_CRYPTO_SPEC_CPSPECIFIED_H__
#define __ELASTOSX_CRYPTO_SPEC_CPSPECIFIED_H__

#include "_Elastosx_Crypto_Spec_CPSpecified.h"
#include <elastos/core/Object.h>
#include "PSource.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CPSpecified)
    , public IPSpecified
    , public PSource
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPSpecified();

    CARAPI constructor();

    /**
     * Creates a new instance of <code>PSpecified</code> with the specified
     * parameter <code>P</code>.
     *
     * @param p
     *            the parameter <code>P</code>.
     * @throws NullPointerException
     *             if <code>p</code> is null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * p);

    /**
     * Returns a copy of the value of the parameter <code>P</code>.
     *
     * @return a copy of the value of the parameter <code>P</code>
     */
    CARAPI GetValue(
        /* [out, callee] */ ArrayOf<Byte> ** result);

private:
    AutoPtr<ArrayOf<Byte> > mP;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CPSPECIFIED_H__
