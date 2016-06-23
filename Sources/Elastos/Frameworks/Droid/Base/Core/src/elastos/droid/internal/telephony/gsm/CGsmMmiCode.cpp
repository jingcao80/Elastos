#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/internal/telephony/gsm/CGsmMmiCode.h"
#include "elastos/droid/internal/telephony/gsm/CGSMPhone.h"
#include "elastos/droid/internal/telephony/gsm/CSsData.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/text/CBidiFormatterHelper.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::CallForwardInfo;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Text::CBidiFormatterHelper;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::IBidiFormatterHelper;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::TextDirectionHeuristics;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::R;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IAppendable;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::IThrowable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

static AutoPtr<IPattern> InitPATTERN()
{
    AutoPtr<IPattern> pattern;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(
            String("((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)")
            , (IPattern**)&pattern);
    return pattern;
}

AutoPtr<IPattern> CGsmMmiCode::sPatternSuppService = InitPATTERN();
AutoPtr<ArrayOf<String> > CGsmMmiCode::sTwoDigitNumberPattern;

CAR_INTERFACE_IMPL_2(CGsmMmiCode, Handler, IGsmMmiCode, IMmiCode)

CAR_OBJECT_IMPL(CGsmMmiCode)

CGsmMmiCode::CGsmMmiCode()
    : mState(IMmiCodeState_PENDING)
    , mIsPendingUSSD(FALSE)
    , mIsUssdRequest(FALSE)
    , mIsCallFwdReg(FALSE)
    , mIsSsInfo(FALSE)
{
}

ECode CGsmMmiCode::constructor()
{
    return NOERROR;
}

ECode CGsmMmiCode::constructor(
    /* [in] */ IGSMPhone* phone,
    /* [in] */ IUiccCardApplication* app)
{
    // The telephony unit-test cases may create GsmMmiCode's
    // in secondary threads
    AutoPtr<IHandler> handler;
    IPhoneBase::Probe(phone)->GetHandler((IHandler**)&handler);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    Handler::constructor(looper);
    mPhone = phone;
    IPhone::Probe(phone)->GetContext((IContext**)&mContext);
    mUiccApplication = app;
    if (app != NULL) {
        app->GetIccRecords((IIccRecords**)&mIccRecords);
    }
    return NOERROR;
}

