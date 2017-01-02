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

#ifndef __ELASTOS_DROID_CONTENT_PM_CVERIFIERINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CVERIFIERINFO_H__

#include "_Elastos_Droid_Content_Pm_CVerifierInfo.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPublicKey;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Contains information about a package verifier as used by
 * {@code PackageManagerService} during package verification.
 *
 * @hide
 */
CarClass(CVerifierInfo)
    , public Object
    , public IVerifierInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CVerifierInfo();

    virtual ~CVerifierInfo();

    CARAPI constructor();

    /**
     * Creates an object that represents a verifier info object.
     *
     * @param packageName the package name in Java-style. Must not be {@code
     *            null} or empty.
     * @param publicKey the public key for the signer encoded in Base64. Must
     *            not be {@code null} or empty.
     * @throws IllegalArgumentException if either argument is null or empty.
     */
    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ IPublicKey* publicKey);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** publicKey);

    CARAPI SetPublicKey(
        /* [in] */ IPublicKey* publicKey);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /** Package name of the verifier. */
    String mPackageName;

    /** Signatures used to sign the package verifier's package. */
    AutoPtr<IPublicKey> mPublicKey;

};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CVERIFIERINFO_H__
