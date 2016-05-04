/*
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

package com.android.internal.telephony.cdma;

using Elastos::Droid::Content::IContext;

using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::IPhone;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IMatcher;

/**
 * This class can handle Puk code Mmi
 *
 * {@hide}
 *
 */
public class CdmaMmiCode  extends Handler implements MmiCode {
    static const String LOG_TAG = "CdmaMmiCode";

    // Constants

    // From TS 22.030 6.5.2
    static const String ACTION_REGISTER = "**";

    // Supplementary Service codes for PIN/PIN2/PUK/PUK2 from TS 22.030 Annex B
    static const String SC_PIN          = "04";
    static const String SC_PIN2         = "042";
    static const String SC_PUK          = "05";
    static const String SC_PUK2         = "052";

    // Event Constant

    static const Int32 EVENT_SET_COMPLETE = 1;

    // Instance Variables

    CDMAPhone mPhone;
    Context mContext;
    UiccCardApplication mUiccApplication;

    String mAction;              // ACTION_REGISTER
    String mSc;                  // Service Code
    String mSia, mSib, mSic;     // Service Info a,b,c
    String mPoundString;         // Entire MMI string up to and including #
    String mDialingNumber;
    String mPwd;                 // For password registration

    State mState = State.PENDING;
    CharSequence mMessage;

    // Class Variables

    static Pattern sPatternSuppService = Pattern->Compile(
        "((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)");
/*       1  2                    3          4  5       6   7         8    9     10  11             12

         1 = Full string up to and including #
         2 = action
         3 = service code
         5 = SIA
         7 = SIB
         9 = SIC
         10 = dialing number
*/

    static const Int32 MATCH_GROUP_POUND_STRING = 1;
    static const Int32 MATCH_GROUP_ACTION = 2;
    static const Int32 MATCH_GROUP_SERVICE_CODE = 3;
    static const Int32 MATCH_GROUP_SIA = 5;
    static const Int32 MATCH_GROUP_SIB = 7;
    static const Int32 MATCH_GROUP_SIC = 9;
    static const Int32 MATCH_GROUP_PWD_CONFIRM = 11;
    static const Int32 MATCH_GROUP_DIALING_NUMBER = 12;


    // Public Class methods

    /**
     * Check if provided string contains Mmi code in it and create corresponding
     * Mmi if it does
     */

    public static CdmaMmiCode
    NewFromDialString(String dialString, CDMAPhone phone, UiccCardApplication app) {
        Matcher m;
        CdmaMmiCode ret = NULL;

        m = sPatternSuppService->Matcher(dialString);

        // Is this formatted like a standard supplementary service code?
        If (m->Matches()) {
            ret = new CdmaMmiCode(phone,app);
            ret.mPoundString = MakeEmptyNull(m->Group(MATCH_GROUP_POUND_STRING));
            ret.mAction = MakeEmptyNull(m->Group(MATCH_GROUP_ACTION));
            ret.mSc = MakeEmptyNull(m->Group(MATCH_GROUP_SERVICE_CODE));
            ret.mSia = MakeEmptyNull(m->Group(MATCH_GROUP_SIA));
            ret.mSib = MakeEmptyNull(m->Group(MATCH_GROUP_SIB));
            ret.mSic = MakeEmptyNull(m->Group(MATCH_GROUP_SIC));
            ret.mPwd = MakeEmptyNull(m->Group(MATCH_GROUP_PWD_CONFIRM));
            ret.mDialingNumber = MakeEmptyNull(m->Group(MATCH_GROUP_DIALING_NUMBER));

        }

        return ret;
    }

    // Private Class methods

    /** make empty strings be NULL.
     *  Regexp returns empty strings for empty groups
     */
    private static String
    MakeEmptyNull (String s) {
        If (s != NULL && s->Length() == 0) return NULL;

        return s;
    }

    // Constructor

    CdmaMmiCode (CDMAPhone phone, UiccCardApplication app) {
        Super(phone->GetHandler()->GetLooper());
        mPhone = phone;
        mContext = phone->GetContext();
        mUiccApplication = app;
    }

    // MmiCode implementation

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
        mPhone.OnMMIDone (this);
    }

    //@Override
    public Boolean IsCancelable() {
        return FALSE;
    }

    // Instance Methods

    /**
     * @return TRUE if the Service Code is PIN/PIN2/PUK/PUK2-related
     */
    Boolean IsPinPukCommand() {
        return mSc != NULL && (mSc->Equals(SC_PIN) || mSc->Equals(SC_PIN2)
                              || mSc->Equals(SC_PUK) || mSc->Equals(SC_PUK2));
    }

    Boolean IsRegister() {
        return mAction != NULL && mAction->Equals(ACTION_REGISTER);
    }

    //@Override
    public Boolean IsUssdRequest() {
        Rlog->W(LOG_TAG, "isUssdRequest is not implemented in CdmaMmiCode");
        return FALSE;
    }

    /** Process a MMI PUK code */
    void
    ProcessCode() {
        try {
            If (IsPinPukCommand()) {
                // TODO: This is the same as the code in GsmMmiCode.java,
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
                            throw new RuntimeException("Unsupported service code=" + mSc);
                        }
                    } else {
                        throw new RuntimeException("No application mUiccApplicaiton is NULL");
                    }
                } else {
                    throw new RuntimeException ("Ivalid register/action=" + mAction);
                }
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

    //@Override
    CARAPI
    HandleMessage (Message msg) {
        AsyncResult ar;

        If (msg.what == EVENT_SET_COMPLETE) {
            ar = (AsyncResult) (msg.obj);
            OnSetComplete(msg, ar);
        } else {
            Rlog->E(LOG_TAG, "Unexpected reply");
        }
    }
    // Private instance methods

    private CharSequence GetScString() {
        If (mSc != NULL) {
            If (IsPinPukCommand()) {
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
                } else {
                    sb->Append(mContext->GetText(
                            R::string::mmiError));
                }
            } else {
                sb->Append(mContext->GetText(
                        R::string::mmiError));
            }
        } else If (IsRegister()) {
            mState = State.COMPLETE;
            sb->Append(mContext->GetText(
                    R::string::serviceRegistered));
        } else {
            mState = State.FAILED;
            sb->Append(mContext->GetText(
                    R::string::mmiError));
        }

        mMessage = sb;
        mPhone->OnMMIDone(this);
    }

}
