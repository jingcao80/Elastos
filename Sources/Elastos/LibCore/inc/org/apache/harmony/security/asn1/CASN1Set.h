
#ifndef __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SET_H__
#define __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SET_H__

#include "_Org_Apache_Harmony_Security_Asn1_CASN1Set.h"
#include <Asn1TypeMacro.h>
#include <ASN1TypeCollection.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CarClass(CASN1Set), public ASN1TypeCollection
{
public:
    ASN1TYPE_METHODS_DECL()

    CARAPI SetType(
        /* [in] */ ArrayOf<IASN1Type*>* type);

    CARAPI GetType(
        /* [out, callee] */ ArrayOf<IASN1Type*>** type);

    CARAPI SetOptional(
        /* [in] */ ArrayOf<Boolean>* optional);

    CARAPI GetOptional(
        /* [out, callee] */ ArrayOf<Boolean>** optional);

    CARAPI SetDefault(
        /* [in] */ ArrayOf<IInterface *>* def);

    CARAPI GetDefault(
        /* [out, callee] */ ArrayOf<IInterface*>** def);

    CARAPI constructor(
        /* [in] */ ArrayOf<IASN1Type*>* type);

private:
    // TODO: Add your private member variables here.
};

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_ASN1_CASN1SET_H__
