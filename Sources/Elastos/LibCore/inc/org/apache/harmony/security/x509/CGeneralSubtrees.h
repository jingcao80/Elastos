
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralSubtrees.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralSubtrees)
    , public Object
    , public IGeneralSubtrees
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSubtrees(
        /* [out] */ Elastos::Utility::IList ** ppSubtrees);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI constructor(
        /* [in] */ Elastos::Utility::IList * pGeneralSubtrees);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__
