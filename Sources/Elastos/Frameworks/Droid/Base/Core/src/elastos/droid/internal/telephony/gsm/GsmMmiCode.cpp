/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.gsm;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::I*;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;

using Elastos::Droid::Os::I*;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;

using static com::Android::Internal::Telephony::CommandsInterface::I*;
using Elastos::Droid::Internal::Telephony::Gsm::ISsData;

using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IMatcher;

/**
 * The motto for this file is:
 *
 * "NOTE:    By using the # as a separator, most cases are expected to be unambiguous."
 *   -- TS 22.030 6.5.2
 *
 * {@hide}
 *
 */
public class GsmMmiCode extends Handler implements MmiCode {
    static const String LOG_TAG = "GsmMmiCode";

    //***** Constants

    // Max Size of the Short Code (aka Short String from TS 22.030 6.5.2)
    static const Int32 MAX_LENGTH_SHORT_CODE = 2;

    // TS 22.030 6.5.2 Every Short String USSD command will end with #-key
    // (known as #-String)
    static const Char32 END_OF_USSD_COMMAND = '#';

    // From TS 22.030 6.5.2
    static const String ACTION_ACTIVATE = "*";
    static const String ACTION_DEACTIVATE = "#";
    static const String ACTION_INTERROGATE = "*#";
    static const String ACTION_REGISTER = "**";
    static const String ACTION_ERASURE = "##";

    // Supp Service codes from TS 22.030 Annex B

    //Called line presentation
    static const String SC_CLIP    = "30";
    static const String SC_CLIR    = "31";

    // Call Forwarding
    static const String SC_CFU     = "21";
    static const String SC_CFB     = "67";
    static const String SC_CFNRy   = "61";
    static const String SC_CFNR    = "62";

    static const String SC_CF_All = "002";
    static const String SC_CF_All_Conditional = "004";

    // Call Waiting
    static const String SC_WAIT     = "43";

    // Call Barring
    static const String SC_BAOC         = "33";
    static const String SC_BAOIC        = "331";
    static const String SC_BAOICxH      = "332";
    static const String SC_BAIC         = "35";
    static const String SC_BAICr        = "351";

    static const String SC_BA_ALL       = "330";
    static const String SC_BA_MO        = "333";
    static const String SC_BA_MT        = "353";

    // Supp Service Password registration
    static const String SC_PWD          = "03";

    // PIN/PIN2/PUK/PUK2
    static const String SC_PIN          = "04";
    static const String SC_PIN2         = "042";
    static const String SC_PUK          = "05";
    static const String SC_PUK2         = "052";

    //***** Event Constants

    static const Int32 EVENT_SET_COMPLETE         = 1;
    static const Int32 EVENT_GET_CLIR_COMPLETE    = 2;
    static const Int32 EVENT_QUERY_CF_COMPLETE    = 3;
    static const Int32 EVENT_USSD_COMPLETE        = 4;
    static const Int32 EVENT_QUERY_COMPLETE       = 5;
    static const Int32 EVENT_SET_CFF_COMPLETE     = 6;
    static const Int32 EVENT_USSD_CANCEL_COMPLETE = 7;

    //***** Instance Variables

    GSMPhone mPhone;
    Context mContext;
    UiccCardApplication mUiccApplication;
    IccRecords mIccRecords;

    String mAction;              // One of ACTION_*
    String mSc;                  // Service Code
    String mSia, mSib, mSic;       // Service Info a,b,c
    String mPoundString;         // Entire MMI string up to and including #
    String mDialingNumber;
    String mPwd;                 // For password registration

    /** Set to TRUE in processCode, not at newFromDialString time */
    private Boolean mIsPendingUSSD;

    private Boolean mIsUssdRequest;

    private Boolean mIsCallFwdReg;
    State mState = State.PENDING;
    CharSequence mMessage;
    private Boolean mIsSsInfo = FALSE;


    //***** Class Variables


    // See TS 22.030 6.5.2 "Structure of the MMI"

    static Pattern sPatternSuppService = Pattern->Compile(
        "((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)");
/*       1  2                    3          4  5       6   7         8    9     10  11             12

         1 = Full string up to and including #
         2 = Action (activation/interrogation/registration/erasure)
         3 = service code
         5 = SIA
         7 = SIB
         9 = SIC
         10 = dialing number
*/

    static const Int32 MATCH_GROUP_POUND_STRING = 1;

    static const Int32 MATCH_GROUP_ACTION = 2;
                        //(activation/interrogation/registration/erasure)

    static const Int32 MATCH_GROUP_SERVICE_CODE = 3;
    static const Int32 MATCH_GROUP_SIA = 5;
    static const Int32 MATCH_GROUP_SIB = 7;
    static const Int32 MATCH_GROUP_SIC = 9;
    static const Int32 MATCH_GROUP_PWD_CONFIRM = 11;
    static const Int32 MATCH_GROUP_DIALING_NUMBER = 12;
    static private String[] sTwoDigitNumberPattern;

    //***** Public Class methods

    /**
     * Some dial strings in GSM are defined to do non-call setup
     * things, such as modify or query supplementary service Settings (eg, call
     * forwarding). These are generally referred to as "MMI codes".
     * We look to see if the dial string contains a valid MMI Code (potentially
     * with a dial string at the end as well) and return info here.
     *
     * If the dial string contains no MMI code, we return an instance with
     * only "dialingNumber" set
     *
     * Please see flow chart in TS 22.030 6.5.3.2
     */

    static GsmMmiCode
    NewFromDialString(String dialString, GSMPhone phone, UiccCardApplication app) {
        Matcher m;
        GsmMmiCode ret = NULL;

        m = sPatternSuppService->Matcher(dialString);

        // Is this formatted like a standard supplementary service code?
        If (m->Matches()) {
            ret = new GsmMmiCode(phone, app);
            ret.mPoundString = MakeEmptyNull(m->Group(MATCH_GROUP_POUND_STRING));
            ret.mAction = MakeEmptyNull(m->Group(MATCH_GROUP_ACTION));
            ret.mSc = MakeEmptyNull(m->Group(MATCH_GROUP_SERVICE_CODE));
            ret.mSia = MakeEmptyNull(m->Group(MATCH_GROUP_SIA));
            ret.mSib = MakeEmptyNull(m->Group(MATCH_GROUP_SIB));
            ret.mSic = MakeEmptyNull(m->Group(MATCH_GROUP_SIC));
            ret.mPwd = MakeEmptyNull(m->Group(MATCH_GROUP_PWD_CONFIRM));
            ret.mDialingNumber = MakeEmptyNull(m->Group(MATCH_GROUP_DIALING_NUMBER));
            // According to TS 22.030 6.5.2 "Structure of the MMI",
            // the dialing number should not ending with #.
            // The dialing number ending # is treated as unique USSD,
            // eg, *400#16 digit number# to recharge the prepaid card
            // in India Operator(Mumbai MTNL)
            If(ret.mDialingNumber != NULL &&
                    ret.mDialingNumber->EndsWith("#") &&
                    dialString->EndsWith("#")){
                ret = new GsmMmiCode(phone, app);
                ret.mPoundString = dialString;
            }
        } else If (dialString->EndsWith("#")) {
            // TS 22.030 sec 6.5.3.2
            // "Entry of any characters defined in the 3GPP TS 23.038 [8] Default Alphabet
            // (up to the maximum defined in 3GPP TS 24.080 [10]), followed by #SEND".

            ret = new GsmMmiCode(phone, app);
            ret.mPoundString = dialString;
        } else If (IsTwoDigitShortCode(phone->GetContext(), dialString)) {
            //Is a country-specific exception to short codes as defined in TS 22.030, 6.5.3.2
            ret = NULL;
        } else If (IsShortCode(dialString, phone)) {
            // this may be a short code, as defined in TS 22.030, 6.5.3.2
            ret = new GsmMmiCode(phone, app);
            ret.mDialingNumber = dialString;
        }

        return ret;
    }

