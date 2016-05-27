
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CDNPARSER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CDNPARSER_H__

#include "_Org_Apache_Harmony_Security_X509_CDNParser.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CDNParser)
    , public Object
    , public IDNParser
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Parse(
        /* [out] */ Elastos::Utility::IList ** ppList);

    CARAPI constructor(
        /* [in] */ const String& dn);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CDNPARSER_H__
