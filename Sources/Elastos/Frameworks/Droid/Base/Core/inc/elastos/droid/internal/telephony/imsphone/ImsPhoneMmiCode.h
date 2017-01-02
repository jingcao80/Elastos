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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEMMICODE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEMMICODE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncResult.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IMmiCodeState;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

/**
  * The motto for this file is:
  *
  * "NOTE:    By using the # as a separator, most cases are expected to be unambiguous."
  *   -- TS 22.030 6.5.2
  *
  * {@hide}
  *
  */
class ImsPhoneMmiCode
    : public Handler
    , public IMmiCode
{
public:
    CAR_INTERFACE_DECL();

    ImsPhoneMmiCode();
    //***** Constructor
    CARAPI constructor(
        /* [in] */ IImsPhone* phone);

    //***** Public Class methods
    /**
      * Some dial strings in GSM are defined to do non-call setup
      * things, such as modify or query supplementary service settings (eg, call
      * forwarding). These are generally referred to as "MMI codes".
      * We look to see if the dial string contains a valid MMI code (potentially
      * with a dial string at the end as well) and return info here.
      *
      * If the dial string contains no MMI code, we return an instance with
      * only "dialingNumber" set
      *
      * Please see flow chart in TS 22.030 6.5.3.2
      */
    static CARAPI_(AutoPtr<IImsPhoneMmiCode>) NewFromDialString(
        /* [in] */ const String& dialString,
        /* [in] */ IImsPhone* phone);

    static CARAPI_(AutoPtr<IImsPhoneMmiCode>) NewNetworkInitiatedUssd(
        /* [in] */ const String& ussdMessage,
        /* [in] */ Boolean isUssdRequest,
        /* [in] */ IImsPhone* phone);

    static CARAPI_(AutoPtr<IImsPhoneMmiCode>) NewFromUssdUserInput(
        /* [in] */ const String& ussdMessage,
        /* [in] */ IImsPhone* phone);

    static CARAPI_(Boolean) IsScMatchesSuppServType(
        /* [in] */ const String& dialString);

    static CARAPI_(Boolean) IsServiceCodeCallForwarding(
        /* [in] */ const String& sc);

    static CARAPI_(Boolean) IsServiceCodeUncondCallFwdTimer(
        /* [in] */ const String& sc);

    static CARAPI_(Boolean) IsServiceCodeCallBarring(
        /* [in] */ const String& sc);

    static CARAPI ScToBarringFacility(
        /* [in] */ const String& sc,
        /* [out] */ String* result);

    //***** MmiCode implementation
    // @Override
    CARAPI GetState(
        /* [out] */ IMmiCodeState* result);

    // @Override
    CARAPI GetMessage(
        /* [out] */ ICharSequence** result);

    // @Override
    CARAPI GetPhone(
        /* [out] */ IPhone** result);

    // inherited javadoc suffices
    // @Override
    CARAPI Cancel();

    // @Override
    CARAPI IsCancelable(
        /* [out] */ Boolean* result);

    //***** Instance Methods
    virtual CARAPI GetDialingNumber(
        /* [out] */ String* result);

    /** Does this dial string contain a structured or unstructured MMI code? */
    virtual CARAPI IsMMI(
        /* [out] */ Boolean* result);

    /* Is this a 1 or 2 digit "short code" as defined in TS 22.030 sec 6.5.3.2? */
    virtual CARAPI IsShortCode(
        /* [out] */ Boolean* result);

    /**
      * @return true if the Service Code is PIN/PIN2/PUK/PUK2-related
      */
    virtual CARAPI IsPinPukCommand(
        /* [out] */ Boolean* result);

    /**
      * See TS 22.030 Annex B.
      * In temporary mode, to suppress CLIR for a single call, enter:
      *      " * 31 # [called number] SEND "
      *  In temporary mode, to invoke CLIR for a single call enter:
      *       " # 31 # [called number] SEND "
      */
    virtual CARAPI IsTemporaryModeCLIR(
        /* [out] */ Boolean* result);

    /**
      * returns CommandsInterface.CLIR_*
      * See also isTemporaryModeCLIR()
      */
    virtual CARAPI GetCLIRMode(
        /* [out] */ Int32* result);

    virtual CARAPI IsActivate(
        /* [out] */ Boolean* result);

    virtual CARAPI IsDeactivate(
        /* [out] */ Boolean* result);

    virtual CARAPI IsInterrogate(
        /* [out] */ Boolean* result);

    virtual CARAPI IsRegister(
        /* [out] */ Boolean* result);

    virtual CARAPI IsErasure(
        /* [out] */ Boolean* result);

    /**
      * Returns true if this is a USSD code that's been submitted to the
      * network...eg, after processCode() is called
      */
    virtual CARAPI IsPendingUSSD(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsUssdRequest(
        /* [out] */ Boolean* result);

    virtual CARAPI IsSupportedOverImsPhone(
        /* [out] */ Boolean* result);

    virtual CARAPI CalForwardAction(
        /* [in] */ const String& dialingNumber,
        /* [out] */ Int32* result);

    /*
      * The below actions are IMS/Volte CallBarring actions.We have not defined
      * these actions in ImscommandInterface.However we have reused existing
      * actions of CallForwarding as, both CF and CB actions are used for same
      * purpose.
      */
    virtual CARAPI CallBarrAction(
        /* [in] */ const String& dialingNumber,
        /* [out] */ Int32* result);

    /** Process a MMI code or short code...anything that isn't a dialing number */
    virtual CARAPI ProcessCode();

    /**
      * Called from ImsPhone
      *
      * An unsolicited USSD NOTIFY or REQUEST has come in matching
      * up with this pending USSD request
      *
      * Note: If REQUEST, this exchange is complete, but the session remains
      *       active (ie, the network expects user input).
      */
    virtual CARAPI OnUssdFinished(
        /* [in] */ const String& ussdMessage,
        /* [in] */ Boolean isUssdRequest);

    /**
      * Called from ImsPhone
      *
      * The radio has reset, and this is still pending
      */
    virtual CARAPI OnUssdFinishedError();

    virtual CARAPI SendUssd(
        /* [in] */ const String& ussdMessage);

    /** Called from ImsPhone.handleMessage; not a Handler subclass */
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /***
      * TODO: It would be nice to have a method here that can take in a dialstring and
      * figure out if there is an MMI code embedded within it.  This code would replace
      * some of the string parsing functionality in the Phone App's
      * SpecialCharSequenceMgr class.
      */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    //***** Private Class methods
    /** make empty strings be null.
      *  Regexp returns empty strings for empty groups
      */
    static CARAPI_(String) MakeEmptyNull(
        /* [in] */ const String& s);

    /** returns true of the string is empty or null */
    static CARAPI_(Boolean) IsEmptyOrNull(
        /* [in] */ ICharSequence* s);

    static CARAPI ScToCallForwardReason(
        /* [in] */ const String& sc,
        /* [out] */ Int32* result);

    static CARAPI SiToServiceClass(
        /* [in] */ const String& si,
        /* [out] */ Int32* result);

    static CARAPI_(Int32) SiToTime(
        /* [in] */ const String& si);

    static CARAPI_(Boolean) IsTwoDigitShortCode(
        /* [in] */ IContext* context,
        /* [in] */ const String& dialString);

    /**
      * Helper function for newFromDialString. Returns true if dialString appears
      * to be a short code AND conditions are correct for it to be treated as
      * such.
      */
    static CARAPI_(Boolean) IsShortCode(
        /* [in] */ const String& dialString,
        /* [in] */ IImsPhone* phone);

    /**
      * Helper function for isShortCode. Returns true if dialString appears to be
      * a short code and it is a USSD structure
      *
      * According to the 3PGG TS 22.030 specification Figure 3.5.3.2: A 1 or 2
      * digit "short code" is treated as USSD if it is entered while on a call or
      * does not satisfy the condition (exactly 2 digits && starts with '1'), there
      * are however exceptions to this rule (see below)
      *
      * Exception (1) to Call initiation is: If the user of the device is already in a call
      * and enters a Short String without any #-key at the end and the length of the Short String is
      * equal or less then the MAX_LENGTH_SHORT_CODE [constant that is equal to 2]
      *
      * The phone shall initiate a USSD/SS commands.
      */
    static CARAPI_(Boolean) IsShortCodeUSSD(
        /* [in] */ const String& dialString,
        /* [in] */ IImsPhone* phone);

    //***** Private instance methods
    CARAPI_(void) ProcessIcbMmiCodeForUpdate();

    CARAPI_(AutoPtr<ICharSequence>) GetErrorMessage(
        /* [in] */ AsyncResult* ar);

    CARAPI_(AutoPtr<ICharSequence>) GetScString();

    CARAPI_(void) OnSetComplete(
        /* [in] */ IMessage* msg,
        /* [in] */ AsyncResult* ar);

    /**
      * @param serviceClass 1 bit of the service class bit vectory
      * @return String to be used for call forward query MMI response text.
      *        Returns null if unrecognized
      */
    CARAPI_(AutoPtr<ICharSequence>) ServiceClassToCFString(
        /* [in] */ Int32 serviceClass);

    /** one CallForwardInfo + serviceClassMask -> one line of text */
    CARAPI_(AutoPtr<ICharSequence>) MakeCFQueryResultMessage(
        /* [in] */ ICallForwardInfo* info,
        /* [in] */ Int32 serviceClassMask);

    CARAPI_(void) OnQueryCfComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnQueryUncondCfTimerComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(AutoPtr<ICharSequence>) MakeCFTQueryResultMessage(
        /* [in] */ ICallForwardInfo* info,
        /* [in] */ Int32 serviceClassMask);

    CARAPI_(void) OnSuppSvcQueryComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnIcbQueryComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnQueryClirComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnQueryComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(AutoPtr<ICharSequence>) CreateQueryCallWaitingResultMessage(
        /* [in] */ Int32 serviceClass);

public:
    static const String LOGTAG;
    //***** Supplementary Service Query Bundle Keys
    // Used by IMS Service layer to put supp. serv. query
    // responses into the ssInfo Bundle.
    static const String UT_BUNDLE_KEY_CLIR;
    static const String UT_BUNDLE_KEY_SSINFO;
    //Displayed as the title for Call Forward Timer
    static const String CfuTimer;
    //resgister/erasure of ICB (Specific DN)
    static const String IcbDnMmi;
    //ICB (Anonymous)
    static const String IcbAnonymousMmi;

private:
    //***** Constants
    // Max Size of the Short Code (aka Short String from TS 22.030 6.5.2)
    static const Int32 MAX_LENGTH_SHORT_CODE = 2;
    // TS 22.030 6.5.2 Every Short String USSD command will end with #-key
    // (known as #-String)
    static const Char16 END_OF_USSD_COMMAND;
    // From TS 22.030 6.5.2
    static const String ACTION_ACTIVATE;
    static const String ACTION_DEACTIVATE;
    static const String ACTION_INTERROGATE;
    static const String ACTION_REGISTER;
    static const String ACTION_ERASURE;
    // Supp Service codes from TS 22.030 Annex B
    //Called line presentation
    static const String SC_CLIP;
    static const String SC_CLIR;
    static const String SC_COLP;
    static const String SC_COLR;
    //Calling name presentation
    static const String SC_CNAP;
    // Call Forwarding
    static const String SC_CFU;
    static const String SC_CFB;
    static const String SC_CFNRy;
    static const String SC_CFNR;
    // Call Forwarding unconditional Timer
    static const String SC_CFUT;
    static const String SC_CF_All;
    static const String SC_CF_All_Conditional;
    // Call Waiting
    static const String SC_WAIT;
    // Call Barring
    static const String SC_BAOC;
    static const String SC_BAOIC;
    static const String SC_BAOICxH;
    static const String SC_BAIC;
    static const String SC_BAICr;
    static const String SC_BA_ALL;
    static const String SC_BA_MO;
    static const String SC_BA_MT;
    // Incoming/Anonymous call barring
    static const String SC_BS_MT;
    static const String SC_BAICa;
    // Supp Service Password registration
    static const String SC_PWD;
    // PIN/PIN2/PUK/PUK2
    static const String SC_PIN;
    static const String SC_PIN2;
    static const String SC_PUK;
    static const String SC_PUK2;
    //***** Event Constants
    static const Int32 EVENT_SET_COMPLETE = 0;
    static const Int32 EVENT_QUERY_CF_COMPLETE = 1;
    static const Int32 EVENT_USSD_COMPLETE = 2;
    static const Int32 EVENT_QUERY_COMPLETE = 3;
    static const Int32 EVENT_SET_CFF_COMPLETE = 4;
    static const Int32 EVENT_USSD_CANCEL_COMPLETE = 5;
    static const Int32 EVENT_GET_CLIR_COMPLETE = 6;
    static const Int32 EVENT_SUPP_SVC_QUERY_COMPLETE = 7;
    static const Int32 EVENT_QUERY_CFUT_COMPLETE = 8;
    static const Int32 EVENT_SET_CFF_TIMER_COMPLETE = 9;
    static const Int32 EVENT_QUERY_ICB_COMPLETE = 10;
    //***** Calling Line Presentation Constants
    static const Int32 NUM_PRESENTATION_ALLOWED = 0;
    static const Int32 NUM_PRESENTATION_RESTRICTED = 1;
    //***** Calling Line Identity Restriction Constants
    // The 'm' parameter from TS 27.007 7.7
    static const Int32 CLIR_NOT_PROVISIONED = 0;
    static const Int32 CLIR_PROVISIONED_PERMANENT = 1;
    static const Int32 CLIR_PRESENTATION_RESTRICTED_TEMPORARY = 3;
    static const Int32 CLIR_PRESENTATION_ALLOWED_TEMPORARY = 4;
    // The 'n' parameter from TS 27.007 7.7
    static const Int32 CLIR_DEFAULT = 0;
    static const Int32 CLIR_INVOCATION = 1;
    static const Int32 CLIR_SUPPRESSION = 2;
    //***** Instance Variables
    AutoPtr<IImsPhone> mPhone;
    AutoPtr<IContext> mContext;
    AutoPtr<IIccRecords> mIccRecords;
    String mAction;
    // One of ACTION_*
    String mSc;
    // Service Code
    String mSia;
    String mSib;
    String mSic;
    // Service Info a,b,c
    String mPoundString;
    // Entire MMI string up to and including #
    String mDialingNumber;
    String mPwd;
    // For password registration
    Boolean mIsPendingUSSD;
    Boolean mIsUssdRequest;
    Boolean mIsCallFwdReg;
    IMmiCodeState mState;
    AutoPtr<ICharSequence> mMessage;
    //***** Class Variables
    // See TS 22.030 6.5.2 "Structure of the MMI"
    static AutoPtr<IPattern> sPatternSuppService;
    /*       1  2                    3          4  5       6   7         8    9     10  11             12

          1 = Full string up to and including #
          2 = action (activation/interrogation/registration/erasure)
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
    static AutoPtr<ArrayOf<String> > sTwoDigitNumberPattern;
};

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_IMSPHONE_IMSPHONEMMICODE_H__

