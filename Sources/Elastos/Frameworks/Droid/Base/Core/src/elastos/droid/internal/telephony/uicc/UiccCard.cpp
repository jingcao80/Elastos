
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/internal/telephony/uicc/UiccCard.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/internal/telephony/cat/CatServiceFactory.h"
#include "elastos/droid/internal/telephony/cat/CCatServiceFactory.h"
#include "elastos/droid/internal/telephony/uicc/IccCardStatus.h"
#include "elastos/droid/internal/telephony/uicc/CUICCConfig.h"
#include "elastos/droid/internal/telephony/uicc/UiccCardApplication.h"
#include "elastos/droid/internal/telephony/uicc/CUiccCardApplication.h"
#include "elastos/droid/internal/telephony/uicc/CUiccCarrierPrivilegeRules.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/preference/CPreferenceManagerHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Internal::Telephony::Cat::CatServiceFactory;
using Elastos::Droid::Internal::Telephony::Cat::CCatServiceFactory;
using Elastos::Droid::Internal::Telephony::Cat::ICatServiceFactory;
using Elastos::Droid::Internal::Telephony::ICommandsInterfaceRadioState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::R;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//            UiccCard::InnerDialogInterfaceOnClickListener1
//=====================================================================
CAR_INTERFACE_IMPL(UiccCard::InnerDialogInterfaceOnClickListener1, Object, IDialogInterfaceOnClickListener);

UiccCard::InnerDialogInterfaceOnClickListener1::InnerDialogInterfaceOnClickListener1(
    /* [in] */ UiccCard* owner)
    : mOwner(owner)
{
}

ECode UiccCard::InnerDialogInterfaceOnClickListener1::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    {
        AutoLock lock(mOwner->mLock);
        if (which == IDialogInterface::BUTTON_POSITIVE) {
            if (DBG) mOwner->Log(String("Reboot due to SIM swap"));
            AutoPtr<IInterface> p;
            mOwner->mContext->GetSystemService(IContext::POWER_SERVICE,
                                    (IInterface**)&p);
            AutoPtr<IPowerManager> pm = IPowerManager::Probe(p);
            pm->Reboot(String("SIM is added."));
        }
    }
    return NOERROR;
}

//=====================================================================
//                        UiccCard::InnerHandler
//=====================================================================
UiccCard::InnerHandler::InnerHandler(
    /* [in] */ UiccCard* owner)
    : mOwner(owner)
{
    Handler::constructor();
}

ECode UiccCard::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    if (mOwner->mDestroyed) {
        mOwner->Loge(String("Received message ") + TO_CSTR(msg) +
            String("[") + StringUtils::ToString(what)
            + String("] while being destroyed. Ignoring."));
        return NOERROR;
    }

    switch (what) {
        case EVENT_CARD_REMOVED:
            mOwner->OnIccSwap(FALSE);
            break;
        case EVENT_CARD_ADDED:
            mOwner->OnIccSwap(TRUE);
            break;
        case EVENT_OPEN_LOGICAL_CHANNEL_DONE:
        case EVENT_CLOSE_LOGICAL_CHANNEL_DONE:
        case EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE:
        case EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE:
        case EVENT_SIM_IO_DONE:
        case EVENT_SIM_GET_ATR_DONE: {
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                if (DBG) {
                    mOwner->Log(String("Error in SIM access with exception") + TO_CSTR(ar->mException));
                }
            }
            AsyncResult::ForMessage(IMessage::Probe(ar->mUserObj), ar->mResult, ar->mException);
            IMessage::Probe(ar->mUserObj)->SendToTarget();
            break;
        }
        case EVENT_CARRIER_PRIVILIGES_LOADED:
            mOwner->OnCarrierPriviligesLoadedMessage();
            break;
        default:
            mOwner->Loge(String("Unknown Event ") + StringUtils::ToString(what));
    }
    return NOERROR;
}

//=====================================================================
//                               UiccCard
//=====================================================================
CAR_INTERFACE_IMPL(UiccCard, Object, IUiccCard);

