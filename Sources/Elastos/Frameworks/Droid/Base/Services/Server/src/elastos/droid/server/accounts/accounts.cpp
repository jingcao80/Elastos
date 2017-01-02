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

#include "elastos/droid/server/accounts/CTestFeaturesSession.h"
#include "elastos/droid/server/accounts/CRemoveAccountSession.h"
#include "elastos/droid/server/accounts/CGetAuthTokenLabelSession.h"
#include "elastos/droid/server/accounts/CGetAuthTokenSession.h"
#include "elastos/droid/server/accounts/CAddAcountSession.h"
#include "elastos/droid/server/accounts/CConfirmCredentialsAsUserSession.h"
#include "elastos/droid/server/accounts/CUpdateCredentialsSession.h"
#include "elastos/droid/server/accounts/CEditPropertiesSession.h"
#include "elastos/droid/server/accounts/CGetAccountsByTypeAndFeatureSession.h"
#include "elastos/droid/server/accounts/CCopyAccountToUserSession.h"
#include "elastos/droid/server/accounts/CCompleteCloningAccountSession.h"
#include "elastos/droid/server/accounts/CAddAccountAsUserSession.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CAR_OBJECT_IMPL(CTestFeaturesSession)

CAR_OBJECT_IMPL(CRemoveAccountSession)

CAR_OBJECT_IMPL(CGetAuthTokenLabelSession)

CAR_OBJECT_IMPL(CGetAuthTokenSession)

CAR_OBJECT_IMPL(CAddAcountSession)

CAR_OBJECT_IMPL(CConfirmCredentialsAsUserSession)

CAR_OBJECT_IMPL(CUpdateCredentialsSession)

CAR_OBJECT_IMPL(CEditPropertiesSession)

CAR_OBJECT_IMPL(CGetAccountsByTypeAndFeatureSession)

CAR_OBJECT_IMPL(CCopyAccountToUserSession)

CAR_OBJECT_IMPL(CCompleteCloningAccountSession)

CAR_OBJECT_IMPL(CAddAccountAsUserSession)

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos
