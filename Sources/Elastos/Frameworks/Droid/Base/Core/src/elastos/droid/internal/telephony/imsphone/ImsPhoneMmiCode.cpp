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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/ims/CImsSsInfo.h"
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/internal/telephony/imsphone/CImsPhoneCallTracker.h"
#include "elastos/droid/internal/telephony/imsphone/CImsPhoneMmiCode.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhone.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneMmiCode.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Ims::CImsSsInfo;
using Elastos::Droid::Ims::IImsSsInfo;
using Elastos::Droid::Internal::Telephony::CallForwardInfo;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::PhoneBase;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                           ImsPhoneMmiCode
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhoneMmiCode, Handler, IMmiCode);

const String ImsPhoneMmiCode::LOGTAG("ImsPhoneMmiCode");
const String ImsPhoneMmiCode::UT_BUNDLE_KEY_CLIR("queryClir");
const String ImsPhoneMmiCode::UT_BUNDLE_KEY_SSINFO("imsSsInfo");
const String ImsPhoneMmiCode::CfuTimer("Call Forward Unconditional Timer");
const String ImsPhoneMmiCode::IcbDnMmi("Specific Incoming Call Barring");
const String ImsPhoneMmiCode::IcbAnonymousMmi("Anonymous Incoming Call Barring");
const Int32 ImsPhoneMmiCode::MAX_LENGTH_SHORT_CODE;
const Char16 ImsPhoneMmiCode::END_OF_USSD_COMMAND = '#';
const String ImsPhoneMmiCode::ACTION_ACTIVATE("*");
const String ImsPhoneMmiCode::ACTION_DEACTIVATE("#");
const String ImsPhoneMmiCode::ACTION_INTERROGATE("*#");
const String ImsPhoneMmiCode::ACTION_REGISTER("**");
const String ImsPhoneMmiCode::ACTION_ERASURE("##");
const String ImsPhoneMmiCode::SC_CLIP("30");
const String ImsPhoneMmiCode::SC_CLIR("31");
const String ImsPhoneMmiCode::SC_COLP("76");
const String ImsPhoneMmiCode::SC_COLR("77");
const String ImsPhoneMmiCode::SC_CNAP("300");
const String ImsPhoneMmiCode::SC_CFU("21");
const String ImsPhoneMmiCode::SC_CFB("67");
const String ImsPhoneMmiCode::SC_CFNRy("61");
const String ImsPhoneMmiCode::SC_CFNR("62");
const String ImsPhoneMmiCode::SC_CFUT("22");
const String ImsPhoneMmiCode::SC_CF_All("002");
const String ImsPhoneMmiCode::SC_CF_All_Conditional("004");
const String ImsPhoneMmiCode::SC_WAIT("43");
const String ImsPhoneMmiCode::SC_BAOC("33");
const String ImsPhoneMmiCode::SC_BAOIC("331");
const String ImsPhoneMmiCode::SC_BAOICxH("332");
const String ImsPhoneMmiCode::SC_BAIC("35");
const String ImsPhoneMmiCode::SC_BAICr("351");
const String ImsPhoneMmiCode::SC_BA_ALL("330");
const String ImsPhoneMmiCode::SC_BA_MO("333");
const String ImsPhoneMmiCode::SC_BA_MT("353");
const String ImsPhoneMmiCode::SC_BS_MT("156");
const String ImsPhoneMmiCode::SC_BAICa("157");
const String ImsPhoneMmiCode::SC_PWD("03");
const String ImsPhoneMmiCode::SC_PIN("04");
const String ImsPhoneMmiCode::SC_PIN2("042");
const String ImsPhoneMmiCode::SC_PUK("05");
const String ImsPhoneMmiCode::SC_PUK2("052");
const Int32 ImsPhoneMmiCode::EVENT_SET_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_QUERY_CF_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_USSD_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_QUERY_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_SET_CFF_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_USSD_CANCEL_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_GET_CLIR_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_SUPP_SVC_QUERY_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_QUERY_CFUT_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_SET_CFF_TIMER_COMPLETE;
const Int32 ImsPhoneMmiCode::EVENT_QUERY_ICB_COMPLETE;
const Int32 ImsPhoneMmiCode::NUM_PRESENTATION_ALLOWED;
const Int32 ImsPhoneMmiCode::NUM_PRESENTATION_RESTRICTED;
const Int32 ImsPhoneMmiCode::CLIR_NOT_PROVISIONED;
const Int32 ImsPhoneMmiCode::CLIR_PROVISIONED_PERMANENT;
const Int32 ImsPhoneMmiCode::CLIR_PRESENTATION_RESTRICTED_TEMPORARY;
const Int32 ImsPhoneMmiCode::CLIR_PRESENTATION_ALLOWED_TEMPORARY;
const Int32 ImsPhoneMmiCode::CLIR_DEFAULT;
const Int32 ImsPhoneMmiCode::CLIR_INVOCATION;
const Int32 ImsPhoneMmiCode::CLIR_SUPPRESSION;

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String& str)
{
    AutoPtr<IPatternHelper> patternHelper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&patternHelper);
    AutoPtr<IPattern> pattern;
    patternHelper->Compile(str, (IPattern**)&pattern);
    return pattern;
}

AutoPtr<IPattern> ImsPhoneMmiCode::sPatternSuppService = InitPattern(String("((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)"));
const Int32 ImsPhoneMmiCode::MATCH_GROUP_POUND_STRING;
const Int32 ImsPhoneMmiCode::MATCH_GROUP_ACTION;
const Int32 ImsPhoneMmiCode::MATCH_GROUP_SERVICE_CODE;
const Int32 ImsPhoneMmiCode::MATCH_GROUP_SIA;
const Int32 ImsPhoneMmiCode::MATCH_GROUP_SIB;
const Int32 ImsPhoneMmiCode::MATCH_GROUP_SIC;
const Int32 ImsPhoneMmiCode::MATCH_GROUP_PWD_CONFIRM;
const Int32 ImsPhoneMmiCode::MATCH_GROUP_DIALING_NUMBER;
AutoPtr< ArrayOf<String> > ImsPhoneMmiCode::sTwoDigitNumberPattern;

ImsPhoneMmiCode::ImsPhoneMmiCode()
    : mIsPendingUSSD(FALSE)
    , mIsUssdRequest(FALSE)
    , mIsCallFwdReg(FALSE)
    , mState(IMmiCodeState_PENDING)
{
}

ECode ImsPhoneMmiCode::constructor(
    /* [in] */ IImsPhone* phone)
{
    // The telephony unit-test cases may create ImsPhoneMmiCode's
    // in secondary threads
    AutoPtr<IHandler> handler;
    IPhoneBase::Probe(phone)->GetHandler((IHandler**)&handler);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    Handler::constructor(looper);
    mPhone = phone;
    IPhone::Probe(phone)->GetContext((IContext**)&mContext);
    AutoPtr<IInterface> obj;
    ((PhoneBase*)((ImsPhone*)mPhone.Get())->mDefaultPhone.Get())->mIccRecords->Get((IInterface**)&obj);
    mIccRecords = IIccRecords::Probe(obj);
    return NOERROR;
}

