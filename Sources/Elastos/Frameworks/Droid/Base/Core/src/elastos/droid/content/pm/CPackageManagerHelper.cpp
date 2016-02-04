
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

