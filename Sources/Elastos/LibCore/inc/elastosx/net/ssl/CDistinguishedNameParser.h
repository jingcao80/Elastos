
#ifndef __ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORY_H__
#define __ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORY_H__

#include "_Elastosx_Net_Ssl_CDistinguishedNameParser.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * A distinguished name (DN) parser. This parser only supports extracting a
 * string value from a DN. It doesn't support values in the hex-string style.
 *
 * @hide
 */
CarClass(CDistinguishedNameParser)
    , public Object
    , public IDistinguishedNameParser
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IX500Principal* principal);

    CARAPI FindMostSpecific(
        /* [in] */ const String& attributeType,
        /* [out] */ String* specific);

    CARAPI GetAllMostSpecificFirst(
        /* [in] */ const String& attributeType,
        /* [out] */ IList** frist);

private:
    // gets next attribute type: (ALPHA 1*keychar) / oid
    CARAPI NextAT(
       /* [out] */ String* at);

    // gets quoted attribute value: QUOTATION *( quotechar / pair ) QUOTATION
    CARAPI QuotedAV(
        /* [out] */ String* av);

    // gets hex string attribute value: "#" hexstring
    CARAPI HexAV(
        /* [out] */ String* av);

    // gets string attribute value: *( stringchar / pair )
    CARAPI EscapedAV(
        /* [out] */ String* av);

    // returns escaped char
    CARAPI GetEscaped(
        /* [out] */ Char32* escaped);

    // decodes UTF-8 char
    // see http://www.unicode.org for UTF-8 bit distribution table
    CARAPI GetUTF8(
        /* [out] */ Char32* utf8);

    // Returns byte representation of a char pair
    // The char pair is composed of DN char in
    // specified 'position' and the next char
    // According to BNF syntax:
    // hexchar    = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    //                    / "a" / "b" / "c" / "d" / "e" / "f"
    CARAPI GetByte(
        /* [in] */ Int32 position,
        /* [out] */ Int32* _byte);
private:
    /*const*/ String mDn;
    /*const*/ Int32 mLength;
    Int32 mPos;
    Int32 mBeg;
    Int32 mEnd;

    /** tmp vars to store positions of the currently parsed item */
    Int32 mCur;

    /** distinguished name chars */
    AutoPtr<ArrayOf<Char32> > mChars;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORY_H__
