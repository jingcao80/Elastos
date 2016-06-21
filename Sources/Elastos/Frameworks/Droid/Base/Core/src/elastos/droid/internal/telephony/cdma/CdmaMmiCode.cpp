#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaMmiCode.h"

using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

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
{
}

ECode CdmaMmiCode::constructor(
    /* [in] */ ICDMAPhone* phone,
    /* [in] */ IUiccCardApplication* app)
{
    // ==================before translated======================
    // super(phone.getHandler().getLooper());
    // mPhone = phone;
    // mContext = phone.getContext();
    // mUiccApplication = app;
    return NOERROR;
}

AutoPtr<ICdmaMmiCode> CdmaMmiCode::NewFromDialString(
    /* [in] */ const String& dialString,
    /* [in] */ ICDMAPhone* phone,
    /* [in] */ IUiccCardApplication* app)
{
    // ==================before translated======================
    // Matcher m;
    // CdmaMmiCode ret = null;
    //
    // m = sPatternSuppService.matcher(dialString);
    //
    // // Is this formatted like a standard supplementary service code?
    // if (m.matches()) {
    //     ret = new CdmaMmiCode(phone,app);
    //     ret.mPoundString = makeEmptyNull(m.group(MATCH_GROUP_POUND_STRING));
    //     ret.mAction = makeEmptyNull(m.group(MATCH_GROUP_ACTION));
    //     ret.mSc = makeEmptyNull(m.group(MATCH_GROUP_SERVICE_CODE));
    //     ret.mSia = makeEmptyNull(m.group(MATCH_GROUP_SIA));
    //     ret.mSib = makeEmptyNull(m.group(MATCH_GROUP_SIB));
    //     ret.mSic = makeEmptyNull(m.group(MATCH_GROUP_SIC));
    //     ret.mPwd = makeEmptyNull(m.group(MATCH_GROUP_PWD_CONFIRM));
    //     ret.mDialingNumber = makeEmptyNull(m.group(MATCH_GROUP_DIALING_NUMBER));
    //
    // }
    //
    // return ret;
    assert(0);
    AutoPtr<ICdmaMmiCode> empty;
    return empty;
}

