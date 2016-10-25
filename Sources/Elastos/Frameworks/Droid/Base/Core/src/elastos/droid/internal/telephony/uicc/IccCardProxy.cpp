
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/internal/telephony/uicc/IccCardProxy.h"
#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaSubscriptionSourceManagerHelper.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
#include "elastos/droid/telephony/CServiceStateHelper.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::RADIO_UNAVAILABLE;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Internal::Telephony::CSubscriptionControllerHelper;
using Elastos::Droid::Internal::Telephony::ICommandsInterfaceRadioState;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManagerHelper;
using Elastos::Droid::Internal::Telephony::Cdma::CCdmaSubscriptionSourceManagerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::CUiccControllerHelper;
using Elastos::Droid::Telephony::IServiceStateHelper;
using Elastos::Droid::Telephony::CServiceStateHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::R;
using Elastos::Droid::Manifest;

using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             IccCardProxy
//=====================================================================
CAR_INTERFACE_IMPL_2(IccCardProxy, Handler, IIccCard, IIccCardProxy);

const Boolean IccCardProxy::DBG = TRUE;
const String IccCardProxy::LOGTAG("IccCardProxy");
const Int32 IccCardProxy::EVENT_RADIO_OFF_OR_UNAVAILABLE;
const Int32 IccCardProxy::EVENT_RADIO_ON;
const Int32 IccCardProxy::EVENT_ICC_CHANGED;
const Int32 IccCardProxy::EVENT_ICC_ABSENT;
const Int32 IccCardProxy::EVENT_ICC_LOCKED;
const Int32 IccCardProxy::EVENT_APP_READY;
const Int32 IccCardProxy::EVENT_RECORDS_LOADED;
const Int32 IccCardProxy::EVENT_IMSI_READY;
const Int32 IccCardProxy::EVENT_PERSO_LOCKED;
const Int32 IccCardProxy::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED;
const Int32 IccCardProxy::EVENT_ICC_RECORD_EVENTS;
const Int32 IccCardProxy::EVENT_SUBSCRIPTION_ACTIVATED;
const Int32 IccCardProxy::EVENT_SUBSCRIPTION_DEACTIVATED;
const Int32 IccCardProxy::EVENT_CARRIER_PRIVILIGES_LOADED;

IccCardProxy::IccCardProxy()
    : mExternalState(IccCardConstantsState_UNKNOWN)
    , mCurrentAppType(IUiccController::APP_FAM_3GPP)
    , mRadioOn(FALSE)
    , mQuietMode(FALSE)
    , mInitialized(FALSE)
    , mIsCardStatusAvailable(FALSE)
    , mPersoSubState(PERSOSUBSTATE_UNKNOWN)
{
    mAbsentRegistrants = new RegistrantList();
    mPinLockedRegistrants = new RegistrantList();
    mPersoLockedRegistrants = new RegistrantList();
}

ECode IccCardProxy::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci)
{
    Handler::constructor();
    Log(String("Creating"));
    mContext = context;
    mCi = ci;
    AutoPtr<ICdmaSubscriptionSourceManagerHelper> hlp;
    CCdmaSubscriptionSourceManagerHelper::AcquireSingleton((ICdmaSubscriptionSourceManagerHelper**)&hlp);
    hlp->GetInstance(context,
            ci, this, EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL, (ICdmaSubscriptionSourceManager**)&mCdmaSSM);
    AutoPtr<IUiccControllerHelper> uicchlp;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&uicchlp);
    uicchlp->GetInstance((IUiccController**)&mUiccController);
    mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);
    ci->RegisterForOn(this,EVENT_RADIO_ON, NULL);
    ci->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_UNAVAILABLE, NULL);
    SetExternalState(IccCardConstantsState_NOT_READY);
    return NOERROR;
}

ECode IccCardProxy::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ Int32 cardIndex)
{
    constructor(context, ci);

    mCardIndex = cardIndex;

    ResetProperties();
    SetExternalState(IccCardConstantsState_NOT_READY, FALSE);
    return NOERROR;
}

ECode IccCardProxy::Dispose()
{
    {
        AutoLock lock(mLock);
        Log(String("Disposing"));
        //Cleanup icc references
        mUiccController->UnregisterForIccChanged(this);
        mUiccController = NULL;
        mCi->UnregisterForOn(this);
        mCi->UnregisterForOffOrNotAvailable(this);
        mCdmaSSM->Dispose(this);
    }
    return NOERROR;
}