ECode CGsmMmiCode::NewFromDialString(
    /* [in] */ const String& dialString,
    /* [in] */ IGSMPhone* phone,
    /* [in] */ IUiccCardApplication* app,
    /* [out] */ IGsmMmiCode** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatcher> m;
    AutoPtr<CGsmMmiCode> ret;

    sPatternSuppService->Matcher(dialString, (IMatcher**)&m);

    // Is this formatted like a standard supplementary service code?
    AutoPtr<IContext> ctx;
    IPhone::Probe(phone)->GetContext((IContext**)&ctx);
    Boolean b;
    if (m->Matches(&b), b) {
        CGsmMmiCode::NewByFriend(phone, app, (CGsmMmiCode**)&ret);
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
        if(ret->mDialingNumber != NULL &&
                ret->mDialingNumber.EndWith("#") &&
                dialString.EndWith("#"))
        {
            CGsmMmiCode::New(phone, app, (IGsmMmiCode**)&ret);
            ret->mPoundString = dialString;
        }
    }
    else if (dialString.EndWith("#")) {
        // TS 22.030 sec 6.5.3.2
        // "Entry of any characters defined in the 3GPP TS 23.038 [8] Default Alphabet
        // (up to the maximum defined in 3GPP TS 24.080 [10]), followed by #SEND".

        CGsmMmiCode::New(phone, app, (IGsmMmiCode**)&ret);
        ret->mPoundString = dialString;
    }
    else if (IsTwoDigitShortCode(ctx, dialString)) {
        //Is a country-specific exception to short codes as defined in TS 22.030, 6.5.3.2
        ret = NULL;
    }
    else if (IsShortCode(dialString, phone)) {
        // this may be a short code, as defined in TS 22.030, 6.5.3.2
        CGsmMmiCode::New(phone, app, (IGsmMmiCode**)&ret);
        ret->mDialingNumber = dialString;
    }

    *result = IGsmMmiCode::Probe(ret);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmMmiCode::NewNetworkInitiatedUssd(
    /* [in] */ const String& _ussdMessage,
    /* [in] */ Boolean isUssdRequest,
    /* [in] */ IGSMPhone* phone,
    /* [in] */ IUiccCardApplication* app,
    /* [out] */ IGsmMmiCode** result)
{
    VALIDATE_NOT_NULL(result)
    String ussdMessage = _ussdMessage;
    AutoPtr<CGsmMmiCode> ret;

    CGsmMmiCode::NewByFriend(phone, app, (CGsmMmiCode**)&ret);

    ret->mMessage->ToString(&ussdMessage);
    ret->mIsUssdRequest = isUssdRequest;

    // If it's a request, set to PENDING so that it's cancelable.
    if (isUssdRequest) {
        ret->mIsPendingUSSD = TRUE;
        ret->mState = IMmiCodeState_PENDING;
    }
    else {
        ret->mState = IMmiCodeState_COMPLETE;
    }

    *result = IGsmMmiCode::Probe(ret);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmMmiCode::NewFromUssdUserInput(
    /* [in] */ const String& _ussdMessage,
    /* [in] */ IGSMPhone* phone,
    /* [in] */ IUiccCardApplication* app,
    /* [out] */ IGsmMmiCode** result)
{
    VALIDATE_NOT_NULL(result)
    String ussdMessage = _ussdMessage;
    AutoPtr<CGsmMmiCode> ret;
    CGsmMmiCode::NewByFriend(phone, app, (CGsmMmiCode**)&ret);

    ret->mMessage->ToString(&ussdMessage);
    ret->mState = IMmiCodeState_PENDING;
    ret->mIsPendingUSSD = TRUE;

    *result = IGsmMmiCode::Probe(ret);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmMmiCode::ProcessSsData(
    /* [in] */ AsyncResult* data)
{
    Logger::D(LOG_TAG, "In processSsData");

    mIsSsInfo = TRUE;
    // try {
    AutoPtr<ISsData> ssData = ISsData::Probe(data->mResult);
    ParseSsData(ssData);
    // } catch (ClassCastException ex) {
    //     Logger::E(LOG_TAG, "Class Cast Exception in parsing SS Data : " + ex);
    // } catch (NullPointerException ex) {
    //     Logger::E(LOG_TAG, "Null Pointer Exception in parsing SS Data : " + ex);
    // }
    return NOERROR;
}

ECode CGsmMmiCode::ParseSsData(
    /* [in] */ ISsData* _ssData)
{
    AutoPtr<CSsData> ssData = (CSsData*)_ssData;
    AutoPtr<ICommandException> ex;

// TODO: Need CommandException
    // CommandException::FromRilErrno(ssData->mResult, (ICommandException**)&ex);
    mSc = GetScStringFromScType(ssData->mServiceType);
    mAction = GetActionStringFromReqType(ssData->mRequestType);
    // Logger::D(LOG_TAG, "parseSsData msc = " + mSc + ", action = " + mAction + ", ex = " + ex);

    Boolean b1, b2, b3;
// TODO: Need ServiceType::isTypeUnConditional
    // ssData->mServiceType->IsTypeUnConditional(&b1);
    // ssData->mServiceType->IsTypeClir(&b2);
    // ssData->mServiceType->IsTypeCF(&b3);
    switch (ssData->mRequestType) {
        case SS_ACTIVATION:
        case SS_DEACTIVATION:
        case SS_REGISTRATION:
        case SS_ERASURE: {
            if ((ssData->mResult == IRILConstants::SUCCESS) && b1) {
                /*
                 * When ServiceType is SS_CFU/SS_CF_ALL and RequestType is activate/register
                 * and ServiceClass is Voice/None, set IccRecords.setVoiceCallForwardingFlag.
                 * Only CF status can be set here since number is not available.
                 */
                Boolean cffEnabled = ((ssData->mRequestType == SS_ACTIVATION ||
                        ssData->mRequestType == SS_REGISTRATION) &&
                        IsServiceClassVoiceorNone(ssData->mServiceClass));

                Logger::D(LOG_TAG, "setVoiceCallForwardingFlag cffEnabled: %d", cffEnabled);
                if (((CGSMPhone*)mPhone.Get())->mIccRecords != NULL) {
                    mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, String(NULL));
                    Logger::D(LOG_TAG, "setVoiceCallForwardingFlag done from SS Info.");
                }
                else {
                    Logger::E(LOG_TAG, "setVoiceCallForwardingFlag aborted. sim records is null.");
                }
            }
            AutoPtr<IArrayOf> iArray;
            CArrayOf::New(EIID_ICallForwardInfo, ssData->mCfInfo->GetLength(), (IArrayOf**)&iArray);
            for (Int32 i = 0; i < ssData->mCfInfo->GetLength(); i++) {
                iArray->Set(i, (*ssData->mCfInfo)[i]);
            }
            OnSetComplete(NULL, new AsyncResult(NULL, iArray, IThrowable::Probe(ex)));
            break;
        }
        case SS_INTERROGATION: {
            if (b2) {
                Logger::D(LOG_TAG, "CLIR INTERROGATION");
                AutoPtr<IArrayOf> iArray;
                CArrayOf::New(EIID_IInteger32, ssData->mSsInfo->GetLength(), (IArrayOf**)&iArray);
                for (Int32 i = 0; i < ssData->mSsInfo->GetLength(); i++) {
                    iArray->Set(i, CoreUtils::Convert((*ssData->mSsInfo)[i]));
                }
                OnGetClirComplete(new AsyncResult(NULL, iArray, IThrowable::Probe(ex)));
            }
            else if (b3) {
                Logger::D(LOG_TAG, "CALL FORWARD INTERROGATION");
                AutoPtr<IArrayOf> iArray;
                CArrayOf::New(EIID_ICallForwardInfo, ssData->mCfInfo->GetLength(), (IArrayOf**)&iArray);
                for (Int32 i = 0; i < ssData->mCfInfo->GetLength(); i++) {
                    iArray->Set(i, (*ssData->mCfInfo)[i]);
                }
                OnQueryCfComplete(new AsyncResult(NULL, iArray, IThrowable::Probe(ex)));
            }
            else {
                AutoPtr<IArrayOf> iArray;
                CArrayOf::New(EIID_IInteger32, ssData->mSsInfo->GetLength(), (IArrayOf**)&iArray);
                for (Int32 i = 0; i < ssData->mSsInfo->GetLength(); i++) {
                    iArray->Set(i, CoreUtils::Convert((*ssData->mSsInfo)[i]));
                }
                OnQueryComplete(new AsyncResult(NULL, iArray, IThrowable::Probe(ex)));
            }
            break;
        }
        default:
            Logger::E(LOG_TAG, "Invaid requestType in SSData : " + ssData->mRequestType);
            break;
    }
    return NOERROR;
}

ECode CGsmMmiCode::IsServiceCodeCallForwarding(
    /* [in] */ const String& sc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = sc != NULL &&
            (sc.Equals(IGsmMmiCode::SC_CFU)
            || sc.Equals(IGsmMmiCode::SC_CFB) || sc.Equals(IGsmMmiCode::SC_CFNRy)
            || sc.Equals(IGsmMmiCode::SC_CFNR) || sc.Equals(IGsmMmiCode::SC_CF_All)
            || sc.Equals(IGsmMmiCode::SC_CF_All_Conditional));
    return NOERROR;
}

ECode CGsmMmiCode::IsServiceCodeCallBarring(
    /* [in] */ const String& sc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IResources> resource = CResources::GetSystem();
    if (sc != NULL) {
        AutoPtr<ArrayOf<String> > barringMMI;
        resource->GetStringArray(
            Elastos::Droid::R::array::config_callBarringMMI, (ArrayOf<String>**)&barringMMI);
        if (barringMMI != NULL) {
            for (Int32 i = 0; i < barringMMI->GetLength(); i++) {
                String match = (*barringMMI)[i];
                if (sc.Equals(match)) *result = TRUE;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CGsmMmiCode::ScToBarringFacility(
    /* [in] */ const String& sc,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);
    if (sc == NULL) {
        // throw new RuntimeException ("invalid call barring sc");
        return E_RUNTIME_EXCEPTION;
    }

    if (sc.Equals(IGsmMmiCode::SC_BAOC)) {
        *result = ICommandsInterface::CB_FACILITY_BAOC;
    }
    else if (sc.Equals(IGsmMmiCode::SC_BAOIC)) {
        *result = ICommandsInterface::CB_FACILITY_BAOIC;
    }
    else if (sc.Equals(IGsmMmiCode::SC_BAOICxH)) {
        *result = ICommandsInterface::CB_FACILITY_BAOICxH;
    }
    else if (sc.Equals(IGsmMmiCode::SC_BAIC)) {
        *result = ICommandsInterface::CB_FACILITY_BAIC;
    }
    else if (sc.Equals(IGsmMmiCode::SC_BAICr)) {
        *result = ICommandsInterface::CB_FACILITY_BAICr;
    }
    else if (sc.Equals(IGsmMmiCode::SC_BA_ALL)) {
        *result = ICommandsInterface::CB_FACILITY_BA_ALL;
    }
    else if (sc.Equals(IGsmMmiCode::SC_BA_MO)) {
        *result = ICommandsInterface::CB_FACILITY_BA_MO;
    }
    else if (sc.Equals(IGsmMmiCode::SC_BA_MT)) {
        *result = ICommandsInterface::CB_FACILITY_BA_MT;
    }
    else {
        // throw new RuntimeException ("invalid call barring sc");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CGsmMmiCode::GetState(
    /* [out] */ IMmiCodeState* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode CGsmMmiCode::GetMessage(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMessage;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmMmiCode::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IPhone::Probe(mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmMmiCode::Cancel()
{
    // Complete or failed cannot be cancelled
    if (mState == IMmiCodeState_COMPLETE || mState == IMmiCodeState_FAILED) {
        return NOERROR;
    }

    mState = IMmiCodeState_CANCELLED;

    if (mIsPendingUSSD) {
        /*
         * There can only be one pending USSD session, so tell the radio to
         * cancel it.
         */
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_USSD_CANCEL_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
        ((CGSMPhone*)mPhone.Get())->mCi->CancelPendingUssd(msg);

        /*
         * Don't call phone.onMMIDone here; wait for CANCEL_COMPLETE notice
         * from RIL.
         */
    }
    else {
        // TODO in cases other than USSD, it would be nice to cancel
        // the pending radio operation. This requires RIL cancellation
        // support, which does not presently exist.

        ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
    }
    return NOERROR;
}

ECode CGsmMmiCode::IsCancelable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    /* Can only cancel pending USSD sessions. */
    *result = mIsPendingUSSD;
    return NOERROR;
}

ECode CGsmMmiCode::IsMMI(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !mPoundString.IsNull();
    return NOERROR;
}

ECode CGsmMmiCode::IsShortCode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPoundString.IsNull()
                && !mDialingNumber.IsNull() && mDialingNumber.GetLength() <= 2;
    return NOERROR;
}

ECode CGsmMmiCode::IsPinPukCommand(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !mSc.IsNull() && (mSc.Equals(SC_PIN) || mSc.Equals(SC_PIN2)
                          || mSc.Equals(SC_PUK) || mSc.Equals(SC_PUK2));
    return NOERROR;
}

ECode CGsmMmiCode::IsTemporaryModeCLIR(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b1, b2;
    *result = !mSc.IsNull() && mSc.Equals(SC_CLIR) && !mDialingNumber.IsNull()
            && ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2));
    return NOERROR;
}

ECode CGsmMmiCode::GetCLIRMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSc != NULL && mSc.Equals(SC_CLIR)) {
        Boolean b;
        if (IsActivate(&b), b) {
            *result = ICommandsInterface::CLIR_SUPPRESSION;
        }
        else if (IsDeactivate(&b), b) {
            *result = ICommandsInterface::CLIR_INVOCATION;
        }
    }

    *result = ICommandsInterface::CLIR_DEFAULT;
    return NOERROR;
}

ECode CGsmMmiCode::IsActivate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !mAction.IsNull() && mAction.Equals(ACTION_ACTIVATE);
    return NOERROR;
}

ECode CGsmMmiCode::IsDeactivate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !mAction.IsNull() && mAction.Equals(ACTION_DEACTIVATE);
    return NOERROR;
}

ECode CGsmMmiCode::IsInterrogate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !mAction.IsNull() && mAction.Equals(ACTION_INTERROGATE);
    return NOERROR;
}

