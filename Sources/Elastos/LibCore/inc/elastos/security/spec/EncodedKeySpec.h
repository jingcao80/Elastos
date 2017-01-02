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

#ifndef __ELASTOS_SECURITY_SPEC_ENCODEDKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_ENCODEDKEYSPEC_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

namespace Elastos {
namespace Security {
namespace Spec {

class EncodedKeySpec
    : public Object
    , public IEncodedKeySpec
    , public IKeySpec
{
public:
    CAR_INTERFACE_DECL()

    EncodedKeySpec();

    EncodedKeySpec(
        /* [in] */ ArrayOf<Byte> *encodedKey);

    /**
     * Returns the encoded key bytes.
     *
     * @return the encoded key bytes.
     */
    virtual CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> **encodedKey);

    virtual CARAPI GetFormat(
        /* [out] */ String *format) = 0;

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> *encodedKey);

private:
    // Encoded key
    AutoPtr<ArrayOf<Byte> > mEncodedKey;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_ENCODEDKEYSPEC_H__