ECode IccCardProxy::SetVoiceRadioTech(
    /* [in] */ Int32 radioTech)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IServiceStateHelper> hlp;
        CServiceStateHelper::AcquireSingleton((IServiceStateHelper**)&hlp);
        if (DBG) {
            String str;
            hlp->RilRadioTechnologyToString(radioTech, &str);
            Log(String("Setting radio tech ") + str);
        }
        Boolean bGsm = FALSE;
        hlp->IsGsm(radioTech, &bGsm);
        if (bGsm) {
            mCurrentAppType = IUiccController::APP_FAM_3GPP;
        }
        else {
            mCurrentAppType = IUiccController::APP_FAM_3GPP2;
        }
        UpdateQuietMode();
        UpdateActiveRecord();
    }
    return NOERROR;
}

ECode IccCardProxy::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_RADIO_OFF_OR_UNAVAILABLE:
            mRadioOn = FALSE;
            ICommandsInterfaceRadioState state;
            mCi->GetRadioState(&state);
            if (RADIO_UNAVAILABLE == state) {
                SetExternalState(IccCardConstantsState_NOT_READY);
            }
            break;
        case EVENT_RADIO_ON:
            mRadioOn = TRUE;
            if (!mInitialized) {
                UpdateQuietMode();
            }
            // When the radio is off, if EVENT_ICC_CHANGED is received, the
            // mExternalState will be updated. After the radio turns on, if
            // EVENT_ICC_CHANGED is not received, the mExternalState will not be
            // be restored. Therefore, updateExternalState when the radio turns on.
            UpdateExternalState();
            break;
        case EVENT_ICC_CHANGED:
            mIsCardStatusAvailable = TRUE;
            if (mInitialized) {
                UpdateIccAvailability();
            }
            break;
        case EVENT_ICC_ABSENT:
            mAbsentRegistrants->NotifyRegistrants();
            SetExternalState(IccCardConstantsState_ABSENT);
            break;
        case EVENT_ICC_LOCKED:
            ProcessLockedState();
            break;
        case EVENT_APP_READY:
            SetExternalState(IccCardConstantsState_READY);
            break;
        case EVENT_RECORDS_LOADED:
            if (mIccRecords != NULL) {
                String oper;
                mIccRecords->GetOperatorNumeric(&oper);
                Int32 slotId = mCardIndex;

                Log(String("operator = ") + oper + String(" slotId = ") + StringUtils::ToString(slotId));

                if (oper != NULL) {
                    Log(String("update icc_operator_numeric=") + oper);
                    SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, slotId, oper);
                    if (mCurrentAppType == IUiccController::APP_FAM_3GPP) {
                        SetSystemProperty(ITelephonyProperties::PROPERTY_APN_SIM_OPERATOR_NUMERIC,
                                slotId, oper);
                        Log(String("update sim_operator_numeric=") + oper);
                    }
                    else if (mCurrentAppType == IUiccController::APP_FAM_3GPP2) {
                        SetSystemProperty(ITelephonyProperties::PROPERTY_APN_RUIM_OPERATOR_NUMERIC,
                                slotId, oper);
                        Log(String("update ruim_operator_numeric=") + oper);
                    }
                    String countryCode = oper.Substring(0, 3);
                    if (!countryCode.IsNull()) {
                        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, slotId,
                                MccTable::CountryCodeForMcc(StringUtils::ParseInt32(countryCode)));
                    }
                    else {
                        Loge(String("EVENT_RECORDS_LOADED Country code is NULL"));
                    }

                    AutoPtr<ISubscriptionControllerHelper> hlp;
                    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
                    AutoPtr<ISubscriptionController> sc;
                    hlp->GetInstance((ISubscriptionController**)&sc);
                    AutoPtr<ArrayOf<Int64> > subId;
                    IISub::Probe(sc)->GetSubId(slotId, (ArrayOf<Int64>**)&subId);
                    // Update MCC MNC device configuration information only for default sub.
                    Int64 defaultSubId = 0;
                    IISub::Probe(sc)->GetDefaultSubId(&defaultSubId);
                    if ((*subId)[0] == defaultSubId) {
                        Log(String("update mccmnc=") + oper + String(" config for default subscription."));
                        MccTable::UpdateMccMncConfiguration(mContext, oper, FALSE);
                    }
                    Int32 res = 0;
                    sc->SetMccMnc(oper, (*subId)[0], &res);
                }
                else {
                    Loge(String("EVENT_RECORDS_LOADED Operator name is NULL"));
                }
            }
            if (mUiccCard != NULL) {
                Boolean b = FALSE;
                mUiccCard->AreCarrierPriviligeRulesLoaded(&b);
                if (!b) {
                    mUiccCard->RegisterForCarrierPrivilegeRulesLoaded(
                        this, EVENT_CARRIER_PRIVILIGES_LOADED, NULL);
                }
            }
            else {
                OnRecordsLoaded();
            }
            break;
        case EVENT_IMSI_READY:
            BroadcastIccStateChangedIntent(IIccCardConstants::INTENT_VALUE_ICC_IMSI, String(NULL));
            break;
        case EVENT_PERSO_LOCKED:
            mUiccApplication->GetPersoSubState(&mPersoSubState);
            mPersoLockedRegistrants->NotifyRegistrants((AsyncResult*)(IObject*)(obj.Get()));
            SetExternalState(IccCardConstantsState_PERSO_LOCKED);
            break;
        case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
            UpdateQuietMode();
            UpdateActiveRecord();
            break;
        case EVENT_SUBSCRIPTION_ACTIVATED:
            Log(String("EVENT_SUBSCRIPTION_ACTIVATED"));
            OnSubscriptionActivated();
            break;

        case EVENT_SUBSCRIPTION_DEACTIVATED:
            Log(String("EVENT_SUBSCRIPTION_DEACTIVATED"));
            OnSubscriptionDeactivated();
            break;

        case EVENT_ICC_RECORD_EVENTS:
            if ((mCurrentAppType == IUiccController::APP_FAM_3GPP) && (mIccRecords != NULL)) {
                Int32 slotId = mCardIndex;
                AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
                Int32 eventCode = 0;
                IInteger32::Probe(ar->mResult)->GetValue(&eventCode);
                if (eventCode == IIccRecords::EVENT_SPN) {
                    String name;
                    mIccRecords->GetServiceProviderName(&name);
                    SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, slotId,
                            name);
                }
            }
            break;

        case EVENT_CARRIER_PRIVILIGES_LOADED:
            Log(String("EVENT_CARRIER_PRIVILEGES_LOADED"));
            if (mUiccCard != NULL) {
                mUiccCard->UnregisterForCarrierPrivilegeRulesLoaded(this);
            }
            OnRecordsLoaded();
            break;

        default:
            Loge(String("Unhandled message with number: ") + StringUtils::ToString(what));
            break;
    }
    return NOERROR;
}

