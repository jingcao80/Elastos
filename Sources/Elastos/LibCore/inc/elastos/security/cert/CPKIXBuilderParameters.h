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

#ifndef __ELASTOS_SECURITY_CERT_CPKIXBUILDERPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CPKIXBUILDERPARAMETERS_H__

#include "_Elastos_Security_Cert_CPKIXBuilderParameters.h"
#include "PKIXParameters.h"

using Elastos::Utility::IDate;
using Elastos::Security::IKeyStore;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPKIXBuilderParameters), public PKIXParameters
{
public:
    CAR_OBJECT_DECL()

    CPKIXBuilderParameters();

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI GetMaxPathLength(
        /* [out] */ Int32 *length);

    CARAPI SetMaxPathLength(
        /* [in] */ Int32 maxPathLength);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISet *trustAnchors,
        /* [in] */ ICertSelector *targetConstraints);

    CARAPI constructor(
        /* [in] */ IKeyStore *keyStore,
        /* [in] */ ICertSelector *targetConstraints);

private:
    // Maximum certificate path length (5 by default)
    Int32 mMaxPathLength;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPKIXBUILDERPARAMETERS_H__
