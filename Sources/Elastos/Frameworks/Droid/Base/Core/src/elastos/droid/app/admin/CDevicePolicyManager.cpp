
#include "elastos/droid/app/admin/CDevicePolicyManager.h"
#include "elastos/droid/app/admin/CDeviceAdminInfo.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/net/CProxy.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Net::ProxyType;
using Elastos::Net::IProxyHelper;
using Elastos::Net::CProxyHelper;
using Elastos::Net::ISocketAddress;
using Elastos::Net::IInetSocketAddress;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::App::Admin::CDeviceAdminInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

const String CDevicePolicyManager::TAG("CDevicePolicyManager");

CAR_INTERFACE_IMPL(CDevicePolicyManager, Object, IDevicePolicyManager)

CAR_OBJECT_IMPL(CDevicePolicyManager)

ECode CDevicePolicyManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> service;
    sm->GetService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&service);
    mService = IIDevicePolicyManager::Probe(service);
    assert(mService != NULL);
    return NOERROR;
}

ECode CDevicePolicyManager::IsAdminActive(
    /* [in] */ IComponentName* who,
    /* [out] */ Boolean* isAdminActive)
{
    return IsAdminActiveAsUser(who, UserHandle::MyUserId(), isAdminActive);
}

ECode CDevicePolicyManager::IsAdminActive(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* isAdminActive)
{
    VALIDATE_NOT_NULL(isAdminActive);
    *isAdminActive = FALSE;
    if (mService != NULL) {
        ECode ec = mService->IsAdminActive(who, userId, isAdminActive);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetActiveAdmins(
    /* [out] */ IList** admins)
{
    return GetActiveAdmins(UserHandle::MyUserId(), admins);
}

ECode CDevicePolicyManager::GetActiveAdmins(
    /* [in] */ Int32 userId,
    /* [out] */ IList** admins)
{
    VALIDATE_NOT_NULL(admins);
    *admins = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetActiveAdmins(userId, admins);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::PackageHasActiveAdmins(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* hasAdmins)
{
    VALIDATE_NOT_NULL(hasAdmins);
    *hasAdmins = FALSE;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->PackageHasActiveAdmins(packageName, myUserId, hasAdmins);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::RemoveActiveAdmin(
    /* [in] */ IComponentName* who)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->RemoveActiveAdmin(who, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::HasGrantedPolicy(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 usesPolicy,
    /* [out] */ Boolean* hasGrant)
{
    VALIDATE_NOT_NULL(hasGrant);
    *hasGrant = FALSE;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->HasGrantedPolicy(admin, usesPolicy, myUserId, hasGrant);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordQuality(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 quality)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordQuality(admin, quality, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordQuality(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordQuality(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordQuality(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;

     if (mService != NULL) {
         ECode ec = mService->GetPasswordQuality(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
     }
     return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumLength(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordMinimumLength(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumLength(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordMinimumLength(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordMinimumLength(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordMinimumLength(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordMinimumUpperCase(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordMinimumUpperCase(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordMinimumUpperCase(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordMinimumLowerCase(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordMinimumLowerCase(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordMinimumLowerCase(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumLetters(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordMinimumLetters(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumLetters(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordMinimumLetters(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordMinimumLetters(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordMinimumLetters(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumNumeric(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordMinimumNumeric(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumNumeric(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordMinimumNumeric(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordMinimumNumeric(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordMinimumNumeric(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumSymbols(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordMinimumSymbols(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumSymbols(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordMinimumSymbols(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordMinimumSymbols(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordMinimumSymbols(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordMinimumNonLetter(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordMinimumNonLetter(admin, myUserId, password);
}

ECode CDevicePolicyManager::GetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordMinimumNonLetter(admin, userHandle, password);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordHistoryLength(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordHistoryLength(admin, length, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordExpirationTimeout(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int64 timeout)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetPasswordExpirationTimeout(admin, timeout, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordExpirationTimeout(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int64* timeout)
{
    VALIDATE_NOT_NULL(timeout);
    *timeout = 0;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->GetPasswordExpirationTimeout(admin, myUserId, timeout);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordExpiration(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int64* expiration)
{
    VALIDATE_NOT_NULL(expiration);
    *expiration = 0;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->GetPasswordExpiration(admin, myUserId, expiration);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordHistoryLength(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetPasswordHistoryLength(admin, myUserId, length);
}

ECode CDevicePolicyManager::GetPasswordHistoryLength(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    *length = 0;

    if (mService != NULL) {
        ECode ec = mService->GetPasswordHistoryLength(admin, userHandle, length);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMaximumLength(
    /* [in] */ Int32 quality,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    // Kind-of arbitrary.
    *length = 16;
    return NOERROR;
}

ECode CDevicePolicyManager::IsActivePasswordSufficient(
    /* [out] */ Boolean* isSufficient)
{
    VALIDATE_NOT_NULL(isSufficient);
    *isSufficient = FALSE;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->IsActivePasswordSufficient(myUserId, isSufficient);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetCurrentFailedPasswordAttempts(
    /* [out] */ Int32* atempts)
{
    VALIDATE_NOT_NULL(atempts);
    *atempts = -1;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->GetCurrentFailedPasswordAttempts(myUserId, atempts);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 num)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetMaximumFailedPasswordsForWipe(admin, num, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* maximum)
{
    VALIDATE_NOT_NULL(maximum);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetMaximumFailedPasswordsForWipe(admin, myUserId, maximum);
}

ECode CDevicePolicyManager::GetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* maximum)
{
    VALIDATE_NOT_NULL(maximum);
    *maximum = 0;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->GetMaximumFailedPasswordsForWipe(admin, myUserId, maximum);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::ResetPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* reset)
{
    VALIDATE_NOT_NULL(reset);
    *reset = FALSE;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->ResetPassword(password, flags, myUserId, reset);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetMaximumTimeToLock(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int64 timeMs)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetMaximumTimeToLock(admin, timeMs, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetMaximumTimeToLock(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetMaximumTimeToLock(admin, myUserId, time);
}

ECode CDevicePolicyManager::GetMaximumTimeToLock(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = 0;

    if (mService != NULL) {
        ECode ec = mService->GetMaximumTimeToLock(admin, userHandle, time);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::LockNow()
{
    if (mService != NULL) {
        ECode ec = mService->LockNow();
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::WipeData(
    /* [in] */ Int32 flags)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->WipeData(flags, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetGlobalProxy(
    /* [in] */ IComponentName* admin,
    /* [in] */ Elastos::Net::IProxy* proxySpec,
    /* [in] */ ArrayOf<String>* exclusionList,
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component);
    *component = NULL;
    VALIDATE_NOT_NULL(admin);
    VALIDATE_NOT_NULL(exclusionList);

    if (proxySpec == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (mService != NULL) {
        AutoPtr<IProxyHelper> helper;
        CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
        AutoPtr<Elastos::Net::IProxy> noPorxy;
        helper->GetNO_PROXY((Elastos::Net::IProxy**)&noPorxy);

        String hostSpec;
        String exclSpec;
        Boolean equals = FALSE;
        IObject::Probe(proxySpec)->Equals(noPorxy, &equals);
        if (equals) {
            hostSpec = NULL;
            exclSpec = NULL;
        }
        else {
            ProxyType specType;
            proxySpec->GetType(&specType);
            if (specType == Elastos::Net::ProxyType_HTTP) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<IInetSocketAddress> sa;
            proxySpec->GetAddress((ISocketAddress**)&sa);

            String hostName;
            Int32 port;
            sa->GetHostName(&hostName);
            sa->GetPort(&port);

            StringBuilder sb(hostName);
            sb += ":";
            sb += StringUtils::ToString(port);
            hostSpec = sb.ToString();
            if (exclusionList == NULL) {
                exclSpec = "";
            }
            else {
                sb.Reset();
                Boolean firstDomain = TRUE;

                Int32 size = exclusionList->GetLength();
                for (Int32 i = 0; i < size; ++i) {
                    String exclDomain = (*exclusionList)[i];
                    if (!firstDomain) {
                        sb += ",";
                    }
                    else {
                        firstDomain = FALSE;
                    }
                    sb += exclDomain.Trim();
                }
                exclSpec = sb.ToString();
            }

            AutoPtr<Elastos::Droid::Net::IProxy> netProxy;
            Elastos::Droid::Net::CProxy::AcquireSingleton((Elastos::Droid::Net::IProxy**)&netProxy);
            Int32 result;
            netProxy->Validate(hostName, StringUtils::ToString(port), exclSpec, &result);
            if (result != Elastos::Droid::Net::IProxy::PROXY_VALID) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        Int32 myUserId;
        AutoPtr<IUserHandleHelper> uhelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhelper);
        uhelper->GetMyUserId(&myUserId);
        AutoPtr<IComponentName> comp;
        ECode ec = mService->SetGlobalProxy(admin, hostSpec, exclSpec, myUserId, (IComponentName**)&comp);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetRecommendedGlobalProxy(
    /* [in] */ IComponentName* admin,
    /* [in] */ IProxyInfo* proxyInfo);
{
    if (mService != NULL) {
        // try {
            return mService->SetRecommendedGlobalProxy(admin, proxyInfo);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "Failed talking with device policy service", e);
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetGlobalProxyAdmin(
    /* [out] */ IComponentName** global)
{
    VALIDATE_NOT_NULL(global);
    *global = NULL;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->GetGlobalProxyAdmin(myUserId, global);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetStorageEncryption(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean encrypt,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetStorageEncryption(admin, encrypt, myUserId, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetStorageEncryption(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->GetStorageEncryption(admin, myUserId, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetStorageEncryptionStatus(
    /* [out] */ Int32* st)
{
    VALIDATE_NOT_NULL(st);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetStorageEncryptionStatus(myUserId, st);
}

ECode CDevicePolicyManager::GetStorageEncryptionStatus(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* st)
{
    VALIDATE_NOT_NULL(st);
    *st = IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;

    if (mService != NULL) {
        ECode ec = mService->GetStorageEncryptionStatus(userHandle, st);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::InstallCaCert(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<Byte>* certBuffer,
    /* [out] */ Booelan* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        // try {
            return mService->InstallCaCert(admin, certBuffer, result);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "Failed talking with device policy service", e);
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::UninstallCaCert(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<Byte>* certBuffer)
{
    ECode ec = NOERROR;
    if (mService != NULL) {
        // try {
        String alias;
        GetCaCertAlias(certBuffer, &alias);
        ec = mService->UninstallCaCert(admin, alias);
        if (ec == (ECode)E_CERTIFICATE_EXCEPTION) {
            Logger::W(TAG, "Unable to parse certificate: CertificateException");
            ec = NOERROR;
        }
        else if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "Failed talking with device policy service RemoteException");
            ec = NOERROR;
        }
        // } catch (CertificateException e) {
        //     Log.w(TAG, "Unable to parse certificate", e);
        // } catch (RemoteException e) {
        //     Log.w(TAG, "Failed talking with device policy service", e);
        // }
    }
    return ec;
}

ECode CDevicePolicyManager::GetInstalledCaCerts(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    assert(0 && "TODO");
    List<byte[]> certs = new ArrayList<byte[]>();
    if (mService != null) {
        try {
            mService.enforceCanManageCaCerts(admin);
            final TrustedCertificateStore certStore = new TrustedCertificateStore();
            for (String alias : certStore.userAliases()) {
                try {
                    certs.add(certStore.getCertificate(alias).getEncoded());
                } catch (CertificateException ce) {
                    Log.w(TAG, "Could not encode certificate: " + alias, ce);
                }
            }
        } catch (RemoteException re) {
            Log.w(TAG, "Failed talking with device policy service", re);
        }
    }
    // return certs;
    return NOERROR;
}

ECode CDevicePolicyManager::UninstallAllUserCaCerts(
    /* [in] */ IComponentName* admin)
{
    if (mService != null) {
        for (String alias : new TrustedCertificateStore().userAliases()) {
            try {
                mService.uninstallCaCert(admin, alias);
            } catch (RemoteException re) {
                Log.w(TAG, "Failed talking with device policy service", re);
            }
        }
    }
}

ECode CDevicePolicyManager::HasCaCertInstalled(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<Byte>* certBuffer,
    /* [out] */ Boolean* result)
{
    if (mService != NULL) {
        try {
            mService->EnforceCanManageCaCerts(admin);
            return GetCaCertAlias(certBuffer) != null;
        } catch (RemoteException re) {
            Log.w(TAG, "Failed talking with device policy service", re);
        } catch (CertificateException ce) {
            Log.w(TAG, "Could not parse certificate", ce);
        }
    }
    // return false;
}

ECode CDevicePolicyManager::InstallKeyPair(
    /* [in] */ IComponentName* who,
    /* [in] */ IPrivateKey* privKey,
    /* [in] */ ICertificate* cert,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    try {
        final byte[] pemCert = Credentials.convertToPem(cert);
        return mService.installKeyPair(who, privKey.getEncoded(), pemCert, alias);
    } catch (CertificateException e) {
        Log.w(TAG, "Error encoding certificate", e);
    } catch (IOException e) {
        Log.w(TAG, "Error writing certificate", e);
    } catch (RemoteException e) {
        Log.w(TAG, "Failed talking with device policy service", e);
    }
    return false;
}

ECode CDevicePolicyManager::GetCaCertAlias(
    /* [in] */ Arrayof<Byte>* certBuffer,
    /* [out] */ String* alias)
{
    final CertificateFactory certFactory = CertificateFactory.getInstance("X.509");
    final X509Certificate cert = (X509Certificate) certFactory.generateCertificate(
                          new ByteArrayInputStream(certBuffer));
    return new TrustedCertificateStore().getCertificateAlias(cert);
}

ECode CDevicePolicyManager::SetCameraDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean disabled)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetCameraDisabled(admin, disabled, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetCameraDisabled(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* disabled)
{
    VALIDATE_NOT_NULL(disabled);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetCameraDisabled(admin, myUserId, disabled);
}

ECode CDevicePolicyManager::GetCameraDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* disabled)
{
    VALIDATE_NOT_NULL(disabled);
    *disabled = FALSE;

    if (mService != NULL) {
        ECode ec = mService->GetCameraDisabled(admin, userHandle, disabled);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

//=======================

ECode CDevicePolicyManager::SetScreenCaptureDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean disabled)
{
    if (mService != null) {
        try {
            mService.setScreenCaptureDisabled(admin, UserHandle.myUserId(), disabled);
        } catch (RemoteException e) {
            Log.w(TAG, "Failed talking with device policy service", e);
        }
    }
}

ECode CDevicePolicyManager::SetScreenCaptureDisabled(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* disabled)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetScreenCaptureDisabled(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* result)
{
    return getScreenCaptureDisabled(admin, UserHandle.myUserId());
}

ECode CDevicePolicyManager::GetScreenCaptureDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    if (mService != null) {
        try {
            return mService.getScreenCaptureDisabled(admin, userHandle);
        } catch (RemoteException e) {
            Log.w(TAG, "Failed talking with device policy service", e);
        }
    }
    return false;
}

ECode CDevicePolicyManager::SetAutoTimeRequired(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean required)
{
    if (mService != null) {
        try {
            mService.setAutoTimeRequired(admin, UserHandle.myUserId(), required);
        } catch (RemoteException e) {
            Log.w(TAG, "Failed talking with device policy service", e);
        }
    }
}

ECode CDevicePolicyManager::GetAutoTimeRequired(
    /* [out] */ Boolean* result)
{
    if (mService != null) {
        try {
            return mService.getAutoTimeRequired();
        } catch (RemoteException e) {
            Log.w(TAG, "Failed talking with device policy service", e);
        }
    }
    return false;
}

//=======================

ECode CDevicePolicyManager::SetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 which)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->SetKeyguardDisabledFeatures(admin, which, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* which)
{
    VALIDATE_NOT_NULL(which);
    Int32 myUserId;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetMyUserId(&myUserId);
    return GetKeyguardDisabledFeatures(admin, myUserId, which);
}

ECode CDevicePolicyManager::GetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* which)
{
    VALIDATE_NOT_NULL(which);
    *which = IDevicePolicyManager::KEYGUARD_DISABLE_FEATURES_NONE;

    if (mService != NULL) {
        ECode ec = mService->GetKeyguardDisabledFeatures(admin, userHandle, which);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetActiveAdmin(
    /* [in] */ IComponentName* policyReceiver,
    /* [in] */ Boolean refreshing)
{
    return SetActiveAdmin(policyReceiver,  refreshing, UserHandle::MyUserId());
}

ECode CDevicePolicyManager::SetActiveAdmin(
    /* [in] */ IComponentName* policyReceiver,
    /* [in] */ Boolean refreshing,
    /* [in] */ Int32 userHandle)
{
    if (mService != NULL) {
        ECode ec = mService->SetActiveAdmin(policyReceiver, refreshing, userHandle);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetAdminInfo(
    /* [in] */ IComponentName* cn,
    /* [out] */ IDeviceAdminInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    String cnStr;
    cn->ToString(&cnStr);

    AutoPtr<IPackageManager> pm;
    FAIL_RETURN(mContext->GetPackageManager((IPackageManager**)&pm));
    AutoPtr<IActivityInfo> ai;
    ECode ec = pm->GetReceiverInfo(cn, IPackageManager::GET_META_DATA, (IActivityInfo**)&ai);
    if (FAILED(ec)) {
        // catch (PackageManager.NameNotFoundException e)
        String msg("PackageManager.NameNotFoundException: Unable to retrieve device policy ");
        msg += cnStr;
        Logger::W(TAG, msg);
        return ec;
    }

    AutoPtr<IResolveInfo> ri;
    CResolveInfo::New((IResolveInfo**)&ri);
    ri->SetActivityInfo(ai.Get());

    ec = CDeviceAdminInfo::New(mContext, ri, info);
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        String warn("XmlPullParserException: Unable to parse device policy");
        warn += cnStr;
        Logger::W(TAG, warn);

        *info = NULL;
        return ec;
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        String warn("IOException: Unable to parse device policy");
        warn += cnStr;
        Logger::W(TAG, warn);

        *info = NULL;
        return ec;
    }

    return ec;
}

ECode CDevicePolicyManager::GetRemoveWarning(
    /* [in] */ IComponentName* admin,
    /* [in] */ IRemoteCallback* result)
{
    if (mService != NULL) {
        Int32 myUserId;
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        helper->GetMyUserId(&myUserId);
        ECode ec = mService->GetRemoveWarning(admin, result, myUserId);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetActivePasswordState(
    /* [in] */ Int32 quality,
    /* [in] */ Int32 length,
    /* [in] */ Int32 letters,
    /* [in] */ Int32 uppercase,
    /* [in] */ Int32 lowercase,
    /* [in] */ Int32 numbers,
    /* [in] */ Int32 symbols,
    /* [in] */ Int32 nonletter,
    /* [in] */ Int32 userHandle)
{
    if (mService != NULL) {
        ECode ec = mService->SetActivePasswordState(
                quality, length, letters, uppercase, lowercase,
                numbers, symbols, nonletter, userHandle);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::ReportFailedPasswordAttempt(
    /* [in] */ Int32 userHandle)
{
    if (mService != NULL) {
        ECode ec = mService->ReportFailedPasswordAttempt(userHandle);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::ReportSuccessfulPasswordAttempt(
    /* [in] */ Int32 userHandle)
{
    if (mService != NULL) {
        ECode ec = mService->ReportSuccessfulPasswordAttempt(userHandle);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetDeviceOwner(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetDeviceOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::IsDeviceOwnerApp(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::IsDeviceOwner(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::ClearDeviceOwnerApp(
    /* [in] */ const String& packageName)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetProfileOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetProfileOwner(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetProfileEnabled(
    /* [in] */ IComponentName* admin)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetProfileName(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& profileName)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::IsProfileOwnerApp(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetProfileOwner(
    /* [out]*/ IComponentName** name)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::AddPersistentPreferredActivity(
    /* [in] */ IComponentName* admin,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ IComponentName* activity)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::ClearPackagePersistentPreferredActivities(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetApplicationRestrictions(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [in] */ IBundle* settings)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetTrustAgentFeaturesEnabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* agent,
    /* [in] */ IList* features)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetTrustAgentFeaturesEnabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* agent,
    /* [out] */ IList** result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetCrossProfileCallerIdDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean disabled)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetCrossProfileCallerIdDisabled(
    /* [in] */ IComponentName* who,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::AddCrossProfileIntentFilter(
    /* [in] */ IComponentName* admin,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 flags)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::ClearCrossProfileIntentFilters(
    /* [in] */ IComponentName* admin)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetPermittedAccessibilityServices(
    /* [in] */ IComponentName* admin,
    /* [in] */ IList* packageNames,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetPermittedAccessibilityServices(
    /* [out] */ IComponentName** admin)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetPermittedInputMethods(
    /* [in] */ IComponentName* admin,
    /* [in] */ IList* packageNames,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetPermittedInputMethods(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** list)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::CreateUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& name,
    /* [out] */ IUserHandle** handle)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::CreateAndInitializeUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& name,
    /* [in] */ const String& ownerName,
    /* [in] */ IComponentName* profileOwnerComponent,
    /* [in] */ IBundle* adminExtras)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::RemoveUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ IUserHandle* userHandle)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SwitchUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetApplicationRestrictions(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ IBundle** bundle)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::AddUserRestriction(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& key)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::ClearUserRestriction(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& key)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetApplicationHidden(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean hide,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::IsApplicationHidden(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::EnableSystemApp(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::EnableSystemApp(
    /* [in] */ IComponentName* admin,
    /* [in] */ IIntent* intent,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetAccountManagementDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean disabled)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetAccountTypesWithManagementDisabled(
    /* [out, callee] */ ArrayOf<String>** result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetLockTaskPackages(
    /* [in] */ IComponentName* admin,
    /* [out, callee] */ ArrayOf<String>** packages)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::IsLockTaskPermitted(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetGlobalSetting(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& setting,
    /* [in] */ const String& value)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetSecureSetting(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& setting,
    /* [in] */ const String& value)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetRestrictionsProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* provider)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetMasterVolumeMuted(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean on)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::IsMasterVolumeMuted(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::SetUninstallBlocked(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean uninstallBlocked)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::IsUninstallBlocked(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::AddCrossProfileWidgetProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::RemoveCrossProfileWidgetProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::GetCrossProfileWidgetProviders(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CDevicePolicyManager::RequireSecureKeyguard(
    /* [out] */ Boolean* isRequired)
{
    VALIDATE_NOT_NULL(isRequired);
    return RequireSecureKeyguard(UserHandle::GetMyUserId(), isRequired);
}

ECode CDevicePolicyManager::RequireSecureKeyguard(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* isRequired)
{
    VALIDATE_NOT_NULL(isRequired);
    *isRequired = TRUE;

    ECode ec = NOERROR;
    if (mService != NULL) {
        // try {
        ec = mService->RequireSecureKeyguard(userHandle, isRequired);
        if (FAILED(ec)) {
            Logger::W(TAG, "Failed to get secure keyguard requirement");
        }
        // } catch (RemoteException e) {
        //     Log.w(TAG, "Failed to get secure keyguard requirement");
        // }
    }
    return ec;
}

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos
