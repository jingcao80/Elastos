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

#ifndef __ELASTOS_SECURITY_SECURERANDOMSPI_H__
#define __ELASTOS_SECURITY_SECURERANDOMSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

/**
 * {@code SecureRandomSpi} is the <i>Service Provider Interface</i> (<b>SPI</b>) definition
 * for {@link SecureRandom}.
 *
 * @see SecureRandom
 */
class SecureRandomSpi
    : public Object
    , public ISecureRandomSpi
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~SecureRandomSpi() = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_SECURERANDOMSPI_H__