const String UiccCard::LOGTAG("UiccCard");
const Boolean UiccCard::DBG = TRUE;
const String UiccCard::OPERATOR_BRAND_OVERRIDE_PREFIX("operator_branding_");
const Int32 UiccCard::EVENT_CARD_REMOVED;
const Int32 UiccCard::EVENT_CARD_ADDED;
const Int32 UiccCard::EVENT_OPEN_LOGICAL_CHANNEL_DONE;
const Int32 UiccCard::EVENT_CLOSE_LOGICAL_CHANNEL_DONE;
const Int32 UiccCard::EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE;
const Int32 UiccCard::EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE;
const Int32 UiccCard::EVENT_SIM_IO_DONE;
const Int32 UiccCard::EVENT_CARRIER_PRIVILIGES_LOADED;
const Int32 UiccCard::EVENT_SIM_GET_ATR_DONE;

UiccCard::UiccCard()
{
    mCardState = -1;
    mUniversalPinState = -1;
    mGsmUmtsSubscriptionAppIndex = 0;
    mCdmaSubscriptionAppIndex = 0;
    mImsSubscriptionAppIndex = 0;
    mDestroyed = FALSE;
    mUiccApplications = ArrayOf<IUiccCardApplication*>::Alloc(IIccCardStatus::CARD_MAX_APPS);
    mHandler = new InnerHandler(this);
    mLastRadioState = RADIO_UNAVAILABLE;
    mAbsentRegistrants = new RegistrantList();
    mCarrierPrivilegeRegistrants = new RegistrantList();
}

ECode UiccCard::constructor(
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IIccCardStatus* ics,
    /* [in] */ Int32 slotId)
{
    mCardState = ((IccCardStatus*)ics)->mCardState;
    mSlotId = slotId;
    Update(c, ci, ics);
    return NOERROR;
}

ECode UiccCard::Dispose()
{
    {
        AutoLock lock(mLock);
        if (DBG) Log(String("Disposing card"));
        if (mCatService != NULL) {
            AutoPtr<ICatServiceFactory> cf;
            CCatServiceFactory::AcquireSingleton((ICatServiceFactory**)&cf);
            cf->DisposeCatService(mSlotId);
        }
        for (Int32 i = 0; i < mUiccApplications->GetLength(); i++) {
            AutoPtr<UiccCardApplication> app = (UiccCardApplication*)((*mUiccApplications)[i]);
            if (app != NULL) {
                app->Dispose();
            }
        }
        mCatService = NULL;
        mUiccApplications = NULL;
        mCarrierPrivilegeRules = NULL;
        mUICCConfig = NULL;
    }
    return NOERROR;
}

