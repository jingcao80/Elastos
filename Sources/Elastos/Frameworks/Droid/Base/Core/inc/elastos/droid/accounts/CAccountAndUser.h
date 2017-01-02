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

#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__

#include "_Elastos_Droid_Accounts_CAccountAndUser.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccountAndUser)
    , public Object
    , public IAccountAndUser
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IAccount *account,
        /* [in] */ Int32 userId);

    CARAPI Equals(
        /* [in] */ IInterface *o,
        /* [out] */ Boolean *isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hash);

    CARAPI ToString(
        /* [out] */ String *info);

    CARAPI GetAccount(
        /* [out] */ IAccount **account);

    CARAPI GetUserId(
        /* [out] */ Int32 *id);

public:
    AutoPtr<IAccount> mAccount;
    Int32 mUserId;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__
