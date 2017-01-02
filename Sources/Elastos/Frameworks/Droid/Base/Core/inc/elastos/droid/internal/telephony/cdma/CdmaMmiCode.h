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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAMMICODE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAMMICODE_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"

// import com.android.internal.telephony.CommandException;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;
// import com.android.internal.telephony.MmiCode;
// import android.telephony.Rlog;
// import java.util.regex.Matcher;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IMmiCodeState;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * This class can handle Puk code Mmi
  *
  * {@hide}
  *
  */
class CdmaMmiCode
    : public Handler
    , public IMmiCode
    , public ICdmaMmiCode
{
public:
    TO_STRING_IMPL("CdmaMmiCode")

    CAR_INTERFACE_DECL();

    CdmaMmiCode();
    // Constructor
    CARAPI constructor(
        /* [in] */ ICDMAPhone* phone,
        /* [in] */ IUiccCardApplication* app);

    // Public Class methods
    /**
      * Check if provided string contains Mmi code in it and create corresponding
      * Mmi if it does
      */
    static CARAPI_(AutoPtr<ICdmaMmiCode>) NewFromDialString(
        /* [in] */ const String& dialString,
        /* [in] */ ICDMAPhone* phone,
        /* [in] */ IUiccCardApplication* app);

    // MmiCode implementation
    // @Override
    CARAPI GetState(
        /* [out] */ IMmiCodeState* result);

    // @Override
    CARAPI GetMessage(
        /* [out] */ ICharSequence** result);

    virtual CARAPI GetPhone(
        /* [out] */ IPhone** result);

    // inherited javadoc suffices
    // @Override
    CARAPI Cancel();

    // @Override
    CARAPI IsCancelable(
        /* [out] */ Boolean* resutl);

    // Instance Methods
    /**
      * @return true if the Service Code is PIN/PIN2/PUK/PUK2-related
      */
    virtual CARAPI IsPinPukCommand(
        /* [out] */ Boolean* result);

    virtual CARAPI IsRegister(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsUssdRequest(
        /* [out] */ Boolean* resutl);

    /** Process a MMI PUK code */
    virtual CARAPI ProcessCode();

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

private:
    // Private Class methods
    /** make empty strings be null.
      *  Regexp returns empty strings for empty groups
      */
    static CARAPI_(String) MakeEmptyNull(
        /* [in] */ const String& s);

    CARAPI_(void) HandlePasswordError(
        /* [in] */ Int32 res);

    // Private instance methods
    CARAPI_(AutoPtr<ICharSequence>) GetScString();

    CARAPI_(void) OnSetComplete(
        /* [in] */ IMessage* msg,
        /* [in] */ AsyncResult* ar);

public:
    static const String LOGTAG;
    // Constants
    // From TS 22.030 6.5.2
    static const String ACTION_REGISTER;
    // Supplementary Service codes for PIN/PIN2/PUK/PUK2 from TS 22.030 Annex B
    static const String SC_PIN;
    static const String SC_PIN2;
    static const String SC_PUK;
    static const String SC_PUK2;
    // Event Constant
    static const Int32 EVENT_SET_COMPLETE = 1;
    // Instance Variables
    AutoPtr<ICDMAPhone> mPhone;
    AutoPtr<IContext> mContext;
    AutoPtr<IUiccCardApplication> mUiccApplication;
    String mAction;
    // ACTION_REGISTER
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
    IMmiCodeState mState;
    AutoPtr<ICharSequence> mMessage;
    // Class Variables
    static AutoPtr<IPattern> sPatternSuppService;// = Pattern.compile("((\\*|#|\\*#|\\*\\*|##)(\\d{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)(.*)");
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
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMAMMICODE_H__