    static GsmMmiCode
    NewNetworkInitiatedUssd (String ussdMessage,
                                Boolean isUssdRequest, GSMPhone phone, UiccCardApplication app) {
        GsmMmiCode ret;

        ret = new GsmMmiCode(phone, app);

        ret.mMessage = ussdMessage;
        ret.mIsUssdRequest = isUssdRequest;

        // If it's a request, set to PENDING so that it's cancelable.
        If (isUssdRequest) {
            ret.mIsPendingUSSD = TRUE;
            ret.mState = State.PENDING;
        } else {
            ret.mState = State.COMPLETE;
        }

        return ret;
    }

    static GsmMmiCode NewFromUssdUserInput(String ussdMessge,
                                           GSMPhone phone,
                                           UiccCardApplication app) {
        GsmMmiCode ret = new GsmMmiCode(phone, app);

        ret.mMessage = ussdMessge;
        ret.mState = State.PENDING;
        ret.mIsPendingUSSD = TRUE;

        return ret;
    }

    /** Process SS Data */
    void
    ProcessSsData(AsyncResult data) {
        Rlog->D(LOG_TAG, "In processSsData");

        mIsSsInfo = TRUE;
        try {
            SsData ssData = (SsData)data.result;
            ParseSsData(ssData);
        } Catch (ClassCastException ex) {
            Rlog->E(LOG_TAG, "Class Cast Exception in parsing SS Data : " + ex);
        } Catch (NullPointerException ex) {
            Rlog->E(LOG_TAG, "Null Pointer Exception in parsing SS Data : " + ex);
        }
    }

