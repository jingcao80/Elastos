/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.internal.telephony.imsphone;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Ims::IImsReasonInfo;
using Elastos::Droid::Ims::IImsSsInfo;
using Elastos::Droid::Ims::IImsUtInterface;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;

using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_NONE;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_VOICE;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_DATA;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_FAX;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_SMS;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_DATA_SYNC;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_DATA_ASYNC;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_PACKET;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_PAD;
using static com::Android::Internal::Telephony::CommandsInterface::ISERVICE_CLASS_MAX;

using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::IPhone;

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
public class ImsPhoneMmiCode extends Handler implements MmiCode {
    static const String LOG_TAG = "ImsPhoneMmiCode";

    //***** Constants

    // Max Size of the Short Code (aka Short String from TS 22.030 6.5.2)
    private static const Int32 MAX_LENGTH_SHORT_CODE = 2;

    // TS 22.030 6.5.2 Every Short String USSD command will end with #-key
    // (known as #-String)
    private static const Char32 END_OF_USSD_COMMAND = '#';

    // From TS 22.030 6.5.2
    private static const String ACTION_ACTIVATE = "*";
    private static const String ACTION_DEACTIVATE = "#";
    private static const String ACTION_INTERROGATE = "*#";
    private static const String ACTION_REGISTER = "**";
    private static const String ACTION_ERASURE = "##";

    // Supp Service codes from TS 22.030 Annex B

    //Called line presentation
    private static const String SC_CLIP    = "30";
    private static const String SC_CLIR    = "31";
    private static const String SC_COLP    = "76";
    private static const String SC_COLR    = "77";

    //Calling name presentation
    private static const String SC_CNAP    = "300";

    // Call Forwarding
    private static const String SC_CFU     = "21";
    private static const String SC_CFB     = "67";
    private static const String SC_CFNRy   = "61";
    private static const String SC_CFNR    = "62";
    // Call Forwarding unconditional Timer
    private static const String SC_CFUT     = "22";

    private static const String SC_CF_All = "002";
    private static const String SC_CF_All_Conditional = "004";

    // Call Waiting
    private static const String SC_WAIT     = "43";

    // Call Barring
    private static const String SC_BAOC         = "33";
    private static const String SC_BAOIC        = "331";
    private static const String SC_BAOICxH      = "332";
    private static const String SC_BAIC         = "35";
    private static const String SC_BAICr        = "351";

    private static const String SC_BA_ALL       = "330";
    private static const String SC_BA_MO        = "333";
    private static const String SC_BA_MT        = "353";

    // Incoming/Anonymous call barring
    private static const String SC_BS_MT        = "156";
    private static const String SC_BAICa        = "157";

    // Supp Service Password registration
    private static const String SC_PWD          = "03";

    // PIN/PIN2/PUK/PUK2
    private static const String SC_PIN          = "04";
    private static const String SC_PIN2         = "042";
    private static const String SC_PUK          = "05";
    private static const String SC_PUK2         = "052";

    //***** Event Constants

    private static const Int32 EVENT_SET_COMPLETE            = 0;
    private static const Int32 EVENT_QUERY_CF_COMPLETE       = 1;
    private static const Int32 EVENT_USSD_COMPLETE           = 2;
    private static const Int32 EVENT_QUERY_COMPLETE          = 3;
    private static const Int32 EVENT_SET_CFF_COMPLETE        = 4;
    private static const Int32 EVENT_USSD_CANCEL_COMPLETE    = 5;
    private static const Int32 EVENT_GET_CLIR_COMPLETE       = 6;
    private static const Int32 EVENT_SUPP_SVC_QUERY_COMPLETE = 7;
    private static const Int32 EVENT_QUERY_CFUT_COMPLETE     = 8;
    private static const Int32 EVENT_SET_CFF_TIMER_COMPLETE  = 9;
    private static const Int32 EVENT_QUERY_ICB_COMPLETE      = 10;

    //***** Calling Line Presentation Constants
    private static const Int32 NUM_PRESENTATION_ALLOWED     = 0;
    private static const Int32 NUM_PRESENTATION_RESTRICTED  = 1;

    //***** Supplementary Service Query Bundle Keys
    // Used by IMS Service layer to put supp. serv. query
    // responses into the ssInfo Bundle.
    public static const String UT_BUNDLE_KEY_CLIR = "queryClir";
    public static const String UT_BUNDLE_KEY_SSINFO = "imsSsInfo";

    //***** Calling Line Identity Restriction Constants
    // The 'm' parameter from TS 27.007 7.7
    private static const Int32 CLIR_NOT_PROVISIONED                    = 0;
    private static const Int32 CLIR_PROVISIONED_PERMANENT              = 1;
    private static const Int32 CLIR_PRESENTATION_RESTRICTED_TEMPORARY  = 3;
    private static const Int32 CLIR_PRESENTATION_ALLOWED_TEMPORARY     = 4;
    // The 'n' parameter from TS 27.007 7.7
    private static const Int32 CLIR_DEFAULT     = 0;
    private static const Int32 CLIR_INVOCATION  = 1;
    private static const Int32 CLIR_SUPPRESSION = 2;

