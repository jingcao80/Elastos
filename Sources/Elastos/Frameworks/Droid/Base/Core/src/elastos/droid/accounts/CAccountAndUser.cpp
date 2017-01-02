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

#include "elastos/droid/accounts/CAccountAndUser.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_OBJECT_IMPL(CAccountAndUser)

CAR_INTERFACE_IMPL(CAccountAndUser, Object, IAccountAndUser)

ECode CAccountAndUser::Equals(
    /* [in] */ IInterface *o,
    /* [out] */ Boolean *isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    if (TO_IINTERFACE(this) == o) {
        *isEqual = TRUE;
        return NOERROR;
    }
    if (!IAccountAndUser::Probe(o)) {
        *isEqual = FALSE;
        return NOERROR;
    }
    const AutoPtr<CAccountAndUser> other = (CAccountAndUser*)IAccountAndUser::Probe(o);
    Boolean equal;
    mAccount->Equals(other->mAccount.Get(), &equal);
    *isEqual = equal && mUserId == other->mUserId;
    return NOERROR;
}

ECode CAccountAndUser::GetHashCode(
    /* [out] */ Int32 *hash)
{
    VALIDATE_NOT_NULL(hash)
    IObject::Probe(mAccount.Get())->GetHashCode(hash);
    *hash += mUserId;
    return NOERROR;
}

ECode CAccountAndUser::ToString(
    /* [out] */ String *info)
{
    VALIDATE_NOT_NULL(info)
    mAccount->ToString(info);
    *info += " u";
    info->AppendFormat("%d", mUserId);
    return NOERROR;
}

ECode CAccountAndUser::GetAccount(
    /* [out] */ IAccount **account)
{
    VALIDATE_NOT_NULL(account)
    *account = mAccount;
    REFCOUNT_ADD(*account)
    return NOERROR;
}

ECode CAccountAndUser::GetUserId(
    /* [out] */ Int32 *id)
{
    VALIDATE_NOT_NULL(id)
    *id = mUserId;
    return NOERROR;
}

ECode CAccountAndUser::constructor(
    /* [in] */ IAccount *account,
    /* [in] */ Int32 userId)
{
    mAccount = account;
    mUserId = userId;
    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
