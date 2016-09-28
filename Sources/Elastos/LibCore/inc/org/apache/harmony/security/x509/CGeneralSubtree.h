
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralSubtree.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

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
        /* [out] */ IGeneralName** ppBase);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ IGeneralName* pBase,
        /* [in] */ Int32 minimum,
        /* [in] */ Int32 maximum);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREE_H__