    //***** Instance Variables

    private ImsPhone mPhone;
    private Context mContext;
    private IccRecords mIccRecords;

    private String mAction;              // One of ACTION_*
    private String mSc;                  // Service Code
    private String mSia, mSib, mSic;       // Service Info a,b,c
    private String mPoundString;         // Entire MMI string up to and including #
    private String mDialingNumber;
    private String mPwd;                 // For password registration

    private Boolean mIsPendingUSSD;

    private Boolean mIsUssdRequest;

    private Boolean mIsCallFwdReg;
    private State mState = State.PENDING;
    private CharSequence mMessage;
    //Displayed as the title for Call Forward Timer
    static const String CfuTimer = "Call Forward Unconditional Timer";
    //resgister/erasure of ICB (Specific DN)
    static const String IcbDnMmi = "Specific Incoming Call Barring";
    //ICB (Anonymous)
    static const String IcbAnonymousMmi = "Anonymous Incoming Call Barring";
    //***** Class Variables


    // See TS 22.030 6.5.2 "Structure of the MMI"

    private static Pattern sPatternSuppService = Pattern->Compile(
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

    private static const Int32 MATCH_GROUP_POUND_STRING = 1;

    private static const Int32 MATCH_GROUP_ACTION = 2;
                        //(activation/interrogation/registration/erasure)

    private static const Int32 MATCH_GROUP_SERVICE_CODE = 3;
    private static const Int32 MATCH_GROUP_SIA = 5;
    private static const Int32 MATCH_GROUP_SIB = 7;
    private static const Int32 MATCH_GROUP_SIC = 9;
    private static const Int32 MATCH_GROUP_PWD_CONFIRM = 11;
    private static const Int32 MATCH_GROUP_DIALING_NUMBER = 12;
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

    static ImsPhoneMmiCode
    NewFromDialString(String dialString, ImsPhone phone) {
        Matcher m;
        ImsPhoneMmiCode ret = NULL;

        m = sPatternSuppService->Matcher(dialString);

        // Is this formatted like a standard supplementary service code?
        If (m->Matches()) {
            ret = new ImsPhoneMmiCode(phone);
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
            If (ret.mDialingNumber != NULL &&
                    ret.mDialingNumber->EndsWith("#") &&
                    dialString->EndsWith("#")){
                ret = new ImsPhoneMmiCode(phone);
                ret.mPoundString = dialString;
            }
        } else If (dialString->EndsWith("#")) {
            // TS 22.030 sec 6.5.3.2
            // "Entry of any characters defined in the 3GPP TS 23.038 [8] Default Alphabet
            // (up to the maximum defined in 3GPP TS 24.080 [10]), followed by #SEND".

            ret = new ImsPhoneMmiCode(phone);
            ret.mPoundString = dialString;
        } else If (IsTwoDigitShortCode(phone->GetContext(), dialString)) {
            //Is a country-specific exception to short codes as defined in TS 22.030, 6.5.3.2
            ret = NULL;
        } else If (IsShortCode(dialString, phone)) {
            // this may be a short code, as defined in TS 22.030, 6.5.3.2
            ret = new ImsPhoneMmiCode(phone);
            ret.mDialingNumber = dialString;
        }

        return ret;
    }

    static ImsPhoneMmiCode
    NewNetworkInitiatedUssd (String ussdMessage,
                                Boolean isUssdRequest, ImsPhone phone) {
        ImsPhoneMmiCode ret;

        ret = new ImsPhoneMmiCode(phone);

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

    static ImsPhoneMmiCode NewFromUssdUserInput(String ussdMessge,
                                           ImsPhone phone) {
        ImsPhoneMmiCode ret = new ImsPhoneMmiCode(phone);

        ret.mMessage = ussdMessge;
        ret.mState = State.PENDING;
        ret.mIsPendingUSSD = TRUE;

        return ret;
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

    static Boolean IsScMatchesSuppServType(String dialString) {
        Boolean isMatch = FALSE;
        Matcher m = sPatternSuppService->Matcher(dialString);
        If (m->Matches()) {
            String sc = MakeEmptyNull(m->Group(MATCH_GROUP_SERVICE_CODE));
            If (sc->Equals(SC_CFUT)) {
                isMatch = TRUE;
            } else If(sc->Equals(SC_BS_MT)) {
                isMatch = TRUE;
            }
        }
        return isMatch;
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
        } else If ((sc->Equals(SC_CFU)) || (sc->Equals(SC_CFUT))) {
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

    static Boolean IsServiceCodeUncondCallFwdTimer(String sc) {
        return sc != NULL && sc->Equals(SC_CFUT);
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

    ImsPhoneMmiCode(ImsPhone phone) {
        // The telephony unit-test cases may create ImsPhoneMmiCode's
        // in secondary threads
        Super(phone->GetHandler()->GetLooper());
        mPhone = phone;
        mContext = phone->GetContext();
        mIccRecords = mPhone.mDefaultPhone.mIccRecords->Get();
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

    //@Override
    public Phone GetPhone() { return mPhone; }

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
            mPhone->CancelUSSD();
        } else {
            mPhone.OnMMIDone (this);
        }

    }

    //@Override
    public Boolean IsCancelable() {
        /* Can only cancel pending USSD sessions. */
        return mIsPendingUSSD;
    }

    //***** Instance Methods

    String GetDialingNumber() {
        return mDialingNumber;
    }

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
    static private Boolean IsShortCode(String dialString, ImsPhone phone) {
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
    static private Boolean IsShortCodeUSSD(String dialString, ImsPhone phone) {
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

    Boolean
    IsSupportedOverImsPhone() {
        If (IsShortCode()) return TRUE;
        else If (mDialingNumber != NULL) return FALSE;
        else If (IsServiceCodeCallForwarding(mSc)
                || IsServiceCodeCallBarring(mSc)
                || (mSc != NULL && mSc->Equals(SC_WAIT))
                || (mSc != NULL && mSc->Equals(SC_CLIR))
                || (mSc != NULL && mSc->Equals(SC_CLIP))
                || (mSc != NULL && mSc->Equals(SC_COLR))
                || (mSc != NULL && mSc->Equals(SC_COLP))
                || (mSc != NULL && mSc->Equals(SC_BS_MT))
                || (mSc != NULL && mSc->Equals(SC_BAICa))) {

            Int32 serviceClass = SiToServiceClass(mSib);
            If (serviceClass != SERVICE_CLASS_NONE
                    && serviceClass != SERVICE_CLASS_VOICE) {
                return FALSE;
            }
            return TRUE;
        } else If (IsPinPukCommand()
                || (mSc != NULL
                    && (mSc->Equals(SC_PWD) || mSc->Equals(SC_CLIP) || mSc->Equals(SC_CLIR)))) {
            return FALSE;
        } else If (mPoundString != NULL) return TRUE;

        return FALSE;
    }

    public Int32 CalForwardAction (String dialingNumber) {
        If (IsActivate()) {
            // 3GPP TS 22.030 6.5.2
            // a call forwarding request with a single * would be
            // interpreted as registration if containing a forwarded-to
            // number, or an activation if not
            If (IsEmptyOrNull(dialingNumber)) {
                mIsCallFwdReg = FALSE;
                return CommandsInterface.CF_ACTION_ENABLE;
            } else {
                mIsCallFwdReg = TRUE;
                return CommandsInterface.CF_ACTION_REGISTRATION;
            }
        } else If (IsDeactivate()) {
            return CommandsInterface.CF_ACTION_DISABLE;
        } else If (IsRegister()) {
            return CommandsInterface.CF_ACTION_REGISTRATION;
        } else If (IsErasure()) {
            return CommandsInterface.CF_ACTION_ERASURE;
        } else {
            throw new RuntimeException ("invalid action");
        }
    }

    /*
     * The below actions are IMS/Volte CallBarring actions.We have not defined
     * these actions in ImscommandInterface.However we have reused existing
     * actions of CallForwarding as, both CF and CB actions are used for same
     * purpose.
     */
    public Int32 CallBarrAction(String dialingNumber) {
        If (IsActivate()) {
            return CommandsInterface.CF_ACTION_ENABLE;
        } else If (IsDeactivate()) {
            return CommandsInterface.CF_ACTION_DISABLE;
        } else If (IsRegister()) {
            If (!IsEmptyOrNull(dialingNumber)) {
                return CommandsInterface.CF_ACTION_REGISTRATION;
            } else {
                throw new RuntimeException ("invalid action");
            }
        } else If (IsErasure()) {
            return CommandsInterface.CF_ACTION_ERASURE;
        } else {
            throw new RuntimeException ("invalid action");
        }
    }

    /** Process a MMI code or short code...anything that isn't a dialing number */
    void
    ProcessCode () throws CallStateException {
        try {
            If (IsShortCode()) {
                Rlog->D(LOG_TAG, "isShortCode");

                // These just get treated as USSD.
                Rlog->D(LOG_TAG, "Sending short code '"
                       + mDialingNumber + "' over CS pipe.");
                throw new CallStateException(ImsPhone.CS_FALLBACK);
            } else If (IsServiceCodeCallForwarding(mSc)) {
                Rlog->D(LOG_TAG, "is CF");
                // service group is not supported

                String dialingNumber = mSia;
                Int32 reason = ScToCallForwardReason(mSc);
                Int32 time = SiToTime(mSic);

                If (IsInterrogate()) {
                    mPhone->GetCallForwardingOption(reason,
                            ObtainMessage(EVENT_QUERY_CF_COMPLETE, this));
                } else {
                    Int32 cfAction;

                    cfAction = CalForwardAction(dialingNumber);

                    Int32 isSettingUnconditional =
                            ((reason == CommandsInterface.CF_REASON_UNCONDITIONAL) ||
                             (reason == CommandsInterface.CF_REASON_ALL)) ? 1 : 0;

                    Int32 isEnableDesired =
                        ((cfAction == CommandsInterface.CF_ACTION_ENABLE) ||
                                (cfAction == CommandsInterface.CF_ACTION_REGISTRATION)) ? 1 : 0;

                    Rlog->D(LOG_TAG, "is CF setCallForward");
                    mPhone->SetCallForwardingOption(cfAction, reason,
                            dialingNumber, time, ObtainMessage(
                                    EVENT_SET_CFF_COMPLETE,
                                    isSettingUnconditional,
                                    isEnableDesired, this));
                }
            } else If (IsServiceCodeUncondCallFwdTimer(mSc)) {
                Rlog->D(LOG_TAG, "is UncondCFTimer");

                String dialingNumber = mSia;
                Int32 reason = ScToCallForwardReason(mSc);

                If (IsInterrogate()) {

                    mPhone->GetCallForwardingOption(reason,
                            ObtainMessage(EVENT_QUERY_CFUT_COMPLETE, this));
                } else {
                    Int32 cfTimerAction;
                    Int32 startHour = 0;
                    Int32 startMinute = 0;
                    Int32 endHour = 0;
                    Int32 endMinute = 0;
                    If (mSic != NULL) {
                        // sic contains string in the format HH:MM$HH:MM.String
                        // will be splitted by '$' that gives two halves.First half
                        // signifies starthour and startminute and second half
                        // signifies endhour and endminute.
                        String duration[] = mSic->Split("\\$");

                        If((duration[0] !=NULL) && (duration[1]!=NULL)) {
                            //splitting to get starthour and startminute
                            String start[] = duration[0].Split("\\:");
                            If (start[0] != NULL) {
                                startHour = Integer->ParseInt(start[0]);
                            }
                            If (start[1] != NULL) {
                                startMinute = Integer->ParseInt(start[1]);
                            }
                            //splitting to get endhour and endminute
                            String end[] = duration[1].Split("\\:");
                            If (end[0] !=NULL) {
                                endHour = Integer->ParseInt(end[0]);
                            }
                            If (end[1] !=NULL) {
                                endMinute = Integer->ParseInt(end[1]);
                            }
                        }
                    }
                    cfTimerAction = CalForwardAction(dialingNumber);
                    Rlog->D(LOG_TAG, "is CFUT setCallForward");
                    mPhone->SetCallForwardingUncondTimerOption(startHour,
                            startMinute, endHour, endMinute, cfTimerAction,
                            reason, dialingNumber,
                            ObtainMessage(EVENT_SET_CFF_TIMER_COMPLETE,this));
                }
            } else If (IsServiceCodeCallBarring(mSc)) {
                // sia = password
                // sib = basic service group
                // service group is not supported

                String password = mSia;
                String facility = ScToBarringFacility(mSc);

                If (IsInterrogate()) {
                    mPhone->GetCallBarring(facility,
                            ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
                } else If (IsActivate() || IsDeactivate()) {
                    mPhone->SetCallBarring(facility, IsActivate(), password,
                            ObtainMessage(EVENT_SET_COMPLETE, this));
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (mSc != NULL && mSc->Equals(SC_CLIR)) {
                // NOTE: Since these supplementary services are accessed only
                //       via MMI codes, methods have not been added to ImsPhone.
                //       Only the UT interface handle is used.
                If (IsActivate()) {
                    try {
                        mPhone.mCT->GetUtInterface()->UpdateCLIR(CommandsInterface.CLIR_INVOCATION,
                            ObtainMessage(EVENT_SET_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for updateCLIR.");
                    }
                } else If (IsDeactivate()) {
                    try {
                        mPhone.mCT->GetUtInterface()->UpdateCLIR(CommandsInterface.CLIR_SUPPRESSION,
                            ObtainMessage(EVENT_SET_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for updateCLIR.");
                    }
                } else If (IsInterrogate()) {
                    try {
                        mPhone.mCT->GetUtInterface()
                            .QueryCLIR(ObtainMessage(EVENT_GET_CLIR_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for queryCLIR.");
                    }
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (mSc != NULL && mSc->Equals(SC_CLIP)) {
                // NOTE: Refer to the note above.
                If (IsInterrogate()) {
                    try {
                        mPhone.mCT->GetUtInterface()
                            .QueryCLIP(ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for queryCLIP.");
                    }
                } else If (IsActivate() || IsDeactivate()) {
                    try {
                        mPhone.mCT->GetUtInterface()->UpdateCLIP(IsActivate(),
                                ObtainMessage(EVENT_SET_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for updateCLIP.");
                    }
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (mSc != NULL && mSc->Equals(SC_COLP)) {
                // NOTE: Refer to the note above.
                If (IsInterrogate()) {
                    try {
                        mPhone.mCT->GetUtInterface()
                            .QueryCOLP(ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for queryCOLP.");
                    }
                } else If (IsActivate() || IsDeactivate()) {
                    try {
                        mPhone.mCT->GetUtInterface()->UpdateCOLP(IsActivate(),
                                 ObtainMessage(EVENT_SET_COMPLETE, this));
                     } Catch (ImsException e) {
                         Rlog->D(LOG_TAG, "Could not get UT handle for updateCOLP.");
                     }
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (mSc != NULL && mSc->Equals(SC_COLR)) {
                // NOTE: Refer to the note above.
                If (IsActivate()) {
                    try {
                        mPhone.mCT->GetUtInterface()->UpdateCOLR(NUM_PRESENTATION_RESTRICTED,
                                ObtainMessage(EVENT_SET_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for updateCOLR.");
                    }
                } else If (IsDeactivate()) {
                    try {
                        mPhone.mCT->GetUtInterface()->UpdateCOLR(NUM_PRESENTATION_ALLOWED,
                                ObtainMessage(EVENT_SET_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for updateCOLR.");
                    }
                } else If (IsInterrogate()) {
                    try {
                        mPhone.mCT->GetUtInterface()
                            .QueryCOLR(ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE, this));
                    } Catch (ImsException e) {
                        Rlog->D(LOG_TAG, "Could not get UT handle for queryCOLR.");
                    }
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (mSc != NULL && (mSc->Equals(SC_BS_MT))) {
                try {
                    If (IsInterrogate()) {
                        mPhone.mCT->GetUtInterface()
                        .QueryCallBarring(ImsUtInterface.CB_BS_MT,
                                          ObtainMessage(EVENT_QUERY_ICB_COMPLETE,this));
                    } else {
                        ProcessIcbMmiCodeForUpdate();
                    }
                 // TODO: IsRegister() case needs to be handled.
                } Catch (ImsException e) {
                    Rlog->D(LOG_TAG, "Could not get UT handle for ICB.");
                }
            } else If (mSc != NULL && mSc->Equals(SC_BAICa)) {
                Int32 callAction =0;
                // TODO: Should we route through QueryCallBarring() here?
                try {
                    If (IsInterrogate()) {
                        mPhone.mCT->GetUtInterface()
                        .QueryCallBarring(ImsUtInterface.CB_BIC_ACR,
                                          ObtainMessage(EVENT_SUPP_SVC_QUERY_COMPLETE,this));
                    } else {
                        If (IsActivate()) {
                            callAction = CommandsInterface.CF_ACTION_ENABLE;
                        } else If (IsDeactivate()) {
                            callAction = CommandsInterface.CF_ACTION_DISABLE;
                        }
                        mPhone.mCT->GetUtInterface()
                                .UpdateCallBarring(ImsUtInterface.CB_BIC_ACR,
                                callAction,
                                ObtainMessage(EVENT_SET_COMPLETE,this),
                                NULL);
                    }
                } Catch (ImsException e) {
                    Rlog->D(LOG_TAG, "Could not get UT handle for ICBa.");
                }
            } else If (mSc != NULL && mSc->Equals(SC_WAIT)) {
                // sia = basic service group
                // service group is not supported
                If (IsActivate() || IsDeactivate()) {
                    mPhone->SetCallWaiting(IsActivate(),
                            ObtainMessage(EVENT_SET_COMPLETE, this));
                } else If (IsInterrogate()) {
                    mPhone->GetCallWaiting(ObtainMessage(EVENT_QUERY_COMPLETE, this));
                } else {
                    throw new RuntimeException ("Invalid or Unsupported MMI Code");
                }
            } else If (mPoundString != NULL) {
                Rlog->D(LOG_TAG, "Sending pound string '"
                       + mDialingNumber + "' over CS pipe.");
                throw new CallStateException(ImsPhone.CS_FALLBACK);
            } else {
                throw new RuntimeException ("Invalid or Unsupported MMI Code");
            }
        } Catch (RuntimeException exc) {
            mState = State.FAILED;
            mMessage = mContext->GetText(R::string::mmiError);
            mPhone->OnMMIDone(this);
        }
    }

    /**
     * Called from ImsPhone
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
     * Called from ImsPhone
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
        // The matching up of this is done in ImsPhone.

        mPhone->SendUSSD(ussdMessage,
            ObtainMessage(EVENT_USSD_COMPLETE, this));
    }

    /** Called from ImsPhone.handleMessage; not a Handler subclass */
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
                    }
                }

                OnSetComplete(msg, ar);
                break;

            case EVENT_SET_CFF_TIMER_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnSetComplete(msg, ar);
                break;

            case EVENT_QUERY_CF_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnQueryCfComplete(ar);
                break;

            case EVENT_QUERY_CFUT_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnQueryUncondCfTimerComplete(ar);
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
                // The matching up of this is done in ImsPhone.

                break;

            case EVENT_USSD_CANCEL_COMPLETE:
                mPhone->OnMMIDone(this);
                break;

            case EVENT_SUPP_SVC_QUERY_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnSuppSvcQueryComplete(ar);
                break;

            case EVENT_QUERY_ICB_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnIcbQueryComplete(ar);
                break;

            case EVENT_GET_CLIR_COMPLETE:
                ar = (AsyncResult) (msg.obj);
                OnQueryClirComplete(ar);
                break;

            default:
                break;
        }
    }

    //***** Private instance methods

    private void
    ProcessIcbMmiCodeForUpdate () {
        String dialingNumber = mSia;
        String[] icbNum = NULL;
        Int32 callAction;
        If (dialingNumber != NULL) {
            icbNum = dialingNumber->Split("\\$");
        }
        callAction = CallBarrAction(dialingNumber);

        try {
            mPhone.mCT->GetUtInterface()
            .UpdateCallBarring(ImsUtInterface.CB_BS_MT,
                               callAction,
                               ObtainMessage(EVENT_SET_COMPLETE,this),
                               icbNum);
        } Catch (ImsException e) {
            Rlog->D(LOG_TAG, "Could not get UT handle for updating ICB.");
        }
    }

    private CharSequence GetErrorMessage(AsyncResult ar) {
        return mContext->GetText(R::string::mmiError);
    }

    private CharSequence GetScString() {
        If (mSc != NULL) {
            If (IsServiceCodeCallBarring(mSc)) {
                return mContext->GetText(R::string::BaMmi);
            } else If (IsServiceCodeCallForwarding(mSc)) {
                return mContext->GetText(R::string::CfMmi);
            } else If (mSc->Equals(SC_PWD)) {
                return mContext->GetText(R::string::PwdMmi);
            } else If (mSc->Equals(SC_WAIT)) {
                return mContext->GetText(R::string::CwMmi);
            } else If (mSc->Equals(SC_CLIP)) {
                return mContext->GetText(R::string::ClipMmi);
            } else If (mSc->Equals(SC_CLIR)) {
                return mContext->GetText(R::string::ClirMmi);
            } else If (mSc->Equals(SC_COLP)) {
                return mContext->GetText(R::string::ColpMmi);
            } else If (mSc->Equals(SC_COLR)) {
                return mContext->GetText(R::string::ColrMmi);
            } else If (mSc->Equals(SC_CFUT)) {
                return CfuTimer;
            } else If (mSc->Equals(SC_BS_MT)) {
                return IcbDnMmi;
            } else If (mSc->Equals(SC_BAICa)) {
                return IcbAnonymousMmi;
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
                    sb->Append(mContext->GetText(
                            R::string::passwordIncorrect));
                } else {
                    sb->Append(mContext->GetText(
                            R::string::mmiError));
                }
            } else {
                ImsException error = (ImsException) ar.exception;
                If (error->GetMessage() != NULL) {
                    sb->Append(error->GetMessage());
                } else {
                    sb->Append(GetErrorMessage(ar));
                }
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
        } else If (IsDeactivate()) {
            mState = State.COMPLETE;
            sb->Append(mContext->GetText(
                    R::string::serviceDisabled));
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
        destinations[1] = PhoneNumberUtils->StringFromStringAndTOA(info.number, info.toa);
        destinations[2] = Integer->ToString(info.timeSeconds);

        If (info.reason == CommandsInterface.CF_REASON_UNCONDITIONAL &&
                (info.serviceClass & serviceClassMask)
                        == CommandsInterface.SERVICE_CLASS_VOICE) {
            Boolean cffEnabled = (info.status == 1);
            If (mIccRecords != NULL) {
                mIccRecords->SetVoiceCallForwardingFlag(1, cffEnabled, info.number);
            }
        }

        return TextUtils->Replace(template, sources, destinations);
    }


    private void
    OnQueryCfComplete(AsyncResult ar) {
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");

        If (ar.exception != NULL) {
            mState = State.FAILED;

            If (ar.exception instanceof ImsException) {
                ImsException error = (ImsException) ar.exception;
                If (error->GetMessage() != NULL) {
                    sb->Append(error->GetMessage());
                } else {
                    sb->Append(GetErrorMessage(ar));
                }
            }
            else {
                sb->Append(GetErrorMessage(ar));
            }
        } else {
            CallForwardInfo infos[];

            infos = (CallForwardInfo[]) ar.result;

            If (infos.length == 0) {
                // Assume the default is not active
                sb->Append(mContext->GetText(R::string::serviceDisabled));

                // Set unconditional CFF in SIM to FALSE
                If (mIccRecords != NULL) {
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

    private void OnQueryUncondCfTimerComplete(AsyncResult ar) {
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
                Rlog->D(LOG_TAG, "In infos.length == 0");
                sb->Append(mContext->GetText(R::string::serviceDisabled));
            } else {
                SpannableStringBuilder tb = new SpannableStringBuilder();
                For (Int32 serviceClassMask = 1
                        ; serviceClassMask <= SERVICE_CLASS_MAX
                        ; serviceClassMask <<= 1) {
                    For (Int32 i = 0, s = infos.length; i < s ; i++) {
                        If (( serviceClassMask & infos[i].serviceClass) ==
                                CommandsInterface.SERVICE_CLASS_VOICE &&
                                infos[i].status != 0) {
                            sb->Append("StartTime: " + infos[i].startHour + ":" +
                                    infos[i].startMinute + "\n");
                            sb->Append("EndTime: " + infos[i].endHour + ":" +
                                    infos[i].endMinute + "\n");
                            sb->Append("Service:" + mContext->GetText(
                                    R::string::serviceClassVoice));
                        } else If ((serviceClassMask & infos[i].serviceClass) != 0) {
                            tb->Append(MakeCFTQueryResultMessage(infos[i],
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

    private CharSequence MakeCFTQueryResultMessage(CallForwardInfo info, Int32 serviceClassMask) {
        Rlog->D(LOG_TAG, "makeCFTQueryResultMessage: ");
        CharSequence template;
        String sources[] = {"{0}", "{1}", "{2}"};
        CharSequence destinations[] = new CharSequence[3];
        Boolean needTimeTemplate;

        If (info.status == 0 && IsEmptyOrNull(info.number)) {
            template = mContext->GetText(
                        R::string::cfTemplateNotForwarded);
        } else {
            template = mContext->GetText(
                        R::string::cfTemplateRegistered);
        }
        destinations[0] = ServiceClassToCFString(info.serviceClass & serviceClassMask);
        destinations[1] = PhoneNumberUtils->StringFromStringAndTOA(info.number, info.toa);
        destinations[2] = Integer->ToString(info.timeSeconds);

        return TextUtils->Replace(template, sources, destinations);
    }

    private void OnSuppSvcQueryComplete(AsyncResult ar) {
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");

        If (ar.exception != NULL) {
            mState = State.FAILED;

            If (ar.exception instanceof ImsException) {
                ImsException error = (ImsException) ar.exception;
                If (error->GetMessage() != NULL) {
                    sb->Append(error->GetMessage());
                } else {
                    sb->Append(GetErrorMessage(ar));
                }
            } else {
                sb->Append(GetErrorMessage(ar));
            }
        } else {
            mState = State.FAILED;
            ImsSsInfo ssInfo = NULL;
            If (ar.result instanceof Bundle) {
                Rlog->D(LOG_TAG, "Received CLIP/COLP/COLR Response.");
                // Response for CLIP, COLP and COLR queries.
                Bundle ssInfoResp = (Bundle) ar.result;
                ssInfo = (ImsSsInfo) ssInfoResp->GetParcelable(UT_BUNDLE_KEY_SSINFO);
                If (ssInfo != NULL) {
                    Rlog->D(LOG_TAG, "ImsSsInfo mStatus = " + ssInfo.mStatus);
                    If (ssInfo.mStatus == ImsSsInfo.DISABLED) {
                        sb->Append(mContext->GetText(R::string::serviceDisabled));
                        mState = State.COMPLETE;
                    } else If (ssInfo.mStatus == ImsSsInfo.ENABLED) {
                        sb->Append(mContext->GetText(R::string::serviceEnabled));
                        mState = State.COMPLETE;
                    } else {
                        sb->Append(mContext->GetText(R::string::mmiError));
                    }
                } else {
                    sb->Append(mContext->GetText(R::string::mmiError));
                }

            } else {
                Rlog->D(LOG_TAG, "Received Call Barring Response.");
                // Response for Call Barring queries.
                ImsSsInfo[] arr = (ImsSsInfo[])ar.result;
                // Check if ImsPhone has received call barring
                // enabled for service class voice.
                For (Int32 i = 0, s = arr.length; i < s ; i++) {
                    If(arr[i].mStatus == 1) {
                        sb->Append(mContext->GetText(com.android.internal
                                .R::string::serviceEnabled));
                        mState = State.COMPLETE;
                    } else {
                        sb->Append(mContext->GetText(com.android.internal
                                .R::string::serviceDisabled));
                        mState = State.COMPLETE;
                    }
                }
            }
        }

        mMessage = sb;
        mPhone->OnMMIDone(this);
    }

    private void OnIcbQueryComplete(AsyncResult ar) {
        Rlog->D(LOG_TAG, "onIcbQueryComplete ");
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");

        If (ar.exception != NULL) {
            mState = State.FAILED;

            If (ar.exception instanceof ImsException) {
                ImsException error = (ImsException) ar.exception;
                If (error->GetMessage() != NULL) {
                    sb->Append(error->GetMessage());
                } else {
                    sb->Append(GetErrorMessage(ar));
                }
            } else {
                sb->Append(GetErrorMessage(ar));
            }
        } else {
            mState = State.FAILED;
            ImsSsInfo[] infos = (ImsSsInfo[])ar.result;
            If (infos.length == 0) {
                sb->Append(mContext->GetText(R::string::serviceDisabled));
                mState = State.COMPLETE;
            } else {
                For (Int32 i = 0, s = infos.length; i < s ; i++) {
                    sb->Append("Num: " + infos[i].mIcbNum + " status: "
                            + infos[i].mStatus + "\n");
                }
                mState = State.COMPLETE;
            }
        }
        mMessage = sb;
        mPhone->OnMMIDone(this);
    }

    private void OnQueryClirComplete(AsyncResult ar) {
        StringBuilder sb = new StringBuilder(GetScString());
        sb->Append("\n");
        mState = State.FAILED;

        If (ar.exception != NULL) {

            If (ar.exception instanceof ImsException) {
                ImsException error = (ImsException) ar.exception;
                If (error->GetMessage() != NULL) {
                    sb->Append(error->GetMessage());
                } else {
                    sb->Append(GetErrorMessage(ar));
                }
            }
        } else {
            Bundle ssInfo = (Bundle) ar.result;
            Int32[] clirInfo = ssInfo->GetIntArray(UT_BUNDLE_KEY_CLIR);
            // clirInfo[0] = The 'n' parameter from TS 27.007 7.7
            // clirInfo[1] = The 'm' parameter from TS 27.007 7.7
            Rlog->D(LOG_TAG, "CLIR param n=" + clirInfo[0]
                    + " m=" + clirInfo[1]);

            // 'm' parameter.
            Switch (clirInfo[1]) {
                case CLIR_NOT_PROVISIONED:
                    sb->Append(mContext->GetText(
                            R::string::serviceNotProvisioned));
                    mState = State.COMPLETE;
                    break;
                case CLIR_PROVISIONED_PERMANENT:
                    sb->Append(mContext->GetText(
                            R::string::CLIRPermanent));
                    mState = State.COMPLETE;
                    break;
                case CLIR_PRESENTATION_RESTRICTED_TEMPORARY:
                    // 'n' parameter.
                    Switch (clirInfo[0]) {
                        case CLIR_DEFAULT:
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOnNextCallOn));
                            mState = State.COMPLETE;
                            break;
                        case CLIR_INVOCATION:
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOnNextCallOn));
                            mState = State.COMPLETE;
                            break;
                        case CLIR_SUPPRESSION:
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOnNextCallOff));
                            mState = State.COMPLETE;
                            break;
                        default:
                            sb->Append(mContext->GetText(
                                    R::string::mmiError));
                            mState = State.FAILED;
                    }
                    break;
                case CLIR_PRESENTATION_ALLOWED_TEMPORARY:
                    // 'n' parameter.
                    Switch (clirInfo[0]) {
                        case CLIR_DEFAULT:
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOffNextCallOff));
                            mState = State.COMPLETE;
                            break;
                        case CLIR_INVOCATION:
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOffNextCallOn));
                            mState = State.COMPLETE;
                            break;
                        case CLIR_SUPPRESSION:
                            sb->Append(mContext->GetText(
                                    R::string::CLIRDefaultOffNextCallOff));
                            mState = State.COMPLETE;
                            break;
                        default:
                            sb->Append(mContext->GetText(
                                    R::string::mmiError));
                            mState = State.FAILED;
                    }
                    break;
                default:
                    sb->Append(mContext->GetText(
                            R::string::mmiError));
                    mState = State.FAILED;
            }
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

            If (ar.exception instanceof ImsException) {
                ImsException error = (ImsException) ar.exception;
                If (error->GetMessage() != NULL) {
                    sb->Append(error->GetMessage());
                } else {
                    sb->Append(GetErrorMessage(ar));
                }
            } else {
                sb->Append(GetErrorMessage(ar));
            }

        } else {
            Int32[] ints = (Int32[])ar.result;

            If (ints.length != 0) {
                If (ints[0] == 0) {
                    sb->Append(mContext->GetText(R::string::serviceDisabled));
                } else If (mSc->Equals(SC_WAIT)) {
                    // Call Waiting includes additional data in the response.
                    sb->Append(CreateQueryCallWaitingResultMessage(ints[1]));
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
        StringBuilder sb = new StringBuilder(
                mContext->GetText(R::string::serviceEnabledFor));

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
        StringBuilder sb = new StringBuilder("ImsPhoneMmiCode {");

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
