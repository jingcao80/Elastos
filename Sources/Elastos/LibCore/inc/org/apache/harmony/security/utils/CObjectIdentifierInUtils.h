
#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILS_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILS_H__

#include "_Org_Apache_Harmony_Security_Utils_CObjectIdentifierInUtils.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

/**
 * Instance of this class represents ObjectIdentifier (OID).
 *
 * OID is represented as a sequence of subidentifier.
 * Each subidentifier is represented as non negative integer value.
 * There are at least 2 subidentifiers in the sequence.
 *
 * Valid values for first subidentifier are 0, 1 and 2.
 * If the first subidentifier has 0 or 1 value the second
 * subidentifier MUST be less then 40.
 *
 * @see <a href="http://asn1.elibel.tm.fr/en/standards/index.htm">ASN.1</a>
 */
CarClass(CObjectIdentifierInUtils)
    , public Object
    , public IObjectIdentifier
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CObjectIdentifierInUtils();

    CARAPI GetOid(
        /* [out, callee] */ ArrayOf<Int32> ** ppOid);

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetGroup(
        /* [out] */ IInterface** ppGroup);

    CARAPI Equals(
        /* [in] */ IInterface * pO,
        /* [out] */ Boolean * pResult);

    CARAPI ToOIDString(
        /* [out] */ String * pStr);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32> * pOid);

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32> * pOid,
        /* [in] */ const String& name,
        /* [in] */ IInterface * pOidGroup);

    /**
     * Validates ObjectIdentifier (OID).
     *
     * @param oid - oid as array of integers
     * @throws NullPointerException     - if oid is null
     * @throws IllegalArgumentException - if oid is invalid
     */
    static CARAPI ValidateOid(
        /* [in] */ ArrayOf<Int32>* oid);

    /**
     * Returns hash code for array of integers
     *
     * @param oid - array of integers
     */
    static CARAPI_(Int32) HashIntArray(
        /* [in] */ ArrayOf<Int32>* array);

private:
    //OID as array of integers
    AutoPtr<ArrayOf<Int32> > mOid;

    //hash code
    Int32 mHash;

    //OID as string
    String mSoid;

    // stores the following: "OID." + soid
    String mSOID;

    // OID alias name
    String mName;

    // OID's group
    AutoPtr<IInterface> mGroup;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_COBJECTIDENTIFIERINUTILS_H__
