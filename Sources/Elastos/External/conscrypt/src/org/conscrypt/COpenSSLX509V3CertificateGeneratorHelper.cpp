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

#include "org/conscrypt/COpenSSLX509V3CertificateGeneratorHelper.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLX509V3CertificateGeneratorHelper)

CAR_INTERFACE_IMPL(COpenSSLX509V3CertificateGeneratorHelper, Singleton, IOpenSSLX509V3CertificateGeneratorHelper)

ECode COpenSSLX509V3CertificateGeneratorHelper::Generate(
    /* [in] */ IPublicKey *pubkey,
    /* [in] */ IPrivateKey *privkey,
    /* [in] */ IBigInteger* serialNumber,
    /* [in] */ IDate* start,
    /* [in] */ IDate* end,
    /* [in] */ const String& subjectName,
    /* [in] */ const String& hashalg,
    /* [out] */ IX509Certificate** result)
{
    return OpenSSLX509V3CertificateGenerator::Generate(pubkey, privkey, serialNumber, start, end, subjectName, hashalg, result);
}

} // namespace Conscrypt
} // namespace Org
