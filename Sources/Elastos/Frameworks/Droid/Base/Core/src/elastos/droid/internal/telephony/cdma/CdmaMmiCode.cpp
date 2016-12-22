#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaMmiCode.h"
#include "elastos/droid/internal/telephony/cdma/CdmaMmiCode.h"
#include "elastos/droid/internal/telephony/cdma/CDMAPhone.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_PUK;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                             CdmaMmiCode
//=====================================================================
CAR_INTERFACE_IMPL_2(CdmaMmiCode, Handler, IMmiCode, ICdmaMmiCode);

const String CdmaMmiCode::LOGTAG("CdmaMmiCode");
const String CdmaMmiCode::ACTION_REGISTER("**");
const String CdmaMmiCode::SC_PIN("04");
const String CdmaMmiCode::SC_PIN2("042");
const String CdmaMmiCode::SC_PUK("05");
const String CdmaMmiCode::SC_PUK2("052");
const Int32 CdmaMmiCode::EVENT_SET_COMPLETE;

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String& regularExpression)
{
    AutoPtr<IPatternHelper> patternHelper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&patternHelper);
    AutoPtr<IPattern> pattern;
    patternHelper->Compile(regularExpression, (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> CdmaMmiCode::sPatternSuppService = InitPattern(String("((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)"));

const Int32 CdmaMmiCode::MATCH_GROUP_POUND_STRING;
const Int32 CdmaMmiCode::MATCH_GROUP_ACTION;
const Int32 CdmaMmiCode::MATCH_GROUP_SERVICE_CODE;
const Int32 CdmaMmiCode::MATCH_GROUP_SIA;
const Int32 CdmaMmiCode::MATCH_GROUP_SIB;
const Int32 CdmaMmiCode::MATCH_GROUP_SIC;
const Int32 CdmaMmiCode::MATCH_GROUP_PWD_CONFIRM;
const Int32 CdmaMmiCode::MATCH_GROUP_DIALING_NUMBER;

CdmaMmiCode::CdmaMmiCode()
    : mState(IMmiCodeState_PENDING)
{
}

ECode CdmaMmiCode::constructor(
    /* [in] */ ICDMAPhone* phone,
    /* [in] */ IUiccCardApplication* app)
{
    AutoPtr<IHandler> handler;
    IPhoneBase::Probe(phone)->GetHandler((IHandler**)&handler);
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    Handler::constructor(looper);
    mPhone = phone;
    IPhone::Probe(phone)->GetContext((IContext**)&mContext);
    mUiccApplication = app;
    return NOERROR;
}

AutoPtr<ICdmaMmiCode> CdmaMmiCode::NewFromDialString(
    /* [in] */ const String& dialString,
    /* [in] */ ICDMAPhone* phone,
    /* [in] */ IUiccCardApplication* app)
{
    AutoPtr<IMatcher> m;
    AutoPtr<CCdmaMmiCode> ret;

    sPatternSuppService->Matcher(dialString, (IMatcher**)&m);

    // Is this formatted like a standard supplementary service code?
    Boolean b;
    if (m->Matches(&b), b) {
        CCdmaMmiCode::NewByFriend(phone, app, (CCdmaMmiCode**)&ret);
        String str;
        ret->mPoundString = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_POUND_STRING, &str), str));
        ret->mAction = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_ACTION, &str), str));
        ret->mSc = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SERVICE_CODE, &str), str));
        ret->mSia = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SIA, &str), str));
        ret->mSib = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SIB, &str), str));
        ret->mSic = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_SIC, &str), str));
        ret->mPwd = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_PWD_CONFIRM, &str), str));
        ret->mDialingNumber = MakeEmptyNull((IMatchResult::Probe(m)->Group(MATCH_GROUP_DIALING_NUMBER, &str), str));
    }

    return ICdmaMmiCode::Probe(ret);
}