ECode UiccCard::Update(
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IIccCardStatus* ics)
{
    {
        AutoLock lock(mLock);
        if (mDestroyed) {
            Loge(String("Updated after destroyed! Fix me!"));
            return NOERROR;
        }
        CardState oldState = mCardState;
        AutoPtr<IccCardStatus> _ics = (IccCardStatus*)ics;
        mCardState = _ics->mCardState;
        mUniversalPinState = _ics->mUniversalPinState;
        mGsmUmtsSubscriptionAppIndex = _ics->mGsmUmtsSubscriptionAppIndex;
        mCdmaSubscriptionAppIndex = _ics->mCdmaSubscriptionAppIndex;
        mImsSubscriptionAppIndex = _ics->mImsSubscriptionAppIndex;
        mContext = c;
        mCi = ci;
        //update applications
        if (mUICCConfig == NULL) {
            CUICCConfig::New((IUICCConfig**)&mUICCConfig);
        }
        if (DBG) {
            Int32 length = _ics->mApplications->GetLength();
            Log(StringUtils::ToString(length) + String(" applications"));
        }
        for (Int32 i = 0; i < mUiccApplications->GetLength(); i++) {
            if ((*mUiccApplications)[i] == NULL) {
                //Create newly added Applications
                if (i < _ics->mApplications->GetLength()) {
                    CUiccCardApplication::New(this,
                            (*(_ics->mApplications))[i], mContext, mCi,
                            (IUiccCardApplication**)&((*mUiccApplications)[i]));
                }
            }
            else if (i >= _ics->mApplications->GetLength()) {
                //Delete removed applications
                ((UiccCardApplication*)((*mUiccApplications)[i]))->Dispose();
                (*mUiccApplications)[i] = NULL;
            }
            else {
                //Update the rest
                ((UiccCardApplication*)((*mUiccApplications)[i]))->Update((*(_ics->mApplications))[i], mContext, mCi);
            }
        }

        CreateAndUpdateCatService();

        // Reload the carrier privilege rules if necessary.
        Log(String("Before privilege rules: ") /*+ mCarrierPrivilegeRules*/ + String(" : ") + StringUtils::ToString(mCardState));
        if (mCarrierPrivilegeRules == NULL && mCardState == CARDSTATE_PRESENT) {
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(EVENT_CARRIER_PRIVILIGES_LOADED, (IMessage**)&msg);
            CUiccCarrierPrivilegeRules::New(this, msg, (IUiccCarrierPrivilegeRules**)&mCarrierPrivilegeRules);
        }
        else if (mCarrierPrivilegeRules != NULL && mCardState != CARDSTATE_PRESENT) {
            mCarrierPrivilegeRules = NULL;
        }

        SanitizeApplicationIndexes();

        ICommandsInterfaceRadioState radioState;
        mCi->GetRadioState(&radioState);
        if (DBG) {
            Log(String("update: radioState=") + StringUtils::ToString(radioState) +
                String(" mLastRadioState=") + StringUtils::ToString(mLastRadioState));
        }
        // No notifications while radio is off or we just powering up
        if (radioState == RADIO_ON && mLastRadioState == RADIO_ON) {
            if (oldState != CARDSTATE_ABSENT &&
                    mCardState == CARDSTATE_ABSENT) {
                if (DBG) {
                    Log(String("update: notify card removed"));
                }
                mAbsentRegistrants->NotifyRegistrants();
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(EVENT_CARD_REMOVED, NULL, (IMessage**)&msg);
                Boolean b = FALSE;
                mHandler->SendMessage(msg, &b);
            }
            else if (oldState == CARDSTATE_ABSENT &&
                    mCardState != CARDSTATE_ABSENT) {
                if (DBG) {
                    Log(String("update: notify card added"));
                }
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(EVENT_CARD_ADDED, NULL, (IMessage**)&msg);
                Boolean b = FALSE;
                mHandler->SendMessage(msg, &b);
            }
        }
        mLastRadioState = radioState;
    }
    return NOERROR;
}

ECode UiccCard::GetCatService(
    /* [out] */ ICatService** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCatService;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode UiccCard::RegisterForAbsent(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);

        mAbsentRegistrants->Add(r);

        if (mCardState == CARDSTATE_ABSENT) {
            r->NotifyRegistrant();
        }
    }
    return NOERROR;
}

ECode UiccCard::UnregisterForAbsent(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mLock);
        mAbsentRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode UiccCard::RegisterForCarrierPrivilegeRulesLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {
        AutoLock lock(mLock);
        AutoPtr<IRegistrant> r;
        CRegistrant::New(h, what, obj, (IRegistrant**)&r);

        mCarrierPrivilegeRegistrants->Add(r);

        Boolean b = FALSE;
        AreCarrierPriviligeRulesLoaded(&b);
        if (b) {
            r->NotifyRegistrant();
        }
    }
    return NOERROR;
}

ECode UiccCard::UnregisterForCarrierPrivilegeRulesLoaded(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mLock);
        mCarrierPrivilegeRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode UiccCard::IsApplicationOnIcc(
    /* [in] */ AppType type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        for (Int32 i = 0 ; i < mUiccApplications->GetLength(); i++) {
            if ((*mUiccApplications)[i] != NULL) {
                AppType iType;
                (*mUiccApplications)[i]->GetType(&iType);
                if (iType == type) {
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode UiccCard::GetCardState(
    /* [out] */ CardState* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mCardState;
        return NOERROR;
    }
    return NOERROR;
}

ECode UiccCard::GetUniversalPinState(
    /* [out] */ PinState* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mUniversalPinState;
    }
    return NOERROR;
}

ECode UiccCard::GetSlotId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        *result = mSlotId;
    }
    return NOERROR;
}

