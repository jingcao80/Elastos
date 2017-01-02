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

#ifndef __ELASTOSX_SECURITY_AUTH_CSUBJECTDOMAINCOMBINER_H__
#define __ELASTOSX_SECURITY_AUTH_CSUBJECTDOMAINCOMBINER_H__

#include "_Elastosx_Security_Auth_CSubjectDomainCombiner.h"
#include "Object.h"

using Elastos::Security::IProtectionDomain;
using Elastos::Core::Object;
using Elastos::Security::IDomainCombiner;

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CSubjectDomainCombiner)
    , public Object
    , public IDomainCombiner
    , public ISubjectDomainCombiner
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Combine(
        /* [in] */ ArrayOf<IProtectionDomain*> *current,
        /* [in] */ ArrayOf<IProtectionDomain*> *assigned,
        /* [out, callee] */ ArrayOf<IProtectionDomain*> **result);

    CARAPI GetSubject(
        /* [out] */ ISubject **subject);

    CARAPI constructor(
        /* [in] */ ISubject *sub);
};

} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_CSUBJECTDOMAINCOMBINER_H__
