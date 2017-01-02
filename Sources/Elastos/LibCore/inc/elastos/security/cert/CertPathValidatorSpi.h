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

#ifndef __ELASTOS_SECURITY_CERT_CERTPATHVALIDATORSPI_H__
#define __ELASTOS_SECURITY_CERT_CERTPATHVALIDATORSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {
namespace Cert {

/**
 * The <i>Service Provider Interface</i> (<b>SPI</b>) for the {@code
 * CertPathValidator} class to be implemented by security providers.
 */
class CertPathValidatorSpi
    : public Object
    , public ICertPathValidatorSpi
{
public:
    CAR_INTERFACE_DECL()

    CertPathValidatorSpi();
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CERTPATHVALIDATORSPI_H__