ECode CGsmMmiCode::IsRegister(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !mAction.IsNull() && mAction.Equals(ACTION_REGISTER);
    return NOERROR;
}

ECode CGsmMmiCode::IsErasure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !mAction.IsNull() && mAction.Equals(ACTION_ERASURE);
    return NOERROR;
}

ECode CGsmMmiCode::IsPendingUSSD(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsPendingUSSD;
    return NOERROR;
}

ECode CGsmMmiCode::IsUssdRequest(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsUssdRequest;
    return NOERROR;
}

ECode CGsmMmiCode::IsSsInfo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsSsInfo;
    return NOERROR;
}

ECode CGsmMmiCode::ProcessCode()
{
    // try {
    Boolean b;
    if (IsShortCode(&b), b) {
        Logger::D(LOG_TAG, "IsShortCode");
        // These just get treated as USSD.
        SendUssd(mDialingNumber);
    }
    else if (mDialingNumber != NULL) {
        // We should have no dialing numbers here
        // throw new RuntimeException ("Invalid or Unsupported MMI Code");
        return E_RUNTIME_EXCEPTION;
    }
    else if (mSc != NULL && mSc.Equals(SC_CLIP)) {
        Logger::D(LOG_TAG, "is CLIP");
        if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->QueryCLIP(msg);
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mSc != NULL && mSc.Equals(SC_CLIR)) {
        Logger::D(LOG_TAG, "is CLIR");
        if (IsActivate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->SetCLIR(ICommandsInterface::CLIR_INVOCATION, msg);
        }
        else if (IsDeactivate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->SetCLIR(ICommandsInterface::CLIR_SUPPRESSION, msg);
        }
        else if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_CLIR_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->GetCLIR(msg);
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (IsServiceCodeCallForwarding(mSc, &b), b) {
        Logger::D(LOG_TAG, "is CF");

        String dialingNumber = mSia;
        Int32 serviceClass;
        SiToServiceClass(mSib, &serviceClass);
        Int32 reason;
        ScToCallForwardReason(mSc, &reason);
        Int32 time = SiToTime(mSic);

        if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_CF_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->QueryCallForwardStatus(
                    reason, serviceClass,  dialingNumber, msg);
        }
        else {
            Int32 cfAction;

            if (IsActivate(&b), b) {
                // 3GPP TS 22.030 6.5.2
                // a call forwarding request with a single * would be
                // interpreted as registration if containing a forwarded-to
                // number, or an activation if not
                if (IsEmptyOrNull(CoreUtils::Convert(dialingNumber))) {
                    cfAction = ICommandsInterface::CF_ACTION_ENABLE;
                    mIsCallFwdReg = FALSE;
                }
                else {
                    cfAction = ICommandsInterface::CF_ACTION_REGISTRATION;
                    mIsCallFwdReg = TRUE;
                }
            }
            else if (IsDeactivate(&b), b) {
                cfAction = ICommandsInterface::CF_ACTION_DISABLE;
            }
            else if (IsRegister(&b), b) {
                cfAction = ICommandsInterface::CF_ACTION_REGISTRATION;
            }
            else if (IsErasure(&b), b) {
                cfAction = ICommandsInterface::CF_ACTION_ERASURE;
            }
            else {
                // throw new RuntimeException ("invalid action");
                return E_RUNTIME_EXCEPTION;
            }

            Int32 isSettingUnconditionalVoice =
                (((reason == ICommandsInterface::CF_REASON_UNCONDITIONAL) ||
                        (reason == ICommandsInterface::CF_REASON_ALL)) &&
                        (((serviceClass & ICommandsInterface::SERVICE_CLASS_VOICE) != 0) ||
                         (serviceClass == ICommandsInterface::SERVICE_CLASS_NONE))) ? 1 : 0;

            Int32 isEnableDesired =
                ((cfAction == ICommandsInterface::CF_ACTION_ENABLE) ||
                        (cfAction == ICommandsInterface::CF_ACTION_REGISTRATION)) ? 1 : 0;

            Logger::D(LOG_TAG, "is CF setCallForward");
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_CFF_COMPLETE, isSettingUnconditionalVoice,
                isEnableDesired, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->SetCallForward(cfAction, reason, serviceClass,
                    dialingNumber, time, msg);
        }
    }
    else if (IsServiceCodeCallBarring(mSc, &b), b) {
        // sia = password
        // sib = basic service group

        String password = mSia;
        Int32 serviceClass;
        SiToServiceClass(mSib, &serviceClass);
        String facility;
        ScToBarringFacility(mSc, &facility);

        Boolean b1, b2;
        if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->QueryFacilityLock(facility, password,
                    serviceClass, msg);
        }
        else if ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2)) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->SetFacilityLock(facility, b1, password,
                    serviceClass, msg);
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }

    }
    else if (mSc != NULL && mSc.Equals(SC_PWD)) {
        // sia = fac
        // sib = old pwd
        // sic = new pwd
        // pwd = new pwd
        String facility;
        String oldPwd = mSib;
        String newPwd = mSic;
        Boolean b1 ,b2;
        if ((IsActivate(&b1), b1) || (IsRegister(&b2), b2)) {
            // Even though ACTIVATE is acceptable, this is really termed a REGISTER
            mAction = ACTION_REGISTER;

            if (mSia == NULL) {
                // If sc was not specified, treat it as BA_ALL.
                facility = ICommandsInterface::CB_FACILITY_BA_ALL;
            }
            else {
                ScToBarringFacility(mSia, &facility);
            }
            if (newPwd.Equals(mPwd)) {
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
                ((CGSMPhone*)mPhone.Get())->mCi->ChangeBarringPassword(facility, oldPwd,
                        newPwd, msg);
            }
            else {
                // password mismatch; return error
                HandlePasswordError(Elastos::Droid::R::string::passwordIncorrect);
            }
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mSc != NULL && mSc.Equals(SC_WAIT)) {
        // sia = basic service group
        Int32 serviceClass;
        SiToServiceClass(mSia, &serviceClass);

        Boolean b1 ,b2;
        if ((IsActivate(&b1), b1) || (IsDeactivate(&b2), b2)) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->SetCallWaiting((IsActivate(&b), b), serviceClass, msg);
        }
        else if (IsInterrogate(&b), b) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_QUERY_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
            ((CGSMPhone*)mPhone.Get())->mCi->QueryCallWaiting(serviceClass, msg);
        }
        else {
            // throw new RuntimeException ("Invalid or Unsupported MMI Code");
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (IsPinPukCommand(&b), b) {
        // TODO: This is the same as the code in CmdaMmiCode.java,
        // MmiCode should be an abstract or base class and this and
        // other common variables and code should be promoted.

        // sia = old PIN or PUK
        // sib = new PIN
        // sic = new PIN
        String oldPinOrPuk = mSia;
        String newPinOrPuk = mSib;
        Int32 pinLen = newPinOrPuk.GetLength();
        AppState state;
        if (IsRegister(&b), b) {
            if (!newPinOrPuk.Equals(mSic)) {
                // password mismatch; return error
                HandlePasswordError(Elastos::Droid::R::string::mismatchPin);
            }
            else if (pinLen < 4 || pinLen > 8 ) {
                // invalid length
                HandlePasswordError(Elastos::Droid::R::string::invalidPin);
            }
            else if (mSc.Equals(SC_PIN)
                    && mUiccApplication != NULL
                    && (mUiccApplication->GetState(&state), state)
                    == Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_PUK) {
                // Sim is puk-locked
                HandlePasswordError(Elastos::Droid::R::string::needPuk);
            }
            else if (mUiccApplication != NULL) {
                Logger::D(LOG_TAG, "process mmi service code using UiccApp sc=%s", mSc.string());

                // We have an app and the pre-checks are OK
                if (mSc.Equals(SC_PIN)) {
                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
                    mUiccApplication->ChangeIccLockPassword(oldPinOrPuk, newPinOrPuk, msg);
                }
                else if (mSc.Equals(SC_PIN2)) {
                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
                    mUiccApplication->ChangeIccFdnPassword(oldPinOrPuk, newPinOrPuk, msg);
                }
                else if (mSc.Equals(SC_PUK)) {
                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
                    mUiccApplication->SupplyPuk(oldPinOrPuk, newPinOrPuk, msg);
                }
                else if (mSc.Equals(SC_PUK2)) {
                    AutoPtr<IMessage> msg;
                    ObtainMessage(EVENT_SET_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
                    mUiccApplication->SupplyPuk2(oldPinOrPuk, newPinOrPuk, msg);
                }
                else {
                    // throw new RuntimeException("uicc unsupported service code=" + mSc);
                    return E_RUNTIME_EXCEPTION;
                }
            }
            else {
                // throw new RuntimeException("No application mUiccApplicaiton is NULL");
                return E_RUNTIME_EXCEPTION;
            }
        }
        else {
            // throw new RuntimeException ("Ivalid register/action=" + mAction);
            return E_RUNTIME_EXCEPTION;
        }
    }
    else if (mPoundString != NULL) {
        SendUssd(mPoundString);
    }
    else {
        // throw new RuntimeException ("Invalid or Unsupported MMI Code");
        return E_RUNTIME_EXCEPTION;
    }
    // } catch (RuntimeException exc) {
    //     mState = IMmiCodeState_FAILED;
    //     mMessage = mContext.getText(Elastos::Droid::R::string::mmiError);
    //     mPhone.onMMIDone(this);
    // }
    return NOERROR;
}

ECode CGsmMmiCode::OnUssdFinished(
    /* [in] */ const String& ussdMessage,
    /* [in] */ Boolean isUssdRequest)
{
    if (mState == IMmiCodeState_PENDING) {
        if (ussdMessage == NULL) {
            mContext->GetText(Elastos::Droid::R::string::mmiComplete, (ICharSequence**)&mMessage);
        }
        else {
            mMessage = CoreUtils::Convert(ussdMessage);
        }
        mIsUssdRequest = isUssdRequest;
        // If it's a request, leave it PENDING so that it's cancelable.
        if (!isUssdRequest) {
            mState = IMmiCodeState_COMPLETE;
        }

        ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
    }
    return NOERROR;
}

ECode CGsmMmiCode::OnUssdFinishedError()
{
    if (mState == IMmiCodeState_PENDING) {
        mState = IMmiCodeState_FAILED;
        mContext->GetText(Elastos::Droid::R::string::mmiError, (ICharSequence**)&mMessage);

        ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
    }
    return NOERROR;
}

ECode CGsmMmiCode::SendUssd(
    /* [in] */ const String& ussdMessage)
{
    // Treat this as a USSD string
    mIsPendingUSSD = TRUE;

    // Note that unlike most everything else, the USSD complete
    // response does not complete this MMI code...we wait for
    // an unsolicited USSD "Notify" or "Request".
    // The matching up of this is done in GSMPhone.

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_USSD_COMPLETE, TO_IINTERFACE(this), (IMessage**)&msg);
    ((CGSMPhone*)mPhone.Get())->mCi->SendUSSD(ussdMessage, msg);
    return NOERROR;
}

