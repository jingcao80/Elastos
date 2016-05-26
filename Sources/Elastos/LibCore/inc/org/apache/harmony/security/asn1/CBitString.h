
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CBITSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CBITSTRING_H__

#include "_Org_Apache_Harmony_Security_Asn1_CBitString.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CBitString)
    , public Object
    , public IBitString
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI_(AutoPtr<ArrayOf<Byte> >) InitStatic();

    CARAPI SetBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI SetUnusedBits(
        /* [in] */ Int32 unusedBits);

    CARAPI GetUnusedBits(
        /* [out] */ Int32* unusedBits);

    CARAPI GetBit(
        /* [in] */ Int32 bit,
        /* [out] */ Boolean* value);

    CARAPI SetBit(
        /* [in] */ Int32 bit,
        /* [in] */ Boolean value);

    CARAPI ToBooleanArray(
        /* [out, callee] */ ArrayOf<Boolean>** array);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 unusedBits);

    CARAPI constructor(
        /* [in] */ ArrayOf<Boolean>* values);

public:
    /** Sequence of bits padded with unused bits. */
    AutoPtr<ArrayOf<Byte> > mBytes;

    /** Number of unused bits in the last byte. */
    Int32 mUnusedBits;

private:
    static AutoPtr<ArrayOf<Byte> > SET_MASK;

    static AutoPtr<ArrayOf<Byte> > RESET_MASK;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CBITSTRING_H__
