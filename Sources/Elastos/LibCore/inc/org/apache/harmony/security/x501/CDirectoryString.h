
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CDIRECTORYSTRING_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CDIRECTORYSTRING_H__

#include "_Org_Apache_Harmony_Security_X501_CDirectoryString.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CarClass(CDirectoryString)
    , public Object
    , public IDirectoryString
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ Org::Apache::Harmony::Security::Asn1::IASN1Choice ** ppAsn1);

    CARAPI SetASN1(
        /* [in] */ Org::Apache::Harmony::Security::Asn1::IASN1Choice * pAsn1);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CDIRECTORYSTRING_H__