ECode IccCardProxy::ResetProperties()
{
    if (mCurrentAppType == IUiccController::APP_FAM_3GPP) {
        Log(String("update icc_operator_numeric="));
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, mCardIndex, String(""));
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, mCardIndex, String(""));
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, mCardIndex, String(""));
     }
    return NOERROR;
}

ECode IccCardProxy::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        if (mIccRecords != NULL) {
            return mIccRecords->GetRecordsLoaded(result);
        }
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode IccCardProxy::GetState(
    /* [out] */ IccCardConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mExternalState;
    }
    return NOERROR;
}

ECode IccCardProxy::GetIccRecords(
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mIccRecords;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode IccCardProxy::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            return mUiccApplication->GetIccFileHandler(result);
        }
        *result = NULL;
    }
    return NOERROR;
}

ECode IccCardProxy::RegisterForAbsent(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);

        mAbsentRegistrants->Add(r);

        IccCardConstantsState state;
        GetState(&state);
        if (state == IccCardConstantsState_ABSENT) {
            r->NotifyRegistrant();
        }
    }
    return NOERROR;
}

ECode IccCardProxy::UnregisterForAbsent(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mLock);
        mAbsentRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode IccCardProxy::RegisterForPersoLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);

        mPersoLockedRegistrants->Add(r);

        IccCardConstantsState state;
        GetState(&state);
        if (state == IccCardConstantsState_PERSO_LOCKED) {
            AutoPtr<AsyncResult> p = new AsyncResult(NULL, CoreUtils::Convert(mPersoSubState), NULL);
            r->NotifyRegistrant(p);
        }
    }
    return NOERROR;
}

ECode IccCardProxy::UnregisterForPersoLocked(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mLock);
        mPersoLockedRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode IccCardProxy::RegisterForLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);

        mPinLockedRegistrants->Add(r);

        IccCardConstantsState state;
        GetState(&state);
        if (state == IccCardConstantsState_PIN_REQUIRED) {
            r->NotifyRegistrant();
        }
    }
    return NOERROR;
}