ECode UiccCard::GetApplication(
    /* [in] */ Int32 family,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    Int32 index = IIccCardStatus::CARD_MAX_APPS;
    switch (family) {
        case IUiccController::APP_FAM_3GPP:
            index = mGsmUmtsSubscriptionAppIndex;
            break;
        case IUiccController::APP_FAM_3GPP2:
            index = mCdmaSubscriptionAppIndex;
            break;
        case IUiccController::APP_FAM_IMS:
            index = mImsSubscriptionAppIndex;
            break;
    }
    if (index >= 0 && index < mUiccApplications->GetLength()) {
        *result = (*mUiccApplications)[index];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode UiccCard::GetApplicationIndex(
    /* [in] */ Int32 index,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        if (index >= 0 && index < mUiccApplications->GetLength()) {
            *result = (*mUiccApplications)[index];
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        *result = NULL;
    }
    return NOERROR;
}

ECode UiccCard::GetApplicationByType(
    /* [in] */ Int32 type,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(mLock);
        for (Int32 i = 0 ; i < mUiccApplications->GetLength(); i++) {
            if ((*mUiccApplications)[i] != NULL) {
                AppType iType;
                (*mUiccApplications)[i]->GetType(&iType);
                if (iType == type) {
                    *result = (*mUiccApplications)[i];
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            }
        }
        *result = NULL;
    }
    return NOERROR;
}

ECode UiccCard::IccOpenLogicalChannel(
    /* [in] */ const String& AID,
    /* [in] */ IMessage* response)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_OPEN_LOGICAL_CHANNEL_DONE, response, (IMessage**)&msg);
    mCi->IccOpenLogicalChannel(AID, msg);
    return NOERROR;
}

ECode UiccCard::IccCloseLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ IMessage* response)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_CLOSE_LOGICAL_CHANNEL_DONE, response, (IMessage**)&msg);
    mCi->IccCloseLogicalChannel(channel, msg);
    return NOERROR;
}

ECode UiccCard::IccTransmitApduLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ Int32 cla,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE, response, (IMessage**)&msg);
    mCi->IccTransmitApduLogicalChannel(channel, cla, command, p1, p2, p3,
            data, msg);
    return NOERROR;
}

ECode UiccCard::IccTransmitApduBasicChannel(
    /* [in] */ Int32 cla,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE, response, (IMessage**)&msg);
    mCi->IccTransmitApduBasicChannel(cla, command, p1, p2, p3,
            data, msg);
    return NOERROR;
}

ECode UiccCard::IccExchangeSimIO(
    /* [in] */ Int32 fileID,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& pathID,
    /* [in] */ IMessage* response)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_SIM_IO_DONE, response, (IMessage**)&msg);
    mCi->IccIO(command, fileID, pathID, p1, p2, p3, String(NULL), String(NULL), msg);
    return NOERROR;
}

ECode UiccCard::GetAtr(
    /* [in] */ IMessage* response)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_SIM_GET_ATR_DONE, response, (IMessage**)&msg);
    mCi->GetAtr(msg);
    return NOERROR;
}

ECode UiccCard::SendEnvelopeWithStatus(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    mCi->SendEnvelopeWithStatus(contents, response);
    return NOERROR;
}

ECode UiccCard::GetNumApplications(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count = 0;
    for (Int32 i = 0; i < mUiccApplications->GetLength(); ++i) {
        if ((*mUiccApplications)[i] != NULL) {
            count++;
        }
    }
    *result = count;
    return NOERROR;
}

ECode UiccCard::AreCarrierPriviligeRulesLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b = FALSE;
    *result = mCarrierPrivilegeRules != NULL
        && (mCarrierPrivilegeRules->AreCarrierPriviligeRulesLoaded(&b), b);
    return NOERROR;
}

ECode UiccCard::GetCarrierPrivilegeStatus(
    /* [in] */ ISignature* signature,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 res = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
    if (mCarrierPrivilegeRules != NULL) {
        mCarrierPrivilegeRules->GetCarrierPrivilegeStatus(signature, packageName, &res);
    }
    *result = res;
    return NOERROR;
}

ECode UiccCard::GetCarrierPrivilegeStatus(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 res = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
    if (mCarrierPrivilegeRules != NULL) {
        mCarrierPrivilegeRules->GetCarrierPrivilegeStatus(packageManager, packageName, &res);
    }
    *result = res;
    return NOERROR;
}

