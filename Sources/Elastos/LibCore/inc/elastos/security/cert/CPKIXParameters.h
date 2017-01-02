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

#ifndef __ELASTOS_SECURITY_CERT_CPKIXPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CPKIXPARAMETERS_H__

#include "_Elastos_Security_Cert_CPKIXParameters.h"
#include "PKIXParameters.h"

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPKIXParameters), public PKIXParameters
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISet *trustAnchors);

    CARAPI constructor(
        /* [in] */ IKeyStore *keyStore);
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPKIXPARAMETERS_H__