ECode IccCardProxy::UnregisterForLocked(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mLock);
        mPinLockedRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode IccCardProxy::SupplyPin(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->SupplyPin(pin, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::SupplyPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->SupplyPuk(puk, newPin, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::SupplyPin2(
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->SupplyPin2(pin2, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::SupplyPuk2(
    /* [in] */ const String& puk2,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->SupplyPuk2(puk2, newPin2, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::SupplyDepersonalization(
    /* [in] */ const String& pin,
    /* [in] */ const String& type,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->SupplyDepersonalization(pin, type, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("CommandsInterface is not set.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::GetIccLockEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        /* defaults to FALSE, if ICC is absent/deactivated */
        Boolean retValue = FALSE;
        if (mUiccApplication != NULL) {
            mUiccApplication->GetIccLockEnabled(&retValue);
        }
        *result = retValue;
    }
    return NOERROR;
}

ECode IccCardProxy::GetIccFdnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        Boolean retValue = FALSE;
        if (mUiccApplication != NULL) {
            mUiccApplication->GetIccFdnEnabled(&retValue);
        }
        *result = retValue;
    }
    return NOERROR;
}

ECode IccCardProxy::GetIccFdnAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean retValue = FALSE;
    if (mUiccApplication != NULL) {
        mUiccApplication->GetIccFdnAvailable(&retValue);
    }
    *result = retValue;
    return NOERROR;
}

ECode IccCardProxy::GetIccPin2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    /* defaults to disabled */
    Boolean retValue = FALSE;
    if (mUiccApplication != NULL) {
        mUiccApplication->GetIccPin2Blocked(&retValue);
    }
    *result = retValue;
    return NOERROR;
}

ECode IccCardProxy::GetIccPuk2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    /* defaults to disabled */
    Boolean retValue = FALSE;
    if (mUiccApplication != NULL) {
        mUiccApplication->GetIccPuk2Blocked(&retValue);
    }
    *result = retValue;
    return NOERROR;
}

ECode IccCardProxy::SetIccLockEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->SetIccLockEnabled(enabled, password, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::SetIccFdnEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->SetIccFdnEnabled(enabled, password, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::ChangeIccLockPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->ChangeIccLockPassword(oldPassword, newPassword, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::ChangeIccFdnPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication != NULL) {
            mUiccApplication->ChangeIccFdnPassword(oldPassword, newPassword, onComplete);
        }
        else if (onComplete != NULL) {
            assert(0 && "TODO");
            // Exception e = new RuntimeException("ICC card is absent.");
            // AsyncResult::ForMessage(onComplete)->mException = e;
            onComplete->SendToTarget();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode IccCardProxy::GetServiceProviderName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        if (mIccRecords != NULL) {
            return mIccRecords->GetServiceProviderName(result);
        }
        *result = String(NULL);
    }
    return NOERROR;
}

ECode IccCardProxy::IsApplicationOnIcc(
    /* [in] */  AppType type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        Boolean retValue = FALSE;
        if (mUiccCard != NULL) {
            mUiccCard->IsApplicationOnIcc(type, &retValue);
        }
        *result = retValue;
    }
    return NOERROR;
}

ECode IccCardProxy::HasIccCard(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        if (mUiccCard != NULL) {
            CardState state;
            mUiccCard->GetCardState(&state);
            if (state != CARDSTATE_ABSENT) {
                *result = TRUE;
                return NOERROR;
            }
        }
        *result = FALSE;
    }
    return NOERROR;
}

ECode IccCardProxy::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("IccCardProxy: ") + TO_CSTR(this));
    pw->Println(String(" mContext=") + TO_CSTR(mContext));
    pw->Println(String(" mCi=") + TO_CSTR(mCi));
    pw->Println(String(" mAbsentRegistrants: size=") + StringUtils::ToString(mAbsentRegistrants->GetSize()));
    for (Int32 i = 0; i < mAbsentRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mAbsentRegistrants->Get(i);
        AutoPtr<IRegistrant> reg = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        reg->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mAbsentRegistrants[") + StringUtils::ToString(i) + String("]=")
                + TO_CSTR(hdl));
    }
    pw->Println(String(" mPinLockedRegistrants: size=")
                + StringUtils::ToString(mPinLockedRegistrants->GetSize()));
    for (Int32 i = 0; i < mPinLockedRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mPinLockedRegistrants->Get(i);
        AutoPtr<IRegistrant> reg = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        reg->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mPinLockedRegistrants[") + StringUtils::ToString(i) + String("]=")
                + TO_CSTR(hdl));
    }
    pw->Println(String(" mPersoLockedRegistrants: size=")
                + StringUtils::ToString(mPersoLockedRegistrants->GetSize()));
    for (Int32 i = 0; i < mPersoLockedRegistrants->GetSize(); i++) {
        AutoPtr<IInterface> p = mPersoLockedRegistrants->Get(i);
        AutoPtr<IRegistrant> reg = IRegistrant::Probe(p);
        AutoPtr<IHandler> hdl;
        reg->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mPersoLockedRegistrants[") + StringUtils::ToString(i) + String("]=")
                + TO_CSTR(hdl));
    }
    pw->Println(String(" mCurrentAppType=") + StringUtils::ToString(mCurrentAppType));
    pw->Println(String(" mUiccController=") + TO_CSTR(mUiccController));
    pw->Println(String(" mUiccCard=") + TO_CSTR(mUiccCard));
    pw->Println(String(" mUiccApplication=") + TO_CSTR(mUiccApplication));
    pw->Println(String(" mIccRecords=") + TO_CSTR(mIccRecords));
    pw->Println(String(" mCdmaSSM=") + TO_CSTR(mCdmaSSM));
    pw->Println(String(" mRadioOn=") + StringUtils::ToString(mRadioOn));
    pw->Println(String(" mQuietMode=") + StringUtils::ToString(mQuietMode));
    pw->Println(String(" mInitialized=") + StringUtils::ToString(mInitialized));
    pw->Println(String(" mExternalState=") + StringUtils::ToString(mExternalState));

    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