ECode UiccCard::GetCarrierPrivilegeStatusForCurrentTransaction(
    /* [in] */ IPackageManager* packageManager,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 res = ITelephonyManager::CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED;
    if (mCarrierPrivilegeRules != NULL) {
        mCarrierPrivilegeRules->GetCarrierPrivilegeStatusForCurrentTransaction(packageManager, &res);
    }
    *result = res;
    return NOERROR;
}

ECode UiccCard::GetCarrierPackageNamesForIntent(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IIntent* intent,
    /* [out] */ IList/*<String>*/** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> res;
    if (mCarrierPrivilegeRules != NULL) {
        mCarrierPrivilegeRules->GetCarrierPackageNamesForIntent(
                packageManager, intent, (IList**)&res);
    }
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode UiccCard::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Log(String("setOperatorBrandOverride: ") + brand);
    String iccId;
    GetIccId(&iccId);
    Log(String("current iccId: ") + iccId);

    if (TextUtils::IsEmpty(iccId)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IPreferenceManagerHelper> hlp;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&hlp);
    AutoPtr<ISharedPreferences> sp;
    hlp->GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);
    AutoPtr<ISharedPreferencesEditor> spEditor;
    sp->Edit((ISharedPreferencesEditor**)&spEditor);
    String key = OPERATOR_BRAND_OVERRIDE_PREFIX + iccId;
    if (brand == NULL) {
        spEditor->Remove(key);
        Boolean b = FALSE;
        spEditor->Commit(&b);
    }
    else {
        spEditor->PutString(key, brand);
        Boolean b = FALSE;
        spEditor->Commit(&b);
    }
    *result = TRUE;
    return NOERROR;
}

ECode UiccCard::GetOperatorBrandOverride(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String iccId;
    GetIccId(&iccId);
    if (TextUtils::IsEmpty(iccId)) {
        *result = String(NULL);
        return NOERROR;
    }
    AutoPtr<IPreferenceManagerHelper> hlp;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&hlp);
    AutoPtr<ISharedPreferences> sp;
    hlp->GetDefaultSharedPreferences(mContext, (ISharedPreferences**)&sp);
    sp->GetString(OPERATOR_BRAND_OVERRIDE_PREFIX + iccId, String(NULL), result);
    return NOERROR;
}

ECode UiccCard::GetIccId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ICCID should be same across all the apps.
    Int32 len = mUiccApplications->GetLength();
    String v;
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IUiccCardApplication> app = (*mUiccApplications)[i];
        if (app != NULL) {
            AutoPtr<IIccRecords> ir;
            app->GetIccRecords((IIccRecords**)&ir);
            if (ir != NULL && (ir->GetIccId(&v), v) != NULL) {
                return ir->GetIccId(result);
            }
        }
    }
    *result = String(NULL);
    return NOERROR;
}

ECode UiccCard::GetUICCConfig(
    /* [out] */ IUICCConfig** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUICCConfig;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode UiccCard::OnRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse)
{
    Int32 len = mUiccApplications->GetLength();
    for (Int32 i = 0; i < len; i++) {
        if ((*mUiccApplications)[i] != NULL) {
            // Let the app know that the refresh occurred
            ((UiccCardApplication*)(*mUiccApplications)[i])->OnRefresh(refreshResponse);
        }
    }
    return NOERROR;
}

