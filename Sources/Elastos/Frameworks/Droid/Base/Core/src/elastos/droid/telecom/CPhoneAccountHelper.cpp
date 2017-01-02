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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CPhoneAccountHelper.h"
#include "elastos/droid/telecom/PhoneAccount.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CPhoneAccountHelper::
//===============================================================
CAR_SINGLETON_IMPL(CPhoneAccountHelper)

CAR_INTERFACE_IMPL(CPhoneAccountHelper, Singleton, IPhoneAccountHelper)

ECode CPhoneAccountHelper::Builder(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ ICharSequence* label,
    /* [out] */ IPhoneAccountBuilder** res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IPhoneAccountBuilder> p = PhoneAccount::_Builder(accountHandle, label);
    *res = p;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos