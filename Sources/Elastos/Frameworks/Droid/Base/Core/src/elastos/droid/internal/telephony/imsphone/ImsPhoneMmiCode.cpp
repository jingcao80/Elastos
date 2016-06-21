
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneMmiCode.h"

using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

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
{
}

ECode ImsPhoneMmiCode::constructor(
    /* [in] */ IImsPhone* phone)
{
    // ==================before translated======================
    // // The telephony unit-test cases may create ImsPhoneMmiCode's
    // // in secondary threads
    // super(phone.getHandler().getLooper());
    // mPhone = phone;
    // mContext = phone.getContext();
    // mIccRecords = mPhone.mDefaultPhone.mIccRecords.get();
    return NOERROR;
}

AutoPtr<IImsPhoneMmiCode> ImsPhoneMmiCode::NewFromDialString(
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhone* phone)
{
    // ==================before translated======================
    // Matcher m;
    // ImsPhoneMmiCode ret = null;
    //
    // m = sPatternSuppService.matcher(dialString);
    //
    // // Is this formatted like a standard supplementary service code?
    // if (m.matches()) {
    //     ret = new ImsPhoneMmiCode(phone);
    //     ret.mPoundString = makeEmptyNull(m.group(MATCH_GROUP_POUND_STRING));
    //     ret.mAction = makeEmptyNull(m.group(MATCH_GROUP_ACTION));
    //     ret.mSc = makeEmptyNull(m.group(MATCH_GROUP_SERVICE_CODE));
    //     ret.mSia = makeEmptyNull(m.group(MATCH_GROUP_SIA));
    //     ret.mSib = makeEmptyNull(m.group(MATCH_GROUP_SIB));
    //     ret.mSic = makeEmptyNull(m.group(MATCH_GROUP_SIC));
    //     ret.mPwd = makeEmptyNull(m.group(MATCH_GROUP_PWD_CONFIRM));
    //     ret.mDialingNumber = makeEmptyNull(m.group(MATCH_GROUP_DIALING_NUMBER));
    //     // According to TS 22.030 6.5.2 "Structure of the MMI",
    //     // the dialing number should not ending with #.
    //     // The dialing number ending # is treated as unique USSD,
    //     // eg, *400#16 digit number# to recharge the prepaid card
    //     // in India operator(Mumbai MTNL)
    //     if (ret.mDialingNumber != null &&
    //             ret.mDialingNumber.endsWith("#") &&
    //             dialString.endsWith("#")){
    //         ret = new ImsPhoneMmiCode(phone);
    //         ret.mPoundString = dialString;
    //     }
    // } else if (dialString.endsWith("#")) {
    //     // TS 22.030 sec 6.5.3.2
    //     // "Entry of any characters defined in the 3GPP TS 23.038 [8] Default Alphabet
    //     // (up to the maximum defined in 3GPP TS 24.080 [10]), followed by #SEND".
    //
    //     ret = new ImsPhoneMmiCode(phone);
    //     ret.mPoundString = dialString;
    // } else if (isTwoDigitShortCode(phone.getContext(), dialString)) {
    //     //Is a country-specific exception to short codes as defined in TS 22.030, 6.5.3.2
    //     ret = null;
    // } else if (isShortCode(dialString, phone)) {
    //     // this may be a short code, as defined in TS 22.030, 6.5.3.2
    //     ret = new ImsPhoneMmiCode(phone);
    //     ret.mDialingNumber = dialString;
    // }
    //
    // return ret;
    assert(0);
    AutoPtr<IImsPhoneMmiCode> empty;
    return empty;
}

AutoPtr<IImsPhoneMmiCode> ImsPhoneMmiCode::NewNetworkInitiatedUssd(
    /* [in] */ const String& ussdMessage,
    /* [in] */ Boolean isUssdRequest,
    /* [in] */ IImsPhone* phone)
{
    // ==================before translated======================
    // ImsPhoneMmiCode ret;
    //
    // ret = new ImsPhoneMmiCode(phone);
    //
    // ret.mMessage = ussdMessage;
    // ret.mIsUssdRequest = isUssdRequest;
    //
    // // If it's a request, set to PENDING so that it's cancelable.
    // if (isUssdRequest) {
    //     ret.mIsPendingUSSD = true;
    //     ret.mState = State.PENDING;
    // } else {
    //     ret.mState = State.COMPLETE;
    // }
    //
    // return ret;
    assert(0);
    AutoPtr<IImsPhoneMmiCode> empty;
    return empty;
}

AutoPtr<IImsPhoneMmiCode> ImsPhoneMmiCode::NewFromUssdUserInput(
    /* [in] */ const String& ussdMessge,
    /* [in] */ IImsPhone* phone)
{
    // ==================before translated======================
    // ImsPhoneMmiCode ret = new ImsPhoneMmiCode(phone);
    //
    // ret.mMessage = ussdMessge;
    // ret.mState = State.PENDING;
    // ret.mIsPendingUSSD = true;
    //
    // return ret;
    assert(0);
    AutoPtr<IImsPhoneMmiCode> empty;
    return empty;
}

Boolean ImsPhoneMmiCode::IsScMatchesSuppServType(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // boolean isMatch = false;
    // Matcher m = sPatternSuppService.matcher(dialString);
    // if (m.matches()) {
    //     String sc = makeEmptyNull(m.group(MATCH_GROUP_SERVICE_CODE));
    //     if (sc.equals(SC_CFUT)) {
    //         isMatch = true;
    //     } else if(sc.equals(SC_BS_MT)) {
    //         isMatch = true;
    //     }
    // }
    // return isMatch;
    assert(0);
    return FALSE;
}

Boolean ImsPhoneMmiCode::IsServiceCodeCallForwarding(
    /* [in] */ const String& sc)
{
    // ==================before translated======================
    // return sc != null &&
    //         (sc.equals(SC_CFU)
    //         || sc.equals(SC_CFB) || sc.equals(SC_CFNRy)
    //         || sc.equals(SC_CFNR) || sc.equals(SC_CF_All)
    //         || sc.equals(SC_CF_All_Conditional));
    assert(0);
    return FALSE;
}

Boolean ImsPhoneMmiCode::IsServiceCodeUncondCallFwdTimer(
    /* [in] */ const String& sc)
{
    // ==================before translated======================
    // return sc != null && sc.equals(SC_CFUT);
    assert(0);
    return FALSE;
}

Boolean ImsPhoneMmiCode::IsServiceCodeCallBarring(
    /* [in] */ const String& sc)
{
    // ==================before translated======================
    // Resources resource = Resources.getSystem();
    // if (sc != null) {
    //     String[] barringMMI = resource.getStringArray(
    //         com.android.internal.R.array.config_callBarringMMI);
    //     if (barringMMI != null) {
    //         for (String match : barringMMI) {
    //             if (sc.equals(match)) return true;
    //         }
    //     }
    // }
    // return false;
    assert(0);
    return FALSE;
}

