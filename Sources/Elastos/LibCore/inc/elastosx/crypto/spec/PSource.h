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

#ifndef __ELASTOSX_CRYPTO_SPEC_PSOURCE_H__
#define __ELASTOSX_CRYPTO_SPEC_PSOURCE_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

namespace Elastosx {
namespace Crypto {
namespace Spec {

class PSource
    : public Object
    , public IPSource
{
public:
    CAR_INTERFACE_DECL()

    PSource();

    /**
     * Creates a new <code>PSource</code> instance with the specified source
     * algorithm identifier.
     *
     * @param pSrcName
     *            the source algorithm identifier.
     * @throws NullPointerException
     *             if pSrcName is null.
     */
    CARAPI constructor(
        /* [in] */ const String& pSrcName);

    /**
     * Returns the source algorithm identifier.
     *
     * @return the source algorithm identifier.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * result);

private:
    String mPSrcName;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_PSOURCE_H__