ECode CdmaMmiCode::GetState(
    /* [out] */ IMmiCodeState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState;
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::GetMessage(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mMessage;
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ((Phone) mPhone);
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::Cancel()
{
    // ==================before translated======================
    // // Complete or failed cannot be cancelled
    // if (mState == State.COMPLETE || mState == State.FAILED) {
    //     return;
    // }
    //
    // mState = State.CANCELLED;
    // mPhone.onMMIDone (this);
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::IsCancelable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::IsPinPukCommand(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSc != null && (mSc.equals(SC_PIN) || mSc.equals(SC_PIN2)
    //                       || mSc.equals(SC_PUK) || mSc.equals(SC_PUK2));
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::IsRegister(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAction != null && mAction.equals(ACTION_REGISTER);
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::IsUssdRequest(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // Rlog.w(LOGTAG, "isUssdRequest is not implemented in CdmaMmiCode");
    // return false;
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::ProcessCode()
{
    // ==================before translated======================
    // try {
    //     if (isPinPukCommand()) {
    //         // TODO: This is the same as the code in GsmMmiCode.java,
    //         // MmiCode should be an abstract or base class and this and
    //         // other common variables and code should be promoted.
    //
    //         // sia = old PIN or PUK
    //         // sib = new PIN
    //         // sic = new PIN
    //         String oldPinOrPuk = mSia;
    //         String newPinOrPuk = mSib;
    //         int pinLen = newPinOrPuk.length();
    //         if (isRegister()) {
    //             if (!newPinOrPuk.equals(mSic)) {
    //                 // password mismatch; return error
    //                 handlePasswordError(com.android.internal.R.string.mismatchPin);
    //             } else if (pinLen < 4 || pinLen > 8 ) {
    //                 // invalid length
    //                 handlePasswordError(com.android.internal.R.string.invalidPin);
    //             } else if (mSc.equals(SC_PIN)
    //                     && mUiccApplication != null
    //                     && mUiccApplication.getState() == AppState.APPSTATE_PUK) {
    //                 // Sim is puk-locked
    //                 handlePasswordError(com.android.internal.R.string.needPuk);
    //             } else if (mUiccApplication != null) {
    //                 Rlog.d(LOGTAG, "process mmi service code using UiccApp sc=" + mSc);
    //
    //                 // We have an app and the pre-checks are OK
    //                 if (mSc.equals(SC_PIN)) {
    //                     mUiccApplication.changeIccLockPassword(oldPinOrPuk, newPinOrPuk,
    //                             obtainMessage(EVENT_SET_COMPLETE, this));
    //                 } else if (mSc.equals(SC_PIN2)) {
    //                     mUiccApplication.changeIccFdnPassword(oldPinOrPuk, newPinOrPuk,
    //                             obtainMessage(EVENT_SET_COMPLETE, this));
    //                 } else if (mSc.equals(SC_PUK)) {
    //                     mUiccApplication.supplyPuk(oldPinOrPuk, newPinOrPuk,
    //                             obtainMessage(EVENT_SET_COMPLETE, this));
    //                 } else if (mSc.equals(SC_PUK2)) {
    //                     mUiccApplication.supplyPuk2(oldPinOrPuk, newPinOrPuk,
    //                             obtainMessage(EVENT_SET_COMPLETE, this));
    //                 } else {
    //                     throw new RuntimeException("Unsupported service code=" + mSc);
    //                 }
    //             } else {
    //                 throw new RuntimeException("No application mUiccApplicaiton is null");
    //             }
    //         } else {
    //             throw new RuntimeException ("Ivalid register/action=" + mAction);
    //         }
    //     }
    // } catch (RuntimeException exc) {
    //     mState = State.FAILED;
    //     mMessage = mContext.getText(com.android.internal.R.string.mmiError);
    //     mPhone.onMMIDone(this);
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaMmiCode::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // AsyncResult ar;
    //
    // if (msg.what == EVENT_SET_COMPLETE) {
    //     ar = (AsyncResult) (msg.obj);
    //     onSetComplete(msg, ar);
    // } else {
    //     Rlog.e(LOGTAG, "Unexpected reply");
    // }
    assert(0);
    return NOERROR;
}

String CdmaMmiCode::MakeEmptyNull(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // if (s != null && s.length() == 0) return null;
    //
    // return s;
    assert(0);
    return String("");
}

void CdmaMmiCode::HandlePasswordError(
    /* [in] */ Int32 res)
{
    // ==================before translated======================
    // mState = State.FAILED;
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    // sb.append(mContext.getText(res));
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

AutoPtr<ICharSequence> CdmaMmiCode::GetScString()
{
    // ==================before translated======================
    // if (mSc != null) {
    //     if (isPinPukCommand()) {
    //         return mContext.getText(com.android.internal.R.string.PinMmi);
    //     }
    // }
    //
    // return "";
    assert(0);
    AutoPtr<ICharSequence> empty;
    return empty;
}

void CdmaMmiCode::OnSetComplete(
    /* [in] */ IMessage* msg,
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    //
    // if (ar.exception != null) {
    //     mState = State.FAILED;
    //     if (ar.exception instanceof CommandException) {
    //         CommandException.Error err = ((CommandException)(ar.exception)).getCommandError();
    //         if (err == CommandException.Error.PASSWORD_INCORRECT) {
    //             if (isPinPukCommand()) {
    //                 // look specifically for the PUK commands and adjust
    //                 // the message accordingly.
    //                 if (mSc.equals(SC_PUK) || mSc.equals(SC_PUK2)) {
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.badPuk));
    //                 } else {
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.badPin));
    //                 }
    //                 // Get the No. of retries remaining to unlock PUK/PUK2
    //                 int attemptsRemaining = msg.arg1;
    //                 if (attemptsRemaining <= 0) {
    //                     Rlog.d(LOGTAG, "onSetComplete: PUK locked,"
    //                             + " cancel as lock screen will handle this");
    //                     mState = State.CANCELLED;
    //                 } else if (attemptsRemaining > 0) {
    //                     Rlog.d(LOGTAG, "onSetComplete: attemptsRemaining="+attemptsRemaining);
    //                     sb.append(mContext.getResources().getQuantityString(
    //                             com.android.internal.R.plurals.pinpuk_attempts,
    //                             attemptsRemaining, attemptsRemaining));
    //                 }
    //             } else {
    //                 sb.append(mContext.getText(
    //                         com.android.internal.R.string.passwordIncorrect));
    //             }
    //         } else if (err == CommandException.Error.SIM_PUK2) {
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.badPin));
    //             sb.append("\n");
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.needPuk2));
    //         } else if (err == CommandException.Error.REQUEST_NOT_SUPPORTED) {
    //             if (mSc.equals(SC_PIN)) {
    //                 sb.append(mContext.getText(com.android.internal.R.string.enablePin));
    //             }
    //         } else {
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.mmiError));
    //         }
    //     } else {
    //         sb.append(mContext.getText(
    //                 com.android.internal.R.string.mmiError));
    //     }
    // } else if (isRegister()) {
    //     mState = State.COMPLETE;
    //     sb.append(mContext.getText(
    //             com.android.internal.R.string.serviceRegistered));
    // } else {
    //     mState = State.FAILED;
    //     sb.append(mContext.getText(
    //             com.android.internal.R.string.mmiError));
    // }
    //
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
