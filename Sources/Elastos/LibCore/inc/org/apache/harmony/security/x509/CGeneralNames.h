
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMES_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMES_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralNames.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralNames)
    , public Object
    , public IGeneralNames
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetNames(
        /* [out] */ IList** ppNames);

    CARAPI GetPairsList(
        /* [out] */ ICollection** ppPairslist);

    CARAPI AddName(
        /* [in] */ IGeneralName* pName);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IList* pGeneralNames);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMES_H__
