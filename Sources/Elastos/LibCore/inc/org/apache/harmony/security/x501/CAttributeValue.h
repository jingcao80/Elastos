
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTEVALUE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTEVALUE_H__

#include "_Org_Apache_Harmony_Security_X501_CAttributeValue.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CarClass(CAttributeValue)
{
public:
    CARAPI GetWasEncoded(
        /* [out] */ Boolean * pWasEncoded);

    CARAPI SetWasEncoded(
        /* [in] */ Boolean wasEncoded);

    CARAPI GetEscapedString(
        /* [out] */ String * pWasEncoded);

    CARAPI SetEscapedString(
        /* [in] */ const String& wasEncoded);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI SetEncoded(
        /* [in] */ ArrayOf<Byte> * pEncoded);

    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte> ** ppBytes);

    CARAPI SetBytes(
        /* [in] */ ArrayOf<Byte> * pBytes);

    CARAPI GetHasQE(
        /* [out] */ Boolean * pHasQE);

    CARAPI SetHasQE(
        /* [in] */ Boolean hasQE);

    CARAPI GetRawString(
        /* [out] */ String * pRawString);

    CARAPI SetRawString(
        /* [in] */ const String& rawString);

    CARAPI GetTag(
        /* [out] */ Int32 * pTag);

    CARAPI GetHexString(
        /* [out] */ String * pHexString);

    CARAPI AppendQEString(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI MakeCanonical(
        /* [out] */ String * pCannonical);

    CARAPI constructor(
        /* [in] */ const String& parsedString,
        /* [in] */ Boolean hasQorE);

    CARAPI constructor(
        /* [in] */ const String& hexString,
        /* [in] */ ArrayOf<Byte> * pEncoded);

    CARAPI constructor(
        /* [in] */ const String& rawString,
        /* [in] */ ArrayOf<Byte> * pEncoded,
        /* [in] */ Int32 tag);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTEVALUE_H__
