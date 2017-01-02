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

#include "Elastos.CoreLibrary.Security.h"
#include "CertPathTrustManagerParameters.h"

using Elastos::Core::ICloneable;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL_2(CertPathTrustManagerParameters, Object, ICertPathTrustManagerParameters, IManagerFactoryParameters)

ECode CertPathTrustManagerParameters::constructor(
    /* [in] */ ICertPathParameters* parameters)
{
    AutoPtr<IInterface> newDate;
    ICloneable::Probe(parameters)->Clone((IInterface**)&newDate);
    mParam = ICertPathParameters::Probe(newDate);
    return NOERROR;
}

ECode CertPathTrustManagerParameters::GetParameters(
    /* [out] */ ICertPathParameters** parameters)
{
    VALIDATE_NOT_NULL(parameters)

    AutoPtr<IInterface> newDate;
    ICloneable::Probe(mParam)->Clone((IInterface**)&newDate);
    *parameters = ICertPathParameters::Probe(newDate);
    REFCOUNT_ADD(*parameters);
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