ECode CGsmMmiCode::HandleMessage (
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_SET_COMPLETE:
            ar = (AsyncResult*)(IObject*)obj.Get();

            OnSetComplete(msg, ar);
            break;

        case EVENT_SET_CFF_COMPLETE: {
            ar = (AsyncResult*)(IObject*)obj.Get();

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
                    ((CGSMPhone*)mPhone.Get())->SetCallForwardingPreference(cffEnabled);
                }
            }

            OnSetComplete(msg, ar);
            break;
        }
        case EVENT_GET_CLIR_COMPLETE:
            ar = (AsyncResult*)(IObject*)obj.Get();
            OnGetClirComplete(ar);
        break;

        case EVENT_QUERY_CF_COMPLETE:
            ar = (AsyncResult*)(IObject*)obj.Get();
            OnQueryCfComplete(ar);
        break;

        case EVENT_QUERY_COMPLETE:
            ar = (AsyncResult*)(IObject*)obj.Get();
            OnQueryComplete(ar);
        break;

        case EVENT_USSD_COMPLETE:
            ar = (AsyncResult*)(IObject*)obj.Get();

            if (ar->mException != NULL) {
                mState = IMmiCodeState_FAILED;
                mMessage = GetErrorMessage(ar);

                ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
            }

            // Note that unlike most everything else, the USSD complete
            // response does not complete this MMI code...we wait for
            // an unsolicited USSD "Notify" or "Request".
            // The matching up of this is done in GSMPhone.

        break;

        case EVENT_USSD_CANCEL_COMPLETE:
            ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
        break;
    }
    return NOERROR;
}

