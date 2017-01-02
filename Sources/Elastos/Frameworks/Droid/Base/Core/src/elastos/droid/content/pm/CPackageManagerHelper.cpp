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
#include "elastos/droid/content/pm/CPackageManagerHelper.h"
#include "elastos/droid/content/pm/PackageManager.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CPackageManagerHelper, Singleton, IPackageManagerHelper)

CAR_SINGLETON_IMPL(CPackageManagerHelper)

ECode CPackageManagerHelper::GetDataDirForUser(
    /* [in] */ Int32 userId,
    /* [in] */ const String& packageName,
    /* [out] */ String* directory)
{
    VALIDATE_NOT_NULL(directory)
    *directory = PackageManager::GetDataDirForUser(userId, packageName);
    return NOERROR;
}

ECode CPackageManagerHelper::InstallStatusToString(
    /* [in] */ Int32 status,
    /* [in] */ const String& msg,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = PackageManager::InstallStatusToString(status, msg);
    return NOERROR;
}

ECode CPackageManagerHelper::InstallStatusToString(
    /* [in] */ Int32 status,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = PackageManager::InstallStatusToString(status);
    return NOERROR;
}

ECode CPackageManagerHelper::InstallStatusToPublicStatus(
    /* [in] */ Int32 status,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = PackageManager::InstallStatusToPublicStatus(status);
    return NOERROR;
}

ECode CPackageManagerHelper::DeleteStatusToString(
    /* [in] */ Int32 status,
    /* [in] */ const String& msg,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = PackageManager::DeleteStatusToString(status, msg);
    return NOERROR;
}

ECode CPackageManagerHelper::DeleteStatusToString(
    /* [in] */ Int32 status,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = PackageManager::DeleteStatusToString(status);
    return NOERROR;
}

ECode CPackageManagerHelper::DeleteStatusToPublicStatus(
    /* [in] */ Int32 status,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = PackageManager::DeleteStatusToPublicStatus(status);
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

