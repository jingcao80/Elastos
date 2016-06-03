#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_USERDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_USERDATA_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.android.internal.telephony.cdma.sms;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
// import com.android.internal.util.HexDump;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

class UserData
    : public Object
    , public IUserData
{
public:
    CAR_INTERFACE_DECL();

    /*
      * TODO(cleanup): Move this very generic functionality somewhere
      * more general.
      */
    /**
      * Given a string generate a corresponding ASCII-encoded byte
      * array, but limited to printable characters.  If the input
      * contains unprintable characters, return null.
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) StringToAscii(
        /* [in] */ const String& str);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) MiddleInitAsciiMap();

public:
    /**
      * US ASCII character mapping table.
      *
      * This table contains only the printable ASCII characters, with a
      * 0x20 offset, meaning that the ASCII SPACE character is at index
      * 0, with the resulting code of 0x20.
      *
      * Note this mapping is also equivalent to that used by both the
      * IA5 and the IS-91 encodings.  For the former this is defined
      * using CCITT Rec. T.50 Tables 1 and 3.  For the latter IS 637 B,
      * Table 4.3.1.4.1-1 -- and note the encoding uses only 6 bits,
      * and hence only maps entries up to the '_' character.
      *
      */
    static AutoPtr< ArrayOf<Byte> > ASCII_MAP;
    /**
      * Character to use when forced to encode otherwise unencodable
      * characters, meaning those not in the respective ASCII or GSM
      * 7-bit encoding tables.  Current choice is SPACE, which is 0x20
      * in both the GSM-7bit and ASCII-7bit encodings.
      */
    static const Byte UNENCODABLE_7_BIT_CHAR;
    static const AutoPtr<ISparseInt32Array> charToAscii;
    /**
      * Contains the data header of the user data
      */
    AutoPtr<ISmsHeader> userDataHeader;
    /**
      * Contains the data encoding type for the SMS message
      */
    Int32 msgEncoding;
    Boolean msgEncodingSet;
    Int32 msgType;
    /**
      * Number of invalid bits in the last byte of data.
      */
    Int32 paddingBits;
    Int32 numFields;
    /**
      * Contains the user data of a SMS message
      * (See 3GPP2 C.S0015-B, v2, 4.5.2)
      */
    AutoPtr<ArrayOf<Byte> > payload;
    String payloadStr;
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_USERDATA_H__
