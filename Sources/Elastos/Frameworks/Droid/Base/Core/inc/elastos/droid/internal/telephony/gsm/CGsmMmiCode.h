
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMMMICODE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMMMICODE_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmMmiCode.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Droid::Internal::Telephony::IMmiCode;
using Elastos::Droid::Internal::Telephony::IMmiCodeState;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::Gsm::RequestType;
using Elastos::Droid::Internal::Telephony::Gsm::ServiceType;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * The motto for this file is:
 *
 * "NOTE:    By using the # as a separator, most cases are expected to be unambiguous."
 *   -- TS 22.030 6.5.2
 *
 * {@hide}
 *
 */
CarClass(CGsmMmiCode)
    , public Handler
    , public IGsmMmiCode
    , public IMmiCode
{
public:
    CGsmMmiCode();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IGSMPhone* phone,
        /* [in] */ IUiccCardApplication* app);

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

    static CARAPI NewFromDialString(
        /* [in] */ const String& dialString,
        /* [in] */ IGSMPhone* phone,
        /* [in] */ IUiccCardApplication* app,
        /* [out] */ IGsmMmiCode** result);

    static CARAPI NewNetworkInitiatedUssd(
        /* [in] */ const String& ussdMessage,
        /* [in] */ Boolean isUssdRequest,
        /* [in] */ IGSMPhone* phone,
        /* [in] */ IUiccCardApplication* app,
        /* [out] */ IGsmMmiCode** result);

    static CARAPI NewFromUssdUserInput(
        /* [in] */ const String& ussdMessage,
        /* [in] */ IGSMPhone* phone,
        /* [in] */ IUiccCardApplication* app,
        /* [out] */ IGsmMmiCode** result);

    /** Process SS Data */
    CARAPI ProcessSsData(
        /* [in] */ AsyncResult* data);

    CARAPI ParseSsData(
        /* [in] */ ISsData* ssData);

    static CARAPI IsServiceCodeCallForwarding(
        /* [in] */ const String& sc,
        /* [out] */ Boolean* result);

    static CARAPI IsServiceCodeCallBarring(
        /* [in] */ const String& sc,
        /* [out] */ Boolean* result);

    static CARAPI ScToBarringFacility(
        /* [in] */ const String& sc,
        /* [out] */ String* result);

    CARAPI GetState(
        /* [out] */ IMmiCodeState* result);

    CARAPI GetMessage(
        /* [out] */ ICharSequence** result);

    CARAPI GetPhone(
        /* [out] */ IPhone** result);

    CARAPI Cancel();

    CARAPI IsCancelable(
        /* [out] */ Boolean* result);

    /** Does this dial string contain a structured or unstructured MMI code? */
    CARAPI IsMMI(
        /* [out] */ Boolean* result);

    /* Is this a 1 or 2 digit "short code" as defined in TS 22.030 sec 6.5.3.2? */
    CARAPI IsShortCode(
        /* [out] */ Boolean* result);

    /**
     * @return true if the Service Code is PIN/PIN2/PUK/PUK2-related
     */
    CARAPI IsPinPukCommand(
        /* [out] */ Boolean* result);

    /**
     * See TS 22.030 Annex B.
     * In temporary mode, to suppress CLIR for a single call, enter:
     *      " * 31 # [called number] SEND "
     *  In temporary mode, to invoke CLIR for a single call enter:
     *       " # 31 # [called number] SEND "
     */
    CARAPI IsTemporaryModeCLIR(
        /* [out] */ Boolean* result);

    /**
     * returns CommandsInterface.CLIR_*
     * See also isTemporaryModeCLIR()
     */
    CARAPI GetCLIRMode(
        /* [out] */ Int32* result);

    CARAPI IsActivate(
        /* [out] */ Boolean* result);

    CARAPI IsDeactivate(
        /* [out] */ Boolean* result);

    CARAPI IsInterrogate(
        /* [out] */ Boolean* result);

    CARAPI IsRegister(
        /* [out] */ Boolean* result);

    CARAPI IsErasure(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this is a USSD code that's been submitted to the
     * network...eg, after processCode() is called
     */
    CARAPI IsPendingUSSD(
        /* [out] */ Boolean* result);

    CARAPI IsUssdRequest(
        /* [out] */ Boolean* result);

    CARAPI IsSsInfo(
        /* [out] */ Boolean* result);

    /** Process a MMI code or short code...anything that isn't a dialing number */
    CARAPI ProcessCode();

    /**
     * Called from GSMPhone
     *
     * An unsolicited USSD NOTIFY or REQUEST has come in matching
     * up with this pending USSD request
     *
     * Note: If REQUEST, this exchange is complete, but the session remains
     *       active (ie, the network expects user input).
     */
    CARAPI OnUssdFinished(
        /* [in] */ const String& ussdMessage,
        /* [in] */ Boolean isUssdRequest);

    /**
     * Called from GSMPhone
     *
     * The radio has reset, and this is still pending
     */
    CARAPI OnUssdFinishedError();

    CARAPI SendUssd(
        /* [in] */ const String& ussdMessage);

    /** Called from GSMPhone.handleMessage; not a Handler subclass */
    CARAPI HandleMessage (
        /* [in] */ IMessage* msg);

    /***
     * TODO: It would be nice to have a method here that can take in a dialstring and
     * figure out if there is an MMI code embedded within it.  This code would replace
     * some of the string parsing functionality in the Phone App's
     * SpecialCharSequenceMgr class.
     */
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(String) GetScStringFromScType(
        /* [in] */ ServiceType sType);

    CARAPI_(String) GetActionStringFromReqType(
        /* [in] */ RequestType rType);

    CARAPI_(Boolean) IsServiceClassVoiceorNone(
        /* [in] */ Int32 serviceClass);

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
        /* [in] */ IGSMPhone* phone);

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
        /* [in] */ IGSMPhone* phone);

    CARAPI_(void) HandlePasswordError(
        /* [in] */ Int32 res);

    CARAPI_(AutoPtr<ICharSequence>) GetErrorMessage(
        /* [in] */ AsyncResult* ar);

    CARAPI_(AutoPtr<ICharSequence>) GetScString();

    CARAPI_(void) OnSetComplete(
        /* [in] */ IMessage* msg,
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnGetClirComplete(
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

    /**
     * Used to format a string that should be displayed as LTR even in RTL locales
     */
    CARAPI_(String) FormatLtr(
        /* [in] */ const String& str);

    CARAPI_(void) OnQueryCfComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnQueryComplete(
        /* [in] */ AsyncResult* ar);

    CARAPI_(AutoPtr<ICharSequence>) CreateQueryCallWaitingResultMessage(
        /* [in] */ Int32 serviceClass);

    CARAPI_(AutoPtr<ICharSequence>) CreateQueryCallBarringResultMessage(
        /* [in] */ Int32 serviceClass);

public:
    AutoPtr<IGSMPhone> mPhone;
    AutoPtr<IContext> mContext;
    AutoPtr<IUiccCardApplication> mUiccApplication;
    AutoPtr<IIccRecords> mIccRecords;

    String mAction;              // One of ACTION_*
    String mSc;                  // Service Code
    String mSia, mSib, mSic;       // Service Info a,b,c
    String mPoundString;         // Entire MMI string up to and including #
    String mDialingNumber;
    String mPwd;                 // For password registration

    IMmiCodeState mState; // = State.PENDING;
    AutoPtr<ICharSequence> mMessage;

    static AutoPtr<IPattern> sPatternSuppService; // = Pattern.compile(
        // "((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)");
/*       1  2                    3          4  5       6   7         8    9     10  11             12

         1 = Full string up to and including #
         2 = action (activation/interrogation/registration/erasure)
         3 = service code
         5 = SIA
         7 = SIB
         9 = SIC
         10 = dialing number
*/

private:
    /** Set to true in processCode, not at newFromDialString time */
    Boolean mIsPendingUSSD;

    Boolean mIsUssdRequest;

    Boolean mIsCallFwdReg;
    Boolean mIsSsInfo; // = false;

    static AutoPtr<ArrayOf<String> > sTwoDigitNumberPattern;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMMMICODE_H__
