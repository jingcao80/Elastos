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

#include "CAccessController.h"
#include "CAccessControlContext.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CAccessController);
CAR_INTERFACE_IMPL(CAccessController, Singleton, IAccessController)
ECode CAccessController::DoPrivileged(
    /* [in] */ IPrivilegedAction *action,
    /* [out] */ IInterface **result)
{
    return action->Run(result);
}

ECode CAccessController::DoPrivileged(
    /* [in] */ IPrivilegedAction *action,
    /* [in] */ IAccessControlContext *context,
    /* [out] */ IInterface **result)
{
    return action->Run(result);
}

ECode CAccessController::DoPrivileged(
    /* [in] */ IPrivilegedExceptionAction *action,
    /* [out] */ IInterface **result)
{
    ECode ec = action->Run(result);
    if (FAILED(ec) && ec != (ECode)E_RUNTIME_EXCEPTION) {
        ec = E_PRIVILEGED_ACTION_EXCEPTION;
    }
    return ec;
}

ECode CAccessController::DoPrivileged(
    /* [in] */ IPrivilegedExceptionAction *action,
    /* [in] */ IAccessControlContext *context,
    /* [out] */ IInterface **result)
{
    return DoPrivileged(action, result);
}

ECode CAccessController::DoPrivilegedWithCombiner(
    /* [in] */ IPrivilegedAction *action,
    /* [out] */ IInterface **result)
{
    return action->Run(result);
}

ECode CAccessController::DoPrivilegedWithCombiner(
    /* [in] */ IPrivilegedExceptionAction *action,
    /* [out] */ IInterface **result)
{
    return DoPrivileged(action, result);
}

ECode CAccessController::CheckPermission(
    /* [in] */ IPermission *permission)
{
    return NOERROR;
}

ECode CAccessController::GetContext(
    /* [out] */ IAccessControlContext **context)
{
    return CAccessControlContext::New(NULL, context);
}

}
}

