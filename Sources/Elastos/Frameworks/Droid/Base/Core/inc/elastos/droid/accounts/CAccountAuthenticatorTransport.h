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

#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORTRANSPORT_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORTRANSPORT_H__

#include "_Elastos_Droid_Accounts_CAccountAuthenticatorTransport.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

class AbstractAccountAuthenticator;

CarClass(CAccountAuthenticatorTransport)
    , public Object
    , public IIAccountAuthenticator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI AddAccount(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ ArrayOf<String>* requiredFeatures,
        /* [in] */ IBundle* options);

    CARAPI ConfirmCredentials(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* options);

    CARAPI GetAuthTokenLabel(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ const String& authTokenType);

    CARAPI GetAuthToken(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ IBundle* options);

    CARAPI UpdateCredentials(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ IBundle* options);

    CARAPI EditProperties(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ const String& accountType);

    CARAPI HasFeatures(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<String>* features);

    CARAPI GetAccountRemovalAllowed(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account);

    CARAPI constructor(
        /* [in] */ Handle32 accounts);

    CARAPI GetAccountCredentialsForCloning(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account);

    CARAPI AddAccountFromCredentials(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* accountCredentials);

private:
    AbstractAccountAuthenticator* mHost;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORTRANSPORT_H__
