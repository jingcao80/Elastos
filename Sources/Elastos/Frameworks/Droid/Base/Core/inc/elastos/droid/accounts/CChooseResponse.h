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

#ifndef __ELASTOS_DROID_ACCOUNTS_CCHOOSERESPONSE_H__
#define __ELASTOS_DROID_ACCOUNTS_CCHOOSERESPONSE_H__

#include "_Elastos_Droid_Accounts_CChooseResponse.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CChooseResponse)
    , public Object
    , public IAccountManagerResponse
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnResult(
        /* [in] */ IBundle* value);

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

    CARAPI constructor (
        /* [in] */ HANDLE host);

private:
    AutoPtr<CAccountManager::GetAuthTokenByTypeAndFeaturesTask> mHost;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CCHOOSERESPONSE_H__