void IccCardProxy::UpdateActiveRecord()
{
    Log(String("updateActiveRecord app type = ") + StringUtils::ToString(mCurrentAppType) +
            String("mIccRecords = "));

    if (mIccRecords == NULL) {
        return;
    }

    Log(TO_STR(mIccRecords));
    if (mCurrentAppType == IUiccController::APP_FAM_3GPP2) {
        Int32 newSubscriptionSource = 0;
        mCdmaSSM->GetCdmaSubscriptionSource(&newSubscriptionSource);
        // Allow RUIM to fetch in CDMA LTE mode if NV LTE mode.
        // Fixes cases where iccid could be unknown on some CDMA NV devices.
        AutoPtr<IPhoneFactory> pf;
        CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
        AutoPtr<IPhone> defaultPhone;
        pf->GetDefaultPhone((IPhone**)&defaultPhone);
        Int32 mode = 0;
        defaultPhone->GetLteOnCdmaMode(&mode);
        if (newSubscriptionSource == ICdmaSubscriptionSourceManager::SUBSCRIPTION_FROM_RUIM
                || mode == IPhoneConstants::LTE_ON_CDMA_TRUE) {
            // Set this as the Active record.
            Log(String("Setting Ruim Record as active"));
            mIccRecords->RecordsRequired();
        }
    }
    else if (mCurrentAppType == IUiccController::APP_FAM_3GPP) {
        Log(String("Setting SIM Record as active"));
        mIccRecords->RecordsRequired();
    }
}

void IccCardProxy::UpdateQuietMode()
{
    {
        AutoLock lock(mLock);
        Boolean oldQuietMode = mQuietMode;
        Boolean newQuietMode;
        // "config_lte_capable" is set to TRUE when the device is
        // LTE capable
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean isLteCapable = FALSE;
        res->GetBoolean(
                R::bool_::config_lte_capable, &isLteCapable);
        Int32 cdmaSource = IPhone::CDMA_SUBSCRIPTION_UNKNOWN;
        if (mCurrentAppType == IUiccController::APP_FAM_3GPP) {
            newQuietMode = FALSE;
            if (DBG) {
                Log(String("updateQuietMode: 3GPP subscription -> newQuietMode=")
                    + StringUtils::ToString(newQuietMode));
            }
        }
        else {
            cdmaSource = IPhone::CDMA_SUBSCRIPTION_UNKNOWN;
            if (mCdmaSSM != NULL) {
                mCdmaSSM->GetCdmaSubscriptionSource(&cdmaSource);
            }

            if (isLteCapable) {
                // For a LTE capable device, always be out of Quiet Mode
                newQuietMode = FALSE;
            }
            else {
                newQuietMode = (cdmaSource == IPhone::CDMA_SUBSCRIPTION_NV)
                        && (mCurrentAppType == IUiccController::APP_FAM_3GPP2);
            }
            if (DBG) {
                Log(String("updateQuietMode: 3GPP2 subscription -> newQuietMode=")
                    + StringUtils::ToString(newQuietMode));
            }
        }

        if (mQuietMode == FALSE && newQuietMode == TRUE) {
            // Last thing to do before switching to quiet mode is
            // broadcast ICC_READY
            Log(String("Switching to QuietMode."));
            SetExternalState(IccCardConstantsState_READY);
            mQuietMode = newQuietMode;
        }
        else if (mQuietMode == TRUE && newQuietMode == FALSE) {
            if (DBG) {
                Log(String("updateQuietMode: Switching out from QuietMode.")
                    + String(" Force broadcast of current state=")
                    + StringUtils::ToString(mExternalState));
            }
            mQuietMode = newQuietMode;
            SetExternalState(mExternalState, TRUE);
        }
        if (DBG) {
            Log(String("updateQuietMode: QuietMode is ")
                + StringUtils::ToString(mQuietMode) + String(" (app_type=")
                + StringUtils::ToString(mCurrentAppType) + String(" cdmaSource=")
                + StringUtils::ToString(cdmaSource) + String(")"));
        }
        mInitialized = TRUE;
        //Send EVENT_ICC_CHANGED only if it is already received atleast once from RIL.
        if (mIsCardStatusAvailable) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_ICC_CHANGED, (IMessage**)&msg);
            Boolean b = FALSE;
            SendMessage(msg, &b);
        }
    }
}

void IccCardProxy::OnSubscriptionActivated()
{
    //mSubscriptionData = SubscriptionManager.getCurrentSubscription(mCardIndex);

    UpdateIccAvailability();
    UpdateStateProperty();
}

void IccCardProxy::OnSubscriptionDeactivated()
{
    ResetProperties();
    mSubscriptionData = NULL;
    UpdateIccAvailability();
    UpdateStateProperty();
}

void IccCardProxy::OnRecordsLoaded()
{
    BroadcastIccStateChangedIntent(IIccCardConstants::INTENT_VALUE_ICC_LOADED, String(NULL));
}

void IccCardProxy::UpdateIccAvailability()
{
    {
        AutoLock lock(mLock);
        AutoPtr<IUiccCard> newCard;
        mUiccController->GetUiccCard(mCardIndex, (IUiccCard**)&newCard);
        CardState state = CARDSTATE_ABSENT;
        AutoPtr<IUiccCardApplication> newApp;
        AutoPtr<IIccRecords> newRecords;
        if (newCard != NULL) {
            newCard->GetCardState(&state);
            newCard->GetApplication(mCurrentAppType, (IUiccCardApplication**)&newApp);
            if (newApp != NULL) {
                newApp->GetIccRecords((IIccRecords**)&newRecords);
            }
        }

        if (mIccRecords != newRecords || mUiccApplication != newApp || mUiccCard != newCard) {
            if (DBG) Log(String("Icc changed. Reregestering."));
            UnregisterUiccCardEvents();
            mUiccCard = newCard;
            mUiccApplication = newApp;
            mIccRecords = newRecords;
            RegisterUiccCardEvents();
            UpdateActiveRecord();
        }
        UpdateExternalState();
    }
}

void IccCardProxy::UpdateExternalState()
{
    // mUiccCard could be NULL at bootup, before valid card states have
    // been received from UiccController.
    if (mUiccCard == NULL) {
        SetExternalState(IccCardConstantsState_NOT_READY);
        return;
    }

    CardState state;
    mUiccCard->GetCardState(&state);
    if (state == CARDSTATE_ABSENT) {
        if (mRadioOn) {
            SetExternalState(IccCardConstantsState_ABSENT);
        }
        else {
            SetExternalState(IccCardConstantsState_NOT_READY);
        }
        return;
    }

    if (state == CARDSTATE_ERROR) {
        SetExternalState(IccCardConstantsState_CARD_IO_ERROR);
        return;
    }

    if (mUiccApplication == NULL) {
        SetExternalState(IccCardConstantsState_NOT_READY);
        return;
    }

    AppState aState;
    mUiccApplication->GetState(&aState);
    switch (aState) {
        case APPSTATE_UNKNOWN:
            SetExternalState(IccCardConstantsState_UNKNOWN);
            break;
        case APPSTATE_PIN:
            SetExternalState(IccCardConstantsState_PIN_REQUIRED);
            break;
        case APPSTATE_PUK:
            SetExternalState(IccCardConstantsState_PUK_REQUIRED);
            break;
        case APPSTATE_SUBSCRIPTION_PERSO: {
            Boolean bLock = FALSE;
            mUiccApplication->IsPersoLocked(&bLock);
            if (bLock) {
                mUiccApplication->GetPersoSubState(&mPersoSubState);
                SetExternalState(IccCardConstantsState_PERSO_LOCKED);
            }
            else {
                SetExternalState(IccCardConstantsState_UNKNOWN);
            }
            break;
        }
        case APPSTATE_READY:
            SetExternalState(IccCardConstantsState_READY);
            break;
    }
}

void IccCardProxy::RegisterUiccCardEvents()
{
    if (mUiccCard != NULL) {
        mUiccCard->RegisterForAbsent(this, EVENT_ICC_ABSENT, NULL);
    }
    if (mUiccApplication != NULL) {
        mUiccApplication->RegisterForReady(this, EVENT_APP_READY, NULL);
        mUiccApplication->RegisterForLocked(this, EVENT_ICC_LOCKED, NULL);
        mUiccApplication->RegisterForPersoLocked(this, EVENT_PERSO_LOCKED, NULL);
    }
    if (mIccRecords != NULL) {
        mIccRecords->RegisterForImsiReady(this, EVENT_IMSI_READY, NULL);
        mIccRecords->RegisterForRecordsLoaded(this, EVENT_RECORDS_LOADED, NULL);
        mIccRecords->RegisterForRecordsEvents(this, EVENT_ICC_RECORD_EVENTS, NULL);
    }
}

void IccCardProxy::UnregisterUiccCardEvents()
{
    if (mUiccCard != NULL) mUiccCard->UnregisterForAbsent(this);
    if (mUiccApplication != NULL) mUiccApplication->UnregisterForReady(this);
    if (mUiccApplication != NULL) mUiccApplication->UnregisterForLocked(this);
    if (mUiccApplication != NULL) mUiccApplication->UnregisterForPersoLocked(this);
    if (mIccRecords != NULL) mIccRecords->UnregisterForImsiReady(this);
    if (mIccRecords != NULL) mIccRecords->UnregisterForRecordsLoaded(this);
    if (mIccRecords != NULL) mIccRecords->UnregisterForRecordsEvents(this);
}

static const String StateToString(
    /* [in] */ IccCardConstantsState state)
{
    switch (state) {
        case IccCardConstantsState_ABSENT:
            return String("ABSENT");
        case IccCardConstantsState_PIN_REQUIRED:
            return String("PIN_REQUIRED");
        case IccCardConstantsState_PUK_REQUIRED:
            return String("PUK_REQUIRED");
        case IccCardConstantsState_PERSO_LOCKED:
            return String("PERSO_LOCKED");
        case IccCardConstantsState_READY:
            return String("READY");
        case IccCardConstantsState_NOT_READY:
            return String("NOT_READY");
        case IccCardConstantsState_PERM_DISABLED:
            return String("PERM_DISABLED");
        case IccCardConstantsState_CARD_IO_ERROR:
            return String("CARD_IO_ERROR");
        case IccCardConstantsState_UNKNOWN:
        default:
            return String("UNKNOWN");
    }
    return String("UNKNOWN");
}

void IccCardProxy::UpdateStateProperty()
{
    IccCardConstantsState state;
    GetState(&state);
    SetSystemProperty(ITelephonyProperties::PROPERTY_SIM_STATE, mCardIndex, StateToString(state));
}

void IccCardProxy::BroadcastIccStateChangedIntent(
    /* [in] */ const String& value,
    /* [in] */ const String& reason)
{
    {
        AutoLock lock(mLock);
        // if (mCardIndex == NULL) {
        //     Loge(String("broadcastIccStateChangedIntent: Card Index is not set; Return!!"));
        //     return;
        // }

        if (mQuietMode) {
            Log(String("QuietMode: NOT Broadcasting intent ACTION_SIM_STATE_CHANGED ") + value
                    + String(" reason ") + reason);
            return;
        }

        AutoPtr<IIntent> intent;
        CIntent::New(ITelephonyIntents::ACTION_SIM_STATE_CHANGED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(IPhoneConstants::PHONE_NAME_KEY, String("Phone"));
        intent->PutExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, value);
        intent->PutExtra(IIccCardConstants::INTENT_KEY_LOCKED_REASON, reason);
        AutoPtr<ISubscriptionManager> sm;
        CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
        sm->PutPhoneIdAndSubIdExtra(intent, mCardIndex);
        Log(String("Broadcasting intent ACTION_SIM_STATE_CHANGED ") +  value
            + String(" reason ") + reason + String(" for mCardIndex : ") + StringUtils::ToString(mCardIndex));
        ActivityManagerNative::BroadcastStickyIntent(intent, Manifest::permission::READ_PHONE_STATE,
                IUserHandle::USER_ALL);
    }
}

