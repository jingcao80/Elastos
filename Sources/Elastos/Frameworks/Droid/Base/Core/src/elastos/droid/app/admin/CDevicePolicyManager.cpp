
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/app/admin/CDevicePolicyManager.h"
// #include "elastos/droid/app/admin/CDeviceAdminInfo.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/keystore/security/CCredentialsHelper.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/net/CProxy.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

// using Elastos::Droid::App::Admin::CDeviceAdminInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::KeyStore::Security::CCredentialsHelper;
using Elastos::Droid::KeyStore::Security::ICredentialsHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::EIID_IByte;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Net::ProxyType;
using Elastos::Net::IProxyHelper;
using Elastos::Net::CProxyHelper;
using Elastos::Net::ISocketAddress;
using Elastos::Net::IInetSocketAddress;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IKey;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
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

ECode CDevicePolicyManager::Create(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [out] */ IDevicePolicyManager** policymanager)
{
    VALIDATE_NOT_NULL(policymanager)
    *policymanager = NULL;

    AutoPtr<IDevicePolicyManager> me;
    CDevicePolicyManager::New(context, handler, (IDevicePolicyManager**)&me);
    if (((CDevicePolicyManager*)me.Get())->mService != NULL) {
        *policymanager = me;
        REFCOUNT_ADD(*policymanager);
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsAdminActive(
    /* [in] */ IComponentName* who,
    /* [out] */ Boolean* isAdminActive)
{
    return IsAdminActiveAsUser(who, UserHandle::GetMyUserId(), isAdminActive);
}

ECode CDevicePolicyManager::IsAdminActiveAsUser(
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetActiveAdmins(
    /* [out] */ IList** admins)
{
    return GetActiveAdminsAsUser(UserHandle::GetMyUserId(), admins);
}

ECode CDevicePolicyManager::GetActiveAdminsAsUser(
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
        return NOERROR;
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
        ECode ec = mService->PackageHasActiveAdmins(packageName, UserHandle::GetMyUserId(), hasAdmins);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::RemoveActiveAdmin(
    /* [in] */ IComponentName* who)
{
    if (mService != NULL) {
        ECode ec = mService->RemoveActiveAdmin(who, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
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
        ECode ec = mService->HasGrantedPolicy(admin, usesPolicy, UserHandle::GetMyUserId(), hasGrant);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordQuality(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 quality)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordQuality(admin, quality, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordQuality(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    return GetPasswordQuality(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
     }
     return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumLength(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordMinimumLength(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumLength(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    return GetPasswordMinimumLength(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordMinimumUpperCase(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    return GetPasswordMinimumUpperCase(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordMinimumLowerCase(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    return GetPasswordMinimumLowerCase(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumLetters(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordMinimumLetters(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumLetters(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    return GetPasswordMinimumLetters(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumNumeric(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordMinimumNumeric(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumNumeric(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    return GetPasswordMinimumNumeric(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumSymbols(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordMinimumSymbols(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumSymbols(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    return GetPasswordMinimumSymbols(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordMinimumNonLetter(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    return GetPasswordMinimumNonLetter(admin, UserHandle::GetMyUserId(), password);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordHistoryLength(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 length)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordHistoryLength(admin, length, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPasswordExpirationTimeout(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int64 timeout)
{
    if (mService != NULL) {
        ECode ec = mService->SetPasswordExpirationTimeout(admin, timeout, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
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
        ECode ec = mService->GetPasswordExpirationTimeout(admin, UserHandle::GetMyUserId(), timeout);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
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
        ECode ec = mService->GetPasswordExpiration(admin, UserHandle::GetMyUserId(), expiration);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPasswordHistoryLength(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    return GetPasswordHistoryLength(admin, UserHandle::GetMyUserId(), length);
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
        return NOERROR;
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
        ECode ec = mService->IsActivePasswordSufficient(UserHandle::GetMyUserId(), isSufficient);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetCurrentFailedPasswordAttempts(
    /* [out] */ Int32* atempts)
{
    VALIDATE_NOT_NULL(atempts);
    *atempts = -1;

    if (mService != NULL) {
        ECode ec = mService->GetCurrentFailedPasswordAttempts(UserHandle::GetMyUserId(), atempts);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 num)
{
    if (mService != NULL) {
        ECode ec = mService->SetMaximumFailedPasswordsForWipe(admin, num, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* maximum)
{
    VALIDATE_NOT_NULL(maximum);
    return GetMaximumFailedPasswordsForWipe(admin, UserHandle::GetMyUserId(), maximum);
}

ECode CDevicePolicyManager::GetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* maximum)
{
    VALIDATE_NOT_NULL(maximum);
    *maximum = 0;

    if (mService != NULL) {
        ECode ec = mService->GetMaximumFailedPasswordsForWipe(admin, userHandle, maximum);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
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
        ECode ec = mService->ResetPassword(password, flags, UserHandle::GetMyUserId(), reset);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetMaximumTimeToLock(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int64 timeMs)
{
    if (mService != NULL) {
        ECode ec = mService->SetMaximumTimeToLock(admin, timeMs, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetMaximumTimeToLock(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    return GetMaximumTimeToLock(admin, UserHandle::GetMyUserId(), time);
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
        return NOERROR;
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
    }
    return NOERROR;
}

ECode CDevicePolicyManager::WipeData(
    /* [in] */ Int32 flags)
{
    if (mService != NULL) {
        ECode ec = mService->WipeData(flags, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetGlobalProxy(
    /* [in] */ IComponentName* admin,
    /* [in] */ Elastos::Net::IProxy* proxySpec,
    /* [in] */ IList* exclusionList,
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
            hostSpec = String(NULL);
            exclSpec = String(NULL);
        }
        else {
            ProxyType specType;
            proxySpec->GetType(&specType);
            if (specType != Elastos::Net::ProxyType_HTTP) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr<ISocketAddress> isa;
            proxySpec->GetAddress((ISocketAddress**)&isa);
            IInetSocketAddress* sa = IInetSocketAddress::Probe(isa);
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

                Int32 size;
                exclusionList->GetSize(&size);
                for (Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> obj;
                    exclusionList->Get(i, (IInterface**)&obj);
                    String exclDomain;
                    ICharSequence::Probe(obj)->ToString(&exclDomain);
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

        ECode ec = mService->SetGlobalProxy(admin, hostSpec, exclSpec, UserHandle::GetMyUserId(), component);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetRecommendedGlobalProxy(
    /* [in] */ IComponentName* admin,
    /* [in] */ IProxyInfo* proxyInfo)
{
    if (mService != NULL) {
        ECode ec = mService->SetRecommendedGlobalProxy(admin, proxyInfo);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetGlobalProxyAdmin(
    /* [out] */ IComponentName** global)
{
    VALIDATE_NOT_NULL(global);
    *global = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetGlobalProxyAdmin(UserHandle::GetMyUserId(), global);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
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
        ECode ec = mService->SetStorageEncryption(admin, encrypt, UserHandle::GetMyUserId(), result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
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
        ECode ec = mService->GetStorageEncryption(admin, UserHandle::GetMyUserId(), result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetStorageEncryptionStatus(
    /* [out] */ Int32* st)
{
    VALIDATE_NOT_NULL(st);
    return GetStorageEncryptionStatus(UserHandle::GetMyUserId(), st);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::InstallCaCert(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<Byte>* certBuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->InstallCaCert(admin, certBuffer, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::UninstallCaCert(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<Byte>* certBuffer)
{
    ECode ec = NOERROR;
    if (mService != NULL) {
        String alias;
        GetCaCertAlias(certBuffer, &alias);
        ec = mService->UninstallCaCert(admin, alias);
        if (ec == (ECode)E_CERTIFICATE_EXCEPTION) {
            Logger::W(TAG, "E_CERTIFICATE_EXCEPTION: Unable to parse certificate");
        }
        else if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetInstalledCaCerts(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    ECode ec = NOERROR;
    AutoPtr<IList> certs; // List<byte[]>
    CArrayList::New((IList**)&certs);

    if (mService != NULL) {
        // try {
            ec = mService->EnforceCanManageCaCerts(admin);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
                *list = certs;
                REFCOUNT_ADD(*list)
                return NOERROR;
            }
            assert(0 && "TODO");
            // AutoPtr<ITrustedCertificateStore> certStore;
            // CTrustedCertificateStore::New((ITrustedCertificateStore**)&certStore);
            // AutoPtr<ISet> set;
            // certStore->UserAliases((ISet**)&set);
            // AutoPtr<IIterator> iterator;
            // set->GetIterator((IIterator**)&iterator);
            // Boolean res;
            // while (iterator->HasNext(&res), res) {
            //     AutoPtr<IInterface> obj;
            //     iterator->GetNext((IInterface**)&obj);
            //     String alias;
            //     ICharSequence::Probe(obj)->ToString(&alias);

            //     // try {
            //         AutoPtr<ICertificate> certificate;
            //         certStore->GetCertificate(alias, (ICertificate**)&certificate);
            //         AutoPtr< ArrayOf<Byte> > encoded;
            //         certificate->GetEncoded((ArrayOf<Byte>**)&encoded);
            //         Int32 length = encoded->GetLength();
            //         AutoPtr<IArrayOf> array;
            //         CArrayOf::New(EIID_IByte, length, (IArrayOf**)&array);
            //         for (Int32 i = 0; i < length, ++i) {
            //             array->Set(i, CoreUtils::ConvertByte((*encoded)[i]));
            //         }
            //         certs->Add(array);
            //     // } catch (CertificateException ce) {
            //     //     Log.w(TAG, "Could not encode certificate: " + alias, ce);
            //     // }
            // }
        // } catch (RemoteException re) {
        //     Log.w(TAG, "Failed talking with device policy service", re);
        // }
    }
    *list = certs;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode CDevicePolicyManager::UninstallAllUserCaCerts(
    /* [in] */ IComponentName* admin)
{
    if (mService != NULL) {
        assert(0 && "TODO");
        // AutoPtr<ITrustedCertificateStore> certStore;
        // CTrustedCertificateStore::New((ITrustedCertificateStore**)&certStore);
        // AutoPtr<ISet> set;
        // certStore->UserAliases((ISet**)&set);
        // AutoPtr<IIterator> iterator;
        // set->GetIterator((IIterator**)&iterator);
        // Boolean res;
        // while (iterator->HasNext(&res), res) {
        //     AutoPtr<IInterface> obj;
        //     iterator->GetNext((IInterface**)&obj);
        //     String alias;
        //     ICharSequence::Probe(obj)->ToString(&alias);

        //     ECode ec = mService->UninstallCaCert(admin, alias);
        //     if (FAILED(ec)) {
        //         Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        //     }
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::HasCaCertInstalled(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<Byte>* certBuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->EnforceCanManageCaCerts(admin);
        if (SUCCEEDED(ec)) {
            String str;
            GetCaCertAlias(certBuffer, &str);
            *result = !str.IsNull();
            return NOERROR;
        }
        else if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        else if (ec == (ECode)E_CERTIFICATE_EXCEPTION) {
            Logger::W(TAG, "E_CERTIFICATE_EXCEPTION: Could not parse certificate");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::InstallKeyPair(
    /* [in] */ IComponentName* who,
    /* [in] */ IPrivateKey* privKey,
    /* [in] */ ICertificate* cert,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // try {
    AutoPtr<ICredentialsHelper> hlp;
    CCredentialsHelper::AcquireSingleton((ICredentialsHelper**)&hlp);
    AutoPtr<ArrayOf<ICertificate*> > certs = ArrayOf<ICertificate*>::Alloc(1);
    certs->Set(0, cert);
    AutoPtr<ArrayOf<Byte> > pemCert;
    hlp->ConvertToPem(certs, (ArrayOf<Byte>**)&pemCert);

    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(privKey)->GetEncoded((ArrayOf<Byte>**)&encoded);
    return mService->InstallKeyPair(who, encoded, pemCert, alias, result);
    // } catch (CertificateException e) {
    //     Log.w(TAG, "Error encoding certificate", e);
    // } catch (IOException e) {
    //     Log.w(TAG, "Error writing certificate", e);
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Failed talking with device policy service", e);
    // }
}

ECode CDevicePolicyManager::GetCaCertAlias(
    /* [in] */ ArrayOf<Byte>* certBuffer,
    /* [out] */ String* alias)
{
    VALIDATE_NOT_NULL(alias)
    *alias = String(NULL);

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> certFactory;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);
    AutoPtr<IInputStream> bais;
    CByteArrayInputStream::New(certBuffer, (IInputStream**)&bais);

    AutoPtr<ICertificate> certificate;
    certFactory->GenerateCertificate(bais, (ICertificate**)&certificate);
    IX509Certificate* cert = IX509Certificate::Probe(certificate);
    assert(0 && "TODO");
    // AutoPtr<ITrustedCertificateStore> tcs;
    // CTrustedCertificateStore::New((ITrustedCertificateStore**)&tcs);
    // return tcs->GetCertificateAlias(cert, &alias);
    return NOERROR;
}

ECode CDevicePolicyManager::SetCameraDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean disabled)
{
    if (mService != NULL) {
        ECode ec = mService->SetCameraDisabled(admin, disabled, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetCameraDisabled(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* disabled)
{
    VALIDATE_NOT_NULL(disabled);
    return GetCameraDisabled(admin, UserHandle::GetMyUserId(), disabled);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetScreenCaptureDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean disabled)
{
    if (mService != NULL) {
        ECode ec = mService->SetScreenCaptureDisabled(admin, UserHandle::GetMyUserId(), disabled);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetScreenCaptureDisabled(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* result)
{
    return GetScreenCaptureDisabled(admin, UserHandle::GetMyUserId(), result);
}

ECode CDevicePolicyManager::GetScreenCaptureDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->GetScreenCaptureDisabled(admin, userHandle, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetAutoTimeRequired(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean required)
{
    if (mService != NULL) {
        ECode ec = mService->SetAutoTimeRequired(admin, UserHandle::GetMyUserId(), required);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetAutoTimeRequired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->GetAutoTimeRequired(result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* admin,
    /* [in] */ Int32 which)
{
    if (mService != NULL) {
        ECode ec = mService->SetKeyguardDisabledFeatures(admin, which, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* admin,
    /* [out] */ Int32* which)
{
    VALIDATE_NOT_NULL(which);
    return GetKeyguardDisabledFeatures(admin, UserHandle::GetMyUserId(), which);
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
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetActiveAdmin(
    /* [in] */ IComponentName* policyReceiver,
    /* [in] */ Boolean refreshing)
{
    return SetActiveAdmin(policyReceiver,  refreshing, UserHandle::GetMyUserId());
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
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetAdminInfo(
    /* [in] */ IComponentName* cn,
    /* [out] */ IDeviceAdminInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    AutoPtr<IPackageManager> pm;
    FAIL_RETURN(mContext->GetPackageManager((IPackageManager**)&pm));
    AutoPtr<IActivityInfo> ai;
    ECode ec = pm->GetReceiverInfo(cn, IPackageManager::GET_META_DATA, (IActivityInfo**)&ai);
    if (FAILED(ec)) {
        // catch (PackageManager.NameNotFoundException e)
        Logger::W(TAG, "PackageManager.NameNotFoundException: Unable to retrieve device policy %s", TO_CSTR(cn));
        return NOERROR;
    }

    AutoPtr<IResolveInfo> ri;
    CResolveInfo::New((IResolveInfo**)&ri);
    ri->SetActivityInfo(ai.Get());

    assert(0 && "TODO");
    // ec = CDeviceAdminInfo::New(mContext, ri, info);
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Logger::W(TAG, "XmlPullParserException: Unable to parse device policy %s", TO_CSTR(cn));
        *info = NULL;
        return NOERROR;
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "IOException: Unable to parse device policy %s", TO_CSTR(cn));
        *info = NULL;
        return NOERROR;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::GetRemoveWarning(
    /* [in] */ IComponentName* admin,
    /* [in] */ IRemoteCallback* result)
{
    if (mService != NULL) {
        ECode ec = mService->GetRemoveWarning(admin, result, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
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
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetDeviceOwner(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    return SetDeviceOwner(packageName, String(NULL), result);
}

ECode CDevicePolicyManager::SetDeviceOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->SetDeviceOwner(packageName, ownerName, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to set device owner");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsDeviceOwnerApp(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->IsDeviceOwner(packageName, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to check device owner");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsDeviceOwner(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    return IsDeviceOwnerApp(packageName, result);
}

ECode CDevicePolicyManager::ClearDeviceOwnerApp(
    /* [in] */ const String& packageName)
{
    if (mService != NULL) {
        ECode ec = mService->ClearDeviceOwner(packageName);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to clear device owner");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetDeviceOwner(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (mService != NULL) {
        ECode ec = mService->GetDeviceOwner(result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to get device owner");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetDeviceOwnerName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (mService != NULL) {
        ECode ec = mService->GetDeviceOwnerName(result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to get device owner name");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetActiveProfileOwner(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& ownerName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        Int32 myUserId = UserHandle::GetMyUserId();
        ECode ec = mService->SetActiveAdmin(admin, FALSE, myUserId);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to set profile owner");
            Logger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: Couldn't set profile owner.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        ec = mService->SetProfileOwner(admin, ownerName, myUserId, result);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to set profile owner");
            Logger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: Couldn't set profile owner.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::ClearProfileOwner(
    /* [in] */ IComponentName* admin)
{
    if (mService != NULL) {
        ECode ec = mService->ClearProfileOwner(admin);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to clear profile owner %s", TO_CSTR(admin));
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::HasUserSetupCompleted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;

    if (mService != NULL) {
        ECode ec = mService->HasUserSetupCompleted(result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to check if user setup has completed");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetProfileOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (packageName.IsNull()) {
        Logger::E(TAG, "E_NULL_POINTER_EXCEPTION: packageName cannot be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, String(""), (IComponentName**)&cn);

    return SetProfileOwner(cn, ownerName, userHandle, result);
}

ECode CDevicePolicyManager::SetProfileOwner(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String str = ownerName;

    if (admin == NULL) {
        Logger::E(TAG, "E_NULL_POINTER_EXCEPTION: admin cannot be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mService != NULL) {
        if (str.IsNull()) {
            str = "";
        }
        ECode ec = mService->SetProfileOwner(admin, str, userHandle, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to set profile owner");
            Logger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: Couldn't set profile owner.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }

    return NOERROR;
}

ECode CDevicePolicyManager::SetProfileEnabled(
    /* [in] */ IComponentName* admin)
{
    if (mService != NULL) {
        ECode ec = mService->SetProfileEnabled(admin);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetProfileName(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& profileName)
{
    if (mService != NULL) {
        ECode ec = mService->SetProfileName(who, profileName);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsProfileOwnerApp(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        AutoPtr<IUserHandle> handle;
        Process::MyUserHandle((IUserHandle**)&handle);
        Int32 id;
        handle->GetIdentifier(&id);
        AutoPtr<IComponentName> profileOwner;
        ECode ec = mService->GetProfileOwner(id, (IComponentName**)&profileOwner);
        if (SUCCEEDED(ec)) {
            if (profileOwner != NULL) {
                String name;
                profileOwner->GetPackageName(&name);
                *result = name.Equals(packageName);
                return NOERROR;
            }
        }
        else if (ec == (ECode) E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to check profile owner");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetProfileOwner(
    /* [out]*/ IComponentName** name)
{
    VALIDATE_NOT_NULL(name)

    AutoPtr<IUserHandle> handle;
    Process::MyUserHandle((IUserHandle**)&handle);
    Int32 id;
    handle->GetIdentifier(&id);
    return GetProfileOwnerAsUser(id, name);
}

ECode CDevicePolicyManager::GetProfileOwnerAsUser(
    /* [in] */ Int32 userId,
    /* [out]*/ IComponentName** name)
{
    VALIDATE_NOT_NULL(name)
    *name = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetProfileOwner(userId, name);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to get profile owner");
            Logger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: Requested profile owner for invalid userId");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetProfileOwnerName(
    /* [out]*/ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (mService != NULL) {
        AutoPtr<IUserHandle> handle;
        Process::MyUserHandle((IUserHandle**)&handle);
        Int32 id;
        handle->GetIdentifier(&id);
        ECode ec = mService->GetProfileOwnerName(id, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to get profile owner");
            Logger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: Requested profile owner for invalid userId");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetProfileOwnerNameAsUser(
    /* [in] */ Int32 userId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (mService != NULL) {
        ECode ec = mService->GetProfileOwnerName(userId, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to get profile owner");
            Logger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: Requested profile owner for invalid userId");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::AddPersistentPreferredActivity(
    /* [in] */ IComponentName* admin,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ IComponentName* activity)
{
    if (mService != NULL) {
        ECode ec = mService->AddPersistentPreferredActivity(admin, filter, activity);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::ClearPackagePersistentPreferredActivities(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName)
{
    if (mService != NULL) {
        ECode ec = mService->ClearPackagePersistentPreferredActivities(admin, packageName);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetApplicationRestrictions(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [in] */ IBundle* settings)
{
    if (mService != NULL) {
        ECode ec = mService->SetApplicationRestrictions(admin, packageName, settings);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetTrustAgentFeaturesEnabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* agent,
    /* [in] */ IList* features)
{
    if (mService != NULL) {
        ECode ec = mService->SetTrustAgentFeaturesEnabled(admin, agent, features, UserHandle::GetMyUserId());
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetTrustAgentFeaturesEnabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* agent,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    if (mService != NULL) {
        ECode ec = mService->GetTrustAgentFeaturesEnabled(admin, agent, UserHandle::GetMyUserId(), result);
        if (SUCCEEDED(ec)) {
            return NOERROR;
        }
        else if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }

    return CArrayList::New(result); // empty list
}

ECode CDevicePolicyManager::SetCrossProfileCallerIdDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean disabled)
{
    if (mService != NULL) {
        ECode ec = mService->SetCrossProfileCallerIdDisabled(who, disabled);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetCrossProfileCallerIdDisabled(
    /* [in] */ IComponentName* who,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->GetCrossProfileCallerIdDisabled(who, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetCrossProfileCallerIdDisabled(
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        Int32 id;
        userHandle->GetIdentifier(&id);
        ECode ec = mService->GetCrossProfileCallerIdDisabledForUser(id, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::AddCrossProfileIntentFilter(
    /* [in] */ IComponentName* admin,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 flags)
{
    if (mService != NULL) {
        ECode ec = mService->AddCrossProfileIntentFilter(admin, filter, flags);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::ClearCrossProfileIntentFilters(
    /* [in] */ IComponentName* admin)
{
    if (mService != NULL) {
        ECode ec = mService->ClearCrossProfileIntentFilters(admin);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPermittedAccessibilityServices(
    /* [in] */ IComponentName* admin,
    /* [in] */ IList* packageNames,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->SetPermittedAccessibilityServices(admin, packageNames, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPermittedAccessibilityServices(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetPermittedAccessibilityServices(admin, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPermittedAccessibilityServices(
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetPermittedAccessibilityServicesForUser(userId, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetPermittedInputMethods(
    /* [in] */ IComponentName* admin,
    /* [in] */ IList* packageNames,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->SetPermittedInputMethods(admin, packageNames, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPermittedInputMethods(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetPermittedInputMethods(admin, list);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetPermittedInputMethodsForCurrentUser(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetPermittedInputMethodsForCurrentUser(list);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::CreateUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& name,
    /* [out] */ IUserHandle** handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = NULL;

    ECode ec = mService->CreateUser(admin, name, handle);
    if (FAILED(ec)) {
        Logger::W(TAG, "E_REMOTE_EXCEPTION: Could not create a user");
    }
    return NOERROR;
}

ECode CDevicePolicyManager::CreateAndInitializeUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& name,
    /* [in] */ const String& ownerName,
    /* [in] */ IComponentName* profileOwnerComponent,
    /* [in] */ IBundle* adminExtras,
    /* [out] */ IUserHandle** handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = NULL;

    ECode ec = mService->CreateAndInitializeUser(admin, name, ownerName, profileOwnerComponent, adminExtras, handle);
    if (FAILED(ec)) {
        Logger::W(TAG, "E_REMOTE_EXCEPTION: Could not create a user");
    }
    return NOERROR;
}

ECode CDevicePolicyManager::RemoveUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ECode ec = mService->RemoveUser(admin, userHandle, result);
    if (FAILED(ec)) {
        Logger::W(TAG, "E_REMOTE_EXCEPTION: Could not remove user");
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SwitchUser(
    /* [in] */ IComponentName* admin,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    ECode ec = mService->SwitchUser(admin, userHandle, result);
    if (FAILED(ec)) {
        Logger::W(TAG, "E_REMOTE_EXCEPTION: Could not remove user");
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetApplicationRestrictions(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetApplicationRestrictions(admin, packageName, bundle);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::AddUserRestriction(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& key)
{
    if (mService != NULL) {
        ECode ec = mService->SetUserRestriction(admin, key, TRUE);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::ClearUserRestriction(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& key)
{
    if (mService != NULL) {
        ECode ec = mService->SetUserRestriction(admin, key, FALSE);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetApplicationHidden(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean hide,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->SetApplicationHidden(admin, packageName, hide, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsApplicationHidden(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->IsApplicationHidden(admin, packageName, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::EnableSystemApp(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName)
{
    if (mService != NULL) {
        ECode ec = mService->EnableSystemApp(admin, packageName);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to install package: %s", packageName.string());
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::EnableSystemApp(
    /* [in] */ IComponentName* admin,
    /* [in] */ IIntent* intent,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (mService != NULL) {
        ECode ec = mService->EnableSystemAppWithIntent(admin, intent, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to install packages matching filter: %s", TO_CSTR(intent));
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetAccountManagementDisabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean disabled)
{
    if (mService != NULL) {
        ECode ec = mService->SetAccountManagementDisabled(admin, accountType, disabled);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetAccountTypesWithManagementDisabled(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return GetAccountTypesWithManagementDisabledAsUser(UserHandle::GetMyUserId(), result);
}

ECode CDevicePolicyManager::GetAccountTypesWithManagementDisabledAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    if (mService != NULL) {
        ECode ec = mService->GetAccountTypesWithManagementDisabledAsUser(userId, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetLockTaskPackages(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<String>* packages)
{
    if (mService != NULL) {
        ECode ec = mService->SetLockTaskPackages(admin, packages);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetLockTaskPackages(
    /* [in] */ IComponentName* admin,
    /* [out, callee] */ ArrayOf<String>** packages)
{
    VALIDATE_NOT_NULL(packages)
    *packages = NULL;

    if (mService != NULL) {
        ECode ec = mService->GetLockTaskPackages(admin, packages);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsLockTaskPermitted(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->IsLockTaskPermitted(pkg, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetGlobalSetting(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& setting,
    /* [in] */ const String& value)
{
    if (mService != NULL) {
        ECode ec = mService->SetGlobalSetting(admin, setting, value);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetSecureSetting(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& setting,
    /* [in] */ const String& value)
{
    if (mService != NULL) {
        ECode ec = mService->SetSecureSetting(admin, setting, value);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed talking with device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetRestrictionsProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* provider)
{
    if (mService != NULL) {
        ECode ec = mService->SetRestrictionsProvider(admin, provider);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to set permission provider on device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetMasterVolumeMuted(
    /* [in] */ IComponentName* admin,
    /* [in] */ Boolean on)
{
    if (mService != NULL) {
        ECode ec = mService->SetMasterVolumeMuted(admin, on);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to setMasterMute on device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsMasterVolumeMuted(
    /* [in] */ IComponentName* admin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->IsMasterVolumeMuted(admin, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to get isMasterMute on device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::SetUninstallBlocked(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean uninstallBlocked)
{
    if (mService != NULL) {
        ECode ec = mService->SetUninstallBlocked(admin, packageName, uninstallBlocked);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to call block uninstall on device policy service");
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManager::IsUninstallBlocked(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->IsUninstallBlocked(admin, packageName, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to call block uninstall on device policy service");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::AddCrossProfileWidgetProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->AddCrossProfileWidgetProvider(admin, packageName, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Error calling addCrossProfileWidgetProvider");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::RemoveCrossProfileWidgetProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService != NULL) {
        ECode ec = mService->RemoveCrossProfileWidgetProvider(admin, packageName, result);
        if (FAILED(ec)) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Error calling removeCrossProfileWidgetProvider");
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManager::GetCrossProfileWidgetProviders(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mService != NULL) {
        AutoPtr<IList> providers;
        ECode ec = mService->GetCrossProfileWidgetProviders(admin, (IList**)&providers);
        if (SUCCEEDED(ec)) {
            if (providers != NULL) {
                *result = providers;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
        else if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Error calling getCrossProfileWidgetProviders");
        }
    }

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    return coll->GetEmptyList(result);
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
            Logger::W(TAG, "E_REMOTE_EXCEPTION: Failed to get secure keyguard requirement");
        }
        // } catch (RemoteException e) {
        //     Log.w(TAG, "Failed to get secure keyguard requirement");
        // }
    }
    return NOERROR;
}

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos
