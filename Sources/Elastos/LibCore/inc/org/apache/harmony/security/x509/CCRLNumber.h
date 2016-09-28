
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCRLNUMBER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCRLNUMBER_H__

#include "_Org_Apache_Harmony_Security_X509_CCRLNumber.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Math::IBigInteger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCRLNumber)
    , public Object
    , public ICRLNumber
    , public IExtensionValue
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb);

    CARAPI GetNumber(
        /* [out] */ IBigInteger** ppNumber);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pEncoding);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCRLNUMBER_H__
