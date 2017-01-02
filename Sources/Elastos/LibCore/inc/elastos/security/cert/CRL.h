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

#ifndef __ELASTOS_SECURITY_CERT_CRL_H__
#define __ELASTOS_SECURITY_CERT_CRL_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

using Elastos::Security::ICertificate;

namespace Elastos {
namespace Security {
namespace Cert {

class CRL
    : public Object
    , public ICRL
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the type of this CRL.
     *
     * @return the type of this CRL.
     */
    CARAPI GetType(
        /* [out] */ String* type);

    /**
     * Returns whether the specified certificate is revoked by this CRL.
     *
     * @param cert
     *            the certificate to check.
     * @return {@code true} if the certificate is revoked by this CRL, otherwise
     *         {@code false}.
     */
    virtual CARAPI IsRevoked(
        /* [in] */ ICertificate* cert,
        /* [out] */  Boolean* isRevoked) = 0;

    /**
     * Returns the string representation of this instance.
     *
     * @return the string representation of this instance.
     */
    virtual CARAPI ToString(
        /* [out] */ String* str) = 0;

protected:
    /**
     * Creates a new certificate revocation list of the specified type.
     *
     * @param type
     *            the type for the CRL.
     */
    CRL(
        /* [in] */ const String& type);

private:
    // The CRL type
    const String mType;
};

} // end Cert
} // end Security
} // end Elastos

#endif // __ELASTOS_SECURITY_CERT_CRL_H__