void IccCardProxy::SetExternalState(
    /* [in] */ IccCardConstantsState newState,
    /* [in] */ Boolean _override)
{
    {
        AutoLock lock(mLock);
        // if (mCardIndex == NULL) {
        //     Loge(String("setExternalState: Card Index is not set; Return!!"));
        //     return;
        // }

        if (!_override && newState == mExternalState) {
            return;
        }
        mExternalState = newState;
        IccCardConstantsState state;
        GetState(&state);
        SetSystemProperty(ITelephonyProperties::PROPERTY_SIM_STATE, mCardIndex, StateToString(state));
        BroadcastIccStateChangedIntent(GetIccStateIntentString(mExternalState),
                GetIccStateReason(mExternalState));
        // TODO: Need to notify registrants for other states as well.
        if (IccCardConstantsState_ABSENT == mExternalState) {
            mAbsentRegistrants->NotifyRegistrants();
        }
    }
}

void IccCardProxy::ProcessLockedState()
{
    {
        AutoLock lock(mLock);
        if (mUiccApplication == NULL) {
            //Don't need to do anything if non-existent application is locked
            return;
        }
        PinState pin1State;
        mUiccApplication->GetPin1State(&pin1State);
        if (pin1State == PINSTATE_ENABLED_PERM_BLOCKED) {
            SetExternalState(IccCardConstantsState_PERM_DISABLED);
            return;
        }

        AppState appState;
        mUiccApplication->GetState(&appState);
        switch (appState) {
            case APPSTATE_PIN:
                mPinLockedRegistrants->NotifyRegistrants();
                SetExternalState(IccCardConstantsState_PIN_REQUIRED);
                break;
            case APPSTATE_PUK:
                SetExternalState(IccCardConstantsState_PUK_REQUIRED);
                break;
            case APPSTATE_DETECTED:
            case APPSTATE_READY:
            case APPSTATE_SUBSCRIPTION_PERSO:
            case APPSTATE_UNKNOWN:
                // Neither required
                break;
        }
    }
}

void IccCardProxy::SetExternalState(
    /* [in] */ IccCardConstantsState newState)
{
    SetExternalState(newState, FALSE);
}

String IccCardProxy::GetIccStateIntentString(
    /* [in] */ IccCardConstantsState state)
{
    switch (state) {
        case IccCardConstantsState_ABSENT: return IIccCardConstants::INTENT_VALUE_ICC_ABSENT;
        case IccCardConstantsState_PIN_REQUIRED: return IIccCardConstants::INTENT_VALUE_ICC_LOCKED;
        case IccCardConstantsState_PUK_REQUIRED: return IIccCardConstants::INTENT_VALUE_ICC_LOCKED;
        case IccCardConstantsState_PERSO_LOCKED: return IIccCardConstants::INTENT_VALUE_ICC_LOCKED;
        case IccCardConstantsState_READY: return IIccCardConstants::INTENT_VALUE_ICC_READY;
        case IccCardConstantsState_NOT_READY: return IIccCardConstants::INTENT_VALUE_ICC_NOT_READY;
        case IccCardConstantsState_PERM_DISABLED: return IIccCardConstants::INTENT_VALUE_ICC_LOCKED;
        case IccCardConstantsState_CARD_IO_ERROR: return IIccCardConstants::INTENT_VALUE_ICC_CARD_IO_ERROR;
        default: return IIccCardConstants::INTENT_VALUE_ICC_UNKNOWN;
    }
}

String IccCardProxy::GetIccStateReason(
    /* [in] */ IccCardConstantsState state)
{
     switch (state) {
         case IccCardConstantsState_PIN_REQUIRED: return IIccCardConstants::INTENT_VALUE_LOCKED_ON_PIN;
         case IccCardConstantsState_PUK_REQUIRED: return IIccCardConstants::INTENT_VALUE_LOCKED_ON_PUK;
         case IccCardConstantsState_PERSO_LOCKED: return IIccCardConstants::INTENT_VALUE_LOCKED_PERSO;
         case IccCardConstantsState_PERM_DISABLED: return IIccCardConstants::INTENT_VALUE_ABSENT_ON_PERM_DISABLED;
         case IccCardConstantsState_CARD_IO_ERROR: return IIccCardConstants::INTENT_VALUE_ICC_CARD_IO_ERROR;
         default: return String(NULL);
    }
}

void IccCardProxy::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ Int32 slotId,
    /* [in] */ const String& value)
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    AutoPtr<ArrayOf<Int64> > subId;
    IISub::Probe(sc)->GetSubId(slotId, (ArrayOf<Int64>**)&subId);
    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    tmhlp->SetTelephonyProperty(property, (*subId)[0], value);
}

void IccCardProxy::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOGTAG, s);
}

void IccCardProxy::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(LOGTAG, msg);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
