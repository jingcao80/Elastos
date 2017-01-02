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

#ifndef __ELASTOSX_NET_SSL_CERTPATHTRUSTMANAGERPARAMETERS_H__
#define __ELASTOSX_NET_SSL_CERTPATHTRUSTMANAGERPARAMETERS_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::Cert::ICertPathParameters;
using Elastosx::Net::Ssl::IManagerFactoryParameters;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * Certification path parameters to provide to certification path
 * based {@link TrustManager}.
 *
 * @since 1.5
 */
class CertPathTrustManagerParameters
    : public Object
    , public ICertPathTrustManagerParameters
    , public IManagerFactoryParameters
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code CertPathTrustManagerParameters} with the specified
     * certification path parameters.
     *
     * @param parameters
     *            the certification path parameters.
     */
    CARAPI constructor(
        /* [in] */ ICertPathParameters* parameters);

    /**
     * Returns a copy of the certification path parameters.
     *
     * @return a copy of the certification path parameters.
     */
    CARAPI GetParameters(
        /* [out] */ ICertPathParameters** parameters);

private:
    /*const*/ AutoPtr<ICertPathParameters> mParam;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_CERTPATHTRUSTMANAGERPARAMETERS_H__