ECode UiccCard::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("UiccCard:"));
    // pw->Println(String(" mCi=") + mCi);
    pw->Println(String(" mDestroyed=") + StringUtils::ToString(mDestroyed));
    pw->Println(String(" mLastRadioState=") + StringUtils::ToString(mLastRadioState));
    // pw->Println(String(" mCatService=") + mCatService);
    pw->Println(String(" mAbsentRegistrants: size=") + StringUtils::ToString(mAbsentRegistrants->GetSize()));
    for (Int32 i = 0; i < mAbsentRegistrants->GetSize(); i++) {
        AutoPtr<IHandler> hdl;
        IRegistrant::Probe(mAbsentRegistrants->Get(i))->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mAbsentRegistrants[") + StringUtils::ToString(i) +
                String("]=")/* + hdl*/);
    }
    for (Int32 i = 0; i < mCarrierPrivilegeRegistrants->GetSize(); i++) {
        AutoPtr<IHandler> hdl;
        IRegistrant::Probe(mCarrierPrivilegeRegistrants->Get(i))->GetHandler((IHandler**)&hdl);
        pw->Println(String("  mCarrierPrivilegeRegistrants[") + StringUtils::ToString(i) +
                String("]=")/* + hdl*/);
    }
    pw->Println(String(" mCardState=") + StringUtils::ToString(mCardState));
    pw->Println(String(" mUniversalPinState=") + StringUtils::ToString(mUniversalPinState));
    pw->Println(String(" mGsmUmtsSubscriptionAppIndex=") + StringUtils::ToString(mGsmUmtsSubscriptionAppIndex));
    pw->Println(String(" mCdmaSubscriptionAppIndex=") + StringUtils::ToString(mCdmaSubscriptionAppIndex));
    pw->Println(String(" mImsSubscriptionAppIndex=") + StringUtils::ToString(mImsSubscriptionAppIndex));
    pw->Println(String(" mImsSubscriptionAppIndex=") + StringUtils::ToString(mImsSubscriptionAppIndex));
    pw->Println(String(" mUiccApplications: length=") + StringUtils::ToString(mUiccApplications->GetLength()));
    for (Int32 i = 0; i < mUiccApplications->GetLength(); i++) {
        if ((*mUiccApplications)[i] == NULL) {
            pw->Println(String("  mUiccApplications[") + StringUtils::ToString(i)
                + String("]=") + String(NULL));
        }
        else {
            AppType type;
            (*mUiccApplications)[i]->GetType(&type);
            pw->Println(String("  mUiccApplications[")
                + StringUtils::ToString(i) + String("]=")
                + StringUtils::ToString(type) + String(" ")/* + (*mUiccApplications)[i]*/);
        }
    }
    pw->Println();
    // Print details of all applications
    for (Int32 i = 0; i < mUiccApplications->GetLength(); i++) {
        AutoPtr<IUiccCardApplication> app = (*mUiccApplications)[i];
        if (app != NULL) {
            assert(0 && "TODO");
            // app->Dump(fd, pw, args);
            pw->Println();
        }
    }
    // Print details of all IccRecords
    for (Int32 i = 0; i < mUiccApplications->GetLength(); i++) {
        AutoPtr<IUiccCardApplication> app = (*mUiccApplications)[i];
        if (app != NULL) {
            AutoPtr<IIccRecords> ir;
            app->GetIccRecords((IIccRecords**)&ir);
            if (ir != NULL) {
                assert(0 && "TODO");
                // ir->Dump(fd, pw, args);
                pw->Println();
            }
        }
    }
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

void UiccCard::CreateAndUpdateCatService()
{
    AutoPtr<ICatServiceFactory> cf;
    CCatServiceFactory::AcquireSingleton((ICatServiceFactory**)&cf);
    if (mUiccApplications->GetLength() > 0 && (*mUiccApplications)[0] != NULL) {
        // Initialize or Reinitialize CatService
        if (mCatService == NULL) {
            cf->MakeCatService(mCi, mContext, this, mSlotId, (ICatService**)&mCatService);
        }
    }
    else {
        if (mCatService != NULL) {
            cf->DisposeCatService(mSlotId);
        }
        mCatService = NULL;
    }
    Logger::E("leliang", "line:%d, func:%s, mCatService:%p\n", __LINE__, __func__, mCatService.Get());
}

ECode UiccCard::Finalize()
{
    if (DBG) Log(String("UiccCard finalized"));
    return NOERROR;
}

void UiccCard::Log(
    /* [in] */ const String& msg)
{
    Logger::D(LOGTAG, msg);
}

AutoPtr<ArrayOf<IUiccCardApplication*> > UiccCard::MiddleInitMuiccapplications()
{
    AutoPtr<ArrayOf<IUiccCardApplication*> > result =
        ArrayOf<IUiccCardApplication*>::Alloc(IIccCardStatus::CARD_MAX_APPS);
    return result;
}

