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

#include "CSubjectDomainCombiner.h"

using Elastos::Security::EIID_IDomainCombiner;

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CSubjectDomainCombiner)

CAR_INTERFACE_IMPL_2(CSubjectDomainCombiner, Object, IDomainCombiner, ISubjectDomainCombiner)

ECode CSubjectDomainCombiner::Combine(
    /* [in] */ ArrayOf<IProtectionDomain*> *current,
    /* [in] */ ArrayOf<IProtectionDomain*> *assigned,
    /* [out, callee] */ ArrayOf<IProtectionDomain*> **result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(current)
    VALIDATE_NOT_NULL(assigned)

    return NOERROR;
}

ECode CSubjectDomainCombiner::GetSubject(
    /* [out] */ ISubject **subject)
{
    VALIDATE_NOT_NULL(subject)
    *subject = NULL;

    return NOERROR;
}

ECode CSubjectDomainCombiner::constructor(
    /* [in] */ ISubject *sub)
{
    VALIDATE_NOT_NULL(sub)

    return NOERROR;
}

} // namespace Auth
} // namespace Security
} // namespace Elastosx
