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

#include "Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/TelecomManager.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// TelecomManager::
//===============================================================
String TelecomManager::TAG("TelecomManager");

CAR_INTERFACE_IMPL(TelecomManager, Object, ITelecomManager)

ECode TelecomManager::From(
    /* [in] */ IContext* context,
    /* [out] */ ITelecomManager** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> serv;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&serv);
    *result = ITelecomManager::Probe(serv);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomManager::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> appContext;
    context->GetApplicationContext((IContext**)&appContext);
    if (appContext != NULL) {
        mContext = appContext;
    }
    else {
        mContext = context;
    }
    return NOERROR;
}

ECode TelecomManager::GetDefaultOutgoingPhoneAccount(
    /* [in] */ const String& uriScheme,
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetDefaultOutgoingPhoneAccount(uriScheme, result);
    }

    *result = NULL;
    return NOERROR;
}

ECode TelecomManager::GetUserSelectedOutgoingPhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetUserSelectedOutgoingPhoneAccount(result);
    }

    *result = NULL;
    return NOERROR;
}

ECode TelecomManager::SetUserSelectedOutgoingPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->SetUserSelectedOutgoingPhoneAccount(accountHandle);
    }
    return NOERROR;
}

ECode TelecomManager::GetSimCallManager(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetSimCallManager(result);
    }

    *result = NULL;
    return NOERROR;
}

ECode TelecomManager::SetSimCallManager(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->SetSimCallManager(accountHandle);
    }
    return NOERROR;
}

ECode TelecomManager::GetSimCallManagers(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetSimCallManagers(result);
    }

    AutoPtr<IList> res;
    CArrayList::New((IList**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomManager::GetConnectionManager(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    return GetSimCallManager(result);
}

ECode TelecomManager::GetPhoneAccountsSupportingScheme(
    /* [in] */ const String& uriScheme,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetPhoneAccountsSupportingScheme(uriScheme, result);
    }

    AutoPtr<IList> res;
    CArrayList::New((IList**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomManager::GetCallCapablePhoneAccounts(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetCallCapablePhoneAccounts(result);
    }

    AutoPtr<IList> res;
    CArrayList::New((IList**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomManager::HasMultipleCallCapableAccounts(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IList> l;
    GetCallCapablePhoneAccounts((IList**)&l);
    Int32 size = 0;
    l->GetSize(&size);
    *result = size > 1;
    return NOERROR;
}

ECode TelecomManager::GetPhoneAccountsForPackage(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        String pakName;
        mContext->GetPackageName(&pakName);
        return ts->GetPhoneAccountsForPackage(pakName, result);
    }

    *result = NULL;
    return NOERROR;
}

ECode TelecomManager::GetPhoneAccount(
    /* [in] */ IPhoneAccountHandle* account,
    /* [out] */ IPhoneAccount** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetPhoneAccount(account, result);
    }

    *result = NULL;
    return NOERROR;
}

ECode TelecomManager::GetAllPhoneAccountsCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetAllPhoneAccountsCount(result);
    }

    *result = 0;
    return NOERROR;
}

ECode TelecomManager::GetAllPhoneAccounts(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetAllPhoneAccounts(result);
    }

    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    return cls->GetEmptyList(result);
}

ECode TelecomManager::GetAllPhoneAccountHandles(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetAllPhoneAccountHandles(result);
    }

    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    return cls->GetEmptyList(result);
}

ECode TelecomManager::RegisterPhoneAccount(
    /* [in] */ IPhoneAccount* account)
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        ts->RegisterPhoneAccount(account);
    }
    return NOERROR;
}

ECode TelecomManager::UnregisterPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        ts->UnregisterPhoneAccount(accountHandle);
    }
    return NOERROR;
}

ECode TelecomManager::ClearAccounts()
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        String pakName;
        mContext->GetPackageName(&pakName);
        ts->ClearAccounts(pakName);
    }
    return NOERROR;
}

ECode TelecomManager::GetDefaultPhoneApp(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetDefaultPhoneApp(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode TelecomManager::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->IsInCall(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode TelecomManager::GetCallState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetCallState(result);
    }

    *result = ITelephonyManager::CALL_STATE_IDLE;
    return NOERROR;
}

ECode TelecomManager::IsRinging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->IsRinging(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode TelecomManager::EndCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->EndCall(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode TelecomManager::AcceptRingingCall()
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        ts->AcceptRingingCall();
    }
    return NOERROR;
}

ECode TelecomManager::SilenceRinger()
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        ts->SilenceRinger();
    }
    return NOERROR;
}

ECode TelecomManager::IsTtySupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->IsTtySupported(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode TelecomManager::GetCurrentTtyMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        return ts->GetCurrentTtyMode(result);
    }
    *result = ITelecomManager::TTY_MODE_OFF;
    return NOERROR;
}

ECode TelecomManager::AddNewIncomingCall(
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [in] */ IBundle* extras)
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        AutoPtr<IBundle> bd;
        CBundle::New((IBundle**)&bd);
        ts->AddNewIncomingCall(
                phoneAccount, extras == NULL ? bd.Get() : extras);
    }
    return NOERROR;
}

ECode TelecomManager::AddNewUnknownCall(
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [in] */ IBundle* extras)
{
    if (IsServiceConnected()) {
        AutoPtr<IITelecomService> ts = GetTelecomService();
        AutoPtr<IBundle> bd;
        CBundle::New((IBundle**)&bd);
        ts->AddNewUnknownCall(
                phoneAccount, extras == NULL ? bd.Get() : extras);
    }
    return NOERROR;
}

ECode TelecomManager::HandleMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IITelecomService> service = GetTelecomService();
    if (service != NULL) {
        return service->HandlePinMmi(dialString, result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode TelecomManager::CancelMissedCallsNotification()
{
    AutoPtr<IITelecomService> service = GetTelecomService();
    if (service != NULL) {
        service->CancelMissedCallsNotification();
    }
    return NOERROR;
}

ECode TelecomManager::ShowInCallScreen(
    /* [in] */ Boolean showDialpad)
{
    AutoPtr<IITelecomService> service = GetTelecomService();
    if (service != NULL) {
        service->ShowInCallScreen(showDialpad);
    }
    return NOERROR;
}

AutoPtr<IITelecomService> TelecomManager::GetTelecomService()
{
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> serv;
    sm->GetService(IContext::TELECOM_SERVICE, (IInterface**)&serv);
    return IITelecomService::Probe(serv);
}

Boolean TelecomManager::IsServiceConnected()
{
    Boolean isConnected = GetTelecomService() != NULL;
    if (!isConnected) {
        Logger::W(TAG, "Telecom Service not found.");
    }
    return isConnected;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