    void ParseSsData(SsData ssData) {
        CommandException ex;

        ex = CommandException->FromRilErrno(ssData.result);
        mSc = GetScStringFromScType(ssData.serviceType);
        mAction = GetActionStringFromReqType(ssData.requestType);
        Rlog->D(LOG_TAG, "parseSsData msc = " + mSc + ", action = " + mAction + ", ex = " + ex);

        Switch (ssData.requestType) {
            case SS_ACTIVATION:
            case SS_DEACTIVATION:
            case SS_REGISTRATION:
            case SS_ERASURE:
                If ((ssData.result == RILConstants.SUCCESS) &&
                      ssData.serviceType->IsTypeUnConditional()) {
                    /*
                     * When ServiceType is SS_CFU/SS_CF_ALL and RequestType is activate/register
                     * and ServiceClass is Voice/None, set IccRecords.setVoiceCallForwardingFlag.
                     * Only CF status can be set here since number is not available.
                     */
                    Boolean cffEnabled = ((ssData.requestType == SsData.RequestType.SS_ACTIVATION ||
                            ssData.requestType == SsData.RequestType.SS_REGISTRATION) &&
                            IsServiceClassVoiceorNone(ssData.serviceClass));

                    Rlog->D(LOG_TAG, "setVoiceCallForwardingFlag cffEnabled: " + cffEnabled);
                    If (mPhone.mIccRecords != NULL) {
                        mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, NULL);
                        Rlog->D(LOG_TAG, "setVoiceCallForwardingFlag done from SS Info.");
                    } else {
                        Rlog->E(LOG_TAG, "setVoiceCallForwardingFlag aborted. sim records is NULL.");
                    }
                }
                OnSetComplete(NULL, new AsyncResult(NULL, ssData.cfInfo, ex));
                break;
            case SS_INTERROGATION:
                If (ssData.serviceType->IsTypeClir()) {
                    Rlog->D(LOG_TAG, "CLIR INTERROGATION");
                    OnGetClirComplete(new AsyncResult(NULL, ssData.ssInfo, ex));
                } else If (ssData.serviceType->IsTypeCF()) {
                    Rlog->D(LOG_TAG, "CALL FORWARD INTERROGATION");
                    OnQueryCfComplete(new AsyncResult(NULL, ssData.cfInfo, ex));
                } else {
                    OnQueryComplete(new AsyncResult(NULL, ssData.ssInfo, ex));
                }
                break;
            default:
                Rlog->E(LOG_TAG, "Invaid requestType in SSData : " + ssData.requestType);
                break;
        }
    }

    private String GetScStringFromScType(SsData.ServiceType sType) {
        Switch (sType) {
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

        return "";
    }

    private String GetActionStringFromReqType(SsData.RequestType rType) {
        Switch (rType) {
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

        return "";
    }

    private Boolean IsServiceClassVoiceorNone(Int32 serviceClass) {
        Return (((serviceClass & CommandsInterface.SERVICE_CLASS_VOICE) != 0) ||
                (serviceClass == CommandsInterface.SERVICE_CLASS_NONE));
    }

    //***** Private Class methods

    /** make empty strings be NULL.
     *  Regexp returns empty strings for empty groups
     */
    private static String
    MakeEmptyNull (String s) {
        If (s != NULL && s->Length() == 0) return NULL;

        return s;
    }

    /** returns TRUE of the string is empty or NULL */
    private static Boolean
    IsEmptyOrNull(CharSequence s) {
        return s == NULL || (s->Length() == 0);
    }


    private static Int32
    ScToCallForwardReason(String sc) {
        If (sc == NULL) {
            throw new RuntimeException ("invalid call forward sc");
        }

        If (sc->Equals(SC_CF_All)) {
           return CommandsInterface.CF_REASON_ALL;
        } else If (sc->Equals(SC_CFU)) {
            return CommandsInterface.CF_REASON_UNCONDITIONAL;
        } else If (sc->Equals(SC_CFB)) {
            return CommandsInterface.CF_REASON_BUSY;
        } else If (sc->Equals(SC_CFNR)) {
            return CommandsInterface.CF_REASON_NOT_REACHABLE;
        } else If (sc->Equals(SC_CFNRy)) {
            return CommandsInterface.CF_REASON_NO_REPLY;
        } else If (sc->Equals(SC_CF_All_Conditional)) {
           return CommandsInterface.CF_REASON_ALL_CONDITIONAL;
        } else {
            throw new RuntimeException ("invalid call forward sc");
        }
    }

    private static Int32
    SiToServiceClass(String si) {
        If (si == NULL || si->Length() == 0) {
                return  SERVICE_CLASS_NONE;
        } else {
            // NumberFormatException should cause MMI fail
            Int32 serviceCode = Integer->ParseInt(si, 10);

            Switch (serviceCode) {
                case 10: return SERVICE_CLASS_SMS + SERVICE_CLASS_FAX  + SERVICE_CLASS_VOICE;
                case 11: return SERVICE_CLASS_VOICE;
                case 12: return SERVICE_CLASS_SMS + SERVICE_CLASS_FAX;
                case 13: return SERVICE_CLASS_FAX;

                case 16: return SERVICE_CLASS_SMS;

                case 19: return SERVICE_CLASS_FAX + SERVICE_CLASS_VOICE;
/*
    Note for code 20:
     From TS 22.030 Annex C:
                "All GPRS bearer services" are not included in "All tele and bearer services"
                    and "All bearer services"."
....so SERVICE_CLASS_DATA, Which (according to 27.007) includes GPRS
*/
                case 20: return SERVICE_CLASS_DATA_ASYNC + SERVICE_CLASS_DATA_SYNC;

                case 21: return SERVICE_CLASS_PAD + SERVICE_CLASS_DATA_ASYNC;
                case 22: return SERVICE_CLASS_PACKET + SERVICE_CLASS_DATA_SYNC;
                case 24: return SERVICE_CLASS_DATA_SYNC;
                case 25: return SERVICE_CLASS_DATA_ASYNC;
                case 26: return SERVICE_CLASS_DATA_SYNC + SERVICE_CLASS_VOICE;
                case 99: return SERVICE_CLASS_PACKET;

                default:
                    throw new RuntimeException("unsupported MMI service code " + si);
            }
        }
    }

    private static Int32
    SiToTime (String si) {
        If (si == NULL || si->Length() == 0) {
            return 0;
        } else {
            // NumberFormatException should cause MMI fail
            return Integer->ParseInt(si, 10);
        }
    }

    static Boolean
    IsServiceCodeCallForwarding(String sc) {
        return sc != NULL &&
                (sc->Equals(SC_CFU)
                || sc->Equals(SC_CFB) || sc->Equals(SC_CFNRy)
                || sc->Equals(SC_CFNR) || sc->Equals(SC_CF_All)
                || sc->Equals(SC_CF_All_Conditional));
    }

    static Boolean
    IsServiceCodeCallBarring(String sc) {
        Resources resource = Resources->GetSystem();
        If (sc != NULL) {
            String[] barringMMI = resource->GetStringArray(
                R.array.config_callBarringMMI);
            If (barringMMI != NULL) {
                For (String match : barringMMI) {
                    If (sc->Equals(match)) return TRUE;
                }
            }
        }
        return FALSE;
    }

    static String
    ScToBarringFacility(String sc) {
        If (sc == NULL) {
            throw new RuntimeException ("invalid call barring sc");
        }

        If (sc->Equals(SC_BAOC)) {
            return CommandsInterface.CB_FACILITY_BAOC;
        } else If (sc->Equals(SC_BAOIC)) {
            return CommandsInterface.CB_FACILITY_BAOIC;
        } else If (sc->Equals(SC_BAOICxH)) {
            return CommandsInterface.CB_FACILITY_BAOICxH;
        } else If (sc->Equals(SC_BAIC)) {
            return CommandsInterface.CB_FACILITY_BAIC;
        } else If (sc->Equals(SC_BAICr)) {
            return CommandsInterface.CB_FACILITY_BAICr;
        } else If (sc->Equals(SC_BA_ALL)) {
            return CommandsInterface.CB_FACILITY_BA_ALL;
        } else If (sc->Equals(SC_BA_MO)) {
            return CommandsInterface.CB_FACILITY_BA_MO;
        } else If (sc->Equals(SC_BA_MT)) {
            return CommandsInterface.CB_FACILITY_BA_MT;
        } else {
            throw new RuntimeException ("invalid call barring sc");
        }
    }

    //***** Constructor

    GsmMmiCode (GSMPhone phone, UiccCardApplication app) {
        // The telephony unit-test cases may create GsmMmiCode's
        // in secondary threads
        Super(phone->GetHandler()->GetLooper());
        mPhone = phone;
        mContext = phone->GetContext();
        mUiccApplication = app;
        If (app != NULL) {
            mIccRecords = app->GetIccRecords();
        }
    }

    //***** MmiCode implementation

    //@Override
    public State
    GetState() {
        return mState;
    }

    //@Override
    public CharSequence
    GetMessage() {
        return mMessage;
    }

    public Phone
    GetPhone() {
        Return ((Phone) mPhone);
    }

    // inherited javadoc suffices
    //@Override
    CARAPI
    Cancel() {
        // Complete or failed cannot be cancelled
        If (mState == State.COMPLETE || mState == State.FAILED) {
            return;
        }

        mState = State.CANCELLED;

        If (mIsPendingUSSD) {
            /*
             * There can only be one pending USSD session, so tell the radio to
             * cancel it.
             */
            mPhone.mCi->CancelPendingUssd(ObtainMessage(EVENT_USSD_CANCEL_COMPLETE, this));

            /*
             * Don't call phone.onMMIDone here; wait for CANCEL_COMPLETE notice
             * from RIL.
             */
        } else {
            // TODO in cases other than USSD, it would be nice to cancel
            // the pending radio operation. This requires RIL cancellation
            // support, which does not presently exist.

            mPhone.OnMMIDone (this);
        }

    }

    //@Override
    public Boolean IsCancelable() {
        /* Can only cancel pending USSD sessions. */
        return mIsPendingUSSD;
    }

    //***** Instance Methods

    /** Does this dial string contain a structured or unstructured MMI code? */
    Boolean
    IsMMI() {
        return mPoundString != NULL;
    }

    /* Is this a 1 or 2 digit "short code" as defined in TS 22.030 sec 6.5.3.2? */
    Boolean
    IsShortCode() {
        return mPoundString == NULL
                    && mDialingNumber != NULL && mDialingNumber->Length() <= 2;

    }

    static private Boolean
    IsTwoDigitShortCode(Context context, String dialString) {
        Rlog->D(LOG_TAG, "isTwoDigitShortCode");

        If (dialString == NULL || dialString->Length() > 2) return FALSE;

        If (sTwoDigitNumberPattern == NULL) {
            sTwoDigitNumberPattern = context->GetResources()->GetStringArray(
                    R.array.config_twoDigitNumberPattern);
        }

        For (String dialnumber : sTwoDigitNumberPattern) {
            Rlog->D(LOG_TAG, "Two Digit Number Pattern " + dialnumber);
            If (dialString->Equals(dialnumber)) {
                Rlog->D(LOG_TAG, "Two Digit Number Pattern -TRUE");
                return TRUE;
            }
        }
        Rlog->D(LOG_TAG, "Two Digit Number Pattern -FALSE");
        return FALSE;
    }

    /**
     * Helper function for newFromDialString. Returns TRUE if dialString appears
     * to be a short code AND conditions are correct for it to be treated as
     * such.
     */
    static private Boolean IsShortCode(String dialString, GSMPhone phone) {
        // Refer to TS 22.030 Figure 3.5.3.2:
        If (dialString == NULL) {
            return FALSE;
        }

        // Illegal dial string characters will give a ZERO length.
        // At this point we do not want to crash as any application with
        // call privileges may send a non dial string.
        // It return FALSE as when the dialString is equal to NULL.
        If (dialString->Length() == 0) {
            return FALSE;
        }

        If (PhoneNumberUtils->IsLocalEmergencyNumber(phone->GetContext(), dialString)) {
            return FALSE;
        } else {
            return IsShortCodeUSSD(dialString, phone);
        }
    }

    /**
     * Helper function for isShortCode. Returns TRUE if dialString appears to be
     * a short code and it is a USSD structure
     *
     * According to the 3PGG TS 22.030 specification Figure 3.5.3.2: A 1 or 2
     * digit "short code" is treated as USSD if it is entered while on a call or
     * does not satisfy the Condition (exactly 2 digits && starts with '1'), there
     * are however exceptions to this Rule (see below)
     *
     * Exception (1) to Call initiation is: If the user of the device is already in a call
     * and enters a Short String without any #-key at the end and the length of the Short String is
     * equal or less then the MAX_LENGTH_SHORT_CODE [constant that is equal to 2]
     *
     * The phone shall initiate a USSD/SS commands.
     */
    static private Boolean IsShortCodeUSSD(String dialString, GSMPhone phone) {
        If (dialString != NULL && dialString->Length() <= MAX_LENGTH_SHORT_CODE) {
            If (phone->IsInCall()) {
                return TRUE;
            }

            If (dialString->Length() != MAX_LENGTH_SHORT_CODE ||
                    dialString->CharAt(0) != '1') {
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * @return TRUE if the Service Code is PIN/PIN2/PUK/PUK2-related
     */
    Boolean IsPinPukCommand() {
        return mSc != NULL && (mSc->Equals(SC_PIN) || mSc->Equals(SC_PIN2)
                              || mSc->Equals(SC_PUK) || mSc->Equals(SC_PUK2));
     }

    /**
     * See TS 22.030 Annex B.
     * In temporary mode, to suppress CLIR for a single call, enter:
     *      " * 31 # [called number] SEND "
     *  In temporary mode, to invoke CLIR for a single call enter:
     *       " # 31 # [called number] SEND "
     */
    Boolean
    IsTemporaryModeCLIR() {
        return mSc != NULL && mSc->Equals(SC_CLIR) && mDialingNumber != NULL
                && (IsActivate() || IsDeactivate());
    }

    /**
     * returns CommandsInterface.CLIR_*
     * See also IsTemporaryModeCLIR()
     */
    Int32
    GetCLIRMode() {
        If (mSc != NULL && mSc->Equals(SC_CLIR)) {
            If (IsActivate()) {
                return CommandsInterface.CLIR_SUPPRESSION;
            } else If (IsDeactivate()) {
                return CommandsInterface.CLIR_INVOCATION;
            }
        }

        return CommandsInterface.CLIR_DEFAULT;
    }

    Boolean IsActivate() {
        return mAction != NULL && mAction->Equals(ACTION_ACTIVATE);
    }

    Boolean IsDeactivate() {
        return mAction != NULL && mAction->Equals(ACTION_DEACTIVATE);
    }

    Boolean IsInterrogate() {
        return mAction != NULL && mAction->Equals(ACTION_INTERROGATE);
    }

    Boolean IsRegister() {
        return mAction != NULL && mAction->Equals(ACTION_REGISTER);
    }

    Boolean IsErasure() {
        return mAction != NULL && mAction->Equals(ACTION_ERASURE);
    }

    /**
     * Returns TRUE if this is a USSD code that's been submitted to the
     * network...eg, after ProcessCode() is called
     */
    public Boolean IsPendingUSSD() {
        return mIsPendingUSSD;
    }

    //@Override
    public Boolean IsUssdRequest() {
        return mIsUssdRequest;
    }

    public Boolean IsSsInfo() {
        return mIsSsInfo;
    }

    /** Process a MMI code or short code...anything that isn't a dialing number */
    void
    ProcessCode () {
        try {
            If (IsShortCode()) {
                Rlog->D(LOG_TAG, "isShortCode");
                // These just get treated as USSD.
                SendUssd(mDialingNumber);
            } else If (mDialingNumber != NULL) {
                // We should have no dialing numbers here
                throw new RuntimeException ("Invalid or Unsupported MMI Code");
            } else If (mSc != NULL && mSc->Equals(SC_CLIP)) {
                Rlog->D(LOG_TAG, "is CLIP");
                If (IsInterrogate()) {
                    mPhone.mCi->QueryCLIP(
                            ObtainMessage(EVENT_QUERY_COMPLETE, this));
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (mSc != NULL && mSc->Equals(SC_CLIR)) {
                Rlog->D(LOG_TAG, "is CLIR");
                If (IsActivate()) {
                    mPhone.mCi->SetCLIR(CommandsInterface.CLIR_INVOCATION,
                        ObtainMessage(EVENT_SET_COMPLETE, this));
                } else If (IsDeactivate()) {
                    mPhone.mCi->SetCLIR(CommandsInterface.CLIR_SUPPRESSION,
                        ObtainMessage(EVENT_SET_COMPLETE, this));
                } else If (IsInterrogate()) {
                    mPhone.mCi->GetCLIR(
                        ObtainMessage(EVENT_GET_CLIR_COMPLETE, this));
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (IsServiceCodeCallForwarding(mSc)) {
                Rlog->D(LOG_TAG, "is CF");

                String dialingNumber = mSia;
                Int32 serviceClass = SiToServiceClass(mSib);
                Int32 reason = ScToCallForwardReason(mSc);
                Int32 time = SiToTime(mSic);

                If (IsInterrogate()) {
                    mPhone.mCi->QueryCallForwardStatus(
                            reason, serviceClass,  dialingNumber,
                                ObtainMessage(EVENT_QUERY_CF_COMPLETE, this));
                } else {
                    Int32 cfAction;

                    If (IsActivate()) {
                        // 3GPP TS 22.030 6.5.2
                        // a call forwarding request with a single * would be
                        // interpreted as registration if containing a forwarded-to
                        // number, or an activation if not
                        If (IsEmptyOrNull(dialingNumber)) {
                            cfAction = CommandsInterface.CF_ACTION_ENABLE;
                            mIsCallFwdReg = FALSE;
                        } else {
                            cfAction = CommandsInterface.CF_ACTION_REGISTRATION;
                            mIsCallFwdReg = TRUE;
                        }
                    } else If (IsDeactivate()) {
                        cfAction = CommandsInterface.CF_ACTION_DISABLE;
                    } else If (IsRegister()) {
                        cfAction = CommandsInterface.CF_ACTION_REGISTRATION;
                    } else If (IsErasure()) {
                        cfAction = CommandsInterface.CF_ACTION_ERASURE;
                    } else {
                        throw new RuntimeException ("invalid action");
                    }

                    Int32 isSettingUnconditionalVoice =
                        (((reason == CommandsInterface.CF_REASON_UNCONDITIONAL) ||
                                (reason == CommandsInterface.CF_REASON_ALL)) &&
                                (((serviceClass & CommandsInterface.SERVICE_CLASS_VOICE) != 0) ||
                                 (serviceClass == CommandsInterface.SERVICE_CLASS_NONE))) ? 1 : 0;

                    Int32 isEnableDesired =
                        ((cfAction == CommandsInterface.CF_ACTION_ENABLE) ||
                                (cfAction == CommandsInterface.CF_ACTION_REGISTRATION)) ? 1 : 0;

                    Rlog->D(LOG_TAG, "is CF setCallForward");
                    mPhone.mCi->SetCallForward(cfAction, reason, serviceClass,
                            dialingNumber, time, ObtainMessage(
                                    EVENT_SET_CFF_COMPLETE,
                                    isSettingUnconditionalVoice,
                                    isEnableDesired, this));
                }
            } else If (IsServiceCodeCallBarring(mSc)) {
                // sia = password
                // sib = basic service group

                String password = mSia;
                Int32 serviceClass = SiToServiceClass(mSib);
                String facility = ScToBarringFacility(mSc);

                If (IsInterrogate()) {
                    mPhone.mCi->QueryFacilityLock(facility, password,
                            serviceClass, ObtainMessage(EVENT_QUERY_COMPLETE, this));
                } else If (IsActivate() || IsDeactivate()) {
                    mPhone.mCi->SetFacilityLock(facility, IsActivate(), password,
                            serviceClass, ObtainMessage(EVENT_SET_COMPLETE, this));
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }

            } else If (mSc != NULL && mSc->Equals(SC_PWD)) {
                // sia = fac
                // sib = old pwd
                // sic = new pwd
                // pwd = new pwd
                String facility;
                String oldPwd = mSib;
                String newPwd = mSic;
                If (IsActivate() || IsRegister()) {
                    // Even though ACTIVATE is acceptable, this is really termed a REGISTER
                    mAction = ACTION_REGISTER;

                    If (mSia == NULL) {
                        // If sc was not specified, treat it as BA_ALL.
                        facility = CommandsInterface.CB_FACILITY_BA_ALL;
                    } else {
                        facility = ScToBarringFacility(mSia);
                    }
                    If (newPwd->Equals(mPwd)) {
                        mPhone.mCi->ChangeBarringPassword(facility, oldPwd,
                                newPwd, ObtainMessage(EVENT_SET_COMPLETE, this));
                    } else {
                        // password mismatch; return error
                        HandlePasswordError(R::string::passwordIncorrect);
                    }
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }

            } else If (mSc != NULL && mSc->Equals(SC_WAIT)) {
                // sia = basic service group
                Int32 serviceClass = SiToServiceClass(mSia);

                If (IsActivate() || IsDeactivate()) {
                    mPhone.mCi->SetCallWaiting(IsActivate(), serviceClass,
                            ObtainMessage(EVENT_SET_COMPLETE, this));
                } else If (IsInterrogate()) {
                    mPhone.mCi->QueryCallWaiting(serviceClass,
                            ObtainMessage(EVENT_QUERY_COMPLETE, this));
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (IsPinPukCommand()) {
                // TODO: This is the same as the code in CmdaMmiCode.java,
                // MmiCode should be an abstract or base class and this and
                // other common variables and code should be promoted.

                // sia = old PIN or PUK
                // sib = new PIN
                // sic = new PIN
                String oldPinOrPuk = mSia;
                String newPinOrPuk = mSib;
                Int32 pinLen = newPinOrPuk->Length();
                If (IsRegister()) {
                    If (!newPinOrPuk->Equals(mSic)) {
                        // password mismatch; return error
                        HandlePasswordError(R::string::mismatchPin);
                    } else If (pinLen < 4 || pinLen > 8 ) {
                        // invalid length
                        HandlePasswordError(R::string::invalidPin);
                    } else If (mSc->Equals(SC_PIN)
                            && mUiccApplication != NULL
                            && mUiccApplication->GetState() == AppState.APPSTATE_PUK) {
                        // Sim is puk-locked
                        HandlePasswordError(R::string::needPuk);
                    } else If (mUiccApplication != NULL) {
                        Rlog->D(LOG_TAG, "process mmi service code using UiccApp sc=" + mSc);

                        // We have an app and the pre-checks are OK
                        If (mSc->Equals(SC_PIN)) {
                            mUiccApplication->ChangeIccLockPassword(oldPinOrPuk, newPinOrPuk,
                                    ObtainMessage(EVENT_SET_COMPLETE, this));
                        } else If (mSc->Equals(SC_PIN2)) {
                            mUiccApplication->ChangeIccFdnPassword(oldPinOrPuk, newPinOrPuk,
                                    ObtainMessage(EVENT_SET_COMPLETE, this));
                        } else If (mSc->Equals(SC_PUK)) {
                            mUiccApplication->SupplyPuk(oldPinOrPuk, newPinOrPuk,
                                    ObtainMessage(EVENT_SET_COMPLETE, this));
                        } else If (mSc->Equals(SC_PUK2)) {
                            mUiccApplication->SupplyPuk2(oldPinOrPuk, newPinOrPuk,
                                    ObtainMessage(EVENT_SET_COMPLETE, this));
                        } else {
                            throw new RuntimeException("uicc unsupported service code=" + mSc);
                        }
                    } else {
                        throw new RuntimeException("No application mUiccApplicaiton is NULL");
                    }
                } else {
                    throw new RuntimeException ("Ivalid register/action=" + mAction);
                }
            } else If (mPoundString != NULL) {
                SendUssd(mPoundString);
            } else {
                throw new RuntimeException ("Invalid or Unsupported MMI Code");
            }
        } Catch (RuntimeException exc) {
            mState = State.FAILED;
            mMessage = mContext->GetText(R::string::mmiError);
            mPhone->OnMMIDone(this);
        }
    }

    private void HandlePasswordError(Int32 res) {
        mState = State.FAILED;
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");
        sb->Append(mContext->GetText(res));
        mMessage = sb;
        mPhone->OnMMIDone(this);
    }

    /**
     * Called from GSMPhone
     *
     * An unsolicited USSD NOTIFY or REQUEST has come in matching
     * up with this pending USSD request
     *
     * Note: If REQUEST, this exchange is complete, but the session remains
     *       Active (ie, the network expects user input).
     */
    void
    OnUssdFinished(String ussdMessage, Boolean isUssdRequest) {
        If (mState == State.PENDING) {
            If (ussdMessage == NULL) {
                mMessage = mContext->GetText(R::string::mmiComplete);
            } else {
                mMessage = ussdMessage;
            }
            mIsUssdRequest = isUssdRequest;
            // If it's a request, leave it PENDING so that it's cancelable.
            If (!isUssdRequest) {
                mState = State.COMPLETE;
            }

            mPhone->OnMMIDone(this);
        }
    }

    /**
     * Called from GSMPhone
     *
     * The radio has reset, and this is still pending
     */

    void
    OnUssdFinishedError() {
        If (mState == State.PENDING) {
            mState = State.FAILED;
            mMessage = mContext->GetText(R::string::mmiError);

            mPhone->OnMMIDone(this);
        }
    }

    void SendUssd(String ussdMessage) {
        // Treat this as a USSD string
        mIsPendingUSSD = TRUE;

        // Note that unlike most everything else, the USSD complete
        // response does not complete this MMI code...we wait for
        // an unsolicited USSD "Notify" or "Request".
        // The matching up of this is done in GSMPhone.

        mPhone.mCi->SendUSSD(ussdMessage,
            ObtainMessage(EVENT_USSD_COMPLETE, this));
    }

    /** Called from GSMPhone.handleMessage; not a Handler subclass */
    //@Override
    CARAPI
    HandleMessage (Message msg) {
        AsyncResult ar;

        Switch (msg.what) {
            case EVENT_SET_COMPLETE:
                ar = (AsyncResult) (msg.obj);

                OnSetComplete(msg, ar);
                break;

            case EVENT_SET_CFF_COMPLETE:
                ar = (AsyncResult) (msg.obj);

                /*
                * msg.arg1 = 1 means to set unconditional voice call forwarding
                * msg.arg2 = 1 means to enable voice call forwarding
                */
                If ((ar.exception == NULL) && (msg.arg1 == 1)) {
                    Boolean cffEnabled = (msg.arg2 == 1);
                    If (mIccRecords != NULL) {
                        mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, mDialingNumber);
                        mPhone->SetCallForwardingPreference(cffEnabled);
                    }
                }

                OnSetComplete(msg, ar);
                break;

            case EVENT_GET_CLIR_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnGetClirComplete(ar);
            break;

            case EVENT_QUERY_CF_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnQueryCfComplete(ar);
            break;

            case EVENT_QUERY_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnQueryComplete(ar);
            break;

            case EVENT_USSD_COMPLETE:
                ar = (AsyncResult) (msg.obj);

                If (ar.exception != NULL) {
                    mState = State.FAILED;
                    mMessage = GetErrorMessage(ar);

                    mPhone->OnMMIDone(this);
                }

                // Note that unlike most everything else, the USSD complete
                // response does not complete this MMI code...we wait for
                // an unsolicited USSD "Notify" or "Request".
                // The matching up of this is done in GSMPhone.

            break;

            case EVENT_USSD_CANCEL_COMPLETE:
                mPhone->OnMMIDone(this);
            break;
        }
    }
    //***** Private instance methods

    private CharSequence GetErrorMessage(AsyncResult ar) {

        If (ar.exception instanceof CommandException) {
            CommandException.Error err = ((CommandException)(ar.exception)).GetCommandError();
            If (err == CommandException.Error.FDN_CHECK_FAILURE) {
                Rlog->I(LOG_TAG, "FDN_CHECK_FAILURE");
                return mContext->GetText(R::string::mmiFdnError);
            } else If (err == CommandException.Error.USSD_MODIFIED_TO_DIAL) {
                Rlog->I(LOG_TAG, "USSD_MODIFIED_TO_DIAL");
                return mContext->GetText(R::string::stk_cc_ussd_to_dial);
            } else If (err == CommandException.Error.USSD_MODIFIED_TO_SS) {
                Rlog->I(LOG_TAG, "USSD_MODIFIED_TO_SS");
                return mContext->GetText(R::string::stk_cc_ussd_to_ss);
            } else If (err == CommandException.Error.USSD_MODIFIED_TO_USSD) {
                Rlog->I(LOG_TAG, "USSD_MODIFIED_TO_USSD");
                return mContext->GetText(R::string::stk_cc_ussd_to_ussd);
            } else If (err == CommandException.Error.SS_MODIFIED_TO_DIAL) {
                Rlog->I(LOG_TAG, "SS_MODIFIED_TO_DIAL");
                return mContext->GetText(R::string::stk_cc_ss_to_dial);
            } else If (err == CommandException.Error.SS_MODIFIED_TO_USSD) {
                Rlog->I(LOG_TAG, "SS_MODIFIED_TO_USSD");
                return mContext->GetText(R::string::stk_cc_ss_to_ussd);
            } else If (err == CommandException.Error.SS_MODIFIED_TO_SS) {
                Rlog->I(LOG_TAG, "SS_MODIFIED_TO_SS");
                return mContext->GetText(R::string::stk_cc_ss_to_ss);
            }
        }

        return mContext->GetText(R::string::mmiError);
    }

    private CharSequence GetScString() {
        If (mSc != NULL) {
            If (IsServiceCodeCallBarring(mSc)) {
                return mContext->GetText(R::string::BaMmi);
            } else If (IsServiceCodeCallForwarding(mSc)) {
                return mContext->GetText(R::string::CfMmi);
            } else If (mSc->Equals(SC_CLIP)) {
                return mContext->GetText(R::string::ClipMmi);
            } else If (mSc->Equals(SC_CLIR)) {
                return mContext->GetText(R::string::ClirMmi);
            } else If (mSc->Equals(SC_PWD)) {
                return mContext->GetText(R::string::PwdMmi);
            } else If (mSc->Equals(SC_WAIT)) {
                return mContext->GetText(R::string::CwMmi);
            } else If (IsPinPukCommand()) {
                return mContext->GetText(R::string::PinMmi);
            }
        }

        return "";
    }

    private void
    OnSetComplete(Message msg, AsyncResult ar){
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");

        If (ar.exception != NULL) {
            mState = State.FAILED;
            If (ar.exception instanceof CommandException) {
                CommandException.Error err = ((CommandException)(ar.exception)).GetCommandError();
                If (err == CommandException.Error.PASSWORD_INCORRECT) {
                    If (IsPinPukCommand()) {
                        // look specifically for the PUK commands and adjust
                        // the message accordingly.
                        If (mSc->Equals(SC_PUK) || mSc->Equals(SC_PUK2)) {
                            sb->Append(mContext->GetText(
                                    R::string::badPuk));
                        } else {
                            sb->Append(mContext->GetText(
                                    R::string::badPin));
                        }
                        // Get the No. of retries remaining to unlock PUK/PUK2
                        Int32 attemptsRemaining = msg.arg1;
                        If (attemptsRemaining <= 0) {
                            Rlog->D(LOG_TAG, "onSetComplete: PUK locked,"
                                    + " cancel as lock screen will handle this");
                            mState = State.CANCELLED;
                        } else If (attemptsRemaining > 0) {
                            Rlog->D(LOG_TAG, "onSetComplete: attemptsRemaining="+attemptsRemaining);
                            sb->Append(mContext->GetResources()->GetQuantityString(
                                    R.plurals.pinpuk_attempts,
                                    attemptsRemaining, attemptsRemaining));
                        }
                    } else {
                        sb->Append(mContext->GetText(
                                R::string::passwordIncorrect));
                    }
                } else If (err == CommandException.Error.SIM_PUK2) {
                    sb->Append(mContext->GetText(
                            R::string::badPin));
                    sb->Append("\n");
                    sb->Append(mContext->GetText(
                            R::string::needPuk2));
                } else If (err == CommandException.Error.REQUEST_NOT_SUPPORTED) {
                    If (mSc->Equals(SC_PIN)) {
                        sb->Append(mContext->GetText(R::string::enablePin));
                    }
                } else If (err == CommandException.Error.FDN_CHECK_FAILURE) {
                    Rlog->I(LOG_TAG, "FDN_CHECK_FAILURE");
                    sb->Append(mContext->GetText(R::string::mmiFdnError));
                } else {
                    sb->Append(GetErrorMessage(ar));
                }
            } else {
                sb->Append(mContext->GetText(
                        R::string::mmiError));
            }
        } else If (IsActivate()) {
            mState = State.COMPLETE;
            If (mIsCallFwdReg) {
                sb->Append(mContext->GetText(
                        R::string::serviceRegistered));
            } else {
                sb->Append(mContext->GetText(
                        R::string::serviceEnabled));
            }
            // Record CLIR setting
            If (mSc->Equals(SC_CLIR)) {
                mPhone->SaveClirSetting(CommandsInterface.CLIR_INVOCATION);
            }
        } else If (IsDeactivate()) {
            mState = State.COMPLETE;
            sb->Append(mContext->GetText(
                    R::string::serviceDisabled));
            // Record CLIR setting
            If (mSc->Equals(SC_CLIR)) {
                mPhone->SaveClirSetting(CommandsInterface.CLIR_SUPPRESSION);
            }
        } else If (IsRegister()) {
            mState = State.COMPLETE;
            sb->Append(mContext->GetText(
                    R::string::serviceRegistered));
        } else If (IsErasure()) {
            mState = State.COMPLETE;
            sb->Append(mContext->GetText(
                    R::string::serviceErased));
        } else {
            mState = State.FAILED;
            sb->Append(mContext->GetText(
                    R::string::mmiError));
        }

        mMessage = sb;
        mPhone->OnMMIDone(this);
    }

    private void
    OnGetClirComplete(AsyncResult ar) {
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");

        If (ar.exception != NULL) {
            mState = State.FAILED;
            sb->Append(GetErrorMessage(ar));
        } else {
            Int32 clirArgs[];

            clirArgs = (Int32[])ar.result;

            // the 'm' parameter from TS 27.007 7.7
            Switch (clirArgs[1]) {
                case 0: // CLIR not provisioned
                    sb->Append(mContext->GetText(
                                R::string::serviceNotProvisioned));
                    mState = State.COMPLETE;
                break;

                case 1: // CLIR provisioned in permanent mode
                    sb->Append(mContext->GetText(
                                R::string::CLIRPermanent));
                    mState = State.COMPLETE;
                break;

                case 2: // Unknown (e.g. no network, etc.)
                    sb->Append(mContext->GetText(
                                R::string::mmiError));
                    mState = State.FAILED;
                break;

                case 3: // CLIR temporary mode presentation restricted

                    // the 'n' parameter from TS 27.007 7.7
                    Switch (clirArgs[0]) {
                        default:
                        case 0: // Default
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOnNextCallOn));
                        break;
                        case 1: // CLIR invocation
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOnNextCallOn));
                        break;
                        case 2: // CLIR suppression
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOnNextCallOff));
                        break;
                    }
                    mState = State.COMPLETE;
                break;

                case 4: // CLIR temporary mode presentation allowed
                    // the 'n' parameter from TS 27.007 7.7
                    Switch (clirArgs[0]) {
                        default:
                        case 0: // Default
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOffNextCallOff));
                        break;
                        case 1: // CLIR invocation
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOffNextCallOn));
                        break;
                        case 2: // CLIR suppression
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOffNextCallOff));
                        break;
                    }

                    mState = State.COMPLETE;
                break;
            }
        }

        mMessage = sb;
        mPhone->OnMMIDone(this);
    }

    /**
     * @param serviceClass 1 bit of the service class bit vectory
     * @return String to be used for call forward query MMI response text.
     *        Returns NULL if unrecognized
     */

    private CharSequence
    ServiceClassToCFString (Int32 serviceClass) {
        Switch (serviceClass) {
            case SERVICE_CLASS_VOICE:
                return mContext->GetText(R::string::serviceClassVoice);
            case SERVICE_CLASS_DATA:
                return mContext->GetText(R::string::serviceClassData);
            case SERVICE_CLASS_FAX:
                return mContext->GetText(R::string::serviceClassFAX);
            case SERVICE_CLASS_SMS:
                return mContext->GetText(R::string::serviceClassSMS);
            case SERVICE_CLASS_DATA_SYNC:
                return mContext->GetText(R::string::serviceClassDataSync);
            case SERVICE_CLASS_DATA_ASYNC:
                return mContext->GetText(R::string::serviceClassDataAsync);
            case SERVICE_CLASS_PACKET:
                return mContext->GetText(R::string::serviceClassPacket);
            case SERVICE_CLASS_PAD:
                return mContext->GetText(R::string::serviceClassPAD);
            default:
                return NULL;
        }
    }


    /** one CallForwardInfo + serviceClassMask -> one line of text */
    private CharSequence
    MakeCFQueryResultMessage(CallForwardInfo info, Int32 serviceClassMask) {
        CharSequence template;
        String sources[] = {"{0}", "{1}", "{2}"};
        CharSequence destinations[] = new CharSequence[3];
        Boolean needTimeTemplate;

        // CF_REASON_NO_REPLY also has a time value associated with
        // it. All others don't.

        needTimeTemplate =
            (info.reason == CommandsInterface.CF_REASON_NO_REPLY);

        If (info.status == 1) {
            If (needTimeTemplate) {
                template = mContext->GetText(
                        R::string::cfTemplateForwardedTime);
            } else {
                template = mContext->GetText(
                        R::string::cfTemplateForwarded);
            }
        } else If (info.status == 0 && IsEmptyOrNull(info.number)) {
            template = mContext->GetText(
                        R::string::cfTemplateNotForwarded);
        } else { /* (info.status == 0) && !IsEmptyOrNull(info.number) */
            // A call forward record that is not active but contains
            // a phone number is considered "registered"

            If (needTimeTemplate) {
                template = mContext->GetText(
                        R::string::cfTemplateRegisteredTime);
            } else {
                template = mContext->GetText(
                        R::string::cfTemplateRegistered);
            }
        }

        // In the Template (from strings.xmls)
        //         {0} is one of "bearerServiceCode*"
        //        {1} is dialing number
        //      {2} is time in seconds

        destinations[0] = ServiceClassToCFString(info.serviceClass & serviceClassMask);
        destinations[1] = FormatLtr(
                PhoneNumberUtils->StringFromStringAndTOA(info.number, info.toa));
        destinations[2] = Integer->ToString(info.timeSeconds);

        If (info.reason == CommandsInterface.CF_REASON_UNCONDITIONAL &&
                (info.serviceClass & serviceClassMask)
                        == CommandsInterface.SERVICE_CLASS_VOICE) {
            Boolean cffEnabled = (info.status == 1);
            If (mIccRecords != NULL) {
                mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, info.number);
                mPhone->SetCallForwardingPreference(cffEnabled);
            }
        }

        return TextUtils->Replace(template, sources, destinations);
    }

    /**
     * Used to format a string that should be displayed as LTR even in RTL locales
     */
    private String FormatLtr(String str) {
        BidiFormatter fmt = BidiFormatter->GetInstance();
        return str == NULL ? str : fmt->UnicodeWrap(str, TextDirectionHeuristics.LTR, TRUE);
    }

    private void
    OnQueryCfComplete(AsyncResult ar) {
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");

        If (ar.exception != NULL) {
            mState = State.FAILED;
            sb->Append(GetErrorMessage(ar));
        } else {
            CallForwardInfo infos[];

            infos = (CallForwardInfo[]) ar.result;

            If (infos.length == 0) {
                // Assume the default is not active
                sb->Append(mContext->GetText(R::string::serviceDisabled));

                // Set unconditional CFF in SIM to FALSE
                If (mIccRecords != NULL) {
                    mPhone->SetCallForwardingPreference(FALSE);
                    mIccRecords->SetVoiceCallForwardingFlag(1, FALSE, NULL);
                }
            } else {

                SpannableStringBuilder tb = new SpannableStringBuilder();

                // Each bit in the service class gets its own result line
                // The service classes may be split up over multiple
                // CallForwardInfos. So, for each service class, find out
                // which CallForwardInfo represents it and then build
                // the response text based on that

                For (Int32 serviceClassMask = 1
                            ; serviceClassMask <= SERVICE_CLASS_MAX
                            ; serviceClassMask <<= 1
                ) {
                    For (Int32 i = 0, s = infos.length; i < s ; i++) {
                        If ((serviceClassMask & infos[i].serviceClass) != 0) {
                            tb->Append(MakeCFQueryResultMessage(infos[i],
                                            serviceClassMask));
                            tb->Append("\n");
                        }
                    }
                }
                sb->Append(tb);
            }

            mState = State.COMPLETE;
        }

        mMessage = sb;
        mPhone->OnMMIDone(this);

    }

    private void
    OnQueryComplete(AsyncResult ar) {
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");

        If (ar.exception != NULL) {
            mState = State.FAILED;
            sb->Append(GetErrorMessage(ar));
        } else {
            Int32[] ints = (Int32[])ar.result;

            If (ints.length != 0) {
                If (ints[0] == 0) {
                    sb->Append(mContext->GetText(R::string::serviceDisabled));
                } else If (mSc->Equals(SC_WAIT)) {
                    // Call Waiting includes additional data in the response.
                    sb->Append(CreateQueryCallWaitingResultMessage(ints[1]));
                } else If (IsServiceCodeCallBarring(mSc)) {
                    // ints[0] for Call Barring is a bit vector of services
                    sb->Append(CreateQueryCallBarringResultMessage(ints[0]));
                } else If (ints[0] == 1) {
                    // for all other services, treat it as a Boolean
                    sb->Append(mContext->GetText(R::string::serviceEnabled));
                } else {
                    sb->Append(mContext->GetText(R::string::mmiError));
                }
            } else {
                sb->Append(mContext->GetText(R::string::mmiError));
            }
            mState = State.COMPLETE;
        }

        mMessage = sb;
        mPhone->OnMMIDone(this);
    }

    private CharSequence
    CreateQueryCallWaitingResultMessage(Int32 serviceClass) {
        StringBuilder sb =
                new StringBuilder(mContext->GetText(R::string::serviceEnabledFor));

        For (Int32 classMask = 1
                    ; classMask <= SERVICE_CLASS_MAX
                    ; classMask <<= 1
        ) {
            If ((classMask & serviceClass) != 0) {
                sb->Append("\n");
                sb->Append(ServiceClassToCFString(classMask & serviceClass));
            }
        }
        return sb;
    }
    private CharSequence
    CreateQueryCallBarringResultMessage(Int32 serviceClass)
    {
        StringBuilder sb = new StringBuilder(mContext->GetText(R::string::serviceEnabledFor));

        For (Int32 classMask = 1
                    ; classMask <= SERVICE_CLASS_MAX
                    ; classMask <<= 1
        ) {
            If ((classMask & serviceClass) != 0) {
                sb->Append("\n");
                sb->Append(ServiceClassToCFString(classMask & serviceClass));
            }
        }
        return sb;
    }

    /***
     * TODO: It would be nice to have a method here that can take in a dialstring and
     * figure out if there is an MMI code embedded within it.  This code would replace
     * some of the string parsing functionality in the Phone App's
     * SpecialCharSequenceMgr class.
     */

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder sb = new StringBuilder("GsmMmiCode {");

        sb->Append("State=" + GetState());
        If (mAction != NULL) sb->Append(" action=" + mAction);
        If (mSc != NULL) sb->Append(" sc=" + mSc);
        If (mSia != NULL) sb->Append(" sia=" + mSia);
        If (mSib != NULL) sb->Append(" sib=" + mSib);
        If (mSic != NULL) sb->Append(" sic=" + mSic);
        If (mPoundString != NULL) sb->Append(" poundString=" + mPoundString);
        If (mDialingNumber != NULL) sb->Append(" dialingNumber=" + mDialingNumber);
        If (mPwd != NULL) sb->Append(" pwd=" + mPwd);
        sb->Append("}");
        return sb->ToString();
    }
}