AutoPtr<IImsPhoneMmiCode> ImsPhoneMmiCode::NewFromDialString(
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhone* phone)
{
    AutoPtr<IMatcher> m;
    AutoPtr<CImsPhoneMmiCode> ret;

    sPatternSuppService->Matcher(dialString, (IMatcher**)&m);

    // Is this formatted like a standard supplementary service code?
    AutoPtr<IContext> ctx;
    IPhone::Probe(phone)->GetContext((IContext**)&ctx);
    Boolean b;
    if (m->Matches(&b), b) {
        CImsPhoneMmiCode::NewByFriend(phone, (CImsPhoneMmiCode**)&ret);
        String str;
        ret->mPoundString = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_POUND_STRING, &str), str));
        ret->mAction = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_ACTION, &str), str));
        ret->mSc = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SERVICE_CODE, &str), str));
        ret->mSia = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SIA, &str), str));
        ret->mSib = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SIB, &str), str));
        ret->mSic = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SIC, &str), str));
        ret->mPwd = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_PWD_CONFIRM, &str), str));
        ret->mDialingNumber = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_DIALING_NUMBER, &str), str));
        // According to TS 22.030 6.5.2 "Structure of the MMI",
        // the dialing number should not ending with #.
        // The dialing number ending # is treated as unique USSD,
        // eg, *400#16 digit number# to recharge the prepaid card
        // in India operator(Mumbai MTNL)
        if (ret->mDialingNumber != NULL &&
                ret->mDialingNumber.EndWith("#") &&
                dialString.EndWith("#")){
            CImsPhoneMmiCode::NewByFriend(phone, (CImsPhoneMmiCode**)&ret);
            ret->mPoundString = dialString;
        }
    }
    else if (dialString.EndWith("#")) {
        // TS 22.030 sec 6.5.3.2
        // "Entry of any characters defined in the 3GPP TS 23.038 [8] Default Alphabet
        // (up to the maximum defined in 3GPP TS 24.080 [10]), followed by #SEND".

        CImsPhoneMmiCode::NewByFriend(phone, (CImsPhoneMmiCode**)&ret);
        ret->mPoundString = dialString;
    }
    else if (IsTwoDigitShortCode(ctx, dialString)) {
        //Is a country-specific exception to short codes as defined in TS 22.030, 6.5.3.2
        ret = NULL;
    }
    else if (IsShortCode(dialString, phone)) {
        // this may be a short code, as defined in TS 22.030, 6.5.3.2
        CImsPhoneMmiCode::NewByFriend(phone, (CImsPhoneMmiCode**)&ret);
        ret->mDialingNumber = dialString;
    }

    return IImsPhoneMmiCode::Probe(ret);
}

AutoPtr<IImsPhoneMmiCode> ImsPhoneMmiCode::NewNetworkInitiatedUssd(
    /* [in] */ const String& ussdMessage,
    /* [in] */ Boolean isUssdRequest,
    /* [in] */ IImsPhone* phone)
{
    AutoPtr<CImsPhoneMmiCode> ret;

    CImsPhoneMmiCode::NewByFriend(phone, (CImsPhoneMmiCode**)&ret);

    ret->mMessage = CoreUtils::Convert(ussdMessage);
    ret->mIsUssdRequest = isUssdRequest;

    // If it's a request, set to PENDING so that it's cancelable.
    if (isUssdRequest) {
        ret->mIsPendingUSSD = TRUE;
        ret->mState = IMmiCodeState_PENDING;
    }
    else {
        ret->mState = IMmiCodeState_COMPLETE;
    }

    return IImsPhoneMmiCode::Probe(ret);
}

AutoPtr<IImsPhoneMmiCode> ImsPhoneMmiCode::NewFromUssdUserInput(
    /* [in] */ const String& ussdMessage,
    /* [in] */ IImsPhone* phone)
{
    AutoPtr<CImsPhoneMmiCode> ret;

    CImsPhoneMmiCode::NewByFriend(phone, (CImsPhoneMmiCode**)&ret);

    ret->mMessage = CoreUtils::Convert(ussdMessage);
    ret->mState = IMmiCodeState_PENDING;
    ret->mIsPendingUSSD = TRUE;

    return IImsPhoneMmiCode::Probe(ret);
}

Boolean ImsPhoneMmiCode::IsScMatchesSuppServType(
    /* [in] */ const String& dialString)
{
    Boolean isMatch = FALSE;
    AutoPtr<IMatcher> m;
    sPatternSuppService->Matcher(dialString, (IMatcher**)&m);

    Boolean b;
    if (m->Matches(&b), b) {
        String str;
        String sc = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SERVICE_CODE, &str), str));
        if (sc.Equals(SC_CFUT)) {
            isMatch = TRUE;
        }
        else if(sc.Equals(SC_BS_MT)) {
            isMatch = TRUE;
        }
    }
    return isMatch;
}

Boolean ImsPhoneMmiCode::IsServiceCodeCallForwarding(
    /* [in] */ const String& sc)
{
    return sc != NULL &&
            (sc.Equals(SC_CFU)
            || sc.Equals(SC_CFB) || sc.Equals(SC_CFNRy)
            || sc.Equals(SC_CFNR) || sc.Equals(SC_CF_All)
            || sc.Equals(SC_CF_All_Conditional));
}

Boolean ImsPhoneMmiCode::IsServiceCodeUncondCallFwdTimer(
    /* [in] */ const String& sc)
{
    return !sc.IsNull() && sc.Equals(SC_CFUT);
}

Boolean ImsPhoneMmiCode::IsServiceCodeCallBarring(
    /* [in] */ const String& sc)
{
    AutoPtr<IResources> resource = CResources::GetSystem();

    if (sc != NULL) {
        AutoPtr<ArrayOf<String> > barringMMI;
        resource->GetStringArray(
            R::array::config_callBarringMMI, (ArrayOf<String>**)&barringMMI);
        if (barringMMI != NULL) {
            for (Int32 i = 0; i < barringMMI->GetLength(); i++) {
                String match = (*barringMMI)[i];
                if (sc.Equals(match)) return TRUE;
            }
        }
    }
    return FALSE;
}

ECode ImsPhoneMmiCode::ScToBarringFacility(
    /* [in] */ const String& sc,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (sc == NULL) {
        // throw new RuntimeException ("invalid call barring sc");
        return E_RUNTIME_EXCEPTION;
    }

    if (sc.Equals(SC_BAOC)) {
        *result = ICommandsInterface::CB_FACILITY_BAOC;
        return NOERROR;
    }
    else if (sc.Equals(SC_BAOIC)) {
        *result = ICommandsInterface::CB_FACILITY_BAOIC;
        return NOERROR;
    }
    else if (sc.Equals(SC_BAOICxH)) {
        *result = ICommandsInterface::CB_FACILITY_BAOICxH;
        return NOERROR;
    }
    else if (sc.Equals(SC_BAIC)) {
        *result = ICommandsInterface::CB_FACILITY_BAIC;
        return NOERROR;
    }
    else if (sc.Equals(SC_BAICr)) {
        *result = ICommandsInterface::CB_FACILITY_BAICr;
        return NOERROR;
    }
    else if (sc.Equals(SC_BA_ALL)) {
        *result = ICommandsInterface::CB_FACILITY_BA_ALL;
        return NOERROR;
    }
    else if (sc.Equals(SC_BA_MO)) {
        *result = ICommandsInterface::CB_FACILITY_BA_MO;
        return NOERROR;
    }
    else if (sc.Equals(SC_BA_MT)) {
        *result = ICommandsInterface::CB_FACILITY_BA_MT;
        return NOERROR;
    }
    else {
        // throw new RuntimeException ("invalid call barring sc");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetState(
    /* [out] */ IMmiCodeState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetMessage(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMessage;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhone::Probe(mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneMmiCode::Cancel()
{
    // Complete or failed cannot be cancelled
    if (mState == IMmiCodeState_COMPLETE || mState == IMmiCodeState_FAILED) {
        return NOERROR;
    }

    mState = IMmiCodeState_CANCELLED;

    if (mIsPendingUSSD) {
        ((ImsPhone*)mPhone.Get())->CancelUSSD();
    }
    else {
        ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsCancelable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    /* Can only cancel pending USSD sessions. */
    *result = mIsPendingUSSD;
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetDialingNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDialingNumber;
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsMMI(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPoundString != NULL;
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsShortCode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPoundString == NULL
                && mDialingNumber != NULL && mDialingNumber.GetLength() <= 2;
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsPinPukCommand(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSc != NULL && (mSc.Equals(SC_PIN) || mSc.Equals(SC_PIN2)
                          || mSc.Equals(SC_PUK) || mSc.Equals(SC_PUK2));
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsTemporaryModeCLIR(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b1, b2;
    *result = mSc != NULL && mSc.Equals(SC_CLIR) && mDialingNumber != NULL
            && ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2));
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetCLIRMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mSc != NULL && mSc.Equals(SC_CLIR)) {
        Boolean b;
        if (IsActivate(&b), b) {
            *result = ICommandsInterface::CLIR_SUPPRESSION;
            return NOERROR;
        }
        else if (IsDeactivate(&b), b) {
            *result = ICommandsInterface::CLIR_INVOCATION;
            return NOERROR;
        }
    }

    *result = ICommandsInterface::CLIR_DEFAULT;
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsActivate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAction != NULL && mAction.Equals(ACTION_ACTIVATE);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsDeactivate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAction != NULL && mAction.Equals(ACTION_DEACTIVATE);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsInterrogate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAction != NULL && mAction.Equals(ACTION_INTERROGATE);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsRegister(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAction != NULL && mAction.Equals(ACTION_REGISTER);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsErasure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAction != NULL && mAction.Equals(ACTION_ERASURE);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsPendingUSSD(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsPendingUSSD;
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsUssdRequest(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsUssdRequest;
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsSupportedOverImsPhone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    if (IsShortCode(&b), b) {
        *result = TRUE;
        return NOERROR;
    }
    else if (!mDialingNumber.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }
    else if (IsServiceCodeCallForwarding(mSc)
            || IsServiceCodeCallBarring(mSc)
            || (mSc != NULL && mSc.Equals(SC_WAIT))
            || (mSc != NULL && mSc.Equals(SC_CLIR))
            || (mSc != NULL && mSc.Equals(SC_CLIP))
            || (mSc != NULL && mSc.Equals(SC_COLR))
            || (mSc != NULL && mSc.Equals(SC_COLP))
            || (mSc != NULL && mSc.Equals(SC_BS_MT))
            || (mSc != NULL && mSc.Equals(SC_BAICa))) {

        Int32 serviceClass;
        SiToServiceClass(mSib, &serviceClass);
        if (serviceClass != ICommandsInterface::SERVICE_CLASS_NONE
                && serviceClass != ICommandsInterface::SERVICE_CLASS_VOICE) {
            *result = FALSE;
            return NOERROR;
        }
        *result = TRUE;
        return NOERROR;
    }
    else if ((IsPinPukCommand(&b), b)
            || (mSc != NULL
                && (mSc.Equals(SC_PWD) || mSc.Equals(SC_CLIP) || mSc.Equals(SC_CLIR)))) {
        *result = FALSE;
        return NOERROR;
    }
    else if (mPoundString != NULL) {
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneMmiCode::CalForwardAction(
    /* [in] */ const String& dialingNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    if (IsActivate(&b), b) {
        // 3GPP TS 22.030 6.5.2
        // a call forwarding request with a single * would be
        // interpreted as registration if containing a forwarded-to
        // number, or an activation if not
        if (IsEmptyOrNull(CoreUtils::Convert(dialingNumber))) {
            mIsCallFwdReg = FALSE;
            *result = ICommandsInterface::CF_ACTION_ENABLE;
            return NOERROR;
        }
        else {
            mIsCallFwdReg = TRUE;
            *result = ICommandsInterface::CF_ACTION_REGISTRATION;
            return NOERROR;
        }
    }
    else if (IsDeactivate(&b), b) {
        *result = ICommandsInterface::CF_ACTION_DISABLE;
        return NOERROR;
    }
    else if (IsRegister(&b), b) {
        *result = ICommandsInterface::CF_ACTION_REGISTRATION;
        return NOERROR;
    }
    else if (IsErasure(&b), b) {
        *result = ICommandsInterface::CF_ACTION_ERASURE;
        return NOERROR;
    }
    else {
        // throw new RuntimeException ("invalid action");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::CallBarrAction(
    /* [in] */ const String& dialingNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    if (IsActivate(&b), b) {
        *result = ICommandsInterface::CF_ACTION_ENABLE;
        return NOERROR;
    }
    else if (IsDeactivate(&b), b) {
        *result = ICommandsInterface::CF_ACTION_DISABLE;
        return NOERROR;
    }
    else if (IsRegister(&b), b) {
        if (!IsEmptyOrNull(CoreUtils::Convert(dialingNumber))) {
            *result = ICommandsInterface::CF_ACTION_REGISTRATION;
            return NOERROR;
        }
        else {
            // throw new RuntimeException ("invalid action");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (IsErasure(&b), b) {
        *result = ICommandsInterface::CF_ACTION_ERASURE;
        return NOERROR;
    }
    else {
        // throw new RuntimeException ("invalid action");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::ProcessCode()
{
    // try {
    Boolean b;
    if (IsShortCode(&b), b) {
        Logger::D(LOGTAG, "isShortCode");

        // These just get treated as USSD.
        Logger::D(LOGTAG, "Sending short code '%s' over CS pipe.", mDialingNumber.string());
        // throw new CallStateException(ImsPhone.CS_FALLBACK);
        return E_CALL_STATE_EXCEPTION;
    }
    else if (IsServiceCodeCallForwarding(mSc)) {
        Logger::D(LOGTAG, "is CF");
        // service group is not supported

        String dialingNumber = mSia;
        Int32 reason;
        ScToCallForwardReason(mSc, &reason);
        Int32 time = SiToTime(mSic);

        if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_CF_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            IPhone::Probe(mPhone)->GetCallForwardingOption(reason, msg);
        }
        else {
            Int32 cfAction;

            CalForwardAction(dialingNumber, &cfAction);

            Int32 isSettingUnconditional =
                    ((reason == ICommandsInterface::CF_REASON_UNCONDITIONAL) ||
                     (reason == ICommandsInterface::CF_REASON_ALL)) ? 1 : 0;

            Int32 isEnableDesired =
                ((cfAction == ICommandsInterface::CF_ACTION_ENABLE) ||
                        (cfAction == ICommandsInterface::CF_ACTION_REGISTRATION)) ? 1 : 0;

            Logger::D(LOGTAG, "is CF setCallForward");
            AutoPtr<IMessage> msg;
            ObtainMessage(
                    EVENT_SET_CFF_COMPLETE,
                    isSettingUnconditional,
                    isEnableDesired, TO_IINTERFACE(this), (IMessage**)&msg);
            IPhone::Probe(mPhone)->SetCallForwardingOption(cfAction, reason,
                    dialingNumber, time, msg);
        }
    }
    else if (IsServiceCodeUncondCallFwdTimer(mSc)) {
        Logger::D(LOGTAG, "is UncondCFTimer");

        String dialingNumber = mSia;
        Int32 reason;
        ScToCallForwardReason(mSc, &reason);

        if (IsInterrogate(&b), b) {

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_CFUT_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            IPhone::Probe(mPhone)->GetCallForwardingOption(reason, msg);
        }
        else {
            Int32 cfTimerAction;
            Int32 startHour = 0;
            Int32 startMinute = 0;
            Int32 endHour = 0;
            Int32 endMinute = 0;
            if (mSic != NULL) {
                // sic contains string in the format HH:MM$HH:MM.String
                // will be splitted by '$' that gives two halves.First half
                // signifies starthour and startminute and second half
                // signifies endhour and endminute.
                AutoPtr<ArrayOf<String> > duration;
                StringUtils::Split(mSic, String("\\$"), (ArrayOf<String>**)&duration);

                if(((*duration)[0] !=NULL) && ((*duration)[1]!=NULL)) {
                    //splitting to get starthour and startminute
                    AutoPtr<ArrayOf<String> > start;
                    StringUtils::Split((*duration)[0], String("\\:"), (ArrayOf<String>**)&start);
                    if ((*start)[0] != NULL) {
                        startHour = StringUtils::ParseInt32((*start)[0]);
                    }
                    if ((*start)[1] != NULL) {
                        startMinute = StringUtils::ParseInt32((*start)[1]);
                    }
                    //splitting to get endhour and endminute
                    AutoPtr<ArrayOf<String> > end;
                    StringUtils::Split((*duration)[1], String("\\:"), (ArrayOf<String>**)&end);
                    if ((*end)[0] !=NULL) {
                        endHour = StringUtils::ParseInt32((*end)[0]);
                    }
                    if ((*end)[1] !=NULL) {
                        endMinute = StringUtils::ParseInt32((*end)[1]);
                    }
                }
            }
            CalForwardAction(dialingNumber, &cfTimerAction);
            Logger::D(LOGTAG, "is CFUT setCallForward");
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_CFF_TIMER_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            IPhone::Probe(mPhone)->SetCallForwardingUncondTimerOption(startHour,
                    startMinute, endHour, endMinute, cfTimerAction,
                    reason, dialingNumber, msg);
        }
    }
    else if (IsServiceCodeCallBarring(mSc)) {
        // sia = password
        // sib = basic service group
        // service group is not supported

        String password = mSia;
        String facility;
        ScToBarringFacility(mSc, &facility);

        Boolean b1, b2;
        if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((ImsPhone*)mPhone.Get())->GetCallBarring(facility, msg);
        }
        else if ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2)) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((ImsPhone*)mPhone.Get())->SetCallBarring(facility, b1, password, msg);
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mSc != NULL && mSc.Equals(SC_CLIR)) {
        // NOTE: Since these supplementary services are accessed only
        //       via MMI codes, methods have not been added to ImsPhone.
        //       Only the UT interface handle is used.
        if (IsActivate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCLIR(ICommandsInterface::CLIR_INVOCATION, msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for updateCLIR.");
            // }
        }
        else if (IsDeactivate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCLIR(ICommandsInterface::CLIR_SUPPRESSION, msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for updateCLIR.");
            // }
        }
        else if (IsInterrogate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_CLIR_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->QueryCLIR(msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for queryCLIR.");
            // }
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mSc != NULL && mSc.Equals(SC_CLIP)) {
        // NOTE: Refer to the note above.
        Boolean b1, b2;
        if (IsInterrogate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->QueryCLIP(msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for queryCLIP.");
            // }
        }
        else if ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2)) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCLIP(b1, msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for updateCLIP.");
            // }
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mSc != NULL && mSc.Equals(SC_COLP)) {
        // NOTE: Refer to the note above.
        Boolean b1, b2;
        if (IsInterrogate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->QueryCOLP(msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for queryCOLP.");
            // }
        }
        else if ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2)) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCOLP(b1, msg);
             // } catch (ImsException e) {
             //     Logger::D(LOGTAG, "Could not get UT handle for updateCOLP.");
             // }
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mSc != NULL && mSc.Equals(SC_COLR)) {
        // NOTE: Refer to the note above.
        if (IsActivate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCOLR(NUM_PRESENTATION_RESTRICTED, msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for updateCOLR.");
            // }
        }
        else if (IsDeactivate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCOLR(NUM_PRESENTATION_ALLOWED, msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for updateCOLR.");
            // }
        }
        else if (IsInterrogate(&b), b) {
            // try {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->QueryCOLR(msg);
            // } catch (ImsException e) {
            //     Logger::D(LOGTAG, "Could not get UT handle for queryCOLR.");
            // }
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mSc != NULL && (mSc.Equals(SC_BS_MT))) {
        // try {
        if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_ICB_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->QueryCallBarring(IImsUtInterface::CB_BS_MT, msg);
        }
        else {
            ProcessIcbMmiCodeForUpdate();
        }
         // TODO: IsRegister() case needs to be handled.
        // } catch (ImsException e) {
        //     Logger::D(LOGTAG, "Could not get UT handle for ICB.");
        // }
    }
    else if (mSc != NULL && mSc.Equals(SC_BAICa)) {
        Int32 callAction = 0;
        // TODO: Should we route through queryCallBarring() here?
        // try {
        if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->QueryCallBarring(IImsUtInterface::CB_BIC_ACR, msg);
        }
        else {
            if (IsActivate(&b), b) {
                callAction = ICommandsInterface::CF_ACTION_ENABLE;
            }
            else if (IsDeactivate(&b), b) {
                callAction = ICommandsInterface::CF_ACTION_DISABLE;
            }
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
            // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCallBarring(IImsUtInterface::CB_BIC_ACR, callAction, msg, NULL);
        }
        // } catch (ImsException e) {
        //     Logger::D(LOGTAG, "Could not get UT handle for ICBa.");
        // }
    }
    else if (mSc != NULL && mSc.Equals(SC_WAIT)) {
        // sia = basic service group
        // service group is not supported
        Boolean b1, b2;
        if ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2)) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            IPhone::Probe(mPhone)->SetCallWaiting(b1, msg);
        }
        else if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            IPhone::Probe(mPhone)->GetCallWaiting(msg);
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mPoundString != NULL) {
        Logger::D(LOGTAG, "Sending pound string '%s' over CS pipe.", mDialingNumber.string());
        // throw new CallStateException(ImsPhone.CS_FALLBACK);
        return E_CALL_STATE_EXCEPTION;
    }
    else {
        // throw new RuntimeException ("Invalid or Unsupported MMI Code");
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (RuntimeException exc) {
    //     mState = IMmiCodeState_FAILED;
    //     mMessage = mContext->GetText(R::string::mmiError);
    //     ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
    // }
    return NOERROR;
}

ECode ImsPhoneMmiCode::OnUssdFinished(
    /* [in] */ const String& ussdMessage,
    /* [in] */ Boolean isUssdRequest)
{
    if (mState == IMmiCodeState_PENDING) {
        if (ussdMessage == NULL) {
            mContext->GetText(R::string::mmiComplete, (ICharSequence**)&mMessage);
        }
        else {
            mMessage = CoreUtils::Convert(ussdMessage);
        }
        mIsUssdRequest = isUssdRequest;
        // If it's a request, leave it PENDING so that it's cancelable.
        if (!isUssdRequest) {
            mState = IMmiCodeState_COMPLETE;
        }

        ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::OnUssdFinishedError()
{
    if (mState == IMmiCodeState_PENDING) {
        mState = IMmiCodeState_FAILED;
        mContext->GetText(R::string::mmiError, (ICharSequence**)&mMessage);

        ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::SendUssd(
    /* [in] */ const String& ussdMessage)
{
    // Treat this as a USSD string
    mIsPendingUSSD = TRUE;

    // Note that unlike most everything else, the USSD complete
    // response does not complete this MMI code...we wait for
    // an unsolicited USSD "Notify" or "Request".
    // The matching up of this is done in ImsPhone.

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_USSD_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
    ((ImsPhone*)mPhone.Get())->SendUSSD(ussdMessage, msg);
    return NOERROR;
}

ECode ImsPhoneMmiCode::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_SET_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            OnSetComplete(msg, ar);
            break;

        case EVENT_SET_CFF_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            /*
            * msg.arg1 = 1 means to set unconditional voice call forwarding
            * msg.arg2 = 1 means to enable voice call forwarding
            */
            Int32 arg1;
            msg->GetArg1(&arg1);
            Int32 arg2;
            msg->GetArg2(&arg2);
            if ((ar->mException == NULL) && (arg1 == 1)) {
                Boolean cffEnabled = (arg2 == 1);
                if (mIccRecords != NULL) {
                    mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, mDialingNumber);
                }
            }

            OnSetComplete(msg, ar);
            break;

        case EVENT_SET_CFF_TIMER_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnSetComplete(msg, ar);
            break;

        case EVENT_QUERY_CF_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnQueryCfComplete(ar);
            break;

        case EVENT_QUERY_CFUT_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnQueryUncondCfTimerComplete(ar);
            break;

        case EVENT_QUERY_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnQueryComplete(ar);
            break;

        case EVENT_USSD_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                mState = IMmiCodeState_FAILED;
                mMessage = GetErrorMessage(ar);

                ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
            }

            // Note that unlike most everything else, the USSD complete
            // response does not complete this MMI code...we wait for
            // an unsolicited USSD "Notify" or "Request".
            // The matching up of this is done in ImsPhone.

            break;

        case EVENT_USSD_CANCEL_COMPLETE:
            ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
            break;

        case EVENT_SUPP_SVC_QUERY_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnSuppSvcQueryComplete(ar);
            break;

        case EVENT_QUERY_ICB_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnIcbQueryComplete(ar);
            break;

        case EVENT_GET_CLIR_COMPLETE:
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            OnQueryClirComplete(ar);
            break;

        default:
            break;
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb("ImsPhoneMmiCode {");

    IMmiCodeState state;
    GetState(&state);
    sb.Append(String("State=") + StringUtils::ToString(state));
    if (!mAction.IsNull()) sb.Append(String(" action=") + mAction);
    if (!mSc.IsNull()) sb.Append(String(" sc=") + mSc);
    if (!mSia.IsNull()) sb.Append(String(" sia=") + mSia);
    if (!mSib.IsNull()) sb.Append(String(" sib=") + mSib);
    if (!mSic.IsNull()) sb.Append(String(" sic=") + mSic);
    if (!mPoundString.IsNull()) sb.Append(String(" poundString=") + mPoundString);
    if (!mDialingNumber.IsNull()) sb.Append(String(" dialingNumber=") + mDialingNumber);
    if (!mPwd.IsNull()) sb.Append(String(" pwd=") + mPwd);
    sb.Append("}");
    return sb.ToString(result);
}

String ImsPhoneMmiCode::MakeEmptyNull(
    /* [in] */ const String& s)
{
    if (!s.IsNull() && s.GetLength() == 0) return String(NULL);

    return s;
}

Boolean ImsPhoneMmiCode::IsEmptyOrNull(
    /* [in] */ ICharSequence* s)
{
    Int32 len;
    return s == NULL || ((s->GetLength(&len), len) == 0);
}

ECode ImsPhoneMmiCode::ScToCallForwardReason(
    /* [in] */ const String& sc,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (sc == NULL) {
        // throw new RuntimeException ("invalid call forward sc");
        return E_RUNTIME_EXCEPTION;
    }

    if (sc.Equals(SC_CF_All)) {
       *result = ICommandsInterface::CF_REASON_ALL;
       return NOERROR;
    }
    else if ((sc.Equals(SC_CFU)) || (sc.Equals(SC_CFUT))) {
        *result = ICommandsInterface::CF_REASON_UNCONDITIONAL;
        return NOERROR;
    }
    else if (sc.Equals(SC_CFB)) {
        *result = ICommandsInterface::CF_REASON_BUSY;
        return NOERROR;
    }
    else if (sc.Equals(SC_CFNR)) {
        *result = ICommandsInterface::CF_REASON_NOT_REACHABLE;
        return NOERROR;
    }
    else if (sc.Equals(SC_CFNRy)) {
        *result = ICommandsInterface::CF_REASON_NO_REPLY;
        return NOERROR;
    }
    else if (sc.Equals(SC_CF_All_Conditional)) {
       *result = ICommandsInterface::CF_REASON_ALL_CONDITIONAL;
       return NOERROR;
    }
    else {
        // throw new RuntimeException ("invalid call forward sc");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode ImsPhoneMmiCode::SiToServiceClass(
    /* [in] */ const String& si,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (si == NULL || si.GetLength() == 0) {
            *result =  ICommandsInterface::SERVICE_CLASS_NONE;
            return NOERROR;
    }
    else {
        // NumberFormatException should cause MMI fail
        Int32 serviceCode = StringUtils::ParseInt32(si, 10);

        switch (serviceCode) {
            case 10:
                *result = ICommandsInterface::SERVICE_CLASS_SMS + ICommandsInterface::SERVICE_CLASS_FAX  + ICommandsInterface::SERVICE_CLASS_VOICE;
                return NOERROR;
            case 11:
                *result = ICommandsInterface::SERVICE_CLASS_VOICE;
                return NOERROR;
            case 12:
                *result = ICommandsInterface::SERVICE_CLASS_SMS + ICommandsInterface::SERVICE_CLASS_FAX;
                return NOERROR;
            case 13:
                *result = ICommandsInterface::SERVICE_CLASS_FAX;
                return NOERROR;

            case 16:
                *result = ICommandsInterface::SERVICE_CLASS_SMS;
                return NOERROR;

            case 19:
                *result = ICommandsInterface::SERVICE_CLASS_FAX + ICommandsInterface::SERVICE_CLASS_VOICE;
                return NOERROR;

            case 20:
                *result = ICommandsInterface::SERVICE_CLASS_DATA_ASYNC + ICommandsInterface::SERVICE_CLASS_DATA_SYNC;
                return NOERROR;

            case 21:
                *result = ICommandsInterface::SERVICE_CLASS_PAD + ICommandsInterface::SERVICE_CLASS_DATA_ASYNC;
                return NOERROR;
            case 22:
                *result = ICommandsInterface::SERVICE_CLASS_PACKET + ICommandsInterface::SERVICE_CLASS_DATA_SYNC;
                return NOERROR;
            case 24:
                *result = ICommandsInterface::SERVICE_CLASS_DATA_SYNC;
                return NOERROR;
            case 25:
                *result = ICommandsInterface::SERVICE_CLASS_DATA_ASYNC;
                return NOERROR;
            case 26:
                *result = ICommandsInterface::SERVICE_CLASS_DATA_SYNC + ICommandsInterface::SERVICE_CLASS_VOICE;
                return NOERROR;
            case 99:
                *result = ICommandsInterface::SERVICE_CLASS_PACKET;
                return NOERROR;

            default:
                // throw new RuntimeException("unsupported MMI service code " + si);
                return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

Int32 ImsPhoneMmiCode::SiToTime(
    /* [in] */ const String& si)
{
    if (si == NULL || si.GetLength() == 0) {
        return 0;
    }
    else {
        // NumberFormatException should cause MMI fail
        return StringUtils::ParseInt32(si, 10);
    }
}

Boolean ImsPhoneMmiCode::IsTwoDigitShortCode(
    /* [in] */ IContext* context,
    /* [in] */ const String& dialString)
{
    Logger::D(LOGTAG, "isTwoDigitShortCode");

    if (dialString.IsNull() || dialString.GetLength() > 2) return FALSE;

    if (sTwoDigitNumberPattern == NULL) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetStringArray(R::array::config_twoDigitNumberPattern,
                (ArrayOf<String>**)&sTwoDigitNumberPattern);
    }

    for (Int32 i = 0; i < sTwoDigitNumberPattern->GetLength(); i++) {
        String dialnumber = (*sTwoDigitNumberPattern)[i];
        Logger::D(LOGTAG, "Two Digit Number Pattern %s", dialnumber.string());
        if (dialString.Equals(dialnumber)) {
            Logger::D(LOGTAG, "Two Digit Number Pattern -TRUE");
            return TRUE;
        }
    }
    Logger::D(LOGTAG, "Two Digit Number Pattern -FALSE");
    return FALSE;
}

Boolean ImsPhoneMmiCode::IsShortCode(
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhone* phone)
{
    // Refer to TS 22.030 Figure 3.5.3.2:
    if (dialString.IsNull()) {
        return FALSE;
    }

    // Illegal dial string characters will give a ZERO length.
    // At this point we do not want to crash as any application with
    // call privileges may send a non dial string.
    // It return FALSE as when the dialString is equal to NULL.
    if (dialString.GetLength() == 0) {
        return FALSE;
    }

    AutoPtr<IContext> ctx;
    IPhone::Probe(phone)->GetContext((IContext**)&ctx);
    Boolean b;
    if (PhoneNumberUtils::IsLocalEmergencyNumber(ctx, dialString, &b), b) {
        return FALSE;
    }
    else {
        return IsShortCodeUSSD(dialString, phone);
    }
    return FALSE;
}

Boolean ImsPhoneMmiCode::IsShortCodeUSSD(
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhone* phone)
{
    if (!dialString.IsNull() && dialString.GetLength() <= MAX_LENGTH_SHORT_CODE) {
        Boolean b;
        if (((ImsPhone*)phone)->IsInCall(&b), b) {
            return TRUE;
        }

        if (dialString.GetLength() != MAX_LENGTH_SHORT_CODE ||
                dialString.GetChar(0) != '1') {
            return TRUE;
        }
    }
    return FALSE;
}

void ImsPhoneMmiCode::ProcessIcbMmiCodeForUpdate()
{
    String dialingNumber = mSia;
    AutoPtr<ArrayOf<String> > icbNum;
    Int32 callAction;
    if (!dialingNumber.IsNull()) {
        StringUtils::Split(dialingNumber, String("\\$"), (ArrayOf<String>**)&icbNum);
    }
    CallBarrAction(dialingNumber, &callAction);

    // try {
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
// TODO: Need IImsUtInterface
    // (ImsPhoneCallTracker*)(((ImsPhone*)mPhone.Get())->mCT.Get())->GetUtInterface()->UpdateCallBarring(IImsUtInterface::CB_BS_MT, callAction, msg, icbNum);
    // } catch (ImsException e) {
    //     Logger::D(LOGTAG, "Could not get UT handle for updating ICB.");
    // }
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::GetErrorMessage(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs;
    mContext->GetText(R::string::mmiError, (ICharSequence**)&cs);
    return cs;
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::GetScString()
{
    AutoPtr<ICharSequence> cs;
    if (mSc != NULL) {
        if (IsServiceCodeCallBarring(mSc)) {
            mContext->GetText(R::string::BaMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (IsServiceCodeCallForwarding(mSc)) {
            mContext->GetText(R::string::CfMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_PWD)) {
            mContext->GetText(R::string::PwdMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_WAIT)) {
            mContext->GetText(R::string::CwMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_CLIP)) {
            mContext->GetText(R::string::ClipMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_CLIR)) {
            mContext->GetText(R::string::ClirMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_COLP)) {
            mContext->GetText(R::string::ColpMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_COLR)) {
            mContext->GetText(R::string::ColrMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_CFUT)) {
            cs = CoreUtils::Convert(CfuTimer);
            return cs;
        }
        else if (mSc.Equals(SC_BS_MT)) {
            cs = CoreUtils::Convert(IcbDnMmi);
            return cs;
        }
        else if (mSc.Equals(SC_BAICa)) {
            cs = CoreUtils::Convert(IcbAnonymousMmi);
            return cs;
        }
    }

    cs = CoreUtils::Convert(String(""));
    return cs;
}

void ImsPhoneMmiCode::OnSetComplete(
    /* [in] */ IMessage* msg,
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    Boolean b;
    cs = NULL;
    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;

        if (ICommandException::Probe(ar->mException) != NULL) {
// TODO: Need ICommandException::Error
            // CommandException.Error err = ((CommandException)(ar->mException)).getCommandError();
            // if (err == CommandException.Error.PASSWORD_INCORRECT) {
            //     sb.Append(mContext->GetText(R::string::passwordIncorrect));
            // }
            // else {
            //     sb.Append(mContext->GetText(R::string::mmiError));
            // }
        }
        else {
// TODO: Need ImsException
            // ImsException error = (ImsException) ar->mException;
            // if (error.getMessage() != NULL) {
            //     sb.Append(error.getMessage());
            // }
            // else {
            //     sb.Append(getErrorMessage(ar));
            // }
        }
    }
    else if (IsActivate(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        if (mIsCallFwdReg) {
            cs = NULL;
            sb.Append((mContext->GetText(R::string::serviceRegistered, (ICharSequence**)&cs), cs));
        }
        else {
            cs = NULL;
            sb.Append((mContext->GetText(R::string::serviceEnabled, (ICharSequence**)&cs), cs));
        }
    }
    else if (IsDeactivate(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        cs = NULL;
        sb.Append((mContext->GetText(R::string::serviceDisabled, (ICharSequence**)&cs), cs));
    }
    else if (IsRegister(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        cs = NULL;
        sb.Append((mContext->GetText(R::string::serviceRegistered, (ICharSequence**)&cs), cs));
    }
    else if (IsErasure(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        cs = NULL;
        sb.Append((mContext->GetText(R::string::serviceErased, (ICharSequence**)&cs), cs));
    }
    else {
        mState = IMmiCodeState_FAILED;
        cs = NULL;
        sb.Append((mContext->GetText(R::string::mmiError, (ICharSequence**)&cs), cs));
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::ServiceClassToCFString(
    /* [in] */ Int32 serviceClass)
{
    AutoPtr<ICharSequence> cs;
    switch (serviceClass) {
        case ICommandsInterface::SERVICE_CLASS_VOICE:
            mContext->GetText(R::string::serviceClassVoice, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_DATA:
            mContext->GetText(R::string::serviceClassData, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_FAX:
            mContext->GetText(R::string::serviceClassFAX, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_SMS:
            mContext->GetText(R::string::serviceClassSMS, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_DATA_SYNC:
            mContext->GetText(R::string::serviceClassDataSync, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_DATA_ASYNC:
            mContext->GetText(R::string::serviceClassDataAsync, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_PACKET:
            mContext->GetText(R::string::serviceClassPacket, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_PAD:
            mContext->GetText(R::string::serviceClassPAD, (ICharSequence**)&cs);
            return cs;
        default:
            return NULL;
    }
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::MakeCFQueryResultMessage(
    /* [in] */ ICallForwardInfo* _info,
    /* [in] */ Int32 serviceClassMask)
{
    AutoPtr<ICharSequence> cs;
    AutoPtr<ArrayOf<String> > sources = ArrayOf<String>::Alloc(3);
    (*sources)[0] = String("{0}");
    (*sources)[1] = String("{1}");
    (*sources)[2] = String("{2}");

    AutoPtr<ArrayOf<ICharSequence*> > destinations = ArrayOf<ICharSequence*>::Alloc(3);
    Boolean needTimeTemplate;

    // CF_REASON_NO_REPLY also has a time value associated with
    // it. All others don't.

    AutoPtr<CallForwardInfo> info = (CallForwardInfo*)_info;
    needTimeTemplate =
        (info->mReason == ICommandsInterface::CF_REASON_NO_REPLY);

    if (info->mStatus == 1) {
        if (needTimeTemplate) {
            mContext->GetText(R::string::cfTemplateForwardedTime, (ICharSequence**)&cs);
        }
        else {
            mContext->GetText(R::string::cfTemplateForwarded, (ICharSequence**)&cs);
        }
    }
    else if (info->mStatus == 0 && IsEmptyOrNull(CoreUtils::Convert(info->mNumber))) {
        mContext->GetText(R::string::cfTemplateNotForwarded, (ICharSequence**)&cs);
    }
    else { /* (info->mStatus == 0) && !IsEmptyOrNull(info->mNumber) */
        // A call forward record that is not active but contains
        // a phone number is considered "registered"

        if (needTimeTemplate) {
            mContext->GetText(R::string::cfTemplateRegisteredTime, (ICharSequence**)&cs);
        }
        else {
            mContext->GetText(R::string::cfTemplateRegistered, (ICharSequence**)&cs);
        }
    }

    // In the template (from strings.xmls)
    //         {0} is one of "bearerServiceCode*"
    //        {1} is dialing number
    //      {2} is time in seconds

    (*destinations)[0] = ServiceClassToCFString(info->mServiceClass & serviceClassMask);
    String str;
    PhoneNumberUtils::StringFromStringAndTOA(info->mNumber, info->mToa, &str);
    (*destinations)[1] = CoreUtils::Convert(str);
    (*destinations)[2] = CoreUtils::Convert(StringUtils::ToString(info->mTimeSeconds));

    if (info->mReason == ICommandsInterface::CF_REASON_UNCONDITIONAL &&
            (info->mServiceClass & serviceClassMask)
                    == ICommandsInterface::SERVICE_CLASS_VOICE) {
        Boolean cffEnabled = (info->mStatus == 1);
        if (mIccRecords != NULL) {
            mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, info->mNumber);
        }
    }

    return TextUtils::Replace(cs, sources, destinations);
}

void ImsPhoneMmiCode::OnQueryCfComplete(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;

// TODO: Need ImsException
        // if (ar->mException instanceof ImsException) {
        //     ImsException error = (ImsException) ar->mException;
        //     if (error.getMessage() != NULL) {
        //         sb.Append(error.getMessage());
        //     }
        //     else {
        //         sb.Append(getErrorMessage(ar));
        //     }
        // }
        // else {
        //     sb.Append(getErrorMessage(ar));
        // }
    }
    else {
        AutoPtr<IArrayOf> infos = IArrayOf::Probe(ar->mResult);     // CallForwardInfo infos[]

        Int32 size;
        infos->GetLength(&size);
        if (size == 0) {
            // Assume the default is not active
            cs = NULL;
            sb.Append(mContext->GetText(R::string::serviceDisabled, (ICharSequence**)&cs));

            // Set unconditional CFF in SIM to FALSE
            if (mIccRecords != NULL) {
                mIccRecords->SetVoiceCallForwardingFlag(1, FALSE, String(NULL));
            }
        }
        else {
            AutoPtr<ISpannableStringBuilder> tb;
            CSpannableStringBuilder::New((ISpannableStringBuilder**)&tb);

            // Each bit in the service class gets its own result line
            // The service classes may be split up over multiple
            // CallForwardInfos. So, for each service class, find out
            // which CallForwardInfo represents it and then build
            // the response text based on that

            for (Int32 serviceClassMask = 1
                        ; serviceClassMask <= ICommandsInterface::SERVICE_CLASS_MAX
                        ; serviceClassMask <<= 1
            ) {
                for (Int32 i = 0, s = size; i < s ; i++) {
                    AutoPtr<IInterface> obj;
                    infos->Get(i, (IInterface**)&obj);
                    AutoPtr<ICallForwardInfo> info = ICallForwardInfo::Probe(obj);
                    if ((serviceClassMask & ((CallForwardInfo*)info.Get())->mServiceClass) != 0) {
                        IAppendable::Probe(tb)->Append(MakeCFQueryResultMessage(info,
                                        serviceClassMask));
                        IAppendable::Probe(tb)->Append(CoreUtils::Convert(String("\n")));
                    }
                }
            }
            sb.Append(tb);
        }

        mState = IMmiCodeState_COMPLETE;
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
}

void ImsPhoneMmiCode::OnQueryUncondCfTimerComplete(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;
        sb.Append(GetErrorMessage(ar));
    }
    else {
        AutoPtr<IArrayOf> infos = IArrayOf::Probe(ar->mResult);     // CallForwardInfo infos[]

        Int32 size;
        infos->GetLength(&size);
        if (size == 0) {
            // Assume the default is not active
            Logger::D(LOGTAG, "In infos.length == 0");
            cs = NULL;
            sb.Append(mContext->GetText(R::string::serviceDisabled, (ICharSequence**)&cs));
        }
        else {
            AutoPtr<ISpannableStringBuilder> tb;
            CSpannableStringBuilder::New((ISpannableStringBuilder**)&tb);

            for (Int32 serviceClassMask = 1
                    ; serviceClassMask <= ICommandsInterface::SERVICE_CLASS_MAX
                    ; serviceClassMask <<= 1) {
                for (Int32 i = 0, s = size; i < s ; i++) {
                    AutoPtr<IInterface> obj;
                    infos->Get(i, (IInterface**)&obj);
                    AutoPtr<ICallForwardInfo> info = ICallForwardInfo::Probe(obj);
                    AutoPtr<CallForwardInfo> cfi = (CallForwardInfo*)info.Get();
                    if (( serviceClassMask & cfi->mServiceClass) ==
                            ICommandsInterface::SERVICE_CLASS_VOICE &&
                            cfi->mStatus != 0) {
                        sb.Append(String("StartTime: ") + StringUtils::ToString(cfi->mStartHour)
                            + String(":") + StringUtils::ToString(cfi->mStartMinute) + String("\n"));
                        sb.Append(String("EndTime: ") + StringUtils::ToString(cfi->mEndHour)
                             + String(":") + StringUtils::ToString(cfi->mEndMinute) + String("\n"));
                        cs = NULL;
                        mContext->GetText(R::string::serviceClassVoice, (ICharSequence**)&cs);
                        sb.Append(String("Service:") + CoreUtils::Unbox(cs));
                    }
                    else if ((serviceClassMask & cfi->mServiceClass) != 0) {
                        IAppendable::Probe(tb)->Append(MakeCFTQueryResultMessage(info,
                                serviceClassMask));
                        IAppendable::Probe(tb)->Append(CoreUtils::Convert(String("\n")));
                    }
                }
            }
            sb.Append(tb);
        }
      mState = IMmiCodeState_COMPLETE;
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::MakeCFTQueryResultMessage(
    /* [in] */ ICallForwardInfo* _info,
    /* [in] */ Int32 serviceClassMask)
{
    Logger::D(LOGTAG, "makeCFTQueryResultMessage: ");
    AutoPtr<ICharSequence> cs;
    AutoPtr<ArrayOf<String> > sources = ArrayOf<String>::Alloc(3);
    (*sources)[0] = String("{0}");
    (*sources)[1] = String("{1}");
    (*sources)[2] = String("{2}");

    AutoPtr<ArrayOf<ICharSequence*> > destinations = ArrayOf<ICharSequence*>::Alloc(3);
    Boolean needTimeTemplate;

    AutoPtr<CallForwardInfo> info = (CallForwardInfo*)_info;
    if (info->mStatus == 0 && IsEmptyOrNull(CoreUtils::Convert(info->mNumber))) {
        mContext->GetText(R::string::cfTemplateNotForwarded, (ICharSequence**)&cs);
    }
    else {
        mContext->GetText(R::string::cfTemplateRegistered, (ICharSequence**)&cs);
    }
    (*destinations)[0] = ServiceClassToCFString(info->mServiceClass & serviceClassMask);
    String str;
    PhoneNumberUtils::StringFromStringAndTOA(info->mNumber, info->mToa, &str);
    (*destinations)[1] = CoreUtils::Convert(str);
    (*destinations)[2] = CoreUtils::Convert(StringUtils::ToString(info->mTimeSeconds));

    return TextUtils::Replace(cs, sources, destinations);
}

void ImsPhoneMmiCode::OnSuppSvcQueryComplete(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;

// TODO: Need ImsException
        // if (ar->mException instanceof ImsException) {
        //     ImsException error = (ImsException) ar->mException;
        //     if (error.getMessage() != NULL) {
        //         sb.Append(error.getMessage());
        //     }
        //     else {
        //         sb.Append(getErrorMessage(ar));
        //     }
        // }
        // else {
        //     sb.Append(getErrorMessage(ar));
        // }
    }
    else {
        mState = IMmiCodeState_FAILED;
        AutoPtr<CImsSsInfo> ssInfo;
        if (IBundle::Probe(ar->mResult)) {
            Logger::D(LOGTAG, "Received CLIP/COLP/COLR Response.");
            // Response for CLIP, COLP and COLR queries.
            AutoPtr<IBundle> ssInfoResp = IBundle::Probe(ar->mResult);
            AutoPtr<IParcelable> p;
            ssInfoResp->GetParcelable(UT_BUNDLE_KEY_SSINFO, (IParcelable**)&p);
            ssInfo = (CImsSsInfo*)IImsSsInfo::Probe(p);

            if (ssInfo != NULL) {
                Logger::D(LOGTAG, "ImsSsInfo mStatus = %d", ssInfo->mStatus);
                if (ssInfo->mStatus == IImsSsInfo::DISABLED) {
                    cs = NULL;
                    sb.Append((mContext->GetText(R::string::serviceDisabled, (ICharSequence**)&cs), cs));
                    mState = IMmiCodeState_COMPLETE;
                }
                else if (ssInfo->mStatus == IImsSsInfo::ENABLED) {
                    cs = NULL;
                    sb.Append((mContext->GetText(R::string::serviceEnabled, (ICharSequence**)&cs), cs));
                    mState = IMmiCodeState_COMPLETE;
                }
                else {
                    cs = NULL;
                    sb.Append((mContext->GetText(R::string::mmiError, (ICharSequence**)&cs), cs));
                }
            }
            else {
                cs = NULL;
                sb.Append((mContext->GetText(R::string::mmiError, (ICharSequence**)&cs), cs));
            }

        }
        else {
            Logger::D(LOGTAG, "Received Call Barring Response.");
            // Response for Call Barring queries.
            AutoPtr<IArrayOf> arr = IArrayOf::Probe(ar->mResult);     // ImsSsInfo[] arr
            // Check if ImsPhone has received call barring
            // enabled for service class voice.
            Int32 size;
            arr->GetLength(&size);
            for (Int32 i = 0, s = size; i < s ; i++) {
                AutoPtr<IInterface> obj;
                arr->Get(i, (IInterface**)&obj);
                AutoPtr<CImsSsInfo> info = (CImsSsInfo*)IImsSsInfo::Probe(obj);

                if(info->mStatus == 1) {
                    cs = NULL;
                    sb.Append(mContext->GetText(R::string::serviceEnabled, (ICharSequence**)&cs));
                    mState = IMmiCodeState_COMPLETE;
                }
                else {
                    cs = NULL;
                    sb.Append(mContext->GetText(R::string::serviceDisabled, (ICharSequence**)&cs));
                    mState = IMmiCodeState_COMPLETE;
                }
            }
        }
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
}

void ImsPhoneMmiCode::OnIcbQueryComplete(
    /* [in] */ AsyncResult* ar)
{
    Logger::D(LOGTAG, "OnIcbQueryComplete ");
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;
// TODO: Need ImsException
        // if (ar->mException instanceof ImsException) {
        //     ImsException error = (ImsException) ar->mException;
        //     if (error.getMessage() != NULL) {
        //         sb.Append(error.getMessage());
        //     }
        //     else {
        //         sb.Append(getErrorMessage(ar));
        //     }
        // }
        // else {
        //     sb.Append(getErrorMessage(ar));
        // }
    }
    else {
        mState = IMmiCodeState_FAILED;
        AutoPtr<IArrayOf> infos = IArrayOf::Probe(ar->mResult);     // ImsSsInfo[] infos

        Int32 size;
        infos->GetLength(&size);
        if (size == 0) {
            cs = NULL;
            sb.Append(mContext->GetText(R::string::serviceDisabled, (ICharSequence**)&cs));
            mState = IMmiCodeState_COMPLETE;
        }
        else {
            for (Int32 i = 0, s = size; i < s ; i++) {
                AutoPtr<IInterface> obj;
                infos->Get(i, (IInterface**)&obj);
                AutoPtr<CImsSsInfo> info = (CImsSsInfo*)IImsSsInfo::Probe(obj);
                sb.Append(String("Num: ") + info->mIcbNum + " status: "
                        + StringUtils::ToString(info->mStatus) + String("\n"));
            }
            mState = IMmiCodeState_COMPLETE;
        }
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
}

void ImsPhoneMmiCode::OnQueryClirComplete(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");
    mState = IMmiCodeState_FAILED;

    if (ar->mException != NULL) {
// TODO: Need ImsException
        // if (ar->mException instanceof ImsException) {
        //     ImsException error = (ImsException) ar->mException;
        //     if (error.getMessage() != NULL) {
        //         sb.Append(error.getMessage());
        //     }
        //     else {
        //         sb.Append(getErrorMessage(ar));
        //     }
        // }
    }
    else {
        AutoPtr<IBundle> ssInfo = IBundle::Probe(ar->mResult);
        AutoPtr<ArrayOf<Int32> > clirInfo;
        ssInfo->GetInt32Array(UT_BUNDLE_KEY_CLIR, (ArrayOf<Int32>**)&clirInfo);
        // clirInfo[0] = The 'n' parameter from TS 27.007 7.7
        // clirInfo[1] = The 'm' parameter from TS 27.007 7.7
        Logger::D(LOGTAG, "CLIR param n=%d m=%d", (*clirInfo)[0], (*clirInfo)[1]);

        // 'm' parameter.
        switch ((*clirInfo)[1]) {
            case CLIR_NOT_PROVISIONED:
                cs = NULL;
                sb.Append(mContext->GetText(R::string::serviceNotProvisioned, (ICharSequence**)&cs));
                mState = IMmiCodeState_COMPLETE;
                break;
            case CLIR_PROVISIONED_PERMANENT:
                cs = NULL;
                sb.Append(mContext->GetText(R::string::CLIRPermanent, (ICharSequence**)&cs));
                mState = IMmiCodeState_COMPLETE;
                break;
            case CLIR_PRESENTATION_RESTRICTED_TEMPORARY:
                // 'n' parameter.
                switch ((*clirInfo)[0]) {
                    case CLIR_DEFAULT:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::CLIRDefaultOnNextCallOn, (ICharSequence**)&cs));
                        mState = IMmiCodeState_COMPLETE;
                        break;
                    case CLIR_INVOCATION:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::CLIRDefaultOnNextCallOn, (ICharSequence**)&cs));
                        mState = IMmiCodeState_COMPLETE;
                        break;
                    case CLIR_SUPPRESSION:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::CLIRDefaultOnNextCallOff, (ICharSequence**)&cs));
                        mState = IMmiCodeState_COMPLETE;
                        break;
                    default:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::mmiError, (ICharSequence**)&cs));
                        mState = IMmiCodeState_FAILED;
                }
                break;
            case CLIR_PRESENTATION_ALLOWED_TEMPORARY:
                // 'n' parameter.
                switch ((*clirInfo)[0]) {
                    case CLIR_DEFAULT:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::CLIRDefaultOffNextCallOff, (ICharSequence**)&cs));
                        mState = IMmiCodeState_COMPLETE;
                        break;
                    case CLIR_INVOCATION:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::CLIRDefaultOffNextCallOn, (ICharSequence**)&cs));
                        mState = IMmiCodeState_COMPLETE;
                        break;
                    case CLIR_SUPPRESSION:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::CLIRDefaultOffNextCallOff, (ICharSequence**)&cs));
                        mState = IMmiCodeState_COMPLETE;
                        break;
                    default:
                        cs = NULL;
                        sb.Append(mContext->GetText(R::string::mmiError, (ICharSequence**)&cs));
                        mState = IMmiCodeState_FAILED;
                }
                break;
            default:
                cs = NULL;
                sb.Append(mContext->GetText(R::string::mmiError, (ICharSequence**)&cs));
                mState = IMmiCodeState_FAILED;
        }
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
}

void ImsPhoneMmiCode::OnQueryComplete(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;

// TODO: Need ImsException
        // if (ar->mException instanceof ImsException) {
        //     ImsException error = (ImsException) ar->mException;
        //     if (error.getMessage() != NULL) {
        //         sb.Append(error.getMessage());
        //     }
        //     else {
        //         sb.Append(getErrorMessage(ar));
        //     }
        // }
        // else {
        //     sb.Append(getErrorMessage(ar));
        // }

    }
    else {
        AutoPtr<IArrayOf> ints = IArrayOf::Probe(ar->mResult);

        Int32 size;
        ints->GetLength(&size);
        if (size != 0) {
            AutoPtr<IInterface> obj;
            ints->Get(0, (IInterface**)&obj);
            Int32 val;
            IInteger32::Probe(obj)->GetValue(&val);

            if (val == 0) {
                cs = NULL;
                sb.Append(mContext->GetText(R::string::serviceDisabled, (ICharSequence**)&cs));
            }
            else if (mSc.Equals(SC_WAIT)) {
                // Call Waiting includes additional data in the response.
                obj = NULL;
                ints->Get(1, (IInterface**)&obj);
                Int32 v;
                IInteger32::Probe(obj)->GetValue(&v);
                sb.Append(CreateQueryCallWaitingResultMessage(v));
            }
            else if (val == 1) {
                // for all other services, treat it as a boolean
                cs = NULL;
                sb.Append(mContext->GetText(R::string::serviceEnabled, (ICharSequence**)&cs));
            }
            else {
                cs = NULL;
                sb.Append(mContext->GetText(R::string::mmiError, (ICharSequence**)&cs));
            }
        }
        else {
            cs = NULL;
            sb.Append(mContext->GetText(R::string::mmiError, (ICharSequence**)&cs));
        }
        mState = IMmiCodeState_COMPLETE;
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((ImsPhone*)mPhone.Get())->OnMMIDone(IImsPhoneMmiCode::Probe(this));
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::CreateQueryCallWaitingResultMessage(
    /* [in] */ Int32 serviceClass)
{
    AutoPtr<ICharSequence> cs;
    mContext->GetText(R::string::serviceEnabledFor, (ICharSequence**)&cs);
    StringBuilder sb(CoreUtils::Unbox(cs));

    for (Int32 classMask = 1
                ; classMask <= ICommandsInterface::SERVICE_CLASS_MAX
                ; classMask <<= 1
    ) {
        if ((classMask & serviceClass) != 0) {
            sb.Append("\n");
            sb.Append(ServiceClassToCFString(classMask & serviceClass));
        }
    }

    String str;
    sb.ToString(&str);
    return CoreUtils::Convert(str);
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
