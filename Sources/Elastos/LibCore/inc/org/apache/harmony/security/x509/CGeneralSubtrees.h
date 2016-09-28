
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralSubtrees.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;

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
        /* [out] */ IList** ppSubtrees);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI constructor(
        /* [in] */ IList* pGeneralSubtrees);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREES_H__
