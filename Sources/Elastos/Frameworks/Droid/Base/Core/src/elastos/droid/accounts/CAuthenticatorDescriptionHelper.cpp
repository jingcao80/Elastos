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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAuthenticatorDescriptionHelper.h"
#include "elastos/droid/accounts/CAuthenticatorDescription.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_SINGLETON_IMPL(CAuthenticatorDescriptionHelper)

CAR_INTERFACE_IMPL(CAuthenticatorDescriptionHelper, Singleton, IAuthenticatorDescriptionHelper)

ECode CAuthenticatorDescriptionHelper::NewKey(
    /* [in] */ const String& type,
    /* [out] */ IAuthenticatorDescription** description)
{
    VALIDATE_NOT_NULL(description);
    return CAuthenticatorDescription::NewKey(type, description);
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
