#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CHARACTERSETS_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CHARACTERSETS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.google.android.mms.pdu;
// import java.io.UnsupportedEncodingException;
// import java.util.HashMap;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class CharacterSets
    : public Object
{
public:
    // Non-instantiatable
    /**
      * Map an MIBEnum number to the name of the charset which this number
      * is assigned to by IANA.
      *
      * @param mibEnumValue An IANA assigned MIBEnum number.
      * @return The name string of the charset.
      * @throws UnsupportedEncodingException
      */
    static CARAPI_(String) GetMimeName(
        /* [in] */ Int32 mibEnumValue);

    /**
      * Map a well-known charset name to its assigned MIBEnum number.
      *
      * @param mimeName The charset name.
      * @return The MIBEnum number assigned by IANA for this charset.
      * @throws UnsupportedEncodingException
      */
    static CARAPI_(Int32) GetMibEnumValue(
        /* [in] */ const String& mimeName);

private:
    CharacterSets();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) MiddleInitMibenumNumbers();

    static CARAPI_(AutoPtr<ArrayOf<String> >) MiddleInitMimeNames();

public:
    /**
      * IANA assigned MIB enum numbers.
      *
      * From wap-230-wsp-20010705-a.pdf
      * Any-charset = <Octet 128>
      * Equivalent to the special RFC2616 charset value "*"
      */
    static const Int32 ANY_CHARSET = 0x00;
    static const Int32 US_ASCII = 0x03;
    static const Int32 ISO_8859_1 = 0x04;
    static const Int32 ISO_8859_2 = 0x05;
    static const Int32 ISO_8859_3 = 0x06;
    static const Int32 ISO_8859_4 = 0x07;
    static const Int32 ISO_8859_5 = 0x08;
    static const Int32 ISO_8859_6 = 0x09;
    static const Int32 ISO_8859_7 = 0x0A;
    static const Int32 ISO_8859_8 = 0x0B;
    static const Int32 ISO_8859_9 = 0x0C;
    static const Int32 SHIFT_JIS = 0x11;
    static const Int32 UTF_8 = 0x6A;
    static const Int32 BIG5 = 0x07EA;
    static const Int32 UCS2 = 0x03E8;
    static const Int32 UTF_16 = 0x03F7;
    /**
      * If the encoding of given data is unsupported, use UTF_8 to decode it.
      */
    static const Int32 DEFAULT_CHARSET = UTF_8;
    /**
      * The Well-known-charset Mime name.
      */
    static const String MIMENAME_ANY_CHARSET;
    static const String MIMENAME_US_ASCII;
    static const String MIMENAME_ISO_8859_1;
    static const String MIMENAME_ISO_8859_2;
    static const String MIMENAME_ISO_8859_3;
    static const String MIMENAME_ISO_8859_4;
    static const String MIMENAME_ISO_8859_5;
    static const String MIMENAME_ISO_8859_6;
    static const String MIMENAME_ISO_8859_7;
    static const String MIMENAME_ISO_8859_8;
    static const String MIMENAME_ISO_8859_9;
    static const String MIMENAME_SHIFT_JIS;
    static const String MIMENAME_UTF_8;
    static const String MIMENAME_BIG5;
    static const String MIMENAME_UCS2;
    static const String MIMENAME_UTF_16;
    static const String DEFAULT_CHARSET_NAME;

private:
    /**
      * Array of MIB enum numbers.
      */
    static AutoPtr<ArrayOf<Int32> > MIBENUM_NUMBERS;
    /**
      * Array of the names of character sets.
      */
    static AutoPtr< ArrayOf<String> > MIME_NAMES;
    static AutoPtr<IHashMap> MIBENUM_TO_NAME_MAP; // Integer, String
    static AutoPtr<IHashMap> NAME_TO_MIBENUM_MAP; // String, Integer
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CHARACTERSETS_H__

