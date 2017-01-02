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

#ifndef __ELASTOS_SECURITY_CACCESSCONTROLCONTEXT_H__
#define __ELASTOS_SECURITY_CACCESSCONTROLCONTEXT_H__

#include "_Elastos_Security_CAccessControlContext.h"
#include "core/Object.h"

namespace Elastos {
namespace Security {

CarClass(CAccessControlContext)
    , public Object
    , public IAccessControlContext
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI CheckPermission(
        /* [in] */ IPermission *perm);

    CARAPI GetDomainCombiner(
        /* [out] */ IDomainCombiner **domCombiner);

    CARAPI constructor(
        /* [in] */ IAccessControlContext *acc,
        /* [in] */ IDomainCombiner *combiner);

    CARAPI constructor(
        /* [in] */ ArrayOf<IProtectionDomain*> *context);
};

}
}

#endif // __ELASTOS_SECURITY_CACCESSCONTROLCONTEXT_H__
