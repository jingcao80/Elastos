
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCRLNUMBER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCRLNUMBER_H__

#include "_Org_Apache_Harmony_Security_X509_CCRLNumber.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCRLNumber)
    , public Object
    , public ICRLNumber
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncode);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValueEx(
        /* [in] */ Elastos::Core::IStringBuilder * pSb);

    CARAPI GetNumber(
        /* [out] */ Elastos::Math::IBigInteger ** ppNumber);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * pEncoding);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCRLNUMBER_H__
