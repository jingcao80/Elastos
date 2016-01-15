#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CActivityManagerHelper.h"
#include "elastos/droid/app/CActivityManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CActivityManagerHelper, Singleton, IActivityManagerHelper)

CAR_SINGLETON_IMPL(CActivityManagerHelper)

/** @hide */
ECode CActivityManagerHelper::StaticGetMemoryClass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);
    *cls = CActivityManager::StaticGetMemoryClass();
    return NOERROR;
}

/** @hide */
ECode CActivityManagerHelper::StaticGetLargeMemoryClass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);
    *cls = CActivityManager::StaticGetLargeMemoryClass();
    return NOERROR;
}

ECode CActivityManagerHelper::IsHighEndGfx(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CActivityManager::IsHighEndGfx();
    return NOERROR;
}

ECode CActivityManagerHelper::IsLowRamDeviceStatic(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CActivityManager::IsLowRamDeviceStatic();
    return NOERROR;
}

ECode CActivityManagerHelper::GetMaxRecentTasksStatic(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);
    *cls = CActivityManager::GetMaxRecentTasksStatic();
    return NOERROR;
}

ECode CActivityManagerHelper::GetDefaultAppRecentsLimitStatic(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);
    *cls = CActivityManager::GetDefaultAppRecentsLimitStatic();
    return NOERROR;
}

ECode CActivityManagerHelper::GetMaxAppRecentsLimitStatic(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);
    *cls = CActivityManager::GetMaxAppRecentsLimitStatic();
    return NOERROR;
}

ECode CActivityManagerHelper::GetMyMemoryState(
    /* [out] */ IActivityManagerRunningAppProcessInfo* outState)
{
    return CActivityManager::GetMyMemoryState(outState);
}

ECode CActivityManagerHelper::IsUserAMonkey(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CActivityManager::IsUserAMonkey();
    return NOERROR;
}

ECode CActivityManagerHelper::IsRunningInTestHarness(
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CActivityManager::IsRunningInTestHarness();
    return NOERROR;
}

ECode CActivityManagerHelper::CheckComponentPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 owningUid,
    /* [in] */ Boolean exported,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CActivityManager::CheckComponentPermission(
            permission, uid, owningUid, exported);
    return NOERROR;
}

ECode CActivityManagerHelper::CheckUidPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CActivityManager::CheckUidPermission(permission, uid);
    return NOERROR;
}

ECode CActivityManagerHelper::HandleIncomingUser(
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean allowAll,
    /* [in] */ Boolean requireFull,
    /* [in] */ const String& name,
    /* [in] */ const String& callerPackage,
    /* [out] */ Int32* value)
{
    return CActivityManager::HandleIncomingUser(
            callingPid, callingUid, userId, allowAll, requireFull,
            name , callerPackage, value);
}

ECode CActivityManagerHelper::GetCurrentUser(
    /* [out] */ Int32* currentUser)
{
    VALIDATE_NOT_NULL(currentUser);
    *currentUser = CActivityManager::GetCurrentUser();
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