ECode CGsmMmiCode::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("GsmMmiCode {");

    IMmiCodeState state;
    GetState(&state);
    sb.Append(String("State=") + state);
    if (!mAction.IsNull()) sb.Append(String(" action=") + mAction);
    if (!mSc.IsNull()) sb.Append(String(" sc=") + mSc);
    if (!mSia.IsNull()) sb.Append(String(" sia=") + mSia);
    if (!mSib.IsNull()) sb.Append(String(" sib=") + mSib);
    if (!mSic.IsNull()) sb.Append(String(" sic=") + mSic);
    if (!mPoundString.IsNull()) sb.Append(String(" poundString=") + mPoundString);
    if (!mDialingNumber.IsNull()) sb.Append(String(" dialingNumber=") + mDialingNumber);
    if (!mPwd.IsNull()) sb.Append(String(" pwd=") + mPwd);
    sb.Append("}");
    sb.ToString(result);
    return NOERROR;
}

String CGsmMmiCode::GetScStringFromScType(
    /* [in] */ ServiceType sType)
{
    switch (sType) {
        case SS_CFU:
            return SC_CFU;
        case SS_CF_BUSY:
            return SC_CFB;
        case SS_CF_NO_REPLY:
            return SC_CFNRy;
        case SS_CF_NOT_REACHABLE:
            return SC_CFNR;
        case SS_CF_ALL:
            return SC_CF_All;
        case SS_CF_ALL_CONDITIONAL:
            return SC_CF_All_Conditional;
        case SS_CLIP:
            return SC_CLIP;
        case SS_CLIR:
            return SC_CLIR;
        case SS_WAIT:
            return SC_WAIT;
        case SS_BAOC:
            return SC_BAOC;
        case SS_BAOIC:
            return SC_BAOIC;
        case SS_BAOIC_EXC_HOME:
            return SC_BAOICxH;
        case SS_BAIC:
            return SC_BAIC;
        case SS_BAIC_ROAMING:
            return SC_BAICr;
        case SS_ALL_BARRING:
            return SC_BA_ALL;
        case SS_OUTGOING_BARRING:
            return SC_BA_MO;
        case SS_INCOMING_BARRING:
            return SC_BA_MT;
    }

    return String("");
}

String CGsmMmiCode::GetActionStringFromReqType(
    /* [in] */ RequestType rType)
{
    switch (rType) {
        case SS_ACTIVATION:
            return ACTION_ACTIVATE;
        case SS_DEACTIVATION:
            return ACTION_DEACTIVATE;
        case SS_INTERROGATION:
            return ACTION_INTERROGATE;
        case SS_REGISTRATION:
            return ACTION_REGISTER;
        case SS_ERASURE:
            return ACTION_ERASURE;
    }

    return String("");
}

Boolean CGsmMmiCode::IsServiceClassVoiceorNone(
    /* [in] */ Int32 serviceClass)
{
    return (((serviceClass & ICommandsInterface::SERVICE_CLASS_VOICE) != 0) ||
            (serviceClass == ICommandsInterface::SERVICE_CLASS_NONE));
}

String CGsmMmiCode::MakeEmptyNull(
    /* [in] */ const String& s)
{
    if (!s.IsNull() && s.GetLength() == 0) return String(NULL);

    return s;
}

Boolean CGsmMmiCode::IsEmptyOrNull(
    /* [in] */ ICharSequence* s)
{
    String str;
    s->ToString(&str);
    return str.IsNull() || (str.GetLength() == 0);
}

