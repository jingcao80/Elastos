
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIER_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIER_H__

#include "_Org_Apache_Harmony_Security_Asn1_CObjectIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CObjectIdentifier)
{
public:
    CAR_OBJECT_DECL()

    static CARAPI Validate(
        /* [in] */ ArrayOf<Int32>* oid);

    static CARAPI ToString(
        /* [in] */ ArrayOf<Int32>* oid,
        /* [out] */ String* str);

    static CARAPI ToIntArray(
        /* [in] */ const String& str,
        /* [out, callee] */ ArrayOf<Int32>** intArray);

    static CARAPI IsOID(
        /* [in] */ const String& str,
        /* [out] */ Boolean* isOID);

    CARAPI Equals(
        /* [in] */ PInterface obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* oid);

    CARAPI constructor(
        /* [in] */ const String& strOid);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetOid();

private:
    static CARAPI ToIntArray(
        /* [in] */ const String& str,
        /* [in] */ Boolean shouldThrow,
        /* [out, callee] */ ArrayOf<Int32>** ret);

private:
        /** OID as array of integers */
    AutoPtr<ArrayOf<Int32> > mOid;

    /** OID as string */
    String mSoid;
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_COBJECTIDENTIFIER_H__
