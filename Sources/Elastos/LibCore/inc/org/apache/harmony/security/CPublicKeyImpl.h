
#ifndef __ORG_APACHE_HARMONY_SECURITY_CPUBLICKEYIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_CPUBLICKEYIMPL_H__

#include "_Org_Apache_Harmony_Security_CPublicKeyImpl.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CarClass(CPublicKeyImpl)
    , public Object
    , public IPublicKeyImpl
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetAlgorithm(
        /* [out] */ String * pAlgorithm);

    CARAPI GetFormat(
        /* [out] */ String * pFormat);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI SetAlgorithm(
        /* [in] */ const String& algorithm);

    CARAPI SetEncoding(
        /* [in] */ ArrayOf<Byte> * pEncoding);

    CARAPI constructor(
        /* [in] */ const String& algorithm);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_CPUBLICKEYIMPL_H__
