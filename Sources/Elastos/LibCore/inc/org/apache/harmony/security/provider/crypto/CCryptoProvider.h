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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CCryptoProvider.h"
#include "Provider.h"

using Elastos::Security::Provider;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

/**
 * Implementation of Provider for SecureRandom. The implementation supports the
 * "SHA1PRNG" algorithm described in JavaTM Cryptography Architecture, API
 * Specification & Reference
 */
CarClass(CCryptoProvider)
    , public Elastos::Security::Provider
{
public:
    CAR_OBJECT_DECL()

    /**
     * Creates a Provider and puts parameters
     */
    CARAPI constructor();

private:
    // private static final long serialVersionUID = 7991202868423459598L;
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CCRYPTOPROVIDER_H__