ECode CGsmMmiCode::ScToCallForwardReason(
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
    }
    else if (sc.Equals(SC_CFU)) {
        *result = ICommandsInterface::CF_REASON_UNCONDITIONAL;
    }
    else if (sc.Equals(SC_CFB)) {
        *result = ICommandsInterface::CF_REASON_BUSY;
    }
    else if (sc.Equals(SC_CFNR)) {
        *result = ICommandsInterface::CF_REASON_NOT_REACHABLE;
    }
    else if (sc.Equals(SC_CFNRy)) {
        *result = ICommandsInterface::CF_REASON_NO_REPLY;
    }
    else if (sc.Equals(SC_CF_All_Conditional)) {
       *result = ICommandsInterface::CF_REASON_ALL_CONDITIONAL;
    }
    else {
        // throw new RuntimeException ("invalid call forward sc");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CGsmMmiCode::SiToServiceClass(
    /* [in] */ const String& si,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (si.IsNull() || si.GetLength() == 0) {
            *result = ICommandsInterface::SERVICE_CLASS_NONE;
    }
    else {
        // NumberFormatException should cause MMI fail
        Int32 serviceCode = StringUtils::ParseInt32(si, 10);

        switch (serviceCode) {
            case 10:
                *result = ICommandsInterface::SERVICE_CLASS_SMS +
                        ICommandsInterface::SERVICE_CLASS_FAX  +
                        ICommandsInterface::SERVICE_CLASS_VOICE;
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
/*
Note for code 20:
 From TS 22.030 Annex C:
            "All GPRS bearer services" are not included in "All tele and bearer services"
                and "All bearer services"."
....so SERVICE_CLASS_DATA, which (according to 27.007) includes GPRS
*/
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

Int32 CGsmMmiCode::SiToTime(
    /* [in] */ const String& si)
{
    if (si.IsNull() || si.GetLength() == 0) {
        return 0;
    }
    else {
        // NumberFormatException should cause MMI fail
        return StringUtils::ParseInt32(si, 10);
    }
}

Boolean CGsmMmiCode::IsTwoDigitShortCode(
    /* [in] */ IContext* context,
    /* [in] */ const String& dialString)
{
    Logger::D(LOG_TAG, "isTwoDigitShortCode");

    if (dialString.IsNull() || dialString.GetLength() > 2) return FALSE;

    if (sTwoDigitNumberPattern == NULL) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetStringArray(Elastos::Droid::R::array::config_twoDigitNumberPattern,
                (ArrayOf<String>**)&sTwoDigitNumberPattern);
    }

    for (Int32 i = 0; i < sTwoDigitNumberPattern->GetLength(); i++) {
        String dialnumber = (*sTwoDigitNumberPattern)[i];
        Logger::D(LOG_TAG, "Two Digit Number Pattern %s", dialnumber.string());
        if (dialString.Equals(dialnumber)) {
            Logger::D(LOG_TAG, "Two Digit Number Pattern -TRUE");
            return TRUE;
        }
    }
    Logger::D(LOG_TAG, "Two Digit Number Pattern -FALSE");
    return FALSE;
}

Boolean CGsmMmiCode::IsShortCode(
    /* [in] */ const String& dialString,
    /* [in] */ IGSMPhone* phone)
{
    // Refer to TS 22.030 Figure 3.5.3.2:
    if (dialString == NULL) {
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
}

Boolean CGsmMmiCode::IsShortCodeUSSD(
    /* [in] */ const String& dialString,
    /* [in] */ IGSMPhone* phone)
{
    if (!dialString.IsNull() && dialString.GetLength() <= MAX_LENGTH_SHORT_CODE) {
        Boolean b;
        if (((CGSMPhone*)phone)->IsInCall(&b), b) {
            return TRUE;
        }

        if (dialString.GetLength() != MAX_LENGTH_SHORT_CODE ||
                dialString.GetChar(0) != '1') {
            return TRUE;
        }
    }
    return FALSE;
}

void CGsmMmiCode::HandlePasswordError(
    /* [in] */ Int32 res)
{
    mState = IMmiCodeState_FAILED;
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");
    cs = NULL;
    mContext->GetText(res, (ICharSequence**)&cs);
    sb.Append(cs);
    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
}

AutoPtr<ICharSequence> CGsmMmiCode::GetErrorMessage(
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICommandException> ce = ICommandException::Probe(ar->mException);
    AutoPtr<ICharSequence> cs;

    if (ce != NULL) {
// TODO: Need CommandException::Error
        // CommandExceptionError err;
        // ce->GetCommandError(&err);

        // if (err == CommandExceptionError_FDN_CHECK_FAILURE) {
        //     Logger::I(LOG_TAG, "FDN_CHECK_FAILURE");
        //     mContext->GetText(Elastos::Droid::R::string::mmiFdnError, (ICharSequence**)&cs);
        //     return cs;
        // }
        // else if (err == CommandExceptionError_USSD_MODIFIED_TO_DIAL) {
        //     Logger::I(LOG_TAG, "USSD_MODIFIED_TO_DIAL");
        //     mContext->GetText(Elastos::Droid::R::string::stk_cc_ussd_to_dial, (ICharSequence**)&cs);
        //     return cs;
        // }
        // else if (err == CommandExceptionError_USSD_MODIFIED_TO_SS) {
        //     Logger::I(LOG_TAG, "USSD_MODIFIED_TO_SS");
        //     mContext->GetText(Elastos::Droid::R::string::stk_cc_ussd_to_ss, (ICharSequence**)&cs);
        //     return cs;
        // }
        // else if (err == CommandExceptionError_USSD_MODIFIED_TO_USSD) {
        //     Logger::I(LOG_TAG, "USSD_MODIFIED_TO_USSD");
        //     mContext->GetText(Elastos::Droid::R::string::stk_cc_ussd_to_ussd, (ICharSequence**)&cs);
        //     return cs;
        // }
        // else if (err == CommandExceptionError_SS_MODIFIED_TO_DIAL) {
        //     Logger::I(LOG_TAG, "SS_MODIFIED_TO_DIAL");
        //     mContext->GetText(Elastos::Droid::R::string::stk_cc_ss_to_dial, (ICharSequence**)&cs);
        //     return cs;
        // }
        // else if (err == CommandExceptionError_SS_MODIFIED_TO_USSD) {
        //     Logger::I(LOG_TAG, "SS_MODIFIED_TO_USSD");
        //     mContext->GetText(Elastos::Droid::R::string::stk_cc_ss_to_ussd, (ICharSequence**)&cs);
        //     return cs;
        // }
        // else if (err == CommandExceptionError_SS_MODIFIED_TO_SS) {
        //     Logger::I(LOG_TAG, "SS_MODIFIED_TO_SS");
        //     mContext->GetText(Elastos::Droid::R::string::stk_cc_ss_to_ss, (ICharSequence**)&cs);
        //     return cs;
        // }
    }

    mContext->GetText(Elastos::Droid::R::string::mmiError, (ICharSequence**)&cs);
    return cs;
}

AutoPtr<ICharSequence> CGsmMmiCode::GetScString()
{
    AutoPtr<ICharSequence> cs;
    if (mSc != NULL) {
        Boolean b;
        if (IsServiceCodeCallBarring(mSc, &b), b) {
            mContext->GetText(Elastos::Droid::R::string::BaMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (IsServiceCodeCallForwarding(mSc, &b), b) {
            mContext->GetText(Elastos::Droid::R::string::CfMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_CLIP)) {
            mContext->GetText(Elastos::Droid::R::string::ClipMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_CLIR)) {
            mContext->GetText(Elastos::Droid::R::string::ClirMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_PWD)) {
            mContext->GetText(Elastos::Droid::R::string::PwdMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (mSc.Equals(SC_WAIT)) {
            mContext->GetText(Elastos::Droid::R::string::CwMmi, (ICharSequence**)&cs);
            return cs;
        }
        else if (IsPinPukCommand(&b), b) {
            mContext->GetText(Elastos::Droid::R::string::PinMmi, (ICharSequence**)&cs);
            return cs;
        }
    }

    return CoreUtils::Convert(String(""));
}

void CGsmMmiCode::OnSetComplete(
    /* [in] */ IMessage* msg,
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ICharSequence> cs = GetScString();
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    cs = NULL;
    Boolean b;
    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;
        AutoPtr<ICommandException> ce = ICommandException::Probe(ar->mException);
        if (ce != NULL) {
// TODO: Need CommandException::Error
            // CommandExceptionError err;
            // ce->GetCommandError(&err);
            // if (err == CommandExceptionError_PASSWORD_INCORRECT) {
            //     if (IsPinPukCommand(&b), b) {
            //         // look specifically for the PUK commands and adjust
            //         // the message accordingly.
            //         if (mSc.Equals(SC_PUK) || mSc.Equals(SC_PUK2)) {
            //             mContext->GetText(
            //                     Elastos::Droid::R::string::badPuk, (ICharSequence**)&cs);
            //             sb.Append(cs);
            //         }
            //         else {
            //             mContext->GetText(
            //                     Elastos::Droid::R::string::badPin, (ICharSequence**)&cs);
            //             sb.Append(cs);
            //         }
            //         // Get the No. of retries remaining to unlock PUK/PUK2
            //         Int32 attemptsRemaining;
            //         msg->GetArg1(&attemptsRemaining);
            //         if (attemptsRemaining <= 0) {
            //             Logger::D(LOG_TAG, "onSetComplete: PUK locked, cancel as lock screen will handle this");
            //             mState = IMmiCodeState_CANCELLED;
            //         }
            //         else if (attemptsRemaining > 0) {
            //             Logger::D(LOG_TAG, "onSetComplete: attemptsRemaining=%d", attemptsRemaining);
            //             AutoPtr<IResources> res;
            //             mContext->GetResources((IResources**)&res);
            //             String str;
            //             res->GetQuantityString(
            //                     Elastos::Droid::R::plurals::pinpuk_attempts,
            //                     attemptsRemaining, attemptsRemaining, &str);
            //             sb.Append(str);
            //         }
            //     }
            //     else {
            //         mContext->GetText(
            //                 Elastos::Droid::R::string::passwordIncorrect, (ICharSequence**)&cs);
            //         sb.Append(cs);
            //     }
            // }
            // else if (err == CommandExceptionError_SIM_PUK2) {
            //     mContext->GetText(
            //             Elastos::Droid::R::string::badPin, (ICharSequence**)&cs);
            //     sb.Append(cs);
            //     sb.Append("\n");
            //     mContext->GetText(
            //             Elastos::Droid::R::string::needPuk2, (ICharSequence**)&cs);
            //     sb.Append(cs);
            // }
            // else if (err == CommandExceptionError_REQUEST_NOT_SUPPORTED) {
            //     if (mSc.Equals(SC_PIN)) {
            //         mContext->GetText(
            //             Elastos::Droid::R::string::enablePin, (ICharSequence**)&cs);
            //         sb.Append(cs);
            //     }
            // }
            // else if (err == CommandExceptionError_FDN_CHECK_FAILURE) {
            //     Logger::I(LOG_TAG, "FDN_CHECK_FAILURE");
            //     mContext->GetText(
            //         Elastos::Droid::R::string::mmiFdnError, (ICharSequence**)&cs);
            //     sb.Append(cs);
            // }
            // else {
            //     sb.Append(GetErrorMessage(ar));
            // }
        }
        else {
            mContext->GetText(
                    Elastos::Droid::R::string::mmiError, (ICharSequence**)&cs);
            sb.Append(cs);
        }
    }
    else if (IsActivate(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        if (mIsCallFwdReg) {
            mContext->GetText(
                    Elastos::Droid::R::string::serviceRegistered, (ICharSequence**)&cs);
            sb.Append(cs);
        }
        else {
            mContext->GetText(
                    Elastos::Droid::R::string::serviceEnabled, (ICharSequence**)&cs);
            sb.Append(cs);
        }
        // Record CLIR setting
        if (mSc.Equals(SC_CLIR)) {
            mPhone->SaveClirSetting(ICommandsInterface::CLIR_INVOCATION);
        }
    }
    else if (IsDeactivate(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        mContext->GetText(
                Elastos::Droid::R::string::serviceDisabled, (ICharSequence**)&cs);
        sb.Append(cs);
        // Record CLIR setting
        if (mSc.Equals(SC_CLIR)) {
            mPhone->SaveClirSetting(ICommandsInterface::CLIR_SUPPRESSION);
        }
    }
    else if (IsRegister(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        mContext->GetText(
                Elastos::Droid::R::string::serviceRegistered, (ICharSequence**)&cs);
        sb.Append(cs);
    }
    else if (IsErasure(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        mContext->GetText(
                Elastos::Droid::R::string::serviceErased, (ICharSequence**)&cs);
        sb.Append(cs);
    }
    else {
        mState = IMmiCodeState_FAILED;
        mContext->GetText(
                Elastos::Droid::R::string::mmiError, (ICharSequence**)&cs);
        sb.Append(cs);
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
}

void CGsmMmiCode::OnGetClirComplete(
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
        AutoPtr<IArrayOf> clirArgs;

        clirArgs = IArrayOf::Probe(ar->mResult);

        // the 'm' parameter from TS 27.007 7.7
        AutoPtr<IInterface> obj;
        clirArgs->Get(1, (IInterface**)&obj);
        Int32 val0, val1;
        IInteger32::Probe(obj)->GetValue(&val1);
        obj = NULL;
        clirArgs->Get(0, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&val0);
        cs = NULL;
        switch (val1) {
            case 0: // CLIR not provisioned
                mContext->GetText(
                        Elastos::Droid::R::string::serviceNotProvisioned, (ICharSequence**)&cs);
                sb.Append(cs);
                mState = IMmiCodeState_COMPLETE;
            break;

            case 1: // CLIR provisioned in permanent mode
                mContext->GetText(
                        Elastos::Droid::R::string::CLIRPermanent, (ICharSequence**)&cs);
                sb.Append(cs);
                mState = IMmiCodeState_COMPLETE;
            break;

            case 2: // unknown (e.g. no network, etc.)
                mContext->GetText(
                        Elastos::Droid::R::string::mmiError, (ICharSequence**)&cs);
                sb.Append(cs);
                mState = IMmiCodeState_FAILED;
            break;

            case 3: // CLIR temporary mode presentation restricted

                // the 'n' parameter from TS 27.007 7.7
                switch (val0) {
                    default:
                    case 0: // Default
                        mContext->GetText(
                                Elastos::Droid::R::string::CLIRDefaultOnNextCallOn, (ICharSequence**)&cs);
                        sb.Append(cs);
                    break;
                    case 1: // CLIR invocation
                        mContext->GetText(
                                Elastos::Droid::R::string::CLIRDefaultOnNextCallOn, (ICharSequence**)&cs);
                        sb.Append(cs);
                    break;
                    case 2: // CLIR suppression
                        mContext->GetText(
                                Elastos::Droid::R::string::CLIRDefaultOnNextCallOff, (ICharSequence**)&cs);
                        sb.Append(cs);
                    break;
                }
                mState = IMmiCodeState_COMPLETE;
            break;

            case 4: // CLIR temporary mode presentation allowed
                // the 'n' parameter from TS 27.007 7.7
                switch (val0) {
                    default:
                    case 0: // Default
                        mContext->GetText(
                                Elastos::Droid::R::string::CLIRDefaultOffNextCallOff, (ICharSequence**)&cs);
                        sb.Append(cs);
                    break;
                    case 1: // CLIR invocation
                        mContext->GetText(
                                Elastos::Droid::R::string::CLIRDefaultOffNextCallOn, (ICharSequence**)&cs);
                        sb.Append(cs);
                    break;
                    case 2: // CLIR suppression
                        mContext->GetText(
                                Elastos::Droid::R::string::CLIRDefaultOffNextCallOff, (ICharSequence**)&cs);
                        sb.Append(cs);
                    break;
                }

                mState = IMmiCodeState_COMPLETE;
            break;
        }
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
}

AutoPtr<ICharSequence> CGsmMmiCode::ServiceClassToCFString(
    /* [in] */ Int32 serviceClass)
{
    AutoPtr<ICharSequence> cs;
    switch (serviceClass) {
        case ICommandsInterface::SERVICE_CLASS_VOICE:
            mContext->GetText(Elastos::Droid::R::string::serviceClassVoice, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_DATA:
            mContext->GetText(Elastos::Droid::R::string::serviceClassData, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_FAX:
            mContext->GetText(Elastos::Droid::R::string::serviceClassFAX, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_SMS:
            mContext->GetText(Elastos::Droid::R::string::serviceClassSMS, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_DATA_SYNC:
            mContext->GetText(Elastos::Droid::R::string::serviceClassDataSync, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_DATA_ASYNC:
            mContext->GetText(Elastos::Droid::R::string::serviceClassDataAsync, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_PACKET:
            mContext->GetText(Elastos::Droid::R::string::serviceClassPacket, (ICharSequence**)&cs);
            return cs;
        case ICommandsInterface::SERVICE_CLASS_PAD:
            mContext->GetText(Elastos::Droid::R::string::serviceClassPAD, (ICharSequence**)&cs);
            return cs;
        default:
            return NULL;
    }
}

AutoPtr<ICharSequence> CGsmMmiCode::MakeCFQueryResultMessage(
    /* [in] */ ICallForwardInfo* _info,
    /* [in] */ Int32 serviceClassMask)
{
    AutoPtr<CallForwardInfo> info = (CallForwardInfo*)info;
    AutoPtr<ICharSequence> cs;
    AutoPtr<ArrayOf<String> > sources = ArrayOf<String>::Alloc(3); //{"{0}", "{1}", "{2}"};
    (*sources)[0] = String("0");
    (*sources)[1] = String("1");
    (*sources)[2] = String("2");
    AutoPtr<ArrayOf<ICharSequence*> > destinations = ArrayOf<ICharSequence*>::Alloc(3);
    Boolean needTimeTemplate;

    // CF_REASON_NO_REPLY also has a time value associated with
    // it. All others don't.

    needTimeTemplate =
        (info->mReason == ICommandsInterface::CF_REASON_NO_REPLY);

    if (info->mStatus == 1) {
        if (needTimeTemplate) {
            mContext->GetText(
                    Elastos::Droid::R::string::cfTemplateForwardedTime, (ICharSequence**)&cs);
        } else {
            mContext->GetText(
                    Elastos::Droid::R::string::cfTemplateForwarded, (ICharSequence**)&cs);
        }
    } else if (info->mStatus == 0 && IsEmptyOrNull(CoreUtils::Convert(info->mNumber))) {
        mContext->GetText(
                    Elastos::Droid::R::string::cfTemplateNotForwarded, (ICharSequence**)&cs);
    } else { /* (info->mStatus == 0) && !IsEmptyOrNull(info->mNumber) */
        // A call forward record that is not active but contains
        // a phone number is considered "registered"

        if (needTimeTemplate) {
            mContext->GetText(
                    Elastos::Droid::R::string::cfTemplateRegisteredTime, (ICharSequence**)&cs);
        } else {
            mContext->GetText(
                    Elastos::Droid::R::string::cfTemplateRegistered, (ICharSequence**)&cs);
        }
    }

    // In the template (from strings.xmls)
    //         {0} is one of "bearerServiceCode*"
    //        {1} is dialing number
    //      {2} is time in seconds

    destinations->Set(0, ServiceClassToCFString(info->mServiceClass & serviceClassMask));
    String str;
    PhoneNumberUtils::StringFromStringAndTOA(info->mNumber, info->mToa, &str);
    destinations->Set(1, CoreUtils::Convert(FormatLtr(str)));
    destinations->Set(2, CoreUtils::Convert(StringUtils::ToString(info->mTimeSeconds)));

    if (info->mReason == ICommandsInterface::CF_REASON_UNCONDITIONAL &&
            (info->mServiceClass & serviceClassMask)
                    == ICommandsInterface::SERVICE_CLASS_VOICE) {
        Boolean cffEnabled = (info->mStatus == 1);
        if (mIccRecords != NULL) {
            mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, info->mNumber);
            ((CGSMPhone*)mPhone.Get())->SetCallForwardingPreference(cffEnabled);
        }
    }

    return TextUtils::Replace(cs, sources, destinations);
}

String CGsmMmiCode::FormatLtr(
    /* [in] */ const String& str)
{
    AutoPtr<IBidiFormatterHelper> helper;
    CBidiFormatterHelper::AcquireSingleton((IBidiFormatterHelper**)&helper);
    AutoPtr<IBidiFormatter> fmt;
    helper->GetInstance((IBidiFormatter**)&fmt);
    String tmp;
    return str == NULL ? str : (fmt->UnicodeWrap(str, TextDirectionHeuristics::LTR, TRUE, &tmp), tmp);
}

void CGsmMmiCode::OnQueryCfComplete(
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
        AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
        Int32 len;
        array->GetLength(&len);

        if (len == 0) {
            // Assume the default is not active
            AutoPtr<ICharSequence> cs;
            mContext->GetText(Elastos::Droid::R::string::serviceDisabled, (ICharSequence**)&cs);
            sb.Append(cs);

            // Set unconditional CFF in SIM to FALSE
            if (mIccRecords != NULL) {
                ((CGSMPhone*)mPhone.Get())->SetCallForwardingPreference(FALSE);
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
                for (Int32 i = 0, s = len; i < s ; i++) {
                    AutoPtr<IInterface> obj;
                    array->Get(i, (IInterface**)&obj);
                    AutoPtr<ICallForwardInfo> info = ICallForwardInfo::Probe(obj);
                    if ((serviceClassMask & ((CallForwardInfo*)info.Get())->mServiceClass) != 0) {
                        IAppendable::Probe(tb)->Append(MakeCFQueryResultMessage(info, serviceClassMask));
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
    ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
}

void CGsmMmiCode::OnQueryComplete(
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
        AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
        Int32 len;
        array->GetLength(&len);

        if (len != 0) {
            AutoPtr<IInterface> obj;
            array->Get(0, (IInterface**)&obj);
            Int32 val;
            IInteger32::Probe(obj)->GetValue(&val);
            AutoPtr<ICharSequence> cs;
            Boolean b;
            if (val == 0) {
                mContext->GetText(Elastos::Droid::R::string::serviceDisabled, (ICharSequence**)&cs);
                sb.Append(cs);
            }
            else if (mSc.Equals(SC_WAIT)) {
                // Call Waiting includes additional data in the response.
                AutoPtr<IInterface> obj;
                array->Get(1, (IInterface**)&obj);
                Int32 val;
                IInteger32::Probe(obj)->GetValue(&val);
                sb.Append(CreateQueryCallWaitingResultMessage(val));
            }
            else if (IsServiceCodeCallBarring(mSc, &b), b) {
                // ints[0] for Call Barring is a bit vector of services
                AutoPtr<IInterface> obj;
                array->Get(0, (IInterface**)&obj);
                Int32 val;
                IInteger32::Probe(obj)->GetValue(&val);
                sb.Append(CreateQueryCallBarringResultMessage(val));
            }
            else if (val == 1) {
                // for all other services, treat it as a boolean
                mContext->GetText(Elastos::Droid::R::string::serviceEnabled, (ICharSequence**)&cs);
                sb.Append(cs);
            }
            else {
                mContext->GetText(Elastos::Droid::R::string::mmiError, (ICharSequence**)&cs);
                sb.Append(cs);
            }
        }
        else {
            mContext->GetText(Elastos::Droid::R::string::mmiError, (ICharSequence**)&cs);
            sb.Append(cs);
        }
        mState = IMmiCodeState_COMPLETE;
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((CGSMPhone*)mPhone.Get())->OnMMIDone(this);
}

AutoPtr<ICharSequence> CGsmMmiCode::CreateQueryCallWaitingResultMessage(
    /* [in] */ Int32 serviceClass)
{
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::R::string::serviceEnabledFor, (ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);

    for (Int32 classMask = 1
                ; classMask <= ICommandsInterface::SERVICE_CLASS_MAX
                ; classMask <<= 1
    ) {
        if ((classMask & serviceClass) != 0) {
            sb.Append("\n");
            sb.Append(ServiceClassToCFString(classMask & serviceClass));
        }
    }

    sb.ToString(&str);
    return CoreUtils::Convert(str);
}

AutoPtr<ICharSequence> CGsmMmiCode::CreateQueryCallBarringResultMessage(
    /* [in] */ Int32 serviceClass)
{
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::R::string::serviceEnabledFor, (ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    StringBuilder sb(str);

    for (Int32 classMask = 1
                ; classMask <= ICommandsInterface::SERVICE_CLASS_MAX
                ; classMask <<= 1
    ) {
        if ((classMask & serviceClass) != 0) {
            sb.Append("\n");
            sb.Append(ServiceClassToCFString(classMask & serviceClass));
        }
    }

    sb.ToString(&str);
    return CoreUtils::Convert(str);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos