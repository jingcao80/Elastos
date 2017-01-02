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

#ifndef __ELASTOS_SECURITY_SPEC_CMGF1PARAMETERSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CMGF1PARAMETERSPEC_H__

#include "_Elastos_Security_Spec_CMGF1ParameterSpec.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CMGF1ParameterSpec)
    , public Object
    , public IMGF1ParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IMGF1ParameterSpec>) InitStatic(
        /* [in] */ const String& type);

    CARAPI GetDigestAlgorithm(
        /* [out] */ String *algorithm);

    CARAPI constructor(
        /* [in] */ const String& mdName);


public:
    /**
     * The predefined MGF1 parameter specification with an "SHA-1" message
     * digest.
     */
    static const AutoPtr<IMGF1ParameterSpec> SHA1;

    /**
     * The predefined MGF1 parameter specification with an "SHA-256" message
     * digest.
     */
    static const AutoPtr<IMGF1ParameterSpec> SHA256;

    /**
     * The predefined MGF1 parameter specification with an "SHA-384" message
     * digest.
     */
    static const AutoPtr<IMGF1ParameterSpec> SHA384;

    /**
     * The predefined MGF1 parameter specification with an "SHA-512" message
     * digest.
     */
    static const AutoPtr<IMGF1ParameterSpec> SHA512;

private:
    //  Message digest algorithm name
    String mMdName;
};

} // namespace Spec
} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_SPEC_CMGF1PARAMETERSPEC_H__