void UiccCard::SanitizeApplicationIndexes()
{
    mGsmUmtsSubscriptionAppIndex =
            CheckIndex(mGsmUmtsSubscriptionAppIndex, Uicc::APPTYPE_SIM, Uicc::APPTYPE_USIM);
    mCdmaSubscriptionAppIndex =
            CheckIndex(mCdmaSubscriptionAppIndex, Uicc::APPTYPE_RUIM, Uicc::APPTYPE_CSIM);
    mImsSubscriptionAppIndex =
            CheckIndex(mImsSubscriptionAppIndex, Uicc::APPTYPE_ISIM, Uicc::APPTYPE_UNKNOWN);
}

Int32 UiccCard::CheckIndex(
    /* [in] */ Int32 index,
    /* [in] */ AppType expectedAppType,
    /* [in] */ AppType altExpectedAppType)
{
    if (mUiccApplications == NULL || index >= mUiccApplications->GetLength()) {
        Loge(String("App index ") + StringUtils::ToString(index)
                + String(" is invalid since there are no applications"));
        return -1;
    }

    if (index < 0) {
        // This is normal. (i.e. no application of this type)
        return -1;
    }

    AppType t;
    (*mUiccApplications)[index]->GetType(&t);
    if (t != expectedAppType &&
        t != altExpectedAppType) {
        Loge(String("App index ") + StringUtils::ToString(index)
                + String(" is invalid since it's not ") +
                StringUtils::ToString(expectedAppType) + String(" and not ")
                + StringUtils::ToString(altExpectedAppType));
        return -1;
    }

    // Seems to be valid
    return index;
}

void UiccCard::OnIccSwap(
    /* [in] */ Boolean isAdded)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean isHotSwapSupported = FALSE;
    res->GetBoolean(
            R::bool_::config_hotswapCapable, &isHotSwapSupported);

    if (isHotSwapSupported) {
        Log(String("onIccSwap: isHotSwapSupported is true, don't prompt for rebooting"));
        return;
    }
    Log(String("onIccSwap: isHotSwapSupported is false, prompt for rebooting"));

    {
        AutoLock lock(mLock);
        // TODO: Here we assume the device can't handle SIM hot-swap
        //      and has to reboot. We may want to add a property,
        //      e.g. REBOOT_ON_SIM_SWAP, to indicate if modem support
        //      hot-swap.
        AutoPtr<IDialogInterfaceOnClickListener> listener;


        // TODO: SimRecords is not reset while SIM ABSENT (only reset while
        //       Radio_off_or_not_available). Have to reset in both both
        //       added or removed situation.
        listener = new InnerDialogInterfaceOnClickListener1(this);

        AutoPtr<IResourcesHelper> resHlp;
        CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHlp);
        AutoPtr<IResources> r;
        resHlp->GetSystem((IResources**)&r);

        String title;
        if (isAdded) {
            r->GetString(R::string::sim_added_title, &title);
        }
        else {
            r->GetString(R::string::sim_removed_title, &title);
        }
        String message;
        if (isAdded) {
            r->GetString(R::string::sim_added_message, &message);
        }
        else {
            r->GetString(R::string::sim_removed_message, &message);
        }
        String buttonTxt;
        r->GetString(R::string::sim_restart_button, &buttonTxt);

        AutoPtr<IAlertDialogBuilder> dialogBuilder;
        CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&dialogBuilder);
        dialogBuilder->SetTitle(CoreUtils::Convert(title));
        dialogBuilder->SetMessage(CoreUtils::Convert(message));
        dialogBuilder->SetPositiveButton(CoreUtils::Convert(buttonTxt), listener);
        AutoPtr<IAlertDialog> dialog;
        dialogBuilder->Create((IAlertDialog**)&dialog);
        AutoPtr<IWindow> wd;
        IDialog::Probe(dialog)->GetWindow((IWindow**)&wd);
        wd->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
        IDialog::Probe(dialog)->Show();
    }
}

void UiccCard::OnCarrierPriviligesLoadedMessage()
{
    AutoLock lock(mLock);
    mCarrierPrivilegeRegistrants->NotifyRegistrants();
}

void UiccCard::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(LOGTAG, msg);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