String ImsPhoneMmiCode::ScToBarringFacility(
    /* [in] */ const String& sc)
{
    // ==================before translated======================
    // if (sc == null) {
    //     throw new RuntimeException ("invalid call barring sc");
    // }
    //
    // if (sc.equals(SC_BAOC)) {
    //     return CommandsInterface.CB_FACILITY_BAOC;
    // } else if (sc.equals(SC_BAOIC)) {
    //     return CommandsInterface.CB_FACILITY_BAOIC;
    // } else if (sc.equals(SC_BAOICxH)) {
    //     return CommandsInterface.CB_FACILITY_BAOICxH;
    // } else if (sc.equals(SC_BAIC)) {
    //     return CommandsInterface.CB_FACILITY_BAIC;
    // } else if (sc.equals(SC_BAICr)) {
    //     return CommandsInterface.CB_FACILITY_BAICr;
    // } else if (sc.equals(SC_BA_ALL)) {
    //     return CommandsInterface.CB_FACILITY_BA_ALL;
    // } else if (sc.equals(SC_BA_MO)) {
    //     return CommandsInterface.CB_FACILITY_BA_MO;
    // } else if (sc.equals(SC_BA_MT)) {
    //     return CommandsInterface.CB_FACILITY_BA_MT;
    // } else {
    //     throw new RuntimeException ("invalid call barring sc");
    // }
    assert(0);
    return String("");
}

