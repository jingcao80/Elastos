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

#ifndef __ORG_CONSCRYPT_OPENSSLPROVIDER_H__
#define __ORG_CONSCRYPT_OPENSSLPROVIDER_H__

#include "Org.Conscrypt.h"
#include "elastos/security/Provider.h"

using Elastos::Security::Provider;

namespace Org {
namespace Conscrypt {

/**
 * Provider that goes through OpenSSL for operations.
 * <p>
 * Every algorithm should have its IANA assigned OID as an alias. See the following URLs for each type:
 * <ul>
 * <li><a href="http://www.iana.org/assignments/hash-function-text-names/hash-function-text-names.xml">Hash functions</a></li>
 * <li><a href="http://www.iana.org/assignments/dssc/dssc.xml">Signature algorithms</a></li>
 * <li><a href="http://csrc.nist.gov/groups/ST/crypto_apps_infra/csor/algorithms.html">NIST cryptographic algorithms</a></li>
 * </ul>
 */
class OpenSSLProvider
    : public Provider
{
public:
    OpenSSLProvider();

    OpenSSLProvider(
        /* [in] */ String providerName);

public:
    static String PROVIDER_NAME;

private:
    static Int64 mSerialVersionUID;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLPROVIDER_H__