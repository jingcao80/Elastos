
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CACCESSDESCRIPTION_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CACCESSDESCRIPTION_H__

#include "_Org_Apache_Harmony_Security_X509_CAccessDescription.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CAccessDescription)
{
public:
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** ppEncoded);

    CARAPI ToString(
        /* [out] */ String * pStr);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CACCESSDESCRIPTION_H__
