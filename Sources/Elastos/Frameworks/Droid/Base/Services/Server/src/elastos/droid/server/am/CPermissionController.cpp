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

#include "elastos/droid/server/am/CPermissionController.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include <Elastos.Droid.App.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IPermissionController;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL_2(CPermissionController, Object, IPermissionController, IBinder)
CAR_OBJECT_IMPL(CPermissionController)

ECode CPermissionController::constructor(
    /* [in] */ IIActivityManager* host)
{
    mActivityManagerService = (CActivityManagerService*)host;
    return NOERROR;
}

ECode CPermissionController::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 res;
    mActivityManagerService->CheckPermission(permission, pid, uid, &res);
    *result = res == IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

ECode CPermissionController::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos