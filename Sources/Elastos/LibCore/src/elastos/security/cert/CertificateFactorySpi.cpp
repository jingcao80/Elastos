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

#include "security/cert/CertificateFactorySpi.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CertificateFactorySpi, Object, ICertificateFactorySpi)
CertificateFactorySpi::CertificateFactorySpi()
{}

ECode CertificateFactorySpi::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertPath** certPath)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CertificateFactorySpi::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [in] */ const String& encoding,
    /* [out] */ ICertPath** certPath)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CertificateFactorySpi::EngineGenerateCertPath(
    /* [in] */ IList* certificates,
    /* [out] */ ICertPath** certPath)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CertificateFactorySpi::EngineGetCertPathEncodings(
    /* [out] */ IIterator** it)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // end Cert
} // end Security
} // end Elastos