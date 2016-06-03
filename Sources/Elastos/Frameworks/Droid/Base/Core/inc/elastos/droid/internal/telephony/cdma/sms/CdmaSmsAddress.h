#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSADDRESS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSADDRESS_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.cdma.sms;
using Elastos::Droid::Utility::ISparseBooleanArray;
// import com.android.internal.telephony.cdma.sms.UserData;
// import com.android.internal.util.HexDump;
using Elastos::Droid::Internal::Telephony::ISmsAddress;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

class CdmaSmsAddress
    : public Object
    , public ISmsAddress
{
public:
    CAR_INTERFACE_DECL();

    /**
      * NOTE: the parsed string address and the raw byte array values
      * are stored in the parent class address and origBytes fields,
      * respectively.
      */
    CdmaSmsAddress();

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
      * Given a string, create a corresponding CdmaSmsAddress object.
      *
      * The result will be null if the input string is not
      * representable using printable ASCII.
      *
      * For numeric addresses, the string is cleaned up by removing
      * common punctuation.  For alpha addresses, the string is cleaned
      * up by removing whitespace.
      */
    static CARAPI_(AutoPtr<CdmaSmsAddress>) Parse(
        /* [in] */ const String& address);

private:
    /*
      * TODO(cleanup): Refactor the parsing for addresses to better
      * share code and logic with GSM.  Also, gather all DTMF/BCD
      * processing code in one place.
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ParseToDtmf(
        /* [in] */ const String& address);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) MiddleInitNumericcharsdialable();

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) MiddleInitNumericcharssugar();

    static CARAPI_(AutoPtr<ISparseBooleanArray>) InitSparseBooleanArray();
    /**
      * Given a numeric address string, return the string without
      * syntactic sugar, meaning parens, spaces, hyphens/minuses, or
      * plus signs.  If the input string contains non-numeric
      * non-punctuation characters, return null.
      */
    static CARAPI_(String) FilterNumericSugar(
        /* [in] */ const String& address);

    /**
      * Given a string, return the string without whitespace,
      * including CR/LF.
      */
    static CARAPI_(String) FilterWhitespace(
        /* [in] */ const String& address);

public:
    Int32 digitMode;
    Int32 numberMode;
    /**
      * This field shall be set to the number of address digits
      * (See 3GPP2 C.S0015-B, v2, 3.4.3.3)
      */
    Int32 numberOfDigits;
    //static protected final int NUMBERING_PLAN_DATA              = 0x3;
    //static protected final int NUMBERING_PLAN_TELEX             = 0x4;
    //static protected final int NUMBERING_PLAN_PRIVATE           = 0x9;
    Int32 numberPlan;

private:
    static AutoPtr< ArrayOf<Byte> > numericCharsDialable;
    static AutoPtr< ArrayOf<Byte> > numericCharsSugar;
    static AutoPtr<ISparseBooleanArray> numericCharDialableMap;
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_CDMASMSADDRESS_H__

