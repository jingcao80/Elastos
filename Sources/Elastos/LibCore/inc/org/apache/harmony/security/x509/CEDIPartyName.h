
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CEDIPARTYNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CEDIPARTYNAME_H__

#include "_Org_Apache_Harmony_Security_X509_CEDIPartyName.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CEDIPartyName)
    , public Object
    , public IEDIPartyName
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CEDIPARTYNAME_H__
