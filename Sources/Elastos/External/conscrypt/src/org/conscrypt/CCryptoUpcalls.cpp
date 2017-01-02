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

#include "org/conscrypt/CCryptoUpcalls.h"
#include "org/conscrypt/CryptoUpcalls.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(CCryptoUpcalls)

CAR_INTERFACE_IMPL(CCryptoUpcalls, Singleton, ICryptoUpcalls)

ECode CCryptoUpcalls::GetExternalProvider(
    /* [in] */ const String& algorithm,
    /* [out] */ IProvider** result)
{
    return CryptoUpcalls::GetExternalProvider(algorithm, result);
}

ECode CCryptoUpcalls::RawSignDigestWithPrivateKey(
    /* [in] */ IPrivateKey* key,
    /* [in] */ ArrayOf<Byte>* message,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return CryptoUpcalls::RawSignDigestWithPrivateKey(key, message, result);
}

ECode CCryptoUpcalls::RawCipherWithPrivateKey(
    /* [in] */ IPrivateKey* key,
    /* [in] */ Boolean encrypt,
    /* [in] */ ArrayOf<Byte>* input,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return CryptoUpcalls::RawCipherWithPrivateKey(key, encrypt, input, result);
}

} // namespace Conscrypt
} // namespace Org
