
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralSubtree.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralSubtree)
    , public Object
    , public IGeneralSubtree
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetBase(
        /* [out] */ Org::Apache::Harmony::Security::X509::IGeneralName ** ppBase);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ Elastos::Core::IStringBuilder * pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralName * pBase,
        /* [in] */ Int32 minimum,
        /* [in] */ Int32 maximum);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__