ECode ImsPhoneMmiCode::GetState(
    /* [out] */ IMmiCodeState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetMessage(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMessage;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPhone;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::Cancel()
{
    // ==================before translated======================
    // // Complete or failed cannot be cancelled
    // if (mState == State.COMPLETE || mState == State.FAILED) {
    //     return;
    // }
    //
    // mState = State.CANCELLED;
    //
    // if (mIsPendingUSSD) {
    //     mPhone.cancelUSSD();
    // } else {
    //     mPhone.onMMIDone (this);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsCancelable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // /* Can only cancel pending USSD sessions. */
    // return mIsPendingUSSD;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetDialingNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDialingNumber;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsMMI(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPoundString != null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsShortCode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mPoundString == null
    //             && mDialingNumber != null && mDialingNumber.length() <= 2;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsPinPukCommand(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSc != null && (mSc.equals(SC_PIN) || mSc.equals(SC_PIN2)
    //                       || mSc.equals(SC_PUK) || mSc.equals(SC_PUK2));
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsTemporaryModeCLIR(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSc != null && mSc.equals(SC_CLIR) && mDialingNumber != null
    //         && (isActivate() || isDeactivate());
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::GetCLIRMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mSc != null && mSc.equals(SC_CLIR)) {
    //     if (isActivate()) {
    //         return CommandsInterface.CLIR_SUPPRESSION;
    //     } else if (isDeactivate()) {
    //         return CommandsInterface.CLIR_INVOCATION;
    //     }
    // }
    //
    // return CommandsInterface.CLIR_DEFAULT;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsActivate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAction != null && mAction.equals(ACTION_ACTIVATE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsDeactivate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAction != null && mAction.equals(ACTION_DEACTIVATE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsInterrogate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAction != null && mAction.equals(ACTION_INTERROGATE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsRegister(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAction != null && mAction.equals(ACTION_REGISTER);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsErasure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAction != null && mAction.equals(ACTION_ERASURE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsPendingUSSD(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsPendingUSSD;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsUssdRequest(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsUssdRequest;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::IsSupportedOverImsPhone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (isShortCode()) return true;
    // else if (mDialingNumber != null) return false;
    // else if (isServiceCodeCallForwarding(mSc)
    //         || isServiceCodeCallBarring(mSc)
    //         || (mSc != null && mSc.equals(SC_WAIT))
    //         || (mSc != null && mSc.equals(SC_CLIR))
    //         || (mSc != null && mSc.equals(SC_CLIP))
    //         || (mSc != null && mSc.equals(SC_COLR))
    //         || (mSc != null && mSc.equals(SC_COLP))
    //         || (mSc != null && mSc.equals(SC_BS_MT))
    //         || (mSc != null && mSc.equals(SC_BAICa))) {
    //
    //     int serviceClass = siToServiceClass(mSib);
    //     if (serviceClass != SERVICE_CLASS_NONE
    //             && serviceClass != SERVICE_CLASS_VOICE) {
    //         return false;
    //     }
    //     return true;
    // } else if (isPinPukCommand()
    //         || (mSc != null
    //             && (mSc.equals(SC_PWD) || mSc.equals(SC_CLIP) || mSc.equals(SC_CLIR)))) {
    //     return false;
    // } else if (mPoundString != null) return true;
    //
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::CalForwardAction(
    /* [in] */ const String& dialingNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (isActivate()) {
    //     // 3GPP TS 22.030 6.5.2
    //     // a call forwarding request with a single * would be
    //     // interpreted as registration if containing a forwarded-to
    //     // number, or an activation if not
    //     if (isEmptyOrNull(dialingNumber)) {
    //         mIsCallFwdReg = false;
    //         return CommandsInterface.CF_ACTION_ENABLE;
    //     } else {
    //         mIsCallFwdReg = true;
    //         return CommandsInterface.CF_ACTION_REGISTRATION;
    //     }
    // } else if (isDeactivate()) {
    //     return CommandsInterface.CF_ACTION_DISABLE;
    // } else if (isRegister()) {
    //     return CommandsInterface.CF_ACTION_REGISTRATION;
    // } else if (isErasure()) {
    //     return CommandsInterface.CF_ACTION_ERASURE;
    // } else {
    //     throw new RuntimeException ("invalid action");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::CallBarrAction(
    /* [in] */ const String& dialingNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (isActivate()) {
    //     return CommandsInterface.CF_ACTION_ENABLE;
    // } else if (isDeactivate()) {
    //     return CommandsInterface.CF_ACTION_DISABLE;
    // } else if (isRegister()) {
    //     if (!isEmptyOrNull(dialingNumber)) {
    //         return CommandsInterface.CF_ACTION_REGISTRATION;
    //     } else {
    //         throw new RuntimeException ("invalid action");
    //     }
    // } else if (isErasure()) {
    //     return CommandsInterface.CF_ACTION_ERASURE;
    // } else {
    //     throw new RuntimeException ("invalid action");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::ProcessCode()
{
    // ==================before translated======================
    // try {
    //     if (isShortCode()) {
    //         Rlog.d(LOGTAG, "isShortCode");
    //
    //         // These just get treated as USSD.
    //         Rlog.d(LOGTAG, "Sending short code '"
    //                + mDialingNumber + "' over CS pipe.");
    //         throw new CallStateException(ImsPhone.CS_FALLBACK);
    //     } else if (isServiceCodeCallForwarding(mSc)) {
    //         Rlog.d(LOGTAG, "is CF");
    //         // service group is not supported
    //
    //         String dialingNumber = mSia;
    //         int reason = scToCallForwardReason(mSc);
    //         int time = siToTime(mSic);
    //
    //         if (isInterrogate()) {
    //             mPhone.getCallForwardingOption(reason,
    //                     obtainMessage(EVENT_QUERY_CF_COMPLETE, this));
    //         } else {
    //             int cfAction;
    //
    //             cfAction = calForwardAction(dialingNumber);
    //
    //             int isSettingUnconditional =
    //                     ((reason == CommandsInterface.CF_REASON_UNCONDITIONAL) ||
    //                      (reason == CommandsInterface.CF_REASON_ALL)) ? 1 : 0;
    //
    //             int isEnableDesired =
    //                 ((cfAction == CommandsInterface.CF_ACTION_ENABLE) ||
    //                         (cfAction == CommandsInterface.CF_ACTION_REGISTRATION)) ? 1 : 0;
    //
    //             Rlog.d(LOGTAG, "is CF setCallForward");
    //             mPhone.setCallForwardingOption(cfAction, reason,
    //                     dialingNumber, time, obtainMessage(
    //                             EVENT_SET_CFF_COMPLETE,
    //                             isSettingUnconditional,
    //                             isEnableDesired, this));
    //         }
    //     } else if (isServiceCodeUncondCallFwdTimer(mSc)) {
    //         Rlog.d(LOGTAG, "is UncondCFTimer");
    //
    //         String dialingNumber = mSia;
    //         int reason = scToCallForwardReason(mSc);
    //
    //         if (isInterrogate()) {
    //
    //             mPhone.getCallForwardingOption(reason,
    //                     obtainMessage(EVENT_QUERY_CFUT_COMPLETE, this));
    //         } else {
    //             int cfTimerAction;
    //             int startHour = 0;
    //             int startMinute = 0;
    //             int endHour = 0;
    //             int endMinute = 0;
    //             if (mSic != null) {
    //                 // sic contains string in the format HH:MM$HH:MM.String
    //                 // will be splitted by '$' that gives two halves.First half
    //                 // signifies starthour and startminute and second half
    //                 // signifies endhour and endminute.
    //                 String duration[] = mSic.split("\\$");
    //
    //                 if((duration[0] !=null) && (duration[1]!=null)) {
    //                     //splitting to get starthour and startminute
    //                     String start[] = duration[0].split("\\:");
    //                     if (start[0] != null) {
    //                         startHour = Integer.parseInt(start[0]);
    //                     }
    //                     if (start[1] != null) {
    //                         startMinute = Integer.parseInt(start[1]);
    //                     }
    //                     //splitting to get endhour and endminute
    //                     String end[] = duration[1].split("\\:");
    //                     if (end[0] !=null) {
    //                         endHour = Integer.parseInt(end[0]);
    //                     }
    //                     if (end[1] !=null) {
    //                         endMinute = Integer.parseInt(end[1]);
    //                     }
    //                 }
    //             }
    //             cfTimerAction = calForwardAction(dialingNumber);
    //             Rlog.d(LOGTAG, "is CFUT setCallForward");
    //             mPhone.setCallForwardingUncondTimerOption(startHour,
    //                     startMinute, endHour, endMinute, cfTimerAction,
    //                     reason, dialingNumber,
    //                     obtainMessage(EVENT_SET_CFF_TIMER_COMPLETE,this));
    //         }
    //     } else if (isServiceCodeCallBarring(mSc)) {
    //         // sia = password
    //         // sib = basic service group
    //         // service group is not supported
    //
    //         String password = mSia;
    //         String facility = scToBarringFacility(mSc);
    //
    //         if (isInterrogate()) {
    //             mPhone.getCallBarring(facility,
    //                     obtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
    //         } else if (isActivate() || isDeactivate()) {
    //             mPhone.setCallBarring(facility, isActivate(), password,
    //                     obtainMessage(EVENT_SET_COMPLETE, this));
    //         } else {
    //             throw new RuntimeException ("Invalid or Unsupported MMI Code");
    //         }
    //     } else if (mSc != null && mSc.equals(SC_CLIR)) {
    //         // NOTE: Since these supplementary services are accessed only
    //         //       via MMI codes, methods have not been added to ImsPhone.
    //         //       Only the UT interface handle is used.
    //         if (isActivate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface().updateCLIR(CommandsInterface.CLIR_INVOCATION,
    //                     obtainMessage(EVENT_SET_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for updateCLIR.");
    //             }
    //         } else if (isDeactivate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface().updateCLIR(CommandsInterface.CLIR_SUPPRESSION,
    //                     obtainMessage(EVENT_SET_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for updateCLIR.");
    //             }
    //         } else if (isInterrogate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface()
    //                     .queryCLIR(obtainMessage(EVENT_GET_CLIR_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for queryCLIR.");
    //             }
    //         } else {
    //             throw new RuntimeException ("Invalid or Unsupported MMI Code");
    //         }
    //     } else if (mSc != null && mSc.equals(SC_CLIP)) {
    //         // NOTE: Refer to the note above.
    //         if (isInterrogate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface()
    //                     .queryCLIP(obtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for queryCLIP.");
    //             }
    //         } else if (isActivate() || isDeactivate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface().updateCLIP(isActivate(),
    //                         obtainMessage(EVENT_SET_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for updateCLIP.");
    //             }
    //         } else {
    //             throw new RuntimeException ("Invalid or Unsupported MMI Code");
    //         }
    //     } else if (mSc != null && mSc.equals(SC_COLP)) {
    //         // NOTE: Refer to the note above.
    //         if (isInterrogate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface()
    //                     .queryCOLP(obtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for queryCOLP.");
    //             }
    //         } else if (isActivate() || isDeactivate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface().updateCOLP(isActivate(),
    //                          obtainMessage(EVENT_SET_COMPLETE, this));
    //              } catch (ImsException e) {
    //                  Rlog.d(LOGTAG, "Could not get UT handle for updateCOLP.");
    //              }
    //         } else {
    //             throw new RuntimeException ("Invalid or Unsupported MMI Code");
    //         }
    //     } else if (mSc != null && mSc.equals(SC_COLR)) {
    //         // NOTE: Refer to the note above.
    //         if (isActivate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface().updateCOLR(NUM_PRESENTATION_RESTRICTED,
    //                         obtainMessage(EVENT_SET_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for updateCOLR.");
    //             }
    //         } else if (isDeactivate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface().updateCOLR(NUM_PRESENTATION_ALLOWED,
    //                         obtainMessage(EVENT_SET_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for updateCOLR.");
    //             }
    //         } else if (isInterrogate()) {
    //             try {
    //                 mPhone.mCT.getUtInterface()
    //                     .queryCOLR(obtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
    //             } catch (ImsException e) {
    //                 Rlog.d(LOGTAG, "Could not get UT handle for queryCOLR.");
    //             }
    //         } else {
    //             throw new RuntimeException ("Invalid or Unsupported MMI Code");
    //         }
    //     } else if (mSc != null && (mSc.equals(SC_BS_MT))) {
    //         try {
    //             if (isInterrogate()) {
    //                 mPhone.mCT.getUtInterface()
    //                 .queryCallBarring(ImsUtInterface.CB_BS_MT,
    //                                   obtainMessage(EVENT_QUERY_ICB_COMPLETE,this));
    //             } else {
    //                 processIcbMmiCodeForUpdate();
    //             }
    //          // TODO: isRegister() case needs to be handled.
    //         } catch (ImsException e) {
    //             Rlog.d(LOGTAG, "Could not get UT handle for ICB.");
    //         }
    //     } else if (mSc != null && mSc.equals(SC_BAICa)) {
    //         int callAction =0;
    //         // TODO: Should we route through queryCallBarring() here?
    //         try {
    //             if (isInterrogate()) {
    //                 mPhone.mCT.getUtInterface()
    //                 .queryCallBarring(ImsUtInterface.CB_BIC_ACR,
    //                                   obtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE,this));
    //             } else {
    //                 if (isActivate()) {
    //                     callAction = CommandsInterface.CF_ACTION_ENABLE;
    //                 } else if (isDeactivate()) {
    //                     callAction = CommandsInterface.CF_ACTION_DISABLE;
    //                 }
    //                 mPhone.mCT.getUtInterface()
    //                         .updateCallBarring(ImsUtInterface.CB_BIC_ACR,
    //                         callAction,
    //                         obtainMessage(EVENT_SET_COMPLETE,this),
    //                         null);
    //             }
    //         } catch (ImsException e) {
    //             Rlog.d(LOGTAG, "Could not get UT handle for ICBa.");
    //         }
    //     } else if (mSc != null && mSc.equals(SC_WAIT)) {
    //         // sia = basic service group
    //         // service group is not supported
    //         if (isActivate() || isDeactivate()) {
    //             mPhone.setCallWaiting(isActivate(),
    //                     obtainMessage(EVENT_SET_COMPLETE, this));
    //         } else if (isInterrogate()) {
    //             mPhone.getCallWaiting(obtainMessage(EVENT_QUERY_COMPLETE, this));
    //         } else {
    //             throw new RuntimeException ("Invalid or Unsupported MMI Code");
    //         }
    //     } else if (mPoundString != null) {
    //         Rlog.d(LOGTAG, "Sending pound string '"
    //                + mDialingNumber + "' over CS pipe.");
    //         throw new CallStateException(ImsPhone.CS_FALLBACK);
    //     } else {
    //         throw new RuntimeException ("Invalid or Unsupported MMI Code");
    //     }
    // } catch (RuntimeException exc) {
    //     mState = State.FAILED;
    //     mMessage = mContext.getText(com.android.internal.R.string.mmiError);
    //     mPhone.onMMIDone(this);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::OnUssdFinished(
    /* [in] */ const String& ussdMessage,
    /* [in] */ Boolean isUssdRequest)
{
    // ==================before translated======================
    // if (mState == State.PENDING) {
    //     if (ussdMessage == null) {
    //         mMessage = mContext.getText(com.android.internal.R.string.mmiComplete);
    //     } else {
    //         mMessage = ussdMessage;
    //     }
    //     mIsUssdRequest = isUssdRequest;
    //     // If it's a request, leave it PENDING so that it's cancelable.
    //     if (!isUssdRequest) {
    //         mState = State.COMPLETE;
    //     }
    //
    //     mPhone.onMMIDone(this);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::OnUssdFinishedError()
{
    // ==================before translated======================
    // if (mState == State.PENDING) {
    //     mState = State.FAILED;
    //     mMessage = mContext.getText(com.android.internal.R.string.mmiError);
    //
    //     mPhone.onMMIDone(this);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::SendUssd(
    /* [in] */ const String& ussdMessage)
{
    // ==================before translated======================
    // // Treat this as a USSD string
    // mIsPendingUSSD = true;
    //
    // // Note that unlike most everything else, the USSD complete
    // // response does not complete this MMI code...we wait for
    // // an unsolicited USSD "Notify" or "Request".
    // // The matching up of this is done in ImsPhone.
    //
    // mPhone.sendUSSD(ussdMessage,
    //     obtainMessage(EVENT_USSD_COMPLETE, this));
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    //
    // switch (msg.what) {
    //     case EVENT_SET_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //
    //         onSetComplete(msg, ar);
    //         break;
    //
    //     case EVENT_SET_CFF_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //
    //         /*
    //         * msg.arg1 = 1 means to set unconditional voice call forwarding
    //         * msg.arg2 = 1 means to enable voice call forwarding
    //         */
    //         if ((ar.exception == null) && (msg.arg1 == 1)) {
    //             boolean cffEnabled = (msg.arg2 == 1);
    //             if (mIccRecords != null) {
    //                 mIccRecords.setVoiceCallForwardingFlag(1, cffEnabled, mDialingNumber);
    //             }
    //         }
    //
    //         onSetComplete(msg, ar);
    //         break;
    //
    //     case EVENT_SET_CFF_TIMER_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //         onSetComplete(msg, ar);
    //         break;
    //
    //     case EVENT_QUERY_CF_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //         onQueryCfComplete(ar);
    //         break;
    //
    //     case EVENT_QUERY_CFUT_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //         onQueryUncondCfTimerComplete(ar);
    //         break;
    //
    //     case EVENT_QUERY_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //         onQueryComplete(ar);
    //         break;
    //
    //     case EVENT_USSD_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //
    //         if (ar.exception != null) {
    //             mState = State.FAILED;
    //             mMessage = getErrorMessage(ar);
    //
    //             mPhone.onMMIDone(this);
    //         }
    //
    //         // Note that unlike most everything else, the USSD complete
    //         // response does not complete this MMI code...we wait for
    //         // an unsolicited USSD "Notify" or "Request".
    //         // The matching up of this is done in ImsPhone.
    //
    //         break;
    //
    //     case EVENT_USSD_CANCEL_COMPLETE:
    //         mPhone.onMMIDone(this);
    //         break;
    //
    //     case EVENT_SUPP_SVC_QUERY_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //         onSuppSvcQueryComplete(ar);
    //         break;
    //
    //     case EVENT_QUERY_ICB_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //         onIcbQueryComplete(ar);
    //         break;
    //
    //     case EVENT_GET_CLIR_COMPLETE:
    //         ar = (AsyncResult) (msg.obj);
    //         onQueryClirComplete(ar);
    //         break;
    //
    //     default:
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneMmiCode::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder("ImsPhoneMmiCode {");
    //
    // sb.append("State=" + getState());
    // if (mAction != null) sb.append(" action=" + mAction);
    // if (mSc != null) sb.append(" sc=" + mSc);
    // if (mSia != null) sb.append(" sia=" + mSia);
    // if (mSib != null) sb.append(" sib=" + mSib);
    // if (mSic != null) sb.append(" sic=" + mSic);
    // if (mPoundString != null) sb.append(" poundString=" + mPoundString);
    // if (mDialingNumber != null) sb.append(" dialingNumber=" + mDialingNumber);
    // if (mPwd != null) sb.append(" pwd=" + mPwd);
    // sb.append("}");
    // return sb.toString();
    assert(0);
    return NOERROR;
}

String ImsPhoneMmiCode::MakeEmptyNull(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // if (s != null && s.length() == 0) return null;
    //
    // return s;
    assert(0);
    return String("");
}

Boolean ImsPhoneMmiCode::IsEmptyOrNull(
    /* [in] */ ICharSequence* s)
{
    // ==================before translated======================
    // return s == null || (s.length() == 0);
    assert(0);
    return FALSE;
}

Int32 ImsPhoneMmiCode::ScToCallForwardReason(
    /* [in] */ const String& sc)
{
    // ==================before translated======================
    // if (sc == null) {
    //     throw new RuntimeException ("invalid call forward sc");
    // }
    //
    // if (sc.equals(SC_CF_All)) {
    //    return CommandsInterface.CF_REASON_ALL;
    // } else if ((sc.equals(SC_CFU)) || (sc.equals(SC_CFUT))) {
    //     return CommandsInterface.CF_REASON_UNCONDITIONAL;
    // } else if (sc.equals(SC_CFB)) {
    //     return CommandsInterface.CF_REASON_BUSY;
    // } else if (sc.equals(SC_CFNR)) {
    //     return CommandsInterface.CF_REASON_NOT_REACHABLE;
    // } else if (sc.equals(SC_CFNRy)) {
    //     return CommandsInterface.CF_REASON_NO_REPLY;
    // } else if (sc.equals(SC_CF_All_Conditional)) {
    //    return CommandsInterface.CF_REASON_ALL_CONDITIONAL;
    // } else {
    //     throw new RuntimeException ("invalid call forward sc");
    // }
    assert(0);
    return 0;
}

Int32 ImsPhoneMmiCode::SiToServiceClass(
    /* [in] */ const String& si)
{
    // ==================before translated======================
    // if (si == null || si.length() == 0) {
    //         return  SERVICE_CLASS_NONE;
    // } else {
    //     // NumberFormatException should cause MMI fail
    //     int serviceCode = Integer.parseInt(si, 10);
    //
    //     switch (serviceCode) {
    //         case 10: return SERVICE_CLASS_SMS + SERVICE_CLASS_FAX  + SERVICE_CLASS_VOICE;
    //         case 11: return SERVICE_CLASS_VOICE;
    //         case 12: return SERVICE_CLASS_SMS + SERVICE_CLASS_FAX;
    //         case 13: return SERVICE_CLASS_FAX;
    //
    //         case 16: return SERVICE_CLASS_SMS;
    //
    //         case 19: return SERVICE_CLASS_FAX + SERVICE_CLASS_VOICE;
    //
    //         case 20: return SERVICE_CLASS_DATA_ASYNC + SERVICE_CLASS_DATA_SYNC;
    //
    //         case 21: return SERVICE_CLASS_PAD + SERVICE_CLASS_DATA_ASYNC;
    //         case 22: return SERVICE_CLASS_PACKET + SERVICE_CLASS_DATA_SYNC;
    //         case 24: return SERVICE_CLASS_DATA_SYNC;
    //         case 25: return SERVICE_CLASS_DATA_ASYNC;
    //         case 26: return SERVICE_CLASS_DATA_SYNC + SERVICE_CLASS_VOICE;
    //         case 99: return SERVICE_CLASS_PACKET;
    //
    //         default:
    //             throw new RuntimeException("unsupported MMI service code " + si);
    //     }
    // }
    assert(0);
    return 0;
}

Int32 ImsPhoneMmiCode::SiToTime(
    /* [in] */ const String& si)
{
    // ==================before translated======================
    // if (si == null || si.length() == 0) {
    //     return 0;
    // } else {
    //     // NumberFormatException should cause MMI fail
    //     return Integer.parseInt(si, 10);
    // }
    assert(0);
    return 0;
}

Boolean ImsPhoneMmiCode::IsTwoDigitShortCode(
    /* [in] */ IContext* context,
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "isTwoDigitShortCode");
    //
    // if (dialString == null || dialString.length() > 2) return false;
    //
    // if (sTwoDigitNumberPattern == null) {
    //     sTwoDigitNumberPattern = context.getResources().getStringArray(
    //             com.android.internal.R.array.config_twoDigitNumberPattern);
    // }
    //
    // for (String dialnumber : sTwoDigitNumberPattern) {
    //     Rlog.d(LOGTAG, "Two Digit Number Pattern " + dialnumber);
    //     if (dialString.equals(dialnumber)) {
    //         Rlog.d(LOGTAG, "Two Digit Number Pattern -true");
    //         return true;
    //     }
    // }
    // Rlog.d(LOGTAG, "Two Digit Number Pattern -false");
    // return false;
    assert(0);
    return FALSE;
}

Boolean ImsPhoneMmiCode::IsShortCode(
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhone* phone)
{
    // ==================before translated======================
    // // Refer to TS 22.030 Figure 3.5.3.2:
    // if (dialString == null) {
    //     return false;
    // }
    //
    // // Illegal dial string characters will give a ZERO length.
    // // At this point we do not want to crash as any application with
    // // call privileges may send a non dial string.
    // // It return false as when the dialString is equal to NULL.
    // if (dialString.length() == 0) {
    //     return false;
    // }
    //
    // if (PhoneNumberUtils.isLocalEmergencyNumber(phone.getContext(), dialString)) {
    //     return false;
    // } else {
    //     return isShortCodeUSSD(dialString, phone);
    // }
    assert(0);
    return FALSE;
}

Boolean ImsPhoneMmiCode::IsShortCodeUSSD(
    /* [in] */ const String& dialString,
    /* [in] */ IImsPhone* phone)
{
    // ==================before translated======================
    // if (dialString != null && dialString.length() <= MAX_LENGTH_SHORT_CODE) {
    //     if (phone.isInCall()) {
    //         return true;
    //     }
    //
    //     if (dialString.length() != MAX_LENGTH_SHORT_CODE ||
    //             dialString.charAt(0) != '1') {
    //         return true;
    //     }
    // }
    // return false;
    assert(0);
    return FALSE;
}

void ImsPhoneMmiCode::ProcessIcbMmiCodeForUpdate()
{
    // ==================before translated======================
    // String dialingNumber = mSia;
    // String[] icbNum = null;
    // int callAction;
    // if (dialingNumber != null) {
    //     icbNum = dialingNumber.split("\\$");
    // }
    // callAction = callBarrAction(dialingNumber);
    //
    // try {
    //     mPhone.mCT.getUtInterface()
    //     .updateCallBarring(ImsUtInterface.CB_BS_MT,
    //                        callAction,
    //                        obtainMessage(EVENT_SET_COMPLETE,this),
    //                        icbNum);
    // } catch (ImsException e) {
    //     Rlog.d(LOGTAG, "Could not get UT handle for updating ICB.");
    // }
    assert(0);
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::GetErrorMessage(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // return mContext.getText(com.android.internal.R.string.mmiError);
    assert(0);
    AutoPtr<ICharSequence> empty;
    return empty;
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::GetScString()
{
    // ==================before translated======================
    // if (mSc != null) {
    //     if (isServiceCodeCallBarring(mSc)) {
    //         return mContext.getText(com.android.internal.R.string.BaMmi);
    //     } else if (isServiceCodeCallForwarding(mSc)) {
    //         return mContext.getText(com.android.internal.R.string.CfMmi);
    //     } else if (mSc.equals(SC_PWD)) {
    //         return mContext.getText(com.android.internal.R.string.PwdMmi);
    //     } else if (mSc.equals(SC_WAIT)) {
    //         return mContext.getText(com.android.internal.R.string.CwMmi);
    //     } else if (mSc.equals(SC_CLIP)) {
    //         return mContext.getText(com.android.internal.R.string.ClipMmi);
    //     } else if (mSc.equals(SC_CLIR)) {
    //         return mContext.getText(com.android.internal.R.string.ClirMmi);
    //     } else if (mSc.equals(SC_COLP)) {
    //         return mContext.getText(com.android.internal.R.string.ColpMmi);
    //     } else if (mSc.equals(SC_COLR)) {
    //         return mContext.getText(com.android.internal.R.string.ColrMmi);
    //     } else if (mSc.equals(SC_CFUT)) {
    //         return CfuTimer;
    //     } else if (mSc.equals(SC_BS_MT)) {
    //         return IcbDnMmi;
    //     } else if (mSc.equals(SC_BAICa)) {
    //         return IcbAnonymousMmi;
    //     }
    // }
    //
    // return "";
    assert(0);
    AutoPtr<ICharSequence> empty;
    return empty;
}

void ImsPhoneMmiCode::OnSetComplete(
    /* [in] */ IMessage* msg,
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    //
    // if (ar.exception != null) {
    //     mState = State.FAILED;
    //
    //     if (ar.exception instanceof CommandException) {
    //         CommandException.Error err = ((CommandException)(ar.exception)).getCommandError();
    //         if (err == CommandException.Error.PASSWORD_INCORRECT) {
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.passwordIncorrect));
    //         } else {
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.mmiError));
    //         }
    //     } else {
    //         ImsException error = (ImsException) ar.exception;
    //         if (error.getMessage() != null) {
    //             sb.append(error.getMessage());
    //         } else {
    //             sb.append(getErrorMessage(ar));
    //         }
    //     }
    // } else if (isActivate()) {
    //     mState = State.COMPLETE;
    //     if (mIsCallFwdReg) {
    //         sb.append(mContext.getText(
    //                 com.android.internal.R.string.serviceRegistered));
    //     } else {
    //         sb.append(mContext.getText(
    //                 com.android.internal.R.string.serviceEnabled));
    //     }
    // } else if (isDeactivate()) {
    //     mState = State.COMPLETE;
    //     sb.append(mContext.getText(
    //             com.android.internal.R.string.serviceDisabled));
    // } else if (isRegister()) {
    //     mState = State.COMPLETE;
    //     sb.append(mContext.getText(
    //             com.android.internal.R.string.serviceRegistered));
    // } else if (isErasure()) {
    //     mState = State.COMPLETE;
    //     sb.append(mContext.getText(
    //             com.android.internal.R.string.serviceErased));
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

AutoPtr<ICharSequence> ImsPhoneMmiCode::ServiceClassToCFString(
    /* [in] */ Int32 serviceClass)
{
    // ==================before translated======================
    // switch (serviceClass) {
    //     case SERVICE_CLASS_VOICE:
    //         return mContext.getText(com.android.internal.R.string.serviceClassVoice);
    //     case SERVICE_CLASS_DATA:
    //         return mContext.getText(com.android.internal.R.string.serviceClassData);
    //     case SERVICE_CLASS_FAX:
    //         return mContext.getText(com.android.internal.R.string.serviceClassFAX);
    //     case SERVICE_CLASS_SMS:
    //         return mContext.getText(com.android.internal.R.string.serviceClassSMS);
    //     case SERVICE_CLASS_DATA_SYNC:
    //         return mContext.getText(com.android.internal.R.string.serviceClassDataSync);
    //     case SERVICE_CLASS_DATA_ASYNC:
    //         return mContext.getText(com.android.internal.R.string.serviceClassDataAsync);
    //     case SERVICE_CLASS_PACKET:
    //         return mContext.getText(com.android.internal.R.string.serviceClassPacket);
    //     case SERVICE_CLASS_PAD:
    //         return mContext.getText(com.android.internal.R.string.serviceClassPAD);
    //     default:
    //         return null;
    // }
    assert(0);
    AutoPtr<ICharSequence> empty;
    return empty;
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::MakeCFQueryResultMessage(
    /* [in] */ ICallForwardInfo* info,
    /* [in] */ Int32 serviceClassMask)
{
    // ==================before translated======================
    // CharSequence template;
    // String sources[] = {"{0}", "{1}", "{2}"};
    // CharSequence destinations[] = new CharSequence[3];
    // boolean needTimeTemplate;
    //
    // // CF_REASON_NO_REPLY also has a time value associated with
    // // it. All others don't.
    //
    // needTimeTemplate =
    //     (info.reason == CommandsInterface.CF_REASON_NO_REPLY);
    //
    // if (info.status == 1) {
    //     if (needTimeTemplate) {
    //         template = mContext.getText(
    //                 com.android.internal.R.string.cfTemplateForwardedTime);
    //     } else {
    //         template = mContext.getText(
    //                 com.android.internal.R.string.cfTemplateForwarded);
    //     }
    // } else if (info.status == 0 && isEmptyOrNull(info.number)) {
    //     template = mContext.getText(
    //                 com.android.internal.R.string.cfTemplateNotForwarded);
    // } else { /* (info.status == 0) && !isEmptyOrNull(info.number) */
    //     // A call forward record that is not active but contains
    //     // a phone number is considered "registered"
    //
    //     if (needTimeTemplate) {
    //         template = mContext.getText(
    //                 com.android.internal.R.string.cfTemplateRegisteredTime);
    //     } else {
    //         template = mContext.getText(
    //                 com.android.internal.R.string.cfTemplateRegistered);
    //     }
    // }
    //
    // // In the template (from strings.xmls)
    // //         {0} is one of "bearerServiceCode*"
    // //        {1} is dialing number
    // //      {2} is time in seconds
    //
    // destinations[0] = serviceClassToCFString(info.serviceClass & serviceClassMask);
    // destinations[1] = PhoneNumberUtils.stringFromStringAndTOA(info.number, info.toa);
    // destinations[2] = Integer.toString(info.timeSeconds);
    //
    // if (info.reason == CommandsInterface.CF_REASON_UNCONDITIONAL &&
    //         (info.serviceClass & serviceClassMask)
    //                 == CommandsInterface.SERVICE_CLASS_VOICE) {
    //     boolean cffEnabled = (info.status == 1);
    //     if (mIccRecords != null) {
    //         mIccRecords.setVoiceCallForwardingFlag(1, cffEnabled, info.number);
    //     }
    // }
    //
    // return TextUtils.replace(template, sources, destinations);
    assert(0);
    AutoPtr<ICharSequence> empty;
    return empty;
}

void ImsPhoneMmiCode::OnQueryCfComplete(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    //
    // if (ar.exception != null) {
    //     mState = State.FAILED;
    //
    //     if (ar.exception instanceof ImsException) {
    //         ImsException error = (ImsException) ar.exception;
    //         if (error.getMessage() != null) {
    //             sb.append(error.getMessage());
    //         } else {
    //             sb.append(getErrorMessage(ar));
    //         }
    //     }
    //     else {
    //         sb.append(getErrorMessage(ar));
    //     }
    // } else {
    //     CallForwardInfo infos[];
    //
    //     infos = (CallForwardInfo[]) ar.result;
    //
    //     if (infos.length == 0) {
    //         // Assume the default is not active
    //         sb.append(mContext.getText(com.android.internal.R.string.serviceDisabled));
    //
    //         // Set unconditional CFF in SIM to false
    //         if (mIccRecords != null) {
    //             mIccRecords.setVoiceCallForwardingFlag(1, false, null);
    //         }
    //     } else {
    //
    //         SpannableStringBuilder tb = new SpannableStringBuilder();
    //
    //         // Each bit in the service class gets its own result line
    //         // The service classes may be split up over multiple
    //         // CallForwardInfos. So, for each service class, find out
    //         // which CallForwardInfo represents it and then build
    //         // the response text based on that
    //
    //         for (int serviceClassMask = 1
    //                     ; serviceClassMask <= SERVICE_CLASS_MAX
    //                     ; serviceClassMask <<= 1
    //         ) {
    //             for (int i = 0, s = infos.length; i < s ; i++) {
    //                 if ((serviceClassMask & infos[i].serviceClass) != 0) {
    //                     tb.append(makeCFQueryResultMessage(infos[i],
    //                                     serviceClassMask));
    //                     tb.append("\n");
    //                 }
    //             }
    //         }
    //         sb.append(tb);
    //     }
    //
    //     mState = State.COMPLETE;
    // }
    //
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

void ImsPhoneMmiCode::OnQueryUncondCfTimerComplete(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    //
    // if (ar.exception != null) {
    //     mState = State.FAILED;
    //     sb.append(getErrorMessage(ar));
    // } else {
    //     CallForwardInfo infos[];
    //     infos = (CallForwardInfo[]) ar.result;
    //     if (infos.length == 0) {
    //         // Assume the default is not active
    //         Rlog.d(LOGTAG, "In infos.length == 0");
    //         sb.append(mContext.getText(com.android.internal.R.string.serviceDisabled));
    //     } else {
    //         SpannableStringBuilder tb = new SpannableStringBuilder();
    //         for (int serviceClassMask = 1
    //                 ; serviceClassMask <= SERVICE_CLASS_MAX
    //                 ; serviceClassMask <<= 1) {
    //             for (int i = 0, s = infos.length; i < s ; i++) {
    //                 if (( serviceClassMask & infos[i].serviceClass) ==
    //                         CommandsInterface.SERVICE_CLASS_VOICE &&
    //                         infos[i].status != 0) {
    //                     sb.append("StartTime: " + infos[i].startHour + ":" +
    //                             infos[i].startMinute + "\n");
    //                     sb.append("EndTime: " + infos[i].endHour + ":" +
    //                             infos[i].endMinute + "\n");
    //                     sb.append("Service:" + mContext.getText(
    //                             com.android.internal.R.string.serviceClassVoice));
    //                 } else if ((serviceClassMask & infos[i].serviceClass) != 0) {
    //                     tb.append(makeCFTQueryResultMessage(infos[i],
    //                             serviceClassMask));
    //                     tb.append("\n");
    //                 }
    //             }
    //         }
    //         sb.append(tb);
    //     }
    //   mState = State.COMPLETE;
    // }
    //
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::MakeCFTQueryResultMessage(
    /* [in] */ ICallForwardInfo* info,
    /* [in] */ Int32 serviceClassMask)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "makeCFTQueryResultMessage: ");
    // CharSequence template;
    // String sources[] = {"{0}", "{1}", "{2}"};
    // CharSequence destinations[] = new CharSequence[3];
    // boolean needTimeTemplate;
    //
    // if (info.status == 0 && isEmptyOrNull(info.number)) {
    //     template = mContext.getText(
    //                 com.android.internal.R.string.cfTemplateNotForwarded);
    // } else {
    //     template = mContext.getText(
    //                 com.android.internal.R.string.cfTemplateRegistered);
    // }
    // destinations[0] = serviceClassToCFString(info.serviceClass & serviceClassMask);
    // destinations[1] = PhoneNumberUtils.stringFromStringAndTOA(info.number, info.toa);
    // destinations[2] = Integer.toString(info.timeSeconds);
    //
    // return TextUtils.replace(template, sources, destinations);
    assert(0);
    AutoPtr<ICharSequence> empty;
    return empty;
}

void ImsPhoneMmiCode::OnSuppSvcQueryComplete(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    //
    // if (ar.exception != null) {
    //     mState = State.FAILED;
    //
    //     if (ar.exception instanceof ImsException) {
    //         ImsException error = (ImsException) ar.exception;
    //         if (error.getMessage() != null) {
    //             sb.append(error.getMessage());
    //         } else {
    //             sb.append(getErrorMessage(ar));
    //         }
    //     } else {
    //         sb.append(getErrorMessage(ar));
    //     }
    // } else {
    //     mState = State.FAILED;
    //     ImsSsInfo ssInfo = null;
    //     if (ar.result instanceof Bundle) {
    //         Rlog.d(LOGTAG, "Received CLIP/COLP/COLR Response.");
    //         // Response for CLIP, COLP and COLR queries.
    //         Bundle ssInfoResp = (Bundle) ar.result;
    //         ssInfo = (ImsSsInfo) ssInfoResp.getParcelable(UT_BUNDLE_KEY_SSINFO);
    //         if (ssInfo != null) {
    //             Rlog.d(LOGTAG, "ImsSsInfo mStatus = " + ssInfo.mStatus);
    //             if (ssInfo.mStatus == ImsSsInfo.DISABLED) {
    //                 sb.append(mContext.getText(com.android.internal.R.string.serviceDisabled));
    //                 mState = State.COMPLETE;
    //             } else if (ssInfo.mStatus == ImsSsInfo.ENABLED) {
    //                 sb.append(mContext.getText(com.android.internal.R.string.serviceEnabled));
    //                 mState = State.COMPLETE;
    //             } else {
    //                 sb.append(mContext.getText(com.android.internal.R.string.mmiError));
    //             }
    //         } else {
    //             sb.append(mContext.getText(com.android.internal.R.string.mmiError));
    //         }
    //
    //     } else {
    //         Rlog.d(LOGTAG, "Received Call Barring Response.");
    //         // Response for Call Barring queries.
    //         ImsSsInfo[] arr = (ImsSsInfo[])ar.result;
    //         // Check if ImsPhone has received call barring
    //         // enabled for service class voice.
    //         for (int i = 0, s = arr.length; i < s ; i++) {
    //             if(arr[i].mStatus == 1) {
    //                 sb.append(mContext.getText(com.android.internal
    //                         .R.string.serviceEnabled));
    //                 mState = State.COMPLETE;
    //             } else {
    //                 sb.append(mContext.getText(com.android.internal
    //                         .R.string.serviceDisabled));
    //                 mState = State.COMPLETE;
    //             }
    //         }
    //     }
    // }
    //
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

void ImsPhoneMmiCode::OnIcbQueryComplete(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "onIcbQueryComplete ");
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    //
    // if (ar.exception != null) {
    //     mState = State.FAILED;
    //
    //     if (ar.exception instanceof ImsException) {
    //         ImsException error = (ImsException) ar.exception;
    //         if (error.getMessage() != null) {
    //             sb.append(error.getMessage());
    //         } else {
    //             sb.append(getErrorMessage(ar));
    //         }
    //     } else {
    //         sb.append(getErrorMessage(ar));
    //     }
    // } else {
    //     mState = State.FAILED;
    //     ImsSsInfo[] infos = (ImsSsInfo[])ar.result;
    //     if (infos.length == 0) {
    //         sb.append(mContext.getText(com.android.internal.R.string.serviceDisabled));
    //         mState = State.COMPLETE;
    //     } else {
    //         for (int i = 0, s = infos.length; i < s ; i++) {
    //             sb.append("Num: " + infos[i].mIcbNum + " status: "
    //                     + infos[i].mStatus + "\n");
    //         }
    //         mState = State.COMPLETE;
    //     }
    // }
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

void ImsPhoneMmiCode::OnQueryClirComplete(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    // mState = State.FAILED;
    //
    // if (ar.exception != null) {
    //
    //     if (ar.exception instanceof ImsException) {
    //         ImsException error = (ImsException) ar.exception;
    //         if (error.getMessage() != null) {
    //             sb.append(error.getMessage());
    //         } else {
    //             sb.append(getErrorMessage(ar));
    //         }
    //     }
    // } else {
    //     Bundle ssInfo = (Bundle) ar.result;
    //     int[] clirInfo = ssInfo.getIntArray(UT_BUNDLE_KEY_CLIR);
    //     // clirInfo[0] = The 'n' parameter from TS 27.007 7.7
    //     // clirInfo[1] = The 'm' parameter from TS 27.007 7.7
    //     Rlog.d(LOGTAG, "CLIR param n=" + clirInfo[0]
    //             + " m=" + clirInfo[1]);
    //
    //     // 'm' parameter.
    //     switch (clirInfo[1]) {
    //         case CLIR_NOT_PROVISIONED:
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.serviceNotProvisioned));
    //             mState = State.COMPLETE;
    //             break;
    //         case CLIR_PROVISIONED_PERMANENT:
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.CLIRPermanent));
    //             mState = State.COMPLETE;
    //             break;
    //         case CLIR_PRESENTATION_RESTRICTED_TEMPORARY:
    //             // 'n' parameter.
    //             switch (clirInfo[0]) {
    //                 case CLIR_DEFAULT:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.CLIRDefaultOnNextCallOn));
    //                     mState = State.COMPLETE;
    //                     break;
    //                 case CLIR_INVOCATION:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.CLIRDefaultOnNextCallOn));
    //                     mState = State.COMPLETE;
    //                     break;
    //                 case CLIR_SUPPRESSION:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.CLIRDefaultOnNextCallOff));
    //                     mState = State.COMPLETE;
    //                     break;
    //                 default:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.mmiError));
    //                     mState = State.FAILED;
    //             }
    //             break;
    //         case CLIR_PRESENTATION_ALLOWED_TEMPORARY:
    //             // 'n' parameter.
    //             switch (clirInfo[0]) {
    //                 case CLIR_DEFAULT:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.CLIRDefaultOffNextCallOff));
    //                     mState = State.COMPLETE;
    //                     break;
    //                 case CLIR_INVOCATION:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.CLIRDefaultOffNextCallOn));
    //                     mState = State.COMPLETE;
    //                     break;
    //                 case CLIR_SUPPRESSION:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.CLIRDefaultOffNextCallOff));
    //                     mState = State.COMPLETE;
    //                     break;
    //                 default:
    //                     sb.append(mContext.getText(
    //                             com.android.internal.R.string.mmiError));
    //                     mState = State.FAILED;
    //             }
    //             break;
    //         default:
    //             sb.append(mContext.getText(
    //                     com.android.internal.R.string.mmiError));
    //             mState = State.FAILED;
    //     }
    // }
    //
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

void ImsPhoneMmiCode::OnQueryComplete(
    /* [in] */ AsyncResult* ar)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(getScString());
    // sb.append("\n");
    //
    // if (ar.exception != null) {
    //     mState = State.FAILED;
    //
    //     if (ar.exception instanceof ImsException) {
    //         ImsException error = (ImsException) ar.exception;
    //         if (error.getMessage() != null) {
    //             sb.append(error.getMessage());
    //         } else {
    //             sb.append(getErrorMessage(ar));
    //         }
    //     } else {
    //         sb.append(getErrorMessage(ar));
    //     }
    //
    // } else {
    //     int[] ints = (int[])ar.result;
    //
    //     if (ints.length != 0) {
    //         if (ints[0] == 0) {
    //             sb.append(mContext.getText(com.android.internal.R.string.serviceDisabled));
    //         } else if (mSc.equals(SC_WAIT)) {
    //             // Call Waiting includes additional data in the response.
    //             sb.append(createQueryCallWaitingResultMessage(ints[1]));
    //         } else if (ints[0] == 1) {
    //             // for all other services, treat it as a boolean
    //             sb.append(mContext.getText(com.android.internal.R.string.serviceEnabled));
    //         } else {
    //             sb.append(mContext.getText(com.android.internal.R.string.mmiError));
    //         }
    //     } else {
    //         sb.append(mContext.getText(com.android.internal.R.string.mmiError));
    //     }
    //     mState = State.COMPLETE;
    // }
    //
    // mMessage = sb;
    // mPhone.onMMIDone(this);
    assert(0);
}

AutoPtr<ICharSequence> ImsPhoneMmiCode::CreateQueryCallWaitingResultMessage(
    /* [in] */ Int32 serviceClass)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder(
    //         mContext.getText(com.android.internal.R.string.serviceEnabledFor));
    //
    // for (int classMask = 1
    //             ; classMask <= SERVICE_CLASS_MAX
    //             ; classMask <<= 1
    // ) {
    //     if ((classMask & serviceClass) != 0) {
    //         sb.append("\n");
    //         sb.append(serviceClassToCFString(classMask & serviceClass));
    //     }
    // }
    // return sb;
    assert(0);
    AutoPtr<ICharSequence> empty;
    return empty;
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