ECode CdmaMmiCode::GetState(
    /* [out] */ IMmiCodeState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

ECode CdmaMmiCode::GetMessage(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMessage;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaMmiCode::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhone::Probe(mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaMmiCode::Cancel()
{
    // Complete or failed cannot be cancelled
    if (mState == IMmiCodeState_COMPLETE || mState == IMmiCodeState_FAILED) {
        return NOERROR;
    }

    mState = IMmiCodeState_CANCELLED;
    ((CDMAPhone*)mPhone.Get())->OnMMIDone (this);

    return NOERROR;
}

ECode CdmaMmiCode::IsCancelable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    return NOERROR;
}

ECode CdmaMmiCode::IsPinPukCommand(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !mSc.IsNull() && (mSc.Equals(SC_PIN) || mSc.Equals(SC_PIN2)
                          || mSc.Equals(SC_PUK) || mSc.Equals(SC_PUK2));

    return NOERROR;
}

ECode CdmaMmiCode::IsRegister(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAction != NULL && mAction.Equals(ACTION_REGISTER);

    return NOERROR;
}

ECode CdmaMmiCode::IsUssdRequest(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::W(LOGTAG, "isUssdRequest is not implemented in CdmaMmiCode");
    *result = FALSE;

    return NOERROR;
}

ECode CdmaMmiCode::ProcessCode()
{
    // try {
    Boolean b;
    if (IsPinPukCommand(&b), b) {
        // TODO: This is the same as the code in GsmMmiCode.java,
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
                HandlePasswordError(R::string::mismatchPin);
            }
            else if (pinLen < 4 || pinLen > 8 ) {
                // invalid length
                HandlePasswordError(R::string::invalidPin);
            }
            else if (mSc.Equals(SC_PIN)
                    && mUiccApplication != NULL
                    && (mUiccApplication->GetState(&state), state) == APPSTATE_PUK) {
                // Sim is puk-locked
                HandlePasswordError(R::string::needPuk);
            }
            else if (mUiccApplication != NULL) {
                Logger::D(LOGTAG, "process mmi service code using UiccApp sc=%s", mSc.string());

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
                    // throw new RuntimeException("Unsupported service code=" + mSc);
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
    // } catch (RuntimeException exc) {
    //     mState = IMmiCodeState_FAILED;
    //     mMessage = mContext.getText(R::string::mmiError);
    //     mPhone.onMMIDone(this);
    // }

    return NOERROR;
}

ECode CdmaMmiCode::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<AsyncResult> ar;

    Int32 what;
    msg->GetWhat(&what);
    if (what == EVENT_SET_COMPLETE) {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);
        OnSetComplete(msg, ar);
    }
    else {
        Logger::E(LOGTAG, "Unexpected reply");
    }

    return NOERROR;
}

String CdmaMmiCode::MakeEmptyNull(
    /* [in] */ const String& s)
{
    if (!s.IsNull() && s.GetLength() == 0) return String(NULL);

    return s;
}

void CdmaMmiCode::HandlePasswordError(
    /* [in] */ Int32 res)
{
    mState = IMmiCodeState_FAILED;
    String str;
    GetScString()->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");
    AutoPtr<ICharSequence> cs;
    mContext->GetText(res, (ICharSequence**)&cs);
    cs->ToString(&str);
    sb.Append(str);
    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((CDMAPhone*)mPhone.Get())->OnMMIDone(this);
}

AutoPtr<ICharSequence> CdmaMmiCode::GetScString()
{
    if (!mSc.IsNull()) {
        Boolean b;
        if (IsPinPukCommand(&b), b) {
            AutoPtr<ICharSequence> cs;
            mContext->GetText(R::string::PinMmi, (ICharSequence**)&cs);
            return cs;
        }
    }

    return CoreUtils::Convert(String(""));
}

void CdmaMmiCode::OnSetComplete(
    /* [in] */ IMessage* msg,
    /* [in] */ AsyncResult* ar)
{
    String str;
    GetScString()->ToString(&str);
    StringBuilder sb(str);
    sb.Append("\n");

    AutoPtr<ICharSequence> cs;
    Boolean b;
    if (ar->mException != NULL) {
        mState = IMmiCodeState_FAILED;
        if (ICommandException::Probe(ar->mException) != NULL) {
// TODO: Need CommandException.Error
            // CommandException.Error err = ((CommandException)(ar->mException)).getCommandError();
            // if (err == CommandException.Error.PASSWORD_INCORRECT) {
            //     if (IsPinPukCommand(&b), b) {
            //         // look specifically for the PUK commands and adjust
            //         // the message accordingly.
            //         if (mSc.Equals(SC_PUK) || mSc.Equals(SC_PUK2)) {
            //             sb.Append((mContext->GetText(R::string::badPuk, (ICharSequence**)&cs), cs));
            //         }
            //         else {
            //             sb.Append((mContext->GetText(R::string::badPin, (ICharSequence**)&cs), cs));
            //         }
            //         // Get the No. of retries remaining to unlock PUK/PUK2
            //         Int32 attemptsRemaining;
            //         msg->GetArg1(&attemptsRemaining);
            //         if (attemptsRemaining <= 0) {
            //             Logger::D(LOGTAG, String("onSetComplete: PUK locked,")
            //                     + " cancel as lock screen will handle this");
            //             mState = IMmiCodeState_CANCELLED;
            //         }
            //         else if (attemptsRemaining > 0) {
            //             Logger::D(LOGTAG, "onSetComplete: attemptsRemaining=%d", attemptsRemaining);
            //             AutoPtr<IResources> res;
            //             mContext->GetResources((IResources**)&res);
            //             res->GetQuantityString(
            //                     R::plurals::pinpuk_attempts,
            //                     attemptsRemaining, attemptsRemaining, &str);
            //             sb.Append(str);
            //         }
            //     }
            //     else {
            //         sb.Append((mContext->GetText(R::string::passwordIncorrect, (ICharSequence**)&cs), cs));
            //     }
            // }
            // else if (err == CommandException.Error.SIM_PUK2) {
            //     sb.Append((mContext->GetText(R::string::badPin, (ICharSequence**)&cs), cs));
            //     sb.Append("\n");
            //     sb.Append((mContext->GetText(R::string::needPuk2, (ICharSequence**)&cs), cs));
            // }
            // else if (err == CommandException.Error.REQUEST_NOT_SUPPORTED) {
            //     if (mSc.Equals(SC_PIN)) {
            //         sb.Append((mContext->GetText(R::string::enablePin, (ICharSequence**)&cs), cs));
            //     }
            // }
            // else {
            //     sb.Append((mContext->GetText(R::string::mmiError, (ICharSequence**)&cs), cs));
            // }
        }
        else {
            sb.Append((mContext->GetText(R::string::mmiError, (ICharSequence**)&cs), cs));
        }
    }
    else if (IsRegister(&b), b) {
        mState = IMmiCodeState_COMPLETE;
        sb.Append((mContext->GetText(R::string::serviceRegistered, (ICharSequence**)&cs), cs));
    }
    else {
        mState = IMmiCodeState_FAILED;
        sb.Append((mContext->GetText(R::string::mmiError, (ICharSequence**)&cs), cs));
    }

    sb.ToString(&str);
    mMessage = CoreUtils::Convert(str);
    ((CDMAPhone*)mPhone.Get())->OnMMIDone(this);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
